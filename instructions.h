#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#define NOP    0x0

#define STORE  0x1
#define LOAD   0x2
#define IN     0x3
#define OUT    0x4

#define ADD    0x11
#define SUB    0x12
#define MUL    0x13
#define DIV    0x14
#define MOD    0x15

#define AND    0x16
#define OR     0x17
#define XOR    0x18
#define SHL    0x19
#define SHR    0x1A
#define NOT    0x1B
#define SHRA   0x1C

#define COMP   0x1F

#define JUMP   0x20
#define JNEG   0x21
#define JZER   0x22
#define JPOS   0x23
#define JNNEG  0x24
#define JNZER  0x25
#define JNPOS  0x26

#define JLES   0x27
#define JEQU   0x28
#define JGRE   0x29
#define JNLES  0x2A
#define JNEQU  0x2B
#define JNGRE  0x2C

#define CALL   0x31
#define EXIT   0x32
#define PUSH   0x33
#define POP    0x34
#define PUSHR  0x35
#define POPR   0x36

#define SVC    0x70

#endif

