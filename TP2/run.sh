#!/bin/bash
#SBATCH --time=1:00
#SBATCH --partition=cpar
###SBATCH --constraint=k20

module load gcc/7.2.0

perf record ./a.out
perf report 