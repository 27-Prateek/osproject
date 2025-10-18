#include "../include/scheduler.h"
#include "../include/battery_monitor.h"
#include "../include/task_manager.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>


// EXAMPLE SCENARIOS


void example_basic_scheduling(void);
void example_battery_aware_scheduling(void);
void example_priority_scheduling(void);
void example_critical_tasks(void);
void example_power_save_mode(void);
void example_mixed_workload(void);


// MAIN FUNCTION


int main(int argc, char *argv[]) {
    printf("\n");
    printf("========================================\n");
    printf("   BATTERY-AWARE SCHEDULER EXAMPLES\n");
    printf("========================================\n\n");
    
    if (argc > 1) {
        int example = atoi(argv[1]);
        
        switch(example) {
            case 1:
                example_basic_scheduling();
                break;
            case 2:
                example_battery_aware_scheduling();
                break;
            case 3:
                example_priority_scheduling();
                break;
            case 4:
                example_critical_tasks();
                break;
            case 5:
                example_power_save_mode();
                break;
            case 6:
                example_mixed_workload();
                break;
            default:
                printf("Invalid example number. Choose 1-6.\n");
                break;
        }
    } else {
        // Run all examples
        printf("=== Running All Examples ===\n\n");
        
        example_basic_scheduling();
        printf("\n");
        
        example_battery_aware_scheduling();
        printf("\n");
        
        example_priority_scheduling();
        printf("\n");
        
        example_critical_tasks();
        printf("\n");
        
        example_power_save_mode();
        printf("\n");
        
        example_mixed_workload();
    }
    
    return EXIT_SUCCESS;
}


// EXAMPLE 1: BASIC SCHEDULING


void example_basic_scheduling(void) {
    printf("========================================\n");
    printf("EXAMPLE 1: Basic FCFS Scheduling\n");
    printf("========================================\n\n");
    
    // Initialize with FCFS algorithm
    scheduler_init(SCHEDULER_FCFS);
    
    printf("Creating tasks with different burst times...\n");
    
    // Create simple tasks
    Task *task1 = create_task("Task A", PRIORITY_MEDIUM, ENERGY_LOW, 200, false, 5000);
    Task *task2 = create_task("Task B", PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 6000);
    Task *task3 = create_task("Task C", PRIORITY_MEDIUM, ENERGY_LOW, 150, false, 4000);
    
    // Admit tasks
    admit_task_to_scheduler(task1);
    admit_task_to_scheduler(task2);
    admit_task_to_scheduler(task3);
    
    printf("\nInitial Battery Level: %d%%\n", get_battery_level());
    print_ready_queue();
    
    // Start scheduler
    printf("Starting FCFS scheduler...\n\n");
    scheduler_start();
    scheduler_run_loop();
    
    // Show results
    printf("\n--- Results ---\n");
    printf("Final Battery Level: %d%%\n", get_battery_level());
    print_task_statistics();
    print_scheduler_statistics();
    
    scheduler_cleanup();
}


// EXAMPLE 2: BATTERY-AWARE SCHEDULING


void example_battery_aware_scheduling(void) {
    printf("========================================\n");
    printf("EXAMPLE 2: Battery-Aware Scheduling\n");
    printf("========================================\n\n");
    
    // Initialize with battery-aware algorithm
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    printf("Creating tasks with different energy costs...\n");
    
    // Create tasks with varying energy costs
    Task *low_energy = create_task("Low Energy Task", PRIORITY_MEDIUM, ENERGY_LOW, 
                                   300, false, 8000);
    Task *medium_energy = create_task("Medium Energy Task", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                                      300, false, 8000);
    Task *high_energy = create_task("High Energy Task", PRIORITY_MEDIUM, ENERGY_HIGH, 
                                    300, false, 8000);
    
    // Admit tasks
    admit_task_to_scheduler(high_energy);
    admit_task_to_scheduler(low_energy);
    admit_task_to_scheduler(medium_energy);
    
    // Set battery to medium level to trigger battery-aware behavior
    BatteryInfo *battery = get_battery_info();
    battery->current_level = 45;
    
    printf("\nInitial Battery Level: %d%%\n", get_battery_level());
    printf("Scheduler will prioritize low-energy tasks...\n");
    print_ready_queue();
    
    // Start scheduler
    printf("\nStarting battery-aware scheduler...\n\n");
    scheduler_start();
    scheduler_run_loop();
    
    // Show results
    printf("\n--- Results ---\n");
    printf("Final Battery Level: %d%%\n", get_battery_level());
    printf("Notice: Low-energy tasks were prioritized\n");
    print_task_statistics();
    print_scheduler_statistics();
    
    scheduler_cleanup();
}


