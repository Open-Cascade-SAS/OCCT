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
#include <Vrml_Normal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_Normal,Standard_Transient)

Vrml_Normal::Vrml_Normal(const Handle(TColgp_HArray1OfVec)& aVector)
{
 myVector = aVector;
}

 Vrml_Normal::Vrml_Normal()
{
 gp_Vec Tmp_Vec;
 myVector = new TColgp_HArray1OfVec (1,1);
 
 Tmp_Vec.SetX(0); Tmp_Vec.SetY(0); Tmp_Vec.SetZ(1);
 myVector->SetValue(1,Tmp_Vec);
}

void Vrml_Normal::SetVector(const Handle(TColgp_HArray1OfVec)& aVector)
{
 myVector = aVector;
}

Handle(TColgp_HArray1OfVec) Vrml_Normal::Vector() const 
{
 return myVector;
}

Standard_OStream& Vrml_Normal::Print(Standard_OStream& anOStream) const 
{
 Standard_Integer i;

 anOStream  << "Normal {\n";
 i = myVector->Lower();
 if ( myVector->Length() == 1 && 
     Abs(myVector->Value(i).X() - 0) < 0.0001 && 
     Abs(myVector->Value(i).Y() - 0) < 0.0001 && 
     Abs(myVector->Value(i).Z() - 1) < 0.0001 )
   {
    anOStream  << "}\n";
    return anOStream;
   }
 else 
  {
  anOStream  << "    vector [\n\t";
   for ( i = myVector->Lower(); i <= myVector->Upper(); i++ )
     {
	 anOStream << myVector->Value(i).X() << " " << myVector->Value(i).Y() << " " << myVector->Value(i).Z();
      if ( i < myVector->Length() )
	 anOStream  << ",\n\t";
     }
    anOStream << " ]\n";
  }  
  anOStream  << "}\n";

 return anOStream;
}

