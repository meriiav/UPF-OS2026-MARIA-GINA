    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <time.h>
    #include <string.h>
    #include "circularBuffer.h"

    long long process_binary(int fd, int bufferSize){
        unsigned long long sum = 0;
        int intSize=sizeof(int);
        //we adjust the buffer size to only fit whole integers (4 bytes)
        int adjustedBuffer=bufferSize-(bufferSize % intSize);
        if(adjustedBuffer<=0){
            adjustedBuffer=intSize;

        }
        unsigned char *buf = malloc(adjustedBuffer); //buffer is pointer to raw bytes, we allocate it for raw bytes
        if(!buf){
            perror("malloc");
            return 0;
        }

        ssize_t bytesRead;
        //read chunks of data from the file
        while((bytesRead=read(fd,buf,adjustedBuffer))>0){
            int count=bytesRead/sizeof(int); //num of integers read
            int *n=(int*)buf; //interpret bytes as integers

            unsigned int unsignedsum = (unsigned int)sum;
            for(int i=0; i<count; i++){
                unsignedsum+=(unsigned int)n[i];
            }
           //store back into the sum
            sum =(long long)(int)unsignedsum;
        }

        if(bytesRead<0){ //handle read error
            perror("read");
        }

        free(buf);
        return sum;
    }



    long long process_text(int fd, int bufferSize){
        unsigned int sum = 0;  // 32-bit accumulator
        CircularBuffer circbuf;
        //initialize circular buffer with given size
        if(buffer_init(&circbuf, bufferSize) != 0){
            perror("buffer_init");
            return 0;
        }

        unsigned char character;
        ssize_t bytes;

        //read input one character at a time
        while(1){
            bytes = read(fd, &character, 1);
            if (bytes < 0){ 
                perror("read"); break;  //read error
            }
            if (bytes == 0) break; //end of file

            //push character into buffer if space is available
            if (buffer_free_bytes(&circbuf) > 0){ 
                buffer_push(&circbuf, character);
            }
            //process numbers ending with ','
            while (1){
                int size = buffer_size_next_element(&circbuf, ',', 0);
                if (size <= 0) break;

                char str[size];
                //pop characters forming the number
                for (int i = 0; i < size; i++)
                    str[i] = buffer_pop(&circbuf);

                int len = size;
                //remove trailing coma (if present )
                if (str[len - 1] == ',') len--;
                str[len] = '\0'; //null-terminate string
                //convert string to number and add (32-bit wrap)
                sum += (unsigned int)atoll(str);  // 32-bit wrap
            }
        }

        //process any remainign number not followed by a coma
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
        //validate argument count 
         if (argc < 4) {
            fprintf(stderr, "%s <binary|text> <path> <bufferSize>\n", argv[0]);
            return 1;
    }
        
        const char *mode=argv[1]; 
        const char *path=argv[2];
        int bufferSize=atoi(argv[3]); //buffer size from user

        //open file for reading
        int fd=open(path,O_RDONLY);
        if(fd<0){
            perror("open");
            return 1;
        }

        
         //choose processing mode
        long long sum;
        if (strcmp(mode, "binary") == 0) {
            sum = process_binary(fd, bufferSize);
        } else if (strcmp(mode, "text") == 0) {
            sum = process_text(fd, bufferSize);
        } else {
            fprintf(stderr, "Invalid mode: %s\n", mode);
            close(fd);
            return 1;
        }
        //print final sum
        fprintf(stdout,"%lld\n", sum);
        close(fd);
        return 0;
    }

        