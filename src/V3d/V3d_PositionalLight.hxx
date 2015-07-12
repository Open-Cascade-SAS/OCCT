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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <V3d_PositionLight.hxx>
#include <V3d_Coordinate.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Coefficient.hxx>
#include <Standard_Real.hxx>
#include <V3d_TypeOfRepresentation.hxx>
class V3d_BadValue;
class V3d_Viewer;
class V3d_View;
class Graphic3d_Group;


class V3d_PositionalLight;
DEFINE_STANDARD_HANDLE(V3d_PositionalLight, V3d_PositionLight)

//! Creation and modification of an isolated
//! (positional) light source.
class V3d_PositionalLight : public V3d_PositionLight
{

public:

  
  //! Creates an isolated light source X,Y,Z in the viewer.
  //! It is also defined by the color Color and
  //! two attenuation factors Attenuation1, Attenuation2.
  //! The resulting attenuation factor determining the
  //! illumination of a surface depends on the following
  //! formula :
  //! F = 1/(A1 + A2*Length)
  //! A1,A2 being the two factors of attenuation
  //! Length is the distance of the isolated source
  //! from the surface.
  //! Warning!  raises BadValue from V3d
  //! if one of the attenuation coefficients is not between 0 et 1.
  Standard_EXPORT V3d_PositionalLight(const Handle(V3d_Viewer)& VM, const V3d_Coordinate X, const V3d_Coordinate Y, const V3d_Coordinate Z, const Quantity_NameOfColor Color = Quantity_NOC_WHITE, const Quantity_Coefficient Attenuation1 = 1.0, const Quantity_Coefficient Attenuation2 = 0.0);
  
  //! Creates a light source of the Positional type
  //! in the viewer.
  //! Xt,Yt,Zt : Coordinate of Target light source.
  //! Xp,Yp,Zp : Coordinate of Position light source.
  //! The light source is also defined by the color Color
  //! and two attenuation factors Attenuation1,
  //! Attenuation2 that determine the illumination of a
  //! surface using the following formula :
  //! F = 1/(A1 + A2*Length) where:
  //! -   A1,A2 are the two attenuation factors, and
  //! -   Length is the distance from the isolated source.
  //! Warning! raises BadValue from V3d
  //! if one of the attenuation coefficients is not between 0 et 1.
  Standard_EXPORT V3d_PositionalLight(const Handle(V3d_Viewer)& VM, const V3d_Coordinate Xt, const V3d_Coordinate Yt, const V3d_Coordinate Zt, const V3d_Coordinate Xp, const V3d_Coordinate Yp, const V3d_Coordinate Zp, const Quantity_NameOfColor Color = Quantity_NOC_WHITE, const Quantity_Coefficient Attenuation1 = 1.0, const Quantity_Coefficient Attenuation2 = 0.0);
  
  //! Defines the position of the light source.
  Standard_EXPORT virtual void SetPosition (const V3d_Coordinate X, const V3d_Coordinate Y, const V3d_Coordinate Z) Standard_OVERRIDE;
  
  //! Defines the attenuation factors.
  //! Warning: raises BadValue from V3d
  //! if one of the attenuation coefficients is not between 0 et 1.
  Standard_EXPORT void SetAttenuation (const Quantity_Coefficient A1, const Quantity_Coefficient A2);
  
  //! Modifies the smoothing radius
  Standard_EXPORT void SetSmoothRadius (const Standard_Real theValue);
  
  //! Display the graphic structure of light source
  //! in the choosen view. We have three type of representation
  //! - SIMPLE   : Only the light source is displayed.
  //! - PARTIAL  : The light source and the light space are
  //! displayed.
  //! - COMPLETE : The light source, the light space and the
  //! radius of light space are displayed.
  //! We can choose the "SAMELAST" as parameter of representation
  //! In this case the graphic structure representation will be
  //! the last displayed.
  Standard_EXPORT void Display (const Handle(V3d_View)& aView, const V3d_TypeOfRepresentation Representation) Standard_OVERRIDE;
  
  //! Returns the position of the light source.
  Standard_EXPORT void Position (V3d_Coordinate& X, V3d_Coordinate& Y, V3d_Coordinate& Z) const Standard_OVERRIDE;
  
  //! Returns the attenuation factors A1,A2 of the light
  //! source used at construction time.
  Standard_EXPORT void Attenuation (Quantity_Coefficient& A1, Quantity_Coefficient& A2) const;




  DEFINE_STANDARD_RTTI(V3d_PositionalLight,V3d_PositionLight)

protected:




private:

  
  //! Defined the representation of the positional light source.
  Standard_EXPORT void Symbol (const Handle(Graphic3d_Group)& gsymbol, const Handle(V3d_View)& aView) const Standard_OVERRIDE;



};







#endif // _V3d_PositionalLight_HeaderFile
