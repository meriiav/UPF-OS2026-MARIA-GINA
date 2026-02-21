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
 int localhist[HISTSIZE]; //each thread has its own histogram
} ThreadInfo;

//global histogram and mutex for merging the final histogram
int globalhist[HISTSIZE];
pthread_mutex_t hist_mutex = PTHREAD_MUTEX_INITIALIZER; 

//thread function , which will be executed by each thread
void* readFilePart(void* arg) {
    ThreadInfo info = *((ThreadInfo*) arg); //make a local copy of the information of the thread

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
            toRead=remaining; // the last lecture can be less
        }

        int n= read(fd,buffer,toRead);
            if(n<=0){
                break;
            }
        
        //count frequency of each pixel in the buffer
        for (int i= 0; i<n; i++){
            unsigned char pixel= buffer[i];
            info.localhist[pixel]++; //increment local histogram
        }
        remaining = remaining - n; 
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

    int width, height, maxval ;

    int headerBytes=parsePGM(inputPath, &width, &height,&maxval); //function for reading PGM header (already given)
    if(headerBytes)<0{
        return 1;
    }


    int totalPixels = width * height; //total number of pixels to read

    //create arrays for the threads and their info 
    pthread_t* threads = malloc(sizeof(pthread_t) * numThreads);
    ThreadInfo* infos = malloc(sizeof(ThreadInfo) * numThreads);

    if (threads == NULL || infos == NULL) //comprovar si el malloc ha functionat
        return 1;
    int chunk  = totalPixels / numThreads ; //num of pixels per thread
    int offset = headerBytes;

//falta crear els threads

    //esperar als altres threads
    for(int i= 0; i<numThreads; i++){
        pthread_join(threads[i],NULL);
    }

//juntar els histogrames

//i escriure l'output
}