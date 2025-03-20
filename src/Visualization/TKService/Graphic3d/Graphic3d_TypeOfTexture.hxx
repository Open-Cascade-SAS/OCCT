// Created on: 1993-03-31
// Created by: NW,JPB,CAL
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

#ifndef _Graphic3d_TypeOfTexture_HeaderFile
#define _Graphic3d_TypeOfTexture_HeaderFile

//! Type of the texture file format.
enum Graphic3d_TypeOfTexture
{
  //! 1D texture (array).
  //! Note that this texture type might be unsupported by graphics API (emulated by 2D texture with
  //! 1 pixel height).
  Graphic3d_TypeOfTexture_1D,

  //! 2D texture (image plane).
  Graphic3d_TypeOfTexture_2D,

  //! 3D texture (a set of image planes).
  Graphic3d_TypeOfTexture_3D,

  //! Cubemap texture (6 image planes defining cube sides).
  Graphic3d_TypeOfTexture_CUBEMAP,

  //! Obsolete type - Graphic3d_TextureRoot::SetMipmapsGeneration() should be used instead.
  Graphic3d_TOT_2D_MIPMAP,

  // old aliases
  Graphic3d_TOT_1D      = Graphic3d_TypeOfTexture_1D,
  Graphic3d_TOT_2D      = Graphic3d_TypeOfTexture_2D,
  Graphic3d_TOT_CUBEMAP = Graphic3d_TypeOfTexture_CUBEMAP
};

#endif // _Graphic3d_TypeOfTexture_HeaderFile
