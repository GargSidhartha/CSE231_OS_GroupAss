#include "loader.h"
#include <signal.h>

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
Elf32_Phdr * segmntHdr; 
int fd;
char* virtual_memC;
void* virtual_mem;

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


void seg_handler(int signum, siginfo_t *info, void *context) {
    printf("Segmentation fault at address %p\n", info->si_addr);
    int temp = (int)(info -> si_addr) / 0x1000;
    
    virtual_mem = mmap((void*)(temp*0x1000), 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (virtual_mem == MAP_FAILED) {
      perror("mmap failed");
      exit(1);
    }
    printf("Virtual memory address %p\n", virtual_mem);
    return;
    
}

int total_page_faults = 0;
int total_page_allocations = 0;



/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {
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

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c
  for(int i = 0; i < ehdr -> e_phnum; i++){
    phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
    read(fd,phdr,sizeof(Elf32_Phdr));
    if (phdr -> p_type == 1 && phdr -> p_vaddr <= ehdr -> e_entry && phdr -> p_vaddr + phdr->p_offset > ehdr -> e_entry){
      segmntHdr = phdr;
    }
  }

  int address = ehdr->e_entry - segmntHdr->p_vaddr;


  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  

  int (*_start)() = (int (*)())(virtual_mem + address);
  
  struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = seg_handler;

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("Error setting up signal handler");
        exit(1);
      }


  

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
