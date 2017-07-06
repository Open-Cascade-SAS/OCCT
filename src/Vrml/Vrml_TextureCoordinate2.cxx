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


#include <Standard_Type.hxx>
#include <Vrml_TextureCoordinate2.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_TextureCoordinate2,Standard_Transient)

Vrml_TextureCoordinate2::Vrml_TextureCoordinate2()
{
  gp_Vec2d tmpVec(0,0);
  myPoint = new TColgp_HArray1OfVec2d(1,1,tmpVec);
}

Vrml_TextureCoordinate2::Vrml_TextureCoordinate2(const Handle(TColgp_HArray1OfVec2d)& aPoint)
{
  myPoint = aPoint;
}

 void Vrml_TextureCoordinate2::SetPoint(const Handle(TColgp_HArray1OfVec2d)& aPoint) 
{
  myPoint = aPoint;
}

 Handle(TColgp_HArray1OfVec2d) Vrml_TextureCoordinate2::Point() const
{
  return myPoint;
}

 Standard_OStream& Vrml_TextureCoordinate2::Print(Standard_OStream& anOStream) const
{
 Standard_Integer i;
 anOStream  << "TextureCoordinate2 {\n";

 if ( myPoint->Length() != 1 || Abs(myPoint->Value(myPoint->Lower()).X() - 0) > 0.0001 || 
                                Abs(myPoint->Value(myPoint->Lower()).Y() - 0) > 0.0001 )
  { 
   anOStream  << "    point [\n\t";
    for ( i = myPoint->Lower(); i <= myPoint->Upper(); i++ )
	{
	 anOStream << myPoint->Value(i).X() << " " << myPoint->Value(i).Y();

         if ( i < myPoint->Length() )
	    anOStream  << ",\n\t";
	}
    anOStream  << " ]\n";
  }
  anOStream  << "}\n";
 return anOStream;
}
