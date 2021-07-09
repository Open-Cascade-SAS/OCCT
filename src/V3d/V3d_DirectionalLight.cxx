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

#include <V3d_DirectionalLight.hxx>

#include <V3d.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_DirectionalLight,V3d_PositionLight)

// =======================================================================
// function : V3d_DirectionalLight
// purpose  :
// =======================================================================
V3d_DirectionalLight::V3d_DirectionalLight (const V3d_TypeOfOrientation theDirection,
                                            const Quantity_Color& theColor,
                                            const Standard_Boolean theIsHeadlight)
: V3d_PositionLight (Graphic3d_TypeOfLightSource_Directional)
{
  SetColor (theColor);
  SetHeadlight (theIsHeadlight);
  SetDirection (V3d::GetProjAxis (theDirection));
}

// =======================================================================
// function : V3d_DirectionalLight
// purpose  :
// =======================================================================
V3d_DirectionalLight::V3d_DirectionalLight (const gp_Dir& theDirection,
                                            const Quantity_Color& theColor,
                                            const Standard_Boolean theIsHeadlight)
: V3d_PositionLight (Graphic3d_TypeOfLightSource_Directional)
{
  SetColor (theColor);
  SetHeadlight (theIsHeadlight);
  SetDirection (theDirection);
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void V3d_DirectionalLight::SetDirection (V3d_TypeOfOrientation theDirection)
{
  SetDirection (V3d::GetProjAxis (theDirection));
}
