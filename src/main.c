#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "battery.h"
#include "scheduler.h"

#define MONITOR_INTERVAL 5  // Seconds between battery checks

int main() {
    printf("Starting Battery-Aware Scheduler...\n");

    // Initialize battery monitoring
    if (!init_battery_monitor()) {
        fprintf(stderr, "Failed to initialize battery monitor\n");
        return 1;
    }

    // Initialize scheduler
    if (!init_scheduler()) {
        fprintf(stderr, "Failed to initialize scheduler\n");
        return 1;
    }

    // Main loop: Monitor battery and schedule tasks
    while (1) {
        BatteryStatus status;
        if (get_battery_status(&status)) {
            printf("Battery: %d%%, Discharging: %s\n", 
                   status.charge_percent, 
                   status.is_discharging ? "Yes" : "No");

            // Adjust scheduling based on battery level
            schedule_tasks(status.charge_percent);
        } else {
            fprintf(stderr, "Failed to read battery status\n");
        }

        // Sleep for monitoring interval
        sleep(MONITOR_INTERVAL);
    }

    // Cleanup (unreachable in this simple example)
    cleanup_battery_monitor();
    cleanup_scheduler();
    return 0;
}