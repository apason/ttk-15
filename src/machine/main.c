#include <stdio.h>
#include <string.h>

//project headers
#include <ttk-15.h>
#include "machine.h"

int main(int argc,char *argv[]){
  options *opts = getOptions(argc, argv);
  machine *m    = newMachine(opts->memsize);

  m->cu->pc = 0;

  if(opts == NULL){
    freeMachine(m);
    return -1;
  }

  if(opts->mode == TTK91)
    m->regs[6] = (MYTYPE) loadFile91(m->mem, opts->file);
  else if(opts->mode == TTK15)
    m->regs[6] = (MYTYPE) loadFile(m->mem, opts->file);
       
  m->regs[7] = m->regs[6];
  
  startMachine(m, opts->debug);
  freeMachine(m);
  freeOptions(opts);

  printf("\n");

  return 0;
}
