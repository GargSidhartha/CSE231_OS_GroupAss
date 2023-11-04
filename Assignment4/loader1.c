#include "loader.h"
#include <signal.h>

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
Elf32_Phdr * segmntHdr; 
int fd;
char* virtual_memC;
void* virtual_mem;
int total_page_faults = 0;
int total_page_allocations = 0;

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  free(ehdr);
  free(phdr);
  free(segmntHdr);
  close(fd);
}

/*
  To check if file is 32-bit 
*/
int is32Bit(Elf32_Ehdr * ehdr) {
  if(ehdr->e_ident[4] == 1){
    return 0;
  }
  else if (ehdr->e_ident[4] == 0) {
    printf("Invalid class ELF file\n");
    exit(1);
    return 1;
  }
  else if (ehdr->e_ident[4] == 2) {
    printf("ELF file is 64-bit\n");
    exit(1);
    return 1;
  }
}

/*
  to check the type
*/
int isExec(Elf32_Ehdr * ehdr){
  if(ehdr->e_type == 2){
    return 0;
  }
  else {
    printf("Not an ELF executable\n");
    exit(1);
    return 1;
  }
}

/*
  to check ELF ident
*/
int checkELFIdent(Elf32_Ehdr * ehdr) {
  if (ehdr->e_ident[0] == 0x7f && ehdr->e_ident[1] == 'E' && ehdr->e_ident[2] == 'L' && ehdr->e_ident[3] == 'F') {
    return 0;
  }
  else {
    printf("Invalid Magic Number");
    exit(1);
    return 1;
  }
}


// void seg_handler(int signum, siginfo_t *info, void *context) {
//     printf("Segmentation fault at address %p\n", info->si_addr);
//     int temp = (int)(info -> si_addr) / 0x1000;
    
//     virtual_mem = mmap((void*)(temp*0x1000), 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//     if (virtual_mem == MAP_FAILED) {
//       perror("mmap failed");
//       exit(1);
//     }
//     printf("Virtual memory address %p\n", virtual_mem);

//     lseek(fd,)
//     return;
    
// }


// void seg_handler(int signum, siginfo_t *info, void *context) {
//     printf("Segmentation fault at address %p\n", info->si_addr);

//     // Calculate the aligned start address of the page
//     void *page_start = (void*)((uintptr_t)info->si_addr & ~((uintptr_t)0xFFF)); // Assuming page size is 4KB
//     for(int i = 0; i < ehdr -> e_phnum; i++){
//       phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
//       read(fd,phdr,sizeof(Elf32_Phdr));
//       if (phdr -> p_vaddr <= (int)info -> si_addr && phdr -> p_vaddr + phdr->p_offset > (int)info -> si_addr){
//         segmntHdr = phdr;
//       }
//     }
//     // Allocate a new page using mmap
//     virtual_mem = mmap(page_start, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

//     if (virtual_mem == MAP_FAILED) {
//         perror("mmap failed");
//         exit(1);
//     }

//     // Update statistics
//     total_page_faults++;
//     total_page_allocations++;

//     // Read the content of the segment from the ELF file
//     lseek(fd, segmntHdr->p_offset, SEEK_SET);
//     read(fd, (char*)virtual_mem, segmntHdr->p_memsz);


//     // Copy the content of the segment into the newly allocated memory
    

//     // Resume execution
//     return;
// }

void seg_handler(int signum, siginfo_t *info, void *context) {
    printf("Segmentation fault at address %p\n", info->si_addr);

    // Calculate the aligned start address of the page
    void *page_start = (void*)((uintptr_t)info->si_addr & ~((uintptr_t)0xFFF)); // Assuming page size is 4KB

    // Find the corresponding program header
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_vaddr <= (uintptr_t)info->si_addr &&
            (phdr[i].p_vaddr + phdr[i].p_memsz) > (uintptr_t)info->si_addr) {
            segmntHdr = &phdr[i];
            break;
        }
    }

    // Allocate a new page using mmap
    virtual_mem = mmap(page_start, segmntHdr->p_memsz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (virtual_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // Update statistics
    total_page_faults++;
    total_page_allocations++;

    // Read the content of the segment from the ELF file
    lseek(fd, segmntHdr->p_offset, SEEK_SET);
    ssize_t bytes_read = read(fd, virtual_mem, segmntHdr->p_memsz);

    if (bytes_read == -1) {
        perror("Error reading segment from ELF file");
        exit(1);
    }

    // Resume execution
    return;
}




/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = seg_handler;

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("Error setting up signal handler");
        exit(1);
      }
      
  fd = open(exe, O_RDONLY);

  // 1. Load entire binary content into the memory from the ELF file.
  ehdr = malloc(sizeof(Elf32_Ehdr));
  read(fd,ehdr,sizeof(Elf32_Ehdr));

  //checks
  int flag = 0;
  flag = flag + is32Bit(ehdr) + isExec(ehdr) + checkELFIdent(ehdr);
  if(flag != 0){
    printf("Errors detected, exiting\n");
    exit(1);
    return;
  }
    


  int address = ehdr->e_entry;
  int (*_start)() = (int (*)())(virtual_mem + address);
  

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  printf("page faults: %d\n",total_page_faults);
  int result = _start();
  
  // 6. Call the "_start" method and print the value returned from the "_start"

  printf("User _start return value = %d\n",result);
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv[1]);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
