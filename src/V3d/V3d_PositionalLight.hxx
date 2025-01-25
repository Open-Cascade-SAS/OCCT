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

#ifndef _V3d_PositionalLight_HeaderFile
#define _V3d_PositionalLight_HeaderFile

#include <V3d_PositionLight.hxx>

//! Creation and modification of an isolated (positional) light source.
//! It is also defined by the color and two attenuation factors ConstAttentuation() and
//! LinearAttentuation(). The resulting attenuation factor determining the illumination of a surface
//! depends on the following formula:
//! @code
//!   F = 1 / (ConstAttenuation() + LinearAttenuation() * Distance)
//! @endcode
//! Where Distance is the distance of the isolated source from the surface.
class V3d_PositionalLight : public V3d_PositionLight
{
  DEFINE_STANDARD_RTTIEXT(V3d_PositionalLight, V3d_PositionLight)
public:
  //! Creates an isolated light source in the viewer with default attenuation factors (1.0, 0.0).
  Standard_EXPORT V3d_PositionalLight(const gp_Pnt&         thePos,
                                      const Quantity_Color& theColor = Quantity_NOC_WHITE);

  using Graphic3d_CLight::Position;
  using Graphic3d_CLight::SetPosition;

  //! @name hidden properties not applicable to positional light
private:
  using Graphic3d_CLight::Angle;
  using Graphic3d_CLight::Concentration;
  using Graphic3d_CLight::Direction;
  using Graphic3d_CLight::SetAngle;
  using Graphic3d_CLight::SetConcentration;
  using Graphic3d_CLight::SetDirection;
};

DEFINE_STANDARD_HANDLE(V3d_PositionalLight, V3d_PositionLight)

#endif // _V3d_PositionalLight_HeaderFile
