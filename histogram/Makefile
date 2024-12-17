# Nom du programme
PROG = histogram

# Chemins CUDA
CUDA_HOME = /opt/cuda
NVCC = nvcc

# Options CUDA
NVCCFLAGS = -I$(CUDA_HOME)/include
NVLDFLAGS = -L$(CUDA_HOME)/lib64 -lcudart

# Options GCC
GCC = gcc
GCCFLAGS_1 = -Wall -O3 \
					 -fno-loop-block \
					 -fno-tree-loop-im \
					 -fno-prefetch-loop-arrays \
					 -fno-strict-aliasing \
					 --tree-parallelize-loops=0 \
					 -fno-openmp \
					 -fno-schedule-insns \
					 -fno-tree-loop-optimize 

# Flags pour la version séquentielle dichotomique
GCCFLAGS_SEQ = -Wall -O3 \
               -fstrict-aliasing \
               -ftree-vectorize \
               -march=native \
               --tree-parallelize-loops=0 \

# Flags pour les versions parallèles (OpenMP)
GCCFLAGS_OMP = -Wall -O3 \
               -fopenmp \
               -fstrict-aliasing \
               -ftree-vectorize \
               -march=native \
               -ftree-parallelize-loops=12 \
               -floop-nest-optimize

GCC_OPENMPFLAGS = -fopenmp

# Règles
.PHONY: all clean

all: $(PROG)_dico $(PROG)_omp $(PROG)_omp_dico $(PROG)_cuda $(PROG)_cuda_shared $(PROG)_cuda_tmp # $(PROG)_naive


# Génération de l'exécutable pour la version naïve
# $(PROG)_naive: histogram.c
#	$(GCC) $(GCCFLAGS_1) -o $@ $<

# Génération de l'exécutable pour la version dichotomique
# $(PROG)_dico: histogram_dico.c
#	$(GCC) $(GCCFLAGS_1) -o $@ $<

$(PROG)_dico: histogram_dico.c
	$(GCC) $(GCCFLAGS_SEQ) -o $@ $<

# Génération de l'exécutable pour la version naïve avec OpenMP
$(PROG)_omp: histogram_omp.c
	$(GCC) $(GCCFLAGS_OMP) -o $@ $<

# Génération de l'exécutable pour la version dichotomique avec OpenMP
$(PROG)_omp_dico: histogram_omp_dico.c
	$(GCC) $(GCCFLAGS_OMP) -o $@ $<

# Génération de l'exécutable pour la version CUDA
$(PROG)_cuda: histogram_cuda.cu
	$(NVCC) $(NVCCFLAGS) $(NVLDFLAGS) -o $@ $<

# Génération de l'exécutable pour la version CUDA avec mémoire partagée
$(PROG)_cuda_shared: histogram_cuda_shared.cu
	$(NVCC) $(NVCCFLAGS) $(NVLDFLAGS) -o $@ $<

# Génération de l'exécutable pour la version CUDA avec mémoire partagée
$(PROG)_cuda_tmp: histogram_cuda_tmp.cu
	$(NVCC) $(NVCCFLAGS) $(NVLDFLAGS) -o $@ $<

clean:
	rm -fv $(PROG)_naive $(PROG)_dico $(PROG)_omp $(PROG)_omp_dico $(PROG)_cuda $(PROG)_cuda_shared $(PROG)_cuda_tmp
