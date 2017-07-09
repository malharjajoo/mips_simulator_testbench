#include <iostream>
#include "test_mips_extension.h"
#include <stdint.h>

// This file only CHECKS errors , but it does not return errors !

void check_error(mips_error err)
{
    if(err!=0)
    {
        cout << " error = " << err << endl ;
        cout << " Printing error ............. " << endl ;
        if(err==0x1000)
        {
            fprintf(stderr,"mips_ErrorNotImplemented") ;
        }

        else if(err==0x1001)
        {
            fprintf(stderr,"mips_ErrorInvalidArgument") ;
        }

         else if(err==0x1002)
        {
            fprintf(stderr,"mips_ErrorInvalidHandle") ;
        }

        else if(err==0x1003)
        {
            fprintf(stderr,"mips_ErrorFileReadError") ;
        }

        else if(err==0x1004)
        {
            fprintf(stderr,"mips_ErrorFileWriteError") ;
        }



    //! Error or exception from the simulated processor or program.
    ///@{
        else if(err==0x2000)
        {
            fprintf(stderr," mips_ExceptionBreak") ;
        }

        else if(err==0x2001)
        {
            fprintf(stderr,"mips_ExceptionInvalidAddress") ;
        }


        else if(err==0x2002)
        {
            fprintf(stderr,"mips_ExceptionInvalidAlignment") ;
        }

         else if(err==0x2003)
        {
            fprintf(stderr,"mips_ExceptionAccessViolation") ;
        }

        else if(err==0x2004)
        {
            fprintf(stderr,"mips_ExceptionInvalidInstruction") ;
        }

         else if(err==0x2005)
        {
            fprintf(stderr,"mips_ExceptionArithmeticOverflow") ;
        }


//-----------------------------------------------------------------
          else if(err==0x3000)
        {
            fprintf(stderr," mips_InternalError") ;
        }

        cout << endl  ;
        cout << endl ;
    }

}

void printPassOrFail(int testId,bool pass ,string message)
{
    if (pass)
    {
        cout << endl ;
        fprintf(stderr, "You have passed TestId = %d - %s", testId,message.c_str());
        cout << endl ;
        cout << "==============================================================================";
        cout << endl ;
        cout << endl ;

    }

    else
    {
        cout << endl ;
        fprintf(stderr, "You have failed TestId = %d - %s", testId,message.c_str());
        cout << endl ;
        cout << "==============================================================================";
        cout << endl ;
    }
}
