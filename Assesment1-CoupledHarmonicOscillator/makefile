#makefile for Assement 1
#Uses the C++ 20 standard and the C 17 standard 
#----------------------------------------------
#source code objects
PATH_SRC = ./src
OBJECTS_SRC = $(PATH_SRC)/MatrixMethods.o $(PATH_SRC)/FileHandeling.o $(PATH_SRC)/Matrix.o
#---------------------------------------------
OBJECTS = CoupledHarmonicOscillator.o $(OBJECTS_SRC)
CC = g++ -std=c++20 #Compiler
DEBUG = -g
LFLAGS = -Wall -g #linker flags
CFLAGS = -Wall -c -march=native -funroll-loops -fconcepts -g
#---------------------------------------------
#Compilation
SEARCHDIR = -I$(PATH_SRC)
Assessment1: $(OBJECTS)
	$(CC) $(LFLAGS) $^ $(SEARCHDIR) -o $@

CoupledHarmonicOscillator.o: CoupledHarmonicOscillator.cpp
	$(CC) $(CFLAGS) $(SEARCHDIR) CoupledHarmonicOscillator.cpp -o CoupledHarmonicOscillator.o
%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(SEARCHDIR) $< -o $@
#Clean binaries for clean recompilation
.PHONY: clean
clean:
	rm *.o $(PATH_SRC)/*.o