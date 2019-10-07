GCC = gcc
CFLAGS = -std=c99
LDFLAGS= -pthread -lpthread

.PHONY: clean

all:tp1

tp1: TP1.c
	$(CC) $(LDFLAGS) $(CFLAGS) TP1.c -o TP1

clean:
	rm -f TP1

check:
	bats test.bats


