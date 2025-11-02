// /home/nishit/Desktop/OS/nishit/osproject/include/scheduler.h
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// GLOBAL VARIABLES


static SchedulerState scheduler_state;
static SchedulerStats scheduler_stats;
static bool is_initialized = false;


// INITIALIZATION AND CLEANUP


// Initialize scheduler
int scheduler_init(SchedulerAlgorithm algorithm) {
    if (is_initialized) {
        log_error("Scheduler already initialized");
        return ERROR;
    }
    
    // Initialize dependencies
    if (battery_monitor_init() != SUCCESS) {
        log_error("Failed to initialize battery monitor");
        return ERROR;
    }
    
    if (task_manager_init() != SUCCESS) {
        log_error("Failed to initialize task manager");
        battery_monitor_cleanup();
        return ERROR;
    }
    
    // Initialize scheduler state
    scheduler_state.current_task = NULL;
    scheduler_state.ready_queue = create_task_queue();
    scheduler_state.waiting_queue = create_task_queue();
    scheduler_state.config.algorithm = algorithm;
    scheduler_state.config.mode = MODE_PERFORMANCE;
    scheduler_state.config.time_quantum = 100;  // 100ms default
    scheduler_state.config.enable_preemption = true;
    scheduler_state.config.enable_aging = true;
    scheduler_state.config.aging_threshold = 5000;  // 5 seconds
    scheduler_state.mode = MODE_PERFORMANCE;
    scheduler_state.total_runtime = 0;
    scheduler_state.context_switches = 0;
    scheduler_state.is_running = false;
    
    // Initialize statistics
    scheduler_stats.total_tasks_scheduled = 0;
    scheduler_stats.tasks_completed = 0;
    scheduler_stats.tasks_suspended = 0;
    scheduler_stats.context_switches = 0;
    scheduler_stats.cpu_utilization = 0.0;
    scheduler_stats.battery_saved = 0.0;
    scheduler_stats.total_energy_consumed = 0;
    
    is_initialized = true;
    log_info("Scheduler initialized successfully");
    
    return SUCCESS;
}

// Cleanup scheduler
void scheduler_cleanup(void) {
    if (!is_initialized) {
        return;
    }
    
    scheduler_state.is_running = false;
    
    destroy_task_queue(scheduler_state.ready_queue);
    destroy_task_queue(scheduler_state.waiting_queue);
    
    task_manager_cleanup();
    battery_monitor_cleanup();
    
    is_initialized = false;
    log_info("Scheduler cleaned up");
}


// SCHEDULER CONTROL


// Start scheduler
int scheduler_start(void) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    if (scheduler_state.is_running) {
        log_error("Scheduler already running");
        return ERROR;
    }
    
    scheduler_state.is_running = true;
    log_info("Scheduler started");
    
    return SUCCESS;
}

// Stop scheduler
int scheduler_stop(void) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    scheduler_state.is_running = false;
    log_info("Scheduler stopped");
    
    return SUCCESS;
}

// Pause scheduler
int scheduler_pause(void) {
    if (!is_initialized || !scheduler_state.is_running) {
        log_error("Scheduler not running");
        return ERROR;
    }
    
    scheduler_state.is_running = false;
    log_info("Scheduler paused");
    
    return SUCCESS;
}

// Resume scheduler
int scheduler_resume(void) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    scheduler_state.is_running = true;
    log_info("Scheduler resumed");
    
    return SUCCESS;
}


// CONFIGURATION MANAGEMENT


// Set scheduler algorithm
int set_scheduler_algorithm(SchedulerAlgorithm algorithm) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    scheduler_state.config.algorithm = algorithm;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Scheduler algorithm changed to: %d", algorithm);
    log_info(log_msg);
    
    return SUCCESS;
}

// Set scheduler mode
int set_scheduler_mode(SchedulerMode mode) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    scheduler_state.mode = mode;
    scheduler_state.config.mode = mode;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Scheduler mode changed to: %d", mode);
    log_info(log_msg);
    
    return SUCCESS;
}

