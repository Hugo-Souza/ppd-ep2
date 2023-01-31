#!/bin/bash
#SBATCH -J laplace-parallel             	    # Job name
#SBATCH -p fast                                 # Job partition
#SBATCH -n 1                                    # Number of processes
#SBATCH -t 01:30:00                             # Run time (hh:mm:ss)
#SBATCH --cpus-per-task=96                      # Number of CPUs per process
#SBATCH --output=%x.%j.out                      # Name of stdout output file - %j expands to jobId and %x to jobName
#SBATCH --error=%x.%j.err                       # Name of stderr output file
#SBATCH --mail-user=hugo.souza@estudante.ufscar.br
#SBATCH --mail-type=All

echo "----------------------------------------------------------"
echo "*** GRID 512x512 ***"
echo "*** SEQUENTIAL ***"
srun singularity run container.sif laplace_seq 512
echo "    "
echo "*** PTHREAD 1 ***"
srun singularity run container.sif laplace_pth 512 1
echo "    "
echo "*** PTHREAD 2 ***"
srun singularity run container.sif laplace_pth 512 2
echo "    "
echo "*** PTHREAD 5 ***"
srun singularity run container.sif laplace_pth 512 5
echo "    "
echo "*** PTHREAD 10 ***"
srun singularity run container.sif laplace_pth 512 10
echo "    "
echo "*** PTHREAD 20 ***"
srun singularity run container.sif laplace_pth 512 20
echo "    "
echo "*** PTHREAD 40 ***"
srun singularity run container.sif laplace_pth 512 40
echo "    "
echo "----------------------------------------------------------"

echo "----------------------------------------------------------"
echo "*** GRID 1024x1024 ***"
echo "*** SEQUENTIAL ***"
srun singularity run container.sif laplace_seq 1024
echo "    "
echo "*** PTHREAD 1 ***"
srun singularity run container.sif laplace_pth 1024 1
echo "    "
echo "*** PTHREAD 2 ***"
srun singularity run container.sif laplace_pth 1024 2
echo "    "
echo "*** PTHREAD 5 ***"
srun singularity run container.sif laplace_pth 1024 5
echo "    "
echo "*** PTHREAD 10 ***"
srun singularity run container.sif laplace_pth 1024 10
echo "    "
echo "*** PTHREAD 20 ***"
srun singularity run container.sif laplace_pth 1024 20
echo "    "
echo "*** PTHREAD 40 ***"
srun singularity run container.sif laplace_pth 1024 40
echo "    "
echo "----------------------------------------------------------"




echo "----------------------------------------------------------"
echo "*** GRID 2048x2048 ***"
echo "*** SEQUENTIAL ***"
srun singularity run container.sif laplace_seq 2048
echo "    "
echo "*** PTHREAD 1 ***"
srun singularity run container.sif laplace_pth 2048 1
echo "    "
echo "*** PTHREAD 2 ***"
srun singularity run container.sif laplace_pth 2048 2
echo "    "
echo "*** PTHREAD 5 ***"
srun singularity run container.sif laplace_pth 2048 5
echo "    "
echo "*** PTHREAD 10 ***"
srun singularity run container.sif laplace_pth 2048 10
echo "    "
echo "*** PTHREAD 20 ***"
srun singularity run container.sif laplace_pth 2048 20
echo "    "
echo "*** PTHREAD 40 ***"
srun singularity run container.sif laplace_pth 2048 40
echo "    "
echo "----------------------------------------------------------"

