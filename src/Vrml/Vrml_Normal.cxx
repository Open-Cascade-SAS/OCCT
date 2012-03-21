// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Vrml_Normal.ixx>

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

 anOStream  << "Normal {" << endl;
 i = myVector->Lower();
 if ( myVector->Length() == 1 && 
     Abs(myVector->Value(i).X() - 0) < 0.0001 && 
     Abs(myVector->Value(i).Y() - 0) < 0.0001 && 
     Abs(myVector->Value(i).Z() - 1) < 0.0001 )
   {
    anOStream  << '}' << endl;
    return anOStream;
   }
 else 
  {
  anOStream  << "    vector [\n\t";
   for ( i = myVector->Lower(); i <= myVector->Upper(); i++ )
     {
	 anOStream << myVector->Value(i).X() << ' ' << myVector->Value(i).Y() << ' ' << myVector->Value(i).Z();
      if ( i < myVector->Length() )
	 anOStream  << ',' << endl << '\t';
     }
    anOStream << " ]" << endl;
  }  
  anOStream  << '}' << endl;

 return anOStream;
}

