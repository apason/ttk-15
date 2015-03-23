#include "ttk-91.h"
uint8_t extractOpcode(MYTYPE x){
  x=x>>(mtl-8);
  uint8_t opc=(uint8_t)x&0xFF; //bit pattern for 8 lsb set
  return opc;
}

int16_t extractAddress(MYTYPE x){
  uint16_t addr=(uint16_t)x&0xFFFF; //bit pattern for 16 lsb set
  return addr;
}

uint8_t extractRj(MYTYPE x){
  x=x>>(mtl-8-3);
  uint8_t rj=(uint8_t)x&0x7; //bit pattern for 3 lsb set
  return rj;
}

uint8_t extractRi(MYTYPE x){
  x=x>>(mtl-8-3-2-3);
  uint8_t ri=(uint8_t)x&0x7; //bit pattern for 3 lsb set
  return ri;
}

uint8_t extractMode(MYTYPE x){
  x=x>>(mtl-8-3-2);
  uint8_t mode=(uint8_t)x&0x3; //bit pattern for 2 lsb set
  return mode;
}
