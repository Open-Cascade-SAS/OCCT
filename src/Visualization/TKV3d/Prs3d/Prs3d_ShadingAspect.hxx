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

#include <Aspect_TypeOfFacingModel.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_BasicAspect.hxx>

//! A framework to define the display of shading.
//! The attributes which make up this definition include:
//! -   fill aspect
//! -   color, and
//! -   material
class Prs3d_ShadingAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_ShadingAspect, Prs3d_BasicAspect)
public:
  //! Constructs an empty framework to display shading.
  Standard_EXPORT Prs3d_ShadingAspect();

  //! Constructor with initialization.
  Prs3d_ShadingAspect(const Handle(Graphic3d_AspectFillArea3d)& theAspect)
      : myAspect(theAspect)
  {
  }

  //! Change the polygons interior color and material ambient color.
  Standard_EXPORT void SetColor(const Quantity_Color&          aColor,
                                const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);

  //! Change the polygons material aspect.
  Standard_EXPORT void SetMaterial(const Graphic3d_MaterialAspect& aMaterial,
                                   const Aspect_TypeOfFacingModel  aModel = Aspect_TOFM_BOTH_SIDE);

  //! Change the polygons transparency value.
  //! Warning : aValue must be in the range 0,1. 0 is the default (NO transparent)
  Standard_EXPORT void SetTransparency(
    const Standard_Real            aValue,
    const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_BOTH_SIDE);

  //! Returns the polygons color.
  Standard_EXPORT const Quantity_Color& Color(
    const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_FRONT_SIDE) const;

  //! Returns the polygons material aspect.
  Standard_EXPORT const Graphic3d_MaterialAspect& Material(
    const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_FRONT_SIDE) const;

  //! Returns the polygons transparency value.
  Standard_EXPORT Standard_Real
    Transparency(const Aspect_TypeOfFacingModel aModel = Aspect_TOFM_FRONT_SIDE) const;

  //! Returns the polygons aspect properties.
  const Handle(Graphic3d_AspectFillArea3d)& Aspect() const { return myAspect; }

  void SetAspect(const Handle(Graphic3d_AspectFillArea3d)& theAspect) { myAspect = theAspect; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  Handle(Graphic3d_AspectFillArea3d) myAspect;
};

DEFINE_STANDARD_HANDLE(Prs3d_ShadingAspect, Prs3d_BasicAspect)

#endif // _Prs3d_ShadingAspect_HeaderFile
