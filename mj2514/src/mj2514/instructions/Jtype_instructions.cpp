#include "Jtype_instructions.h"

void fetch_J_Type(uint32_t instruction,uint32_t* instruction_index)

{

    // NOTE- this is zero - extension.
     *instruction_index = (instruction) & 0x3FFFFFF;
}
