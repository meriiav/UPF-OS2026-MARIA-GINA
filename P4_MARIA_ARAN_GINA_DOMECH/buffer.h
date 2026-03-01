#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>

#define nBuffer 10

//declaring variables
extern unsigned char* buffer[nBuffer];
extern int bufferSizes[nBuffer];
extern int head;
extern int tail;
extern int elementsInBuffer;
extern int activeProducers;
extern int readPos;

extern sem_t full;
extern sem_t empty;
extern pthread_mutex_t lock_buffer;
extern pthread_mutex_t lock_read;

#endif