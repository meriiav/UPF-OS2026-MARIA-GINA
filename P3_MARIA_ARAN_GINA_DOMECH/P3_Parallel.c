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
 int offset; // Offset from the beginning of the file (including header)
 int bytesToRead;
 int localhist[HISTSIZE]; //each thread has its own local histogram
} ThreadInfo;

//global histogram and mutex for merging the final histogram
int globalhist[HISTSIZE];
pthread_mutex_t hist_mutex = PTHREAD_MUTEX_INITIALIZER; 

//thread function, executed by each thread
void* readFilePart(void* arg) {
    ThreadInfo* info = ((ThreadInfo*) arg); //local copy of the information of the thread

    char buffer[BUFFERSIZE];
    int fd=open(info.path, O_RDONLY);
    if(fd<0){
        pthread_exit(NULL);
    }

    lseek(fd,info.offset,SEEK_SET); //position pointer at the offset which corresponds

    int remaining=info.bytesToRead; //how many bytes remain to be read?

    while(remaining>0){
        int toRead;
        if(remaining>BUFFERSIZE){
            toRead=BUFFERSIZE; //read maximum BUFFERSIZE each time
        }
        else{
            toRead=remaining; //last call can be less
        }

        int n= read(fd,buffer,toRead);
        if(n<=0) break;
        
        //count frequency of each pixel in the buffer
        for (int i= 0; i<n; i++){
            unsigned char pixel= buffer[i];
            info->localhist[pixel]++; //increment local histogram
        }
        remaining -= n; 
    }

    close(fd);

// falta  copiar l'histograma original a l'estruct original o algo aixi
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

    int headerBytes=parsePGM(inputPath, &width, &height,&maxval); //function for reading PGM header (already given)
    if(headerBytes<0){
        return 1;
    }

    memset(globalhist, 0, sizeof(globalhist)); //new

    int totalPixels = width * height; //total number of pixels to read
    int chunk  = totalPixels / numThreads ; //num of pixels per thread

    //create arrays for the threads and their info 
    pthread_t* threads = malloc(sizeof(pthread_t) * numThreads);
    ThreadInfo* infos = malloc(sizeof(ThreadInfo) * numThreads);
    //int offset = headerBytes;

    if (threads == NULL || infos == NULL){ //comprovar si el malloc ha functionat
        return 1;
    }

    //creating threads
    for(int i = 0; i < numThreads; i++){

        infos[i].path = inputPath;
        infos[i].offset = headerBytes+i*chunk;

        if(i == numThreads - 1)
            infos[i].bytesToRead = totalPixels-i*chunk;
        else
            infos[i].bytesToRead = chunk;

        memset(infos[i].localhist, 0, sizeof(infos[i].localhist));

        pthread_create(&threads[i], NULL, readFilePart, &infos[i]);
    }

    //waiting threads
    for(int i= 0; i<numThreads; i++){
        pthread_join(threads[i],NULL);
    }

    //joining histograms
    for(int i = 0; i < numThreads; i++){
        for(int j = 0; j < HISTSIZE; j++){
            globalhist[j] += infos[i].localhist[j];
        }
    }

    //writing output
    FILE* out = fopen(outputPath, "w");
    if(!out) return 1;
    for(int i = 0; i < HISTSIZE; i++){
        fprintf(out, "%d\n", globalhist[i]);
    }

    //ending program
    fclose(out);
    free(threads);
    free(infos);
    return 0;

}