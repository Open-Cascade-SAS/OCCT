// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
