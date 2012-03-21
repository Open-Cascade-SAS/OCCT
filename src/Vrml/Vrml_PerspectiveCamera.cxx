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

#include <Vrml_PerspectiveCamera.ixx>

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
 anOStream  << "PerspectiveCamera {" << endl;
 if ( Abs(myPosition.X() - 0) > 0.0001 || 
      Abs(myPosition.Y() - 0) > 0.0001 || 
      Abs(myPosition.Z() - 1) > 0.0001 )
   {
    anOStream  << "    position" << "\t\t";
    anOStream << myPosition.X() << ' ' << myPosition.Y() << ' ' << myPosition.Z() << endl;
   }

 if ( Abs(myOrientation.RotationX() - 0) > 0.0001 || 
     Abs(myOrientation.RotationY() - 0) > 0.0001 || 
     Abs(myOrientation.RotationZ() - 1) > 0.0001 ||
     Abs(myOrientation.Angle() - 0) > 0.0001 )
   {
    anOStream  << "    orientation" << "\t\t";
    anOStream << myOrientation.RotationX() << ' ' << myOrientation.RotationY() << ' ';
    anOStream << myOrientation.RotationZ() << ' ' << myOrientation.Angle() << endl;
   }

 if ( Abs(myFocalDistance - 5) > 0.0001 )
   {
    anOStream  << "    focalDistance" << '\t';
    anOStream << myFocalDistance << endl;
   }
 if ( Abs(myHeightAngle - 0.785398) > 0.0000001 )
   {
    anOStream  << "    heightAngle" << "\t\t";
    anOStream << myHeightAngle << endl;
   }
 anOStream  << '}' << endl;
 return anOStream;
}
