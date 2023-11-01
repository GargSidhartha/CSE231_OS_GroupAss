#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
Elf32_Phdr * segmntHdr; 
int fd;
int pageFault = 0;
int pageAllocated = 0;
void * virtual_mem;


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

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {
  fd = open(exe, O_RDONLY);

  // 1. Load entire binary content into the memory from the ELF file.
  ehdr = malloc(sizeof(Elf32_Ehdr));
  read(fd,ehdr,sizeof(Elf32_Ehdr));

  // printf("e_entry = %x\n",ehdr -> e_entry);

  // for(int i = 0; i < 16; i++){
  //   printf("%x ", ehdr->e_ident[i]);
  // }
  // printf("\n");

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

  // for(int i = 0; i < ehdr -> e_phnum; i++){
  //   phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
  //   read(fd,phdr,sizeof(Elf32_Phdr));
  //   if (phdr -> p_type == 1 && phdr -> p_vaddr <= ehdr -> e_entry && phdr -> p_vaddr + 0x1000 > ehdr -> e_entry){   // check the 1000 one 
  //     segmntHdr = phdr;
  //   }
  // }

  // second program header check 
  // printf("%x\n", phdr->p_align);

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  
  // printf("%p\n", virtual_mem);
  
  char * virtual_memC = virtual_mem;
  // printf("%s\n", virtual_memC);
 
  lseek(fd, segmntHdr->p_offset, SEEK_SET);
  read(fd, virtual_memC, segmntHdr->p_memsz);

  // for(int i = 0; i < segmntHdr->p_memsz; i++){
  //   printf("%x ", virtual_memC[i]);
  // }
  // printf("\n");

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step

  int address = ehdr->e_entry - segmntHdr->p_vaddr;
  // printf("%x", address);

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  // int _start() = virtual_memC[address];

  int (*_start)() = (int (*)())(virtual_mem + address);

  // 6. Call the "_start" method and print the value returned from the "_start"

  int result = _start();
  printf("User _start return value = %d\n",result);
}

// Signal handler for segmentation faults
void seg_handler(int signum, siginfo_t *info, void *context) {
    printf("Segmentation fault at address %p\n", info->si_addr);

    // Allocate memory using mmap
    virtual_mem = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);


    // Load and copy content into allocated memory
    // ...

    // Update program counter or instruction pointer
    // ...

    // Update statistics
    pageFault++;
    pageAllocated++;
    // internal_fragmentation += /* calculate internal fragmentation based on your logic */;

    // Resume execution
    return;
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = seg_handler;

  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
      perror("Error setting up signal handler");
      exit(1);
  }
  // 1. carry out necessary checks on the input ELF file
  // 2. passing it to the loader for carrying out the loading/execution
  load_and_run_elf(argv[1]);
  // 3. invoke the cleanup routine inside the loader  
  loader_cleanup();
  return 0;
}
