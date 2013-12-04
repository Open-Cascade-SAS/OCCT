// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Visual3d_Light.ixx>

#include <Graphic3d_GraphicDriver.hxx>

// =======================================================================
// function : Visual3d_Light
// purpose  :
// =======================================================================
Visual3d_Light::Visual3d_Light()
{
  myCLight.Type = Visual3d_TOLS_AMBIENT;
}

// =======================================================================
// function : Visual3d_Light
// purpose  :
// =======================================================================
Visual3d_Light::Visual3d_Light (const Quantity_Color& theColor)
{
  myCLight.Type      = Visual3d_TOLS_AMBIENT;
  myCLight.Color.r() = Standard_ShortReal (theColor.Red());
  myCLight.Color.g() = Standard_ShortReal (theColor.Green());
  myCLight.Color.b() = Standard_ShortReal (theColor.Blue());
}

// =======================================================================
// function : Visual3d_Light
// purpose  :
// =======================================================================
Visual3d_Light::Visual3d_Light (const Quantity_Color&   theColor,
                                const Graphic3d_Vector& theDir,
                                const Standard_Boolean  theIsHeadlight)
{
  Visual3d_LightDefinitionError_Raise_if (theDir.LengthZero(),
                                          "Bad value for LightDirection");
  myCLight.Type        = Visual3d_TOLS_DIRECTIONAL;
  myCLight.IsHeadlight = theIsHeadlight;
  myCLight.Color.r()   = Standard_ShortReal (theColor.Red());
  myCLight.Color.g()   = Standard_ShortReal (theColor.Green());
  myCLight.Color.b()   = Standard_ShortReal (theColor.Blue());

  Standard_Real X, Y, Z;
  theDir.Coord (X, Y, Z);
  const Standard_Real aNorm = Sqrt (X * X + Y * Y + Z * Z);
  myCLight.Direction.x() = Standard_ShortReal (X / aNorm);
  myCLight.Direction.y() = Standard_ShortReal (Y / aNorm);
  myCLight.Direction.z() = Standard_ShortReal (Z / aNorm);
}

// =======================================================================
// function : Visual3d_Light
// purpose  :
// =======================================================================
Visual3d_Light::Visual3d_Light (const Quantity_Color&   theColor,
                                const Graphic3d_Vertex& thePos,
                                const Standard_Real     theFact1,
                                const Standard_Real     theFact2)
{
  Visual3d_LightDefinitionError_Raise_if ((theFact1 == 0.0 && theFact2 == 0.0)
                                       || (theFact1  < 0.0 || theFact1 >  1.0)
                                       || (theFact2  < 0.0 || theFact2 >  1.0),
                                          "Bad value for LightAttenuation");
  myCLight.Type         = Visual3d_TOLS_POSITIONAL;
  myCLight.IsHeadlight  = Standard_False;
  myCLight.Color.r()    = Standard_ShortReal (theColor.Red());
  myCLight.Color.g()    = Standard_ShortReal (theColor.Green());
  myCLight.Color.b()    = Standard_ShortReal (theColor.Blue());
  myCLight.Position.x() = Standard_ShortReal (thePos.X());
  myCLight.Position.y() = Standard_ShortReal (thePos.Y());
  myCLight.Position.z() = Standard_ShortReal (thePos.Z());
  myCLight.ChangeConstAttenuation()  = Standard_ShortReal (theFact1);
  myCLight.ChangeLinearAttenuation() = Standard_ShortReal (theFact2);
}

// =======================================================================
// function : Visual3d_Light
// purpose  :
// =======================================================================
Visual3d_Light::Visual3d_Light (const Quantity_Color&   theColor,
                                const Graphic3d_Vertex& thePos,
                                const Graphic3d_Vector& theDir,
                                const Standard_Real     theConcentration,
                                const Standard_Real     theFact1,
                                const Standard_Real     theFact2,
                                const Standard_Real     theAngleCone)
{
  Visual3d_LightDefinitionError_Raise_if (theDir.LengthZero(),
                                          "Bad value for LightDirection");
  Visual3d_LightDefinitionError_Raise_if (theConcentration < 0.0 || theConcentration > 1.0,
                                          "Bad value for LightConcentration");
  Visual3d_LightDefinitionError_Raise_if ((theFact1 == 0.0 && theFact2 == 0.0)
                                       || (theFact1  < 0.0 || theFact1 >  1.0)
                                       || (theFact2  < 0.0 || theFact2 >  1.0),
                                          "Bad value for LightAttenuation");
  Visual3d_LightDefinitionError_Raise_if (!Visual3d_Light::IsValid (theAngleCone),
                                          "Bad value for LightAngle");
  myCLight.Type         = Visual3d_TOLS_SPOT;
  myCLight.IsHeadlight  = Standard_False;
  myCLight.Color.r()    = Standard_ShortReal (theColor.Red());
  myCLight.Color.g()    = Standard_ShortReal (theColor.Green());
  myCLight.Color.b()    = Standard_ShortReal (theColor.Blue());
  myCLight.Position.x() = Standard_ShortReal (thePos.X());
  myCLight.Position.y() = Standard_ShortReal (thePos.Y());
  myCLight.Position.z() = Standard_ShortReal (thePos.Z());

  Standard_Real X, Y, Z;
  theDir.Coord (X, Y, Z);
  myCLight.Direction.x() = Standard_ShortReal (X);
  myCLight.Direction.y() = Standard_ShortReal (Y);
  myCLight.Direction.z() = Standard_ShortReal (Z);

  myCLight.ChangeConcentration()     = Standard_ShortReal (theConcentration);
  myCLight.ChangeConstAttenuation()  = Standard_ShortReal (theFact1);
  myCLight.ChangeLinearAttenuation() = Standard_ShortReal (theFact2);
  myCLight.ChangeAngle()             = Standard_ShortReal (theAngleCone);
}

