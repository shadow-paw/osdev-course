#ifndef KERNEL_DDK_DISPLAY_H_
#define KERNEL_DDK_DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk_pci.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DISPLAY_MODEINFO {
    unsigned int mode;
    unsigned int width, height, bpp;
    unsigned int pitch;
};

struct DISPLAY_DEVICE {
    struct PCI_DRIVER*      pci_driver;
    struct PCI_ADDR         pci_addr;
    struct PCI_INFO         pci_info;
    struct DISPLAY_MODEINFO info;
    void*                   frame_buffer;
    size_t                  frame_buffer_size;
};

struct DISPLAY_DRIVER {
    bool (*open)(struct DISPLAY_DEVICE* device, struct PCI_DRIVER* pci_driver, const struct PCI_ADDR* addr, const struct PCI_INFO* info);
    bool (*close)(struct DISPLAY_DEVICE* device);
    bool (*get_modeinfo)(struct DISPLAY_DEVICE* device, unsigned int mode, struct DISPLAY_MODEINFO* info);
    bool (*set_mode)(struct DISPLAY_DEVICE* device, unsigned int mode);
    bool (*clear_screen)(struct DISPLAY_DEVICE* device);
};

#ifdef __cplusplus
};
#endif

#endif  // KERNEL_DDK_DISPLAY_H_
