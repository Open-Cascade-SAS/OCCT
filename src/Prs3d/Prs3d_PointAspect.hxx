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

#include <Prs3d_BasicAspect.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_MarkerImage.hxx>

//! This  class  defines  attributes for the points
//! The points are drawn using markers, whose size does not depend on
//! the zoom value of the views.
class Prs3d_PointAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_PointAspect, Prs3d_BasicAspect)
public:

  Standard_EXPORT Prs3d_PointAspect(const Aspect_TypeOfMarker theType, const Quantity_Color& theColor, const Standard_Real theScale);
  
  //! Defines the user defined marker point.
  Standard_EXPORT Prs3d_PointAspect (const Quantity_Color& theColor,
                                     const Standard_Integer theWidth,
                                     const Standard_Integer theHeight,
                                     const Handle(TColStd_HArray1OfByte)& theTexture);
  
  Prs3d_PointAspect (const Handle(Graphic3d_AspectMarker3d)& theAspect) : myAspect (theAspect) {}

  //! defines the color to be used when drawing a point.
  //! Default value: Quantity_NOC_YELLOW
  void SetColor (const Quantity_Color& theColor) { myAspect->SetColor (theColor); }

  //! defines the type of representation to be used when drawing a point.
  //! Default value: Aspect_TOM_PLUS
  void SetTypeOfMarker (const Aspect_TypeOfMarker theType) { myAspect->SetType (theType); }
  
  //! defines the size of the marker used when drawing a point.
  //! Default value: 1.
  void SetScale (const Standard_Real theScale) { myAspect->SetScale (theScale); }
  
  const Handle(Graphic3d_AspectMarker3d)& Aspect() const { return myAspect; }

  void SetAspect (const Handle(Graphic3d_AspectMarker3d)& theAspect) { myAspect = theAspect; }

  //! Returns marker's texture size.
  void GetTextureSize (Standard_Integer& theWidth, Standard_Integer& theHeight) const { myAspect->GetTextureSize (theWidth, theHeight); }
  
  //! Returns marker's texture.
  const Handle(Graphic3d_MarkerImage)& GetTexture() const { return myAspect->GetMarkerImage(); }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  Handle(Graphic3d_AspectMarker3d) myAspect;

};

DEFINE_STANDARD_HANDLE(Prs3d_PointAspect, Prs3d_BasicAspect)

#endif // _Prs3d_PointAspect_HeaderFile
