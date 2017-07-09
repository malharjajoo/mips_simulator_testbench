#ifndef TEST_MIPS_EXTENSION_H_INCLUDED
#define TEST_MIPS_EXTENSION_H_INCLUDED

#include "mips_core.h"
#include<iostream>
using namespace std ;
#ifdef __cplusplus
extern "C"{
#endif
// simple function to test for errors.
void check_error(mips_error err) ;

void printPassOrFail(int testId,bool pass ,string message) ;

#ifdef __cplusplus
};
#endif

#endif // TEST_MIPS_EXTENSION_H_INCLUDED
