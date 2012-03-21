/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

int nbr_ret_func = 5 ;
PVOID_FUNCTION TABRETFUNC[5] ; 

#define nbrptrfunc 0
PVOID_FUNCTION TABFUNC[1] ;

#define nbrptr 3
void *TABPTR[nbrptr] ;

#define nbrval 1
int TABVAL[nbrval] ;

TABFUNC[0] = NULL ;
TABPTR[0] = NULL ;
TABVAL[0] = *KVERBOSEINIT ;
