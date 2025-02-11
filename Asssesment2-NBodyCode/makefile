#makefile for Assement 1
#Uses the C++ 20 standard and the C 17 standard 
#----------------------------------------------
#source code objects
PATH_SRC = ./src
OBJECTS_SRC = $(PATH_SRC)/Body.o $(PATH_SRC)/Integrator.o $(PATH_SRC)/MultithreadingLockGuards.o $(PATH_SRC)/Observer.o $(PATH_SRC)/FileReader.o
#---------------------------------------------
OBJECTS = NBodyCode.o $(OBJECTS_SRC)
CC = g++ -std=c++20 #Compiler
DEBUG = -g
LFLAGS = -Wall -Wextra -g -o3#linker flags
CFLAGS = -Wall -Wextra -c -march=native -funroll-loops -fconcepts -g -fopenmp -o3
#---------------------------------------------
#Compilation
SEARCHDIR = -I$(PATH_SRC)
Assessment2: $(OBJECTS)
	$(CC) $(LFLAGS) $^ $(SEARCHDIR) -o $@

NBodyCode.o: NBodyCode.cpp
	$(CC) $(CFLAGS) $(SEARCHDIR) NBodyCode.cpp -o NBodyCode.o
%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(SEARCHDIR) $< -o $@
#Clean binaries for clean recompilation
.PHONY: clean
clean:
	rm *.o $(PATH_SRC)/*.o