#!/bin/bash -l
#SBATCH --nodes 1
#SBATCH --ntasks 12
#SBATCH --time=07:00:00
#SBATCH --partition=plgrid
#SBATCH --account=plgmpr23-cpu 

module add .plgrid plgrid/tools/openmpi
mpicc -std=c99 -o pi pi.c

for type in 0 1
do    
    for proc in $(seq 1 12);
    do
        mpiexec -np $proc ./pi $1 $type
    done
done
