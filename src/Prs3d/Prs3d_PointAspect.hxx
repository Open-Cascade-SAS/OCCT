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

#ifndef _Prs3d_PointAspect_HeaderFile
#define _Prs3d_PointAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Prs3d_BasicAspect.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Standard_Real.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <Graphic3d_MarkerImage.hxx>
class Graphic3d_AspectMarker3d;
class Quantity_Color;


class Prs3d_PointAspect;
DEFINE_STANDARD_HANDLE(Prs3d_PointAspect, Prs3d_BasicAspect)

//! This  class  defines  attributes for the points
//! The points are drawn using markers, whose size does not depend on
//! the zoom value of the views.
class Prs3d_PointAspect : public Prs3d_BasicAspect
{

public:

  
  Standard_EXPORT Prs3d_PointAspect(const Aspect_TypeOfMarker aType, const Quantity_Color& aColor, const Standard_Real aScale);
  
  Standard_EXPORT Prs3d_PointAspect(const Aspect_TypeOfMarker aType, const Quantity_NameOfColor aColor, const Standard_Real aScale);
  
  //! defines only the urer defined marker point.
  Standard_EXPORT Prs3d_PointAspect(const Quantity_Color& AColor, const Standard_Integer AWidth, const Standard_Integer AHeight, const Handle(TColStd_HArray1OfByte)& ATexture);
  
  Standard_EXPORT Prs3d_PointAspect(const Handle(Graphic3d_AspectMarker3d)& theAspect);
  
  Standard_EXPORT void SetColor (const Quantity_Color& aColor);
  
  //! defines the color to be used when drawing a point.
  //! Default value: Quantity_NOC_YELLOW
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor);
  
  //! defines the type of representation to be used when drawing a point.
  //! Default value: Aspect_TOM_PLUS
  Standard_EXPORT void SetTypeOfMarker (const Aspect_TypeOfMarker aType);
  
  //! defines the size of the marker used when drawing a point.
  //! Default value: 1.
  Standard_EXPORT void SetScale (const Standard_Real aScale);
  
  Standard_EXPORT Handle(Graphic3d_AspectMarker3d) Aspect() const;
  
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectMarker3d)& theAspect);
  
  //! Returns marker's texture size.
  Standard_EXPORT void GetTextureSize (Standard_Integer& AWidth, Standard_Integer& AHeight);
  
  //! Returns marker's texture.
  Standard_EXPORT const Handle(Graphic3d_MarkerImage)& GetTexture();




  DEFINE_STANDARD_RTTI(Prs3d_PointAspect,Prs3d_BasicAspect)

protected:




private:


  Handle(Graphic3d_AspectMarker3d) myAspect;


};







#endif // _Prs3d_PointAspect_HeaderFile
