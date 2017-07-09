#ifndef MIPS_STRUCT_H_INCLUDED
#define MIPS_STRUCT_H_INCLUDED
#include "mips_mem.h"

struct mips_cpu_impl
{
    // GPRs and Program Counters
    uint32_t pc ;
     uint32_t counter ;
     int32_t offset ;
    uint32_t reg[32];
    uint32_t jump_target_address ;

    uint32_t HI_reg ;
    uint32_t LO_reg ;

    bool was_a_jump ;
    bool was_a_rtype_jump ;

    uint32_t jalr_address ;

    //bool was_a_conditional_branch ;

    mips_mem_h mem ;

    unsigned logLevel; // used for debugging.
    FILE *logDst;
};

typedef struct mips_cpu_impl* mips_cpu_h ;
#endif // MIPS_STRUCT_H_INCLUDED
