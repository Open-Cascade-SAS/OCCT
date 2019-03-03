// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Graphic3d_AspectMarker3d_HeaderFile
#define _Graphic3d_AspectMarker3d_HeaderFile

#include <Graphic3d_Aspects.hxx>

//! Creates and updates an attribute group for marker type primitives.
//! This group contains the type of marker, its color, and its scale factor.
class Graphic3d_AspectMarker3d : public Graphic3d_Aspects
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectMarker3d, Graphic3d_Aspects)
public:

  //! Creates a context table for marker primitives
  //! defined with the following default values:
  //!
  //! Marker type : TOM_X
  //! Color       : YELLOW
  //! Scale factor: 1.0
  Standard_EXPORT Graphic3d_AspectMarker3d();
  
  Standard_EXPORT Graphic3d_AspectMarker3d(const Aspect_TypeOfMarker theType, const Quantity_Color& theColor, const Standard_Real theScale);
  
  //! Creates a context table for marker primitives
  //! defined with the specified values.
  Standard_EXPORT Graphic3d_AspectMarker3d(const Quantity_Color& theColor, const Standard_Integer theWidth, const Standard_Integer theHeight, const Handle(TColStd_HArray1OfByte)& theTextureBitmap);
  
  //! Creates a context table for marker primitives
  //! defined with the specified values.
  Standard_EXPORT Graphic3d_AspectMarker3d(const Handle(Image_PixMap)& theTextureImage);

  //! Return scale factor.
  Standard_ShortReal Scale() const { return myMarkerScale; }

  //! Modifies the scale factor.
  //! Marker type Aspect_TOM_POINT is not affected by the marker size scale factor.
  //! It is always the smallest displayable dot.
  //! Warning: Raises Standard_OutOfRange if the scale is a negative value.
  void SetScale (const Standard_ShortReal theScale)
  {
    SetMarkerScale (theScale);
  }

  //! Assign scale factor.
  void SetScale (const Standard_Real theScale) { SetScale ((float )theScale); }

  //! Return marker type.
  Aspect_TypeOfMarker Type() const { return myMarkerType; }

  //! Modifies the type of marker.
  void SetType (const Aspect_TypeOfMarker theType) { myMarkerType = theType; }

  //! Returns marker's texture size.
  Standard_EXPORT void GetTextureSize (Standard_Integer& theWidth, Standard_Integer& theHeight) const;
  
  //! Returns marker's image texture.
  //! Could be null handle if marker aspect has been initialized as default type of marker.
  const Handle(Graphic3d_MarkerImage)& GetMarkerImage() const { return myMarkerImage; }
  
  Standard_EXPORT void SetBitMap (const Standard_Integer theWidth, const Standard_Integer theHeight, const Handle(TColStd_HArray1OfByte)& theTexture);

};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectMarker3d, Graphic3d_Aspects)

#endif // _Graphic3d_AspectMarker3d_HeaderFile
