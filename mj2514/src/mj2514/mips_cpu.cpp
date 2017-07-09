
/* This file is an implementation of the functions
   defined in mips_cpu.h
 */

#include <iostream>
#include <stdio.h> // for fprintf.


#include "Rtype_instructions.h"
#include "Itype_instructions.h"
#include "Jtype_instructions.h"


#ifdef __cplusplus
extern "C"{
#endif


//============================ EXTRA FUNCTIONS ADDED BY ME =========================

uint32_t to_big(const uint8_t *pData) ;


mips_error increment_pc(mips_cpu_h state) ;


// =======================================================================================

/*! Represents the STATE of a cpu-
	This is another opaque data type, similar to \ref mips_mem_provider.
*/ // In the mips_struct.h file.

//===============================================================================================

/*! Creates and initialises a new CPU instance.

	The CPU should be bound to the given
	\ref mips_mem_core "memory space", and have all registers set
	to zero. The memory is NOT owned by the CPU, so it should not
	be \ref mips_mem_free "freed" when the CPU is \ref mips_cpu_free "freed".

	\param mem The memory space the processor is connected to; think of it
	as the address bus to which the CPU has been wired.
*/
mips_cpu_h  mips_cpu_create(mips_mem_h mem)
{
    mips_cpu_impl*  mips_cpu_instance = ((mips_cpu_impl*)malloc(sizeof(mips_cpu_impl))) ;

    mips_cpu_instance->pc= 100 ;
//================NEW ===============================================================================
    mips_cpu_instance->counter = 0 ;  // This is a counter to help in delaying the branch.
    mips_cpu_instance->offset = 0 ;  // NOTICE-  that this has been included as part of the cpu state !
                                    //          ( We didnt use static variable inside function - think why !
                                    //            If 2 instances of cpu state then ? it will intergfere .... )
    mips_cpu_instance->HI_reg = 0 ;
    mips_cpu_instance->LO_reg = 0 ;

    mips_cpu_instance->was_a_jump = false ;
    mips_cpu_instance->was_a_rtype_jump = false ;

    mips_cpu_instance->jump_target_address = 0;
    mips_cpu_instance->jalr_address= 0 ;


//================================================================================================
     mips_cpu_instance->logLevel = 2; // used for debugging.
    mips_cpu_instance->logDst = stderr ;

    // set all registers to zero.
    for(int i = 0; i<32 ; i++)
    {
        mips_cpu_instance->reg[i] = 0 ;
    }

    // bind cpu and memory
    mips_cpu_instance -> mem = mem  ;

    return mips_cpu_instance ;
}

//===============================================================================================

/*! Reset the CPU as if it had just been created, with all registers zeroed.
	However, it should not modify RAM. Imagine this as asserting the reset
	input of the CPU core.
*/
mips_error mips_cpu_reset(mips_cpu_h state)
{
// COMPLETE THIS

     state->pc = 100 ;
     state->counter = 0 ;
     state->offset = 0 ;

     state->HI_reg = 0 ;
    state->LO_reg = 0 ;
    state->jalr_address = 0  ;

      state->was_a_jump = false ;
      state->was_a_rtype_jump = false ;
    state->jump_target_address = 0;

    for(int i = 0; i<32 ; i++)
    {
        state->reg[i] = 0 ;

    }

    state->logLevel = 2; // used for debugging.
    state->logDst = stderr ;

    return mips_Success ;
}

//===============================================================================================
/*! Returns the current value of one of the 32 general purpose MIPS registers */
mips_error mips_cpu_get_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t *value		//!< Where to write the value to
)
{
    if(state==0)
    {
        return  mips_ErrorInvalidHandle ;
    }

    if(index < 0 || index > 31)
    {
        return   mips_ErrorInvalidArgument;
    }

    *value = state->reg[index] ;

     if(index==0)
    {
        *value = 0 ;
    }

    return mips_Success ;
}

// ======================================================================================

/*! Modifies one of the 32 general purpose MIPS registers. */
mips_error mips_cpu_set_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t value		//!< New value to write into register file
)
{
    if(state==0)
    {
        return  mips_ErrorInvalidHandle ;
    }

    if(index < 0 || index > 31)
    {
        return  mips_ErrorInvalidArgument ;
    }

    // Ensures that reg 0 is never set by the testbench.
    if(index==0)
    {
        state->reg[0] = 0 ;
    }
    else if(index!=0)
    {
        state->reg[index] = value ;
    }


     return mips_Success ;
}



// =======================================================================================

/*! Sets the program counter for the next instruction to the specified byte address.

	While this sets the value of the PC, it should not cause any kind of
	execution to happen. Once you look at branches in detail, you will
	see that there is some slight ambiguity about this function. Choose the
	only option that makes sense.
*/
mips_error mips_cpu_set_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t pc			//!< Address of the next instruction to execute.
)
{
    if(state==0)
    {
        return  mips_ErrorInvalidHandle ;
    }

    if((pc%4)!=0)
    {
        return mips_ExceptionInvalidAddress ;
    }
    state->pc = pc ;
    return mips_Success;
}

