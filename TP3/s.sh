#!/bin/bash
#SBATCH --time=1:00
#SBATCH --ntasks=4
#SBATCH --partition=cpar
mpic++ -O2 -o k-means k-means.c
mpirun -np 4 ./k-means