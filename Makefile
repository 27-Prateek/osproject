# Battery-Aware Scheduler Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -I./include
LDFLAGS = -lm -lpthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BIN_DIR = bin
OBJ_DIR = obj
LOG_DIR = logs
OUTPUT_DIR = output
EXAMPLES_DIR = examples

# Create directories if they don't exist
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR) $(LOG_DIR) $(OUTPUT_DIR))

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
EXAMPLE_SRC = $(EXAMPLES_DIR)/example_tasks.c

# Object files (intermediate compiled files)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Common object files (exclude main.c and example_tasks.c)
COMMON_OBJS = $(OBJ_DIR)/battery_monitor.o $(OBJ_DIR)/task_manager.o \
              $(OBJ_DIR)/scheduler.o $(OBJ_DIR)/utils.o

# Executables
MAIN_EXEC = $(BIN_DIR)/scheduler
EXAMPLE_EXEC = $(BIN_DIR)/example_tasks
TEST_BATTERY = $(BIN_DIR)/test_battery_monitor
TEST_TASK = $(BIN_DIR)/test_task_manager
TEST_SCHEDULER = $(BIN_DIR)/test_scheduler

# ============================================
# Main Targets
# ============================================

# Default target - builds everything
all: $(MAIN_EXEC) $(EXAMPLE_EXEC) tests
	@echo "=========================================="
	@echo "Build Complete!"
	@echo "Main program: $(MAIN_EXEC)"
	@echo "Examples: $(EXAMPLE_EXEC)"
	@echo "Tests: $(BIN_DIR)/test_*"
	@echo "=========================================="

# Build main scheduler program
$(MAIN_EXEC): $(OBJ_DIR)/main.o $(COMMON_OBJS)
	@echo "Linking main program..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Main program built: $@"

# Build example tasks program
$(EXAMPLE_EXEC): $(EXAMPLE_SRC) $(COMMON_OBJS)
	@echo "Linking example tasks..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Example tasks built: $@"

# Build test executables
tests: $(TEST_BATTERY) $(TEST_TASK) $(TEST_SCHEDULER)
	@echo "✓ All tests built"

$(TEST_BATTERY): $(TEST_DIR)/test_battery_monitor.c $(OBJ_DIR)/battery_monitor.o $(OBJ_DIR)/utils.o
	@echo "Building battery monitor test..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_TASK): $(TEST_DIR)/test_task_manager.c $(OBJ_DIR)/task_manager.o $(OBJ_DIR)/utils.o
	@echo "Building task manager test..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_SCHEDULER): $(TEST_DIR)/test_scheduler.c $(COMMON_OBJS)
	@echo "Building scheduler test..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ============================================
# Object File Compilation
# ============================================

# Pattern rule: how to create .o files from .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# ============================================
# Utility Targets
# ============================================

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/*
	rm -rf $(LOG_DIR)/*.log
	@echo "✓ Clean complete"

# Deep clean (removes all generated files and logs)
distclean: clean
	@echo "Performing deep clean..."
	rm -rf $(OUTPUT_DIR)/*
	@echo "✓ Deep clean complete"

# Run main program
run: $(MAIN_EXEC)
	@echo "Running scheduler..."
	./$(MAIN_EXEC)

# Run simulation mode
simulate: $(MAIN_EXEC)
	@echo "Running simulation mode..."
	./$(MAIN_EXEC) --simulate

# Run all tests
test: tests
	@echo "=========================================="
	@echo "Running All Tests"
	@echo "=========================================="
	@echo "\n[1/3] Battery Monitor Tests:"
	-./$(TEST_BATTERY)
	@echo "\n[2/3] Task Manager Tests:"
	-./$(TEST_TASK)
	@echo "\n[3/3] Scheduler Tests:"
	-./$(TEST_SCHEDULER)
	@echo "=========================================="
	@echo "Tests Complete"
	@echo "=========================================="

# Check for compilation errors only (no linking)
check:
	@echo "Checking for syntax errors..."
	$(CC) $(CFLAGS) -fsyntax-only $(SRCS)
	@echo "✓ No syntax errors found"

# Show help
help:
	@echo "=========================================="
	@echo "Battery-Aware Scheduler Makefile"
	@echo "=========================================="
	@echo "Available targets:"
	@echo "  make all        - Build everything (default)"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make distclean  - Remove all generated files"
	@echo "  make run        - Build and run main program"
	@echo "  make simulate   - Run in simulation mode"
	@echo "  make test       - Build and run all tests"
	@echo "  make check      - Check syntax without building"
	@echo "  make help       - Show this help message"
	@echo "=========================================="

# Declare phony targets (targets that don't create files)
.PHONY: all clean distclean run simulate test tests check help
