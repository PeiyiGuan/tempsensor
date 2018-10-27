CC=gcc
CFLAGS= -Wall -pedantic  
test: test.c libmyifttt.a
	$(CC) test.c -L. -lmyifttt -lwiringPi -lcurl -o test

libmyifttt.a:	ifttt.o
	ar -rcs libmyifttt.a ifttt.o

ifttt.o: 	ifttt.c ifttt.h
	$(CC) $(CFLAGS) -c -ansi $<

clean:
	rm test *.o
