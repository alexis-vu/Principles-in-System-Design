#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int MAX_TOKENS = 25;
int threads = 0;
int bThreads = 0;
pid_t p[40];
pid_t bgp[40];

// PARSE COMMAND LINE
char** parse() {
  // read in input line from stdin
  char* line = NULL;
  size_t size = 0;
  getline(&line, &size, stdin);

  // check if empty line entered, return NULL to flag continue in main
  if (strcmp(line, "\n") == 0) {
    return NULL;
  }

  // setup for parsing individualized tokens
  char** args = (char**) malloc(MAX_TOKENS * sizeof(char*));
  char* arg = NULL;
  int i = 0;

  // check for successful allocation
  if (args == NULL) {
    perror("Allocation Error: parse()");
    exit(1);
  }

  // beging tokenizing, skip all whitespace
  arg = strtok(line, " \n\t\r\a");
  while (arg != NULL) {
    // if # input tokens exceeds, reallocate
    if (i >= MAX_TOKENS) {
      MAX_TOKENS *= 2;
      args = (char**) realloc(args, MAX_TOKENS * sizeof(char*));

      // check for successful reallocation
      if (args == NULL) {
        perror("Reallocation error: parse()");
        exit(1);
      }
    }

    // otherwise continue tokenizing
    args[i] = arg;
    ++i;
    arg = strtok(NULL, " \n\t\r\a");
  }

  // mark last position as nullptr
  args[i] = NULL;

  return args;
}

// BACKGROUND PROCESS CHECK
bool background(char* *args) {
  int i = 0;

  // parse args to search for background process flag &
  for (i = 0; args[i] != NULL; ++i) {
    if (strcmp(args[i], "&") == 0) {
      args[i] = NULL;
      return true;
    }
  }

  return false;
}

bool redirectInput(char* *args, char* *inFile) {
  int i = 0,
  j = 0;

  // parse args to search for input redirection
  for (i = 0; args[i] != NULL; ++i) {
    // check if file provided, otherwise redirection impossible
    if ((strcmp(args[i], "<") == 0) && (args[i + 1] != NULL)) {
      // file name should be subsequent arg
      *inFile = args[i + 1];

      // since found and flagged, shift args in case of multiple ops
      for (j = i; args[j - 1] != NULL; ++j) {
        args[j] = args[j + 2];
      }

      return true;
    }
  }

  return false;
}

bool redirectOutput(char* *args, char* *outFile) {
  int i = 0,
  j = 0;

  // parse args to search for output redirection
  for (i = 0; args[i] != NULL; ++i) {
    // check if file provided, otherwise redir impossible
    if ((strcmp(args[i], ">") == 0) && (args[i + 1] != NULL)) {
      // file name should be subsequent arg
      *outFile = args[i + 1];

      // shift args
      for (j = i; args[j - 1] != NULL; ++j) {
        args[j] = args[j + 2];
      }

      return true;
    }
  }

  return false;
}

// CLEANUP/REAP
void cleanup() {
  int status,
  i = 0;

  // kill all processes
  for (i = 0; i < threads; ++i) {
    kill(p[i], 2);
    waitpid(p[i], &status, 0);
  }

  for (i = 0; i < bThreads; ++i) {
    kill(bgp[i], 2);
    waitpid(bgp[i], &status, 0);
  }
}

// EXECUTE COMMANDS
void execute(char* *args, bool background, bool in, char* inFile, bool out, char* outFile) {
  pid_t pid;
  int status;

  // create new process to execute in
  pid = fork();
  if (pid == 0) { // child process -> execute commands
    // input redirection flagged, overwrite stdin in FDT
    if (in) {
      if ((freopen(inFile, "r", stdin)) == NULL) {
        perror("Error: execute() -> redirect input");
      }
    }

    // output redirection flagged, overwrite stdout in FDT
    if (out) {
      if ((freopen(outFile, "w", stdout)) == NULL) {
        perror("Error: execute() -> redirect output");
      }
    }

    // execute command
    if (execvp(args[0], args) == -1) {
      perror("Error: execute() -> invalid command");
    }
    // execute shouldn't return, if it does sumn WRONG
    exit(EXIT_FAILURE);
  }
  else { // parent process
    if (background) {
      // add background process to p and add to thread count
      bgp[bThreads] = pid;
      ++bThreads;
    }
    else {
      // keep process id and wait for process to finish
      p[threads] = pid;
      ++threads;
      waitpid(pid, &status, 0);
    }
  }
}


int main(int argc, char **argv) {
  // set up flags
  bool done = false;
  bool bg = false;
  bool in = false;
  bool out = false;

  // set up args array and inFile/outFile names
  char* *args = NULL;
  char* inFile = NULL;
  char* outFile = NULL;

  while (!done) {
    // print prompt and get command line
    printf("prompt> ");
    args = parse();

    // if command = quit, quit
    if (args == NULL) {
      continue;
    }
    else if (strcmp(args[0], "quit") == 0) {
      done = true;
      cleanup();
    }
    else {
      // check for conditions: redirection or background process
      bg = background(args);
      in = redirectInput(args, &inFile);
      out = redirectOutput(args, &outFile);

      // execute arguments depending on flags
      execute(args, bg, in, inFile, out, outFile);
      wait(NULL);
    }
  }
  free(args);
  return 0;
}
