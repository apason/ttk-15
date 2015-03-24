CC= 	gcc
STD=	_GNU_SOURCE_
OBJS=	main.o bitwise.o cu.o helpers.o instructions.o machine.o mmu.o loader.c

.c.o:
	$(CC) -c -Wall $(CFLAGS) -D$(STD) $<

all:	ttk-15

ttk-15:	$(OBJS)
	$(CC) $(OBJS) -o ttk-15

clean:
	rm -f *.o

clobber:	clean
	rm -f ttk-15
