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

#include <Vrml_Scale.ixx>

Vrml_Scale::Vrml_Scale()
{
  gp_Vec tmpV(1,1,1);
  myScaleFactor = tmpV;
}

Vrml_Scale::Vrml_Scale(const gp_Vec& aScaleFactor)
{
  myScaleFactor = aScaleFactor;
}

 void Vrml_Scale::SetScaleFactor(const gp_Vec& aScaleFactor) 
{
  myScaleFactor = aScaleFactor;
}

 gp_Vec Vrml_Scale::ScaleFactor() const
{
  return  myScaleFactor;
}

 Standard_OStream& Vrml_Scale::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Scale {" << endl;

 if ( Abs(myScaleFactor.X() - 1) > 0.0001 || 
     Abs(myScaleFactor.Y() - 1) > 0.0001 || 
     Abs(myScaleFactor.Z() - 1) > 0.0001 ) 
   {
    anOStream  << "    scaleFactor" << '\t';
    anOStream << myScaleFactor.X() << ' ' << myScaleFactor.Y() << ' ' << myScaleFactor.Z() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
