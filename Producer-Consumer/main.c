#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Global Constants
#define MAX_PRODUCERS 16
#define MAX_CONSUMERS 16

// Structures
typedef struct {
    int number;
    int items;
} Producer;

typedef struct {
    int number;
    int items;
} Consumer;

// Function Declarations
void processArgs(Producer**, Consumer**, char*);

int main(int argc, char* argv[]) {
    Producer* p;
    Consumer* c;
    processArgs(&p, &c, *argv);
    return 0;
}
void processArgs(Producer** producers, Consumer** consumers, char* argv) {
    printf("%s\n", argv[1]);
    int numProducers = atoi(&argv[1]);
    int numConsumers = atoi(&argv[2]);
    int numItems = atoi(&argv[3]);
    int delay = atoi(&argv[4]);

    // # producers and # consumers must be between 1 and 16
    if (numProducers > 0 && numProducers <= MAX_PRODUCERS &&
        numConsumers > 0 && numConsumers <= MAX_CONSUMERS) {
            // allocate memory for producers and consumers
            *producers = (Producer*) malloc(sizeof(Producer) * numProducers);
            *consumers = (Consumer*) malloc(sizeof(Consumer) * numConsumers);

            int i = 0;
            for (i = 0; i < numProducers; ++i) {
                producers[i]->number = i;
                producers[i]->items = numItems;
            }

            for (i = 0; i < numConsumers; ++i) {
                consumers[i]->number = i;
                consumers[i]->items = (numProducers * numItems) / numConsumers;
            }
    }
    else {
        producers = NULL;
        consumers = NULL;
    }
}



