#include "jr.h"
#include "jr_config.h"

#include <string.h>

#define INCN(v, n)	(((v)+(n))&(rb->size-1))
#define INC(v) 		INCN(v, 1)

static int read_internal(jr_ringbuf_t *rb, uint8_t *dst, int len) {
	int available_end = rb->size - rb->rp;
	if (available_end >= len) {
		memcpy(dst, &rb->data[rb->rp], len);
	} else {
		memcpy(dst, &rb->data[rb->rp], available_end);
		memcpy(dst + available_end, &rb->data[0], len - available_end);
	}
	rb->rp = INCN(rb->rp, len);
	rb->count -= len;
	return len;
}

void jr_ringbuf_init(jr_ringbuf_t *rb, uint8_t *buffer, int size) {
	rb->data = buffer;
	rb->size = size;
	rb->count = 0;
	rb->rp = 0;
	rb->wp = 0;
}

int jr_ringbuf_count(jr_ringbuf_t *rb) {
	return rb->count;
}

int jr_ringbuf_remain(jr_ringbuf_t *rb) {
	return rb->size - rb->count;
}

int jr_ringbuf_discard_start(jr_ringbuf_t *rb, int len) {
	if (len > rb->count) {
		return JR_RANGE_ERROR;
	}
	rb->rp = INCN(rb->rp, len);
	rb->count -= len;
	return JR_OK;
}

int jr_ringbuf_discard_end(jr_ringbuf_t *rb, int len) {
	if (len > rb->count) {
		return JR_RANGE_ERROR;
	}
	if (rb->wp >= len) {
		rb->wp -= len;
	} else {
		rb->wp = rb->size - (len - rb->wp);
	}
	rb->count -= len;
	return JR_OK;
}

jr_status_t jr_ringbuf_push(jr_ringbuf_t *rb, uint8_t b) {
	if (rb->count == rb->size) {
		return JR_NO_MEM;
	}
	rb->count++;
	rb->data[rb->wp] = b;
	rb->wp = INC(rb->wp);
	return JR_OK;
}

uint8_t jr_ringbuf_shift(jr_ringbuf_t *rb) {
	if (rb->count == 0) {
		return 0;
	}
	uint8_t b = rb->data[rb->rp];
	rb->rp = INCN(rb->rp, 1);
	rb->count -= 1;
	return b;
}

jr_status_t jr_ringbuf_write(jr_ringbuf_t *rb, uint8_t *src, int len) {
	if (jr_ringbuf_remain(rb) < len) {
		return JR_NO_MEM;
	}
	int remain_end = rb->size - rb->wp;
	if (remain_end >= len) {
		memcpy(&rb->data[rb->wp], src, len);
	} else {
		memcpy(&rb->data[rb->wp], src, remain_end);
		memcpy(&rb->data[0], src + remain_end, len - remain_end);
	}
	rb->wp = INCN(rb->wp, len);
	rb->count += len;
	return JR_OK;
}

jr_status_t jr_ringbuf_read(jr_ringbuf_t *rb, uint8_t *dst, int len) {
	if (rb->count < len) {
		return JR_RANGE_ERROR;
	}
	read_internal(rb, dst, len);
	return JR_OK;
}

int jr_ringbuf_read_upto(jr_ringbuf_t *rb, uint8_t *dst, int len) {
	if (rb->count == 0) {
		return 0;
	}
	return read_internal(rb, dst, (rb->count < len) ? rb->count : len);
}