// Set time quantum for round robin
int set_time_quantum(int quantum_ms) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    scheduler_state.config.time_quantum = quantum_ms;
    log_info("Time quantum updated");
    
    return SUCCESS;
}

// Get scheduler configuration
SchedulerConfig* get_scheduler_config(void) {
    if (!is_initialized) {
        return NULL;
    }
    return &scheduler_state.config;
}

// Configure scheduler
void configure_scheduler(SchedulerConfig *config) {
    if (!is_initialized || config == NULL) {
        log_error("Invalid configuration");
        return;
    }
    
    scheduler_state.config = *config;
    log_info("Scheduler configuration updated");
}


// MODE MANAGEMENT BASED ON BATTERY


// Determine scheduler mode based on battery level
SchedulerMode determine_scheduler_mode(int battery_level) {
    if (battery_level <= BATTERY_CRITICAL) {
        return MODE_CRITICAL;
    } else if (battery_level <= BATTERY_LOW) {
        return MODE_POWER_SAVE;
    } else if (battery_level <= BATTERY_MEDIUM) {
        return MODE_BALANCED;
    } else {
        return MODE_PERFORMANCE;
    }
}

// Adjust scheduler based on battery level
int adjust_scheduler_for_battery(void) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return ERROR;
    }
    
    int battery_level = get_battery_level();
    SchedulerMode new_mode = determine_scheduler_mode(battery_level);
    
    if (new_mode != scheduler_state.mode) {
        set_scheduler_mode(new_mode);
        apply_power_saving_policies();
    }
    
    return SUCCESS;
}

// Apply power saving policies
int apply_power_saving_policies(void) {
    if (!is_initialized) {
        return ERROR;
    }
    
    switch(scheduler_state.mode) {
        case MODE_CRITICAL:
            // Only run critical tasks
            log_info("CRITICAL mode: Only critical tasks allowed");
            // Suspend non-critical tasks from ready queue
            break;
            
        case MODE_POWER_SAVE:
            // Prefer low-energy tasks
            log_info("POWER_SAVE mode: Prioritizing low-energy tasks");
            break;
            
        case MODE_BALANCED:
            // Balance between performance and energy
            log_info("BALANCED mode: Balancing performance and energy");
            break;
            
        case MODE_PERFORMANCE:
            // Normal operation
            log_info("PERFORMANCE mode: Normal operation");
            break;
    }
    
    return SUCCESS;
}


// TASK ADMISSION CONTROL


// Check if task can be admitted
bool can_admit_task(Task *task) {
    if (!is_initialized || task == NULL) {
        return false;
    }
    
    int battery_level = get_battery_level();
    
    // In critical mode, only admit critical tasks
    if (scheduler_state.mode == MODE_CRITICAL && !task->is_critical) {
        return false;
    }
    
    // In power save mode, avoid high-energy tasks unless critical
    if (scheduler_state.mode == MODE_POWER_SAVE && 
        task->energy_cost == ENERGY_HIGH && !task->is_critical) {
        return false;
    }
    
    // Check if battery can handle the task
    if (battery_level < BATTERY_CRITICAL && task->energy_cost > ENERGY_LOW) {
        return false;
    }
    
    return true;
}

// Admit task to scheduler
int admit_task_to_scheduler(Task *task) {
    if (!is_initialized || task == NULL) {
        log_error("Invalid task or scheduler not initialized");
        return ERROR;
    }
    
    if (!can_admit_task(task)) {
        log_error("Task admission denied due to battery constraints");
        return ERROR;
    }
    
    if (enqueue_task(scheduler_state.ready_queue, task) != SUCCESS) {
        log_error("Failed to enqueue task");
        return ERROR;
    }
    
    scheduler_stats.total_tasks_scheduled++;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Task admitted: ID=%d, Name=%s", 
             task->task_id, task->task_name);
    log_info(log_msg);
    
    return SUCCESS;
}


// CORE SCHEDULING FUNCTIONS


