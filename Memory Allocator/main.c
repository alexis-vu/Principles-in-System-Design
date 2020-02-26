// Alexis Vu 16426804 Lynette Nguyen 33259665

// ---------- TO DO -----------
// If no more blocks can be allocated, should not print
// Implement splitting
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_SIZE 127
#define MAX_ARGS 3

// global variables
// initialize the heap with 0 --> all addr
int heap[HEAP_SIZE] = {0};

// keeps track of the block size and whether or not it is allocated
// negative number --> block WAS allocated, but is free
// 0 --> free block
// positive number --> block IS allocated
int block_list[HEAP_SIZE] = {0};

// function declarations
char** parse();
int findFreeBlock(int);
void split(int, int);
void my_malloc(int);
int charToHex(char);
void my_free(int);
void coalesce();
void blocklist();
void printmem(int, int);
void writemem(int, char*);

int main() {
  // begin memory allocator
  bool end = false;       // flag for ending shell
  char* *args = NULL;     // user input

  block_list[0] = -127;

  while (!end) {
    // print shell prompt and take in user input
    printf(">");
    args = parse();

    // continue to take next command if no user input
    if (args == NULL) {
      continue;
    }
    // check if quit command, set flag to true to end system
    else if (strcmp(args[0], "quit") == 0) {
      end = true;
    }
    // otherwise, check for system commands
    else {
      // malloc command
      if (strcmp(args[0], "malloc") == 0) {
        if (args[1] == NULL) {
          continue;
        }
        else {
          my_malloc(atoi(args[1]));
        }
      }
      // free command
      else if (strcmp(args[0], "free") == 0) {
        if (args[1] == NULL) {
          continue;
        }
        else {
          my_free(atoi(args[1]));
        }
      }
      // blocklist command
      else if (strcmp(args[0], "blocklist") == 0) {
        blocklist();
      }
      // writemem command
      else if (strcmp(args[0], "writemem") == 0) {
        if (args[1] == NULL || args[2] == NULL) {
          continue;
        }
        else {
          writemem(atoi(args[1]), args[2]);
        }
      }
      // printmem command
      else if (strcmp(args[0], "printmem") == 0) {
        if (args[1] == NULL || args[2] == NULL) {
          continue;
        }
        else {
          printmem(atoi(args[1]), atoi(args[2]));
        }
      }
    }
  }

  // free user input pointer
  printf("\n");
  free(args);
  return 0;
}

// parse user input
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

// Loops through the heap to find the first addr (index) that is 0 and
// has enough space to allocate, use best fit algorithm
// return -1 if no free blocks
int findFreeBlock(int size) {
  int addr = 0;
  int minFound = HEAP_SIZE;
  int remainingByte = HEAP_SIZE;
  int memorySize;

  while(addr < HEAP_SIZE){
    // check if positive -> allocated, if negative -> not allocated
    memorySize = abs(block_list[addr]);
    // If memory size is bigger than size and block is free
    if (memorySize > size && block_list[addr] < 0) {
      // If this left over byte is less than what is stored
      if (memorySize - size < remainingByte){
        remainingByte = memorySize - size;
        minFound = addr;
      }
    }
    addr += memorySize;
  }

  if (minFound == HEAP_SIZE) {
    return -1;
  }

  return minFound;
}

// split blocks during allocation
void split(int start, int size) {
  int originalSize = abs(block_list[start]);
  block_list[start] = size + 1;
  block_list[start + (size + 1)] = (originalSize - (size + 1)) * -1;

  heap[start + (size + 1)] = ((originalSize - (size + 1)) << 1);
}

void my_malloc(int size) {
  // addr of the header for the block
  int addr = findFreeBlock(size);
  // set the header with the size of the block (intial size + 1 for the header)
  // if find free block returns -1, nothing could be allocated
  if (addr != -1) {
    heap[addr] = ((size + 1) << 1) + 1;
    int memorySize = abs(block_list[addr]);
    if (size != memorySize - 1) {
      split(addr, size);
    }
    else{
      block_list[addr] = size + 1;
    }
    printf("%d\n", addr + 1);
  }
}

// free block at given address
void my_free(int addr){
  if (block_list[addr - 1] > 0) {
    int size = heap[addr - 1];
    block_list[addr - 1] *= -1;
    coalesce(addr-1);
  }
}

// coalesce blocks during free
void coalesce(int start) {
  int next = start + abs(block_list[start]);

  if (block_list[next] < 0) {
    block_list[start] = abs(block_list[start]) + abs(block_list[next]);
    block_list[start] *= -1;
    block_list[next] = 0;

    heap[start] = ((abs(block_list[start]) + abs(block_list[next])) << 1);
  }
}

// print out blocklist
void blocklist(){
  int addr = 0;
  while (addr < HEAP_SIZE){
    int size = abs(block_list[addr]);

    if(block_list[addr] > 0){
      printf("%d, %d, allocated\n", addr+1, size-1);
    }
    else{
      printf("%d, %d, free\n", addr+1, size-1);
    }
    addr += size;
  }
}

// print memory at address in hexadecimal
void printmem(int start, int numAddr) {
  int i;
  for (i = 0; i < numAddr; ++i) {
    printf("%02x ", heap[start + i]);
  }
  printf("\n");
}

// write memory at address
void writemem(int addr, char* args){
  int size = strlen(args);
  int i;

  for (i = 0; i < size; ++i) {
    heap[addr + i] = (int) args[i];
  }
}
