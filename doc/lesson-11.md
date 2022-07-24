# 11. Userland Application I

## Ingredients of Userland Applications
1. libc ([newlib](https://sourceware.org/newlib/) is chosen here)
2. libgloss (the glue for newlib)
3. [crt0](https://en.wikipedia.org/wiki/Crt0)
4. application code (main.c)
5. linker script

## Runtime Libraries
### libc (newlib)
Before we integrate our glue code into newlib, we can abuse the `i686-elf` target which has a minimal functionality (aka
 `nosys`). In the install script we just build `newlib` without attaching our os glue code, this is particularly useful
in the early development phase. Once you have a more stable `libgloss` you can follow the re-configure section to
integrate the glue tightly with `newlib` and enable additional features.

### crt0
We just implement a minimal setup for the C runtime.

## Building Testapp
We build and link the example `testapp` with `freestanding` and `nostdlib` options, and manually provide the libc in the
command line. We also have another linker script to link the application.

### Re-configure `newlib`
Once you have a stable implementation of the `libgloss`, you can re-configure `newlib` by following the [osdev wiki](https://wiki.osdev.org/Porting_Newlib).
By doing so you can also enable additional features provided by `newlib`.

### Re-configure `gcc`
You can re-configure `gcc` to have the `newlib`, `crt0` and the linker script as default option. This way application
developer will be able to build app without the long command line.

## Further Reference
1. https://wiki.osdev.org/Porting_Newlib