// EXAMPLE 3: PRIORITY SCHEDULING


void example_priority_scheduling(void) {
    printf("========================================\n");
    printf("EXAMPLE 3: Priority-Based Scheduling\n");
    printf("========================================\n\n");
    
    // Initialize with priority algorithm
    scheduler_init(SCHEDULER_PRIORITY);
    
    printf("Creating tasks with different priorities...\n");
    
    // Create tasks with different priorities
    Task *low_priority = create_task("Low Priority", PRIORITY_LOW, ENERGY_MEDIUM, 
                                     400, false, 10000);
    Task *high_priority = create_task("High Priority", PRIORITY_HIGH, ENERGY_MEDIUM, 
                                      400, false, 5000);
    Task *medium_priority = create_task("Medium Priority", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                                        400, false, 7000);
    
    // Admit tasks in random order
    admit_task_to_scheduler(low_priority);
    admit_task_to_scheduler(high_priority);
    admit_task_to_scheduler(medium_priority);
    
    printf("\nInitial Battery Level: %d%%\n", get_battery_level());
    printf("Tasks will execute in priority order (High -> Medium -> Low)...\n");
    print_ready_queue();
    
    // Start scheduler
    printf("\nStarting priority scheduler...\n\n");
    scheduler_start();
    scheduler_run_loop();
    
    // Show results
    printf("\n--- Results ---\n");
    printf("Final Battery Level: %d%%\n", get_battery_level());
    print_task_statistics();
    print_scheduler_statistics();
    
    scheduler_cleanup();
}


// EXAMPLE 4: CRITICAL TASKS


void example_critical_tasks(void) {
    printf("========================================\n");
    printf("EXAMPLE 4: Critical Task Handling\n");
    printf("========================================\n\n");
    
    // Initialize with battery-aware algorithm
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    printf("Creating critical and normal tasks...\n");
    
    // Create mix of critical and normal tasks
    Task *critical1 = create_task("Critical System Monitor", PRIORITY_HIGH, ENERGY_LOW, 
                                  200, true, 3000);
    Task *normal1 = create_task("Normal File Sync", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                                400, false, 8000);
    Task *critical2 = create_task("Critical Backup", PRIORITY_HIGH, ENERGY_MEDIUM, 
                                  300, true, 4000);
    Task *normal2 = create_task("Normal Analysis", PRIORITY_LOW, ENERGY_HIGH, 
                                500, false, 10000);
    
    // Admit tasks
    admit_task_to_scheduler(normal1);
    admit_task_to_scheduler(critical1);
    admit_task_to_scheduler(normal2);
    admit_task_to_scheduler(critical2);
    
    // Set battery to critical level
    BatteryInfo *battery = get_battery_info();
    battery->current_level = 8;
    
    printf("\nInitial Battery Level: %d%% (CRITICAL!)\n", get_battery_level());
    printf("Only critical tasks will be executed...\n");
    print_ready_queue();
    
    // Start scheduler
    printf("\nStarting scheduler in critical mode...\n\n");
    scheduler_start();
    scheduler_run_loop();
    
    // Show results
    printf("\n--- Results ---\n");
    printf("Final Battery Level: %d%%\n", get_battery_level());
    printf("Notice: Only critical tasks were executed\n");
    print_task_statistics();
    print_scheduler_statistics();
    
    scheduler_cleanup();
}


// EXAMPLE 5: POWER SAVE MODE


