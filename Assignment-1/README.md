# Assignment 01 - Group 50 - SimpleLoader: An ELF Loader from Scratch
Instructor: Vivek Kumar </br>
Submitted by:
- Saksham Singh - 2022434
- Sidhartha Garg - 2022499


## Summary
A simpler ELF loader for loading an ELF 32-bit executable in plain-C without any library APIs available for manipulating ELF files.

## Directories and How to Run
### For with-bonus
- Navigate to `starter/with-bonus` and in terminal execute `make all`
- Navigate to `starter/with-bonus/launcher` and in terminal run `./launcher ../test/fib`
#### Diretory Structure
- `/bin` contains the shared library `lib_simpleloader.so` which is compiled from `/loader/loader.c`
- `/launcher` contains the C code for the launcher implementation. `launch.c` is compiled and linked with the above stated shared library to output `launcher` - an ELF 32-bit executable
- `/test` contains the ELF executable to be loaded and launched by the SimpleLoader

### For without-bonus
- Navigate to `starter/without-bonus` and in terminal execute `make all`
- in terminal run `./loader fib`

### Checks
Added checks to confirm the following
- ELF identification number verification
- Checks to match the Architecture of the ELF file and the type of the ELF file (whether it is executable or relocatable file)
- Also checks to verify the OS of the machine
- Minor checks regarding file reading operations

## Citations
- https://wiki.osdev.org/ELF_Tutorial
- https://man7.org/linux/man-pages/man5/elf.5.html
- https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
- https://man7.org/linux/man-pages/man2/mmap.2.html
- https://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html

## Contribution
- All the work was collectively and equally done by both the participants Sidhartha Garg and Saksham Singh
- The code programming and compilation for loader and launcher was collectively discussed and executed.
- Code compilation was carried on Sidhartha Garg's Intel based machine
- Checks were handled by Saksham Singh, and MakeFiles were made by Sidhartha Garg
- GitHub repo : https://github.com/SakshxmSingh/CSE231_OS_GroupAss