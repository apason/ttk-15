CC=       gcc
STD=      _GNU_SOURCE_
OBJS=     converter.o
INCLUDE=  -I "../../../src/include/"
EXTERN=   ../../../src/machine/loader.o
CFLAGS=   $(LIBRARY) -Wall -D$(STD) $(INCLUDE) -c
TARGET=   ../../target/

.c.o:
	$(CC) $(CFLAGS) $<

all:	converter

converter:	$(OBJS)
	$(CC) $(OBJS) $(EXTERN)  -o $(TARGET)converter
	
clean:
	rm *.o

clobber:	clean
	rm $(TARGET)converter

