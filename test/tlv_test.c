#include "tinytest.h"
#include "jr_config.h"

void test_sheep() {
  	// ASSERT("Sheep are cool", are_sheep_cool());
  	// ASSERT_EQUALS(4, sheep.legs);
}

void test_cheese() {
	// ASSERT("Cheese is tangy", cheese.tanginess > 0);
	// ASSERT_STRING_EQUALS("Wensleydale", cheese.name);
}

int main() {
	RUN(test_sheep);
	RUN(test_cheese);
	return TEST_REPORT();
}
