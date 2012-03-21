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

#include <Vrml_SFRotation.ixx>

Vrml_SFRotation::Vrml_SFRotation()
{
}

Vrml_SFRotation::Vrml_SFRotation( const Standard_Real aRotationX, 
				  const Standard_Real aRotationY, 
				  const Standard_Real aRotationZ, 
				  const Standard_Real anAngle )
{
    myRotationX = aRotationX;
    myRotationY = aRotationY;
    myRotationZ = aRotationZ;
    myAngle     = anAngle;
}

void Vrml_SFRotation::SetRotationX(const Standard_Real aRotationX)
{
    myRotationX = aRotationX;
}

Standard_Real Vrml_SFRotation::RotationX() const 
{
  return myRotationX;
}

void Vrml_SFRotation::SetRotationY(const Standard_Real aRotationY)
{
    myRotationY = aRotationY;
}

Standard_Real Vrml_SFRotation::RotationY() const 
{
  return myRotationY;
}

void Vrml_SFRotation::SetRotationZ(const Standard_Real aRotationZ)
{
    myRotationZ = aRotationZ;
}

Standard_Real Vrml_SFRotation::RotationZ() const 
{
  return myRotationZ;
}

void Vrml_SFRotation::SetAngle(const Standard_Real anAngle)
{
    myAngle = anAngle;
}

Standard_Real Vrml_SFRotation::Angle() const 
{
  return myAngle;
}
