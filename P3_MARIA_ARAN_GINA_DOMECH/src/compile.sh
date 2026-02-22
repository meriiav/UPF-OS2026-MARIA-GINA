#!/bin/bash
# Compile the parallel histogram program with pthread support

# Compile the program in the current folder
gcc P3_Parallel.c parsePGM.c -o computeHistogramParallel -lpthread