#include producer.h
#include "buffer.h"  // contains buffer, semaphores, mutexes
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int blockSize = 1024 * 16;
// Position from which the Producer will try to read a block. This needs to be initialised to just after the header.
int readPos; 
pthread_mutex_t lock_read;

extern sem_t full;   //counts filled slots
extern sem_t empty;  //counts empty slots

void * Producer (void* arg) {
    char * path = (char *) arg; // Need to pass the file as a path
    int fd = open(path, O_RDONLY);
    int nBytesRead; 
    int readPosLocal;
    while (1) {
        pthread_mutex_lock(&lock_read);
        readPosLocal = readPos;
        readPos += blockSize;
        pthread_mutex_unlock(&lock_read);

        // Produce an item by reading a block
        lseek(fd, readPosLocal, SEEK_SET);
        unsigned char* buff = malloc(blockSize);
        nBytesRead = read(fd, buff, blockSize);
        if (nBytesRead <= 0) {
            free(buff);
            break;
        }

        //WAIT FOR EMPTY SLOT
        sem_wait(&empty);                  // wait if buffer is full

        pthread_mutex_lock(&lock_buffer);  // lock buffer to safely insert
        buffer[tail] = buff;
        bufferSizes[tail] = nBytesRead;
        tail = (tail + 1) % nBuffer;
        elementsInBuffer++;
        pthread_mutex_unlock(&lock_buffer);

        sem_post(&full);                   // signal consumer there is a new item
    }
    // If exiting, make sure you wake up all sleeping threads before exiting 
    // (and that they don't go to sleep if the finishes)

    close(fd);

    //LAST PRODUCER
    pthread_mutex_lock(&lock_buffer);
    activeProducers--;
    pthread_mutex_unlock(&lock_buffer);
    sem_post(&full);  // wake any consumer that might be waiting

    pthread_exit(NULL);
}
