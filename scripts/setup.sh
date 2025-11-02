# /home/nishit/Desktop/OS/nishit/osproject/include/scheduler.h
#!/bin/bash


# Battery-Aware Scheduler Setup Script

# This script sets up the development environment
# and builds the battery-aware scheduler project


echo "========================================"
echo "  Battery-Aware Scheduler Setup"
echo "========================================"
echo ""


# Color definitions for output

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color


# Check if running on Linux

if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo -e "${YELLOW}Warning: This script is designed for Linux systems.${NC}"
    echo -e "${YELLOW}Some features may not work on other platforms.${NC}"
    echo ""
fi


# Check for required tools

echo "Checking for required tools..."

# Check for gcc
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}Error: gcc is not installed${NC}"
    echo "Please install gcc: sudo apt-get install gcc"
    exit 1
else
    echo -e "${GREEN}✓ gcc found: $(gcc --version | head -n1)${NC}"
fi

# Check for make
if ! command -v make &> /dev/null; then
    echo -e "${RED}Error: make is not installed${NC}"
    echo "Please install make: sudo apt-get install make"
    exit 1
else
    echo -e "${GREEN}✓ make found: $(make --version | head -n1)${NC}"
fi

echo ""


# Create necessary directories

echo "Creating necessary directories..."

mkdir -p bin
mkdir -p obj
mkdir -p logs
mkdir -p output

echo -e "${GREEN}✓ Directories created${NC}"
echo ""


# Check directory structure

echo "Verifying project structure..."

REQUIRED_DIRS=("src" "include" "tests" "examples" "scripts")
MISSING_DIRS=0

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ ! -d "$dir" ]; then
        echo -e "${RED}✗ Missing directory: $dir${NC}"
        MISSING_DIRS=$((MISSING_DIRS + 1))
    else
        echo -e "${GREEN}✓ Found directory: $dir${NC}"
    fi
done

if [ $MISSING_DIRS -gt 0 ]; then
    echo -e "${RED}Error: Missing required directories${NC}"
    exit 1
fi

echo ""


# Check source files

echo "Checking source files..."

REQUIRED_SRC=("main.c" "scheduler.c" "battery_monitor.c" "task_manager.c" "utils.c")
MISSING_SRC=0

for file in "${REQUIRED_SRC[@]}"; do
    if [ ! -f "src/$file" ]; then
        echo -e "${RED}✗ Missing source file: src/$file${NC}"
        MISSING_SRC=$((MISSING_SRC + 1))
    else
        echo -e "${GREEN}✓ Found source file: src/$file${NC}"
    fi
done

if [ $MISSING_SRC -gt 0 ]; then
    echo -e "${YELLOW}Warning: Some source files are missing${NC}"
fi

echo ""


# Check header files

echo "Checking header files..."

REQUIRED_HEADERS=("scheduler.h" "battery_monitor.h" "task_manager.h" "utils.h")
MISSING_HEADERS=0

for file in "${REQUIRED_HEADERS[@]}"; do
    if [ ! -f "include/$file" ]; then
        echo -e "${RED}✗ Missing header file: include/$file${NC}"
        MISSING_HEADERS=$((MISSING_HEADERS + 1))
    else
        echo -e "${GREEN}✓ Found header file: include/$file${NC}"
    fi
done

if [ $MISSING_HEADERS -gt 0 ]; then
    echo -e "${YELLOW}Warning: Some header files are missing${NC}"
fi

echo ""


# Set permissions

echo "Setting script permissions..."

chmod +x scripts/*.sh
echo -e "${GREEN}✓ Script permissions set${NC}"
echo ""


# Build the project

echo "Building the project..."
echo ""

if [ -f "Makefile" ]; then
    make clean
    if make; then
        echo ""
        echo -e "${GREEN}✓ Build successful!${NC}"
    else
        echo ""
        echo -e "${RED}✗ Build failed${NC}"
        exit 1
    fi
else
    echo -e "${YELLOW}Warning: Makefile not found${NC}"
    echo "Attempting manual compilation..."
    
    gcc -c src/utils.c -o obj/utils.o -Iinclude
    gcc -c src/battery_monitor.c -o obj/battery_monitor.o -Iinclude
    gcc -c src/task_manager.c -o obj/task_manager.o -Iinclude
    gcc -c src/scheduler.c -o obj/scheduler.o -Iinclude
    gcc -c src/main.c -o obj/main.o -Iinclude
    
    gcc obj/utils.o obj/battery_monitor.o obj/task_manager.o obj/scheduler.o obj/main.o -o bin/scheduler -lm
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Manual compilation successful!${NC}"
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        exit 1
    fi
fi

echo ""


# Build tests

echo "Building test suites..."

if [ -f "tests/test_scheduler.c" ]; then
    gcc tests/test_scheduler.c src/scheduler.c src/battery_monitor.c src/task_manager.c src/utils.c -o bin/test_scheduler -Iinclude -lm
    echo -e "${GREEN}✓ test_scheduler built${NC}"
fi

if [ -f "tests/test_battery_monitor.c" ]; then
    gcc tests/test_battery_monitor.c src/battery_monitor.c src/utils.c -o bin/test_battery_monitor -Iinclude -lm
    echo -e "${GREEN}✓ test_battery_monitor built${NC}"
fi

if [ -f "tests/test_task_manager.c" ]; then
    gcc tests/test_task_manager.c src/task_manager.c src/utils.c -o bin/test_task_manager -Iinclude -lm
    echo -e "${GREEN}✓ test_task_manager built${NC}"
fi

echo ""


# Build examples

echo "Building examples..."

if [ -f "examples/example_tasks.c" ]; then
    gcc examples/example_tasks.c src/scheduler.c src/battery_monitor.c src/task_manager.c src/utils.c -o bin/example_tasks -Iinclude -lm
    echo -e "${GREEN}✓ example_tasks built${NC}"
fi

echo ""


# Summary

echo "========================================"
echo "  Setup Complete!"
echo "========================================"
echo ""
echo "Available executables:"
echo "  - bin/scheduler          (Main program)"
echo "  - bin/test_scheduler     (Scheduler tests)"
echo "  - bin/test_battery_monitor (Battery tests)"
echo "  - bin/test_task_manager  (Task manager tests)"
echo "  - bin/example_tasks      (Example scenarios)"
echo ""
echo "Quick start commands:"
echo "  Run main program:        ./bin/scheduler"
echo "  Run simulation:          ./bin/scheduler --simulate"
echo "  Run all tests:           ./scripts/run_tests.sh"
echo "  Run examples:            ./bin/example_tasks"
echo "  Clean build:             make clean"
echo ""
echo -e "${GREEN}Setup completed successfully!${NC}"
echo ""
