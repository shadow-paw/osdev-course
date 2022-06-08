# 6. I/O, IRQ, Interrupt & Exception

## I/O
- Port
- Memory mapped I/O

## Interrupt
- ISR - Interrupt service routine, must preserve machine registers state upon return.
- IDT - Interrupt descriptor table - max 256 functions (ISR) for 256 interrupts.
- CPU Exception.
- IRQ - event from hardware, usually follow by reading I/O in the ISR.
- NMI - Non-maskable interrupt, e.g. hardware failure.
- Trigger by `INT` instruction.

## 8259 PIC - Programmable Interrupt Controller
- Dual PIC cascade structure - 15 IRQ in total
- Mark event read

## PIT - Programmable Interval Timer
- One shot mode
- Periodic mode

## Advanced PIC
- Built-in high performance timer.
- I/O APIC → similar to 8259.
- Inter-processor interrupts (e.g. use for TLB shootdown).
- Certain PCI features rely on APIC.
- x2APIC, etc.

## PCI Bus
- bus scanning
- BGA
