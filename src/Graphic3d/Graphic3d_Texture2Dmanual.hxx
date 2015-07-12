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

#ifndef _Graphic3d_Texture2Dmanual_HeaderFile
#define _Graphic3d_Texture2Dmanual_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_NameOfTexture2D.hxx>
#include <Image_PixMap_Handle.hxx>
class TCollection_AsciiString;


class Graphic3d_Texture2Dmanual;
DEFINE_STANDARD_HANDLE(Graphic3d_Texture2Dmanual, Graphic3d_Texture2D)

//! This class defined a manual texture 2D
//! facets MUST define texture coordinate
//! if you want to see somethings on.
class Graphic3d_Texture2Dmanual : public Graphic3d_Texture2D
{

public:

  
  //! Creates a texture from a file
  Standard_EXPORT Graphic3d_Texture2Dmanual(const TCollection_AsciiString& theFileName);
  
  //! Creates a texture from a predefined texture name set.
  Standard_EXPORT Graphic3d_Texture2Dmanual(const Graphic3d_NameOfTexture2D theNOT);
  
  //! Creates a texture from the pixmap.
  Standard_EXPORT Graphic3d_Texture2Dmanual(const Image_PixMap_Handle& thePixMap);




  DEFINE_STANDARD_RTTI(Graphic3d_Texture2Dmanual,Graphic3d_Texture2D)

protected:




private:




};







#endif // _Graphic3d_Texture2Dmanual_HeaderFile
