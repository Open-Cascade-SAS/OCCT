// Created on: 2013-07-25
// Created by: Anton POLETAEV
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

#ifndef OpenGl_RenderFilter_HeaderFile
#define OpenGl_RenderFilter_HeaderFile

//! Filter for rendering elements.
enum OpenGl_RenderFilter
{
  OpenGl_RenderFilter_Empty               = 0x000, //!< disabled filter

  OpenGl_RenderFilter_OpaqueOnly          = 0x001, //!< render only opaque elements and any non-filling elements   (conflicts with OpenGl_RenderFilter_TransparentOnly)
  OpenGl_RenderFilter_TransparentOnly     = 0x002, //!< render only semitransparent elements and OpenGl_AspectFace (conflicts with OpenGl_RenderFilter_OpaqueOnly)

  OpenGl_RenderFilter_NonRaytraceableOnly = 0x004, //!< render only non-raytraceable elements
  OpenGl_RenderFilter_FillModeOnly        = 0x008, //!< render only filled elements

  OpenGl_RenderFilter_SkipTrsfPersistence = 0x010, //!< render only normal 3D objects without transformation persistence
};

#endif
