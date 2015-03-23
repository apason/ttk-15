CC= 	gcc
STD=	_GNU_SOURCE_
OBJS=	main.o bitwise.o cu.o helpers.o instructions.o machine.o mmu.o loader.c

.c.o:
	$(CC) -c -Wall $(CFLAGS) -D$(STD) $<

all:	ttk-91

ttk-91:	$(OBJS)
	$(CC) $(OBJS) -o ttk-91

clean:
	rm -f *.o

clobber:	clean
	rm -f ttk-91
