    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include "circularBuffer.h"

    long long process_binary(int fd, int bufferSize){
        unsigned long long sum = 0;
        int intSize=sizeof(int);
        //we adjust the buffer size to only fit whole integers (4 bytes)
        int adjustedBuffer=bufferSize-(bufferSize % intSize);
        if(adjustedBuffer<=0){
            adjustedBuffer=intSize;

        }
        unsigned char *buf = malloc(adjustedBuffer); //buffer is pointer to raw bytes
        if(!buf){
            perror("malloc");
            return 0;
        }

        ssize_t bytesRead;
        while((bytesRead=read(fd,buf,adjustedBuffer))>0){
            int count=bytesRead/sizeof(int);
            int *n=(int*)buf;

            unsigned int unsignedsum = (unsigned int)sum;
            for(int i=0; i<count; i++){
                unsignedsum+=(unsigned int)n[i];
            }

            sum =(long long)(int)unsignedsum;
        }

        if(bytesRead<0){
            perror("read");
        }

        free(buf);
        return sum;
    }



    long long process_text(int fd, int bufferSize){
        unsigned int sum = 0;  // 32-bit accumulator
        CircularBuffer circbuf;
        if(buffer_init(&circbuf, bufferSize) != 0){
            perror("buffer_init");
            return 0;
        }

        unsigned char character;
        ssize_t bytes;

        while(1){
            bytes = read(fd, &character, 1);
            if (bytes < 0){ perror("read"); break; }
            if (bytes == 0) break;

            if (buffer_free_bytes(&circbuf) > 0){ 
                buffer_push(&circbuf, character);
            }
            while (1){
                int size = buffer_size_next_element(&circbuf, ',', 0);
                if (size <= 0) break;

                char str[size];
                for (int i = 0; i < size; i++)
                    str[i] = buffer_pop(&circbuf);

                int len = size;
                if (str[len - 1] == ',') len--;
                str[len] = '\0';

                sum += (unsigned int)atoll(str);  // 32-bit wrap
            }
        }

        if (buffer_used_bytes(&circbuf) > 0){
            int size = buffer_used_bytes(&circbuf);
            char str[size + 1];
            for (int i = 0; i < size; i++)
                str[i] = buffer_pop(&circbuf);
            str[size] = '\0';
            sum += (unsigned int)atoll(str);  // 32-bit wrap
        }

        buffer_deallocate(&circbuf);
        return (long long)sum;  // cast to long long for return
    }

    

    int main(int argc, char* argv[]){
        const char* file = "int_text_small.txt";
        int fd;
        fd = open(file, O_RDONLY);

        if (fd<0){
            perror("open");
            return 1;
        }
        printf("File abierto: fd= %d\n",fd);
        printf("%lld\n",process_text(fd,1024));

        close(fd);
        return 0;
        
    }
    