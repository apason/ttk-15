#ifndef CU_H
#define CU_H

#include <ttk-15.h>

#ifndef int_i_macro
#define int_i_macro
static int    _i;
#endif

#ifndef MYTYPE_tmp_macro
#define MYTYPE_tmp_macro
static MYTYPE _tmp;
#endif

#ifndef MYTYPE_result_macro
#define MYTYPE_result_macro
static MYTYPE _result;
#endif

/*
 * ri=0 means no index register used 
 * so this function first saves r0, 
 * puts 0 to it and recovers when done
 */

#define calculateSecondOperand(M, MODE, RI, ADDR, TR) \
    _result = (MYTYPE) ADDR;			      \
    _tmp = M->regs[0];             /* save r0     */  \
    M->regs[0] = 0;                /* set r0 to 0 */  \
    _result += M->regs[ri];                           \
                                                      \
    /*fetch as many times as needed */                \
    for(_i = 0; _i < MODE; _i++){                     \
	mmuGetData(M->mmu, M->mem, _result);          \
	_result = M->mmu->mbr;                        \
    }                                                 \
			         		      \
    M->regs[0] = _tmp;             /* restore r0  */  \
    TR = _result;
    

#endif