// ======================================================================================

/*! Gets the pc for the next instruction.

	Returns the program counter for the next instruction to be executed.
*/
mips_error mips_cpu_get_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t *pc		//!< Where to write the byte address too
)
{

    if(state==0)
    {
        return  mips_ErrorInvalidHandle ;
    }

    *pc = state->pc ;
    return mips_Success ;
}

// ======================================================================================

/*! Advances the processor by one instruction. Call this ONLY AFTER SETTING PC.

	If an exception or error occurs, the CPU and memory state
	should be left unchanged. This is so that the user can
	inspect what happened and find out what went wrong. So
	this should be true:

		uint32_t pcOrig, pcGot;
		mips_cpu_get_pc(cpu, &pcOrig);
		mips_error err=mips_cpu_step(cpu);
		if(err!=mips_Success){
			mips_cpu_get_pc(cpu, &pcGot);
			assert(pcOrig==pcGot);
			assert(mips_cpu_step(cpu)==err);
	    }

	Maintaining this property in all cases is actually pretty
	difficult, so _try_ to maintain it, but don't worry too
	much if under some exceptions it doesn't quite work.
*/

mips_error mips_cpu_step(mips_cpu_h state) //! Valid (non-empty) handle to a CPU
{
    if(state == 0)
    {
        return mips_ErrorInvalidHandle ;
    }

    bool at_least_one_instruction = false;

    uint32_t length = 4 ;
    uint8_t buffer[4] ;

   // bool pc_increment_flag = false ; // this is so that we can place all pc_increment at the end of the fucntion.
                                    // We are trying to delay it ! BUT , we WONT going to delay "returns" if error is there!

// --------------------- - FETCH -----------------------------------------------------------------------
    // the address to start read transaction at is program counter. Hence set pc in test file.
    mips_error err =  mips_mem_read(state->mem,state->pc,length ,buffer ) ;

    if(err != 0)
    {
        return err ;
    }

    uint32_t instruction = to_big(buffer);

    uint32_t opcode,index_src_reg_1,index_src_reg_2,index_dest_reg ,shift ,functioncode ,immediate ,instruction_index  , effective_target_address ,  offset ;

    // NOTE - We have called a R Type function but we fetch the "GENERAL" OPCODE
    opcode =  (instruction>>26) & 0x3F;



// PUT INTO FUNCTION - DECODE

    //============================ R Type instruction  =========================================
    if(opcode==0)
    {
         fetch_R_Type( instruction,&index_src_reg_1 ,
                  &index_src_reg_2 , &index_dest_reg,
                  &shift ,&functioncode ) ;

            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "R-Type : dst=%u, src1=%u, src2=%u, shift=%u, function=%u.\n  instr=%08x\n,PC= %u",
                    index_dest_reg, index_src_reg_1, index_src_reg_2, shift, functioncode, instruction,state->pc);

                cout << endl ;
            }

            // 32 - ADD
            // 33 - ADDU
            if( ((functioncode == 32) || (functioncode == 33)) && (shift==0))
            {
                at_least_one_instruction = true ;
                err = ADD(state ,index_src_reg_1,index_src_reg_2,index_dest_reg,functioncode) ;

                if(err!= mips_Success)
                {
                    return err ; // this is determined by the test bench -check_error function.
                }
            }
            // functioncode =36 - AND.
            else if((functioncode==36) && (shift==0))
            {
                at_least_one_instruction = true ;
                cout << " I am in AND " << endl ;
                bitwise_AND(state ,index_src_reg_1,index_src_reg_2,index_dest_reg,functioncode ) ;


            }

            //  43 - SLTU
            // 42 - SLT
            else if( ((functioncode==43)&&(shift==0)) || ((functioncode==42)&&(shift==0)))
            {
                at_least_one_instruction = true ;
                // SLTU
                if(functioncode==43)
                {
                    state->reg[index_dest_reg] = ( (state->reg[index_src_reg_1]) < (state->reg[index_src_reg_2]) ) ;
                }

                // SLT
                 else if(functioncode==42)
                {
                    state->reg[index_dest_reg] = ( (int32_t(state->reg[index_src_reg_1])) < (int32_t(state->reg[index_src_reg_2]) )) ;
                }

            }

            // mult
            else if((functioncode==24) && ((index_dest_reg|shift) == 0) )
            {
                at_least_one_instruction = true ;
                int32_t a = int32_t(state->reg[index_src_reg_1 ]) ;
                int32_t b = int32_t(state->reg[index_src_reg_2]) ;

                int64_t c = int32_t(a*b) ;

                int32_t store_to_lo = int32_t(c & 0xFFFFFFFF) ;
                int32_t store_to_hi = int32_t( c & 0xFFFFFFFF00000000 ) ;

                 state->LO_reg = (store_to_lo) ;
                 state->HI_reg = (store_to_hi) ;
            }

             // mult
            else if((functioncode==25) && ((index_dest_reg|shift) == 0) )
            {
                at_least_one_instruction = true ;
                uint32_t a = uint32_t(state->reg[index_src_reg_1 ]) ;
                uint32_t b = uint32_t(state->reg[index_src_reg_2]) ;

                uint64_t c = uint32_t(a*b) ;

                uint32_t store_to_lo = uint32_t(c & 0xFFFFFFFF) ;
                uint32_t store_to_hi = uint32_t( c & 0xFFFFFFFF00000000 ) ;

                 state->LO_reg = (store_to_lo) ;
                 state->HI_reg = (store_to_hi) ;
            }

            else if (((functioncode==3)&&(index_src_reg_1==0)) || ((functioncode==7)&&(shift==0)) )
            {
                at_least_one_instruction = true ;

                if(functioncode==7)
                {   cout << " SRAV " <<endl ;
                    shift = (state->reg[index_src_reg_1] & 0x1F ) ; // low order 5 bits.
                }

                (state->reg[index_dest_reg]) = (int32_t(state->reg[index_src_reg_2]) >> shift ) ;
                cout << "SRA/SRAV output " << int32_t(state->reg[index_dest_reg]) << endl ;


            }

            // functioncode = 37 - OR.
            else if((functioncode == 37)&&(shift==0))
            {
                at_least_one_instruction = true ;
                cout << " I am in OR " << endl ;
               (state->reg[index_dest_reg]) = ((state->reg[index_src_reg_1]) | (state->reg[index_src_reg_2])) ;

            }

            // functioncode = 38 - X OR.
            else if((functioncode == 38)&&(shift==0))
            {
                at_least_one_instruction = true ;
                cout << " I am in XOR " << endl ;
               (state->reg[index_dest_reg]) = ((state->reg[index_src_reg_1]) ^ (state->reg[index_src_reg_2])) ;

            }
