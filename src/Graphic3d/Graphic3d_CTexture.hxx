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

#ifndef _Graphic3d_CTexture_HeaderFile
#define _Graphic3d_CTexture_HeaderFile

#include <Graphic3d_TextureMap.hxx>

class Graphic3d_CTexture
{

public:

  Graphic3d_CTexture()
  : doTextureMap (0)
  {
    //
  }

public:

  Handle(Graphic3d_TextureMap) TextureMap;   //!< handle to texture
  int                          doTextureMap; //!< flag indicates to use texture or not

};

#endif // Graphic3d_CTexture_HeaderFile
