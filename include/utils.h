#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

// CONSTANTS AND MACROS
#define MAX_TASKS 50
#define MAX_TASK_NAME 64
#define MAX_LOG_MSG 256

// Battery level thresholds (percentage)
#define BATTERY_CRITICAL 10
#define BATTERY_LOW 25
#define BATTERY_MEDIUM 50
#define BATTERY_HIGH 75

// Task priority levels
#define PRIORITY_HIGH 1
#define PRIORITY_MEDIUM 2
#define PRIORITY_LOW 3

// Energy consumption levels
#define ENERGY_HIGH 3
#define ENERGY_MEDIUM 2
#define ENERGY_LOW 1

// Return codes
#define SUCCESS 0
#define ERROR -1

// DATA STRUCTURES

// Battery state enumeration
typedef enum {
    BATTERY_STATE_CHARGING,
    BATTERY_STATE_DISCHARGING,
    BATTERY_STATE_FULL,
    BATTERY_STATE_UNKNOWN
} BatteryState;

// Task state enumeration
typedef enum {
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_WAITING,
    TASK_STATE_COMPLETED,
    TASK_STATE_SUSPENDED
} TaskState;

// ===== LOGGING FUNCTIONS =====
void init_logging(void);
void close_logging(void);
void get_timestamp(char *buffer, size_t size);
void log_info(const char *format, ...);
void log_debug(const char *format, ...);
void log_error(const char *format, ...);
void log_message(const char *level, const char *message);

// ===== DISPLAY FUNCTIONS =====
void print_status(void);
void display_all_tasks(void);
void display_ready_queue(void);
void print_battery_status(void);
void print_scheduler_status(void);
void print_task_statistics(void);
void print_scheduler_statistics(void);
void display_statistics(void);

// UTILITY FUNCTIONS

// Time utilities
long get_current_time_ms(void);
void sleep_ms(int milliseconds);

// String utilities
char* trim_whitespace(char *str);
int string_to_int(const char *str);

// Math utilities
int min(int a, int b);
int max(int a, int b);
double calculate_percentage(int value, int total);

// Memory utilities
void* safe_malloc(size_t size);
void safe_free(void **ptr);

#endif // UTILS_H