//&& (index_src_reg_1==0) && (index_src_reg_2==0) && (shift ==0))
             // functioncode = 16 - MFHI.
            else if((functioncode == 16) )
            {
                at_least_one_instruction = true ;
                cout << " I am in MFHI " << endl ;
                cout << " value going into dest reg is =" << (int32_t(state->HI_reg)) ;
               (state->reg[index_dest_reg]) = (int32_t(state->HI_reg));

            }

             // functioncode = 17 - MTHI.
            else if((functioncode == 17) )
            {
                at_least_one_instruction = true ;
                cout << " I am in MTHI " << endl ;
                cout << " value going into HI reg is =" << state->reg[index_src_reg_1] ;
               state->HI_reg =   (state->reg[index_src_reg_1]) ;

            }

            // functioncode = 18 - MFLO.
            else if((functioncode == 18) )
            {
                at_least_one_instruction = true ;
                cout << " I am in MFLO " << endl ;
                cout << " value going into dest reg is =" << (int32_t(state->LO_reg)) ;
               (state->reg[index_dest_reg]) = (int32_t(state->LO_reg)) ;

            }

            // functioncode = 19 - MTLO.
            else if((functioncode == 19))
            {
                at_least_one_instruction = true ;
                cout << " I am in MTLO " << endl ;
                cout << " value going into LO reg is =" << state->reg[index_src_reg_1];
                state->LO_reg =   (state->reg[index_src_reg_1]) ;

            }

            else if((functioncode==26) && (shift==0) && (index_dest_reg ==0) )
            {
                at_least_one_instruction = true ;
                cout << " I am in DIV" << endl ;
                if(state->reg[index_src_reg_2]!=0)
               {
                cout << "Value of src_reh_1="<< (int32_t(state->reg[index_src_reg_1]))<<endl ;
                cout << "Value of src_reh_2="<< (int32_t(state->reg[index_src_reg_2]))<<endl ;

                uint32_t lo = (int32_t(state->reg[index_src_reg_1])) / (int32_t(state->reg[index_src_reg_2])) ;
                state->LO_reg = int32_t(lo) ;
                cout << " Lo = " << int32_t(lo) << endl ;


                 uint32_t hi = ( (int32_t(state->reg[index_src_reg_1])) % (int32_t(state->reg[index_src_reg_2])) );
                 state->HI_reg = int32_t(hi) ;
                // cout << " Hi = " << int32_t(hi) << endl ;
               }
               cout <<endl << " DIV ends " << endl ;
            }

             else if((functioncode==27) && (shift==0) && (index_dest_reg==0))
            {
                at_least_one_instruction = true ;
                cout << " I am in DIVU " << endl ;

                if(state->reg[index_src_reg_2]!=0)
               {

                cout << "Value of src_reh_1="<< (uint32_t(state->reg[index_src_reg_1]))<<endl ;
                cout << "Value of src_reh_2="<< (uint32_t(state->reg[index_src_reg_2]))<<endl ;

                uint32_t lo = (uint32_t(state->reg[index_src_reg_1])) / (uint32_t(state->reg[index_src_reg_2])) ;
                state->LO_reg = lo ;
                cout << " Lo = " << lo << endl ;

                 uint32_t hi = (uint32_t(state->reg[index_src_reg_1])) % (uint32_t(state->reg[index_src_reg_2])) ;
                 state->HI_reg = hi ;
                 cout << " Hi = " << hi << endl ;
               }
            }

              // functioncode = 35 - SUBU.
              // functioncode = 34 - SUB
            else if(((functioncode == 35) || (functioncode == 34)) && (shift==0))
            {
                at_least_one_instruction = true ;
                cout << " I am in SUBU/SUB " << endl ;

                uint32_t a ;
                uint32_t b ;
                uint32_t c ;

                a =  (uint32_t(state->reg[index_src_reg_1]));
                b =  (uint32_t(state->reg[index_src_reg_2]));

                cout << "reg 1 in SUBU/SUB = " << a << endl ;
                cout << "reg 2 in SUBU/SUB = " << b << endl ;

               (state->reg[index_dest_reg]) = ( (uint32_t(state->reg[index_src_reg_1])) - (uint32_t(state->reg[index_src_reg_2]))) ;

                c =  (state->reg[index_dest_reg]);

               cout << "value of subu/sub result = " << c << endl ;

                        if(functioncode == 34)
                        {cout << "lolololo"<< endl;
                            bool sub_signed_overflow = false;

                             signed_overflow( b , c, &sub_signed_overflow ) ;

                             if(sub_signed_overflow)
                             {
                                 cout << "2SCOMPLEMENT OVERFLOW "<< endl ;
                                  return mips_ExceptionArithmeticOverflow ;
                             }
                        }


            }

            // SRL and SLL
            // SRLV and SLLV
            else if(((functioncode==2)&&(index_src_reg_1==0)) || ((functioncode == 6)&&(shift==0)) || ((index_src_reg_1==0)&&(functioncode == 0)) || ((functioncode == 4) && (shift==0)) )
            {
                at_least_one_instruction = true ;
                if((functioncode==6) || (functioncode==4))
                {

                    cout << " SRLv / SLLV " <<endl ;
                    shift = (state->reg[index_src_reg_1] & 0x1F ) ; // low order 5 bits.
                }

                 if((functioncode==2) || (functioncode == 6))
                 {
                     (state->reg[index_dest_reg]) = ((state->reg[index_src_reg_2]) >> shift ) ;
                 }

                else if((functioncode==0) || (functioncode == 4))
                 {
                     (state->reg[index_dest_reg]) = ((state->reg[index_src_reg_2]) << shift ) ;
                 }

                cout << "SRL/SRLV/SLL/SLLV output " << state->reg[index_dest_reg] << endl ;
            }



            else if( ((functioncode==9) && (shift==0) && (index_src_reg_2==0) )|| ((functioncode==8) && (index_src_reg_2==0) && (index_dest_reg==0) && (shift==0)  ))
            {
                at_least_one_instruction = true ;
                cout << " I am in JALR/JR " << endl ;

                cout << " counter has been incremented" << endl ;
                (state->counter) = (state->counter)+1 ;
                cout << " value of counter after incrementing is= " << state->counter << endl ;

                //cout << endl << " storing return address..........." << endl ;
                state->reg[index_dest_reg] = (state->pc)+8 ;

                //cout << "stroing pc ........ JALR " << endl ;
                state->jalr_address = state->reg[index_src_reg_1] ;
                //cout << "jalr address = " << state->jalr_address << endl ;

               state->was_a_rtype_jump = true;
                increment_pc(state) ;
                return mips_Success ;

            }



    }


