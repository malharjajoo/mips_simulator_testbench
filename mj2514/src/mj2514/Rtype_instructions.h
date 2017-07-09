#ifndef RTYPE_INSTRUCTIONS_H_INCLUDED
#define RTYPE_INSTRUCTIONS_H_INCLUDED

#include "mips_struct.h"
#include "mips_cpu.h"



void fetch_R_Type(uint32_t instruction,uint32_t* index_src_reg_1 ,
                  uint32_t* index_src_reg_2 ,  uint32_t* index_dest_reg,
                   uint32_t* shift ,uint32_t* functioncode) ;


mips_error ADD(mips_cpu_h state ,uint32_t index_src_reg_1,uint32_t index_src_reg_2,uint32_t index_dest_reg,
               uint32_t functioncode);

mips_error bitwise_AND(mips_cpu_h state ,uint32_t index_src_reg_1,uint32_t index_src_reg_2_or_immediate,
                 uint32_t index_dest_reg , uint32_t distinguishing_code) ;

void signed_overflow( uint32_t v1 , uint32_t v2 , bool* signed_overflow  ) ;
#endif // RTYPE_INSTRUCTIONS_H_INCLUDED

