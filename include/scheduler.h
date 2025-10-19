
#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_TASKS 10

typedef struct {
    int id;
    int energy_cost;
    int priority;
    char command[256];
} Task;

int init_scheduler(void);
void schedule_tasks(int battery_level, int is_discharging, const char *csv_file);
void cleanup_scheduler(const char *csv_file);
int init_brightness_control(void);
void set_brightness(int battery_level, int is_discharging, const char *csv_file);
void restore_brightness(const char *csv_file);
void log_event(const char *message, int battery_level, int is_discharging, const char *csv_file);

#endif
