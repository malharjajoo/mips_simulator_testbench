
#include <iostream>
#include <stdio.h> // this is for allowing commands like fprintf.


#include "mips_test.h"

#include "Itype_instructions.h"
#include "test_mips_extension.h"


#include <cstdlib>
#include <fstream>
using namespace std ;

// =============== EXTRA FUNCTIONS BY ME ================
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


//=================END OF EXTRA FUNCTIONS=====================================

int main()
{
    uint32_t blockSize = 4 ;
    uint32_t cbMem = 5000 ; // this is 5 KB ,, roughly.

    // STEP 1 - CREATE Cpu and mem instances.
    // NOTE- errors have already been handled inside the function by using
    //       exit(1) so no need to worry.
     mips_mem_h  mem = mips_mem_create_ram(cbMem,blockSize) ;
    mips_cpu_h state = mips_cpu_create(mem);

    mips_error e=mips_cpu_set_debug_level(state, 4, stderr);
    if(e!=mips_Success){
        fprintf(stderr, "mips_cpu_set_debug_level : failed.\n");
        exit(1);
    }

    // Begin Suite
    mips_test_begin_suite();

        int testId ;


// ---------------------------- BEGIN Instructions ------------------------------------------------

        // Declarations.
            unsigned index_first ;   // index of source register 1
            unsigned index_second ; // index of source register 2
            uint32_t immediate ;
            unsigned index_dest ;
            uint32_t value_first ; // value of src register 1
            uint32_t value_second ;
            uint32_t instruction_index ;


            uint32_t instr ;
            //uint32_t instruction_length ; // multiple of 4. decides ending point of writing to memory.
            uint32_t instruction_address ; // multiple of 4.decides starting point of writing to memory.
            mips_error err;
            uint32_t got ; // gets value from dest reg.




    ifstream file ;
    file.open("learnt.csv");

    string instruction_name_s , testing_message_s ,extra_message_s; // We will be using this.

    // We will NOT be using this.
    string index_first_s ,index_second_s , immediate_s , instruction_index_s, index_dest_s,value_first_s , value_second_s
    ,opcode_s ,shift_s , functioncode_s , expected_error_s,expected_value_s ;

    // We will be using these.
    unsigned opcode ,shift , functioncode  , expected_error,expected_value;

    bool passed = false ;


// Skip the rubbish .
    if(true)
   {
        getline ( file, instruction_name_s, ',' );

          getline ( file,index_first_s , ',' );


          getline ( file,index_second_s , ',' );

          getline ( file,immediate_s , ',' );

           getline ( file,immediate_s , ',' );

          getline ( file,index_dest_s , ',' );

         getline ( file,value_first_s , ',' );

         getline ( file,value_second_s , ',' );

         getline ( file,opcode_s , ',' );

         getline ( file,shift_s , ',' );

          getline ( file,functioncode_s , ',' );

        //getline ( file,instruction_address_s , ',' );


          getline ( file,expected_error_s ,',');

          getline ( file,expected_value_s ,',');

           getline ( file,extra_message_s ,',');

          getline ( file,testing_message_s );

   }


//int i = 1 ;  // this is the number of instructions.

while ( getline ( file, instruction_name_s, ',' )  )
{


        passed = false ;

    // get the info from the CSV file.
        if(true)
       {


        getline ( file,index_first_s , ',' );
        index_first = atoi(index_first_s.c_str());

          getline ( file,index_second_s , ',' );
        index_second = atoi(index_second_s.c_str());

        getline ( file,immediate_s , ',' );
        immediate = atoi(immediate_s.c_str());

        getline ( file,instruction_index_s , ',' );
        instruction_index = atoi(instruction_index_s.c_str());

          getline ( file,index_dest_s , ',' );
        index_dest = atoi(index_dest_s.c_str());

         getline ( file,value_first_s , ',' );
        value_first = atoi(value_first_s.c_str());


         getline ( file,value_second_s , ',' );
        value_second = atoi(value_second_s.c_str());

         getline ( file,opcode_s , ',' );
       opcode = atoi(opcode_s.c_str());


         getline ( file,shift_s , ',' );
        shift = atoi(shift_s.c_str()) ;

          getline ( file,functioncode_s , ',' );
        functioncode = atoi(functioncode_s.c_str());



          getline ( file,expected_error_s,',' );
        expected_error = atoi(expected_error_s.c_str());

          getline ( file,expected_value_s,',' );
        expected_value = atoi(expected_value_s.c_str());

        getline ( file,extra_message_s,',' );


          getline ( file,testing_message_s );
       }

    // All of the R-type and I type NON BRANCH instructions .
    if((opcode!= 4 ) && (opcode!=1) && (opcode!=7) && (opcode!=6) && (opcode!=2) && (!((opcode == 0) && (functioncode == 9)))  && (opcode!=2) &&
       (!((opcode == 0) && (functioncode == 8))) && (opcode!=3) && (opcode!=5))
    {

        if(true)
        {


            testId =  mips_test_begin_test(instruction_name_s.c_str());


            err = mips_cpu_set_register(state,index_first ,value_first);
            check_error(err) ;


            err = mips_cpu_set_register(state,index_second ,value_second);
            check_error(err) ;

            // SW , SH , SB instruction.

            if(( opcode == 43) ||  (opcode == 40) ||  (opcode == 41) )
            {
                 err = mips_cpu_set_register(state,index_dest ,value_second);
                check_error(err) ;
            }

            if(opcode == 0)
           {
                instr =
            ((opcode << 26) // opcode
            |
            ((index_first) << 21) // index_src2
            |
            ((index_second) << 16) // index_src2
            |
            ((index_dest) << 11) // index_dst
            |
            (shift << 6) // shift
            |
            (functioncode << 0)); // functioncode
           }

           // I type instruction for now.
           else
           {
                instr =
            ((opcode << 26) // opcode
            |
            ((index_first) << 21) // index_src2
            |
            ((index_dest) << 16) // index_dest
            |
            (immediate & (0xFFFF) )); // immediate
           }

        // Writing to memory -
            uint8_t buffer[4];
            buffer[0]=(instr>>24)&0xFF;
            buffer[1]=(instr>>16)&0xFF;
            buffer[2]=(instr>>8)&0xFF;
            buffer[3]=(instr>>0)&0xFF;

            //instruction_length = 4 ;    // no. of bytes in the instruction.
           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
            check_error(err) ;


        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
            //err = mips_cpu_set_pc(state,instruction_address);
           // check_error(err) ;



            err = mips_cpu_step(state) ;
            check_error(err) ;



            if(extra_message_s == "get_value_from_dest_reg")
           {
                err = mips_cpu_get_register(state,index_dest,&got) ;
                check_error(err) ;
                cout << "Got = " << got << endl ;
                passed = ((err==expected_error) && (got==expected_value)) ;
           }


        // In this case dest reg is not set to any sensible value.
        // Hence we avoid the case above.
           else if(extra_message_s == "check_overflow" )
           {
               cout << " The error is = " << err << endl ;
               passed = (err == expected_error) ;

           }

            else if(extra_message_s == "simply_check_success")
            {
                passed = (err==mips_Success) ;
            }

            else if(extra_message_s == "check_invalid_alignment")
            {
                cout << "yo yo honey singh " << endl ;
                passed = (err== expected_error) ;
            }



            else if(extra_message_s == "read_full_word_from_memory")
            {
                int32_t p_offset;
                uint32_t base ;
                uint32_t r ;
                uint32_t sw_address ;

                 p_offset = sign_extend_immediate(immediate) ;
                mips_cpu_get_register(state,index_first,&base) ;
                sw_address = base + p_offset ;

                 uint8_t sw_buffer[4] ;
                 mips_mem_read(mem,sw_address,4,sw_buffer)  ;
                uint32_t compare = to_big(sw_buffer);

                mips_cpu_get_register(state,index_dest,&r) ;
                passed = ((compare == r) && (err==expected_error)); // this si just in case we have an address alignment issue.

            }

             else if(extra_message_s == "read_half_word_from_memory")
            {
               int32_t p_offset;
                uint32_t base ;
                uint32_t r ;
                uint32_t sh_address ;

                 p_offset = sign_extend_immediate(immediate) ;
                mips_cpu_get_register(state,index_first,&base) ;
                sh_address = base + p_offset ;

                cout << " memeory address from whcihc we read " << sh_address << endl ;
                 uint8_t sh_buffer[4] ;
                 mips_mem_read(mem,sh_address,4,sh_buffer)  ;
                uint32_t compare = to_big(sh_buffer);
                compare = (compare) & (0xFFFF) ;

                mips_cpu_get_register(state,index_dest,&r) ;
                r = (r) & (0xFFFF) ; // extracts lower 16 bits of index reg.

                cout << " Compare = " << compare << endl ;
                cout << " 16 bit value in dest reg = " << r << endl ;
                passed = ((compare == r) && (err==expected_error)); // this si just in case we have an address alignment issue.

            }

             else if(extra_message_s == "read_byte_from_memory")
            {
                int32_t p_offset;
                uint32_t base ;
                uint32_t r ;
                uint32_t sb_address ;

                 p_offset = sign_extend_immediate(immediate) ;
                mips_cpu_get_register(state,index_first,&base) ;
                sb_address = base + p_offset ;

                 uint8_t sb_buffer[4] ;
                 mips_mem_read(mem,sb_address,4,sb_buffer)  ;
                uint32_t compare = to_big(sb_buffer);
                compare = (compare) & (0xFF) ;
                cout << " SB compare = " << compare << endl ;
                mips_cpu_get_register(state,index_dest,&r) ;
                r = (r) & (0xFF) ; // extracts lower 8 bits that we want to compare.

                passed = ((compare == r) && (err==expected_error)); // this si just in case we have an address alignment issue.
            }


            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed , testing_message_s.c_str());

            printPassOrFail(testId,passed,testing_message_s.c_str()) ;
        }

    }


    // BEQ - 4
    // BGEZ - 1,1
    // BGEZAL - 1,17
    // BGTZ - 1,
    // BLEZ - 1,
    // BNE - 5
    // J- JUMP
    // JR

    else if((opcode == 4) || (opcode == 1) || (opcode==5)|| (opcode ==7) || (opcode==6) || (opcode==2) || ((opcode == 0) && (functioncode == 9)) || (opcode==3) || ((opcode == 0) && (functioncode == 8)))
    {
             uint32_t return_address_reg ;

                // Other than JALR - BGEZAL , BLTZAL
                if(opcode==1)
                {
                    return_address_reg = 31 ;
                }

                // Other than JALR - JAL
                 if(opcode==3)
                {
                    return_address_reg = 31 ;
                }

                    testId =  mips_test_begin_test(instruction_name_s.c_str());


                    err = mips_cpu_set_register(state,index_first ,value_first);
                    check_error(err) ;

                    // NOTE - We are setting the detination register - this is only for the BEQ instruction !!
                    err = mips_cpu_set_register(state,index_dest ,value_second);
                    check_error(err) ;


                       // JALR and JR
                         if(((opcode == 0) && (functioncode == 9)) || ((opcode == 0) && (functioncode == 8)))
                       {
                           return_address_reg = index_dest ;

                                    instr =
                                ((opcode << 26) // opcode
                                |
                                ((index_first) << 21) // index_src2
                                |
                                ((index_second) << 16) // index_src2
                                |
                                ((index_dest) << 11) // index_dst
                                |
                                (shift << 6) // shift
                                |
                                (functioncode << 0)); // functioncode
                       }

                        // JUMP or JAL
                    else if((opcode==2) || (opcode==3))
                    {

                         instr =
                            ((opcode << 26) // opcode
                            |
                            (instruction_index &0x3FFFFFF )); // instruction_index


                    }


                    else if(opcode!=2)
                    {
                                instr =
                                ((opcode << 26) // opcode
                                |
                                ((index_first) << 21) // index_src2
                                |
                                ((index_dest) << 16) // index_dest
                                |
                                (immediate &0xFFFF )); // immediate
                    }




                     uint8_t buffer[4];
                    buffer[0]=(instr>>24)&0xFF;
                    buffer[1]=(instr>>16)&0xFF;
                    buffer[2]=(instr>>8)&0xFF;
                    buffer[3]=(instr>>0)&0xFF;


                    //instruction_length = 4 ;    // no. of bytes in the instruction.
                   err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                    err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                    check_error(err) ;


                // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                    //err = mips_cpu_set_pc(state,instruction_address);
                   // check_error(err) ;



                    err = mips_cpu_step(state) ; // Execution of BEQ or BGEZ or BGEZAL or BGTZ or J or JALR
                    check_error(err) ;

                            uint32_t expected_pc ;
                            uint32_t unexpected_pc ;
                            uint32_t expected_return_address ;

                            uint32_t getPcVal ;
                             mips_cpu_get_pc(state,&getPcVal); // multiple of 4. need not worry about the value.

                            int32_t offset ;

                            expected_return_address  = getPcVal+4 ;


                              // JALR
                              if(((opcode == 0) && (functioncode == 9)) || ((opcode == 0) && (functioncode == 8)))
                              {
                                   mips_cpu_get_register( state,index_first, &expected_pc);
                                   cout << " expectd pc = " << expected_pc << endl ;
                              }

                              // JUMP , JAL
                             else if((opcode==2) || (opcode ==3))
                             {
                                  uint32_t temp_var ;
                                  mips_cpu_get_pc(state,&temp_var);

                                   temp_var = (temp_var & 0xF0000000) ;
                                    temp_var = ((temp_var ) | ((instruction_index & 0x3FFFFFF) << 2 ) ) ;
                                     expected_pc = temp_var ;
                             }

                            else if((opcode!=2) && (opcode!=3))
                             {

                                     int32_t  temp ;
                                    temp = sign_extend_immediate(immediate) ;
                                    //cout << "casted immediate = " << (int32_t(immediate )) << endl ;
                                    offset = ((int32_t)(temp << 2)) ;  // offset is now 18 bit wide and SIGNED.

                                     expected_pc = getPcVal+offset ; // we do  because we have to add offset to the branch delay instruction.
                                     unexpected_pc = getPcVal+4 ;  // since we will be executing two instrucions in a row.



                             }






                        // this is the branch delay
                        mips_cpu_step(state) ;



                    if(extra_message_s == "check_expected_pc")
                    {
                        cout << " checking expected pc...... " << endl ;
                        uint32_t temp1 ;
                        mips_cpu_get_pc(state,&temp1) ;
                        cout << " Pc that we get is " << temp1 << endl ;
                        cout << " offst that we have is "<< offset << endl ;
                        cout << " pc that we expect is " << expected_pc << endl ;
                        passed = (temp1==expected_pc) ;
                    }

                    else if(extra_message_s == "check_UNexpected_pc")
                    {
                        cout << " checking UNexpected pc...... " << endl ;
                        uint32_t temp4 ;
                        mips_cpu_get_pc(state,&temp4) ;
                        cout << " Pc that we get is " << temp4 << endl ;
                        //cout << " offst that we have is "<< offset << endl ;
                        cout << " pc that we expect is " << unexpected_pc << endl ;
                        passed = (temp4==unexpected_pc) ;
                    }


                    else if(extra_message_s == "check_expected_pc_and_return_address")
                    {
                        cout << " checking expected pc and return address...." << endl ;
                        uint32_t temp2 ;
                        uint32_t temp3 ;

                         mips_cpu_get_pc(state,&temp2) ;

                         cout << " Pc that we get is " << temp2 << endl ;
                        cout << " offst that we have is "<< offset << endl ;
                        cout << " pc that we expect is " << expected_pc << endl ;

                        mips_cpu_get_register(state,return_address_reg,&temp3) ;
                        cout << " return address that we get is = "<< temp3 << endl ;
                        cout << " return adress that we expect is = " << expected_return_address << endl ;

                        passed = ((temp2==expected_pc) && (temp3 == expected_return_address))  ;
                    }

                    else if(extra_message_s == "check_UNexpected_pc_and_return_address")
                    {
                        cout << " checking unexpected pc and return address......" << endl ;
                        uint32_t temp5 ;
                        uint32_t temp6 ;

                         mips_cpu_get_pc(state,&temp5) ;

                        mips_cpu_get_register(state,31,&temp6) ;

                        passed = ((temp5==unexpected_pc) && (temp6 == expected_return_address))  ;
                    }



                    cout << " this is after extra message " << endl ;
                    // Pass in a good comprehensive message as the 3rd argument
                    mips_test_end_test(testId,passed,testing_message_s.c_str());

                    printPassOrFail(testId,(passed),testing_message_s.c_str()) ;

        // ======================= END OF conditional Branches EXECUTION ===================================================


            }




}

