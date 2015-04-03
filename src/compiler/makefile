CC= 	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  compiler.o countsize.o file_operations.o
INCLUDE=  -I "../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c
TARGET=   ../../target/

.c.o:
	$(CC) $(CFLAGS) $<

all:	compiler

compiler:	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET)compiler

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)compiler
