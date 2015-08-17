CC=	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  main.o utilities.o instructions.o machine.o loader.o options.o ncurses.o
INCLUDE=  -I "../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c -Werror -Wno-unused-function -g
TARGET=   ../../target/

.c.o:
	$(CC) $(CFLAGS) $<

all:	machine

machine:	$(OBJS)
		$(CC) $(OBJS) -o $(TARGET)ttk-15 -lncurses

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)ttk-15
