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


#include <gp_Vec.hxx>
#include <TCollection_AsciiString.hxx>
#include <Vrml_WWWInline.hxx>

Vrml_WWWInline::Vrml_WWWInline()
{
  myName = "";
  gp_Vec tmpVec(0,0,0);
  myBboxSize = tmpVec;
  myBboxCenter = tmpVec;
}

Vrml_WWWInline::Vrml_WWWInline(const TCollection_AsciiString& aName,
			       const gp_Vec& aBboxSize,
			       const gp_Vec& aBboxCenter)
{
  myName = aName;
  myBboxSize = aBboxSize;
  myBboxCenter = aBboxCenter;
}

void Vrml_WWWInline::SetName(const TCollection_AsciiString& aName) 
{
  myName = aName;
}

TCollection_AsciiString Vrml_WWWInline::Name() const
{
  return myName;
}

void Vrml_WWWInline::SetBboxSize(const gp_Vec& aBboxSize) 
{
  myBboxSize = aBboxSize;
}

gp_Vec Vrml_WWWInline::BboxSize() const
{
  return myBboxSize;
}

void Vrml_WWWInline::SetBboxCenter(const gp_Vec& aBboxCenter) 
{
  myBboxCenter = aBboxCenter;
}

gp_Vec Vrml_WWWInline::BboxCenter() const
{
  return myBboxCenter;
}

Standard_OStream& Vrml_WWWInline::Print(Standard_OStream& anOStream) const
{
 anOStream  << "WWWInline {\n";

 if ( !(myName.IsEqual ("") ) )
   {
    anOStream  << "    name\t";
    anOStream << '"' << myName << '"' << "\n";
   }

 if ( Abs(myBboxSize.X() - 0) > 0.0001 || 
     Abs(myBboxSize.Y() - 0) > 0.0001 || 
     Abs(myBboxSize.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    bboxSize\t";
    anOStream << myBboxSize.X() << " " << myBboxSize.Y() << " " << myBboxSize.Z() << "\n";
   }

 if ( Abs(myBboxCenter.X() - 0) > 0.0001 || 
     Abs(myBboxCenter.Y() - 0) > 0.0001 || 
     Abs(myBboxCenter.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    bboxCenter\t";
    anOStream << myBboxCenter.X() << " " << myBboxCenter.Y() << " " << myBboxCenter.Z() << "\n";
   }

 anOStream  << "}\n";
 return anOStream;
}
