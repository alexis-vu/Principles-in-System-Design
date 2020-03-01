/*
  Alexis Vu
  ID: 16426804
  UCINetID: alexilv1
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_LINE 200
#define MAX_CHAR 256

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
  char* size;
  char* content;
} Message;

// Function Declarations
void echo(int);         // communication protocol client -> server

int main(int argc, char** argv) {
  int client_fd,                  // client socket descriptor
      port;                       // port number

  char* host;                     // host name
  char* ip;                       // host IP address

  InternetSocketAddr serverAddr;  // server socket address
  HostEntry* server;              // host server

  // host and port specified in command line
  host = argv[1];
  port = atoi(argv[2]);

  // find hose using hose name
  server = gethostbyname(host);

  if (server == NULL) {
    perror("Host could not be found...\n");
    exit(0);
  }

  // resolve and get host IP address
  ip = inet_ntoa(*((struct in_addr*) server->h_addr_list[0]));

  // create client socket
  client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (client_fd < 0) {
    perror("Socket creation failed...\n");
    exit(0);
  }

  // flush out values and store new information
  bzero(&serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(ip);
  serverAddr.sin_port = htons(port);

  if ((connect(client_fd, (SocketAddr*) &serverAddr, sizeof(serverAddr))) != 0) {
    perror("Connection with server failed...\n");
    exit(0);
  }

  // communication protocol
  echo(client_fd);

  // close descriptor
  close(client_fd);
}

void echo(int client_fd) {
  char buffer[MAX_CHAR];

  // infinitely commmunicate with host
  while(1) {
    bzero(buffer, sizeof(buffer));
    printf(">");

    fgets(buffer, MAX_CHAR, stdin);
    write(client_fd, buffer, strlen(buffer));
    read(client_fd, &buffer, MAX_CHAR);
    printf("%s\n", buffer);
  }
}
