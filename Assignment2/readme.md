# Simple Shell Implementation Documentation

This documentation explains the structure and functionality of a simple shell implementation in C. The code allows users to execute shell commands and pipe commands.

## Table of Contents

- [Simple Shell Implementation Documentation](#simple-shell-implementation-documentation)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Code Structure](#code-structure)
  - [Functions](#functions)
    - [init\_shell()](#init_shell)
    - [launch()](#launch)
    - [cntrl\_cHandler()](#cntrl_chandler)
    - [execute()](#execute)
    - [execute\_pip()](#execute_pip)
  - [Main Function](#main-function)
  - [Usage](#usage)

## Introduction

This C code implements a basic shell with the following features:
- Command execution
- Handling Ctrl+C (SIGINT) to display information about executed commands
- Piping multiple commands
- Command history

## Code Structure

The code is structured as follows:

- It defines a `command_info` struct to store information about executed commands.
- The `init_shell()` function initializes the shell and displays a welcome message.
- The `launch()` function is responsible for creating child processes to execute commands.
- The `cntrl_cHandler()` function is a signal handler for Ctrl+C (SIGINT).
- The `execute()` function parses and executes single commands.
- The `execute_pip()` function handles the execution of piped commands.
- The `main()` function is the entry point of the shell.

## Functions

### init_shell()

This function initializes the shell and displays a welcome message. It also prints the username.

### launch()

The `launch()` function creates a child process to execute a command. It handles command execution, including redirection for piped commands. It records information about the executed command in the `command_info` struct.

### cntrl_cHandler()

This function is a signal handler for Ctrl+C (SIGINT). It displays information about previously executed commands, including their PIDs, arguments, start times, and execution times.

### execute()

The `execute()` function parses and executes single commands. It supports built-in commands such as "exit" and "history," as well as external commands. It records executed commands in the command history.

### execute_pip()

The `execute_pip()` function handles the execution of piped commands. It splits the input command into individual commands based on the "|" delimiter and executes them sequentially, passing output from one command as input to the next.

## Main Function

The `main()` function is the entry point of the shell. It repeatedly prompts the user for input, reads commands, and executes them. It also maintains a history of commands.

## Usage

1. Compile the code.
   ```shell
   gcc -o simple_shell your_code.c
