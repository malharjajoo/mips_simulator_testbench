#include "Itype_instructions.h"

void fetch_I_Type(uint32_t instruction,uint32_t* index_src_reg_1,
                    uint32_t* index_dest_reg,uint32_t* immediate)
{

     *index_src_reg_1 = (instruction>> 21 ) & 0x1F;
     *index_dest_reg = (instruction>> 16 ) & 0x1F;

    // This does the work of zero-extending.

     *immediate  = (instruction>> 0) & 0xFFFF; //

}

uint32_t sign_extend_immediate(uint32_t immediate)
{
    if((immediate>>15 )& 0x1 )
    {
        cout << endl  << " This is going to need sign-extension ! " << endl ;
       // since the 15th(16th actually)bit is set , we need to sign extend.
       uint32_t sign_extended_immediate = (0xFFFF0000 | immediate ) ;
       return sign_extended_immediate ;
    }

    else
    { // no need to sign extend.
        return immediate ;
    }
}
