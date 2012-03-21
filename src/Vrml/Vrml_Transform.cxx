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

#include <Vrml_Transform.ixx>

Vrml_Transform::Vrml_Transform()
{
  gp_Vec tmpV(0,0,0);
  myTranslation = tmpV;

  Vrml_SFRotation tmpSFR(0,0,1,0);
  myRotation = tmpSFR;

  tmpV.SetX(1);
  tmpV.SetY(1);
  tmpV.SetZ(1);
  myScaleFactor = tmpV;

  tmpSFR.SetRotationX(0);
  tmpSFR.SetRotationY(0);
  tmpSFR.SetRotationZ(1);
  tmpSFR.SetAngle(0);
  myScaleOrientation = tmpSFR;

  tmpV.SetX(0);
  tmpV.SetY(0);
  tmpV.SetZ(0);
  myCenter = tmpV;
}

Vrml_Transform::Vrml_Transform(const gp_Vec& aTranslation,
			       const Vrml_SFRotation& aRotation,
			       const gp_Vec& aScaleFactor,
			       const Vrml_SFRotation& aScaleOrientation,
			       const gp_Vec& aCenter)
{
  myTranslation = aTranslation;
  myRotation = aRotation;
  myScaleFactor = aScaleFactor;
  myScaleOrientation = aScaleOrientation;
  myCenter = aCenter;
}

 void Vrml_Transform::SetTranslation(const gp_Vec& aTranslation) 
{
  myTranslation = aTranslation;
}

 gp_Vec Vrml_Transform::Translation() const
{
  return  myTranslation;
}

 void Vrml_Transform::SetRotation(const Vrml_SFRotation& aRotation) 
{
  myRotation = aRotation;
}

 Vrml_SFRotation Vrml_Transform::Rotation() const
{
  return  myRotation;
}

 void Vrml_Transform::SetScaleFactor(const gp_Vec& aScaleFactor) 
{
  myScaleFactor = aScaleFactor;
}

 gp_Vec Vrml_Transform::ScaleFactor() const
{
  return myScaleFactor;
}

 void Vrml_Transform::SetScaleOrientation(const Vrml_SFRotation& aScaleOrientation) 
{
  myScaleOrientation = aScaleOrientation;
}

 Vrml_SFRotation Vrml_Transform::ScaleOrientation() const
{
  return  myScaleOrientation;
}

 void Vrml_Transform::SetCenter(const gp_Vec& aCenter) 
{
  myCenter = aCenter;
}

 gp_Vec Vrml_Transform::Center() const
{
  return  myCenter;
}

 Standard_OStream& Vrml_Transform::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Transform {" << endl;

 if ( Abs(myTranslation.X() - 0) > 0.0001 || 
     Abs(myTranslation.Y() - 0) > 0.0001 || 
     Abs(myTranslation.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    translation" << "\t\t";
    anOStream << myTranslation.X() << ' ' << myTranslation.Y() << ' ' << myTranslation.Z() << endl;
   }

 if ( Abs(myRotation.RotationX() - 0) > 0.0001 || 
      Abs(myRotation.RotationY() - 0) > 0.0001 || 
      Abs(myRotation.RotationZ() - 1) > 0.0001 ||
      Abs(myRotation.Angle() - 0) > 0.0001 ) 
   {
    anOStream  << "    rotation" << "\t\t";
    anOStream << myRotation.RotationX() << ' ' << myRotation.RotationY() << ' ';
    anOStream << myRotation.RotationZ() << ' ' << myRotation.Angle() << endl;
   }

 if ( Abs(myScaleFactor.X() - 1) > 0.0001 || 
     Abs(myScaleFactor.Y() - 1) > 0.0001 || 
     Abs(myScaleFactor.Z() - 1) > 0.0001 ) 
   {
    anOStream  << "    scaleFactor" << "\t\t";
    anOStream << myTranslation.X() << ' ' << myTranslation.Y() << ' ' << myTranslation.Z() << endl;
   }

 if ( Abs(myScaleOrientation.RotationX() - 0) > 0.0001 || 
     Abs(myScaleOrientation.RotationY() - 0) > 0.0001 || 
     Abs(myScaleOrientation.RotationZ() - 1) > 0.0001 || 
     Abs(myScaleOrientation.Angle() - 0) > 0.0001 ) 
   {
    anOStream  << "    scaleOrientation" << '\t';
    anOStream << myScaleOrientation.RotationX() << ' ' << myScaleOrientation.RotationY() << ' ';
    anOStream << myScaleOrientation.RotationZ() << ' ' << myScaleOrientation.Angle() << endl;
   }

 if ( Abs(myCenter.X() - 0) > 0.0001 || 
     Abs(myCenter.Y() - 0) > 0.0001 || 
     Abs(myCenter.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    center" << "\t\t";
    anOStream << myCenter.X() << ' ' << myCenter.Y() << ' ' << myCenter.Z() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
