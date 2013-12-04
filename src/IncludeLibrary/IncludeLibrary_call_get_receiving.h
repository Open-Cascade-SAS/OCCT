/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and / or modify it
 under the terms of the GNU Lesser General Public version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
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
