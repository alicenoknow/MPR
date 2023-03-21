#!/bin/bash

mpicc -std=c99 -o pi pi.c

for type in 0 1
do         
     for proc in $(seq 1 12);
     do
        echo "$type $proc"
        echo "$type $proc" >> vnode_out.txt
        mpiexec -machinefile ./allnodes -np $proc ./pi 2000000000 $type >> vnode_out.txt
     done
done