
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include "scheduler.h"

#define LOW_BATTERY_THRESHOLD 0
#define BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/brightness"
#define BRIGHTNESS_MAX_PATH "/sys/class/backlight/intel_backlight/max_brightness"
#define BRIGHTNESS_LOW_PERCENT 10
#define BRIGHTNESS_HIGH_PERCENT 100

static Task tasks[MAX_TASKS];
static int task_count = 0;
static int original_brightness = -1;

void log_event(const char *message, int battery_level, int is_discharging, const char *csv_file) {
    FILE *log = fopen("scheduler.log", "a");
    FILE *csv = fopen(csv_file, "a");
    if (log && csv) {
        time_t now;
        time(&now);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0';
        fprintf(log, "[%s] Battery: %d%%, Discharging: %s, %s\n", 
                time_str, battery_level, is_discharging ? "Yes" : "No", message);
        fprintf(csv, "%ld,%d,%s\n", now, battery_level, is_discharging ? "Yes" : "No");
        fclose(log);
        fclose(csv);
    } else {
        fprintf(stderr, "Failed to open log or CSV file\n");
    }
}

int init_scheduler() {
    tasks[0].id = 0; tasks[0].energy_cost = 2; tasks[0].priority = 5; strcpy(tasks[0].command, "uptime");
    tasks[1].id = 1; tasks[1].energy_cost = 8; tasks[1].priority = 4; strcpy(tasks[1].command, "stress --cpu 1 --timeout 5");
    tasks[2].id = 2; tasks[2].energy_cost = 2; tasks[2].priority = 3; strcpy(tasks[2].command, "date");
    task_count = 3;
    log_event("Scheduler initialized", 0, 0, "scheduler_mode.csv");
    printf("Scheduler initialized with %d tasks\n", task_count);
    return 1;
}

int init_brightness_control() {
    FILE *fp = fopen(BRIGHTNESS_MAX_PATH, "r");
    if (!fp) {
        log_event("Cannot access max brightness file", 0, 0, "scheduler_mode.csv");
        fprintf(stderr, "Cannot access max brightness file\n");
        return 0;
    }
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), fp)) {
        fclose(fp);
    } else {
        fclose(fp);
        log_event("Failed to read max brightness", 0, 0, "scheduler_mode.csv");
        fprintf(stderr, "Failed to read max brightness\n");
        return 0;
    }

    fp = fopen(BRIGHTNESS_PATH, "r");
    if (!fp) {
        log_event("Cannot access brightness file", 0, 0, "scheduler_mode.csv");
        fprintf(stderr, "Cannot access brightness file\n");
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        original_brightness = atoi(buffer);
        char log_msg[64];
        snprintf(log_msg, sizeof(log_msg), "Original brightness: %d", original_brightness);
        log_event(log_msg, 0, 0, "scheduler_mode.csv");
        printf("%s\n", log_msg);
    } else {
        fclose(fp);
        log_event("Failed to read current brightness", 0, 0, "scheduler_mode.csv");
        fprintf(stderr, "Failed to read current brightness\n");
        return 0;
    }
    fclose(fp);
    return 1;
}

void set_brightness(int battery_level, int is_discharging, const char *csv_file) {
    FILE *fp = fopen(BRIGHTNESS_MAX_PATH, "r");
    if (!fp) {
        log_event("Failed to open max brightness file", battery_level, is_discharging, csv_file);
        fprintf(stderr, "Failed to open max brightness file\n");
        return;
    }
    char buffer[32];
    int max_brightness;
    if (fgets(buffer, sizeof(buffer), fp)) {
        max_brightness = atoi(buffer);
    } else {
        fclose(fp);
        log_event("Failed to read max brightness", battery_level, is_discharging, csv_file);
        fprintf(stderr, "Failed to read max brightness\n");
        return;
    }
    fclose(fp);

    int target_brightness_percent = (battery_level <= LOW_BATTERY_THRESHOLD) ? BRIGHTNESS_LOW_PERCENT : BRIGHTNESS_HIGH_PERCENT;
    int target_brightness = (max_brightness * target_brightness_percent) / 100;

    fp = fopen(BRIGHTNESS_PATH, "w");
    if (fp) {
        fprintf(fp, "%d", target_brightness);
        fclose(fp);
        char log_msg[100];
        snprintf(log_msg, sizeof(log_msg), "Set brightness to %d%% (%d/%d)", 
                 target_brightness_percent, target_brightness, max_brightness);
        log_event(log_msg, battery_level, is_discharging, csv_file);
        printf("%s\n", log_msg);
    } else {
        log_event("Failed to set brightness (run as root)", battery_level, is_discharging, csv_file);
        fprintf(stderr, "Failed to set brightness (run as root)\n");
    }
}

void schedule_tasks(int battery_level, int is_discharging, const char *csv_file) {
    set_brightness(battery_level, is_discharging, csv_file);
    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "Scheduling tasks");
    log_event(log_msg, battery_level, is_discharging, csv_file);
    printf("Scheduling tasks (battery: %d%%):\n", battery_level);
    for (int i = 0; i < task_count; i++) {
        if (battery_level <= LOW_BATTERY_THRESHOLD && tasks[i].energy_cost > 5) {
            snprintf(log_msg, sizeof(log_msg), "Skipping high-energy task %d (command: %s, energy: %d)", 
                     tasks[i].id, tasks[i].command, tasks[i].energy_cost);
            printf("%s\n", log_msg);
            log_event(log_msg, battery_level, is_discharging, csv_file);
            continue;
        }
        snprintf(log_msg, sizeof(log_msg), "Running task %d (command: %s, energy: %d, priority: %d)", 
                 tasks[i].id, tasks[i].command, tasks[i].energy_cost, tasks[i].priority);
        printf("%s\n", log_msg);
        log_event(log_msg, battery_level, is_discharging, csv_file);

        // Execute the task
        pid_t pid = fork();
        if (pid == 0) { // Child process
            char *args[] = {"/bin/sh", "-c", tasks[i].command, NULL};
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        } else if (pid > 0) { // Parent process
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                snprintf(log_msg, sizeof(log_msg), "Task %d completed successfully", tasks[i].id);
            } else {
                snprintf(log_msg, sizeof(log_msg), "Task %d failed", tasks[i].id);
            }
            log_event(log_msg, battery_level, is_discharging, csv_file);
            printf("%s\n", log_msg);
        } else {
            snprintf(log_msg, sizeof(log_msg), "Failed to fork for task %d", tasks[i].id);
            log_event(log_msg, battery_level, is_discharging, csv_file);
            fprintf(stderr, "%s\n", log_msg);
        }
    }
}

void restore_brightness(const char *csv_file) {
    if (original_brightness == -1) return;
    FILE *fp = fopen(BRIGHTNESS_PATH, "w");
    if (fp) {
        fprintf(fp, "%d", original_brightness);
        fclose(fp);
        char log_msg[64];
        snprintf(log_msg, sizeof(log_msg), "Restored brightness to %d", original_brightness);
        log_event(log_msg, 0, 0, csv_file);
        printf("%s\n", log_msg);
    } else {
        log_event("Failed to restore brightness (run as root)", 0, 0, csv_file);
        fprintf(stderr, "Failed to restore brightness (run as root)\n");
    }
}

void cleanup_scheduler(const char *csv_file) {
    restore_brightness(csv_file);
    task_count = 0;
    log_event("Scheduler cleaned up", 0, 0, csv_file);
}
