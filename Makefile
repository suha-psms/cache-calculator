
#
# Makefile for CSF Assignment 3
#
#
#

# compiler to use
CC = g++

# compiler flags:
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu++11

#SRCS and OBJS
SRCS = csim.cpp csim.h main.cpp
OBJS = $(SRCS:%.cpp=%.o)

# makes the csim executable
csim : $(OBJS)
	$(CC) -o $@ $(OBJS) 

# remove .o files and exectuable
clean:
	rm -f *.o csim

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak