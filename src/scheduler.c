#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scheduler.h"

#define MAX_TASKS 10
#define LOW_BATTERY_THRESHOLD 20  // % battery to trigger low-power mode
#define BRIGHTNESS_PATH "/sys/class/backlight/intel_backlight/brightness"
#define BRIGHTNESS_MAX_PATH "/sys/class/backlight/intel_backlight/max_brightness"
#define BRIGHTNESS_LOW_PERCENT 50  // 50% brightness when battery is low
#define BRIGHTNESS_HIGH_PERCENT 100  // 100% brightness when battery is high

// Simulated task structure
typedef struct {
    int id;
    int energy_cost;  // Relative energy cost (1=low, 10=high)
    int priority;     // Priority (higher = more important)
} Task;

static Task tasks[MAX_TASKS];
static int task_count = 0;
static int original_brightness = -1;  // Store original brightness

int init_scheduler() {
    // Initialize with sample tasks (simulated)
    for (int i = 0; i < 3; i++) {
        tasks[i].id = i;
        tasks[i].energy_cost = (i % 2 == 0) ? 2 : 8;  // Low or high energy
        tasks[i].priority = 5 - i;  // Higher ID, lower priority
        task_count++;
    }
    printf("Scheduler initialized with %d tasks\n", task_count);
    return 1;
}

int init_brightness_control() {
    // Read maximum brightness
    FILE *fp = fopen(BRIGHTNESS_MAX_PATH, "r");
    if (!fp) {
        fprintf(stderr, "Cannot access max brightness file\n");
        return 0;
    }
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), fp)) {
        fclose(fp);
    } else {
        fclose(fp);
        fprintf(stderr, "Failed to read max brightness\n");
        return 0;
    }

    // Read and store current brightness
    fp = fopen(BRIGHTNESS_PATH, "r");
    if (!fp) {
        fprintf(stderr, "Cannot access brightness file\n");
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        original_brightness = atoi(buffer);
        printf("Original brightness: %d\n", original_brightness);
    } else {
        fclose(fp);
        fprintf(stderr, "Failed to read current brightness\n");
        return 0;
    }
    fclose(fp);
    return 1;
}

void set_brightness(int battery_level) {
    // Read maximum brightness
    FILE *fp = fopen(BRIGHTNESS_MAX_PATH, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open max brightness file\n");
        return;
    }
    char buffer[32];
    int max_brightness;
    if (fgets(buffer, sizeof(buffer), fp)) {
        max_brightness = atoi(buffer);
    } else {
        fclose(fp);
        fprintf(stderr, "Failed to read max brightness\n");
        return;
    }
    fclose(fp);

    // Calculate target brightness
    int target_brightness_percent = (battery_level <= LOW_BATTERY_THRESHOLD) ? BRIGHTNESS_LOW_PERCENT : BRIGHTNESS_HIGH_PERCENT;
    int target_brightness = (max_brightness * target_brightness_percent) / 100;

    // Set brightness
    fp = fopen(BRIGHTNESS_PATH, "w");
    if (fp) {
        fprintf(fp, "%d", target_brightness);
        fclose(fp);
        printf("Set brightness to %d%% (%d/%d)\n", target_brightness_percent, target_brightness, max_brightness);
    } else {
        fprintf(stderr, "Failed to set brightness (run as root)\n");
    }
}

void schedule_tasks(int battery_level) {
    // Set brightness based on battery level
    set_brightness(battery_level);

    // Schedule tasks
    printf("Scheduling tasks (battery: %d%%):\n", battery_level);
    for (int i = 0; i < task_count; i++) {
        if (battery_level <= LOW_BATTERY_THRESHOLD && tasks[i].energy_cost > 5) {
            printf("Skipping high-energy task %d (energy: %d)\n", tasks[i].id, tasks[i].energy_cost);
            continue;
        }
        printf("Running task %d (energy: %d, priority: %d)\n", 
               tasks[i].id, tasks[i].energy_cost, tasks[i].priority);
        // Simulate task execution
        sleep(1);
    }
}

void restore_brightness() {
    if (original_brightness == -1) return;  // No original brightness saved
    FILE *fp = fopen(BRIGHTNESS_PATH, "w");
    if (fp) {
        fprintf(fp, "%d", original_brightness);
        fclose(fp);
        printf("Restored brightness to %d\n", original_brightness);
    } else {
        fprintf(stderr, "Failed to restore brightness (run as root)\n");
    }
}

void cleanup_scheduler() {
    // Restore original brightness
    restore_brightness();
    // Clear tasks
    task_count = 0;
}
