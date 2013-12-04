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

/* Returns a unique number according to the string passed as 1fst argument */ 
int osd_getkey(char *name)
{
    int   ii = 0;
    register  char *pp;

        pp = name;
        while( *pp ) ii = ii << 1 ^ *pp++;
        if( ii < 0 ) ii = -ii;

    return ii;
}
