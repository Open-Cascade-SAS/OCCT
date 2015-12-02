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
#include <Vrml_Info.hxx>

Vrml_Info::Vrml_Info(const TCollection_AsciiString& aString)
{
  myString = aString;
}

 void Vrml_Info::SetString(const TCollection_AsciiString& aString) 
{
  myString = aString;
}

 TCollection_AsciiString Vrml_Info::String() const
{
  return myString; 
}

 Standard_OStream& Vrml_Info::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Info {\n";

 if ( !(myString.IsEqual( "<Undefined info>") ) )
   {
    anOStream  << "    string\t";
    anOStream << '"' << myString << '"' << "\n";
   }

 anOStream  << "}\n";
 return anOStream;
}
