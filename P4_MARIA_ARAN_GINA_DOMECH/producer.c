#include producer.h

int blockSize = 1024 * 16;
// Position from which the Producer will try to read a block. This needs to be initialised to just after the header.
int readPos; 
pthread_mutex_t lock_read;

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
        
        //Consumer part of adding to the buffer: need to 
        while (elementsInBuffer == nBuffer){ 
            pthread_cond_wait(&not_full, &lock_buffer);
        }
        buffer[tail] = buff;
        bufferSizes[tail] = nBytesRead;

        tail = (tail + 1) % nBuffer;
        elementsInBuffer++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock_buffer);
    }
    // If exiting, make sure you wake up all sleeping threads before exiting 
    // (and that they don't go to sleep if the finishes)

    close(fd);

    pthread_mutex_lock(&lock_buffer);
    activeProducers--;

    if(activeProducers == 0)
        pthread_cond_broadcast(&not_empty);

    pthread_mutex_unlock(&lock_buffer);

    pthread_exit(NULL);
}
