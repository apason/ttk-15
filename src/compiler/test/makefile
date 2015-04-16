CC= 	  gcc
STD=	  _GNU_SOURCE_
OBJS=	  main.o utility_test.o 
INCLUDE=  -I "../../include/"
CFLAGS=   -Wall -D$(STD) $(INCLUDE) -c

.c.o:
	$(CC) $(CFLAGS) $<

all:	 utility utility_test

utility_test:	$(OBJS)
		$(CC) $(OBJS) utility.o -o test

clean:
	rm -f *.o

utility:
	$(CC) $(CFLAGS) ../utility.c -o utility.o
