#include <stdlib.h>
#include <stdio.h>
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

void parse(char, char*);

int main() {
  PageTableEntry pt[NUM_PTE];
  MainMemoryEntry mainMem[NUM_ADDR_MM];
  int i;

  // set up page table
  for (i = 0; i < NUM_PTE; ++i) {
    pt[i].entry = i;
    pt[i].valid = 0;
    pt[i].dirty = 0;
    pt[i].pageNum = i;
  }

  // for (i = 0; i < NUM_PTE; ++i) {
  //   printf("%d:%d:%d:%d\n", pt[i].entry, pt[i].valid, pt[i].dirty, pt[i].pageNum);
  // }

  // set up main memory
  int page = 0;
  for (i = 0; i < NUM_ADDR_MM; ++i) {
    mainMem[i].page = i % 2 == 1 ? page++ : page;
    mainMem[i].address = i;
    mainMem[i].data = -1;
  }

  // for (i = 0; i < NUM_ADDR_MM; ++i) {
  //   printf("%i:%i:%i\n", mainMem[i].page, mainMem[i].address, mainMem[i].data);

  bool end = false;
  char input[MAX_CHAR];
  char** line[MAX_ARGS];
  while (!end) {
    // print prompt
    printf("> ");
    fgets(input, sizeof(input) - 1, stdin);
    line[0] = strtok(input, " \n");
    for (i = 1; i < MAX_ARGS; ++i) {
      line[i] = strtok(NULL, " \n");
    }

    for (i = 0; i < sizeof(line)/sizeof(line[0]); ++i) {
      printf("%s ", line[i]);
    }
    printf("\n");
  }
  return 0;
}
