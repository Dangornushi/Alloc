CFLAGS=-Wall -std=c++20 -g
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.c=.o)

Alloc: $(OBJS)
	g++ -std=c++20 -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): Alloc.h


.PHONY: Alloc 
