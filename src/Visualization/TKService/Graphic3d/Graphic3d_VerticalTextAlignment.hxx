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

#ifndef _Graphic3d_VerticalTextAlignment_HeaderFile
#define _Graphic3d_VerticalTextAlignment_HeaderFile

//! Defines the vertical position of the text
//! relative to its anchor.
enum Graphic3d_VerticalTextAlignment
{
  Graphic3d_VTA_BOTTOM, //!< The anchor lies on the last line of the text.
  Graphic3d_VTA_CENTER, //!< The anchor lies on the center of the text (not bounding box).
  Graphic3d_VTA_TOP,    //!< The anchor lies on the line preceding to the first line of the text.
  Graphic3d_VTA_TOPFIRSTLINE //!< The anchor lies on the first line of the text.
};

#endif // _Graphic3d_VerticalTextAlignment_HeaderFile
