#include "loader.h"
#include <signal.h>
#include <stdbool.h>

#define PAGESIZE 0x1000
#define MAXFAULTS 1000

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
Elf32_Phdr * segmntHdr; 
int fd;
char* virtual_memC;
void* virtual_mem;
int faultnum = 0;
int allocnum = 0;
int internalFrag = 0;
void * pageArray[MAXFAULTS];

/*
 * release memory and other cleanups
 */
void loader_cleanup() {
  free(ehdr);
  free(phdr);
  free(segmntHdr);
  for(int i = 0; i < faultnum; i ++){
    munmap(pageArray[i],PAGESIZE);
  }
  
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
    if (lseek(fd, ehdr->e_phoff, SEEK_SET) == (off_t)-1) {
      printf("error in seeking");
      _exit(EXIT_FAILURE);
    }

    printf("Segmentation fault at address %p\n", info->si_addr);

    bool flag = false;

    for(int i = 0; i < ehdr -> e_phnum; i++){
      phdr = malloc(ehdr -> e_phnum * sizeof(Elf32_Phdr));
      if(!phdr){
        printf("Failed to allocate");
        close(fd);
        exit(EXIT_FAILURE);
      }
      ssize_t buffer = read(fd,phdr,sizeof(Elf32_Phdr));
      if (buffer == -1){
        printf("failed to read");
        close(fd);
        exit(EXIT_FAILURE);
      }
      
      if (phdr -> p_vaddr <= (int)info -> si_addr && phdr -> p_vaddr + phdr->p_memsz >= (int)info -> si_addr){
        segmntHdr = phdr;
        printf("Segment start address: %0x\n",segmntHdr -> p_vaddr);
        flag = true;

      }
    }
    
    if (flag == true){
      int numpages = segmntHdr -> p_memsz / PAGESIZE + 1;
      for(int i = 0; i < numpages; i++){
        int page_start = segmntHdr -> p_vaddr + i*PAGESIZE;
        int page_end = page_start + PAGESIZE;

        if (i == numpages - 1){
          internalFrag = internalFrag + (PAGESIZE - (segmntHdr -> p_memsz) % 0x1000);
        }

        if ((int)info -> si_addr >= page_start && (int) info -> si_addr < page_end){
          virtual_mem = mmap((void*)page_start, PAGESIZE , PROT_READ | PROT_WRITE | PROT_EXEC , MAP_PRIVATE, fd, segmntHdr -> p_offset);
          pageArray[faultnum] = virtual_mem;

          printf("Allocating a page at address: %0x\n",(int)virtual_mem);
          if (virtual_mem == MAP_FAILED) {
                perror("mmap failed");
                exit(1);
            }
          faultnum++;
          allocnum++;
          break;
        }
      }
    }
    else{
      printf("Segment not found\n");
    }

    
    
    printf("\n");
    // Resume execution
}

void load_and_run_elf(char* exe) {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = seg_handler;

  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("Error setting up signal handler");
        exit(1);
      }
      
  fd = open(exe, O_RDONLY);
  if (fd == -1){
      perror("Error opening in file");
    }

  ehdr = malloc(sizeof(Elf32_Ehdr));
  if (!ehdr) {
      perror("Failed to allocate memory for ELF header");
      
      close(fd);
      exit(EXIT_FAILURE);
  }
  ssize_t buffer = read(fd, ehdr, sizeof(Elf32_Ehdr));
  if (buffer == -1) {
      perror("Failed to read ELF header");
      free(ehdr);
      close(fd);
      exit(EXIT_FAILURE);
  } else if (buffer != sizeof(Elf32_Ehdr)) {
      fprintf(stderr, "Incomplete ELF header\n");
      free(ehdr);
      close(fd);
      exit(EXIT_FAILURE);
  }


  int flag = 0;
  flag = flag + is32Bit(ehdr) + isExec(ehdr) + checkELFIdent(ehdr);
  if(flag != 0){
    printf("Errors detected, exiting\n");
    exit(1);
    return;
  }
    


  int address = ehdr->e_entry;
  int (*_start)() = (int (*)())(address);
  

  
  int result = _start();
 
  printf("\n--------------------------------------Summary---------------------------------------\n");
  printf("User _start return value = %d\n",result);
}

int main(int argc, char** argv) 
{
  if(argc != 2) {
    printf("Usage: %s <ELF Executable> \n",argv[0]);
    exit(1);
  }
  
  load_and_run_elf(argv[1]);

  printf("\n");
  printf("Page faults: %d\n",faultnum);
  printf("Page allocations: %d\n",allocnum);
  printf("Total internal Fragmentation: %d bytes and %f kB\n",internalFrag,((double)internalFrag) / 1024.0);

  loader_cleanup();
  return 0;
}
