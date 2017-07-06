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
#include <Vrml_Coordinate3.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_Coordinate3,Standard_Transient)

Vrml_Coordinate3::Vrml_Coordinate3(const Handle(TColgp_HArray1OfVec)& aPoint)
{
 myPoint = aPoint;
}

 Vrml_Coordinate3::Vrml_Coordinate3()
{
 gp_Vec Tmp_Vec;
 
 myPoint = new TColgp_HArray1OfVec(1,1);
 Tmp_Vec.SetX(0); Tmp_Vec.SetY(0); Tmp_Vec.SetZ(0);
 myPoint->SetValue (1,Tmp_Vec);
}

void Vrml_Coordinate3::SetPoint(const Handle(TColgp_HArray1OfVec)& aPoint)
{
 myPoint = aPoint;
}

Handle(TColgp_HArray1OfVec) Vrml_Coordinate3::Point() const 
{
 return myPoint;
}

Standard_OStream& Vrml_Coordinate3::Print(Standard_OStream& anOStream) const 
{
 Standard_Integer i;
 
 anOStream  << "Coordinate3 {\n";
 
 i = myPoint->Lower();
 if (  myPoint->Length() == 1 && Abs(myPoint->Value(i).X() - 0) < 0.0001 
                              && Abs(myPoint->Value(i).Y() - 0) < 0.0001 
                              && Abs(myPoint->Value(i).Z() - 0) < 0.0001 )
   {
    anOStream << "}\n";
    return anOStream;
   }
 else
   {
    anOStream  << "    point [\n\t";
    for ( i = myPoint->Lower(); i <= myPoint->Upper(); i++ )
	{
	 anOStream << myPoint->Value(i).X() << ' ' << myPoint->Value(i).Y() << ' ' << myPoint->Value(i).Z();
	 if ( i < myPoint->Length() )
	    anOStream  << ",\n\t";
        }
    anOStream  << " ]\n";
   }
 anOStream << "}\n";

 return anOStream;
}

