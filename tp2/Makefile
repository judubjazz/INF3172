GCC = gcc
CFLAGS = -std=gnu99
LDFLAGS= -pthread -lpthread

.PHONY: clean

all:tp2

tp2: utils.c validator
	$(GCC) $(LDFLAGS) $(CFLAGS) TP2.c -o TP2

validator:
	$(GCC) $(LDFLAGS) $(CFLAGS) validator.c -o validator

clean:
	rm -f TP2
	rm -f validator
	rm -f *.o
	rm -f replica.txt
	rm -f resultat.txt


test:tp2
	./TP2
