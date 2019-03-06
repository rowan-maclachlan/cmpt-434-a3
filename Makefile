########################################
# Rowan MacLachlan
# rdm695
# 11165820
# March 18th 2019 
# CMPT 434 Eager 
# Makefile

########################################
# OS and architecture macros
CURR_OS := $(shell uname -s)
ARCH := $(shell uname -m)
MAC_OS="Darwin"
LINUX_OS="Linux"
########################################
TARGET = test_distance test_messages
########################################
# directories
OBJ = ./obj/
INC = ./include/
SRC = ./src/
$(shell mkdir -p $(OBJ))
########################################
# compiler and linker options
CC = gcc 
AR_OPTIONS = cr
C_FLAGS = -g -Wall -pedantic
INC_FLAGS = -I$(INC)
########################################
# filename macros
########################################
# recipes
.PHONY: all clean

all: $(TARGET)

# DISTANCE
test_distance: $(OBJ)distance_test.o $(OBJ)distance.o
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)distance_test.o: $(SRC)distance_test.c $(SRC)distance.c $(INC)distance.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

$(OBJ)distance.o: $(SRC)distance.c $(INC)distance.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

# MESSAGES 
test_messages: $(OBJ)messages_test.o $(OBJ)messages.o
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)messages_test.o: $(SRC)messages_test.c $(SRC)messages.c $(INC)messages.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

$(OBJ)messages.o: $(SRC)messages.c $(INC)messages.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

clean:
	rm -f $(OBJ)*
	rmdir $(OBJ)
	rm -f $(EXEC) test
