# Tools
CC = clang
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -DDEBUG
DEPFLAGS = -MMD -MP
RM = rm -rf

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
TEST_DIR = tests

# Files
SRC = $(shell find $(SRC_DIR) -name "*.c")
SRC_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

TEST_SRC = $(shell find $(TEST_DIR) -name "*.c")
TEST_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, $(TEST_SRC))

OBJ = $(SRC_OBJ) $(TEST_OBJ)
DEP = $(OBJ:.o=.d)

TARGET = md2html
TEST_TARGET = md2html_test

# Libraries
LIB = 

# Colors
C_GREEN = \033[92m
C_RESET = \033[0m

.PHONY: all clean test

all: $(BIN_DIR)/$(TARGET) $(BIN_DIR)/$(TEST_TARGET)

clean:
	@echo -e '$(C_GREEN)Cleaning:$(C_RESET)'
	$(RM) -- $(BUILD_DIR) $(BIN_DIR)

test: $(BIN_DIR)/$(TEST_TARGET)
	@echo -e '$(C_GREEN)Running tests:$(C_RESET)'
	$(BIN_DIR)/$(TEST_TARGET)

$(BIN_DIR)/$(TARGET): $(SRC_OBJ)
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Linking $@:$(C_RESET)'
	$(CC) -fsanitize=address $^ -L$(LIB_DIR) $(addprefix -l, $(LIB)) -o $@

$(BIN_DIR)/$(TEST_TARGET): $(TEST_OBJ)
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Linking $@:$(C_RESET)'
	$(CC) -fsanitize=address $^ -L$(LIB_DIR) $(addprefix -l, $(LIB)) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Compiling $<:$(C_RESET)'
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(INC_DIR) -c $< -o $@

-include $(DEP)
