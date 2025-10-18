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

// Sleep for specified milliseconds
void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}


// LOGGING UTILITIES

// Generic log message with level
void log_message(const char *level, const char *message) {
    time_t now;
    time(&now);
    char time_str[26];
    strncpy(time_str, ctime(&now), 26);
    time_str[24] = '\0';  // Remove newline
    
    printf("[%s] [%s] %s\n", time_str, level, message);
    fflush(stdout);
}

// Log error message
void log_error(const char *message) {
    char formatted_msg[MAX_LOG_MSG];
    snprintf(formatted_msg, MAX_LOG_MSG, "ERROR: %s", message);
    log_message("ERROR", message);
}

// Log info message
void log_info(const char *message) {
    log_message("INFO", message);
}

// Log debug message
void log_debug(const char *message) {
    log_message("DEBUG", message);
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
