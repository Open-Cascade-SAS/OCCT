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
#include <Quantity_Color.hxx>
#include <Vrml_SpotLight.hxx>

Vrml_SpotLight::Vrml_SpotLight():
  myOnOff(Standard_True),
  myIntensity(1),
  myDropOffRate(0), 
  myCutOffAngle(0.785398) 
{
  gp_Vec tmpVec(0,0,1);
  myLocation = tmpVec;

  tmpVec.SetCoord(0,0,-1);
  myDirection = tmpVec;

  Quantity_Color tmpColor(1,1,1,Quantity_TOC_RGB);
  myColor = tmpColor;
}

 Vrml_SpotLight::Vrml_SpotLight( const Standard_Boolean aOnOff, 
				 const Standard_Real aIntensity, 
			         const Quantity_Color& aColor, 
			         const gp_Vec& aLocation,
			         const gp_Vec& aDirection,
				 const Standard_Real aDropOffRate, 
				 const Standard_Real aCutOffAngle) 
{
  myOnOff = aOnOff;
  if (aIntensity < 0. || aIntensity > 1.)
    {
      throw Standard_Failure("Error : Light intensity must be in the range 0.0 to 1.0, inclusive.");
    }
  myIntensity = aIntensity;
  myColor = aColor;
  myLocation = aLocation;
  myDirection = aDirection;
  myDropOffRate = aDropOffRate;
  myCutOffAngle = aCutOffAngle;
}

void Vrml_SpotLight::SetOnOff(const Standard_Boolean aOnOff)
{
  myOnOff = aOnOff;
}

Standard_Boolean Vrml_SpotLight::OnOff() const 
{
  return myOnOff;
}

void Vrml_SpotLight::SetIntensity(const Standard_Real aIntensity)
{
  if (aIntensity < 0. || aIntensity > 1.)
    {
      throw Standard_Failure("Error : Light intensity must be in the range 0.0 to 1.0, inclusive.");
    }
  myIntensity = aIntensity;
}

Standard_Real Vrml_SpotLight::Intensity() const 
{
  return myIntensity;
}

void Vrml_SpotLight::SetColor(const Quantity_Color& aColor)
{
  myColor = aColor;
}

Quantity_Color Vrml_SpotLight::Color() const 
{
  return  myColor;
}

void Vrml_SpotLight::SetLocation(const gp_Vec& aLocation)
{
  myLocation = aLocation;
}

gp_Vec Vrml_SpotLight::Location() const 
{
  return myLocation;
}

void Vrml_SpotLight::SetDirection(const gp_Vec& aDirection)
{
  myDirection = aDirection;
}

gp_Vec Vrml_SpotLight::Direction() const 
{
  return myDirection;
}

void Vrml_SpotLight::SetDropOffRate(const Standard_Real aDropOffRate)
{
  myDropOffRate = aDropOffRate;
}

Standard_Real Vrml_SpotLight::DropOffRate() const 
{
  return myDropOffRate;
}

void Vrml_SpotLight::SetCutOffAngle(const Standard_Real aCutOffAngle)
{
  myCutOffAngle = aCutOffAngle;
}

Standard_Real Vrml_SpotLight::CutOffAngle() const 
{
  return myCutOffAngle;
}

Standard_OStream& Vrml_SpotLight::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "SpotLight {\n";

 if ( myOnOff != Standard_True )
   {
    anOStream  << "    on\t\tFALSE\n";
//    anOStream << myOnOff << "\n";
   }

 if ( Abs(myIntensity - 1) > 0.0001 )
   {
    anOStream  << "    intensity\t";
    anOStream << myIntensity << "\n";
   }

 if ( Abs(myColor.Red() - 1) > 0.0001 || 
     Abs(myColor.Green() - 1) > 0.0001 || 
     Abs(myColor.Blue() - 1) > 0.0001 )
   {
    anOStream  << "    color\t";
    anOStream << myColor.Red() << " " << myColor.Green() << " " << myColor.Blue() << "\n";
   }

 if ( Abs(myLocation.X() - 0) > 0.0001 || 
     Abs(myLocation.Y() - 0) > 0.0001 || 
     Abs(myLocation.Z() - 1) > 0.0001 ) 
   {
    anOStream  << "    location\t";
    anOStream << myLocation.X() << " " << myLocation.Y() << " " << myLocation.Z() << "\n";
   }

 if ( Abs(myDirection.X() - 0) > 0.0001 || 
     Abs(myDirection.Y() - 0) > 0.0001 || 
     Abs(myDirection.Z() + 1) > 0.0001 ) 
   {
    anOStream  << "    direction\t";
    anOStream << myDirection.X() << " " << myDirection.Y() << " " << myDirection.Z() << "\n";
   }

 if ( Abs(myDropOffRate - 0) > 0.0001 )
   {
    anOStream  << "    dropOffRate\t";
    anOStream << myDropOffRate << "\n";
   }

 if ( Abs(myCutOffAngle - 0.785398) > 0.0000001 )
   {
    anOStream  << "    cutOffAngle\t";
    anOStream << myCutOffAngle << "\n";
   }

 anOStream  << "}\n";
 return anOStream;
}
