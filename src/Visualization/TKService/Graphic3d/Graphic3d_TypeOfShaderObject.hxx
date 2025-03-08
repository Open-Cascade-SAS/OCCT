// Created on: 2013-09-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_TypeOfShaderObject_HeaderFile
#define _Graphic3d_TypeOfShaderObject_HeaderFile

//! Type of the shader object.
enum Graphic3d_TypeOfShaderObject
{
  // rendering shaders
  Graphic3d_TOS_VERTEX          = 0x01, //!< vertex shader object, mandatory
  Graphic3d_TOS_TESS_CONTROL    = 0x02, //!< tessellation control shader object, optional
  Graphic3d_TOS_TESS_EVALUATION = 0x04, //!< tessellation evaluation shader object, optional
  Graphic3d_TOS_GEOMETRY        = 0x08, //!< geometry shader object, optional
  Graphic3d_TOS_FRAGMENT        = 0x10, //!< fragment shader object, mandatory
  // general-purpose compute shader
  Graphic3d_TOS_COMPUTE = 0x20 //!< compute shader object, should be used as alternative to shader
                               //!< object types for rendering
};

#endif
