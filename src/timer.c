#include <jr.h>
#include <stdlib.h>
#include <string.h>

#include "jr_config.h"

#define TYPE_MASK			(0x3 << 30)
#define STATE_MASK			(0x3 << 28)
#define INTERVAL_MASK		0x0FFFFFFF

#define STATE_IDLE			0
#define STATE_ENQUEUED		(1<<28)
#define STATE_RUNNING		(1<<29)

#define F(t)				(t)->_flags

#define TYPE(t)				(F(t) & TYPE_MASK)
#define STATE(t)			(F(t) & STATE_MASK)
#define INTERVAL(t)			(F(t) & INTERVAL_MASK)

#define SET_STATE(t,s)		(F(t) = (F(t) & ~STATE_MASK) | s)

#define ROLLOVER 			0x7FFFFFFF

static jr_timer_t *s_head = NULL;
static uint32_t s_now = 0;

static inline void insert(jr_timer_t *t) {
	uint32_t dl = t->_deadline;
	if (s_head == NULL || s_head->_deadline > dl) {
		t->_next = s_head;
		s_head = t;
	} else {
		jr_timer_t *curr = s_head;
		while (curr->_next != NULL && curr->_next->_deadline <= dl) {
			curr = curr->_next;
		}
		t->_next = curr->_next;
		curr->_next = t;
	}
}

static void remove(jr_timer_t *t) {
	if (s_head == t) {
		s_head = s_head->_next;
	} else {
		jr_timer_t *curr = s_head;
		while (curr) {
			if (curr->_next == t) {
				curr->_next = t->_next;
				break;
			}
			curr = curr->_next;
		}
	}
}

jr_status_t jr_timers_init() {
	return JR_OK;
}

void jr_timers_tick(uint32_t ticks) {
	s_now += ticks;

	JR_CRIT_ENTER();

	while (s_head && s_now >= s_head->_deadline) {
		jr_timer_t *curr = s_head;
		s_head = s_head->_next;
		SET_STATE(curr, STATE_RUNNING);

		JR_CRIT_LEAVE();
		
		// Run the timer outside the critical section.
		// At this point curr is not in the linked list and the list itself
		// is in a consistent state so it's safe for a handler
		// to call timer functions (whether on curr or any other timer)
		int ret = curr->_handler(curr);

		JR_CRIT_ENTER_NO_DECL();

		// If the timer state is still RUNNING it means no timer functions
		// were called on this timer during the handler's invocation. Use
		// the timer's type to determine the next action.
		if (STATE(curr) == STATE_RUNNING) {
			if (TYPE(curr) == JR_TIMER_ONE_SHOT || !ret) {
				SET_STATE(curr, STATE_IDLE);
			} else {
				if (TYPE(curr) == JR_TIMER_FIXED_INTERVAL) {
					curr->_deadline = s_now + INTERVAL(curr);
				} else if (TYPE(curr) == JR_TIMER_FIXED_RATE) {
					// TODO: how do we handle a missed schedule here?
					// idea - global error function that returns NEW next deadline
					curr->_deadline += INTERVAL(curr);
				}
				SET_STATE(curr, STATE_ENQUEUED);
				insert(curr);
			}
		}

		// The other possible states for the timer are:
		// IDLE - the timer was stopped during its execution. Nothing to do.
		// ENQEUED - the timer was restarted. Nothing to do.
	}

	if (s_now > ROLLOVER) {
		s_now -= ROLLOVER;
		jr_timer_t *curr = s_head;
		while (curr) {
			curr->_deadline -= ROLLOVER;
			curr = curr->_next;
		}
	}

	JR_CRIT_LEAVE();
}

void jr_timer_init(jr_timer_t *t, uint32_t type, jr_timer_hnd handler, jr_userdata_t userdata) {
	memset(t, 0, sizeof(jr_timer_t));
	t->_handler = handler;
	t->_flags = type;
	t->userdata = userdata;
}

jr_status_t jr_timer_start(jr_timer_t *t, uint32_t initial_delay, uint32_t repeat_interval) {
	JR_CRIT_ENTER();

	if (STATE(t) == STATE_ENQUEUED) {
		remove(t);
	}

	// Set up the task + enqueue
	t->_deadline = s_now + initial_delay;
	t->_flags = TYPE(t) | STATE_ENQUEUED | repeat_interval;
	insert(t);

	JR_CRIT_LEAVE();

	return JR_OK;
}

jr_status_t jr_timer_stop(jr_timer_t *t) {
	JR_CRIT_ENTER();
	if (STATE(t) == STATE_ENQUEUED) {
		remove(t);
	}
	SET_STATE(t, STATE_IDLE);
	JR_CRIT_LEAVE();
	return JR_OK;
}
