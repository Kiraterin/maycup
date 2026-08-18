# Version
VER := $(shell cat VERSION)
VER_MAJOR := $(word 1,$(subst ., ,$(VER)))
VER_MINOR := $(word 2,$(subst ., ,$(VER)))
VER_PATCH := $(word 3,$(subst ., ,$(VER)))
SO_MAJOR := $(VER_MAJOR)

# Names
NAME := maycup
TARGET_NAME := $(NAME)
ALIBTARGET_NAME := lib$(NAME).a
SLIBTARGET_NAME := lib$(NAME).so
SLIBTARGET_FULLNAME := $(SLIBTARGET_NAME).$(VER)
SLIBTARGET_SONAME := $(SLIBTARGET_NAME).$(SO_MAJOR)

# Directories
PREFIX ?= /usr/local
SRC_DIR := src
INCLUDE_DIR := include
TEST_DIR := test
BUILD_DIR_ROOT := build
INC_DIR := $(INCLUDE_DIR) $(SRC_DIR) $(TEST_DIR)
LIB_DIR := lib

# Tools
CC := clang
AR := ar
RM := rm
LN := ln
CFLAGS += -std=c23 -Wall -Wextra -Werror -D_GNU_SOURCE -DMAYCUP_VERSION=\"$(VER)\" -fPIC
LD_FLAGS +=
DEPFLAGS += -MMD -MP

# Build configuration
CONFIG :=
BUILD_DIR = $(BUILD_DIR_ROOT)/$(CONFIG)
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
LIBTARGET_DIR = $(BUILD_DIR)/lib
TARGET = $(BIN_DIR)/$(TARGET_NAME)
ALIBTARGET = $(LIBTARGET_DIR)/$(ALIBTARGET_NAME)
SLIBTARGET = $(LIBTARGET_DIR)/$(SLIBTARGET_FULLNAME)

# Files
SRC := $(shell find $(SRC_DIR) -name "*.c")
SRC_OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC))

TEST_SRC := $(shell find $(TEST_DIR) -name "*.c")
TEST_OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(TEST_SRC))

OBJ :=
DEP :=

# Libraries
LIB :=

ifeq ($(CONFIG), release)
	CFLAGS += -O3 -DNDEBUG -ffunction-sections -fdata-sections
	LD_FLAGS += -s -Wl,--gc-sections,-soname,$(SLIBTARGET_SONAME)
	OBJ := $(SRC_OBJ)
else ifeq ($(CONFIG), debug)
	CFLAGS += -g -O0 -fsanitize=address -DDEBUG
	LD_FLAGS += -fsanitize=address -fno-omit-frame-pointer
	OBJ := $(SRC_OBJ)
else ifeq ($(CONFIG), test)
	CFLAGS += -g -O0 -fsanitize=address -fprofile-instr-generate -fcoverage-mapping -DDEBUG -DTEST
	LD_FLAGS += -fsanitize=address -fprofile-instr-generate -fno-omit-frame-pointer
	OBJ := $(SRC_OBJ) $(TEST_OBJ)
	OBJ := $(filter-out %/main.o, $(OBJ))
	TARGET := $(BIN_DIR)/$(NAME)_test
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
.PHONY: nothing
	build_target build_alibtarget build_slibtarget \
	install uninstall \
	all release debug test \
	clean cov \
	test_not_run test_run cov_inner \

nothing:

all:
	$(MAKE) release --no-print-directory
	$(MAKE) debug --no-print-directory
	$(MAKE) test --no-print-directory
	$(MAKE) cov --no-print-directory

build_target: $(TARGET)
build_alibtarget: $(ALIBTARGET)
build_slibtarget: $(SLIBTARGET)

