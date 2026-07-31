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

Vrml_SFRotation::Vrml_SFRotation() = default;

Vrml_SFRotation::Vrml_SFRotation(const double aRotationX,
                                 const double aRotationY,
                                 const double aRotationZ,
                                 const double anAngle)
{
  myRotationX = aRotationX;
  myRotationY = aRotationY;
  myRotationZ = aRotationZ;
  myAngle     = anAngle;
}

void Vrml_SFRotation::SetRotationX(const double aRotationX)
{
  myRotationX = aRotationX;
}

double Vrml_SFRotation::RotationX() const
{
  return myRotationX;
}

void Vrml_SFRotation::SetRotationY(const double aRotationY)
{
  myRotationY = aRotationY;
}

double Vrml_SFRotation::RotationY() const
{
  return myRotationY;
}

void Vrml_SFRotation::SetRotationZ(const double aRotationZ)
{
  myRotationZ = aRotationZ;
}

double Vrml_SFRotation::RotationZ() const
{
  return myRotationZ;
}

void Vrml_SFRotation::SetAngle(const double anAngle)
{
  myAngle = anAngle;
}

double Vrml_SFRotation::Angle() const
{
  return myAngle;
}
