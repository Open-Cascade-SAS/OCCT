// Created on: 2014-05-14
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_RenderingParams_HeaderFile
#define _Graphic3d_RenderingParams_HeaderFile

#include <Graphic3d_RenderingMode.hxx>

//! Helper class to store rendering parameters.
class Graphic3d_RenderingParams
{
public:

  //! Default ray-tracing depth.
  static const Standard_Integer THE_DEFAULT_DEPTH = 3;

public:

  //! Creates default rendering parameters.
  Graphic3d_RenderingParams()
  : Method (Graphic3d_RM_RASTERIZATION),
    RaytracingDepth (THE_DEFAULT_DEPTH),
    IsShadowEnabled (Standard_True),
    IsReflectionEnabled (Standard_False),
    IsAntialiasingEnabled (Standard_False),
    IsTransparentShadowEnabled (Standard_False)
  {
    //
  }

public:

  //! Specifies rendering mode.
  Graphic3d_RenderingMode Method;

  //! Maximum ray-tracing depth.
  Standard_Integer RaytracingDepth;

  //! Enables/disables shadows rendering.
  Standard_Boolean IsShadowEnabled;

  //! Enables/disables specular reflections.
  Standard_Boolean IsReflectionEnabled;
  
  //! Enables/disables adaptive anti-aliasing.
  Standard_Boolean IsAntialiasingEnabled;

  //! Enables/disables light propagation through transparent media.
  Standard_Boolean IsTransparentShadowEnabled;

};

#endif // _Graphic3d_RenderingParams_HeaderFile
