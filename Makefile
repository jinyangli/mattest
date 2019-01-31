EIGEN=/usr/local/include/eigen3
IGL=/home/jinyang/co/libigl/include/igl

CFLAGS=-c -Wall -I$(EIGEN) -I$(IGL) -g -Ofast
#CFLAGS=-c -Wall -I$(EIGEN) -I$(IGL) -g -Og
#CFLAGS=-c -Wall -I$(EIGEN) -I$(IGL) -g -O0
CC=g++

all: simple 

%.o: %.cpp
	$(CC) $(CFLAGS) $<
 
simple: simple.o
	$(CC) $^ -o $@

clean:
	rm -f *.o
