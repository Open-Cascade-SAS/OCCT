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


#include <Vrml_Switch.hxx>

Vrml_Switch::Vrml_Switch(const Standard_Integer aWhichChild)
{
  myWhichChild = aWhichChild;
}

 void Vrml_Switch::SetWhichChild(const Standard_Integer aWhichChild) 
{
  myWhichChild = aWhichChild;
}

 Standard_Integer Vrml_Switch::WhichChild() const
{
  return myWhichChild;
}

 Standard_OStream& Vrml_Switch::Print(Standard_OStream& anOStream) const
{
  anOStream  << "Switch {\n";
  if ( myWhichChild != -1 )
    {
      anOStream  << "    whichChild\t";
      anOStream << myWhichChild << "\n";
    }
 anOStream  << "}\n";
 return anOStream;
}
