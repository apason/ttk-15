CC= 	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  main.o linking.o modulecreation.o memory.o options.o error.o
INCLUDE=  -I "../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c -Wno-unused-function -g -Wno-varargs
TARGET=   ../../target/

.c.o:
	$(CC) $(CFLAGS) $<

all:	linker

linker:		$(OBJS)
		$(CC) $(OBJS) -o $(TARGET)linker

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)linker
