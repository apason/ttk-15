#include <stdio.h>

//project header
#include <ttk-91.h>

int main(int argc,char *argv[]){
  machine *m = newMachine(65536 /*64k system*/);

  initializeGlobals();
  m->cu->pc = 0;
  m->regs[6] = (MYTYPE)loadFile(m->mem, argv[1]);
  m->regs[7] = m->regs[6];
  
  startMachine(m);

  return 0;
}
