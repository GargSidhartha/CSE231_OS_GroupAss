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

  printf("e_entry = %x\n",ehdr -> e_entry);

  for(int i = 0; i < 16; i++){
    printf("%x ", ehdr->e_ident[i]);
  }
  printf("\n");

  // 2. Iterate through the PHDR table and find the section of PT_LOAD 
  //    type that contains the address of the entrypoint method in fib.c
  for(int i = 0; i < ehdr -> e_phnum; i++){
    phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
    read(fd,phdr,sizeof(Elf32_Phdr));
    if (phdr -> p_type == 1 && phdr -> p_vaddr <= ehdr -> e_entry && phdr -> p_vaddr + 0x1000 > ehdr -> e_entry){
      segmntHdr = phdr;
    }
  }
  // second program header check 
  printf("%x\n", phdr->p_align);

  // 3. Allocate memory of the size "p_memsz" using mmap function 
  //    and then copy the segment content
  void * virtual_mem = mmap(NULL, segmntHdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  printf("%p\n", virtual_mem);

  char * virtual_memC = virtual_mem;
  // printf("%s\n", virtual_memC);
 
  lseek(fd, segmntHdr->p_offset, SEEK_SET);
  read(fd, virtual_memC, segmntHdr->p_memsz);

  for(int i = 0; i < segmntHdr->p_memsz; i++){
    printf("%x ", virtual_memC[i]);
  }
  printf("\n");

  // 4. Navigate to the entrypoint address into the segment loaded in the memory in above step

  int address = ehdr->e_entry - segmntHdr->p_vaddr;
  printf("%x", address);

  // 5. Typecast the address to that of function pointer matching "_start" method in fib.c.

  int _start() = virtual_memC[address];


  // 6. Call the "_start" method and print the value returned from the "_start"

  int result = _start();
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
