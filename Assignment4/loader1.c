#include "loader.h"
#include <signal.h>
#include <stdbool.h>

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
//     lseek(fd,ehdr -> e_phoff, SEEK_SET);
//     printf("Segmentation fault at address %p\n", info->si_addr);

//     // Calculate the aligned start address of the page
//     // void *page_start = (void*)((uintptr_t)info->si_addr & ~((uintptr_t)0xFFF)); // Assuming page size is 4KB
//     void *page_start = (void*)(((int)info -> si_addr / 0x1000) * 0x1000);
//     // Find the corresponding program header
//     bool flag = false;

//     for(int i = 0; i < ehdr -> e_phnum; i++){
//       phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
//       read(fd,phdr,sizeof(Elf32_Phdr));
//       if (phdr -> p_vaddr <= (int)info -> si_addr && phdr -> p_vaddr + phdr->p_memsz >= (int)info -> si_addr){
//         segmntHdr = phdr;
//         printf("%0x",segmntHdr -> p_vaddr);
//         flag = true;
//       }
//     }
    
    
//     // Allocate a new page using mmap
//     if (flag == true){

//       virtual_mem = mmap((void*)segmntHdr -> p_vaddr, (segmntHdr->p_memsz/0x1000 + 1) * 0x1000 , PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE, fd, segmntHdr -> p_offset);
//       printf("\n%0x",(int)virtual_mem);
//       if (virtual_mem == MAP_FAILED) {
//         perror("mmap failed");
//         exit(1);
//     }

//     total_page_faults++;
//     total_page_allocations++;

//     }
//     else{
//       printf("Segment not found\n");
//     }
    
    

//     printf("exiting...\n");
//     // Resume execution
// }



void seg_handler(int signum, siginfo_t *info, void *context) {
    lseek(fd,ehdr -> e_phoff, SEEK_SET);
    printf("Segmentation fault at address %p\n", info->si_addr);

    // Calculate the aligned start address of the page
    // void *page_start = (void*)((uintptr_t)info->si_addr & ~((uintptr_t)0xFFF)); // Assuming page size is 4KB
    // void *page_start = (void*)(((int)info -> si_addr / 0x1000) * 0x1000);
    // Find the corresponding program header
    bool flag = false;

    for(int i = 0; i < ehdr -> e_phnum; i++){
      phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
      read(fd,phdr,sizeof(Elf32_Phdr));
      
      if (phdr -> p_vaddr <= (int)info -> si_addr && phdr -> p_vaddr + phdr->p_memsz >= (int)info -> si_addr){
        segmntHdr = phdr;
        printf("%0x",segmntHdr -> p_vaddr);
        flag = true;

      }
    }
    
    
    // Allocate a new page using mmap
    if (flag == true){
      int numpages = segmntHdr -> p_memsz / 0x1000 + 1;
      printf("page numbers: %d",numpages);
      for(int i = 0; i < numpages; i++){
        int page_start = segmntHdr -> p_vaddr + i*0x1000;
        int page_end = page_start + 0x1000;

        if ((int)info -> si_addr >= page_start && (int) info -> si_addr < page_end){
          virtual_mem = mmap((void*)page_start, 0x1000 , PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE, fd, segmntHdr -> p_offset);
          printf("\n%0x",(int)virtual_mem);
          if (virtual_mem == MAP_FAILED) {
                perror("mmap failed");
                exit(1);
            }
          total_page_faults++;
          total_page_allocations++;
          break;
        }
      }
    }
    else{
      printf("Segment not found\n");
    }
    lseek(fd,ehdr -> e_phoff, SEEK_SET);
    
    
    printf("exiting...\n");
    // Resume execution
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
  int (*_start)() = (int (*)())(address);
  

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  
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
  
  printf("page faults: %d\n",total_page_faults);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
