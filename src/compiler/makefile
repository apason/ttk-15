CC= 	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  compiler.o module.o file_operations.o utility.o options.o
INCLUDE=  -I "../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c
TARGET=   ../../target/

.c.o:
	$(CC) $(CFLAGS) $<

all:	compiler

compiler:	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET)ttc

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)ttc
