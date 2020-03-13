// Alexis Vu 16426804
// Lynette Nguyen 33259665

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h> 

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

typedef struct {
    int* items;                 // array of items on buffer
    int capacity;               // amount buffer can hold
    int size;                   // current # items in buffer
    pthread_mutex_t mutex;      // mutex for mutual exclusive use of buffer
    pthread_cond_t consume;     // condition variable to signal consumer item is ready
    pthread_cond_t produce;     // condition variable to signal producer can put item on buffer
} Buffer;

// Global Variables
Producer* producers;            // list of Producers
Consumer* consumers;            // list of Consumers

int numProducers,               // command-line arguments used
    numConsumers,               // to init producers/consumers
    numItems,
    delay;

pthread_mutex_t pidMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;     // mutex for producer id
pthread_mutex_t cidMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;     // mutex for consumer id

int runningProducers = 0;       // number of running producer threads
int runningConsumers = 0;       // number of running consumer threads

// Function Declarations
void processArgs(char**);
Producer* populateProducers();
Consumer* populateConsumers();
void* producerThread(void*);
void* consumerThread(void*);
int calculateItem(int, int);

int main(int argc, char* argv[]) {
    // process command line arguments
    processArgs(argv);

    // populate the producer and consumer lists
    producers = populateProducers();
    consumers = populateConsumers();
    
    //initialize the buffer
    Buffer buffer;
    buffer.items = (int*) malloc(sizeof(int) * (numProducers * numItems));
    buffer.capacity = numProducers * numItems;
    buffer.size = 0;
    buffer.mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    buffer.produce = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    buffer.consume = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    // initialize producer and consumer
    pthread_t pThreads[numProducers];
    pthread_t cThreads[numConsumers];

    // create and start threads
    int i = 0;
    for (i = 0; i < numProducers; ++i) {
        pthread_create(&pThreads[i], NULL, producerThread, (void*) &buffer);
    }
    
    for (i = 0; i < numConsumers; ++i) {
        pthread_create(&cThreads[i], NULL, consumerThread, (void*) &buffer);
    }

    // join threads
    for (i = 0; i < numProducers; ++i) {
        pthread_join(pThreads[i], NULL);
    }

    for (i = 0; i < numConsumers; ++i) {
        pthread_join(cThreads[i], NULL);
    }

    // clean up mutexes
    pthread_mutex_destroy(&buffer.mutex);
    pthread_mutex_destroy(&pidMutex);
    pthread_mutex_destroy(&cidMutex);

    return 0;
}

// Function Definitions

// processes and initializes program variables with command line arguments
void processArgs(char** argv) {
    numProducers = atoi(argv[1]);
    numConsumers = atoi(argv[2]);
    numItems = atoi(argv[3]);
    delay = atoi(argv[4]);
}

// populates the list of producers
Producer* populateProducers() {
    if (numProducers > 0 && numProducers <= MAX_PRODUCERS) {
        Producer* producers = (Producer*) malloc(sizeof(Producer) * numProducers);

        int i = 0;
        for (i = 0; i < numProducers; ++i) {
            producers[i].number = i;
            producers[i].items = numItems;
        }

        return producers;
    }
    else
        return NULL;
}

// populates the list of consumers
Consumer* populateConsumers() {
    if (numConsumers > 0 && numConsumers <= MAX_CONSUMERS) {
        Consumer* consumers = (Consumer*) malloc(sizeof(Consumer) * numConsumers);

        int i = 0;
        for (i = 0; i < numConsumers; ++i) {
            consumers[i].number = i;
            consumers[i].items = (numProducers * numItems) / numConsumers;
        }
        return consumers;
    }
    else
        return NULL;
}

// producer thread
void* producerThread(void* arg) {
    // reference to the buffer
    Buffer* buffer = (Buffer*) arg;

    // store producer id and increase, then allow next thread to use
    pthread_mutex_lock(&pidMutex);
    int id = runningProducers;
    ++runningProducers;
    pthread_mutex_unlock(&pidMutex);

    // produce until all items are added to buffer
    while (1) {
        // prevent other threads from using buffer
        pthread_mutex_lock(&buffer->mutex);

        // if buffer full, can't add item, signal when space opens up
        while (buffer->size == buffer->capacity) {
            pthread_cond_wait(&buffer->produce, &buffer->mutex);
        }

        // add an item to the buffer
        buffer->items[buffer->size] = calculateItem(id, producers[id].items);
        printf("producer_%d produced item %d\n", id, buffer->items[buffer->size]);
        --(producers[id].items);
        ++(buffer->size);
        
        // suspend producer for 0.5 seconds
        if (delay == 0)
            usleep(500000);

        // signal consumer thread, release buffer for other threads
        pthread_cond_signal(&buffer->consume);
        pthread_mutex_unlock(&buffer->mutex);

        // exit if all items have been added
        if (producers[id].items == 0)
            pthread_exit(NULL);
    }
}

// consumer thread
void* consumerThread(void* arg) {
    // reference to the buffer
    Buffer* buffer = (Buffer*) arg;

    // store consumer id and increase, allow next thread to use
    pthread_mutex_lock(&cidMutex);
    int id = runningConsumers;
    ++runningConsumers;
    pthread_mutex_unlock(&cidMutex);

    // consume until consumption threshold met
    while(1) {
        // prevent other threads from using buffer
        pthread_mutex_lock(&buffer->mutex);

        // if buffer empty, wait until resource available to consume
        while (buffer->size == 0) {
            pthread_cond_wait(&buffer->consume, &buffer->mutex);
        }

        // remove item from buffer
        --(buffer->size);
        printf("consumer_%d consumed item %d\n", id, buffer->items[buffer->size]);
        --(consumers[id].items);
        
        // suspend consumer for 0.5 seconds
        if (delay == 1)
            usleep(500000);

        // signal producer thread, release buffer for other threads
        pthread_cond_signal(&buffer->produce);
        pthread_mutex_unlock(&buffer->mutex);

        // exit if all items have been consumed
        if (consumers[id].items == 0)
            pthread_exit(NULL);
    }
}

// calculate producer's item in buffer
int calculateItem(int prodNum, int itemNum) {
    return (((prodNum + 1) * numItems) - itemNum);
}
