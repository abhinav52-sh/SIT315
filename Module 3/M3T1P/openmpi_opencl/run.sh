#!/bin/sh
mpic++ code.cpp -o out -lOpenCL
mpirun --hostfile hosts.txt -np 3 ./out