// Extra instructions - s
if(true)
{

//======================= BEGIN - DIV - INSTRUCTION TESTING ===============================================

            testId =  mips_test_begin_test("DIV");

            index_first = 3 ;
            index_second = 17 ;
            index_dest = 0 ;

            value_first = -9 ;
            value_second = 2 ;

            err = mips_cpu_set_register(state,index_first ,value_first);
            check_error(err) ;


            err = mips_cpu_set_register(state,index_second ,value_second);
            check_error(err) ;

            instr =
            ((0 << 26) // opcode
            |
            ((index_first) << 21) // index_src2
            |
            ((index_second) << 16) // index_src2
            |
            ((index_dest) << 11) // index_dst
            |
            (0 << 6) // shift
            |
            (26 << 0)); // functioncode


            uint8_t buffer[4] ;
            buffer[0]=(instr>>24)&0xFF;
            buffer[1]=(instr>>16)&0xFF;
            buffer[2]=(instr>>8)&0xFF;
            buffer[3]=(instr>>0)&0xFF;

            //instruction_length = 4 ;    // no. of bytes in the instruction.
           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
            check_error(err) ;


        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
            //err = mips_cpu_set_pc(state,instruction_address);
           // check_error(err) ;



            err = mips_cpu_step(state) ;
            check_error(err) ;

       // ===================== MFHI and MFLO instructions to be executed. = ===========================

                            // First MFHI
                             index_dest = 8 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (16 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;

                            uint32_t check_hi ;

                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &check_hi
                            );

                            int32_t signed_check_hi ;

                            signed_check_hi = int32_t(check_hi) ;

                    //=======================// Second -  MFLO ===================
                             index_dest = 9 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (18 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;

                            uint32_t check_lo ;

                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &check_lo
                            );

                            int32_t signed_check_lo ;
                            signed_check_lo = (int32_t(check_lo) );

                            cout << " HEY ! " << endl ;

            // ================= END OF MFHI AND MFLO Instructions ================================

            passed = ( (signed_check_hi == -1 ) && (signed_check_lo == -4 ) ) ;



            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"basic testing DIV");

            printPassOrFail(testId,(passed),"basic testing DIV") ;

  // =========================== END OF DIV ======================================================================================================













