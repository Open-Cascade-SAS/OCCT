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

#ifndef _V3d_DirectionalLight_HeaderFile
#define _V3d_DirectionalLight_HeaderFile

#include <V3d_PositionLight.hxx>
#include <V3d_TypeOfOrientation.hxx>

//! Directional light source for a viewer.
class V3d_DirectionalLight : public V3d_PositionLight
{
  DEFINE_STANDARD_RTTIEXT(V3d_DirectionalLight, V3d_PositionLight)
public:

  //! Creates a directional light source in the viewer.
  Standard_EXPORT V3d_DirectionalLight (const V3d_TypeOfOrientation theDirection = V3d_XposYposZpos,
                                        const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                        const Standard_Boolean theIsHeadlight = Standard_False);

  //! Creates a directional light source in the viewer.
  Standard_EXPORT V3d_DirectionalLight (const gp_Dir& theDirection,
                                        const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                        const Standard_Boolean theIsHeadlight = Standard_False);

  //! Defines the direction of the light source by a predefined orientation.
  Standard_EXPORT void SetDirection (V3d_TypeOfOrientation theDirection);
  using Graphic3d_CLight::SetDirection;

//! @name hidden properties not applicable to directional light
private:

  using Graphic3d_CLight::Position;
  using Graphic3d_CLight::SetPosition;
  using Graphic3d_CLight::ConstAttenuation;
  using Graphic3d_CLight::LinearAttenuation;
  using Graphic3d_CLight::Attenuation;
  using Graphic3d_CLight::SetAttenuation;
  using Graphic3d_CLight::Angle;
  using Graphic3d_CLight::SetAngle;
  using Graphic3d_CLight::Concentration;
  using Graphic3d_CLight::SetConcentration;

};

DEFINE_STANDARD_HANDLE(V3d_DirectionalLight, V3d_PositionLight)

#endif // _V3d_DirectionalLight_HeaderFile
