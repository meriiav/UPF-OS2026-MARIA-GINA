#include <stdio.h>
#include <stdlib.h>
#include "circularBuffer.h"


long long process_binary(int fd, int bufferSize){
    long long sum = 0;
    int intSize=sizeof(int);

    //we adjust the buffer size to only fit whole integers (4 bytes)
    int adjustedBuffer=bufferSize-(bufferSize % intSize);
    unsigned char *buf = malloc(adjustedBuffer);
    if(!buf){
        perror("malloc");
        return 0;
    }
    
    ssize_t bytesRead;

    while((bytesRead=read(fd,buf,adjustedBuffer))>0){

    }
}

long long process_text(int fd, int bufferSize){
    long long sum = 0;
    CircularBuffer cb;
    if(buffer_init(&cb,bufferSize)!=0){
        perror("buffer_init");
        return 0;
    }




}

int main(int argc, char* argv[]){

}