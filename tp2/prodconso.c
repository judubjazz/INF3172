
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define GETOPTSERR -1

int empty = BUFFER_SIZE;
int full = 0;
pthread_mutex_t mutex;
buffer_item buffer[BUFFER_SIZE];
/**
 * insert_item:  Inserts an item into the buffer
 * Paramaters:   buffer_item, item to be inserted
 * Returns:      int, 0 if success; nonzero otherwise
 **/
int insert_item(buffer_item item) {
    if(full == BUFFER_SIZE)
        return -1;
    buffer[full] = item;
    return 0;
}

/**
 * remove_item:  Removes an item from the buffer
 * Paramaters:   buffer_item *, pointer item to be removed
 * Returns:      int, 0 if success; nonzero otherwise
 **/
int remove_item(buffer_item *item) {
    if(empty == BUFFER_SIZE)
        return -1;
    *item = buffer[full-1];
    buffer[full-1] = 0;
    return 0;
}

/**
 * producer:     Function that is called when a new producer
                 thread is created. Waits until lock is available
                 and inserts an item into buffer
 * Paramaters:   void*, arguments
 * Returns:      void
 **/
void *producer(void *param) {
    // Item to insert
    buffer_item item;
    while(1) {
        usleep(rand() % 100000);
        item = rand();
        if(pthread_mutex_lock(&mutex) == 0) {
            if(insert_item(item))
                printf("Error: Buffer full unable to produce\n");
            else {
                full++;
                empty--;
                printf("Thread %d Produced %d\n", pthread_self(), item);
            }
            pthread_mutex_unlock(&mutex);
        }
    }
}

/**
 * consumer:     Function that is called when a new consumer
                 thread is created. Waits until lock is available
                 and removes an item from buffer
 * Paramaters:   void*, arguments
 * Returns:      void
 **/
void *consumer(void *param) {
    // Holds item that is removed
    buffer_item item;
    while(1) {
        usleep(rand() % 100000);
        if(pthread_mutex_lock(&mutex) == 0) {
            if(remove_item(&item))
                printf("Error: Buffer empty unable to consume\n");
            else {
                full--;
                empty++;
                printf("Thread %d Consumed %d\n", pthread_self(),item);
            }
            pthread_mutex_unlock(&mutex);
        }
    }
}

/**
 * main:         gets user input for sleep time, number of
                 producer/consumer threads to create. Creates
                 threads and then waits
 * Paramaters:   void*, arguments
 * Returns:      void
 **/
int main(int argc, char *argv[]) {
    // Amount of time to sleep for
    int sleepTime = 0;
    // Number of producer threads to create
    int numProducerThreads = 0;
    // Number of consumer threads to create
    int numConsumerThreads = 0;
    // Index variables
    int index = 0;
    // Index variable for looping through non-option arguments
    int optionsIndex = 0;

    for(optionsIndex = optind; optionsIndex < argc; optionsIndex++) {
        if(optionsIndex == 1)
            sleepTime = atoi(argv[optionsIndex]);
        if(optionsIndex == 2)
            numProducerThreads = atoi(argv[optionsIndex]);
        if(optionsIndex == 3)
            numConsumerThreads = atoi(argv[optionsIndex]);
    }

    memset(&buffer, 0, sizeof(BUFFER_SIZE));
    // Array of producer threads to create
    pthread_t producerThreads[numProducerThreads];
    // Array of consumer threads to create
    pthread_t consumerThreads[numConsumerThreads];
    pthread_mutex_init(&mutex, NULL);

    for(index = 0; index < numProducerThreads; index++) {
        if(pthread_create(&producerThreads[index], NULL, &producer, NULL))
            printf("Error\n");
    }

    for(index = 0; index < numConsumerThreads; index++) {
        if(pthread_create(&consumerThreads[index], NULL, &consumer, NULL))
            printf("Error\n");
    }
    sleep(sleepTime);
    return 0;
}