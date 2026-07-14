# Tools
CC = clang
CFLAGS = -Wall -Wextra -Werror -g -fsanitize=address -DDEBUG
DEPFLAGS = -MMD -MP
RM = rm -f

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib
TEST_DIR = test

# Files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))
DEP = $(OBJ:.o=.d)
TARGET = md2html
TARGET_PATH = $(BIN_DIR)/$(TARGET)

# Libraries
LIB = 

# Colors
C_GREEN = \033[92m
C_RESET = \033[0m

all: $(TARGET_PATH)

$(TARGET_PATH): $(OBJ)
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Linking $(TARGET_PATH):$(C_RESET)'
	$(CC) -fsanitize=address $^ -L$(LIB_DIR) $(addprefix -l, $(LIB)) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo -e '$(C_GREEN)Compiling $<:$(C_RESET)'
	$(CC) $(CFLAGS) $(DEPFLAGS) -I$(INC_DIR) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

-include $(DEP)

.PHONY: clean test

clean:
	@echo -e '$(C_GREEN)Cleaning:$(C_RESET)'
	$(RM) $(OBJ) $(DEP) $(TARGET_PATH)

test:
	@echo -e '$(C_GREEN)Running tests:$(C_RESET)'