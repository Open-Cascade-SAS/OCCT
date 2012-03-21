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

#include <Vrml_Texture2Transform.ixx>

Vrml_Texture2Transform::Vrml_Texture2Transform()
{
  gp_Vec2d tmpVec(0,0);
  myTranslation = tmpVec;
  myCenter = tmpVec; 

  myRotation = 0;

  tmpVec.SetCoord(1,1);
  myScaleFactor = tmpVec;
}

Vrml_Texture2Transform::Vrml_Texture2Transform(const gp_Vec2d& aTranslation,
					       const Standard_Real aRotation,
					       const gp_Vec2d& aScaleFactor,
					       const gp_Vec2d& aCenter)
{
  myTranslation = aTranslation;
  myRotation = aRotation;
  myScaleFactor = aScaleFactor;
  myCenter = aCenter; 
}

 void Vrml_Texture2Transform::SetTranslation(const gp_Vec2d& aTranslation) 
{
  myTranslation = aTranslation;
}

 gp_Vec2d Vrml_Texture2Transform::Translation() const
{
  return myTranslation;
}

 void Vrml_Texture2Transform::SetRotation(const Standard_Real aRotation) 
{
  myRotation = aRotation;
}

 Standard_Real Vrml_Texture2Transform::Rotation() const
{
  return myRotation;
}

 void Vrml_Texture2Transform::SetScaleFactor(const gp_Vec2d& aScaleFactor) 
{
  myScaleFactor = aScaleFactor;
}

 gp_Vec2d Vrml_Texture2Transform::ScaleFactor() const
{
  return myScaleFactor;
}

 void Vrml_Texture2Transform::SetCenter(const gp_Vec2d& aCenter) 
{
  myCenter = aCenter; 
}

 gp_Vec2d Vrml_Texture2Transform::Center() const
{
  return myCenter;
}

 Standard_OStream& Vrml_Texture2Transform::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Texture2Transform {" << endl;

 if ( Abs(myTranslation.X() - 0) > 0.0001 || Abs(myTranslation.Y() - 0) > 0.0001 ) 
   {
    anOStream  << "    translation" << '\t';
    anOStream << myTranslation.X() << ' ' << myTranslation.Y() << endl;
   }

 if ( Abs(myRotation - 0) > 0.0001 )
   {
    anOStream  << "    rotation" << '\t';
    anOStream << myRotation << endl;
   }

 if ( Abs(myScaleFactor.X() - 0) > 0.0001 || Abs(myScaleFactor.Y() - 0) > 0.0001 ) 
   {
    anOStream  << "    scaleFactor" << '\t';
    anOStream << myScaleFactor.X() << ' ' << myScaleFactor.Y() << endl;
   }

 if ( Abs(myCenter.X() - 0) > 0.0001 || Abs(myCenter.Y() - 0) > 0.0001 ) 
   {
    anOStream  << "    center" << '\t';
    anOStream << myCenter.X() << ' ' << myCenter.Y() << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;

}
