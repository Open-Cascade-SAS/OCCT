// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TextureUnit_HeaderFile
#define _Graphic3d_TextureUnit_HeaderFile

//! Texture unit.
enum Graphic3d_TextureUnit
{
  // value as index number
  Graphic3d_TextureUnit_0,
  Graphic3d_TextureUnit_1,
  Graphic3d_TextureUnit_2,
  Graphic3d_TextureUnit_3,
  Graphic3d_TextureUnit_4,
  Graphic3d_TextureUnit_5,
  Graphic3d_TextureUnit_6,
  Graphic3d_TextureUnit_7,
  Graphic3d_TextureUnit_8,
  Graphic3d_TextureUnit_9,
  Graphic3d_TextureUnit_10,
  Graphic3d_TextureUnit_11,
  Graphic3d_TextureUnit_12,
  Graphic3d_TextureUnit_13,
  Graphic3d_TextureUnit_14,
  Graphic3d_TextureUnit_15,

  Graphic3d_TextureUnit_BaseColor         = Graphic3d_TextureUnit_0, //!< base color of the material
  //Graphic3d_TextureUnit_Normal            = Graphic3d_TextureUnit_1, //!< tangent space normal map
  //Graphic3d_TextureUnit_MetallicRoughness = Graphic3d_TextureUnit_2, //!< metalness+roughness of the material
  //Graphic3d_TextureUnit_Emissive          = Graphic3d_TextureUnit_3, //!< emissive map controls the color and intensity of the light being emitted by the material
  //Graphic3d_TextureUnit_Occlusion         = Graphic3d_TextureUnit_4, //!< occlusion map indicating areas of indirect lighting
};
enum
{
  Graphic3d_TextureUnit_NB = Graphic3d_TextureUnit_15 + 1
};

#endif // _Graphic3d_TextureUnit_HeaderFile
