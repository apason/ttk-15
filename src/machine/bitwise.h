//project headers
#include <ttk-15.h>
#include "machine.h"

#define OPCODE_L  8
#define ADDR_L    16
#define REG_L     3
#define MODE_L    2

/* 
 * mtl stands for MyType Length. Its number of bytes in MYTYPE 
 * and is used in bitshifts. e.g. to read bits in position [15,16]
 * it is needed to shift bits 16 times to right: x >> (mtl - 16)
 *
 */

static MYTYPE _tmp;                         //for handling instruction
static int mtl = sizeof(MYTYPE) * 8;        //size of mytype in bits

//extract first byte
#define extractOpcode(INS, X)                              \
  _tmp = INS >> (mtl - OPCODE_L);                          \
  X   = (uint8_t) _tmp & 0xFF;              //bit pattern for 8 lsb set

//extract address
#define extractAddress(INS, X)                            \
  X = (uint16_t) INS & 0xFFFF;              //bit pattern for 16 lsb set

//extract bytes in [9,11] 
#define extractRj(INS, X)                                  \
  _tmp = INS >> (mtl - OPCODE_L - REG_L);                  \
  X   = (uint8_t) _tmp & 0x7;               //bit pattern for 3 lsb set
 
//extract bytes in [14,16]
#define extractRi(INS, X)                                  \
  _tmp = INS >> (mtl - OPCODE_L - REG_L - MODE_L - REG_L); \
  X   = (uint8_t) _tmp & 0x7;               //bit pattern for 3 lsb set  

//extraxt bytes in [12,13]
#define extractMode(INS, X)                                \
  _tmp = INS >> (mtl - OPCODE_L - REG_L - MODE_L);         \
  X   = (uint8_t) _tmp & 0x3;               //bit pattern for 2 lsb set

