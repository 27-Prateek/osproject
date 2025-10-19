
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "battery.h"
#include "scheduler.h"

#define MONITOR_INTERVAL 5
#define RUN_DURATION 600 // 10 minutes in seconds

int main(int argc, char *argv[]) {
    const char *csv_file = (argc > 1 && strcmp(argv[1], "--non-scheduler") == 0) ? "non_scheduler_mode.csv" : "scheduler_mode.csv";
    printf("Starting Battery-Aware Scheduler (%s mode)...\n", (argc > 1 && strcmp(argv[1], "--non-scheduler") == 0) ? "Non-Scheduler" : "Scheduler");

    // Initialize CSV file
    FILE *csv = fopen(csv_file, "w");
    if (csv) {
        fprintf(csv, "Timestamp,BatteryPercent,Discharging\n");
        fclose(csv);
    } else {
        fprintf(stderr, "Failed to initialize %s\n", csv_file);
    }

    if (!init_battery_monitor()) {
        fprintf(stderr, "Failed to initialize battery monitor\n");
        return 1;
    }

    if (!init_scheduler()) {
        fprintf(stderr, "Failed to initialize scheduler\n");
        return 1;
    }

    if (!init_brightness_control()) {
        fprintf(stderr, "Failed to initialize brightness control\n");
        return 1;
    }

    time_t start_time = time(NULL);
    while (1) {
        if (time(NULL) - start_time >= RUN_DURATION) {
            break;
        }
        BatteryStatus status;
        if (get_battery_status(&status)) {
            char log_msg[128];
            snprintf(log_msg, sizeof(log_msg), "Battery status updated");
            log_event(log_msg, status.charge_percent, status.is_discharging, csv_file);
            printf("Battery: %d%%, Discharging: %s\n", 
                   status.charge_percent, status.is_discharging ? "Yes" : "No");
            schedule_tasks(status.charge_percent, status.is_discharging, csv_file);
        } else {
            log_event("Failed to read battery status", 0, 0, csv_file);
            fprintf(stderr, "Failed to read battery status\n");
        }
        sleep(MONITOR_INTERVAL);
    }

    cleanup_battery_monitor();
    cleanup_scheduler(csv_file);
    return 0;
}
