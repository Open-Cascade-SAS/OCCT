// Created on: 1992-01-22
// Created by: GG 
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _V3d_SpotLight_HeaderFile
#define _V3d_SpotLight_HeaderFile

#include <V3d_PositionLight.hxx>
#include <V3d_TypeOfOrientation.hxx>

//! Creation and modification of a spot.
//! The attenuation factor F determines the illumination of a surface:
//! @code
//!   F = 1/(ConstAttenuation() + LinearAttenuation() * Distance)
//! @endcode
//! Where Distance is the distance from the source to the surface.
//! The default values (1.0, 0.0) correspond to a minimum of attenuation.
//! The concentration factor determines the dispersion of the light on the surface, the default value (1.0) corresponds to a minimum of dispersion.
class V3d_SpotLight : public V3d_PositionLight
{
  DEFINE_STANDARD_RTTIEXT(V3d_SpotLight, V3d_PositionLight)
public:

  //! Creates a light source of the Spot type in the viewer with default attenuation factors (1.0, 0.0),
  //! concentration factor 1.0 and spot angle 30 degrees.
  Standard_EXPORT V3d_SpotLight (const gp_Pnt& thePos,
                                 const V3d_TypeOfOrientation theDirection = V3d_XnegYnegZpos,
                                 const Quantity_Color& theColor = Quantity_NOC_WHITE);

  //! Creates a light source of the Spot type in the viewer with default attenuation factors (1.0, 0.0),
  //! concentration factor 1.0 and spot angle 30 degrees.
  Standard_EXPORT V3d_SpotLight (const gp_Pnt& thePos,
                                 const gp_Dir& theDirection,
                                 const Quantity_Color& theColor = Quantity_NOC_WHITE);

  //! Defines the direction of the light source
  //! according to a predefined directional vector.
  Standard_EXPORT void SetDirection (V3d_TypeOfOrientation theOrientation);
  using Graphic3d_CLight::SetDirection;
  using Graphic3d_CLight::Position;
  using Graphic3d_CLight::SetPosition;

};

DEFINE_STANDARD_HANDLE(V3d_SpotLight, V3d_PositionLight)

#endif // _V3d_SpotLight_HeaderFile
