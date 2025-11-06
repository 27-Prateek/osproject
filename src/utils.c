// /home/nishit/Desktop/OS/nishit/osproject/src/utils.c
#define _DEFAULT_SOURCE
#include "../include/utils.h"
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>

// TIME UTILITIES

// Get current time in milliseconds
long get_current_time_ms(void) {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}



// Global log file pointers
static FILE *log_file = NULL;

// Initialize logging
void init_logging(void) {
    log_file = fopen("logs/scheduler.log", "w");
    if (log_file) {
        fprintf(log_file, "=== Logging initialized ===\n");
        fflush(log_file);
        printf("[LOGGING] Initialized: logs/scheduler.log\n");
    } else {
        printf("[ERROR] Cannot create logs/scheduler.log\n");
    }
}
// Close logging
void close_logging(void) {
    if (log_file) {
        fprintf(log_file, "=== Logging closed ===\n");
        fflush(log_file);
        fclose(log_file);
        log_file = NULL;
        printf("[LOGGING] Closed\n");
    }
}

// Get current timestamp
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "[%a %b %d %H:%M:%S %Y]", tm_info);
}

// LOGGING UTILITIES

// Generic log message with level
void log_message(const char *level, const char *message) {
    char timestamp[100];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Print to CONSOLE
    printf("%s [%s] %s\n", timestamp, level, message);
    
    // WRITE TO FILE
    if (log_file) {
        fprintf(log_file, "%s [%s] %s\n", timestamp, level, message);
        fflush(log_file);
    }
}

// Log error message
void log_error(const char *format, ...) {
    char timestamp[100];
    get_timestamp(timestamp, sizeof(timestamp));
    
    va_list args;
    
    // Print to CONSOLE
    printf("%s [ERROR] ", timestamp);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    
    // WRITE TO FILE
    if (log_file) {
        fprintf(log_file, "%s [ERROR] ", timestamp);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
}

// Log info message
void log_info(const char *format, ...) {
    char timestamp[100];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Print to console
    printf("%s [INFO] ", timestamp);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");

    // Write to log file
    if (log_file) {
        fprintf(log_file, "%s [INFO] ", timestamp);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n"); 
        fflush(log_file);
    }
}

// Log debug message
void log_debug(const char *format, ...) {
    char timestamp[100];
    get_timestamp(timestamp, sizeof(timestamp));
    
    va_list args;
    
    // Print to CONSOLE
    printf("%s [DEBUG] ", timestamp);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    
    // WRITE TO FILE
    if (log_file) {
        fprintf(log_file, "%s [DEBUG] ", timestamp);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
}

// STRING UTILITIES


// Trim leading and trailing whitespace
char* trim_whitespace(char *str) {
    char *end;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    // Write new null terminator
    *(end + 1) = '\0';
    
    return str;
}

// Convert string to integer safely
int string_to_int(const char *str) {
    if (str == NULL) return 0;
    return atoi(str);
}


// MATH UTILITIES

// Return minimum of two integers
int min(int a, int b) {
    return (a < b) ? a : b;
}

// Return maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Calculate percentage
double calculate_percentage(int value, int total) {
    if (total == 0) return 0.0;
    return ((double)value / (double)total) * 100.0;
}

// MEMORY UTILITIES

// Safe malloc with error checking
void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        log_error("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    memset(ptr, 0, size);
    return ptr;
}

// Safe free that sets pointer to NULL
void safe_free(void **ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

// Sleep for specified milliseconds
void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}
