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

#include <Vrml_TextureCoordinate2.ixx>

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
 anOStream  << "TextureCoordinate2 {" << endl;

 if ( myPoint->Length() != 1 || Abs(myPoint->Value(myPoint->Lower()).X() - 0) > 0.0001 || 
                                Abs(myPoint->Value(myPoint->Lower()).Y() - 0) > 0.0001 )
  { 
   anOStream  << "    point [" << endl << '\t';
    for ( i = myPoint->Lower(); i <= myPoint->Upper(); i++ )
	{
	 anOStream << myPoint->Value(i).X() << ' ' << myPoint->Value(i).Y();

         if ( i < myPoint->Length() )
	    anOStream  << ',' << endl << '\t';
	}
    anOStream  << " ]" << endl;
  }
  anOStream  << '}' << endl;
 return anOStream;
}
