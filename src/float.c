#include <stdint.h>
#include <stdio.h>

f16 f16Encode(float f){

    int32_t n = *(volatile int32_t*) &f;
    
    int32_t sign = (n & 0x80000000);
    int32_t exp  = (n & 0x7f800000) >> 23;
    int32_t man  = (n & 0x7fffff);

    if(exp -127 > 16 || exp -127 < -16) printf("exponent too big!\n");
    if(man & 0x1fff)  printf("mantissa too big!\n");

    return (f16) ((((sign >> 16) | ( (exp -127 +15) << 10)) & ~0x3ff) | (man >> 13));
}

float f16Decode(f16 f){
    
    int32_t ret;
    int32_t sign = (f & 0x8000);
    int32_t exp  = (f & 0x7c00) >> 10;
    int32_t man  = (f & 0x3ff );

    ret = ((sign << 16) & 0x80000000) | (((exp -15 +127) << 23) & 0xff800000) | ((man << 13) & 0xffffe000);

    return *(volatile float*) &ret;
}

