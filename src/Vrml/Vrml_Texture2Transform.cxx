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


#include <gp_Vec2d.hxx>
#include <Vrml_Texture2Transform.hxx>

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
 anOStream  << "Texture2Transform {\n";

 if ( Abs(myTranslation.X() - 0) > 0.0001 || Abs(myTranslation.Y() - 0) > 0.0001 ) 
   {
    anOStream  << "    translation\t";
    anOStream << myTranslation.X() << " " << myTranslation.Y() << "\n";
   }

 if ( Abs(myRotation - 0) > 0.0001 )
   {
    anOStream  << "    rotation\t";
    anOStream << myRotation << "\n";
   }

 if ( Abs(myScaleFactor.X() - 0) > 0.0001 || Abs(myScaleFactor.Y() - 0) > 0.0001 ) 
   {
    anOStream  << "    scaleFactor\t";
    anOStream << myScaleFactor.X() << " " << myScaleFactor.Y() << "\n";
   }

 if ( Abs(myCenter.X() - 0) > 0.0001 || Abs(myCenter.Y() - 0) > 0.0001 ) 
   {
    anOStream  << "    center\t";
    anOStream << myCenter.X() << " " << myCenter.Y() << "\n";
   }

 anOStream  << "}\n";
 return anOStream;

}
