// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef math_Memory_HeaderFile 
#define math_Memory_HeaderFile 

#include <string.h>

// uniquement parce que memmove n'existe pas sur SUN
#ifndef WNT
void *reverse_move(void *s1, void *s2, int size);

inline void *memmove(void *s1, void *s2, int size) {

/*
     void *result;

     if(s2 < s1) {
       result = reverse_move(s1, s2, size);
     }
     else {
       result = memcpy(s1, s2, size);
     }      
     return result;
*/
     return memcpy(s1, s2, size);
   }
#endif
#endif
