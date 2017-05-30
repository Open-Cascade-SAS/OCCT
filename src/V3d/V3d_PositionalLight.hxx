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

class V3d_PositionalLight;
DEFINE_STANDARD_HANDLE(V3d_PositionalLight, V3d_PositionLight)

//! Creation and modification of an isolated
//! (positional) light source.
class V3d_PositionalLight : public V3d_PositionLight
{
public:

  //! Creates an isolated light source theX, theY, theZ in the viewer.
  //! It is also defined by the color theColor and
  //! two attenuation factors theConstAttentuation, theLinearAttentuation.
  //! The resulting attenuation factor determining the
  //! illumination of a surface depends on the following
  //! formula :
  //! F = 1/(ConstAttenuation + LinearAttenuation*Length)
  //! Length is the distance of the isolated source
  //! from the surface.  //! Warning!  raises BadValue from V3d
  //! if one of the attenuation coefficients is not in range [0, 1].
  Standard_EXPORT V3d_PositionalLight (const Handle(V3d_Viewer)& theViewer,
                                       const Standard_Real theX,
                                       const Standard_Real theY,
                                       const Standard_Real theZ,
                                       const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                       const Standard_Real theConstAttenuation = 1.0,
                                       const Standard_Real theLinearAttenuation = 0.0);

  //! Creates a light source of the Positional type in the viewer.
  //! theXt, theYt, theZt : Coordinate of Target light source.
  //! theXp, theYp, theZp : Coordinate of Position light source.
  //! The light source is also defined by the color Color
  //! and two attenuation factors theConstAttentuation,
  //! theLinearAttentuation that determine the illumination of a
  //! surface using the following formula :
  //! F = 1/(ConstAttenuation + LinearAttenuation*Length)
  //! Length is the distance of the isolated source
  //! from the surface.  //! Warning! raises BadValue from V3d
  //! if one of the attenuation coefficients is not between 0 et 1.
  Standard_EXPORT V3d_PositionalLight (const Handle(V3d_Viewer)& theViewer,
                                       const Standard_Real theXt,
                                       const Standard_Real theYt,
                                       const Standard_Real theZt,
                                       const Standard_Real theXp,
                                       const Standard_Real theYp,
                                       const Standard_Real theZp,
                                       const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                       const Standard_Real theConstAttenuation = 1.0,
                                       const Standard_Real theLinearAttenuation = 0.0);

  //! Defines the position of the light source.
  Standard_EXPORT virtual void SetPosition (const Standard_Real theX,
                                            const Standard_Real theY,
                                            const Standard_Real theZ) Standard_OVERRIDE;

  //! Defines the attenuation factors.
  //! Warning: raises BadValue from V3d
  //! if one of the attenuation coefficients is not between 0 et 1.
  Standard_EXPORT void SetAttenuation (const Standard_Real theConstAttenuation,
                                       const Standard_Real theLinearAttenuation);

  //! Modifies the smoothing radius
  Standard_EXPORT void SetSmoothRadius (const Standard_Real theValue);

  //! Display the graphic structure of light source
  //! in the chosen view. We have three type of representation
  //! - SIMPLE   : Only the light source is displayed.
  //! - PARTIAL  : The light source and the light space are
  //! displayed.
  //! - COMPLETE : The light source, the light space and the
  //! radius of light space are displayed.
  //! We can choose the "SAMELAST" as parameter of representation
  //! In this case the graphic structure representation will be
  //! the last displayed.
  Standard_EXPORT void Display (const Handle(V3d_View)& theView,
                                const V3d_TypeOfRepresentation theRepresentation) Standard_OVERRIDE;

  //! Returns the position of the light source.
  Standard_EXPORT void Position (Standard_Real& theX,
                                 Standard_Real& theY,
                                 Standard_Real& theZ) const Standard_OVERRIDE;

  //! Returns the attenuation factors.
  Standard_EXPORT void Attenuation (Standard_Real& theConstAttenuation,
                                    Standard_Real& theLinearAttenuation) const;

  DEFINE_STANDARD_RTTIEXT(V3d_PositionalLight,V3d_PositionLight)

private:

  //! Defined the representation of the positional light source.
  Standard_EXPORT void Symbol (const Handle(Graphic3d_Group)& theSymbol,
                               const Handle(V3d_View)& theView) const Standard_OVERRIDE;
};

#endif // _V3d_PositionalLight_HeaderFile
