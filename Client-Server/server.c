/*
  Alexis Vu
  ID: 16426804
  UCINetID: alexilv1
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_LINE 200
#define MAX_CHAR 256
int NUM_FIELDS = 0;

typedef struct sockaddr SocketAddr;
/*
  struct sockaddr {
    uint16_t  sa_family;
    char      sa_data[14];
  };
*/

typedef struct sockaddr_in InternetSocketAddr;
/*
  struct sockaddr_in {
    uint16_t        sin_family;
    uint16_t        sin_port;
    struct in_addr  sin_addr;
    unsigned char   sin_zero[8];
  };
*/

typedef struct hostent HostEntry;
/*
  struct hostent {
    char*   h_name;
    char**  h_aliases;
    int     h_addrtype;
    int     h_length;
    char**  h_addr_list;
    char*   h_addr;
  };
*/

typedef struct {
  char** fields;
} Entry;

// Function Declarations
void echo(int, Entry*);             // communication from server -> client
Entry* populate(char*);             // create array of entries as database
char** tokenize(char*);             // tokenize csv row
char* search(Entry*, char*, char*); // search for client's request

int main(int argc, char** argv) {
  int server_fd,                  // server socket descriptor
      conn_fd,                    // connection socket descriptor
      port,                       // port number
      clientLen,                  // client address length
      opt = 1;                    // option value for socket

  InternetSocketAddr serverAddr;  // server socket address
  InternetSocketAddr clientAddr;  // client socket address

  char* fileName;                 // database filename (.csv)
  Entry* database;

  // port number specified in command line
  port = atoi(argv[2]);

  // create server socket
  server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (server_fd < 0) {
    perror("Socket creation failled...\n");
    exit(0);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("Failed to set socket option...\n");
    exit(0);
  }

  // fill in server server information
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(port);

  // bind socket to server
  if ((bind(server_fd, (SocketAddr*) &serverAddr, sizeof(serverAddr))) != 0) {
    perror("Socket bind failed...\n");
    exit(0);
  }

  // set socket to listening
  if ((listen(server_fd, 3)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  }

  // set length of client address
  clientLen = sizeof(clientAddr);

  // accept incoming connection requests
  conn_fd = accept(server_fd, (SocketAddr*) &clientAddr, (socklen_t*) &clientLen);
  if (conn_fd < 0) {
    perror("Server accept failed...\n");
    exit(0);
  }

  // set up database for parsing after successful connection
  fileName = argv[1];
  database = populate(fileName);

  // begin communication protocol
  echo(conn_fd, database);

  // close when finished
  close(server_fd);
  return 0;
}

// Function Definitions
void echo(int conn_fd, Entry* database) {
  char buffer[MAX_CHAR];
  char* id = NULL;
  char* field = NULL;
  char* result = NULL;
  char* pos = NULL;

  // infinitely read client input and return server output
  while(1) {
    bzero(buffer, MAX_CHAR);
    read(conn_fd, buffer, MAX_CHAR);
    printf("%s", buffer);

    if ((strcmp(buffer, "quit\n")) == 0) {
      break;
    }

    id = strtok_r(buffer, " \n\t\r\v\f", &pos);
    field = strtok_r(NULL, " \n\t\r\v\f", &pos);

    result = search(database, id, field);

    write(conn_fd, result, MAX_CHAR);
  }

  free(id);
  free(field);
  free(result);

  id = NULL;
  field = NULL;
  result = NULL;
}

Entry* populate(char* file) {
  FILE* fp = fopen(file, "r");

  char* line = NULL;
  char* temp = NULL;
  char* token = NULL;
  char* pos = NULL;
  size_t size = 0;

  int row;

  // get first line,
  getline(&line, &size, fp);
  temp = (char*) malloc(sizeof(line));
  temp = strdup(line);

  token = strtok_r(line, ",\n", &pos);
  while (token != NULL) {
    ++NUM_FIELDS;
    token = strtok_r(NULL, ",\n", &pos);
  }

  Entry* database = (Entry*) malloc(sizeof(Entry) * MAX_LINE);
  database[0].fields = tokenize(temp);
  free(temp);

  row = 1;
  while ((getline(&line, &size, fp) != -1) && (row != MAX_LINE)) {
    database[row].fields = tokenize(line);
    ++row;
  }

  return database;
}

char** tokenize(char* line) {
  char** tokens = (char**) malloc(NUM_FIELDS * sizeof(char*));
  char* token = NULL;
  int i = 0;

  // get first arg, skip whitespace
  token = strtok(line, ",\n");
  while (token != NULL) {
    tokens[i] = strdup(token);
    ++i;
    token = strtok(NULL, ",\n");
  }

  return tokens;
}

char* search(Entry* database, char* id, char* field) {
  char* result = NULL;
  int row = 1;
  int col = 0;
  bool found = false;

  int resRow = 0;
  int resCol = 0;

  while (!found && col < NUM_FIELDS) {
    if (strcmp(database[row].fields[col], id) == 0) {
      resRow = row;
      found = true;
    }

    row = (row != MAX_LINE - 1) ? row + 1 : 1;
    col += (row == 1) ? 1 : 0;
  }

  if (!found) {
    result = strdup("unknown");
  }
  else {
    found = false;
    col = 0;

    while (!found && col != NUM_FIELDS) {
      if (strcmp(database[0].fields[col], field) == 0) {
        resCol = col;
        found = true;
      }
      else {
        ++col;
      }
    }

    if (!found) {
      result = strdup("unknown");
    }
    else {
      result = strdup(database[resRow].fields[resCol]);
    }
  }

  return result;
}
