

#include <iostream>
using namespace std ;
#include "Rtype_instructions.h"

//==================================================================================
// -----------------------Fetch R-type -------------------------------------
void fetch_R_Type(uint32_t instruction,uint32_t* index_src_reg_1 ,
                  uint32_t* index_src_reg_2 ,  uint32_t* index_dest_reg,
                   uint32_t* shift ,uint32_t* functioncode)
{

     *index_src_reg_1 = (instruction>> 21 ) & 0x1F;
     *index_src_reg_2 = (instruction>> 16 ) & 0x1F;
     *index_dest_reg  = (instruction>> 11 ) & 0x1F;
     *shift = (instruction>> 6 ) & 0x1F ;
     *functioncode = (instruction>> 0 ) & 0x3F ;

}

//====================================================================================

mips_error ADD(mips_cpu_h state ,uint32_t index_src_reg_1,uint32_t index_src_reg_2,
               uint32_t index_dest_reg, uint32_t functioncode)
{
     // DEAL WITH OVERFLOW -

     bool overflow_flag = false ;
     bool check_signed_overflow = false ;
     uint32_t v1 ;
     uint32_t v2 ;
     uint32_t intermediate ;
    mips_error err ;

 // ADD and ADDU instruction
if((functioncode==32) | (functioncode ==33))
{
    if(functioncode == 32)
     {
         check_signed_overflow = true ;
         if(state->logLevel >= 1)
        {
            cout << endl ;
            fprintf(state->logDst, "ADD %u, %u, %u.\n", index_dest_reg, index_src_reg_1, index_src_reg_2);
            cout << endl ;
        }
     }

     else if ( functioncode ==33)
     {
         check_signed_overflow = false;
         if(state->logLevel >= 1)
        {
            cout << endl ;
            fprintf(state->logDst, "ADDU %u, %u, %u.\n", index_dest_reg, index_src_reg_1, index_src_reg_2);
            cout << endl ;
        }
     }



    mips_error err ;
    v1 = state-> reg[index_src_reg_1] ;
     v2 = state-> reg[index_src_reg_2] ;
    intermediate = v1+v2 ;

    if(check_signed_overflow)
    {
        signed_overflow( v1,v2,&overflow_flag ) ;

            if(overflow_flag)
            {
                return mips_ExceptionArithmeticOverflow ;
            }
    }


       err = mips_cpu_set_register(state,index_dest_reg,intermediate);
       if(err!=0)
       {
           return err ;
       }

        cout << endl ;
        return mips_Success ;
}


        // 8 - ADDI
        // 9 - ADDIU

        else if((functioncode == 8) | (functioncode == 9))
        {
                        // ADDI
                        if(functioncode == 8)
                        {
                             check_signed_overflow = true ;
                             if(state->logLevel >= 1)
                            {
                                cout << endl ;
                                fprintf(state->logDst, "ADDI %u, %u, %u.\n", index_dest_reg, index_src_reg_1, index_src_reg_2);
                                cout << endl ;
                            }
                        }

                        // ADDIU
                        if(functioncode == 9)
                        {
                             check_signed_overflow = false;
                             if(state->logLevel >= 1)
                            {
                                cout << endl ;
                                fprintf(state->logDst, "ADDIU %u, %u, %u.\n", index_dest_reg, index_src_reg_1, index_src_reg_2);
                                cout << endl ;
                            }
                        }


                 v1 = state-> reg[index_src_reg_1] ;
                 v2 = index_src_reg_2 ;
                 intermediate = v1+v2 ;

                if(check_signed_overflow)
                {
                    signed_overflow( v1,v2,&overflow_flag ) ;

                        if(overflow_flag)
                        {
                            return mips_ExceptionArithmeticOverflow ;
                        }
                }


                    err = mips_cpu_set_register(state,index_dest_reg,intermediate);
                   if(err!=0)
                   {
                       return err ;
                   }

                    cout << endl ;
                    return mips_Success ;


        }

        return mips_ErrorNotImplemented;
}


mips_error bitwise_AND(mips_cpu_h state ,uint32_t index_src_reg_1,uint32_t index_src_reg_2_or_immediate,
                 uint32_t index_dest_reg , uint32_t distinguishing_code)
{
    mips_error err ;
    uint32_t a , b, c ;

    if(distinguishing_code ==36)
    {
        err = mips_cpu_get_register(state,index_src_reg_1,&a);
        err = mips_cpu_get_register(state,index_src_reg_2_or_immediate,&b);

        if(err!=0)
        {
            return err ;
        }

         c = (a)  &  (b) ;

        err =  mips_cpu_set_register(state,index_dest_reg,c);
    }

    else if(distinguishing_code==12)
    {
         err = mips_cpu_get_register(state,index_src_reg_1,&a);

        if(err!=0)
        {
            return err ;
        }

         c = (a)  &  (index_src_reg_2_or_immediate) ;

        err =  mips_cpu_set_register(state,index_dest_reg,c);
    }

    if(err!=0)
    {
        return err ;
    }

    return mips_Success ;
}


void signed_overflow( uint32_t v1 , uint32_t v2 , bool* signed_overflow )
{

            uint32_t A = (v1>>31) & 0x1 ;  // first_bit_of_reg_v1
            uint32_t B = (v2>>31) & 0x1 ;  // first_bit_of_reg_v2
            uint32_t C = ((v1+v2)>>31)& 0x1 ;  //first_bit_of_intermediate

            cout <<" a = " << A << endl ;
            cout <<" b = " << B << endl ;
            cout <<" c = " << C << endl ;

            if( (((~A)&(~B)&(C)) | ((A)&(B)&(~C)) )) // just using boolean logic
            {

                *signed_overflow = true ;
            }


/*
        cout << " value"  << int64_t((v1) + (v2)) << endl ;

         if( ( int64_t((v1) + (v2)) < (-2147483648)) ||  (int64_t((v1) + (v2)) > (2147483647))  )
        {
           *signed_overflow = true  ;
        }
*/

}
