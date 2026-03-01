#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"
#include "producer.h"
#include "consumer.h"

// --- Global Variable Definitions (the "Externs" from your other files) ---
unsigned char* buffer[nBuffer];
int bufferSizes[nBuffer];
int head = 0;
int tail = 0;
int elementsInBuffer = 0;
int activeProducers = 0;
int readPos = 0; // Starting position for the file read

sem_t full;
sem_t empty;
pthread_mutex_t lock_buffer;
pthread_mutex_t lock_read;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    char *input_path = argv[1];
    char *output_path = argv[2];

    sem_init(&full, 0, 0);          // Starts at 0 (nothing to consume)
    sem_init(&empty, 0, nBuffer);   // Starts at nBuffer (all slots free)
    
    pthread_mutex_init(&lock_buffer, NULL);
    pthread_mutex_init(&lock_read, NULL);

    //Thread IDs
    pthread_t producer_tid, consumer_tid;
    activeProducers = 1; //starting one producer

    //create Threads
    printf("Starting Producer and Consumer...\n");
    if (pthread_create(&producer_tid, NULL, Producer, (void*)input_path) != 0) {
        perror("Failed to create producer thread");
        return 1;
    }
    if (pthread_create(&consumer_tid, NULL, Consumer, (void*)output_path) != 0) {
        perror("Failed to create consumer thread");
        return 1;
    }

    //wait for threads to finish
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    //clean everything
    sem_destroy(&full);
    sem_destroy(&empty);
    pthread_mutex_destroy(&lock_buffer);
    pthread_mutex_destroy(&lock_read);

    return 0;
}