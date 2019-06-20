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

//! Definition of the color shading model.
enum Graphic3d_TypeOfShadingModel
{
  //! Use Shading Model, specified as default for entire Viewer.
  Graphic3d_TOSM_DEFAULT = -1,

  //! Unlit Shading (or shadeless), lighting is ignored and facet is fully filled by its material color.
  //! This model is useful for artificial/auxiliary objects, not intended to be lit,
  //! or for objects with pre-calculated lighting information (e.g. captured by camera).
  Graphic3d_TOSM_UNLIT = 0,

  //! Flat Shading, calculated per-facet basing on facet normal.
  //! This model is useful for mesh element analysis.
  //! Note that unlike Graphic3d_TOSM_VERTEX/Graphic3d_TOSM_FRAGMENT,
  //! this shading model does NOT require normals to be defined within vertex attributes.
  Graphic3d_TOSM_FACET,

  //! Gouraud Shading, calculated per-vertex basing on nodal normal, and then color is interpolated across fragments.
  //! This shading model was default within Fixed-Function Pipeline of old OpenGL (and Direct3d);
  //! still can be used as alternative to Graphic3d_TOSM_FRAGMENT for better performance on low-poly (and middle-poly) models
  //! (per-fragment shading is more optimal for considerably high-poly models, since expensive shading is not computed for discarded fragments).
  //! Shading model requires normals to be defined within vertex attributes.
  Graphic3d_TOSM_VERTEX,

  //! Phong Shading, calculated per-fragment basing on nodal normal, so that normal is interpolated across fragments.
  //! This is a main shading model nowadays, providing the best image quality.
  //! Shading model requires normals to be defined within vertex attributes.
  Graphic3d_TOSM_FRAGMENT,

  //! Metallic-roughness physically based (PBR) illumination system.
  Graphic3d_TOSM_PBR,

  //! Same as Graphic3d_TOSM_PBR but using flat per-triangle normal.
  Graphic3d_TOSM_PBR_FACET,

  // obsolete aliases
  Graphic3d_TOSM_NONE = Graphic3d_TOSM_UNLIT,
  V3d_COLOR   = Graphic3d_TOSM_NONE,
  V3d_FLAT    = Graphic3d_TOSM_FACET,
  V3d_GOURAUD = Graphic3d_TOSM_VERTEX,
  V3d_PHONG   = Graphic3d_TOSM_FRAGMENT
};

enum
{
  //! Auxiliary value defining the overall number of values in enumeration Graphic3d_TypeOfShadingModel
  Graphic3d_TypeOfShadingModel_NB = Graphic3d_TOSM_PBR_FACET + 1
};

#endif // _Graphic3d_TypeOfShadingModel_HeaderFile
