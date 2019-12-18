GCC = gcc
CFLAGS = -std=gnu99
LDFLAGS= -pthread -lpthread

.PHONY: all clean test

all:tp2

tp2: utils.c
	$(GCC) $(LDFLAGS) $(CFLAGS) TP2.c -o TP2
	cp TP2 valider

clean:
	rm -f TP2
	rm -f valider
	rm -f *.o

test:tp2
	./TP2
