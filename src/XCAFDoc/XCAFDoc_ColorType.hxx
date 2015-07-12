// Created on: 2000-08-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_ColorType_HeaderFile
#define _XCAFDoc_ColorType_HeaderFile

//! Defines types of color assignments
//! Color of shape is defined following way
//! in dependance with type of color.
//! If type of color is XCAFDoc_ColorGen - then this color
//! defines default color for surfaces and curves.
//! If for shape color with types XCAFDoc_ColorSurf or XCAFDoc_ColorCurv is specified
//! then such color overrides generic color.
//! simple color
//! color of surfaces
//! color of curves
enum XCAFDoc_ColorType
{
XCAFDoc_ColorGen,
XCAFDoc_ColorSurf,
XCAFDoc_ColorCurv
};

#endif // _XCAFDoc_ColorType_HeaderFile