// =======================================================================
// function : Color
// purpose  :
// =======================================================================
Quantity_Color Visual3d_Light::Color() const
{
  return Quantity_Color (Standard_Real (myCLight.Color.r()),
                         Standard_Real (myCLight.Color.g()),
                         Standard_Real (myCLight.Color.b()),
                         Quantity_TOC_RGB);
}

// =======================================================================
// function : LightType
// purpose  :
// =======================================================================
Visual3d_TypeOfLightSource Visual3d_Light::LightType() const
{
  return (Visual3d_TypeOfLightSource )myCLight.Type;
}

// =======================================================================
// function : Headlight
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_Light::Headlight() const
{
  return myCLight.IsHeadlight;
}

// =======================================================================
// function : SetHeadlight
// purpose  :
// =======================================================================
void Visual3d_Light::SetHeadlight (const Standard_Boolean theValue)
{
  myCLight.IsHeadlight = theValue;
}

// =======================================================================
// function : Values
// purpose  :
// =======================================================================
void Visual3d_Light::Values (Quantity_Color& theColor) const
{
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_AMBIENT,
                                          "Light Type != Visual3d_TOLS_AMBIENT");
  theColor.SetValues (Standard_Real (myCLight.Color.r()),
                      Standard_Real (myCLight.Color.g()),
                      Standard_Real (myCLight.Color.b()),
                      Quantity_TOC_RGB);
}

// =======================================================================
// function : Values
// purpose  :
// =======================================================================
void Visual3d_Light::Values (Quantity_Color&   theColor,
                             Graphic3d_Vector& theDir) const
{
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_DIRECTIONAL,
                                          "Light Type != Visual3d_TOLS_DIRECTIONAL");
  theColor.SetValues (Standard_Real (myCLight.Color.r()),
                      Standard_Real (myCLight.Color.g()),
                      Standard_Real (myCLight.Color.b()),
                      Quantity_TOC_RGB);
  theDir.SetCoord (Standard_Real (myCLight.Direction.x()),
                   Standard_Real (myCLight.Direction.y()),
                   Standard_Real (myCLight.Direction.z()));
}

// =======================================================================
// function : Values
// purpose  :
// =======================================================================
void Visual3d_Light::Values (Quantity_Color&   theColor,
                             Graphic3d_Vertex& thePos,
                             Standard_Real&    theFact1,
                             Standard_Real&    theFact2) const
{
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_POSITIONAL,
                                          "Light Type != Visual3d_TOLS_POSITIONAL");
  theColor.SetValues (Standard_Real (myCLight.Color.r()),
                      Standard_Real (myCLight.Color.g()),
                      Standard_Real (myCLight.Color.b()),
                      Quantity_TOC_RGB);
  thePos.SetCoord (Standard_Real (myCLight.Position.x()),
                   Standard_Real (myCLight.Position.y()),
                   Standard_Real (myCLight.Position.z()));
  theFact1 = Standard_Real (myCLight.ConstAttenuation());
  theFact2 = Standard_Real (myCLight.LinearAttenuation());
}

// =======================================================================
// function : Values
// purpose  :
// =======================================================================
void Visual3d_Light::Values (Quantity_Color&   theColor,
                             Graphic3d_Vertex& thePos,
                             Graphic3d_Vector& theDir,
                             Standard_Real&    theConcentration,
                             Standard_Real&    theFact1,
                             Standard_Real&    theFact2,
                             Standard_Real&    theAngleCone) const
{
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_SPOT,
                                          "Light Type != Visual3d_TOLS_SPOT");
  theColor.SetValues (Standard_Real (myCLight.Color.r()),
                      Standard_Real (myCLight.Color.g()),
                      Standard_Real (myCLight.Color.b()),
                      Quantity_TOC_RGB);
  thePos.SetCoord (Standard_Real (myCLight.Position.x()),
                   Standard_Real (myCLight.Position.y()),
                   Standard_Real (myCLight.Position.z()));
  theDir.SetCoord (Standard_Real (myCLight.Direction.x()),
                   Standard_Real (myCLight.Direction.y()),
                   Standard_Real (myCLight.Direction.z()));
  theConcentration = Standard_Real (myCLight.Concentration());
  theFact1         = Standard_Real (myCLight.ConstAttenuation());
  theFact2         = Standard_Real (myCLight.LinearAttenuation());
  theAngleCone     = Standard_Real (myCLight.Angle());
}

