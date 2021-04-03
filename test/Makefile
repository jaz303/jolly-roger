SRC_DIR := ../src
OBJ_DIR := obj

JR_SRC := $(shell find $(SRC_DIR)/*.c)
JR_OBJ := $(addprefix $(OBJ_DIR)/,$(notdir $(JR_SRC:.c=.o)))

TEST_SRC := $(shell find *_test.c)
TEST_BIN := $(TEST_SRC:.c=.test)

%.test: %.c $(JR_OBJ)
	gcc -o $@ -I../include $(JR_OBJ) $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	gcc -c -o $@ -I. -I../include $<	

default: $(TEST_BIN)
	@for t in $(TEST_BIN); do ./$$t; done

$(OBJ_DIR):
	mkdir -p obj

clean:
	rm -rf $(TEST_BIN)
	rm -rf $(OBJ_DIR)

.PHONY: default clean

.KEEP: $(JR_OBJ)
