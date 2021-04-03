#include "jr.h"

#include <stdint.h>
#include <string.h>

#ifndef JR_MAX_EVENT_SIZE
#error You must define JR_MAX_EVENT_SIZE
#endif

#ifndef JR_MAX_PENDING_EVENTS
#error You must define JR_MAX_PENDING_EVENTS
#endif

typedef struct {
    // this must be the first struct field so it's always 4-byte aligned
    uint8_t         data[JR_MAX_EVENT_SIZE];
    jr_sched_cb     callback;
    jr_evt_size_t   len;
} __attribute__ ((aligned (4))) sched_event_t;

#define SET_PENDSV()    (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk)
#define CLEAR_PENDSV()  (SCB->ICSR &= ~(SCB_ICSR_PENDSVSET_Msk))

#define INC(v)          (((v)+1)&(JR_MAX_PENDING_EVENTS-1))

static sched_event_t    s_events[JR_MAX_PENDING_EVENTS];
volatile static int     s_event_count               = 0;
static int              s_rp                        = 0;
static int              s_wp                        = 0;
static int              s_running                   = 0;

void jr_sched_init(jr_sched_init_t *cfg) {
    NVIC_SetPriority(PendSV_IRQn, cfg->background_irq_priority);
}

void jr_sched_start() {
    s_running = 1;
    if (s_event_count > 0) {
        SET_PENDSV();
    }
}

jr_status_t jr_sched_put(jr_sched_cb callback, void *data, jr_evt_size_t len) {
    if (len > JR_MAX_EVENT_SIZE) {
        return JR_RANGE_ERROR;
    }

    jr_status_t ret = JR_OK;

    JR_CRIT_ENTER();
    
    if (s_event_count == JR_MAX_PENDING_EVENTS) {
        ret = JR_NO_MEM;
    } else {
        s_events[s_wp].callback = callback;
        s_events[s_wp].len = len;
        if (len > 0) {
            memcpy(s_events[s_wp].data, data, len); 
        }
        s_event_count++;
        s_wp = INC(s_wp);
    }

    JR_CRIT_LEAVE();

    if (ret == JR_OK && s_running) {
        SET_PENDSV();
    }
    
    return ret;
}

void PendSV_Handler() {
    JR_CRIT_ENTER();
    
    while (s_event_count > 0) {
        JR_CRIT_LEAVE();

        sched_event_t evt;
        evt = s_events[s_rp];
        evt.callback(evt.data, evt.len);
        
        JR_CRIT_ENTER_NO_DECL();

        s_event_count--;
        s_rp = INC(s_rp);
    }

    // When the loop is exited we know that:
    //
    // a) interrupts are disabled
    // b) the event queue is empty
    //
    // This means it's safe to clear the PendSV interrupt as any events that were
    // added to the queue during the loop must have been handled, and no more events
    // can arrive.
    //
    // It is good practice to explicitly clear the PendSV interrupt:
    // https://embeddedgurus.com/state-space/2011/09/whats-the-state-of-your-cortex/
    CLEAR_PENDSV();
    
    JR_CRIT_LEAVE();

    // if a new event arrives now, PendSV's pending flag will set and the handler
    // we be re-entered.
}