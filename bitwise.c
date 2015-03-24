#include "ttk-91.h"

//lengths of bit fields
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

//extract first byte
uint8_t extractOpcode(MYTYPE x){
  x = x >> (mtl - OPCODE_L);
  uint8_t opc = (uint8_t)x & 0xFF;       //bit pattern for 8 lsb set
  return opc;
}

//extract last 2 bytes
int16_t extractAddress(MYTYPE x){
  uint16_t addr = (uint16_t)x & 0xFFFF;  //bit pattern for 16 lsb set
  return addr;
}

//extract bytes in [9,11] 
uint8_t extractRj(MYTYPE x){
  x = x >> (mtl - OPCODE_L - REG_L);
  uint8_t rj = (uint8_t)x & 0x7;         //bit pattern for 3 lsb set
  return rj;
}

//extract bytes in [14,16]
uint8_t extractRi(MYTYPE x){
  x = x >> (mtl - OPCODE_L - REG_L - MODE_L - REG_L);
  uint8_t ri=(uint8_t)x & 0x7;           //bit pattern for 3 lsb set
  return ri;
}

//extraxt bytes in [12,13]
uint8_t extractMode(MYTYPE x){
  x = x >> (mtl - OPCODE_L - REG_L - MODE_L);
  uint8_t mode = (uint8_t)x & 0x3;      //bit pattern for 2 lsb set
  return mode;
}
