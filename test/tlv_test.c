#include "tinytest.h"
#include "jr_config.h"

//
//

static void test_empty() {
	jr_status_t ret = jr_tlv_parse(NULL, 0, 0, 1, 1, NULL, JR_NONE);
	ASSERT_EQUALS(JR_OK, ret);
}

//
//

static void test_valid() {
	static uint8_t input[] = {
		1, 4, 1, 2, 3, 4,
		2, 3, 1, 2, 3,
		3, 2, 1, 2
	};

	jr_status_t ret = jr_tlv_parse(input, sizeof(input), 0, 1, 1, NULL, JR_NONE);
	ASSERT_EQUALS(JR_OK, ret);
}

//
//

static void test_invalid() {
	static uint8_t input[] = {
		1, 4, 1, 2, 3, 4,
		2, 5, 1, 2, 3
	};

	jr_status_t ret = jr_tlv_parse(input, sizeof(input), 0, 1, 1, NULL, JR_NONE);
	ASSERT_EQUALS(JR_PARSE_ERROR, ret);
}

//
//

static int count = 0;
static int types[3];
static int lengths[3];

static jr_status_t callback(uint32_t type, const uint8_t *data, uint32_t len, jr_userdata_t userdata) {
	if (!ASSERT_EXP("userdata", userdata.u32 == 100)) {
		return JR_OK;
	}
	
	switch (count) {
		case 0:
			if (!ASSERT_EXP("type", 1 == type)
				|| !ASSERT_EXP("len", 0 == len)) {
				return JR_OK;
			}
			break;
		case 1:
			if (!ASSERT_EXP("type", 2 == type)
				|| !ASSERT_EXP("len", 1 == len)
				|| !ASSERT_EXP("data[0]", data[0] == 1)) {
				return JR_OK;
			}
			break;
		case 2:
			if (!ASSERT_EXP("type", 3 == type)
				|| !ASSERT_EXP("len", 2 == len)
				|| !ASSERT_EXP("data[0]", data[0] == 2)
				|| !ASSERT_EXP("data[1]", data[1] == 3)) {
				return JR_OK;
			}
			break;
		default:
			break;
	}

	count++;
	return JR_OK;
}

static void test_callback_invocation() {
	static uint8_t input[] = {
		1, 0,
		2, 1, 1,
		3, 2, 2, 3
	};	
	jr_userdata_t ud = { .u32 = 100 };
	jr_status_t ret = jr_tlv_parse(input, sizeof(input), 0, 1, 1, callback, ud);
	ASSERT_EQUALS(JR_OK, ret);
	ASSERT_EQUALS(3, count);
}

//
//

static int data_invalid__called = 0;

static jr_status_t data_invalid_callback(uint32_t type, const uint8_t *data, uint32_t len, jr_userdata_t userdata) {
	data_invalid__called = 1;
	return JR_OK;
}

static void test_callback_not_called_when_data_invalid() {
	static uint8_t input[] = {
		1, 0,
		2, 6, 1
	};	
	jr_status_t ret = jr_tlv_parse(input, sizeof(input), 0, 1, 1, data_invalid_callback, JR_NONE);
	ASSERT_EQUALS(0, data_invalid__called);
}

//
//

int main() {
	RUN(test_empty);
	RUN(test_valid);
	RUN(test_invalid);
	RUN(test_callback_invocation);
	RUN(test_callback_not_called_when_data_invalid);
	return TEST_REPORT();
}
