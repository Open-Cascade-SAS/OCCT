#ifndef _Standard_JmpBuf_HeaderFile
#define _Standard_JmpBuf_HeaderFile

#include <stdlib.h>
#include <setjmp.h>
    
typedef 
#ifdef SOLARIS
           sigjmp_buf   
#elif IRIX
           sigjmp_buf   
#else
           jmp_buf      
#endif
Standard_JmpBuf;

#endif
