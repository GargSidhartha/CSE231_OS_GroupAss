# Assignment 04 - Group 50 - SimpleSmartLoader: An Upgraded Simple Loader in C
Instructor: Vivek Kumar </br>
Submitted by:
- Saksham Singh - 2022434
- Sidhartha Garg - 2022499

## Summary

This C code is a simple ELF (Executable and Linkable Format) loader that employs the use of lazy allocation and loads segments one page at a time. The loader dynamically allocates memory for the ELF header, program header, and segment header. It uses a signal handler to catch segmentation faults and dynamically allocates memory for the faulting page, simulating a basic memory paging mechanism.

## How to Run
- in the root directory, run `make all`
- in the root directory, run `./smartloader fib` to execute the ELF executable `fib` in the root directory and print the output, or run `./smartloader sum` to execute the ELF executable `sum` in the root directory and print the output
- run `make clean` to remove all the executables and object files

## Functionality

The ELF loader performs the following functions:

- **Segmentation Fault Handling:**
  - Sets up a signal handler for segmentation faults (SIGSEGV).
  - When a segmentation fault occurs, the handler identifies the faulting address and attempts to allocate a new page of memory for the corresponding segment.

- **Memory Allocation and Deallocation:**
  - Dynamically allocates memory for the ELF header, program header, and segment header.
  - Allocates memory for faulting pages in the event of a segmentation fault.
  - Releases allocated memory during cleanup.

- **Execution:**
  - Reads the ELF header to obtain the entry point address.
  - Transfers control to the entry point to execute the ELF executable.
  - Prints the return value of the `_start` function.

- **Statistics:**
  - Displays the number of page faults that occurred during execution.
  - Provides information on the total number of page allocations.
  - Reports the total internal fragmentation in bytes and kilobytes.
  
- **File Validation:**
  - Checks if the provided file is a valid ELF file by verifying the ELF magic number.
  - Ensures that the ELF file is a 32-bit executable.


## Checks

The code includes checks for the following conditions:

- **Valid ELF file:** Verifies the ELF magic number and class.
- **Executable ELF:** Ensures that the provided ELF file is an executable.
- **ELF Ident:** Checks for a valid ELF magic number.
- **File opening:** Checks for errors when opening the ELF file.
- **Memory allocation:** Validates successful dynamic memory allocation.
- **File reading:** Verifies successful reading of the ELF header.
- **Completeness of ELF header:** Ensures that the entire ELF header is read.
- **Seek operation:** Checks for errors during the seek operation.
- **Segmentation fault:** Handles segmentation faults during execution.

## Citations
- https://wiki.osdev.org/ELF_Tutorial
- https://man7.org/linux/man-pages/man5/elf.5.html
- https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
- https://man7.org/linux/man-pages/man2/mmap.2.html
- https://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html
- https://man7.org/linux/man-pages/man7/signal.7.html

## Contribution
- All the work was collectively and equally done by both the participants Sidhartha Garg and Saksham Singh
- The code programming and compilation for SmartLoader was collectively discussed and executed.
- Code compilation was carried on Sidhartha Garg's Intel based machine
- GitHub repo : https://github.com/SakshxmSingh/CSE231_OS_GroupAss
- 