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


#include <TCollection_AsciiString.hxx>
#include <Vrml_Instancing.hxx>

Vrml_Instancing::Vrml_Instancing(const TCollection_AsciiString& aString)
{
 TCollection_AsciiString tmpAS = aString;
 tmpAS.ChangeAll(' ', '_', Standard_True);
 myName = tmpAS;
}

 Standard_OStream& Vrml_Instancing::DEF(Standard_OStream& anOStream) const
{
 anOStream  << "DEF " << myName << "\n";
 return anOStream;
}

 Standard_OStream& Vrml_Instancing::USE(Standard_OStream& anOStream) const
{
 anOStream  << "USE " << myName << "\n";
 return anOStream;
}