//======================= BEGIN -  DIVU - INSTRUCTION TESTING ============================================================================================

            passed = false ;
            testId =  mips_test_begin_test("DIVU");

            index_first = 3 ;
            index_second = 17 ;
            index_dest = 0 ;

            value_first = 9 ;
            value_second = -2 ;

            err = mips_cpu_set_register(state,index_first ,value_first);
            check_error(err) ;


            err = mips_cpu_set_register(state,index_second ,value_second);
            check_error(err) ;

            instr =
            ((0 << 26) // opcode
            |
            ((index_first) << 21) // index_src2
            |
            ((index_second) << 16) // index_src2
            |
            ((index_dest) << 11) // index_dst
            |
            (0 << 6) // shift
            |
            (27 << 0)); // functioncode


            buffer[0]=(instr>>24)&0xFF;
            buffer[1]=(instr>>16)&0xFF;
            buffer[2]=(instr>>8)&0xFF;
            buffer[3]=(instr>>0)&0xFF;

            //instruction_length = 4 ;    // no. of bytes in the instruction.
           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
            check_error(err) ;


        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
            //err = mips_cpu_set_pc(state,instruction_address);
           // check_error(err) ;



            err = mips_cpu_step(state) ;
            check_error(err) ;

       // ===================== MFHI and MFLO instructions to be executed. = ===========================

                            // First MFHI
                             index_dest = 8 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (16 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;



                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &check_hi
                            );

                            signed_check_hi = int32_t(check_hi) ;

                    //=======================// Second -  MFLO ===================
                             index_dest = 9 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (18 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;



                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &check_lo
                            );

                            signed_check_lo = int32_t(check_lo) ;
                            cout << " HEY ! " << endl ;

            // ================= END OF MFHI AND MFLO Instructions ================================

            passed = ( (signed_check_hi == 9 ) && (signed_check_lo == 0 ) ) ;



            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"basic testing DIVU");

            printPassOrFail(testId,(passed),"basic testing DIVU") ;


