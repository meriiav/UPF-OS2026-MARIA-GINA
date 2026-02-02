#define PROGRAM_H

#include <unistd.h> 
#include "circularBuffer.h"

long long process_binary(int fd, int bufferSize);
long long process_text(int fd, int bufferSize);

