#ifndef MIPS_STRUCT_H_INCLUDED
#define MIPS_STRUCT_H_INCLUDED
#include "mips_mem.h"

// This implementation is provided as a handle in the mips_cpu.h file 
// for use by a client.
struct mips_cpu_impl
{
    // GPRs and Program Counter (PC)
    uint32_t pc ;
     uint32_t counter ;
     int32_t offset ;
    uint32_t reg[32];	// an array to represent register file
    uint32_t jump_target_address ;

    uint32_t HI_reg ;
    uint32_t LO_reg ;

    bool was_a_jump ;
    bool was_a_rtype_jump ;

    uint32_t jalr_address ;

    //bool was_a_conditional_branch ;

   // CPU contains a handle to memory
    mips_mem_h mem ;

    unsigned logLevel; // used for debugging.
    FILE *logDst;
};

typedef struct mips_cpu_impl* mips_cpu_h ;
#endif 
