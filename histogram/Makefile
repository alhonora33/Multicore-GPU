# Nom du programme
PROG = histogram

# Fichiers sources
SRC = histogram.cu
OBJ = $(SRC:.cu=.o)

# Chemins CUDA
CUDA_HOME = /opt/cuda
NVCC = nvcc

# Options CUDA
NVCCFLAGS = -ccbin /usr/bin/gcc-13 -std=c++14 -I$(CUDA_HOME)/include
NVLDFLAGS = -L$(CUDA_HOME)/lib64 -lcudart

# Règles
.PHONY: all clean

all: $(PROG)

$(PROG): $(OBJ)
	$(NVCC) $(NVLDFLAGS) -o $@ $^

%.o: %.cu
	$(NVCC) $(NVCCFLAGS) -c -o $@ $<

clean:
	rm -fv $(PROG) $(OBJ)

