# /home/nishit/Desktop/OS/nishit/osproject/include/scheduler.h
#!/bin/bash

# ============================================
# Battery-Aware Scheduler Clean Script
# ============================================
# This script cleans all build artifacts and
# temporary files from the project
# ============================================

echo "========================================"
echo "  Battery-Aware Scheduler Clean"
echo "========================================"
echo ""

# ============================================
# Color definitions for output
# ============================================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ============================================
# Parse command line arguments
# ============================================
DEEP_CLEAN=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --deep)
            DEEP_CLEAN=1
            shift
            ;;
        --help|-h)
            echo "Usage: ./clean.sh [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --deep    Perform deep clean (removes logs and output)"
            echo "  --help    Display this help message"
            echo ""
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# ============================================
# Clean object files
# ============================================
echo "Cleaning object files..."

if [ -d "obj" ]; then
    if [ "$(ls -A obj)" ]; then
        rm -f obj/*.o
        echo -e "${GREEN}✓ Removed object files from obj/${NC}"
    else
        echo -e "${BLUE}  obj/ directory is already empty${NC}"
    fi
else
    echo -e "${YELLOW}  obj/ directory does not exist${NC}"
fi

echo ""

# ============================================
# Clean binary files
# ============================================
echo "Cleaning binary files..."

if [ -d "bin" ]; then
    if [ "$(ls -A bin)" ]; then
        rm -f bin/scheduler
        rm -f bin/test_scheduler
        rm -f bin/test_battery_monitor
        rm -f bin/test_task_manager
        rm -f bin/example_tasks
        rm -f bin/*
        echo -e "${GREEN}✓ Removed executables from bin/${NC}"
    else
        echo -e "${BLUE}  bin/ directory is already empty${NC}"
    fi
else
    echo -e "${YELLOW}  bin/ directory does not exist${NC}"
fi

echo ""

# ============================================
# Clean backup files
# ============================================
echo "Cleaning backup and temporary files..."

BACKUP_COUNT=0

# Remove editor backup files
find . -name "*~" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))
find . -name "*.swp" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))
find . -name "*.swo" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))
find . -name ".*.swp" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))

# Remove core dumps
find . -name "core" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))
find . -name "core.*" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))

# Remove temporary files
find . -name "*.tmp" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))
find . -name "*.temp" -type f -delete 2>/dev/null && BACKUP_COUNT=$((BACKUP_COUNT + 1))

if [ $BACKUP_COUNT -gt 0 ]; then
    echo -e "${GREEN}✓ Removed backup and temporary files${NC}"
else
    echo -e "${BLUE}  No backup or temporary files found${NC}"
fi

echo ""

# ============================================
# Deep clean (optional)
# ============================================
if [ $DEEP_CLEAN -eq 1 ]; then
    echo "Performing deep clean..."
    echo ""
    
    # Clean logs
    if [ -d "logs" ]; then
        if [ "$(ls -A logs)" ]; then
            rm -f logs/*.log
            rm -f logs/*.txt
            echo -e "${GREEN}✓ Removed log files from logs/${NC}"
        else
            echo -e "${BLUE}  logs/ directory is already empty${NC}"
        fi
    fi
    
    # Clean output
    if [ -d "output" ]; then
        if [ "$(ls -A output)" ]; then
            rm -f output/*
            echo -e "${GREEN}✓ Removed files from output/${NC}"
        else
            echo -e "${BLUE}  output/ directory is already empty${NC}"
        fi
    fi
    
    # Remove directories
    echo ""
    echo "Removing build directories..."
    rm -rf obj/
    rm -rf bin/
    echo -e "${GREEN}✓ Removed obj/ and bin/ directories${NC}"
    
    echo ""
fi

# ============================================
# Run make clean if Makefile exists
# ============================================
if [ -f "Makefile" ]; then
    echo "Running make clean..."
    make clean 2>/dev/null
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ make clean completed${NC}"
    else
        echo -e "${YELLOW}  make clean encountered warnings${NC}"
    fi
    echo ""
fi

# ============================================
# Display summary
# ============================================
echo "========================================"
echo "  Clean Complete!"
echo "========================================"
echo ""

if [ $DEEP_CLEAN -eq 1 ]; then
    echo -e "${GREEN}Deep clean performed:${NC}"
    echo "  - Object files removed"
    echo "  - Binary files removed"
    echo "  - Backup files removed"
    echo "  - Log files removed"
    echo "  - Output files removed"
    echo "  - Build directories removed"
else
    echo -e "${GREEN}Standard clean performed:${NC}"
    echo "  - Object files removed"
    echo "  - Binary files removed"
    echo "  - Backup files removed"
    echo ""
    echo -e "${BLUE}Tip: Use './clean.sh --deep' for a more thorough clean${NC}"
fi

echo ""
echo "You can rebuild the project by running:"
echo "  ./scripts/setup.sh"
echo ""
echo -e "${GREEN}Clean completed successfully!${NC}"
echo ""
