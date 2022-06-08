#ifndef KERNEL_BGA_H_
#define KERNEL_BGA_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "pci.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DISPLAY_MODEINFO {
    unsigned int mode;
    unsigned int width, height, bpp;
    unsigned int pitch;
};

struct BGA {
    struct PCI_BUS_ADDR pci_addr;
    struct PCI_BUS_INFO pci_info;
    int                 current_mode;
    uint8_t*            frame_buffer;
    size_t              frame_buffer_size;
};

bool bga_init(const struct PCI_BUS_ADDR* addr, const struct PCI_BUS_INFO* info);
bool bga_fini();
const struct DISPLAY_MODEINFO* bga_get_modeinfo(unsigned int mode);
bool bga_set_mode(unsigned int mode);
bool bga_clear_screen();

#ifdef __cplusplus
};
#endif

#endif  // KERNEL_BGA_H_
