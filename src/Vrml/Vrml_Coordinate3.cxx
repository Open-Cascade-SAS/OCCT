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

#include <Vrml_Coordinate3.ixx>

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
 
 anOStream  << "Coordinate3 {" << endl;
 
 i = myPoint->Lower();
 if (  myPoint->Length() == 1 && Abs(myPoint->Value(i).X() - 0) < 0.0001 
                              && Abs(myPoint->Value(i).Y() - 0) < 0.0001 
                              && Abs(myPoint->Value(i).Z() - 0) < 0.0001 )
   {
    anOStream << '}' << endl;
    return anOStream;
   }
 else
   {
    anOStream  << "    point [" << endl << '\t';
    for ( i = myPoint->Lower(); i <= myPoint->Upper(); i++ )
	{
	 anOStream << myPoint->Value(i).X() << ' ' << myPoint->Value(i).Y() << ' ' << myPoint->Value(i).Z();
	 if ( i < myPoint->Length() )
	    anOStream  << ',' << endl << '\t';
        }
    anOStream  << " ]" << endl;
   }
 anOStream << '}' << endl;

 return anOStream;
}

