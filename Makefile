EIGEN=/usr/local/include/eigen3
IGL=/home/jinyang/co/libigl/include/igl

CFLAGS=-c -Wall -I$(EIGEN) -I$(IGL) -g -Ofast
#CFLAGS=-c -Wall -I$(EIGEN) -I$(IGL) -g -Og
#CFLAGS=-c -Wall -I$(EIGEN) -I$(IGL) -g -O0
CC=g++

all: simple  codegen

%.o: %.cpp
	$(CC) $(CFLAGS) $<
 
simple: simple.o util.o mymatmul.o
	$(CC) $^ -o $@

codegen: codegen.o util.o
	$(CC) $^ -o $@

mymatmul.cpp: codegen
	./codegen g > $@

mymatmul.o: mymatmul.cpp
	$(CC) -c -Ofast $<

clean:
	rm -f *.o mymatmul.cpp
