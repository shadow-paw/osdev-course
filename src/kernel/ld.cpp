#include "kmalloc.h"
#include "kstring.h"
#include "elf.h"
#include "vfs.h"
#include "mmu.h"
#include "ld.h"
#include "kdebug.h"

namespace kernel {

ProgramLoader::ProgramLoader() {
}
ProgramLoader::~ProgramLoader() {
    for (unsigned int i=0; i<_symbols.count(); i++) {
        if (_symbols[i].name) {
            kfree(_symbols[i].name);
            _symbols[i].name = nullptr;
        }
    }
}
bool ProgramLoader::load_program(const char* pathname) {
    struct FS_FSTAT stat;
    char* buffer;
    const struct ELF32_PHDR* elf_phdr = 0;
    const struct ELF32_EHDR* elf_ehdr = 0;
    if (!kernel::vfs.fstat(pathname, &stat)) return false;
    if (!(buffer = (char*)kmalloc(stat.filelen))) return false;
    if (kernel::vfs.read(pathname, buffer, 0, stat.filelen) != (int64_t)stat.filelen) {
        goto fail;
    }
    // check headers
    elf_ehdr = (const struct ELF32_EHDR*)buffer;
    if (elf_ehdr->e_ident[0] != 0x464C457F) goto fail;
    if (elf_ehdr->e_machine != EM_386) goto fail;
    if (elf_ehdr->e_type != ET_EXEC) goto fail;
    // Load program into vaddr
    for(unsigned int i=0; i<(unsigned int)elf_ehdr->e_phnum; i++) {
        elf_phdr = (const struct ELF32_PHDR*)(buffer + elf_ehdr->e_phoff + i * elf_ehdr->e_phentsize);
        switch (elf_phdr->p_type) {
        case PT_LOAD: {
            if (elf_phdr->p_memsz < elf_phdr->p_filesz) goto fail;
            const SEGMENT& seg = _segments.append(SEGMENT{
                (uintptr_t)elf_phdr->p_vaddr,
                (uintptr_t)elf_phdr->p_vaddr,
                (((size_t)elf_phdr->p_memsz + 4095)>>12)<<12
            });
            mmu_mmap((void*)seg.base, 0, seg.size, MMU_PROT_RW|MMU_PROT_USER);
            kmemcpy((void*)seg.base, buffer + elf_phdr->p_offset, elf_phdr->p_filesz);
            if (elf_phdr->p_filesz < elf_phdr->p_memsz) {
                kmemset((void*)(seg.base + elf_phdr->p_filesz), 0, elf_phdr->p_memsz - elf_phdr->p_filesz);
            }
            break;
        }
        case PT_NULL:
        case PT_DYNAMIC:
        case PT_INTERP:
        case PT_NOTE:
        case PT_SHLIB:
        case PT_PHDR:
            break;
        }
    }
    relocate(buffer);
    _entrypoint = translate_addr(elf_ehdr->e_entry);
    kfree(buffer);
    return true;
fail:
    kfree(buffer);
    return false;
}
uintptr_t ProgramLoader::get_entrypoint() const {
    return _entrypoint;
}
uintptr_t ProgramLoader::get_base(uintptr_t vaddr) const {
    for (unsigned int i=0; i<_segments.count(); i++) {
        if ( vaddr >= _segments[i].vaddr && vaddr <= _segments[i].vaddr + _segments[i].size ) {
            return _segments[i].base - _segments[i].vaddr;
        }
    } return 0;
}
uintptr_t ProgramLoader::translate_addr(uintptr_t vaddr) const {
    for (unsigned int i=0; i<_segments.count(); i++) {
        if (vaddr >= _segments[i].vaddr && vaddr <= _segments[i].vaddr + _segments[i].size) {
            return (vaddr - _segments[i].vaddr) + _segments[i].base;
        }
    } return 0;
}
uintptr_t ProgramLoader::get_addr(const char* symbol) const {
    for (unsigned int i=0; i<_symbols.count(); i++) {
        if (kstrcmp(_symbols[i].name, symbol) == 0) {
            return _symbols[i].addr;
        }
    } return 0;
}
bool ProgramLoader::relocate(const char* buffer) {
    const ELF32_EHDR* elf_ehdr = (const struct ELF32_EHDR*)buffer;
    const struct ELF32_SHDR* elf_shdr = nullptr;
    const struct ELF32_SYM*  elf_sym = nullptr;
    const struct ELF32_REL*  elf_rel = nullptr;
    unsigned int elf_sym_count = 0;
    unsigned int elf_rel_count = 0;
    const char* elf_sym_name = 0;
    unsigned int symbol_id = 0;
    uintptr_t patchaddr = 0;
    uint32_t patch32 = 0;
    // Get Headers
    for (unsigned int i=0; i<(unsigned int)elf_ehdr->e_shnum; i++ ) {
        elf_shdr = (const struct ELF32_SHDR*)(buffer + elf_ehdr->e_shoff + i * elf_ehdr->e_shentsize);
        switch (elf_shdr->sh_type) {
        case SHT_SYNSYM:
            elf_sym = (const struct ELF32_SYM*)(buffer + elf_shdr->sh_offset);
            elf_sym_count = (unsigned int)(elf_shdr->sh_size / sizeof(struct ELF32_SYM));
            elf_sym_name  = (const char*)(buffer + elf_shdr->sh_offset + elf_shdr->sh_size);
            break;
        case SHT_REL:
            elf_rel = (const struct ELF32_REL*)(buffer + elf_shdr->sh_offset);
            elf_rel_count = (unsigned int)(elf_shdr->sh_size / sizeof(struct ELF32_REL));
            break;
        }
    }
    // Relocate
    for (unsigned int i=0; i<elf_rel_count; i++) {
        switch (ELF32_R_TYPE(elf_rel[i].r_info)) {
        case R_386_32:  // base + S + A
            symbol_id = (unsigned int)ELF32_R_SYM(elf_rel[i].r_info);
            if (symbol_id >= elf_sym_count) return false;
            patchaddr = translate_addr((uintptr_t)elf_rel[i].r_offset);
            if (patchaddr == 0) return false;
            patch32 = translate_addr(elf_sym[symbol_id].st_value);
            if (patch32 == 0) return false;
            *(uint32_t*) patchaddr += patch32;
            kdebug("REL[R_386_32] (%s) := %X\n",
                elf_sym_name + elf_sym[symbol_id].st_name,
                *(uint32_t*)patchaddr
            );
            break;
        case R_386_PC32:  // base + S + A - p
            symbol_id = (unsigned int)ELF32_R_SYM(elf_rel[i].r_info);
            if (symbol_id >= elf_sym_count) return false;
            patchaddr = translate_addr((uintptr_t)elf_rel[i].r_offset);
            if (patchaddr == 0) return false;
            if (elf_sym[symbol_id].st_value != 0) {
                patch32 = (uint32_t)translate_addr(elf_sym[symbol_id].st_value);
            } else {
                patch32 = (uint32_t)get_addr(elf_sym_name + elf_sym[symbol_id].st_name);
            }
            if (patch32 == 0) return false;
            patch32 -= patchaddr +4;
            *(uint32_t*)patchaddr = patch32;
            kdebug("REL[R_386_PC32] (%s) PatchAddr=%X Offset=%X Value= %X\n",
                elf_sym_name + elf_sym[symbol_id].st_name,
                patchaddr,
                (uintptr_t)elf_rel[i].r_offset,
                *(uint32_t*)patchaddr
            );
            break;
        case R_386_RELATIVE:  // B + A
            patchaddr = translate_addr((uintptr_t)elf_rel[i].r_offset);
            if (patchaddr == 0) return false;
            *(uint32_t*) patchaddr += get_base((uintptr_t)elf_rel[i].r_offset);
            kdebug("REL[R_386_RELATIVE] PatchAddr=%X Offset=%X Value= %X\n",
                patchaddr, (uintptr_t)elf_rel[i].r_offset,
                *(uint32_t*) patchaddr
            );
            break;
        default:
            kdebug("ELF.rel(%d) Unsupported. Offset:%X info:%X\n",
                ELF32_R_TYPE(elf_rel[i].r_info),
                elf_rel[i].r_offset,
                elf_rel[i].r_info
            );
            return false;
        }
    }
    // Extract symbols
    for (unsigned int i=0; i<(unsigned int)elf_sym_count; i++) {
        if (elf_sym[i].st_name == 0) continue;
        _symbols.append(SYMBOL{
            elf_sym_name + elf_sym[i].st_name,
            translate_addr(elf_sym[i].st_value)
        });
    }
    return true;
}

}  // namespace kernel
