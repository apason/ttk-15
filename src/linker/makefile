CC= 	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  main.o linking.o modulecreation.o memory.o options.o
INCLUDE=  -I "../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c -Werror -Wno-unused-function
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
