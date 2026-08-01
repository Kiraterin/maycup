# Tools
CC := clang
CFLAGS += -Wall -Wextra -Werror
CFLAGS_LINK +=
DEPFLAGS += -MMD -MP
RM := rm -rf

# Directories
SRC_DIR := src
BUILD_DIR_ROOT := build
BIN_DIR_ROOT := bin
INC_DIR := include test
LIB_DIR := lib
TEST_DIR := test

# Files
SRC := $(shell find $(SRC_DIR) -name "*.c")
SRC_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

TEST_SRC := $(shell find $(TEST_DIR) -name "*.c")
TEST_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(TEST_SRC))

OBJ :=
DEP :=

TARGET :=

# Libraries
LIB := 

# Build configuration
CONFIG :=
BUILD_DIR = $(BUILD_DIR_ROOT)/$(CONFIG)
BIN_DIR = $(BIN_DIR_ROOT)/$(CONFIG)

ifeq ($(CONFIG), release)
	CFLAGS += -O2 -DNDEBUG
	OBJ := $(SRC_OBJ)
	TARGET := $(BIN_DIR)/md2html
else ifeq ($(CONFIG), debug)
	CFLAGS += -g -O0 $(CFLAGS_LINK) -DDEBUG
	CFLAGS_LINK += -fsanitize=address -fno-omit-frame-pointer 
	OBJ := $(SRC_OBJ)
	TARGET := $(BIN_DIR)/md2html
else ifeq ($(CONFIG), test)
	CFLAGS += -g -O0 -fsanitize=address -DDEBUG -DTEST
	CFLAGS_LINK += -fsanitize=address -fno-omit-frame-pointer
	OBJ := $(SRC_OBJ) $(TEST_OBJ)
	OBJ := $(filter-out %/main.o, $(OBJ))
	TARGET := $(BIN_DIR)/md2html_test
else ifeq ($(CONFIG),)
else
$(error Invalid CONFIG type: $(CONFIG))
endif

DEP := $(OBJ:.o=.d)

# Colors
C_GREEN := \033[92m
C_RESET := \033[0m

# Goal control
.DEFAULT_GOAL := all
.PHONY: nothing all build release debug test test_not_run run_test clean

nothing:

all:
	$(MAKE) release --no-print-directory
	$(MAKE) debug --no-print-directory
	$(MAKE) test --no-print-directory

build: $(TARGET)

clean:
	@echo -e '$(C_GREEN)Cleaning:$(C_RESET)'
	$(RM) -- $(BUILD_DIR_ROOT) $(BIN_DIR_ROOT)

release:
	@echo -e '\n$(C_GREEN)Current Configuration: release $(C_RESET)'
	$(MAKE) CONFIG=release build --no-print-directory

debug:
	@echo -e '\n$(C_GREEN)Current Configuration: debug $(C_RESET)'
	$(MAKE) CONFIG=debug build --no-print-directory

test:
	@echo -e '\n$(C_GREEN)Current Configuration: test $(C_RESET)'
	$(MAKE) CONFIG=test run_test --no-print-directory
	
test_not_run:
	@echo -e '\n$(C_GREEN)Current Configuration: test $(C_RESET)'
	$(MAKE) CONFIG=test build --no-print-directory

run_test: build
	@echo -e '$(C_GREEN)Running test:$(C_RESET)'
	$(TARGET)



$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Linking $@:$(C_RESET)'
	$(CC) $(CFLAGS_LINK) $^ -L$(LIB_DIR) $(addprefix -l, $(LIB)) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Compiling $<:$(C_RESET)'
	$(CC) $(CFLAGS) $(DEPFLAGS) $(addprefix -I, $(INC_DIR)) -c $< -o $@

-include $(DEP)
