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


#include <Vrml_Group.hxx>

Vrml_Group::Vrml_Group()
{
  myFlagPrint = 0;
}

Standard_OStream& Vrml_Group::Print(Standard_OStream& anOStream)
{
  if ( myFlagPrint == 0 )
    {
      anOStream  << "Group {\n";
      myFlagPrint = 1;
    } //End of if
  else 
    {
     anOStream  << "}\n";
     myFlagPrint = 0;
    }
 return anOStream;
}

