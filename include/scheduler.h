#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "utils.h"
#include "battery_monitor.h"
#include "task_manager.h"


// SCHEDULER STRUCTURES

// Scheduling algorithm types
typedef enum {
    SCHEDULER_FCFS,                 // First Come First Serve
    SCHEDULER_SJF,                  // Shortest Job First
    SCHEDULER_PRIORITY,             // Priority-based scheduling
    SCHEDULER_ROUND_ROBIN,          // Round Robin
    SCHEDULER_BATTERY_AWARE         // Battery-aware custom scheduling
} SchedulerAlgorithm;

// Scheduler mode based on battery level
typedef enum {
    MODE_PERFORMANCE,               // High battery: all tasks run normally
    MODE_BALANCED,                  // Medium battery: prioritize efficient tasks
    MODE_POWER_SAVE,                // Low battery: only critical/low-energy tasks
    MODE_CRITICAL                   // Critical battery: minimal essential tasks
} SchedulerMode;

// Scheduler configuration
typedef struct {
    SchedulerAlgorithm algorithm;   // Current scheduling algorithm
    SchedulerMode mode;             // Current scheduler mode
    int time_quantum;               // Time quantum for round robin (ms)
    bool enable_preemption;         // Allow task preemption
    bool enable_aging;              // Prevent starvation with aging
    int aging_threshold;            // Time before priority boost (ms)
} SchedulerConfig;

// Scheduler state
typedef struct {
    Task *current_task;             // Currently executing task
    TaskQueue *ready_queue;         // Queue of ready tasks
    TaskQueue *waiting_queue;       // Queue of waiting/suspended tasks
    SchedulerConfig config;         // Scheduler configuration
    SchedulerMode mode;             // Current operating mode
    long total_runtime;             // Total scheduler runtime (ms)
    int context_switches;           // Number of context switches
    bool is_running;                // Is scheduler active
} SchedulerState;

// Scheduler statistics
typedef struct {
    int total_tasks_scheduled;      // Total tasks scheduled
    int tasks_completed;            // Tasks completed successfully
    int tasks_suspended;            // Tasks suspended due to low battery
    int context_switches;           // Number of context switches
    double cpu_utilization;         // CPU utilization percentage
    double battery_saved;           // Estimated battery saved (%)
    long total_energy_consumed;     // Total energy consumed
} SchedulerStats;


// SCHEDULER FUNCTIONS

// Initialization and cleanup
int scheduler_init(SchedulerAlgorithm algorithm);
void scheduler_cleanup(void);

// Scheduler control
int scheduler_start(void);
int scheduler_stop(void);
int scheduler_pause(void);
int scheduler_resume(void);

// Configuration management
int set_scheduler_algorithm(SchedulerAlgorithm algorithm);
int set_scheduler_mode(SchedulerMode mode);
int set_time_quantum(int quantum_ms);
SchedulerConfig* get_scheduler_config(void);
void configure_scheduler(SchedulerConfig *config);

// Core scheduling functions
Task* select_next_task(void);
int schedule_task(Task *task);
int execute_task(Task *task);
int preempt_task(Task *task);
int suspend_task(Task *task);
int resume_task(Task *task);

// Mode management based on battery
SchedulerMode determine_scheduler_mode(int battery_level);
int adjust_scheduler_for_battery(void);
int apply_power_saving_policies(void);

// Task admission control
bool can_admit_task(Task *task);
int admit_task_to_scheduler(Task *task);

// Scheduling algorithms implementation
Task* schedule_fcfs(void);
Task* schedule_sjf(void);
Task* schedule_priority(void);
Task* schedule_round_robin(void);
Task* schedule_battery_aware(void);

// Context switching
int perform_context_switch(Task *old_task, Task *new_task);

// Scheduler loop
void scheduler_run_loop(void);

// Statistics and monitoring
SchedulerStats* get_scheduler_statistics(void);
void update_scheduler_statistics(void);
void print_scheduler_status(void);
void print_scheduler_statistics(void);

// Debug and logging
void log_scheduling_decision(Task *task, const char *reason);
void print_ready_queue(void);

#endif // SCHEDULER_H
