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
        long long sum = 0;
        CircularBuffer circbuf;
        if(buffer_init(&circbuf,bufferSize)!=0){
            perror("buffer_init");
            return 0;
            }

        unsigned char character;
        ssize_t bytes;


        while(1){
            bytes = read(fd, &character, 1);
            if (bytes==0){
                int size=buffer_size_next_element(&circbuf,',',1);
                if(size!=-1){
                    char str[64]; //
                    int i=0;
                    while(buffer_used_bytes(&circbuf)>0){
                        str[i++]=buffer_pop(&circbuf);
                    }
                    str[i]='\0';
                    sum+=atoll(str);
                    }
                    break;
                }


            if (bytes<0){
                perror("read");
                break;
            }

            if (buffer_free_bytes(&circbuf)>0){
                buffer_push(&circbuf,character);
            }

            if(character==','){
                char str[64];
                int i=0;

                while(buffer_used_bytes(&circbuf)>0){
                    char character2 = buffer_pop(&circbuf);
                    if(character2!=','){
                        str[i++]=character2;

                    }
                    else{
                        break;
                    }
                }
                str[i]='\0';
                sum+=atoll(str);
        }
     }
        buffer_deallocate(&circbuf);
        return sum;
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
    