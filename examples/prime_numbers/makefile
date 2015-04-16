TARGET=../../target/
CC=$(TARGET)ttc
LD=$(TARGET)linker
SOURCES=$(wildcard *.k15)
OBJS=$(SOURCES:.k15=.o15) 
TARGET=prime_numbers.b15


all:	$(OBJS)
	$(LD) $(OBJS) -o $(TARGET)

%.o15: %.k15
	$(CC) $< -o $@

clean:
	rm *.o15

