
all: emulator

emulator.o: emulator.asm a.out.b15
	nasm emulator.asm -o emulator.o -f elf64
emulator: emulator.o
	ld emulator.o -o emulator -s
a.out.b15: test.k15
	ttc test.k15
