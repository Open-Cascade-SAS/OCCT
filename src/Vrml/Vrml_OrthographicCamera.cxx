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
#include <Vrml_OrthographicCamera.hxx>
#include <Vrml_SFRotation.hxx>

Vrml_OrthographicCamera::Vrml_OrthographicCamera():
  myFocalDistance(5),
  myHeight(2)
{

  gp_Vec tmpVec(0,0,1);
  myPosition = tmpVec;

  Vrml_SFRotation tmpSFR(0,0,1,0);
  myOrientation = tmpSFR;
}

Vrml_OrthographicCamera::Vrml_OrthographicCamera( const gp_Vec&          aPosition, 
						  const Vrml_SFRotation& aOrientation, 
						  const Standard_Real    aFocalDistance, 
						  const Standard_Real    aHeight)
{
    myPosition      = aPosition;
    myOrientation   = aOrientation;
    myFocalDistance = aFocalDistance;
    myHeight        = aHeight;
}

void Vrml_OrthographicCamera::SetPosition(const gp_Vec& aPosition)
{
    myPosition = aPosition;
}

gp_Vec Vrml_OrthographicCamera::Position() const 
{
   return  myPosition;
}

void Vrml_OrthographicCamera::SetOrientation(const Vrml_SFRotation& aOrientation)
{
   myOrientation   = aOrientation;
}

Vrml_SFRotation Vrml_OrthographicCamera::Orientation() const 
{
   return myOrientation; 
}

void Vrml_OrthographicCamera::SetFocalDistance(const Standard_Real aFocalDistance)
{
   myFocalDistance = aFocalDistance;
}

Standard_Real Vrml_OrthographicCamera::FocalDistance() const 
{
   return myFocalDistance;
}

void Vrml_OrthographicCamera::SetHeight(const Standard_Real aHeight)
{
    myHeight = aHeight;
}

Standard_Real Vrml_OrthographicCamera::Height() const 
{
   return myHeight;
}

Standard_OStream& Vrml_OrthographicCamera::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "OrthographicCamera {\n";
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
 if ( Abs(myHeight - 2) > 0.0001 )
   {
    anOStream  << "    height\t\t";
    anOStream << myHeight << "\n";
   }
 anOStream  << "}\n";
 return anOStream;
}