install: CONFIG := release
install: release
	@echo -e '$(C_GREEN)Installing...$(C_RESET)'
	install -Dm 755 $(TARGET) $(PREFIX)/bin/$(TARGET_NAME)
	install -Dm 644 $(ALIBTARGET) $(PREFIX)/lib/$(ALIBTARGET_NAME)
	install -Dm 755 $(SLIBTARGET) $(PREFIX)/lib/$(SLIBTARGET_FULLNAME)
	install -Dm 644 $(INCLUDE_DIR)/$(NAME).h $(PREFIX)/include/$(NAME).h
	install -d $(PREFIX)/include/$(NAME)
	install -m 644 $(INCLUDE_DIR)/$(NAME)/*.h $(PREFIX)/include/$(NAME)/
	$(LN) -sf $(SLIBTARGET_FULLNAME) $(PREFIX)/lib/$(SLIBTARGET_SONAME)
	$(LN) -sf $(SLIBTARGET_FULLNAME) $(PREFIX)/lib/$(SLIBTARGET_NAME)
	@echo -e '$(C_GREEN)Installation completed$(C_RESET)'

uninstall: CONFIG := release
uninstall:
	@echo -e '$(C_GREEN)Uninstalling...$(C_RESET)'
	$(RM) -r -- $(PREFIX)/bin/$(TARGET_NAME)
	$(RM) -r -- $(PREFIX)/lib/$(ALIBTARGET_NAME)
	$(RM) -r -- $(PREFIX)/lib/$(SLIBTARGET_NAME)
	$(RM) -r -- $(PREFIX)/lib/$(SLIBTARGET_NAME).*
	$(RM) -r -- $(PREFIX)/include/$(NAME).h
	$(RM) -rf -- $(PREFIX)/include/$(NAME)

clean:
	@echo -e '$(C_GREEN)Cleaning...$(C_RESET)'
	$(RM) -rf -- $(BUILD_DIR_ROOT)

release:
	@echo -e '\n$(C_GREEN)Current Configuration: release $(C_RESET)'
	$(MAKE) CONFIG=release build_target build_alibtarget build_slibtarget --no-print-directory

debug:
	@echo -e '\n$(C_GREEN)Current Configuration: debug $(C_RESET)'
	$(MAKE) CONFIG=debug build_target build_alibtarget build_slibtarget --no-print-directory

test:
	@echo -e '\n$(C_GREEN)Current Configuration: test $(C_RESET)'
	$(MAKE) CONFIG=test test_run --no-print-directory

test_not_run:
	@echo -e '\n$(C_GREEN)Current Configuration: test $(C_RESET)'
	$(MAKE) CONFIG=test build_target --no-print-directory
	@mkdir -p $(BUILD_DIR)/test/tmp

test_run: build_target
	@mkdir -p $(BUILD_DIR)/tmp
	@echo -e '$(C_GREEN)Running test:$(C_RESET)'
	LLVM_PROFILE_FILE="$(BUILD_DIR)/test.profraw" $(TARGET)
	@echo -e '$(C_GREEN)Saving perfdata:$(C_RESET)'
	llvm-profdata merge -sparse $(BUILD_DIR)/test.profraw -o $(BUILD_DIR)/test.profdata

cov:
	$(MAKE) CONFIG=test cov_inner --no-print-directory

cov_inner:
	@if [ ! -f "$(BUILD_DIR)/test.profdata" ]; then \
		$(MAKE) CONFIG=test test_run --no-print-directory; \
	fi
	@echo -e '$(C_GREEN)Test coverage:$(C_RESET)'
	llvm-cov report $(TARGET) -instr-profile=$(BUILD_DIR)/test.profdata \
		-show-region-summary=false \
		-show-branch-summary=false \
		-ignore-filename-regex='(^|/)test/.*|(^|/)src/debug/.*|(^|/)src/base/result.c'
	llvm-cov show $(TARGET) -instr-profile=$(BUILD_DIR)/test.profdata \
		-ignore-filename-regex='(^|/)test/.*|(^|/)src/debug/.*|(^|/)src/base/result.c' \
		-format=html -output-dir=$(BUILD_DIR)/cov
	@echo -e '$(C_GREEN)Coverage report: $(BUILD_DIR)/cov/index.html$(C_RESET)'

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Linking $@:$(C_RESET)'
	$(CC) -L$(LIB_DIR) $(addprefix -l, $(LIB)) $^ -o $@ $(LD_FLAGS)

$(ALIBTARGET): $(filter-out %/main.o, $(OBJ))
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Archiving $@:$(C_RESET)'
	$(AR) rcs $@ $^

$(SLIBTARGET): $(filter-out %/main.o, $(OBJ))
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Linking $@:$(C_RESET)'
	$(CC) -shared -L$(LIB_DIR) $(addprefix -l, $(LIB)) $^ -o $@ $(LD_FLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Compiling $<:$(C_RESET)'
	$(CC) $(CFLAGS) $(DEPFLAGS) $(addprefix -I, $(INC_DIR)) -c $< -o $@

-include $(DEP)