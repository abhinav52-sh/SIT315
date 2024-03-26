#!/bin/sh
mpic++ code.cpp -o out
mpirun --hostfile hosts.txt -np 3 ./out