// =======================================  END OG DIVU ===============================================






// ====================================== BEGIN OF MTHI =====================================

            passed = false ;
            testId =  mips_test_begin_test("MTHI");

            index_first = 3 ;

            value_first = 100 ;

            err = mips_cpu_set_register(state,index_first ,value_first);
            check_error(err) ;

            instr =
            ((0 << 26) // opcode
            |
            ((index_first) << 21) // index_src2
            |
            ((0) << 16) // index_src2
            |
            ((0) << 11) // index_dst
            |
            (0 << 6) // shift
            |
            (17 << 0)); // functioncode = 17


            buffer[0]=(instr>>24)&0xFF;
            buffer[1]=(instr>>16)&0xFF;
            buffer[2]=(instr>>8)&0xFF;
            buffer[3]=(instr>>0)&0xFF;

            //instruction_length = 4 ;    // no. of bytes in the instruction.
           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
            check_error(err) ;


        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
            //err = mips_cpu_set_pc(state,instruction_address);
           // check_error(err) ;



            err = mips_cpu_step(state) ;
            check_error(err) ;


            // NOW MFHI

                             index_dest = 8 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (16 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;

                            uint32_t mthi_check ;

                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &mthi_check
                            );

                             passed =  (mthi_check == value_first )  ;



            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"basic testing MTHI");

            printPassOrFail(testId,(passed),"basic testing MTHI") ;


