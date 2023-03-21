#!/bin/bash

mpicc -std=c99 -o ping_pong ping_pong.c

for mode in 0 1 2
do
   echo "MODE"
   mpiexec -machinefile ./onenode -np 2 ./ping_pong $mode >> pp1_out.txt
   mpiexec -machinefile ./twonodes -np 2 ./ping_pong $mode >> pp2_out.txt
done