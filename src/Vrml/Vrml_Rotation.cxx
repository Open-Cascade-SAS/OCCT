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

#include <Vrml_Rotation.ixx>

Vrml_Rotation::Vrml_Rotation()
{
  Vrml_SFRotation tmpR(0,0,1,0);
  myRotation = tmpR;
}

Vrml_Rotation::Vrml_Rotation(const Vrml_SFRotation& aRotation)
{
  myRotation = aRotation;
}

 void Vrml_Rotation::SetRotation(const Vrml_SFRotation& aRotation) 
{
  myRotation = aRotation;
}

 Vrml_SFRotation Vrml_Rotation::Rotation() const
{
  return myRotation;
}

 Standard_OStream& Vrml_Rotation::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Rotation {" << endl;

 if ( Abs(myRotation.RotationX() - 0) > 0.0001 || 
     Abs(myRotation.RotationY() - 0) > 0.0001 || 
     Abs(myRotation.RotationZ() - 1) > 0.0001 ||
     Abs(myRotation.Angle() - 0) > 0.0001 ) 
   {
    anOStream  << "    rotation" << '\t';
    anOStream << myRotation.RotationX() << ' ' << myRotation.RotationY() << ' ';
    anOStream << myRotation.RotationZ() << ' ' << myRotation.Angle() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
