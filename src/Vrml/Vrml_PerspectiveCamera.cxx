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


#include <gp_Vec.hxx>
#include <Vrml_PerspectiveCamera.hxx>
#include <Vrml_SFRotation.hxx>

Vrml_PerspectiveCamera::Vrml_PerspectiveCamera():
  myFocalDistance(5),
  myHeightAngle(0.785398)
{

  gp_Vec tmpVec(0,0,1);
  myPosition = tmpVec;

  Vrml_SFRotation tmpSFR(0,0,1,0);
  myOrientation = tmpSFR;
}

Vrml_PerspectiveCamera::Vrml_PerspectiveCamera(   const gp_Vec&          aPosition, 
						  const Vrml_SFRotation& aOrientation, 
						  const Standard_Real    aFocalDistance, 
						  const Standard_Real    aHeightAngle)
{
    myPosition      = aPosition;
    myOrientation   = aOrientation;
    myFocalDistance = aFocalDistance;
    myHeightAngle   = aHeightAngle;
}

void Vrml_PerspectiveCamera::SetPosition(const gp_Vec& aPosition)
{
    myPosition = aPosition;
}

gp_Vec Vrml_PerspectiveCamera::Position() const 
{
   return  myPosition;
}

void Vrml_PerspectiveCamera::SetOrientation(const Vrml_SFRotation& aOrientation)
{
   myOrientation   = aOrientation;
}

Vrml_SFRotation Vrml_PerspectiveCamera::Orientation() const 
{
   return myOrientation; 
}

void Vrml_PerspectiveCamera::SetFocalDistance(const Standard_Real aFocalDistance)
{
   myFocalDistance = aFocalDistance;
}

Standard_Real Vrml_PerspectiveCamera::FocalDistance() const 
{
   return myFocalDistance;
}

void Vrml_PerspectiveCamera::SetAngle(const Standard_Real aHeightAngle)
{
    myHeightAngle = aHeightAngle;
}

Standard_Real Vrml_PerspectiveCamera::Angle() const 
{
   return myHeightAngle;
}

Standard_OStream& Vrml_PerspectiveCamera::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "PerspectiveCamera {\n";
 if ( Abs(myPosition.X() - 0) > 0.0001 || 
      Abs(myPosition.Y() - 0) > 0.0001 || 
      Abs(myPosition.Z() - 1) > 0.0001 )
   {
    anOStream  << "    position\t\t";
    anOStream << myPosition.X() << " " << myPosition.Y() << " " << myPosition.Z() << "\n";
   }

 if ( Abs(myOrientation.RotationX() - 0) > 0.0001 || 
     Abs(myOrientation.RotationY() - 0) > 0.0001 || 
     Abs(myOrientation.RotationZ() - 1) > 0.0001 ||
     Abs(myOrientation.Angle() - 0) > 0.0001 )
   {
    anOStream  << "    orientation\t\t";
    anOStream << myOrientation.RotationX() << " " << myOrientation.RotationY() << " ";
    anOStream << myOrientation.RotationZ() << " " << myOrientation.Angle() << "\n";
   }

 if ( Abs(myFocalDistance - 5) > 0.0001 )
   {
    anOStream  << "    focalDistance\t";
    anOStream << myFocalDistance << "\n";
   }
 if ( Abs(myHeightAngle - 0.785398) > 0.0000001 )
   {
    anOStream  << "    heightAngle\t\t";
    anOStream << myHeightAngle << "\n";
   }
 anOStream  << "}\n";
 return anOStream;
}