// =========================END OF MTHI ===================================================







// =========================== BEGIN MFHI =======================================

 // NOW MFHI
                              passed = false ;
                            testId =  mips_test_begin_test("MFHI");

                             index_dest = 8 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (16 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;

                            uint32_t mfhi_check ;

                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &mfhi_check
                            );

                             passed =  (mfhi_check == value_first )  ;



            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"basic testing MFHI");

            printPassOrFail(testId,(passed),"basic testing MFHI") ;


// =========================END OF MFHI ===================================================
















// ====================================== BEGIN OF MTLO =====================================

            passed = false ;
            testId =  mips_test_begin_test("MTLO");

            index_first = 3 ;

            value_first = 200 ;

            err = mips_cpu_set_register(state,index_first ,value_first);
            check_error(err) ;

            instr =
            ((0 << 26) // opcode
            |
            ((index_first) << 21) // index_src2
            |
            ((0) << 16) // index_src2
            |
            ((0) << 11) // index_dst
            |
            (0 << 6) // shift
            |
            (19 << 0)); // functioncode = 17


            buffer[0]=(instr>>24)&0xFF;
            buffer[1]=(instr>>16)&0xFF;
            buffer[2]=(instr>>8)&0xFF;
            buffer[3]=(instr>>0)&0xFF;

            //instruction_length = 4 ;    // no. of bytes in the instruction.
           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
            check_error(err) ;


        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
            //err = mips_cpu_set_pc(state,instruction_address);
           // check_error(err) ;



            err = mips_cpu_step(state) ;
            check_error(err) ;


            // NOW MFLO
                             index_dest = 9;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (18 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;

                            uint32_t mtlo_check ;

                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &mtlo_check
                            );

                             passed =  (mtlo_check == value_first )  ;



            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"basic testing MTLO");

            printPassOrFail(testId,(passed),"basic testing MTLO") ;


