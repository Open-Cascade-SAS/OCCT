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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_MarkerImage.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Aspect_AspectMarker.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <Image_PixMap_Handle.hxx>
class Quantity_Color;


class Graphic3d_AspectMarker3d;
DEFINE_STANDARD_HANDLE(Graphic3d_AspectMarker3d, Aspect_AspectMarker)

//! Creates and updates an attribute group for
//! marker type primitives. This group contains the type
//! of marker, its colour, and its scale factor.
class Graphic3d_AspectMarker3d : public Aspect_AspectMarker
{

public:

  
  //! Creates a context table for marker primitives
  //! defined with the following default values:
  //!
  //! Marker type : TOM_X
  //! Colour      : YELLOW
  //! Scale factor: 1.0
  Standard_EXPORT Graphic3d_AspectMarker3d();
  
  Standard_EXPORT Graphic3d_AspectMarker3d(const Aspect_TypeOfMarker theType, const Quantity_Color& theColor, const Standard_Real theScale);
  
  //! Creates a context table for marker primitives
  //! defined with the specified values.
  Standard_EXPORT Graphic3d_AspectMarker3d(const Quantity_Color& theColor, const Standard_Integer theWidth, const Standard_Integer theHeight, const Handle(TColStd_HArray1OfByte)& theTextureBitmap);
  
  //! Creates a context table for marker primitives
  //! defined with the specified values.
  Standard_EXPORT Graphic3d_AspectMarker3d(const Image_PixMap_Handle& theTextureImage);
  
  //! Returns marker's texture size.
  Standard_EXPORT void GetTextureSize (Standard_Integer& theWidth, Standard_Integer& theHeight) const;
  
  //! Returns marker's image texture.
  //! Could be null handle if marker aspect has been initialized as
  //! default type of marker.
  Standard_EXPORT const Handle(Graphic3d_MarkerImage)& GetMarkerImage() const;
  
  //! Set marker's image texture.
  Standard_EXPORT void SetMarkerImage (const Handle(Graphic3d_MarkerImage)& theImage);
  
  Standard_EXPORT void SetBitMap (const Standard_Integer theWidth, const Standard_Integer theHeight, const Handle(TColStd_HArray1OfByte)& theTexture);
  
  //! Sets up OpenGL/GLSL shader program.
  Standard_EXPORT void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram);
  
  Standard_EXPORT const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const;




  DEFINE_STANDARD_RTTI(Graphic3d_AspectMarker3d,Aspect_AspectMarker)

protected:


  Handle(Graphic3d_MarkerImage) myMarkerImage;


private:


  Handle(Graphic3d_ShaderProgram) MyShaderProgram;


};







#endif // _Graphic3d_AspectMarker3d_HeaderFile
