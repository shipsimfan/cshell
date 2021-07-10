# DIRECTORIES
SRC_DIR := ./src
OBJ_DIR := ./obj
INCLUDE_DIR := ./include
SYSROOT_DIR := ../../sysroot

# SOURCE FILES
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# OBJECT FILES
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# TARGET
TARGET := shell.app

# PROGRAMS
CC := clang
CC_FLAGS := --target=x86_64-los --sysroot=$(SYSROOT_DIR) -Wall -g -I$(INCLUDE_DIR)

# COLORS
CYAN := \033[36;1m
RESET := \033[0m

# RULES
all: dirs $(TARGET)

clean:
	@rm -rf $(OBJ_DIR)/*
	@rm -f $(TARGET)
	
# COMPILATION RULES
.SECONDEXPANSION:

$(TARGET): $(OBJ_FILES)
	@echo "     $(CYAN)Linking$(RESET) $@ . . ."
	@$(CC) $(CC_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $$(@D)/.
	@echo "   $(CYAN)Compiling$(RESET) $^ . . ."
	@$(CC) $(CC_FLAGS) -c -o $@ $^

# DIRECTORY RULES
$(OBJ_DIR)/.:
	@mkdir -p $@

$(OBJ_DIR)%/.:
	@mkdir -p $@

dirs:
	@mkdir -p $(OBJ_DIR)

# . RULES
.PRECIOUS: $(OBJ_DIR)/. $(OBJ_DIR)%/.
.PHONY: dirs
