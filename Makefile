# -*- MakeFile -*-
#target: dependencies
#	action
#DIRS
SRC=src
OBJ=object
BIN=bin
LIBS=libs
INC=include
DEP=dep





#COMPILER CONFIG
C_STD=-std=c11
#WARN=-Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code 
#Treatira warning kao error
WARN=-Wall -Wextra -Wconversion -Wunreachable-code
COMP=gcc -fopenmp -march=native
OPT=-O3
DEPFLAGS=-MMD -MP -MF
DEBUG=
FLAGS= $(C_STD) $(DEBUG) $(foreach D,$(INC),-I./$(D)/) $(OPT)

TARGET= Main
LIBFILES=

all: $(BIN)/$(TARGET)

lib: $(LIBS)/$(LIBFILES)


setup:
	mkdir $(SRC) $(OBJ) $(BIN) $(LIBS) $(INC) $(DEP)

clean_all:clean clean_libs

clean:
	rm -f $(OBJ)/* $(BIN)/* $(DEP)/*

clean_libs:
	rm -f $(LIBS)/*
# -l m znaci linkuj math,h
#rule se treba zvati ko path file da moze prepoznati dal postoji
# $@ ono sto je sa desne starne tj target





-include ./$(DEP)/*.d  #Ovo je da se povezu dependenciji

$(OBJ)/%.o: $(SRC)/%.c
	$(COMP) $(FLAGS) -c  $< -o $@ $(DEPFLAGS) $(subst $(OBJ),$(DEP),$(@:.o=.d))

$(LIBS)/%.a: $(OBJ)/%.o
	ar src $@ $^

.PRECIOUS: ./$(OBJ)/%.o
#Ovo je da sacuva o. fajlove
# cwd is the one i run make from
$(BIN)/$(TARGET): $(OBJ)/wavheader.o $(OBJ)/samplewindow.o $(OBJ)/polynomial.o $(OBJ)/decentralizedBuuffer.o $(OBJ)/sampleBuffer.o $(OBJ)/matrix.o $(OBJ)/$(TARGET).o 
	$(COMP) $(FLAGS)  $^ -o $@ -lm