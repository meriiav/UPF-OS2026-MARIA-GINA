    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include "circularBuffer.h"


    long long process_binary(int fd, int bufferSize){
        long long sum = 0;
        int intSize=sizeof(int);

        //we adjust the buffer size to only fit whole integers (4 bytes)
        int adjustedBuffer=bufferSize-(bufferSize % intSize);
        unsigned char *buf = malloc(adjustedBuffer);  //buffer is pointer to raw bytes
        if(!buf){
            perror("malloc");
            return 0;
        }
        
        ssize_t bytesRead;
        while((bytesRead=read(fd,buf,adjustedBuffer))>0){
            int count=bytesRead/sizeof(int);
            int *n=(int*)buf;
            for(int i = 0; i < count; i++){
                sum+=n[i];
            } 
        }
        return sum;
    }

    //long long process_text(int fd, int bufferSize){
    //    long long sum = 0;
    //    CircularBuffer cb;
        // if(buffer_init(&cb,bufferSize)!=0){
        //     perror("buffer_init");
        //     return 0;
        // }
        
        // unsigned char circbuf*=malloc(bufferSize);
        // if(!circbuf){
        //     perror("malloc");
        //     return 0;

        // }

        // ssize_t=bytesRead;
        // EOF=0;

    //}

    int main(int argc, char* argv[]){
        const char* file = "test_small.dat";
        int fd;
        fd = open(file, O_RDONLY);

        if (fd<0){
            perror("open");
            return 1;
        }
        printf("File abierto: fd= %d\n",fd);
        printf("%lld\n",process_binary(fd,1024));

        close(fd);
        return 0;
        
    }