// =========================END OF MTLO ===================================================








// =========================== BEGIN MFLO =======================================

 
                            passed = false ;
                            testId =  mips_test_begin_test("MFLO");

                             index_dest = 7 ;

                            instr =
                            ((0 << 26) // opcode
                            |
                            ((0) << 21) // index_src2
                            |
                            ((0) << 16) // index_src2
                            |
                            ((index_dest) << 11) // index_dst
                            |
                            (0 << 6) // shift
                            |
                            (18 << 0)); // functioncode


                            buffer[0]=(instr>>24)&0xFF;
                            buffer[1]=(instr>>16)&0xFF;
                            buffer[2]=(instr>>8)&0xFF;
                            buffer[3]=(instr>>0)&0xFF;

                            //instruction_length = 4 ;    // no. of bytes in the instruction.
                           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

                            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
                            check_error(err) ;


                        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
                            //err = mips_cpu_set_pc(state,instruction_address);
                           // check_error(err) ;

                            err = mips_cpu_step(state) ;
                            check_error(err) ;

                            uint32_t mflo_check ;

                             mips_cpu_get_register(
                                state,
                                index_dest,
                                &mflo_check
                            );

                             passed =  (mflo_check == value_first )  ;



            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"basic testing MFLO");

            printPassOrFail(testId,(passed),"basic testing MFLO") ;


