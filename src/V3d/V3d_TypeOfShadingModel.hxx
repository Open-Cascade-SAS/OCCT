// Created on: 1992-11-13
// Created by: GG
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _V3d_TypeOfShadingModel_HeaderFile
#define _V3d_TypeOfShadingModel_HeaderFile


//! Defines the type of shading for the graphic object:
//! -   V3d_COLOR: simple surface color (Graphic3d_TOM_NONE),
//! -   V3d_FLAT: flat shading (Graphic3d_TOM_FACET),
//! -   V3d_GOURAUD: Gouraud shading (Graphic3d_TOM_VERTEX),
//! -   V3d_PHONG: Phong shading (Graphic3d_TOM_FRAGMENT).
enum V3d_TypeOfShadingModel
{
V3d_COLOR,
V3d_FLAT,
V3d_GOURAUD,
V3d_PHONG
};

#endif // _V3d_TypeOfShadingModel_HeaderFile
