CC = gcc
CFLAGS = -Wall -Iinclude
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
TARGET = $(BIN_DIR)/scheduler

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/battery.c $(SRC_DIR)/scheduler.c
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/battery.o $(OBJ_DIR)/scheduler.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean