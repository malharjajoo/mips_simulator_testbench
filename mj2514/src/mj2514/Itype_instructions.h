#ifndef ITYPE_INSTRUCTIONS_H_INCLUDED
#define ITYPE_INSTRUCTIONS_H_INCLUDED

#include "mips_core.h"

using namespace std ;
#include <iostream>

void fetch_I_Type(uint32_t instruction,uint32_t* index_src_reg_1 ,
                    uint32_t* index_dest_reg,uint32_t* immediate) ;

uint32_t sign_extend_immediate(uint32_t immediate) ;

#endif // ITYPE_INSTRUCTIONS_H_INCLUDED
