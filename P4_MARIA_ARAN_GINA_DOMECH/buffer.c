#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>

#define nBuffer 10  //size of the circular buffer

// The actual storage
extern unsigned char* buffer[nBuffer]; 
extern int bufferSizes[nBuffer];

//pointers
extern int head;
extern int tail;
extern int elementsInBuffer;
extern int activeProducers;

// semaphors
extern sem_t full;            //filled slots
extern sem_t empty;           //empty slots
extern pthread_mutex_t lock_buffer; //protects head/tail/elementsInBuffer

#endif