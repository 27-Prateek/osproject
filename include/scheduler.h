#ifndef SCHEDULER_H
#define SCHEDULER_H

int init_scheduler(void);
void schedule_tasks(int battery_level);
void cleanup_scheduler(void);

#endif