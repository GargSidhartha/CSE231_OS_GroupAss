#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
Elf32_Phdr * segmntHdr; 
int fd;

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
 * Load and run the ELF executable file
 */
void load_and_run_elf(char* exe) {
  fd = open(exe, O_RDONLY);

  // 1. Load entire binary content into the memory from the ELF file.
  ehdr = malloc(sizeof(Elf32_Ehdr));
  read(fd,ehdr,sizeof(Elf32_Ehdr));


  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c
  for(int i = 0; i < ehdr -> e_phnum; i++){
    phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
    read(fd,phdr,sizeof(Elf32_Phdr));
    if (phdr -> p_type == 1 && phdr -> p_vaddr <= ehdr -> e_entry && phdr -> p_vaddr + 0x1000 > ehdr -> e_entry){
      segmntHdr = phdr;
    }
  }

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  void * virtual_mem = mmap(NULL, segmntHdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  char * virtual_memC = virtual_mem;
 
  lseek(fd, segmntHdr->p_offset, SEEK_SET);
  read(fd, virtual_memC, segmntHdr->p_memsz);

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step
  int address = ehdr->e_entry - segmntHdr->p_vaddr;

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.
  // int _start() = virtual_memC[address];
  int (*_start)() = (int (*)())(virtual_mem + address);

  // 6. Call the "_start" method and print the value returned from the "_start"
  int result = _start();
  printf("User _start return value = %d\n",result);
  
}