//================================== I TYPE instructions =======================================
    // opcode = 8 - ADDI
    // opcode = 9 - ADDIU
    else if((opcode== 8) | (opcode == 9))
    {
        at_least_one_instruction = true ;
        // This is a ADDI or ADDIU Instruction
        fetch_I_Type(instruction,&index_src_reg_1 ,
                    &index_dest_reg,&immediate) ;


            // debugging
            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                    index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);

                cout << endl ;
            }


        uint32_t sign_extended_version = sign_extend_immediate(immediate) ;

        err = ADD(state ,index_src_reg_1,sign_extended_version,index_dest_reg,opcode) ;



         if (err!= mips_Success)
        {
            return err ; // this is sent to testbench to determine what it is ( overflow ?,other error ? )
        }
    }

    // opcode 12 - ANDI
    else if(opcode == 12)
    {
        at_least_one_instruction = true ;
        fetch_I_Type(instruction,&index_src_reg_1,
                    &index_dest_reg,&immediate) ;

        // debugging
            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                    index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);

                cout << endl ;
            }

            bitwise_AND(state ,index_src_reg_1,immediate,index_dest_reg,opcode ) ;

    }

    // opcode 11 - SLTIU
    // opcode = 10 - SLTI
    else if(( opcode == 11 ) || (opcode == 10 ))
    {
        at_least_one_instruction = true ;
        fetch_I_Type(instruction,&index_src_reg_1,
                    &index_dest_reg,&immediate) ;

         if(state->logLevel >= 2)
        {
            cout << endl ;
            fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);

            cout << endl ;
        }

        immediate = sign_extend_immediate(immediate) ;
        // SLTI
        if(opcode == 10)
        {
             state->reg[index_dest_reg] = ( (state->reg[index_src_reg_1]) < ( int32_t(immediate) ) )  ;
        }

        // SLTIU
        else if(opcode ==11 )
        {
            state->reg[index_dest_reg] = ( (state->reg[index_src_reg_1]) < ( uint32_t(immediate) ) )  ;

        }


    }
    // opcode = 43 - SW
    // opcode = 41 - SH
    // opcode = 40 - SB
    else if( (opcode == 43) || (opcode == 40) || (opcode == 41) )
    {
        at_least_one_instruction = true ;
         fetch_I_Type(instruction,&index_src_reg_1,
                    &index_dest_reg,&immediate) ;

          if(state->logLevel >= 2)
        {
            cout << endl ;
            fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);

            cout << endl ;
        }
            //======================= COMMON ===================================
            int32_t sw_offset = sign_extend_immediate(immediate) ;
            cout << "sw offset = " << sw_offset << endl ;

            uint32_t sw_address = (state->reg[index_src_reg_1]+sw_offset) ;
            cout << "sw_address in sw = " << sw_address << endl ;

            if( (sw_address % 4 )!= 0 )
            {
                return mips_ExceptionInvalidAlignment;
            }

            uint8_t read_buffer[4] ;

            uint32_t final_write ; // final content to write to the memory.


            err =  mips_mem_read(state->mem,sw_address ,4 ,read_buffer ) ;
             if(err!=0)
            {
                cout << " Returning read error " << endl ;
                return err ;
            }


            // now you have aligned the stuff in memory into normal form.
            uint32_t stuff_in_memory = to_big(read_buffer) ;

        // =========================END OF  COMMON ===================================

        // debugging
            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                    index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);

                cout << endl ;
            }
            // SH
            if(opcode==41)
            {
                cout << "store half word " << endl ;
                 uint32_t sh_content = (state->reg[index_dest_reg]) & 0xFFFF ;
                 stuff_in_memory = ((stuff_in_memory) & (0xFFFF0000) )  ;

               final_write = ((stuff_in_memory) | (sh_content)) ; // now all we need to do is arrange into a write_buffer.

            }

            // SB
            else if(opcode == 40)
            {
                cout << "store  byte " << endl ;
                 uint32_t sb_content = (state->reg[index_dest_reg]) & 0xFF ;
                 stuff_in_memory = ((stuff_in_memory) & (0xFFFFFF00) ) ;

                 final_write = ((stuff_in_memory) | (sb_content)) ;
            }

            // SW
            else if(opcode == 43)
           {
                cout << " store full word " << endl ;
                uint32_t sw_content = state->reg[index_dest_reg] ;

                 final_write = sw_content ;
           }

            uint8_t write_buffer[4] ;
            write_buffer[0]=(final_write>>24)&0xFF; // storing the MSB at the lowest address.
            write_buffer[1]=(final_write>>16)&0xFF;
            write_buffer[2]=(final_write>>8)&0xFF;
            write_buffer[3]=(final_write>>0)&0xFF;

            // We write to the memory just like the testbench.
            err = mips_mem_write(
            state->mem,	        //!< Handle to target memory
            sw_address,	    //!< Byte address to start transaction at
            4,write_buffer);



            cout << " Stored the contents of the index_dest reg at the base+offset in memory .........." << endl ;
            if(err!=0)
            {
                cout << " Returning write error " << endl ;
                return err ;
            }

    }

    else if((opcode==34) || (opcode==38))
    {
        return mips_ErrorNotImplemented ;
    }

    // 35 - LW
    // 32 - LB
    // 36 - LBU
    // 33 - LH
    // 37 - LHU
    else if((opcode==35) || (opcode==32) || (opcode==33) || (opcode==37) || (opcode==36))
    {
        at_least_one_instruction = true ;
        fetch_I_Type(instruction,&index_src_reg_1,
                    &index_dest_reg,&immediate) ;

          if(state->logLevel >= 2)
        {
            cout << endl ;
            fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);

            cout << endl ;
        }
