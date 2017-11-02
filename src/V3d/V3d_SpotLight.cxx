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

#include <V3d_SpotLight.hxx>

#include <V3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_SpotLight,V3d_PositionLight)

// =======================================================================
// function : V3d_SpotLight
// purpose  :
// =======================================================================
V3d_SpotLight::V3d_SpotLight (const gp_Pnt& thePos,
                              const V3d_TypeOfOrientation theDirection,
                              const Quantity_Color& theColor)
: V3d_PositionLight (Graphic3d_TOLS_SPOT, Handle(V3d_Viewer)())
{
  SetColor (theColor);
  SetPosition (thePos);
  SetDirection (V3d::GetProjAxis (theDirection));
}

// =======================================================================
// function : V3d_SpotLight
// purpose  :
// =======================================================================
V3d_SpotLight::V3d_SpotLight (const gp_Pnt& thePos,
                              const gp_Dir& theDirection,
                              const Quantity_Color& theColor)
: V3d_PositionLight (Graphic3d_TOLS_SPOT, Handle(V3d_Viewer)())
{
  SetColor (theColor);
  SetPosition (thePos);
  SetDirection (theDirection);
}

// =======================================================================
// function : V3d_SpotLight
// purpose  :
// =======================================================================
V3d_SpotLight::V3d_SpotLight (const Handle(V3d_Viewer)& theViewer,
                              const Standard_Real theX,
                              const Standard_Real theY,
                              const Standard_Real theZ,
                              const V3d_TypeOfOrientation theDirection,
                              const Quantity_Color& theColor,
                              const Standard_Real theConstAttenuation,
                              const Standard_Real theLinearAttenuation,
                              const Standard_Real theConcentration,
                              const Standard_Real theAngle)
: V3d_PositionLight (Graphic3d_TOLS_SPOT, theViewer)
{
  SetColor (theColor);
  SetPosition (theX, theY, theZ);
  SetDirection (V3d::GetProjAxis (theDirection));
  SetAttenuation ((float )theConstAttenuation, (float )theLinearAttenuation);
  SetConcentration ((float )theConcentration);
  SetAngle ((float )theAngle);
}

// =======================================================================
// function : V3d_SpotLight
// purpose  :
// =======================================================================
V3d_SpotLight::V3d_SpotLight (const Handle(V3d_Viewer)& theViewer,
                              const Standard_Real theXt,
                              const Standard_Real theYt,
                              const Standard_Real theZt,
                              const Standard_Real theXp,
                              const Standard_Real theYp,
                              const Standard_Real theZp,
                              const Quantity_Color& theColor,
                              const Standard_Real theConstAttenuation,
                              const Standard_Real theLinearAttenuation,
                              const Standard_Real theConcentration,
                              const Standard_Real theAngle)
: V3d_PositionLight (Graphic3d_TOLS_SPOT, theViewer)
{
  SetColor (theColor);
  SetPosition (theXp, theYp, theZp);
  SetDirection (theXt - theXp, theYt - theYp, theZt - theZp);
  SetAttenuation ((float )theConstAttenuation, (float )theLinearAttenuation);
  SetConcentration ((float )theConcentration);
  SetAngle ((float )theAngle);
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void V3d_SpotLight::SetDirection (V3d_TypeOfOrientation theDirection)
{
  SetDirection (V3d::GetProjAxis (theDirection));
}
