#include <stdio.h>
#include <string.h>

//project header
#include <ttk-15.h>

int main(int argc,char *argv[]){
  options *opts = getOptions(argc, argv);
  machine *m    = newMachine(65536);

  initializeGlobals();
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
  //freeopts

  printf("\n");

  return 0;
}
