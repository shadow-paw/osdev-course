#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ioport.h"
#include "mmu.h"
#include "driver_bga.h"
#include "kdebug.h"

#define VBE_DISPI_IOPORT_INDEX  0x01CE
#define VBE_DISPI_IOPORT_DATA   0x01CF
#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)

// TODO: dynamic alloc from kernel
#define FRAME_BUFFER_VMA 0xC0000000

const struct DISPLAY_MODEINFO __bga_allowed_mode[] = {
    { 0, 640,  480, 32, 640*4 },
    { 1, 800,  600, 32, 800*4 },
    { 2, 1024, 768, 32, 1024*4 }
};
unsigned int bga_get_maxmode() {
    return (unsigned int)(sizeof(__bga_allowed_mode) / sizeof(struct DISPLAY_MODEINFO) -1);
}

bool bga_open(struct DISPLAY_DEVICE* device, struct PCI_DRIVER* pci_driver, const struct PCI_ADDR* addr, const struct PCI_INFO* info) {
    if (!device || !pci_driver || !addr || !info) return false;
    if (info->class_code != 3 || info->subclass_code != 0) return false;
    if (info->device_id != 0x1111 || info->vendor_id != 0x1234) return false;
    device->pci_driver = pci_driver;
    __builtin_memcpy(&device->pci_addr, addr, sizeof(device->pci_addr));
    __builtin_memcpy(&device->pci_info, info, sizeof(device->pci_info));
    __builtin_memset(&device->info, 0, sizeof(device->info));
    device->frame_buffer = 0;
    device->frame_buffer_size = 0;
    return true;
}
bool bga_close(struct DISPLAY_DEVICE* device) {
    if (!device) return false;
    device->pci_driver = 0;
    __builtin_memset(&device->pci_addr, 0, sizeof(device->pci_addr));
    __builtin_memset(&device->pci_info, 0, sizeof(device->pci_info));
    __builtin_memset(&device->info, 0, sizeof(device->info));
    device->frame_buffer = 0;
    device->frame_buffer_size = 0;
    return true;
}
bool bga_get_modeinfo(struct DISPLAY_DEVICE* device, unsigned int mode, struct DISPLAY_MODEINFO* info) {
    if (!device) return false;
    if (mode > bga_get_maxmode() ) return false;
    __builtin_memcpy(info, &__bga_allowed_mode[mode], sizeof(struct DISPLAY_MODEINFO));
    return true;
}
bool bga_set_mode(struct DISPLAY_DEVICE* device, unsigned int mode) {
    uint32_t bar, size;

    if (!device) return false;
    if (!bga_get_modeinfo(device, mode, &device->info)) return false;
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
    outw(VBE_DISPI_IOPORT_DATA, 0);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)device->info.width);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)device->info.height);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)device->info.bpp);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
    outw(VBE_DISPI_IOPORT_DATA, 0x41);
    device->info.mode = mode;

    if (!device->pci_driver->get_bar(&device->pci_addr, 0, &bar, &size)) return false;
    // unmap previous frame buffer
    if (device->frame_buffer_size != 0) {
        mmu_munmap(device->frame_buffer, device->frame_buffer_size, MMU_MUNMAP_NORELEASE);
        device->frame_buffer_size = 0;
    }
    bar = bar & 0xFFFFFFF0;
    const size_t bytes_per_pixel = device->info.bpp / 8;
    const size_t frame_buffer_size = (uint32_t)(device->info.pitch * device->info.height * bytes_per_pixel);
    if (size > frame_buffer_size) {
        size = frame_buffer_size;
    }
    device->frame_buffer = (uint8_t*)FRAME_BUFFER_VMA;
    device->frame_buffer_size = (size_t)size;
    if (!mmu_mmap(device->frame_buffer, bar, size, MMU_PAGE_MAPPHY)) return false;

    kdebug("    BGA : Change video mode: %d = %dx%dx%d, BAR:%X SIZE:%X\n",
           mode, device->info.width, device->info.height, device->info.bpp, bar, size);
    return true;
}
bool bga_clear_screen(struct DISPLAY_DEVICE* device) {
    if (!device) return false;
    kdebug("    BGA : Clear Screen\n");
    uint32_t* p = (uint32_t*)device->frame_buffer;
    unsigned int R = device->info.width * device->info.width + device->info.height * device->info.height;
    for (unsigned int y = 0; y<device->info.height; y++) {
        for (unsigned int x = 0; x<device->info.width; x++) {
            int r = 255 * x / device->info.width;
            int g = 255 * y / device->info.height;
            int b = 255 * (x*x + y*y) / R;
            uint32_t rgba = 0xFF000000 | (r<<16) | (g << 8) | b;
            *p++ = rgba;
        }
    }
    return true;
}
bool driver_bga(struct DISPLAY_DRIVER* driver) {
    if (!driver) return false;
    driver->open = bga_open;
    driver->close = bga_close;
    driver->get_modeinfo = bga_get_modeinfo;
    driver->set_mode = bga_set_mode;
    driver->clear_screen = bga_clear_screen;
    return true;
}
