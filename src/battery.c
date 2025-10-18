#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "battery.h"

// File paths for battery info (Linux-specific)
#define BATTERY_CAPACITY "/sys/class/power_supply/BAT1/capacity"  // or BATC
#define BATTERY_STATUS "/sys/class/power_supply/BAT1/status"      // or BATC

// Initialize battery monitoring by checking file access
int init_battery_monitor() {
    FILE *fp = fopen(BATTERY_CAPACITY, "r");
    if (!fp) {
        fprintf(stderr, "Cannot access battery capacity file\n");
        return 0;
    }
    fclose(fp);
    return 1;
}





// Read battery status (charge percentage and charging state)
int get_battery_status(BatteryStatus *status) {
    if (!status) {
        fprintf(stderr, "Invalid status pointer\n");
        return 0;
    }

    FILE *fp;
    char buffer[32];

    // Read battery capacity (percentage)
    fp = fopen(BATTERY_CAPACITY, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open battery capacity file\n");
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        status->charge_percent = atoi(buffer);
    } else {
        fclose(fp);
        fprintf(stderr, "Failed to read battery capacity\n");
        return 0;
    }
    fclose(fp);

    // Read battery status (charging/discharging)
    fp = fopen(BATTERY_STATUS, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open battery status file\n");
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), fp)) {
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline
        status->is_discharging = (strcmp(buffer, "Discharging") == 0);
    } else {
        fclose(fp);
        fprintf(stderr, "Failed to read battery status\n");
        return 0;
    }
    fclose(fp);

    return 1;
}

// Clean up battery monitoring resources
void cleanup_battery_monitor() {
    // No cleanup needed for file-based monitoring
}