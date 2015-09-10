
static char *dis_asm[255] = {
    "nop", "store", "load", "in", "out",\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    "add", "sub", "mul", "div", "mod",\
    "and", "or", "xor", "shl", "shr", "not", "shra", "comp",\
    "jump", "jneg", "jzer", "jpos", "jnneg", "jnzer", "jnpos",\
    "jles", "jequ", "jgre", NULL, "jnles", "jnequ", "jngre",\
    NULL, NULL, NULL, NULL,\
    "call", "exit", "push", "pop", "pushr", "popr",\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  "svc",\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,\
    "fload", "fin", "fout",\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "fadd", "fsub", "fmul", "fdiv",\
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "fcomp",\
    NULL, "fjneg", "fjzer", "fjpos", "fjnneg", "fjnzer", "fjnpos"

};
