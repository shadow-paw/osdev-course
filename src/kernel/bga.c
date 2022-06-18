#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ioport.h"
#include "mmu.h"
#include "bga.h"
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

const struct DISPLAY_MODEINFO bga_allowed_mode[] = {
    { 0, 640,  480, 32, 640*4 },
    { 1, 800,  600, 32, 800*4 },
    { 2, 1024, 768, 32, 1024*4 }
};

struct BGA bga;

bool bga_init(const struct PCI_DRIVER* pci_driver, const struct PCI_ADDR* addr, const struct PCI_INFO* info) {
    if (!pci_driver || !addr || !info) return false;
    if (info->class_code != 3 || info->subclass_code != 0) return false;
    if (info->device_id != 0x1111 || info->vendor_id != 0x1234) return false;
    // outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ID);
    // kdebug ("bga: %X\n", (int)inw(VBE_DISPI_IOPORT_DATA) );
    bga.pci_driver = pci_driver;
    __builtin_memcpy(&bga.pci_addr, addr, sizeof(bga.pci_addr));
    __builtin_memcpy(&bga.pci_info, info, sizeof(bga.pci_info));
    bga.current_mode = 0;
    bga.frame_buffer = 0;
    bga.frame_buffer_size = 0;
    return true;
}
bool bga_fini() {
    __builtin_memset(&bga.pci_addr, 0, sizeof(bga.pci_addr));
    __builtin_memset(&bga.pci_info, 0, sizeof(bga.pci_info));
    bga.current_mode = 0;
    bga.frame_buffer = 0;
    bga.frame_buffer_size = 0;
    return true;
}
unsigned int bga_get_maxmode() {
    return (unsigned int)(sizeof(bga_allowed_mode) / sizeof(struct DISPLAY_MODEINFO) -1);
}
const struct DISPLAY_MODEINFO* bga_get_modeinfo(unsigned int mode) {
    if (mode > bga_get_maxmode() ) return 0;
    return &bga_allowed_mode[mode];
}
bool bga_set_mode(unsigned int mode) {
    uint32_t bar, size;

    const struct DISPLAY_MODEINFO* mode_info = bga_get_modeinfo(mode);
    if (!mode_info) return false;

    bga.current_mode = mode;
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
    outw(VBE_DISPI_IOPORT_DATA, 0);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)mode_info->width);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)mode_info->height);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP);
    outw(VBE_DISPI_IOPORT_DATA, (uint16_t)mode_info->bpp);
    outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
    outw(VBE_DISPI_IOPORT_DATA, 0x41);

    if (!bga.pci_driver->get_bar(&bga.pci_addr, 0, &bar, &size)) return false;
    if (bga.frame_buffer_size != 0) {
        mmu_munmap(bga.frame_buffer, bga.frame_buffer_size, MMU_MUNMAP_NORELEASE);
        bga.frame_buffer_size = 0;
    }
    bar = bar & 0xFFFFFFF0;
    if (size > (uint32_t)(mode_info->pitch * mode_info->height * 4) ) {
        size = (uint32_t)(mode_info->pitch * mode_info->height * 4);
    }
    // size = display->mode_info.height * display->mode_info.pitch;
    bga.frame_buffer = (uint8_t*)0xC0000000;
    if (!mmu_mmap(bga.frame_buffer, bar, size, MMU_PAGE_MAPPHY)) return false;
    bga.frame_buffer_size = (size_t)size;

    kdebug("    BGA : Change video mode: %d = %dx%dx%d, BAR:%X SIZE:%X\n",
           mode, mode_info->width, mode_info->height, mode_info->bpp, bar, size);
    return true;
}
bool bga_clear_screen() {
    if (bga.frame_buffer_size == 0) return false;
    kdebug("    BGA : Clear Screen\n");
    const struct DISPLAY_MODEINFO* mode_info = bga_get_modeinfo(bga.current_mode);
    uint32_t* p = (uint32_t*)bga.frame_buffer;
    double R = mode_info->width * mode_info->width + mode_info->height * mode_info->height;
    for (unsigned int y = 0; y<mode_info->height; y++) {
        for (unsigned int x = 0; x<mode_info->width; x++) {
            int r = 255 * x / mode_info->width;
            int g = 255 * y / mode_info->height;
            int b = 255 * (x*x + y*y) / R;
            uint32_t rgba = 0xFF000000 | (r<<16) | (g << 8) | b;
            *p++ = rgba;
        }
    }
    return true;
}
