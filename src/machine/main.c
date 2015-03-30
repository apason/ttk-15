#include <stdio.h>
#include <string.h>

//project header
#include <ttk-15.h>

int main(int argc,char *argv[]){
  machine *m = newMachine(65536 /*64k system*/);

  initializeGlobals();
  m->cu->pc = 0;

  if(argc >= 3){
    if(!strncmp(argv[1], "-91", 3))
       m->regs[6] = (MYTYPE)loadFile91(m->mem, argv[2]);
  }
  else
    m->regs[6] = (MYTYPE)loadFile(m->mem, argv[1]);
       
  m->regs[7] = m->regs[6];
  
  startMachine(m);

  return 0;
}
