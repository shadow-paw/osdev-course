#ifndef KERNEL_ELF_H_
#define KERNEL_ELF_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELF32_R_SYM(i)  ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))

enum {
    ET_NONE         = 0,        // No File Type
    ET_REL          = 1,        // Relocatable File
    ET_EXEC         = 2,        // Executable File
    ET_DYN          = 3,        // Shared Object File
    ET_CORE         = 4,        // Core File
    ET_LOPROC       = 0xFF00,   // Processor-specific
    ET_HIPROC       = 0xFFFF,   // Processor-specific

    EM_NONE         = 0,        // No Machine
    EM_M32          = 1,        // AT&T WE 32100
    EM_SPARC        = 2,        // SPARC
    EM_386          = 3,        // Intel 80386
    EM_68K          = 4,        // Motorola 68000
    EM_88K          = 5,        // Motorola 88000
    EM_860          = 7,        // Intel 80860
    EM_MIPS         = 8,        // MIPS RS3000
    EM_X86_64       = 0x3E,     // AMD x86-64

    PT_NULL         = 0,
    PT_LOAD         = 1,
    PT_DYNAMIC      = 2,
    PT_INTERP       = 3,
    PT_NOTE         = 4,
    PT_SHLIB        = 5,
    PT_PHDR         = 6,
    PT_LOPROC       = 0x70000000,
    PT_HIPROC       = 0x7FFFFFFF,

    SHT_NULL        = 0,
    SHT_PROGBITS    = 1,
    SHT_SYMTAB      = 2,
    SHT_STRTAB      = 3,
    SHT_RELA        = 4,
    SHT_HASH        = 5,
    SHT_DYNAMIC     = 6,
    SHT_NOTE        = 7,
    SHT_NOBITS      = 8,
    SHT_REL         = 9,
    SHT_SHLIB       = 10,
    SHT_SYNSYM      = 11,
    SHT_LOPROC      = 0x70000000,
    SHT_HIPROC      = 0x7FFFFFFF,
    SHT_LOUSER      = 0x80000000,
    SHT_HIUSER      = 0xFFFFFFFF,

    R_386_NONE      = 0,
    R_386_32        = 1,
    R_386_PC32      = 2,
    R_386_GOT32     = 3,
    R_386_PLT32     = 4,
    R_386_COPY      = 5,
    R_386_GLOB_DAT  = 6,
    R_386_JMP_SLOT  = 7,
    R_386_RELATIVE  = 8,
    R_386_GOTOFF    = 9,
    R_386_GOTPC     = 10,
};

// ELF32
typedef uint32_t ELF32_Addr;
typedef uint32_t ELF32_Off;
typedef uint16_t ELF32_Half;
typedef uint32_t ELF32_Word;
typedef int32_t  ELF32_Sword;

struct __attribute__ ((__packed__)) ELF32_EHDR {
    uint32_t   e_ident[4];  // 16 bytes
    ELF32_Half e_type;
    ELF32_Half e_machine;
    ELF32_Word e_version;
    ELF32_Addr e_entry;
    ELF32_Off  e_phoff;
    ELF32_Off  e_shoff;
    ELF32_Word e_flags;
    ELF32_Half e_ehsize;
    ELF32_Half e_phentsize;
    ELF32_Half e_phnum;
    ELF32_Half e_shentsize;
    ELF32_Half e_shnum;
    ELF32_Half e_shstrndx;
};
struct __attribute__ ((__packed__)) ELF32_PHDR {
    ELF32_Word p_type;
    ELF32_Off  p_offset;
    ELF32_Addr p_vaddr;
    ELF32_Addr p_paddr;
    ELF32_Word p_filesz;
    ELF32_Word p_memsz;
    ELF32_Word p_flags;
    ELF32_Word p_align;
};
struct __attribute__ ((__packed__)) ELF32_SHDR{
    ELF32_Word sh_name;
    ELF32_Word sh_type;
    ELF32_Word sh_flags;
    ELF32_Addr sh_addr;
    ELF32_Off  sh_offset;
    ELF32_Word sh_size;
    ELF32_Word sh_link;
    ELF32_Word sh_info;
    ELF32_Word sh_addralign;
    ELF32_Word sh_entsize;
};
struct __attribute__ ((__packed__)) ELF32_SYM {
    ELF32_Word st_name;
    ELF32_Addr st_value;
    ELF32_Word st_size;
    uint8_t    st_info;
    uint8_t    st_other;
    ELF32_Half st_shndx;
};
struct __attribute__ ((__packed__)) ELF32_REL {
    ELF32_Addr r_offset;
    ELF32_Word r_info;
};
struct __attribute__ ((__packed__)) ELF32_RELA {
    ELF32_Addr  r_offset;
    ELF32_Word  r_info;
    ELF32_Sword r_addend;
};

#ifdef __cplusplus
};
#endif

#endif // KERNEL_ELF_H_
