#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_CHAR 120
#define MAX_LINE 100

void adventure();
void loaddungeon();

typedef struct {
    int number;
    char* description;
    int north;
    int south;
    int east;
    int west;
} room;

room rooms[MAX_LINE];

int main(int argc, char *argv[]) {
    adventure();
    return 0;
}

// MAIN FUNCTION
void adventure() {
    bool end = false;
    bool dungeonLoaded = false;
    bool roomFound = false;
    char input[MAX_CHAR];
    char* token;
    int currentRoom = 0;
    
    while (!end) {
        // print prompt
        printf("$ ");
        
        // get user input
        fgets(input, sizeof(input) - 1, stdin);
        
        // tokenize - get first token
        token = strtok(input, " ");
        
        // reset roomFound to false
        roomFound = false;
        
        // load dungeon file if not yet loaded
        if (strcmp(token, "loaddungeon\n") == 0 || strcmp(token, "loaddungeon") == 0) {
            if (!dungeonLoaded) {
                // parse for file name
                token = strtok(NULL, " ");
                
                if (token != NULL) {
                    token[strcspn(token, "\n")] = '\0';
                    
                    // load dungeon file
                    loaddungeon(token);
                    dungeonLoaded = true;
                    
                    // after successful load, set current room to first room
                    // and print description
                    currentRoom = 0;
                    printf("%s\n", rooms[currentRoom].description);
                }
            }
            else {
                printf("Dungeon already loaded\n");
            }
        }
        else if (strcmp(token, "north\n") == 0) {
            if (rooms[currentRoom].north == -1) {
                printf("You can't go there.\n");
            }
            else {
                int i = 0;
                while (!roomFound) {
                    if (rooms[i].number == rooms[currentRoom].north) {
                        roomFound = true;
                    }
                    else {
                        ++i;
                    }
                }
                
                currentRoom = i;
                printf("%s\n", rooms[currentRoom].description);
            }
        }
        else if (strcmp(token, "south\n") == 0) {
            if (rooms[currentRoom].south == -1) {
                printf("You can't go there.\n");
            }
            else {
                int i = 0;
                while (!roomFound) {
                    if (rooms[i].number == rooms[currentRoom].south) {
                        roomFound = true;
                    }
                    else {
                        ++i;
                    }
                }
                
                currentRoom = i;
                printf("%s\n", rooms[currentRoom].description);
            }
        }
        else if (strcmp(token, "east\n") == 0) {
            if (rooms[currentRoom].east == -1) {
                printf("You can't go there.\n");
            }
            else {
                int i = 0;
                while (!roomFound) {
                    if (rooms[i].number == rooms[currentRoom].east) {
                        roomFound = true;
                    }
                    else {
                        ++i;
                    }
                }
                
                currentRoom = i;
                printf("%s\n", rooms[currentRoom].description);
            }
        }
        else if (strcmp(token, "west\n") == 0) {
            if (rooms[currentRoom].west == -1) {
                printf("You can't go there.\n");
            }
            else {
                int i = 0;
                while (!roomFound) {
                    if (rooms[i].number == rooms[currentRoom].west) {
                        roomFound = true;
                    }
                    else {
                        ++i;
                    }
                }
                
                currentRoom = i;
                printf("%s\n", rooms[currentRoom].description);
            }
        }
        else if (strcmp(token, "quit\n") == 0) {
            end = true;
        }
    }
}

void loaddungeon(char* fileName) {
    FILE* fp = fopen(fileName, "r");
    char line[MAX_CHAR];
    char* token;
    //char* description;
    
    // begin parsing file
    int i = 0;
    while (fgets(line, sizeof(line) - 1,fp) != NULL) {
        if (line[0] != '\n') {
            // get room number
            token = strtok(line, "+");
            rooms[i].number = atoi(token);
            
            // get description
            token = strtok(NULL, "+");
            rooms[i].description = malloc(strlen(token) + 1);
            strcpy(rooms[i].description, token);
            
            // get north house
            token = strtok(NULL, " ");
            rooms[i].north = atoi(token);
            
            // get south House
            token = strtok(NULL, " ");
            rooms[i].south = atoi(token);
            
            // get east house
            token = strtok(NULL, " ");
            rooms[i].east = atoi(token);
            
            // get west house
            token = strtok(NULL, " ");
            rooms[i].west = atoi(token);
            
            ++i;
        }
    }
    
    fclose(fp);
}
