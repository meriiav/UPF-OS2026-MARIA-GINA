#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct {
 char* path;
 int offset; // Offset from the beginning of the file (including header)
 int bytesToRead;
 // any other info you might want to pass
} ThreadInfo;


void* readFilePart(void* arg) {

    }

int main() {
    pthread_t thread;
    ThreadInfo info = {"test.txt", 0, 20};  // Example: read first 20 bytes
}