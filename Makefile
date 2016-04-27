CXX ?= g++
CFLAGS = -O3 -std=c++14 -Wall -Wfatal-errors
RM ?= rm

all: cds

cds: cds.cpp lodepng.o
	$(CXX) $(CFLAGS) -o cds cds.cpp lodepng.o

lodepng.o: lodepng.cpp lodepng.h
	$(CXX) $(CFLAGS) -c -o lodepng.o lodepng.cpp

clean:
	$(RM) -f cds lodepng.o
