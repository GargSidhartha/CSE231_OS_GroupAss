# Assignment 05 - Group 50 - SimpleMultithreader: Using Multithreading with Ease 
Instructor: Vivek Kumar </br>
Submitted by:
- Saksham Singh - 2022434
- Sidhartha Garg - 2022499

## Summary

The SimpleMultithreader is a header-only C++ implementation designed to simplify the usage of Pthread-based parallelism. It provides main method, parallel_for, to facilitate parallel execution of loop bodies using C++11 lambda expressions.

## How to Run
- in the root directory, run `make all`
- in the root directory, run `./vector {Number of Threads}` to execute the executable `vector` in the root directory and print the output by threading, or run `./matrix {Number of Threads}` to execute the executable `matrix` in the root directory and print the output
- run `make clean` to remove all the executables and object files

## Functionality

The ELF loader performs the following functions:

- **parallel_for Method for the vector file**
  - Accepts a lambda function representing the loop body.
  - Runs the loop body in parallel using a specified number of Pthreads (numThreads).
  - Dynamically creates Pthreads without utilizing task/thread pools.
  - Ensures the exact number of threads specified by the programmer, including the main thread.

- **parallel_for Method for the matrix file**
  - Designed for parallelizing two-dimensional for-loops.
  - Utilizes C++11 lambda expressions for the loop body.
  - Creates a new set of Pthreads for each call, terminating upon the scope's completion.


## Contribution
- All the work was collectively and equally done by both the participants Sidhartha Garg and Saksham Singh
- The code programming and compilation for Multithreader was collectively discussed and executed.
- Code compilation was carried on Sidhartha Garg's Intel based machine
- GitHub repo : https://github.com/SakshxmSingh/CSE231_OS_GroupAss
