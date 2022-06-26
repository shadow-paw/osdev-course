#include "inlineasm.h"
#include "kmalloc.h"
#include "mmu.h"
#include "scheduler.h"
#include "kdebug.h"

struct SCHEDULER  g_scheduler;
struct TCB g_kthread_main;
unsigned int _scheudler_next_thread_id = 1;

bool scheduler_init(void) {
    __builtin_memset(&g_scheduler, 0, sizeof(g_scheduler));
    for ( int i=0; i<SCHEDULE_PRIORITY_MAX; i++ ) {
        g_scheduler.active[i].tail = &g_scheduler.active[i].head;
    }
    g_scheduler.slept.tail  = &g_scheduler.slept.head;
    g_scheduler.zombie.tail = &g_scheduler.zombie.head;
    // create "kmain thread"
    __builtin_memset(&g_kthread_main, 0, sizeof(g_kthread_main));
    g_kthread_main.priority = SCHEDULE_PRIORITY_IDLE;
    g_kthread_main.quantum  = SCHEDULE_QUANTUM_SHORT;
    g_scheduler.current = &g_kthread_main;
    g_scheduler.quantum = 1;
    return true;
}
bool scheduler_add(struct TCB *tcb) {
    struct TCB_LIST* list = &g_scheduler.active[tcb->priority];
    tcb->next = NULL;
    tcb->prev = list->tail;
    list->tail->next = tcb;
    list->tail = tcb;
    return true;
}
bool scheduler_run(struct TCB *tcb) {
    tcb->thread_id = __sync_fetch_and_add(&_scheudler_next_thread_id, 1);
    return scheduler_add(tcb);
}
struct TCB* scheduler_current(void) {
    return g_scheduler.current;
}
void scheduler_sleep(uint32_t tick) {
    if (tick) {
        g_scheduler.current->sleep_tick = tick;
        g_scheduler.current->flags |= SCHEDULER_FLAG_SLEPT;
    }
    scheduler_yield();
}
void scheduler_exitthread() {
    g_scheduler.current->flags |= SCHEDULER_FLAG_ZOMBIE;
    scheduler_yield();
}
struct TCB* scheduler_pick(void) {
    struct TCB *current, *target, *next, *slept;
    current = g_scheduler.current;
    current->elapsed += g_scheduler.elapsed;
    if (current->flags & SCHEDULER_FLAG_ZOMBIE) {
        current->next = g_scheduler.zombie.head.next;
        g_scheduler.zombie.head.next = current;
    } else if (current->flags & SCHEDULER_FLAG_SLEPT) {
        for (slept=&g_scheduler.slept.head; ; slept=next) {
            if ((next = slept->next) == NULL) break;
            if (current->sleep_tick <= next->sleep_tick) {
                next->sleep_tick -= current->sleep_tick;
                break;
            } else {
                current->sleep_tick -= next->sleep_tick;
            }
        }
        current->next = next;
        slept->next = current;
        current->prev = slept;
        if (next) next->prev = current;
    } else {
        scheduler_add(current);
    }
    // Awake slept process
    for (;;) {
        if ((slept = g_scheduler.slept.head.next) == NULL) break;
        if (slept->sleep_tick > g_scheduler.elapsed) {
            slept->sleep_tick -= g_scheduler.elapsed;
            break;
        }
        slept->sleep_tick = 0;
        slept->flags ^= SCHEDULER_FLAG_SLEPT;
        g_scheduler.slept.head.next = slept->next;
        if (slept->next) {
            slept->next->prev = slept->prev;
        }
        scheduler_add(slept);
    }
    for (int i=0; i<SCHEDULE_PRIORITY_MAX; i++) {
        struct TCB_LIST* list = &g_scheduler.active[i];
        if ((target = list->head.next) != NULL) {
            // target->prev->next = target->next;
            list->head.next = target->next;
            if (target->next) target->next->prev = &list->head;
            if (list->tail == target) list->tail = &list->head;
        }
        if (target != NULL) break;
    }
    if (target->elapsed >= target->quantum) target->elapsed = 0;
    g_scheduler.elapsed = 0;
    g_scheduler.quantum = target->quantum - target->elapsed;
    return target;
}
bool scheduler_cleanzombie(void) {
    struct TCB *tcb;
    _CLI();
    if ((tcb = g_scheduler.zombie.head.next) != NULL) {
        g_scheduler.zombie.head.next = tcb->next;
    };
    _STI();
    if (tcb == NULL) return false;
    kdebug("Clean zombie thread: %X\n", tcb);
    if (tcb->kstack) kfree(tcb->kstack);
    //
    // TODO: release pages
    //
    kfree(tcb);
    return true;
}