//=============================================
            int32_t lw_offset = sign_extend_immediate(immediate) ;
            cout << "lw offset = " << lw_offset << endl ;

            uint32_t lw_address = (state->reg[index_src_reg_1]+lw_offset) ;
            cout << "lw_address in sw = " << lw_address << endl ;

            if( (lw_address % 4 )!= 0 )
            {
                return mips_ExceptionInvalidAlignment;
            }

            uint8_t read_buffer[4] ;

            uint32_t final_write ; // final content to write to register.

            err =  mips_mem_read(state->mem,lw_address ,4 ,read_buffer ) ;

             if(err!=0)
            {
                cout << " Returning read error " << endl ;
                return err ;
            }

            // now you have aligned the stuff in memory into normal form.
            uint32_t stuff_in_memory = to_big(read_buffer) ;
            uint32_t load_to_dest_reg  ;
            uint32_t load_to_dest_reg_intermediate ;
  // ===============================================================
             if(opcode== 32) // LB
             {
                 uint32_t load_to_dest_reg_intermediate = (stuff_in_memory) & 0xFF ;
                 if(( load_to_dest_reg_intermediate >> 7) == 1) // need to sign extend
                 {
                     load_to_dest_reg = ( (load_to_dest_reg_intermediate) | (0xFFFFFF00) );
                 }

                 else
                 {
                     load_to_dest_reg = load_to_dest_reg_intermediate ;
                 }
             }

             else if(opcode== 36) // LBU
             {
                 load_to_dest_reg = (stuff_in_memory) & 0xFF ;
             }

             else if(opcode== 33) // LH
             {
                 uint32_t load_to_dest_reg_intermediate = (stuff_in_memory) & 0xFFFF ;
                load_to_dest_reg = sign_extend_immediate(load_to_dest_reg_intermediate) ;

             }

             else if(opcode== 37) // LHU
             {
                 load_to_dest_reg = (stuff_in_memory) & 0xFFFF ;
             }

            else if(opcode == 35) // LW
            {
                load_to_dest_reg = stuff_in_memory ;
            }


    //=======================================================


            cout << "load to dest reg " << load_to_dest_reg << endl ;

            err = mips_cpu_set_register(state,index_dest_reg,load_to_dest_reg);


            if(err!=0)
            {
                return err ;
            }




    }

    // LUI
    else if(opcode == 15)
    {
         fetch_I_Type(instruction,&index_src_reg_1,
                        &index_dest_reg,&immediate) ;

        if(index_src_reg_1 ==0)
        {
            at_least_one_instruction = true ;
            state->reg[index_dest_reg] = (immediate) << 16 ;
        }

    }
    // opcode= 4 - BEQ
    // opcode =1 , index_dest_reg = 1 , 17 , 0 ,16- BGEZ / BGEZAL / BLTZ / BLTZAL
    // opcode = 7 - BGTZ
    // opcode = 6 - BLEZ
    // opcode =5 - BNE


    else if ((opcode ==4) || (opcode==1) || (opcode==7) || (opcode==6) || (opcode==5))

    {


        fetch_I_Type(instruction,&index_src_reg_1 ,
                    &index_dest_reg,&immediate) ;

        // debugging.
        if(state->logLevel >= 2)
            {
                        if(opcode==4)
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BEQ -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

                        else if(opcode==5)
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BNE -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

                        else if ((opcode==1) && (index_dest_reg == 1) )
                        {
                            at_least_one_instruction = true  ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BGEZ -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

                        else if ((opcode==1) && (index_dest_reg == 17) )
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BGEZAL -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

                         else if ((opcode==1) && (index_dest_reg == 16) )
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BLTZAL -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

                         else if ((opcode==1) && (index_dest_reg == 0) )
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BLTZ -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }


                        else if (opcode==7 )
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BGTZ -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

                        else if (opcode==6 )
                        {
                            at_least_one_instruction = true ;
                            cout << endl ;
                            fprintf(state->logDst, "I-Type : BLEZ -  dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                            index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                            cout << endl ;
                        }

            }


            immediate = sign_extend_immediate(immediate) ;
        cout << "casted immediate = " << (int32_t(immediate )) << endl ;
        (state->offset) = ((int32_t)(immediate << 2)) ;  // offset is now 18 bit wide and SIGNED.
        cout << " Offset = " << (state->offset) << endl ;


            // If(condition) , then increment counter.

            // if (BEQ)
            if(opcode==4)
          {

                    if((state->reg[index_src_reg_1]) == (state->reg[index_dest_reg]))
                    {
                        cout << " register values are equal ! " << endl ;
                        cout << " counter has been incremented" << endl ;
                        (state->counter) = (state->counter)+1 ;
                         cout << " value of counter after incrementing is= " << state->counter << endl ;
                    }
          }

            // if (BNE)
            else if(opcode==5)
          {

                    if((state->reg[index_src_reg_1]) != (state->reg[index_dest_reg]))
                    {
                        cout << " register values are NOT equal ! " << endl ;
                        cout << " counter has been incremented" << endl ;
                        (state->counter) = (state->counter)+1 ;
                         cout << " value of counter after incrementing is= " << state->counter << endl ;
                    }
          }

            // If BGEZ or BGEZAL
            else if(opcode==1)
          {
              // if(BGEZ)
                if(index_dest_reg == 1)
                {

                    if(( (state->reg[index_src_reg_1]) >> 31 )  == 0)
                    {
                        cout << " register value is >=0 ! " << endl ;
                        cout << " counter has been incremented" << endl ;
                        (state->counter) = (state->counter)+1 ;
                        cout << " value of counter after incrementing is= " << state->counter << endl ;
                    }
                }

                // if(BGEZAL)
                 else if(index_dest_reg == 17)
                {
                            // return error if reg = 31 ???????????????????

                             // Storing the return address in reg 31.
                            ( state->reg[31] ) = (( state->pc)+8 )  ;
                            //cout << endl << " the value of the return address is = " << ( state->reg[31] ) << endl ;

                            if(( (state->reg[index_src_reg_1]) >> 31 )  == 0)
                            {
                                cout << " within BGEZAL the IF " << endl ;
                               // cout << " register value is >=0 ! " << endl ;
                               // cout << " counter has been incremented" << endl ;
                                (state->counter) = (state->counter)+1 ;
                                //cout << " value of counter after incrementing is= " << state->counter << endl ;
                            }
                }

                 // if(BLTZAL
                 else if(index_dest_reg == 16)
                {
                            // return error if reg = 31 ???????????????????

                             // Storing the return address in reg 31.
                            ( state->reg[31] ) = (( state->pc)+8 )  ;
                            //cout << endl << " the value of the return address is = " << ( state->reg[31] ) << endl ;

                            if(( (state->reg[index_src_reg_1]) >> 31 )  == 1)
                            {
                                cout << " within BLTZAL the IF " << endl ;
                               // cout << " register value is >=0 ! " << endl ;
                               // cout << " counter has been incremented" << endl ;
                                (state->counter) = (state->counter)+1 ;
                                //cout << " value of counter after incrementing is= " << state->counter << endl ;
                            }
                }


                // if(BLTZ)
                else if(index_dest_reg == 0)
                {

                    if(( (state->reg[index_src_reg_1]) >> 31 )  == 1)
                    {
                        cout << " this is BLTZ " << endl ;
                        cout << " register value is < 0  ! " << endl ;
                        cout << " counter has been incremented" << endl ;
                        (state->counter) = (state->counter)+1 ;
                        cout << " value of counter after incrementing is= " << state->counter << endl ;
                    }
                }

          }

          // opcode = 7 - BGTZ
           else if(opcode==7)
          {
                    // if the msb is zero and if the whole value ! = 0 .
                    if(  (((state->reg[index_src_reg_1]) >> 31 )== 0) &&  ( (state->reg[index_src_reg_1])!= 0)  )
                    {
                        cout << " thisis BGTZ ---------" << endl;
                        cout << " register value is > 0 ! " << endl ;
                        cout << " counter has been incremented" << endl ;
                        (state->counter) = (state->counter)+1 ;
                        cout << " value of counter after incrementing is= " << state->counter << endl ;
                    }
          }

          // opcode = 6 - BLEZ
          else if(opcode==6)
          {
                    // if the msb is 1 OR  if the whole value == 0 .
                    if(  (((state->reg[index_src_reg_1]) >> 31 )== 1) ||  ( (state->reg[index_src_reg_1])== 0)  )
                    {
                        cout << " register value is <= 0 ! " << endl ;
                        cout << " counter has been incremented" << endl ;
                        (state->counter) = (state->counter)+1 ;
                        cout << " value of counter after incrementing is= " << state->counter << endl ;
                    }
          }


            // only in BRANCHES opcode cases - we will "deviate"
            // from using the pc_increment flag.
            increment_pc(state) ;
            return mips_Success ;
    }

    // opcode = 13 - simple bitwise OR I.
    else if(opcode ==13)
    {
        at_least_one_instruction = true ;
        fetch_I_Type(instruction,&index_src_reg_1,
                    &index_dest_reg,&immediate) ;

        // debugging
            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                cout << endl ;
            }

           (state->reg[index_dest_reg]) =  ( (state->reg[index_src_reg_1])  |   (immediate) );


    }


    // opcode = 14 - simple bitwise XOR I.
    else if(opcode ==14)
    {
        at_least_one_instruction = true ;
        fetch_I_Type(instruction,&index_src_reg_1,
                    &index_dest_reg,&immediate) ;

        // debugging
            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "I-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                cout << endl ;
            }

           (state->reg[index_dest_reg]) =  ( (state->reg[index_src_reg_1])  ^   (immediate) );


    }




