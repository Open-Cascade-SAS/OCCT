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

#ifndef Graphic3d_BufferType_HeaderFile
#define Graphic3d_BufferType_HeaderFile

//! Define buffers available for dump
enum Graphic3d_BufferType
{
  Graphic3d_BT_RGB,                 //!< color buffer without alpha component
  Graphic3d_BT_RGBA,                //!< color buffer
  Graphic3d_BT_Depth,               //!< depth buffer
  Graphic3d_BT_RGB_RayTraceHdrLeft, //!< left view HDR color buffer for Ray-Tracing
  Graphic3d_BT_Red,                 //!< color buffer, red channel
  Graphic3d_BT_ShadowMap,           //!< buffer with shadow map
};

#endif // _Graphic3d_BufferType_H__
