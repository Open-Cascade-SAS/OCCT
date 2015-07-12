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


#include <Vrml_SFRotation.hxx>

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
