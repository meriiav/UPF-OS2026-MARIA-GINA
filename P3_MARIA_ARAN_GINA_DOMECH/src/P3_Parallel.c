#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>


#include "parsePGM.h"

#define BUFFERSIZE 1024
#define HISTSIZE 256

typedef struct {
 char* path;
 int offset; //offset from the beginning of the file (including header)
 int bytesToRead;
} ThreadInfo;

//global histogram and mutex for merging the final histogram
int globalhist[HISTSIZE];
pthread_mutex_t hist_mutex = PTHREAD_MUTEX_INITIALIZER; 

//thread function, executed by each thread
void* readFilePart(void* arg) {
    ThreadInfo* info = (ThreadInfo*) arg;
    int localhist[HISTSIZE] = {0};

    char buffer[BUFFERSIZE];
    int fd = open(info->path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        pthread_exit(NULL);
    }

    if(lseek(fd, info->offset, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        pthread_exit(NULL);
    }

    int remaining = info->bytesToRead;
    while (remaining > 0) {
        int toRead = (remaining > BUFFERSIZE) ? BUFFERSIZE : remaining;
        int n = read(fd, buffer, toRead);
        if (n <= 0) break;

        for (int i = 0; i < n; i++) {
            unsigned char pixel = buffer[i];
            localhist[pixel]++;
        }
        remaining -= n;
    }

    close(fd);

    pthread_mutex_lock(&hist_mutex);
    for (int i = 0; i < HISTSIZE; i++) {
        globalhist[i] += localhist[i];
    }
    pthread_mutex_unlock(&hist_mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    //pthread_t thread;
    //ThreadInfo info = {"test.txt", 0, 20};  // Example: read first 20 bytes
    //expected usage: program input.pgm output.txt num_threads
    if(argc!=4){
        return 1;
    }
    char* inputPath = argv[1]; //input file
    char* outputPath = argv[2]; //output file
    int numThreads = atoi(argv[3]); //turn num of threads into an integer
    if (numThreads <= 0){
        return 1; 
    }

    int width, height, maxval;

    int headerBytes=parse_pgm_header(inputPath, &width, &height,&maxval); //function for reading PGM header (already given)
    if(headerBytes<0){
        return 1;
    }

    memset(globalhist, 0, sizeof(globalhist)); //set to zero

    long totalPixels = width * height; //total number of pixels to read
    int chunk  = totalPixels / numThreads ; //num of pixels per thread

    //create arrays for the threads and their info 
    pthread_t* threads = malloc(sizeof(pthread_t) * numThreads);
    ThreadInfo* infos = malloc(sizeof(ThreadInfo) * numThreads);

    if (threads == NULL || infos == NULL){ //malloc error
        return 1;
    }

    //creating threads
    int currentOffset = headerBytes;
    
    for(int i = 0; i < numThreads; i++) {

        infos[i].path = inputPath;
        infos[i].offset = currentOffset;

        infos[i].bytesToRead = chunk;
        if(i == numThreads - 1)
            infos[i].bytesToRead += totalPixels - i * chunk;
        currentOffset += infos[i].bytesToRead;

        pthread_create(&threads[i], NULL, readFilePart, &infos[i]);
    }

    //waiting threads
    for(int i= 0; i<numThreads; i++){
        pthread_join(threads[i],NULL);
    }

    //writing output
    FILE* out = fopen(outputPath, "w");
    if(!out) return 1;
    for(int i = 0; i < HISTSIZE; i++){
        fprintf(out, "%d,%d\n", i, globalhist[i]);
    }
    fprintf(out, "\n");

    //ending program
    fclose(out);
    free(threads);
    free(infos);
    pthread_mutex_destroy(&hist_mutex);
    return 0;

}

// gcc P3_parallel.c parsePGM.c -o build/computeHistogramSequential
// build/computeHistogramSequential Data/heart.pgm Data/histogram_heart.txt
// python showHistogram.py Data/histogram_heart.txt
// python3 showHistogram.py Data/histogram_heart.txt