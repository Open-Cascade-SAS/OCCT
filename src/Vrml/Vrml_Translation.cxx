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

#include <Vrml_Translation.ixx>

Vrml_Translation::Vrml_Translation()
{
  gp_Vec tmpV(0,0,0);
  myTranslation = tmpV;
}

Vrml_Translation::Vrml_Translation(const gp_Vec& aTranslation)
{
  myTranslation = aTranslation;
}

 void Vrml_Translation::SetTranslation(const gp_Vec& aTranslation) 
{
  myTranslation = aTranslation;
}

 gp_Vec Vrml_Translation::Translation() const
{
  return myTranslation;
}

 Standard_OStream& Vrml_Translation::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Translation {" << endl;

 if ( Abs(myTranslation.X() - 0) > 0.0001 || 
     Abs(myTranslation.Y() - 0) > 0.0001 || 
     Abs(myTranslation.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    translation" << '\t';
    anOStream << myTranslation.X() << ' ' << myTranslation.Y() << ' ' << myTranslation.Z() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
