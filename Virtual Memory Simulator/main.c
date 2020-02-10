#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NUM_ADDR_VM 32
#define NUM_PAGE_VM 8
#define NUM_PTE 8

#define NUM_ADDR_MM 16
#define NUM_PAGE_MM 4

#define NUM_ADDR_DISK 32
#define NUM_PAGE_DISK 8

#define MAX_ARGS 3
#define MAX_CHAR 50

typedef struct {
  int entry;
  int valid;
  int dirty;
  int pageNum;
} PageTableEntry;

typedef struct {
  int page;
  int address;
  int data;
} MemoryEntry;

// MEMORY TABLES
PageTableEntry pTable[NUM_PTE];     // page table
MemoryEntry mainMem[NUM_ADDR_MM];   // main memory
MemoryEntry disk[NUM_ADDR_DISK];    // disk memory

void initialize() {
  int i;
  // initialize page table
  for (i = 0; i < NUM_PTE; ++i) {
    pTable[i].entry = i;
    pTable[i].valid = 0;
    pTable[i].dirty = 0;
    pTable[i].pageNum = i;
  }

  // initialize main memory
  int page = 0;
  for (i = 0; i < NUM_ADDR_MM; ++i) {
    mainMem[i].page = i % 2 == 0 ? page : page++;
    mainMem[i].address = i;
    mainMem[i].data = -1;
  }

  // initialize disk memory
  page = 0;
  for (i = 0; i < NUM_ADDR_DISK; ++i) {
    disk[i].page = i % 4 == 0 ? page : page++;
    disk[i].address = i;
    disk[i].data = -1;
  }
}

char** parse() {
  char* line = NULL;
  size_t size = 0;
  getline(&line, &size, stdin);

  // if empty line, return null to skip and return prompt
  if (strcmp(line, "\n") == 0) {
    return NULL;
  }

  // otherwise, setup for tokenizing input line
  char** args = (char**) malloc(MAX_ARGS * sizeof(char*));
  char* arg = NULL;
  int i = 0;

  // get first arg, skip whitespace
  arg = strtok(line, " \n\t\r\a");
  while (arg != NULL) {
    args[i] = arg;
    ++i;
    arg = strtok(NULL, " \n\t\r\a");
  }

  return args;
}

void showPTable() {
  int i;
  for (i = 0; i < NUM_PTE; ++i) {
    printf("%d:%d:%d:%d\n", pTable[i].entry, pTable[i].valid, pTable[i].dirty, pTable[i].pageNum);
  }
}

void printMemory(MemoryEntry memory[], size_t size) {
  int i;
  printf("MEMORY:\n");
  for (i = 0; i < size; ++i) {
    printf("%i:%i:%i\n", memory[i].page, memory[i].address, memory[i].data);
  }
}

int main(int argc, char* argv[]) {
  bool end = false;     // flag for ending shell
  char* *args = NULL;   // user input
  int i;                // counter variable

  int vAddr;     // virtual address
  int vPage;     // virtual page
  int pAddr;     // physical address
  int pPage;     // physical page (main mem)
  int dPage;     // disk page
  int dAddr;     // disk address
  int offset;    // address offset from page start

  initialize();

  while (!end) {
    // print prompt
    printf("> ");
    args = parse();

    // continue if empty line, otherwise execute according to command
    if (args == NULL) {
      continue;
    }
    else if (strcmp(args[0], "quit") == 0) {
      end = true;
    }
    else {
      if (strcmp(args[0], "read") == 0) {
        if (args[1] == NULL) {
          continue;
        }
        else {
          vAddr = atoi(args[1]);
          vPage = vAddr >> 2;
          // printf("%d %d ", vAddr, vPage);
          pPage = pTable[vPage].pageNum;

          // printf("%d ", pPage);

          if (pTable[vPage].valid == 0) {  // on disk
            printf("A Page Fault Has Occurred\n");
          }
          else { // in main memory
            pPage = pTable[vPage].pageNum;

          }
        }

      }
      else if (strcmp(args[0], "write") == 0) {

      }
      else if (strcmp(args[0], "showmain") == 0) {
        if (args[1] == NULL) {
          continue;
        }
        else {
          pPage = atoi(args[1]);
          pAddr = pPage * 4;
          for (i = 0; i < 4; ++i) {
            printf("%d:%d\n", mainMem[pAddr + i].address, mainMem[pAddr + i].data);
          }
        }
      }
      else if (strcmp(args[0], "showdisk") == 0) {
        if (args[1] == NULL) {
          continue;
        }
        else {
          dPage = atoi(args[1]);
          dAddr = dPage * 4;
          for (i = 0; i < 4; ++i) {
            printf("%d:%d\n", disk[dAddr + i].address, disk[dAddr + i].data);
          }
        }
      }
      else if (strcmp(args[0], "showptable") == 0) {
        showPTable();
      }
    }
  }

  return 0;
}
