// Created on: 1993-04-26
// Created by: Jean-Louis Frenkel
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Prs3d_ShadingAspect_HeaderFile
#define _Prs3d_ShadingAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Prs3d_BasicAspect.hxx>
#include <Aspect_TypeOfFacingModel.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Standard_Real.hxx>
class Graphic3d_AspectFillArea3d;
class Quantity_Color;
class Graphic3d_MaterialAspect;


class Prs3d_ShadingAspect;
DEFINE_STANDARD_HANDLE(Prs3d_ShadingAspect, Prs3d_BasicAspect)

//! A framework to define the display of shading.
//! The attributes which make up this definition include:
//! -   fill aspect
//! -   color, and
//! -   material
class Prs3d_ShadingAspect : public Prs3d_BasicAspect
{

public:

  
  //! Constructs an empty framework to display shading.
  Standard_EXPORT Prs3d_ShadingAspect();
  
  Standard_EXPORT Prs3d_ShadingAspect(const Handle(Graphic3d_AspectFillArea3d)& theAspect);
  
  //! Change the polygons interior color and material ambient color.
  Standard_EXPORT void SetColor (const Quantity_Color& aColor, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  //! Change the polygons interior color and material ambient color.
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  //! Change the polygons material aspect.
  Standard_EXPORT void SetMaterial (const Graphic3d_MaterialAspect& aMaterial, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  Standard_EXPORT void SetMaterial (const Graphic3d_NameOfMaterial aMaterial, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  //! Change the polygons transparency value.
  //! Warning : aValue must be in the range 0,1. 0 is the default (NO transparent)
  Standard_EXPORT void SetTransparency (const Standard_Real aValue, const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);
  
  //! Returns the polygons color.
  Standard_EXPORT const Quantity_Color& Color (const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_FRONT_SIDE) const;
  
  //! Returns the polygons material aspect.
  Standard_EXPORT const Graphic3d_MaterialAspect& Material (const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_FRONT_SIDE) const;
  
  //! Returns the polygons transparency value.
  Standard_EXPORT Standard_Real Transparency (const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_FRONT_SIDE) const;
  
  //! Returns the polygons aspect properties.
  Standard_EXPORT Handle(Graphic3d_AspectFillArea3d) Aspect() const;
  
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspect);




  DEFINE_STANDARD_RTTI(Prs3d_ShadingAspect,Prs3d_BasicAspect)

protected:




private:


  Handle(Graphic3d_AspectFillArea3d) myAspect;


};







#endif // _Prs3d_ShadingAspect_HeaderFile
