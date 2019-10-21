GCC = gcc
CFLAGS = -std=c99
LDFLAGS= -pthread -lpthread

.PHONY: clean

all:tp1

tp1: utils.c
	$(GCC) $(LDFLAGS) $(CFLAGS) TP1.c -o TP1

clean:
	rm -f TP1
	rm -f utils

test:tp1
	./TP1
