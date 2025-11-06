## Authors

Developed as an Operating Systems course project.

Group Members:
202351106 - Nishit Prajapati
202351109 -Prateek
202351097 - Parth


# Battery-Aware Task Scheduler

A power-efficient operating system task scheduler that dynamically adjusts scheduling decisions based on battery levels to maximize system runtime while maintaining critical task execution.

## Overview

This project implements a battery-aware scheduling algorithm that outperforms traditional schedulers (FCFS, SJF, Round Robin) by considering both task priority and energy consumption. The scheduler adapts its behavior based on current battery levels, ensuring critical tasks execute even when battery is low while deferring energy-intensive tasks.

## Features

- **Five Scheduling Algorithms**
  - Battery-Aware (custom algorithm)
  - First Come First Serve (FCFS)
  - Shortest Job First (SJF)
  - Priority-Based Scheduling
  - Round Robin (configurable time quantum)

- **Dynamic Battery Management**
  - Real-time battery monitoring
  - Four operational modes: PERFORMANCE, BALANCED, POWER_SAVE, CRITICAL
  - Automatic mode switching based on battery thresholds
  - Energy consumption simulation

- **Task Management**
  - Priority levels (HIGH, MEDIUM, LOW)
  - Energy cost classification (LOW, MEDIUM, HIGH)
  - Critical task designation
  - Deadline-based scheduling support
  - Task state management (READY, RUNNING, WAITING, COMPLETED, SUSPENDED)

- **Two Operation Modes**
  - Simulation Mode: Automated algorithm comparison
  - Interactive Mode: Manual task creation and scheduler control

## Project Structure
```bash
osproject/
├── include/                 # Header files
│   ├── scheduler.h         # Scheduler definitions and functions
│   ├── battery_monitor.h   # Battery management declarations
│   ├── task_manager.h      # Task structure and operations
│   └── utils.h            # Constants, macros, utilities
├── src/                   # Source files
│   ├── main.c            # Entry point and modes
│   ├── scheduler.c       # Scheduling algorithms implementation
│   ├── battery_monitor.c # Battery state and drain simulation
│   ├── task_manager.c    # Task lifecycle management
│   └── utils.c          # Logging, time, display utilities
├── test/                 # Unit tests
│   ├── test_scheduler.c
│   ├── test_battery_monitor.c
│   └── test_task_manager.c
├── examples/             # Example configurations and tasks
│   ├── example_tasks.c
│   └── example_config.cfg
├── output/              # Results and logs
│   ├── comparison_results.txt
│   └── scheduler.log
├── scripts/             # Utility scripts
│   ├── setup.sh        # Environment setup
│   └── clean.sh        # Clean build artifacts
└── Makefile            # Build configuration
```

## Building the Project

### Prerequisites

- GCC compiler (version 7.0 or higher)
- GNU Make
- Linux/Unix environment (tested on Ubuntu)

### Compilation
Note: Perform all the below commands in the terminal at root directory.
Build all components:
```bash
make clean
make all
```

Build specific targets:
```bash
make scheduler    # Main scheduler program
make tests        # Unit test programs
make examples     # Example programs
```

Clean build artifacts:
```bash
make clean
```

## Running the Project

### Simulation Mode (Automated Comparison)

Runs all four scheduling algorithms with identical task sets and generates a comparison report.

```bash
./bin/scheduler --simulate
```

Runs all four scheduling algorithms with identical task sets and generates comparison report. Output includes comparison table and results saved to output/comparison_results.txt with execution logs in logs/scheduler.log.

### Interactive Mode

```bash
./bin/scheduler 
```

Provides menu-driven interface with 11 options: Create Sample Tasks, Add Custom Task, Start Scheduler, Stop Scheduler, Show Battery Status, Show Scheduler Status, Show Task Statistics, Show Scheduler Statistics, Change Scheduler Algorithm, Simulate Battery Drain, Run Full Simulation, Exit.

## Algorithm Comparison Results

| Algorithm | Energy Consumed | Tasks Completed | Context Switches | Battery Remaining |
|-----------|----------------|-----------------|------------------|-------------------|
| Battery-Aware | 114 units | 7/8 | 53 | 0% |
| FCFS | 120 units | 8/8 | 55 | 0% |
| SJF | 120 units | 8/8 | 55 | 0% |
| Round Robin | 120 units | 8/8 | 55 | 0% |

Energy Savings: Battery-Aware scheduler saves 6 units (5%) compared to FCFS by intelligently skipping high-energy non-critical tasks when battery is low.

## Battery Modes and Behavior

| Mode | Battery Level | Behavior |
|------|---------------|----------|
| PERFORMANCE | >50% | All tasks execute normally |
| BALANCED | 25-50% | Optimizes energy efficiency |
| POWER_SAVE | 10-25% | Only high-priority and low/medium energy tasks |
| CRITICAL | <10% | Only critical low-energy tasks |

