#ifndef KERNEL_SCHEDULER_H_
#define KERNEL_SCHEDULER_H_

#include <stddef.h>
#include "tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SCHEDULER_FLAG {
    SCHEDULER_FLAG_SLEPT = 1<<0,
    SCHEDULER_FLAG_ZOMBIE = 1<<1,
};
enum SCHEDULE_PRIORITY {
    SCHEDULE_PRIORITY_HIGH,
    SCHEDULE_PRIORITY_NORMAL,
    SCHEDULE_PRIORITY_LOW,
    SCHEDULE_PRIORITY_IDLE,
    SCHEDULE_PRIORITY_MAX,
};
enum SCHEDULE_QUANTUM {
    SCHEDULE_QUANTUM_SHORT  = 4,
    SCHEDULE_QUANTUM_NORMAL = 10,
    SCHEDULE_QUANTUM_LONG   = 30,
};
struct TCB_LIST {
    struct TCB head, *tail;
};
struct SCHEDULER {
    uint32_t        elapsed;
    uint32_t        quantum;
    struct TCB*     current;
    struct TCB_LIST active[SCHEDULE_PRIORITY_MAX];
    struct TCB_LIST slept;
    struct TCB_LIST zombie;
};

// schedule.s
void scheduler_yield();
// scheduler.c
bool scheduler_init(void);
bool scheduler_run(struct TCB *tcb);
struct TCB* scheduler_current(void);
void scheduler_sleep(uint32_t tick);
void scheduler_exitthread();
void scheduler_tick(void);
bool scheduler_cleanzombie(void);
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_SCHEDULER_H_