//============================= J TYPE ==================================================

    else if((opcode == 2) || (opcode==3))
    {
        at_least_one_instruction = true ;
         fetch_J_Type(instruction,&instruction_index) ;
        // debugging
            if(state->logLevel >= 2)
            {
                cout << endl ;
                fprintf(state->logDst, "J-Type : dst=%u, src1=%u,immediate=%u.\n  instr=%08x\n,PC= %u",
                index_dest_reg, index_src_reg_1, immediate,instruction ,state->pc);
                cout << endl ;
            }

            if(opcode == 3)
            {
                cout << "storing return address inn reg 31" << endl ;
                state->reg[31] = (state->pc) + 8 ;


            }

           (state->jump_target_address) = ( instruction_index << 2 )  ; // 28 bit wide

             cout << " counter has been incremented" << endl ;
            (state->counter) = (state->counter)+1 ;
            cout << " value of counter after incrementing is= " << state->counter << endl ;

            state->was_a_jump = true ;
            increment_pc(state) ;
            return mips_Success ;


    }
//============================= COMMON SECTION FOR ALL INSTRUCTIONS ==============

 // counter == 1 means that we have already executed a branch instruction before !!
    // Also , NOTE that this if condition will NOT be activated after the branch instruction itself
    // Since we are returning mips_success there directly !

