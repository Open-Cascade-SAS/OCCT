// Created on: 1992-01-21
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

#ifndef _V3d_AmbientLight_HeaderFile
#define _V3d_AmbientLight_HeaderFile

#include <V3d_Light.hxx>

class V3d_Viewer;

//! Creation of an ambient light source in a viewer.
class V3d_AmbientLight : public Graphic3d_CLight
{
  DEFINE_STANDARD_RTTIEXT(V3d_AmbientLight, Graphic3d_CLight)
public:

  //! Constructs an ambient light source in the viewer.
  //! The default Color of this light source is WHITE.
  Standard_EXPORT V3d_AmbientLight (const Quantity_Color& theColor = Quantity_NOC_WHITE);

//! @name hidden properties not applicable to ambient light
private:

  using Graphic3d_CLight::IsHeadlight;
  using Graphic3d_CLight::Headlight;
  using Graphic3d_CLight::SetHeadlight;
  using Graphic3d_CLight::Position;
  using Graphic3d_CLight::SetPosition;
  using Graphic3d_CLight::ConstAttenuation;
  using Graphic3d_CLight::LinearAttenuation;
  using Graphic3d_CLight::Attenuation;
  using Graphic3d_CLight::SetAttenuation;
  using Graphic3d_CLight::Direction;
  using Graphic3d_CLight::SetDirection;
  using Graphic3d_CLight::Angle;
  using Graphic3d_CLight::SetAngle;
  using Graphic3d_CLight::Concentration;
  using Graphic3d_CLight::SetConcentration;
  using Graphic3d_CLight::Smoothness;
  using Graphic3d_CLight::SetSmoothRadius;
  using Graphic3d_CLight::SetSmoothAngle;

};

DEFINE_STANDARD_HANDLE(V3d_AmbientLight, Graphic3d_CLight)

#endif // _V3d_AmbientLight_HeaderFile
