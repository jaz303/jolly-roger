#include "tinytest.h"
#include "jr_config.h"

static void test_mem_read_uint() {
	uint8_t input1[] = {0x01};
	ASSERT_EQUALS(0x01, jr_mem_read_uint(input1, 1, 1));
	ASSERT_EQUALS(0x01, jr_mem_read_uint(input1, 1, 0));

	uint8_t input2[] = {0x01, 0x02};
	ASSERT_EQUALS(0x0102, jr_mem_read_uint(input2, 2, 1));
	ASSERT_EQUALS(0x0201, jr_mem_read_uint(input2, 2, 0));

	uint8_t input3[] = {0x01, 0x02, 0x03};
	ASSERT_EQUALS(0x010203, jr_mem_read_uint(input3, 3, 1));
	ASSERT_EQUALS(0x030201, jr_mem_read_uint(input3, 3, 0));

	uint8_t input4[] = {0x01, 0x02, 0x03, 0x4};
	ASSERT_EQUALS(0x01020304, jr_mem_read_uint(input4, 4, 1));
	ASSERT_EQUALS(0x04030201, jr_mem_read_uint(input4, 4, 0));
}

static void test_mem_read_uint16be() {
	uint8_t input[] = {0x01, 0x02};
	ASSERT_EQUALS(0x0102, jr_mem_read_uint16be(input));
}

static void test_mem_read_uint32be() {
	uint8_t input[] = {0x01, 0x02, 0x03, 0x4};
	ASSERT_EQUALS(0x01020304, jr_mem_read_uint32be(input));	
}

static void test_mem_read_uint16le() {
	uint8_t input[] = {0x01, 0x02};
	ASSERT_EQUALS(0x0201, jr_mem_read_uint16le(input));
}

static void test_mem_read_uint32le() {
	uint8_t input[] = {0x01, 0x02, 0x03, 0x4};
	ASSERT_EQUALS(0x04030201, jr_mem_read_uint32le(input));	
}

int main() {
	RUN(test_mem_read_uint);
	RUN(test_mem_read_uint16be);
	RUN(test_mem_read_uint32be);
	RUN(test_mem_read_uint16le);
	RUN(test_mem_read_uint32le);
	return TEST_REPORT();
}