// =======================================================================
// function : SetAngle
// purpose  :
// =======================================================================
void Visual3d_Light::SetAngle (const Standard_Real theAngleCone)
{
  Visual3d_LightDefinitionError_Raise_if (!Visual3d_Light::IsValid (theAngleCone),
                                          "Bad value for LightAngle");
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_SPOT,
                                          "Light Type != Visual3d_TOLS_SPOT");
  myCLight.ChangeAngle() = Standard_ShortReal (theAngleCone);
}

// =======================================================================
// function : SetAttenuation1
// purpose  :
// =======================================================================
void Visual3d_Light::SetAttenuation1 (const Standard_Real theFact1)
{
  Visual3d_LightDefinitionError_Raise_if (theFact1 < 0.0 || theFact1 > 1.0,
                                          "Bad value for LightAttenuation");
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_POSITIONAL
                                       && myCLight.Type != Visual3d_TOLS_SPOT,
                                          "Light Type != Visual3d_TOLS_SPOT and != Visual3d_TOLS_POSITIONAL");
  myCLight.ChangeConstAttenuation() = Standard_ShortReal (theFact1);
}

// =======================================================================
// function : SetAttenuation2
// purpose  :
// =======================================================================
void Visual3d_Light::SetAttenuation2 (const Standard_Real theFact2)
{
  Visual3d_LightDefinitionError_Raise_if (theFact2 < 0.0 || theFact2 > 1.0,
                                          "Bad value for LightAttenuation");
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_POSITIONAL
                                       && myCLight.Type != Visual3d_TOLS_SPOT,
                                          "Light Type != Visual3d_TOLS_SPOT and != Visual3d_TOLS_POSITIONAL");
  myCLight.ChangeLinearAttenuation() = Standard_ShortReal (theFact2);
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void Visual3d_Light::SetColor (const Quantity_Color& theColor)
{
  myCLight.Color.r() = float (theColor.Red());
  myCLight.Color.g() = float (theColor.Green());
  myCLight.Color.b() = float (theColor.Blue());
}

// =======================================================================
// function : SetConcentration
// purpose  :
// =======================================================================
void Visual3d_Light::SetConcentration (const Standard_Real theConcentration)
{
  Visual3d_LightDefinitionError_Raise_if (theConcentration < 0.0 || theConcentration > 1.0,
                                          "Bad value for LightConcentration");
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_SPOT,
                                          "Light Type != Visual3d_TOLS_SPOT");
  myCLight.ChangeConcentration() = Standard_ShortReal (theConcentration);
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void Visual3d_Light::SetDirection (const Graphic3d_Vector& theDir)
{
  Visual3d_LightDefinitionError_Raise_if (theDir.LengthZero(),
                                          "Bad value for LightDirection");
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_SPOT
                                       && myCLight.Type != Visual3d_TOLS_DIRECTIONAL,
                                          "Light Type != Visual3d_TOLS_DIRECTIONAL and != Visual3d_TOLS_SPOT");
  Standard_Real X, Y, Z;
  theDir.Coord (X, Y, Z);
  const Standard_Real aNorm = Sqrt (X * X + Y * Y + Z * Z);
  myCLight.Direction.x() = float (X / aNorm);
  myCLight.Direction.y() = float (Y / aNorm);
  myCLight.Direction.z() = float (Z / aNorm);
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void Visual3d_Light::SetPosition (const Graphic3d_Vertex& thePos)
{
  Visual3d_LightDefinitionError_Raise_if (myCLight.Type != Visual3d_TOLS_SPOT
                                       && myCLight.Type != Visual3d_TOLS_POSITIONAL,
                                          "Light Type != Visual3d_TOLS_POSITIONAL and != Visual3d_TOLS_SPOT");
  myCLight.Position.x() = float (thePos.X());
  myCLight.Position.y() = float (thePos.Y());
  myCLight.Position.z() = float (thePos.Z());
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
Standard_Boolean Visual3d_Light::IsValid (const Standard_Real theAngle)
{
  return (theAngle <  M_PI)
      && (theAngle >= 0.0);
}

// =======================================================================
// function : CLight
// purpose  :
// =======================================================================
const Graphic3d_CLight& Visual3d_Light::CLight() const
{
  return myCLight;
}