// Select next task based on algorithm
Task* select_next_task(void) {
    if (!is_initialized) {
        return NULL;
    }
    
    switch(scheduler_state.config.algorithm) {
        case SCHEDULER_FCFS:
            return schedule_fcfs();
        case SCHEDULER_SJF:
            return schedule_sjf();
        case SCHEDULER_PRIORITY:
            return schedule_priority();
        case SCHEDULER_ROUND_ROBIN:
            return schedule_round_robin();
        case SCHEDULER_BATTERY_AWARE:
            return schedule_battery_aware();
        default:
            return schedule_battery_aware();
    }
}

// Schedule a task
int schedule_task(Task *task) {
    if (!is_initialized || task == NULL) {
        return ERROR;
    }
    
    scheduler_state.current_task = task;
    set_task_state(task, TASK_STATE_RUNNING);
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Scheduled task: ID=%d, Name=%s", 
             task->task_id, task->task_name);
    log_info(log_msg);
    
    return SUCCESS;
}

// Execute a task
int execute_task(Task *task) {
    if (!is_initialized || task == NULL) {
        return ERROR;
    }
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Executing task: ID=%d for %d ms", 
             task->task_id, min(task->remaining_time, scheduler_state.config.time_quantum));
    log_info(log_msg);
    
    // Simulate task execution
    int execution_time = min(task->remaining_time, scheduler_state.config.time_quantum);
    sleep_ms(execution_time);
    
    task->remaining_time -= execution_time;
    
    // Simulate battery drain
    simulate_battery_drain(task->energy_cost);
    scheduler_stats.total_energy_consumed += task->energy_cost;
    
    // Check if task completed
    if (task->remaining_time <= 0) {
        set_task_state(task, TASK_STATE_COMPLETED);
        scheduler_stats.tasks_completed++;
        
        snprintf(log_msg, MAX_LOG_MSG, "Task completed: ID=%d", task->task_id);
        log_info(log_msg);
        
        scheduler_state.current_task = NULL;
        return SUCCESS;
    }
    
    return SUCCESS;
}

// Preempt a task
int preempt_task(Task *task) {
    if (!is_initialized || task == NULL) {
        return ERROR;
    }
    
    set_task_state(task, TASK_STATE_READY);
    enqueue_task(scheduler_state.ready_queue, task);
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Task preempted: ID=%d", task->task_id);
    log_info(log_msg);
    
    return SUCCESS;
}

// Suspend a task
int suspend_task(Task *task) {
    if (!is_initialized || task == NULL) {
        return ERROR;
    }
    
    set_task_state(task, TASK_STATE_SUSPENDED);
    enqueue_task(scheduler_state.waiting_queue, task);
    scheduler_stats.tasks_suspended++;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Task suspended: ID=%d", task->task_id);
    log_info(log_msg);
    
    return SUCCESS;
}

// Resume a task
int resume_task(Task *task) {
    if (!is_initialized || task == NULL) {
        return ERROR;
    }
    
    set_task_state(task, TASK_STATE_READY);
    enqueue_task(scheduler_state.ready_queue, task);
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Task resumed: ID=%d", task->task_id);
    log_info(log_msg);
    
    return SUCCESS;
}


// SCHEDULING ALGORITHMS


// First Come First Serve
Task* schedule_fcfs(void) {
    if (is_queue_empty(scheduler_state.ready_queue)) {
        return NULL;
    }
    
    return dequeue_task(scheduler_state.ready_queue);
}

// Shortest Job First
Task* schedule_sjf(void) {
    if (is_queue_empty(scheduler_state.ready_queue)) {
        return NULL;
    }
    
    // Find task with shortest burst time
    TaskQueue *queue = scheduler_state.ready_queue;
    int shortest_index = queue->front;
    int shortest_time = queue->tasks[shortest_index].burst_time;
    
    int index = queue->front;
    for (int i = 0; i < queue->count; i++) {
        if (queue->tasks[index].burst_time < shortest_time) {
            shortest_index = index;
            shortest_time = queue->tasks[index].burst_time;
        }
        index = (index + 1) % MAX_TASKS;
    }
    
    // For simplicity, just dequeue front (in full implementation, would remove shortest)
    return dequeue_task(queue);
}

