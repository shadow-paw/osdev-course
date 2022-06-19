# 7. Hardware Abstraction Layer

## Uniform Environment
One of the purpose for OS is to provide an uniform environment so that application don't need
to care about certain level of hardware differences.

## Drivers and Devices
Normally you would have a driver manager to load drivers (code) based on hardware present on the computer. For specific type of driver it may be managing multiple devices on the same type. For example, you may have an UART driver, and multiple UART devices.

## Micro Kernel vs Monolithic Kernel
- https://wiki.osdev.org/Microkernel
- https://wiki.osdev.org/Monolithic_Kernel

## Exposing Access to Applications
### Device Listing
- Uniform list, e.g. `sysfs` (aka `/dev`), see [device model](https://linux-kernel-labs.github.io/refs/heads/master/labs/device_model.html)
- Specialized list, e.g. [GetCommPorts](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcommports)

### Control & Using Hardware
- Uniform functions, e.g. [ioctl](https://man7.org/linux/man-pages/man2/ioctl.2.html), file, shared memory, etc.
- Specialized functions, e.g. [SetCommState](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setcommstate)

# Workshop
- Add CPP support
  - compiler flags
  - `new` / `delete`
- Reorganize codebase to introduce:
  - HAL class
    - Driver repository (`uart`, `bga`, `pci`)
    - Device repository (`uart0`, `uart1`, `uart2`, `uart3`, `bga0`)
    - Expose API functions
