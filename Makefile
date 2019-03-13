########################################
# Rowan MacLachlan # rdm695
# 11165820
# March 18th 2019 
# CMPT 434 Eager # Makefile

########################################
# OS and architecture macros
CURR_OS := $(shell uname -s)
ARCH := $(shell uname -m)
MAC_OS="Darwin"
LINUX_OS="Linux"
########################################
TEST = test_distance test_messages test_common
EXEC = sensor logger
TARGET = $(EXEC)
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

all: $(TARGET) $(TEST) tests

COM_OBJ=$(OBJ)distance.o $(OBJ)messages.o $(OBJ)common.o
COM_INC=$(INC)distance.h $(INC)messages.h $(INC)common.h

tests:
	./test_common
	./test_messages
	./test_distance

# SENSOR NODE
sensor: $(OBJ)sensor.o $(OBJ)sensor_aux.o $(COM_OBJ) 
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)sensor.o: $(SRC)sensor.c $(INC)sensor_aux.h $(COM_INC) 
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

$(OBJ)sensor_aux.o: $(SRC)sensor_aux.c $(INC)sensor_aux.h $(COM_INC) 
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

# LOGGER 
logger: $(OBJ)logger.o $(COM_OBJ) 
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)logger.o: $(SRC)logger.c $(COM_INC) 
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

# COMMON
test_common: $(OBJ)common_test.o $(OBJ)common.o
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)common_test.o: $(SRC)common_test.c $(SRC)common.c $(INC)common.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

$(OBJ)common.o: $(SRC)common.c $(INC)common.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

# DISTANCE
test_distance: $(OBJ)distance_test.o $(OBJ)distance.o
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)distance_test.o: $(SRC)distance_test.c $(SRC)distance.c $(INC)distance.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

$(OBJ)distance.o: $(SRC)distance.c $(INC)distance.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

# MESSAGES 
test_messages: $(OBJ)messages_test.o $(OBJ)messages.o $(OBJ)common.o
	$(CC) $(C_FLAGS) $(INC_FLAGS) $^ -o $@

$(OBJ)messages_test.o: $(SRC)messages_test.c $(SRC)messages.c $(INC)messages.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

$(OBJ)messages.o: $(SRC)messages.c $(INC)messages.h
	$(CC) $(C_FLAGS) -c $(INC_FLAGS) $< -o $@

clean:
	rm -f $(OBJ)*
	rmdir $(OBJ)
	rm -f $(EXEC) $(TEST)