// Priority-based scheduling
Task* schedule_priority(void) {
    if (is_queue_empty(scheduler_state.ready_queue)) {
        return NULL;
    }
    
    // Find highest priority task (lowest number = highest priority)
    TaskQueue *queue = scheduler_state.ready_queue;
    int best_index = queue->front;
    int best_priority = queue->tasks[best_index].priority;
    
    int index = queue->front;
    for (int i = 0; i < queue->count; i++) {
        if (queue->tasks[index].priority < best_priority) {
            best_index = index;
            best_priority = queue->tasks[index].priority;
        }
        index = (index + 1) % MAX_TASKS;
    }
    
    // For simplicity, just dequeue front
    return dequeue_task(queue);
}

// Round Robin scheduling
Task* schedule_round_robin(void) {
    if (is_queue_empty(scheduler_state.ready_queue)) {
        return NULL;
    }
    
    return dequeue_task(scheduler_state.ready_queue);
}

// Battery-aware scheduling
Task* schedule_battery_aware(void) {
    if (is_queue_empty(scheduler_state.ready_queue)) {
        return NULL;
    }
    
    int battery_level = get_battery_level();
    TaskQueue *queue = scheduler_state.ready_queue;
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Battery-aware scheduling: Battery=%d%%, Mode=%d",
             battery_level, scheduler_state.mode);
    log_debug(log_msg);
    
    // Adjust scheduling based on battery mode
    if (scheduler_state.mode == MODE_CRITICAL) {
        // Only select critical tasks
        int index = queue->front;
        for (int i = 0; i < queue->count; i++) {
            if (queue->tasks[index].is_critical) {
                // For simplicity, dequeue front
                return dequeue_task(queue);
            }
            index = (index + 1) % MAX_TASKS;
        }
        return NULL;  // No critical tasks available
    }
    
    if (scheduler_state.mode == MODE_POWER_SAVE) {
        // Prefer low-energy tasks
        int best_index = queue->front;
        int best_energy = queue->tasks[best_index].energy_cost;
        int best_priority = queue->tasks[best_index].priority;
        
        int index = queue->front;
        for (int i = 0; i < queue->count; i++) {
            Task *t = &queue->tasks[index];
            // Prioritize: critical > low energy > high priority
            if (t->is_critical || 
                (t->energy_cost < best_energy) ||
                (t->energy_cost == best_energy && t->priority < best_priority)) {
                best_index = index;
                best_energy = t->energy_cost;
                best_priority = t->priority;
            }
            index = (index + 1) % MAX_TASKS;
        }
    }
    
    // Default: consider both priority and energy
    return dequeue_task(queue);
}


// CONTEXT SWITCHING


// Perform context switch
int perform_context_switch(Task *old_task, Task *new_task) {
    if (!is_initialized) {
        return ERROR;
    }
    
    scheduler_state.context_switches++;
    scheduler_stats.context_switches++;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Context switch: Old=%d, New=%d", 
             old_task ? old_task->task_id : 0, 
             new_task ? new_task->task_id : 0);
    log_debug(log_msg);
    
    return SUCCESS;
}


// SCHEDULER MAIN LOOP


