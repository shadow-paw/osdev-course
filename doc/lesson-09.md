# 9. Multi-Tasking I

## Thread
I hope everyone know what thread is, from application perspective. But for kernel, there is a whole new concept for it.

Kernel manage threads with the associated "snapshot" machine state and the scheduling parameter. In essence, each CPU
core only runs one thread at a time. In a naïve implementation, multi-tasking is archived by either application give up
its time (cooperative multitasking) or the kernel has some quota system driven by timer or other events (preemptive
multitasking).

### TCB (Thread Control Block)
1. Machine State
2. Scheduling Parameters

### Context Switching
Either cooperative or preemptive, to actually perform task switch, you "simply" save the machine state of current thread
, then restore the state of another thread. Ideally, the threads will seamlessly continue.

```
scheduler_yield:
    pushf
    cli  ; disable interrupt to prevent re-entrance
    ; Save CPU Context
    ...
    ; Pick next process
    call    scheduler_pick
    ; Load CPU Context
    ...
    popf
    ret
```
> [You are not expected to understand this.](https://en.wikipedia.org/wiki/Lions%27_Commentary_on_UNIX_6th_Edition,_with_Source_Code#%22You_are_not_expected_to_understand_this%22)

### Context Information
1. Register state
2. FPU / MMX / XMM registers (trick below)

#### FPU / MMX / XMM context saving trick
As it is relative slow to preserve the FPU/MMX/XMM registers, a common trick is the disable the FPU/MMX/XMM unit for the
thread, so when any FPU/MMX/XMM operation is performed, the kernel has a chance to mark "dirty" flag and only preserve the
states if it has been used.

## Scheduler
### Triggers
1. timer
2. IRQ
3. when "leaving" kernel call

### Quota-Based Time-Sharing
1. Priority Queues
2. Delta Queue
3. Quantum (Quota)
4. Fair Scheduler. Throttle time consuming thread or boost low priority thread.

### Sleep
Just skip / remove it from the task list. Re-active thread after time passed.

### Zombie
For performance reason, upon thread (or process) end, you might not clean up immediately. This is called a zombie state.
Most OS have a low priority thread to do the hose keeping and release resources for the zombie.
