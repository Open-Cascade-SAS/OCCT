// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TextureSetBits_HeaderFile
#define _Graphic3d_TextureSetBits_HeaderFile

#include <Graphic3d_TextureUnit.hxx>

//! Standard texture units combination bits.
enum Graphic3d_TextureSetBits
{
  Graphic3d_TextureSetBits_NONE              = 0,
  Graphic3d_TextureSetBits_BaseColor         = (unsigned int )(1 << int(Graphic3d_TextureUnit_BaseColor)),
  Graphic3d_TextureSetBits_Emissive          = (unsigned int )(1 << int(Graphic3d_TextureUnit_Emissive)),
  Graphic3d_TextureSetBits_Occlusion         = (unsigned int )(1 << int(Graphic3d_TextureUnit_Occlusion)),
  Graphic3d_TextureSetBits_Normal            = (unsigned int )(1 << int(Graphic3d_TextureUnit_Normal)),
  Graphic3d_TextureSetBits_MetallicRoughness = (unsigned int )(1 << int(Graphic3d_TextureUnit_MetallicRoughness)),
};

#endif // _Graphic3d_TextureSetBits_HeaderFile