void example_power_save_mode(void) {
    printf("========================================\n");
    printf("EXAMPLE 5: Power Save Mode\n");
    printf("========================================\n\n");
    
    // Initialize with battery-aware algorithm
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    printf("Demonstrating adaptive behavior across battery levels...\n");
    
    // Create various tasks
    Task *task1 = create_task("Background Service", PRIORITY_LOW, ENERGY_LOW, 
                              250, false, 8000);
    Task *task2 = create_task("Network Sync", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                              350, false, 7000);
    Task *task3 = create_task("Data Processing", PRIORITY_HIGH, ENERGY_HIGH, 
                              450, false, 6000);
    
    // Start with medium battery
    BatteryInfo *battery = get_battery_info();
    battery->current_level = 22;
    
    printf("\nInitial Battery Level: %d%% (POWER SAVE MODE)\n", get_battery_level());
    printf("Scheduler will avoid high-energy tasks unless critical...\n");
    
    // Admit tasks
    admit_task_to_scheduler(task1);
    admit_task_to_scheduler(task2);
    
    // High energy task should be rejected or deprioritized
    if (!can_admit_task(task3)) {
        printf("High-energy task admission denied due to low battery!\n");
    } else {
        admit_task_to_scheduler(task3);
    }
    
    print_ready_queue();
    
    // Start scheduler
    printf("\nStarting scheduler in power save mode...\n\n");
    scheduler_start();
    scheduler_run_loop();
    
    // Show results
    printf("\n--- Results ---\n");
    printf("Final Battery Level: %d%%\n", get_battery_level());
    printf("Notice: Battery consumption was minimized\n");
    print_task_statistics();
    print_scheduler_statistics();
    
    scheduler_cleanup();
}


// EXAMPLE 6: MIXED WORKLOAD


void example_mixed_workload(void) {
    printf("========================================\n");
    printf("EXAMPLE 6: Mixed Workload Simulation\n");
    printf("========================================\n\n");
    
    // Initialize with battery-aware algorithm
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    printf("Simulating realistic mixed workload...\n");
    
    // Create diverse set of tasks
    Task *tasks[10];
    tasks[0] = create_task("System Monitor", PRIORITY_HIGH, ENERGY_LOW, 150, true, 3000);
    tasks[1] = create_task("Email Sync", PRIORITY_MEDIUM, ENERGY_MEDIUM, 400, false, 8000);
    tasks[2] = create_task("Photo Upload", PRIORITY_LOW, ENERGY_HIGH, 800, false, 15000);
    tasks[3] = create_task("Calendar Update", PRIORITY_MEDIUM, ENERGY_LOW, 200, false, 5000);
    tasks[4] = create_task("Video Encoding", PRIORITY_LOW, ENERGY_HIGH, 1000, false, 20000);
    tasks[5] = create_task("Security Scan", PRIORITY_HIGH, ENERGY_MEDIUM, 600, true, 10000);
    tasks[6] = create_task("Cache Cleanup", PRIORITY_LOW, ENERGY_LOW, 300, false, 12000);
    tasks[7] = create_task("Database Sync", PRIORITY_MEDIUM, ENERGY_MEDIUM, 500, false, 9000);
    tasks[8] = create_task("Backup Service", PRIORITY_HIGH, ENERGY_HIGH, 700, true, 12000);
    tasks[9] = create_task("Log Writer", PRIORITY_LOW, ENERGY_LOW, 100, false, 6000);
    
    // Set initial battery to 60%
    BatteryInfo *battery = get_battery_info();
    battery->current_level = 60;
    
    printf("\nInitial Battery Level: %d%%\n", get_battery_level());
    printf("Admitting 10 diverse tasks...\n");
    
    // Admit all tasks
    int admitted = 0;
    for (int i = 0; i < 10; i++) {
        if (admit_task_to_scheduler(tasks[i]) == SUCCESS) {
            admitted++;
        }
    }
    
    printf("Admitted %d tasks\n", admitted);
    print_ready_queue();
    
    // Start scheduler
    printf("\nStarting scheduler...\n\n");
    scheduler_start();
    scheduler_run_loop();
    
    // Show comprehensive results
    printf("\n--- Final Results ---\n");
    printf("Final Battery Level: %d%%\n", get_battery_level());
    printf("Battery Consumed: %d%%\n", 60 - get_battery_level());
    print_battery_status();
    print_task_statistics();
    print_scheduler_statistics();
    
    printf("\nKey Observations:\n");
    printf("- Critical tasks were prioritized\n");
    printf("- Energy-efficient scheduling preserved battery\n");
    printf("- Task completion balanced with power constraints\n");
    
    scheduler_cleanup();
}
