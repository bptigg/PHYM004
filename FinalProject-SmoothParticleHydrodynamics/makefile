#makefile for FinalProject
#Uses the C++ 20 standard and the C 17 standard 
#----------------------------------------------
#source code objects
PATH_SRC = ./src
OBJECTS_SRC = $(PATH_SRC)/particle.o $(PATH_SRC)/Integrator.o $(PATH_SRC)/MultithreadingLockGuards.o #$(PATH_SRC)/Observer.o $(PATH_SRC)/FileReader.o
#---------------------------------------------
OBJECTS = NBodyCode.o $(OBJECTS_SRC)
CC = g++ -std=c++20 #Compiler
DEBUG = -g
LFLAGS = -Wall -Wextra -g #-o3#linker flags
CFLAGS = -Wall -Wextra -c -march=native -funroll-loops -fconcepts -g -fopenmp #-o3
#---------------------------------------------
#Compilation
SEARCHDIR = -I$(PATH_SRC)
SPH: $(OBJECTS)
	$(CC) $(LFLAGS) $^ $(SEARCHDIR) -o $@

SPH.o: SPH.cpp
	$(CC) $(CFLAGS) $(SEARCHDIR) SPH.cpp -o SPH.o
%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(SEARCHDIR) $< -o $@
#Clean binaries for clean recompilation
.PHONY: clean
clean:
	rm *.o $(PATH_SRC)/*.o