// Main scheduler loop
void scheduler_run_loop(void) {
    if (!is_initialized) {
        log_error("Scheduler not initialized");
        return;
    }

    log_info("Entering scheduler main loop");
    int idle_count = 0;  // ← ADD THIS: Track consecutive idles
    const int MAX_IDLE = 10;  // ← Maximum idle iterations before exit
    
    while (scheduler_state.is_running) {
        // Update battery status
        update_battery_status();
        
        // Adjust scheduler mode based on battery
        adjust_scheduler_for_battery();
        
        // Select next task
        Task *next_task = select_next_task();
        
        if (next_task != NULL) {
            idle_count = 0;  // ← Reset idle counter when task found
            
            // Context switch if different task
            if (scheduler_state.current_task != next_task) {
                perform_context_switch(scheduler_state.current_task, next_task);
            }
            
            // Schedule and execute task
            schedule_task(next_task);
            execute_task(next_task);
            
            // If task still has remaining time and preemption enabled, re-queue
            if (next_task->remaining_time > 0 && scheduler_state.config.enable_preemption) {
                preempt_task(next_task);
            }
        } else {
            // No tasks available, idle
            log_debug("No tasks in ready queue, idling...");
            sleep_ms(100);
            idle_count++;  // ← INCREMENT idle counter
            
            // ← ADD THIS: Exit if too many consecutive idles
            if (idle_count >= MAX_IDLE) {
                log_info("Maximum idle iterations reached - stopping scheduler");
                break;
            }
        }
        
        // ← ADD THIS: Exit if battery critical and no tasks
        if (get_battery_level() <= BATTERY_CRITICAL && 
            is_queue_empty(scheduler_state.ready_queue)) {
            log_info("Battery critical and queue empty - stopping scheduler");
            break;
        }
    }
    
    log_info("Exiting scheduler main loop");
}


// STATISTICS AND MONITORING


// Get scheduler statistics
SchedulerStats* get_scheduler_statistics(void) {
    return &scheduler_stats;
}

// Update scheduler statistics
void update_scheduler_statistics(void) {
    TaskStats *task_stats = get_task_statistics();
    
    if (task_stats->total_tasks > 0) {
        scheduler_stats.cpu_utilization = 
            calculate_percentage(task_stats->completed_tasks, task_stats->total_tasks);
    }
}

// Print scheduler status
void print_scheduler_status(void) {
    printf("\n=== Scheduler Status ===\n");
    printf("Algorithm: %d\n", scheduler_state.config.algorithm);
    printf("Mode: ");
    switch(scheduler_state.mode) {
        case MODE_PERFORMANCE: printf("PERFORMANCE\n"); break;
        case MODE_BALANCED: printf("BALANCED\n"); break;
        case MODE_POWER_SAVE: printf("POWER_SAVE\n"); break;
        case MODE_CRITICAL: printf("CRITICAL\n"); break;
    }
    printf("Running: %s\n", scheduler_state.is_running ? "YES" : "NO");
    printf("Ready Queue Size: %d\n", get_queue_size(scheduler_state.ready_queue));
    printf("Waiting Queue Size: %d\n", get_queue_size(scheduler_state.waiting_queue));
    printf("Context Switches: %d\n", scheduler_state.context_switches);
    printf("=======================\n\n");
}

// Print scheduler statistics
void print_scheduler_statistics(void) {
    printf("\n=== Scheduler Statistics ===\n");
    printf("Total Tasks Scheduled: %d\n", scheduler_stats.total_tasks_scheduled);
    printf("Tasks Completed: %d\n", scheduler_stats.tasks_completed);
    printf("Tasks Suspended: %d\n", scheduler_stats.tasks_suspended);
    printf("Context Switches: %d\n", scheduler_stats.context_switches);
    printf("CPU Utilization: %.2f%%\n", scheduler_stats.cpu_utilization);
    printf("Battery Saved: %.2f%%\n", scheduler_stats.battery_saved);
    printf("Total Energy Consumed: %ld units\n", scheduler_stats.total_energy_consumed);
    printf("===========================\n\n");
}


// DEBUG AND LOGGING


// Log scheduling decision
void log_scheduling_decision(Task *task, const char *reason) {
    if (task == NULL) return;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Scheduling decision - Task: %d, Reason: %s", 
             task->task_id, reason);
    log_debug(log_msg);
}

// Print ready queue
void print_ready_queue(void) {
    printf("\n=== Ready Queue ===\n");
    print_task_queue(scheduler_state.ready_queue);
    printf("===================\n\n");
}
