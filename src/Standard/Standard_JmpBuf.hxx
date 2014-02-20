// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
