#!/bin/sh

export MDP_LIB=/home/benjha/cuda-workspace/temp/CUDA_MDP_LINUX/Release

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MDP_LIB

mpirun -np 5 ./CASIM_LINUX_MPI


