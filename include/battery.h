#ifndef BATTERY_H
#define BATTERY_H

typedef struct {
    int charge_percent;    // Battery charge level (0-100%)
    int is_discharging;   // 1 if discharging, 0 if charging
} BatteryStatus;

int init_battery_monitor(void);
int get_battery_status(BatteryStatus *status);
void cleanup_battery_monitor(void);

#endif