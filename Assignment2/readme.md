# Assignment 02 - Group 50 - Shellvetica: A SimpleShell in C
Instructor: Vivek Kumar </br>
Submitted by:
- Saksham Singh - 2022434
- Sidhartha Garg - 2022499

This documentation explains the structure and functionality of a simple shell implementation in C. The code allows users to execute shell commands, pipe commands, and provides basic command history functionality.

## Table of Contents

- [Assignment 02 - Group 50 - Shellvetica: A SimpleShell in C](#assignment-02---group-50---shellvetica-a-simpleshell-in-c)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Usage](#usage)
  - [Code Structure](#code-structure)
  - [Functions](#functions)
    - [init\_shell()](#init_shell)
    - [launch()](#launch)
    - [cntrl\_cHandler()](#cntrl_chandler)
    - [execute()](#execute)
    - [execute\_pip()](#execute_pip)
    - [shInterpreter()](#shinterpreter)
  - [Main Function](#main-function)
  - [Contribution](#contribution)

## Introduction

This C code implements a basic shell with the following features:

- Command execution
- Handling Ctrl+C (SIGINT) to display information about executed commands
- Piping multiple commands
- Command history

## Usage

1. Compile and run the code:

   ```shell
   make run_shell

2. Welcome to the shell
   -  The shell will accept all the externally defined commands in `/usr/bin` with their appropriate flags and syntax
   -  The shell will also accept appropriate ELF executables and run them with with their said arguments
   -  The shell is also capable of running bash scripts compatible with the OS accordingly
   -  The shell can handle pipes (`|`) and ampersand (`&`) operations
   -  The start time and execution times are in the unit of clock ticks - which are dependent on the machine running the process, and the PIDs are corresponding to the machine's PID counter
   -  The following internal commands are handled:
      -  `exit` : exits the shell and gives data of all the executed processes
      -  `history` : prints the list of all the commands entered
   -  The following commands are known to not work because of not being present in `/usr/bin` and being internally coded, and were not implemented in this shell for the reason of being outside the scope of this assignment
      -  `cd`
  
3. `fib.c` can be compiled to `./fib` by
   ```shell
   make fib
  
4. Cleanup by
    ```shell
    make clean


## Code Structure

The code is structured as follows:

- It defines a `command_info` struct to store information about executed commands.
- The `init_shell()` function initializes the shell and displays a welcome message.
- The `launch()` function is responsible for creating child processes to execute commands. It handles command execution, including redirection for piped commands, and records information about the executed command in the `command_info` struct.
- The `cntrl_cHandler()` function is a signal handler for Ctrl+C (SIGINT). It displays information about previously executed commands, including their PIDs, arguments, start times, and execution times.
- The `execute()` function parses and executes single commands. It supports built-in commands such as "exit" and "history," as well as external commands. It also records executed commands in the command history.
- The `execute_pip()` function handles the execution of piped commands. It splits the input command into individual commands based on the "|" delimiter and executes them sequentially, passing the output from one command as input to the next.
- The `shInterpreter()` function reads and executes shell script files.

## Functions

### init_shell()

This function initializes the shell, displays a welcome message, and prints the username of the current user.

### launch()

The `launch()` function creates a child process to execute a command. It handles command execution, including redirection for piped commands. It records information about the executed command in the `command_info` struct.

### cntrl_cHandler()

This function is a signal handler for Ctrl+C (SIGINT). When Ctrl+C is pressed, it displays a list of previously executed commands, including their PIDs, arguments, start times, and execution times.

### execute()

The `execute()` function parses and executes single commands. It supports built-in commands such as "exit" and "history," as well as external commands. It records executed commands in the command history.

### execute_pip()

The `execute_pip()` function handles the execution of piped commands. It splits the input command into individual commands based on the "|" delimiter and executes them sequentially, passing the output from one command as input to the next.

### shInterpreter()

The `shInterpreter()` function reads and executes shell script files. It reads the contents of a shell script file line by line and executes each line as a command.

## Main Function

The `main()` function is the entry point of the shell. It repeatedly prompts the user for input, reads commands, and executes them. It also maintains a history of commands.

## Contribution
- All the work was collectively and equally done by both the participants Sidhartha Garg and Saksham Singh
- The code programming and compilation for the shell was collectively discussed and executed.
- Code compilation was carried on Sidhartha Garg's Intel based machine, but were known to work just as well on Saksham Singh's ARM based machine.
- Bonus one was handled by Sidhartha Garg, Bonus two by Saksham Singh
- GitHub repo : https://github.com/SakshxmSingh/CSE231_OS_GroupAss