// =========================END OF MFHI ===================================================























/* // SPECIAL CASE OF JUMP

//======================= BEGIN - J SPECIAL - INSTRUCTION TESTING ===============================================

            testId =  mips_test_begin_test("J");


            instr =
            ((2 << 26) // opcode
            |
            (67108863 & 0x3FFFFFF ));  // all ones.



            buffer[0]=(instr>>24)&0xFF;
            buffer[1]=(instr>>16)&0xFF;
            buffer[2]=(instr>>8)&0xFF;
            buffer[3]=(instr>>0)&0xFF;

            //instruction_length = 4 ;    // no. of bytes in the instruction.
           err = mips_cpu_get_pc(state,&instruction_address); // multiple of 4. need not worry about the value.

            err = mips_mem_write(mem,instruction_address,4,(buffer)) ;
            check_error(err) ;


        // STEP 3 - Need to set Program counter since this is what is read by the mips_cpu_step.
            //err = mips_cpu_set_pc(state,instruction_address);
           // check_error(err) ;



            err = mips_cpu_step(state) ;
            check_error(err) ;






            // Pass in a good comprehensive message as the 3rd argument
            mips_test_end_test(testId,passed ,"Special testing JUMP");

            printPassOrFail(testId,(passed),"Special testing jump") ;

  // =========================== END OF J - Special case ======================================================================================================

*/

}
mips_test_end_suite();

    mips_cpu_free(state) ;
    mips_mem_free(mem);

    return 0 ;

}





