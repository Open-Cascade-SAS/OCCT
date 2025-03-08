// Created on: 1991-04-24
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Draw_MarkerShape_HeaderFile
#define _Draw_MarkerShape_HeaderFile

//! Circle is not sensible to zoom, like
//! other MarkerShape, contrarily to CircleZoom
enum Draw_MarkerShape
{
  Draw_Square,
  Draw_Losange,
  Draw_X,
  Draw_Plus,
  Draw_Circle,
  Draw_CircleZoom
};

#endif // _Draw_MarkerShape_HeaderFile
