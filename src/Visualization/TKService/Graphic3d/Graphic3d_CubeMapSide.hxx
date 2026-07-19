// Author: Ilya Khramov
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

#ifndef _Graphic3d_CubeMapSide_HeaderFile
#define _Graphic3d_CubeMapSide_HeaderFile

//! Sides of cubemap in order of OpenGL rules
enum Graphic3d_CubeMapSide
{
  Graphic3d_CMS_POS_X, //!< X axis positive direction side
  Graphic3d_CMS_NEG_X, //!< X axis negative direction side
  Graphic3d_CMS_POS_Y, //!< Y axis positive direction side
  Graphic3d_CMS_NEG_Y, //!< Y axis negative direction side
  Graphic3d_CMS_POS_Z, //!< Z axis positive direction side
  Graphic3d_CMS_NEG_Z, //!< Z axis negative direction side
};

#endif // _Graphic3d_CubeMapSide_HeaderFile
