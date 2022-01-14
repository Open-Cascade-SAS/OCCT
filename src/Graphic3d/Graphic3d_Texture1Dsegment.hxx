// Created on: 1997-07-28
// Created by: Pierre CHALAMET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Graphic3d_Texture1Dsegment_HeaderFile
#define _Graphic3d_Texture1Dsegment_HeaderFile

#include <Standard.hxx>

#include <Graphic3d_Texture1D.hxx>
#include <Graphic3d_NameOfTexture1D.hxx>
class TCollection_AsciiString;


class Graphic3d_Texture1Dsegment;
DEFINE_STANDARD_HANDLE(Graphic3d_Texture1Dsegment, Graphic3d_Texture1D)

//! This class provides the implementation
//! of a 1D texture applyable along a segment.
//! You might use the SetSegment() method
//! to set the way the texture is "stretched" on facets.
class Graphic3d_Texture1Dsegment : public Graphic3d_Texture1D
{

public:

  
  //! Creates a texture from a file
  Standard_EXPORT Graphic3d_Texture1Dsegment(const TCollection_AsciiString& theFileName);
  
  //! Creates a texture from a predefined texture name set.
  Standard_EXPORT Graphic3d_Texture1Dsegment(const Graphic3d_NameOfTexture1D theNOT);
  
  //! Creates a texture from the pixmap.
  Standard_EXPORT Graphic3d_Texture1Dsegment(const Handle(Image_PixMap)& thePixMap);
  
  //! Sets the texture application bounds. Defines the way
  //! the texture is stretched across facets.
  //! Default values are <0.0, 0.0, 0.0> , <0.0, 0.0, 1.0>
  Standard_EXPORT void SetSegment (const Standard_ShortReal theX1, const Standard_ShortReal theY1, const Standard_ShortReal theZ1, const Standard_ShortReal theX2, const Standard_ShortReal theY2, const Standard_ShortReal theZ2);
  
  //! Returns the values of the current segment X1, Y1, Z1 , X2, Y2, Z2.
  Standard_EXPORT void Segment (Standard_ShortReal& theX1, Standard_ShortReal& theY1, Standard_ShortReal& theZ1, Standard_ShortReal& theX2, Standard_ShortReal& theY2, Standard_ShortReal& theZ2) const;




  DEFINE_STANDARD_RTTIEXT(Graphic3d_Texture1Dsegment,Graphic3d_Texture1D)

protected:




private:


  Standard_ShortReal myX1;
  Standard_ShortReal myY1;
  Standard_ShortReal myZ1;
  Standard_ShortReal myX2;
  Standard_ShortReal myY2;
  Standard_ShortReal myZ2;


};







#endif // _Graphic3d_Texture1Dsegment_HeaderFile
