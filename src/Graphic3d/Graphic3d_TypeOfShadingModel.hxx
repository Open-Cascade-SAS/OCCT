// Created on: 1991-10-07
// Created by: NW,JPB,CAL
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

#ifndef _Graphic3d_TypeOfShadingModel_HeaderFile
#define _Graphic3d_TypeOfShadingModel_HeaderFile

//! Definition of the rendering (colour shading) model
//! Graphic3d_TOSM_NONE     No lighting, only white ambient light
//! Graphic3d_TOSM_FACET    No interpolation, constant shading      (Flat    Shading)
//! Graphic3d_TOSM_VERTEX   Interpolation of color based on normals (Gouraud Shading)
//! Graphic3d_TOSM_FRAGMENT Interpolation of color based on normals (Phong   Shading)
enum Graphic3d_TypeOfShadingModel
{
  Graphic3d_TOSM_NONE,
  Graphic3d_TOSM_FACET,
  Graphic3d_TOSM_VERTEX,
  Graphic3d_TOSM_FRAGMENT
};

#endif // _Graphic3d_TypeOfShadingModel_HeaderFile
