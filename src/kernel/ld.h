#ifndef KERNEL_LD_H_
#define KERNEL_LD_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "kvector.h"

namespace kernel {

class ProgramLoader {
  public:
    ProgramLoader();
    ~ProgramLoader();
    bool load_program(const char* pathname);

    uintptr_t get_entrypoint() const;
    uintptr_t get_addr(const char* symbol) const;

  private:
    typedef struct {
        uintptr_t vaddr;
        uintptr_t base;
        size_t    size;
    } SEGMENT;
    typedef struct {
        const char* name;
        uintptr_t addr;
    } SYMBOL;
    kvector<SEGMENT> _segments;
    kvector<SYMBOL> _symbols;
    uintptr_t _entrypoint;

    uintptr_t get_base(uintptr_t vaddr) const;
    uintptr_t translate_addr(uintptr_t vaddr) const;
    bool relocate(const char* buffer);
};

}  // namespace kernel

#endif // KERNEL_LD_H_
