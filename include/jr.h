#ifndef JR_H_
#define JR_H_

#include <stdint.h>

//
// Status

typedef int jr_status_t;

#define JR_OK               0
#define JR_ERR              -1
#define JR_NO_MEM           -2
#define JR_INVALID_STATE    -3
#define JR_RANGE_ERROR      -4
#define JR_PARSE_ERROR      -5
#define JR_INVALID_ARG      -6

//
// Common types

typedef union jr_userdata {
    int32_t     i32;
    uint32_t    u32;
    void*       p;
} jr_userdata_t;

extern const jr_userdata_t JR_NONE;

//
// Memory utilities

uint32_t jr_mem_read_uint(const uint8_t *ptr, int bytes, int big_endian);

uint16_t jr_mem_read_uint16be(const uint8_t *ptr);
uint32_t jr_mem_read_uint32be(const uint8_t *ptr);

uint16_t jr_mem_read_uint16le(const uint8_t *ptr);
uint32_t jr_mem_read_uint32le(const uint8_t *ptr);

//
// Ringbuffer (byte-oriented, not threadsafe)

typedef struct jr_ringbuf {

    // private
    uint8_t *data;
    int size, count, rp, wp;
} jr_ringbuf_t;

void jr_ringbuf_init(jr_ringbuf_t *rb, uint8_t *buffer, int size);

// Returns number of bytes currently in ring buffer
int jr_ringbuf_count(jr_ringbuf_t *rb);

// Returns free space
int jr_ringbuf_remain(jr_ringbuf_t *rb);

// Discard len bytes from the start of buffer
// Returns JR_RANGE_ERROR if the buffer contains less than len bytes
int jr_ringbuf_discard_start(jr_ringbuf_t *rb, int len);

// Discard len bytes from the end of buffer
// Returns JR_RANGE_ERROR if the buffer contains less than len bytes
int jr_ringbuf_discard_end(jr_ringbuf_t *rb, int len);

// Write a single byte into the ring buffer
jr_status_t jr_ringbuf_push(jr_ringbuf_t *rb, uint8_t b);

// Remove first byte from ring buffer
// Returns 0 if ring buffer is empty so you should only use this function
// the ring buffer is known to be non-empty.
uint8_t jr_ringbuf_shift(jr_ringbuf_t *rb);

// Write exactly len bytes from src into the ring buffer
// Returns JR_NO_MEM if less than len bytes capacity is available for writing
jr_status_t jr_ringbuf_write(jr_ringbuf_t *rb, uint8_t *src, int len);

// Read exactly len bytes from buffer into dst
// Returns JR_??? if less than len bytes are available for reading
jr_status_t jr_ringbuf_read(jr_ringbuf_t *rb, uint8_t *dst, int len);

// Read up to len bytes from buffer into dst
// Returns number of bytes read
int jr_ringbuf_read_upto(jr_ringbuf_t *rb, uint8_t *dst, int len);

//
// Scheduler

typedef struct {
    int background_irq_priority;
} jr_sched_init_t;

typedef uint16_t jr_evt_size_t;
typedef void (*jr_sched_cb)(void *data, jr_evt_size_t len);

void jr_sched_init(jr_sched_init_t *cfg);
void jr_sched_start();
jr_status_t jr_sched_put(jr_sched_cb callback, void *data, jr_evt_size_t len);

//
// Timer

#define JR_TIMER_ONE_SHOT       0x40000000
#define JR_TIMER_FIXED_INTERVAL 0x80000000
#define JR_TIMER_FIXED_RATE     0xC0000000

#define JR_TIMER_MAX_INTERVAL   0x0FFFFFFF

typedef struct jr_timer jr_timer_t;

// Timer handler. Called in an interrupt context.
// Return value is ignored for single-shot timers.
// For repeating timers, return 0 to stop the timer and
// non-zero to keep the timer going.
typedef int (*jr_timer_hnd)(jr_timer_t *timer);

struct jr_timer {
    jr_userdata_t   userdata;

    // private
    jr_timer_hnd    _handler;
    uint32_t        _deadline;
    uint32_t        _flags;
    jr_timer_t*     _next;
};

jr_status_t jr_timers_init();
void jr_timers_tick(uint32_t ticks);

void jr_timer_init(jr_timer_t *t, uint32_t type, jr_timer_hnd handler, jr_userdata_t userdata);
jr_status_t jr_timer_start(jr_timer_t *t, uint32_t initial_delay, uint32_t repeat_interval);
jr_status_t jr_timer_stop(jr_timer_t *t);

// jr_status_t jr_set_timeout(jr_timer_cb cb, uint32_t delay, jr_userdata_t userdata);
// jr_status_t jr_set_interval(jr_timer_cb cb, uint32_t initial_dealy, uint32_t interval, jr_userdata_t userdata);
// jr_status_t jr_clear_timeout(jr_status_t instance);

//
// TLV

typedef jr_status_t (*jr_tlv_cb)(uint32_t type, const uint8_t *data, uint32_t length, jr_userdata_t userdata);

// Parse a TLV structure, calling the provided callback for each chunk.
// If callback is not provide this function checks the syntax only.
jr_status_t jr_tlv_parse(const uint8_t *bytes, int length, int big_endian, int type_size, int length_size, jr_tlv_cb chunk_cb, jr_userdata_t userdata);

#include "jr_config.h"

//
// Configurable macros

#ifndef JR_CRIT_ENTER
#define JR_CRIT_ENTER() \
    uint32_t jr_prev_primask = __get_PRIMASK(); \
    __disable_irq(); \
    __DMB()
#endif

#ifndef JR_CRIT_ENTER_NO_DECL
#define JR_CRIT_ENTER_NO_DECL() \
    jr_prev_primask = __get_PRIMASK(); \
    __disable_irq(); \
    __DMB()
#endif

#ifndef JR_CRIT_LEAVE
#define JR_CRIT_LEAVE() \
    __DMB(); \
    __set_PRIMASK(jr_prev_primask)
#endif

#ifndef JR_SCHED_SET_PRIORITY
#define JR_SCHED_SET_PRIORITY(p) NVIC_SetPriority(PendSV_IRQn, (p))
#endif

#ifndef JR_SCHED_SET_PENDING
#define JR_SCHED_SET_PENDING(p) (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk)
#endif

#ifndef JR_SCHED_CLEAR_PENDING
#define JR_SCHED_CLEAR_PENDING(p) (SCB->ICSR &= ~(SCB_ICSR_PENDSVSET_Msk))
#endif

#endif