// this sis executed in the delay instruction after the branch.
    if((state->counter) >= 1 )
    {
        cout << "Counter is 1 - check!" << endl ;
        (state->counter) = 0 ; // re-initialize.
        cout << " value of counter after re-initialization is= " << state->counter << endl ;

        (state->pc) = (state->pc) + (state->offset) - 4 ;  // notice the 4 since we will be incrmenting pc later.
                                                            // for the SAKE OF UNIFORMITY.
        //cout << " pc after adding offset is = " << state->pc << endl ;

        if(state->was_a_jump)
        {
            cout << " Previous was a jump " << endl ;
            (state->was_a_jump )= false ;
            uint32_t temp_var ;
            mips_cpu_get_pc(state,&temp_var);

            temp_var = (temp_var & 0xF0000000) ;
            effective_target_address = ((temp_var ) | (state->jump_target_address) )- 4  ;
            cout << "effectibve trget address = " << effective_target_address << endl ;
             (state->pc) = effective_target_address ;

        }

        else if( state->was_a_rtype_jump)
        {
             cout << " Previous was a r-type jump " << endl ;
            (state->was_a_rtype_jump )= false ;
             (state->pc) = state->jalr_address - 4;

        }
    }

    // NOTE - this assumes that there were NO ERRORS earlier. We will hence return mips_success as well !!

        err = increment_pc(state) ;
        if(err!=0)
        {
            cout << " returning increment pc error " <<endl ;
            return err ;
        }

        cout << " pc after adding offset and increment is = " << (state->pc) << endl ;


        if(at_least_one_instruction)
        {
           return mips_Success ;
        }

        else
        {
            return mips_ExceptionInvalidInstruction ;
        }

}

