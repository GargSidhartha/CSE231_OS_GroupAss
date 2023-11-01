# Assignment 03: SimpleShell with Integrated Process Scheduler
Instructor: Vivek Kumar </br>
Submitted by:
- Saksham Singh - 2022434
- Sidhartha Garg - 2022499

This SimpleShell implementation integrates a process scheduler employing round-robin scheduling, utilizing shared memory, semaphores, and signal interrupts for effective process management.

## Features

- **Shell Commands:** Execute various shell commands, and support for piping and background execution.
- **Job Submission:** Submit jobs with or without priority to the integrated scheduler.
- **History:** View and execute previously entered commands from the command history.
- **Scheduler:** Round-robin scheduler that manages processes in shared memory using semaphores.

## Usage

1. **Compile:** Compile the code using the provided Makefile.
    ```bash
    make scheduler
    ```
2. **Run Shell:**
    ```bash
    ./SimpleScheduler <number_of_cpus> <time_slice>
    ```
    - `<number_of_cpus>`: Number of CPUs for the scheduler.
    - `<time_slice>`: Time slice for the round-robin scheduler.

3. **Shell Commands:**
    - Enter standard shell commands.
    - Use `submit` to submit a job to the integrated scheduler.

4. **Scheduler:**
    - The scheduler runs in the background and manages processes based on priority and round-robin scheduling.

5. **Exit:**
    - Type `exit` to exit the shell.

## Integrated Scheduler: `schedule()`

The `schedule()` function is the core of the scheduler. This function manages processes using round-robin scheduling across multiple priority queues. Shared memory facilitates communication between processes, and semaphores ensure proper synchronization. The scheduler's loop continuously cycles through priority queues, selecting processes for execution. It uses signals (`SIGCONT` and `SIGSTOP`) to implement round-robin scheduling.

### Signal-Based Scheduling

- **SIGCONT (Continue):** The scheduler sends `SIGCONT` to the selected process, allowing it to continue its execution.
- **SIGSTOP (Pause):** After a time slice, the scheduler sends `SIGSTOP` to pause the currently running process, preventing it from consuming more CPU time.

## Job Submission: `submit_launch()`

The `submit_launch()` function handles job submission to the scheduler. It accommodates jobs with or without specified priorities. If no priority is specified, the job defaults to the first priority queue (`q1`). If a priority is specified, the job is added to the corresponding queue (`q1` to `q4`). Semaphores guard critical sections during queue manipulation.

## Process Forking and Signal Handling

The `launch()` function forks a new process to execute a command. The child process executes the specified command using `execvp`. The `signal_Handler()` function manages the `SIGINT` signal, allowing the shell to print the command history and exit gracefully in response to user interrupts.

## Shared Memory and Semaphores

The scheduler utilizes shared memory and semaphores for inter-process communication and synchronization. Each priority queue is protected by a semaphore, ensuring safe manipulation of process queues.

## Examples

```bash
./simpleshell 4 100
```

## Contribution
- All the work was collectively and equally done by both the participants Sidhartha Garg and Saksham Singh
- The code programming and compilation for the shell was collectively discussed and executed.
- Code compilation was carried on Sidhartha Garg's Intel based machine, but were known to work just as well on Saksham Singh's ARM based machine for some time.
- GitHub repo : https://github.com/SakshxmSingh/CSE231_OS_GroupAss