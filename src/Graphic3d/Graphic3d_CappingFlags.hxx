// Created on: 2016-08-04
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CappingFlags_HeaderFile
#define _Graphic3d_CappingFlags_HeaderFile

//! Enumeration of capping flags.
enum Graphic3d_CappingFlags
{
  Graphic3d_CappingFlags_None           = 0x0000, //!< no flags
  Graphic3d_CappingFlags_ObjectMaterial = 0x0001, //!< use object material
  Graphic3d_CappingFlags_ObjectTexture  = 0x0002, //!< use object texture
  Graphic3d_CappingFlags_ObjectShader   = 0x0008, //!< use object GLSL program
  Graphic3d_CappingFlags_ObjectAspect =
    Graphic3d_CappingFlags_ObjectMaterial | Graphic3d_CappingFlags_ObjectTexture | Graphic3d_CappingFlags_ObjectShader //!< use entire fill area aspect from object
};

#endif // _Graphic3d_CappingFlags_HeaderFile
