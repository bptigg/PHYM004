#makefile for Assement 1
#Uses the C++ 20 standard and the C 17 standard 
ARMADILLO = -larmadillo -lopenblas -llapack -fopenmp -DARMA_NO_DEBUG
#----------------------------------------------
#source code objects
PATH_SRC = ./src
OBJECTS_SRC = #$(PATH_SRC)/Body.o $(PATH_SRC)/Intergrator.o #$(PATH_SRC)/FileHandeling.o $(PATH_SRC)/Matrix.o
#---------------------------------------------
OBJECTS = main.o $(OBJECTS_SRC)
CC = g++ -std=c++20 #Compiler
DEBUG = -g
LFLAGS = -Wall -g #linker flags
CFLAGS = -Wall -c -march=native -funroll-loops -fconcepts -g
#---------------------------------------------
#Compilation
SEARCHDIR = -I$(PATH_SRC) $(ARMADILLO)
ProjectileFlight: $(OBJECTS)
	$(CC) $(LFLAGS) $^ $(SEARCHDIR) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $(SEARCHDIR) main.cpp -o main.o
%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(SEARCHDIR) $< -o $@
#Clean binaries for clean recompilation
.PHONY: clean
clean:
	rm *.o $(PATH_SRC)/*.o