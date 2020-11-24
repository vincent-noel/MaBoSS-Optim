#
# MaBoSS project Makefile to compile parameter fitting
#
# Institut Curie, France
# Vincent Noël

CC = g++
FLAGS = -Wall -Werror -std=c++11

SRC_DIR = src

MABOSS_SRC_DIR = src/MaBoSS-env-2.0/engine/src
MABOSS_INCLUDE_DIR = src/MaBoSS-env-2.0/engine/include
MABOSS_LIB_DIR = src/MaBoSS-env-2.0/engine/lib

PLSA_SRC_DIR = src/plsa/src
PLSA_DIR = src/plsa

all: clean MaBoSS-Optim

$(MABOSS_LIB_DIR)/libMaBoSS.a:
	make -C $(MABOSS_SRC_DIR) install_alib

$(MABOSS_INCLUDE_DIR)/MaBEstEngine.h:
	make -C $(MABOSS_SRC_DIR) install_alib

$(PLSA_DIR)/libplsa-serial.so: 
	make -C $(PLSA_DIR) libplsa-serial.so

main.o: $(SRC_DIR)/main.c $(MABOSS_INCLUDE_DIR)/MaBEstEngine.h
	$(CC) $(FLAGS) -c $(SRC_DIR)/main.c -I$(MABOSS_INCLUDE_DIR) -I$(PLSA_SRC_DIR) -o main.o

PSetSimulation.o: $(SRC_DIR)/PSetSimulation.h $(SRC_DIR)/PSetSimulation.cc 
	$(CC) $(FLAGS) -c $(SRC_DIR)/PSetSimulation.cc -I$(MABOSS_INCLUDE_DIR) -o PSetSimulation.o

Optimization.o: $(SRC_DIR)/Optimization.h $(SRC_DIR)/Optimization.cc
	$(CC) $(FLAGS) -c $(SRC_DIR)/Optimization.cc -I$(MABOSS_INCLUDE_DIR) -I$(PLSA_SRC_DIR) -o Optimization.o

MaBoSS-Optim: main.o PSetSimulation.o Optimization.o $(MABOSS_LIB_DIR)/libMaBoSS.a $(PLSA_DIR)/libplsa-serial.so
	$(CC) $(FLAGS) main.o PSetSimulation.o Optimization.o $(MABOSS_LIB_DIR)/libMaBoSS.a $(PLSA_DIR)/libplsa-serial.so -lm -ljsoncpp -lpthread -ldl -o MaBoSS-Optim
	rm -f *.o

# Test
test:
	make
	./MaBoSS-Optim -c examples/Four_cycle.cfg -s examples/settings.json -o tests/result.json examples/Four_cycle.bnd
	python -m unittest tests.test_results
	rm -f tests/result.json

clean: 
	rm -f MaBoSS-Optim main.o PSetSimulation.o Optimization.o $(MABOSS_LIB_DIR)/libmaboss.so $(MABOSS_INCLUDE_DIR)/*.h plsa.log plsa.state