## Energy Consumption Model

Battery drain calculation: Battery Drain (%) = (Energy Cost × Burst Time) / 1000

Where:
- Energy Cost: 1 (LOW), 2 (MEDIUM), 3 (HIGH)
- Burst Time: Task execution time in milliseconds

Example:
- Task with HIGH energy (3) running for 50ms: 3 × 50 / 1000 = 0.15% drain
- Task with LOW energy (1) running for 30ms: 1 × 30 / 1000 = 0.03% drain

## Sample Task Set

1. System Monitor - HIGH priority, LOW energy, critical
2. File Sync - HIGH priority, MEDIUM energy
3. Log Writer - MEDIUM priority, LOW energy
4. Video Processing - LOW priority, HIGH energy
5. Network Sync - MEDIUM priority, MEDIUM energy
6. Emergency Backup - HIGH priority, HIGH energy, critical
7. Cache Cleanup - LOW priority, LOW energy
8. Data Analysis - MEDIUM priority, MEDIUM energy

## Testing
```bash
make test
./bin/test_battery_monitor
./bin/test_scheduler
./bin/test_task_manager
```


## Configuration

Battery thresholds in include/utils.h:

 #define BATTERY_CRITICAL 10
 #define BATTERY_LOW 25
 #define BATTERY_MEDIUM 50
 #define BATTERY_HIGH 75


Time quantum for Round Robin (default 100ms) configurable in interactive mode.

## Logging

Logs generated at logs/scheduler.log with INFO, DEBUG, ERROR levels including timestamps and execution traces.

## Key Algorithms

### Battery-Aware Scheduling Logic

```bash
f battery_level <= CRITICAL_THRESHOLD:
select only critical AND low-energy tasks
elif battery_level <= LOW_THRESHOLD:
prefer high-priority AND low/medium energy tasks
elif battery_level <= MEDIUM_THRESHOLD:
balance priority and energy efficiency
else:
normal priority-based scheduling
```

### Task Admission Control

Tasks are rejected if:
- Battery is critical and task is non-critical
- Battery is in POWER_SAVE mode and task has HIGH energy cost
- Insufficient battery to complete the task

## Performance Metrics

The scheduler tracks total tasks scheduled and completed, tasks suspended due to battery constraints, context switch count, CPU utilization percentage, total energy consumed, and battery savings compared to baseline.

## File Descriptions

**main.c**: Entry point, command-line argument parsing, simulation mode, interactive mode with 11 user options.

**scheduler.c**: All five scheduling algorithms (FCFS, SJF, Priority, Round Robin, Battery-Aware), battery-aware mode management, task admission control, context switching, main scheduler loop (650+ lines).

**battery_monitor.c**: Battery state management (level, voltage, temperature), battery drain simulation, mode determination (PERFORMANCE/BALANCED/POWER_SAVE/CRITICAL), discharge rates.

**task_manager.c**: Task creation, lifecycle management (READY, RUNNING, SUSPENDED, COMPLETED states), queue operations, task statistics.

**utils.c**: Logging system (INFO/DEBUG/ERROR levels), timestamp generation, display utilities, system helper functions.

**task_manager.h**: Task structure with ID, name, priority, energy cost, burst time, criticality, deadline. Queue management functions.

**scheduler.h**: SchedulerState with algorithm selection, ready/waiting queues, configuration parameters, task admission control logic.

**battery_monitor.h**: BatteryInfo structure with level, state (CHARGING/DISCHARGING/FULL), voltage, temperature, discharge rate. Battery threshold definitions.

**utils.h**: Constants (MAX_TASKS, MAX_LOG_MSG, BATTERY thresholds), macro definitions, function declarations.

**test_scheduler.c**: Unit tests for all five scheduling algorithms, context switch verification, mode determination, task admission logic.

**test_battery_monitor.c**: Tests battery initialization, drain simulation, mode switching, voltage calculation, charging simulation.

**test_task_manager.c**: Tests task creation, queue operations, state transitions, priority handling, energy cost assignment.

**example_tasks.c**: Pre-configured task definitions demonstrating various priority levels, energy costs, task types.

**example_config.cfg**: Sample configuration file with scheduler parameters, battery thresholds, default settings.

## Limitations and Future Work

Current limitations: Simulated battery model (not actual hardware integration), Fixed energy cost values (not dynamic measurement), Single-core scheduler simulation.

Potential improvements: Integration with actual battery management systems, Dynamic energy profiling of tasks, Multi-core scheduling support, Machine learning for energy prediction, Real-time deadline guarantees.

## Dependencies

Runtime dependencies: Standard C library (libc), POSIX threads (pthread) - future support. No external libraries required.


## License

This project is provided for educational purposes.

## Acknowledgments

Inspired by research in energy-aware scheduling and mobile operating systems power management techniques.

