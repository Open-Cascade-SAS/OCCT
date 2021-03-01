// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _Graphic3d_AlphaMode_HeaderFile
#define _Graphic3d_AlphaMode_HeaderFile

//! Defines how alpha value of base color / texture should be treated.
enum Graphic3d_AlphaMode
{
  Graphic3d_AlphaMode_Opaque = 0,     //!< rendered output is fully opaque and alpha value is ignored
  Graphic3d_AlphaMode_Mask,           //!< rendered output is either fully opaque or fully transparent depending on the alpha value and the alpha cutoff value
  Graphic3d_AlphaMode_Blend,          //!< rendered output is combined with the background
  Graphic3d_AlphaMode_MaskBlend,      //!< performs in-place blending (without implicit reordering of opaque objects) with alpha-test

  Graphic3d_AlphaMode_BlendAuto = -1, //!< special value defined for backward compatibility - it is equal to Graphic3d_AlphaMode_Blend when Material transparency is not zero and Graphic3d_AlphaMode_Opaque otherwise;
                                      //   since this check ignores possible transparency defined by per-vertex colors and textures - NOT recommended to use!
};

#endif // _Graphic3d_AlphaModeHeaderFile
