#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NUM_ADDR_VM 32
#define NUM_ADDR_MM 16
#define NUM_PAGE_VM 8
#define NUM_PAGE_MM 4
#define NUM_PTE 8

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
} MainMemoryEntry;

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

void showPTable(PageTableEntry pt[]) {
  int i;
  for (i = 0; i < NUM_PTE; ++i) {
    printf("%d:%d:%d:%d\n", pt[i].entry, pt[i].valid, pt[i].dirty, pt[i].pageNum);
  }
}

void printMain(MainMemoryEntry mainMem[]) {
  int i;
  printf("MAIN MEMORY:\n");
  for (i = 0; i < NUM_ADDR_MM; ++i) {
    printf("%i:%i:%i\n", mainMem[i].page, mainMem[i].address, mainMem[i].data);
  }
}

int main() {
  PageTableEntry pt[NUM_PTE];             // page table
  MainMemoryEntry mainMem[NUM_ADDR_MM];   // main memory
  bool end = false;                       // flag for ending shell
  char* *args = NULL;                     // user input
  int i;                                  // counter variable

  // initialize page table
  for (i = 0; i < NUM_PTE; ++i) {
    pt[i].entry = i;
    pt[i].valid = 0;
    pt[i].dirty = 0;
    pt[i].pageNum = i;
  }

  // initialize main memory
  int page = 0;
  for (i = 0; i < NUM_ADDR_MM; ++i) {
    mainMem[i].page = i % 2 == 1 ? page++ : page;
    mainMem[i].address = i;
    mainMem[i].data = -1;
  }

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

      }
      else if (strcmp(args[0], "write") == 0) {

      }
      else if (strcmp(args[0], "showmain") == 0) {

      }
      else if (strcmp(args[0], "showdisk") == 0) {
        
      }
      else if (strcmp(args[0], "showptable") == 0) {
        showPTable(pt);
      }
    }
  }

  return 0;
}