// ===================== END of mips_cpu_step ============================================

mips_error mips_cpu_set_debug_level(mips_cpu_h state, unsigned level, FILE *dest)
{
    state->logLevel = level;
    state->logDst = dest;
    return mips_Success;
}

/*! Free all resources associated with state.

	\param state Either a handle to a valid simulation state, or an empty (NULL) handle.

	It is legal to pass an empty handle to mips_cpu_free. It is illegal
	to pass the same non-empty handle to mips_cpu_free twice, and will
	result in undefined behaviour (i.e. anything could happen):

		mips_cpu_h cpu=mips_cpu_create(...);
		...
		mips_cpu_free(h);	// This is fine
		...
		mips_cpu_free(h);	// BANG! or nothing. Or format the hard disk.

	A better pattern is to always zero the variable after calling free,
	in case elsewhere you are not sure if resources have been released yet:

		mips_cpu_h cpu=mips_cpu_create(...);
		...
		mips_cpu_free(h);	// This is fine
		h=0;	// Make sure it is now empty
		...
		mips_cpu_free(h);	// Fine, nothing happens
		h=0;    // Does nothing here, might could stop other errors
*/

//The memory is NOT owned by the CPU, so it should not
//	be \ref mips_mem_free "freed" when the CPU is \ref mips_cpu_free "freed".
void mips_cpu_free(mips_cpu_h state)
{
    free(state) ;
    state = 0 ;
}


//============================ EXTRA FUNCTIONS ADDED BY ME =========================

uint32_t to_big(const uint8_t *pData)
{
    return
        (((uint32_t)pData[0])<<24)
        |
        (((uint32_t)pData[1])<<16)
        |
        (((uint32_t)pData[2])<<8)
        |
        (((uint32_t)pData[3])<<0);
}

mips_error increment_pc(mips_cpu_h state)
{
    uint32_t a ;
    mips_error err ;

    // First lets set the reg[0] = 0
     mips_cpu_set_register(state,0,0);

    cout << endl << " Incrmeneted pc " << endl ;

    // check if there were errors while incrementing pc.
    err = mips_cpu_get_pc(state,&a);

    if(err!=0)
    {
        return err ;
    }
        a = a+4 ;
   err =  mips_cpu_set_pc(state,a);
    if(err!=0)
    {
        return err ;
    }

    return mips_Success ;
}

#ifdef __cplusplus
};
#endif


