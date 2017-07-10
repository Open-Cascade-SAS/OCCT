// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _OpenGl_ArbSamplerObject_Header
#define _OpenGl_ArbSamplerObject_Header

#include <OpenGl_GlFunctions.hxx>

//! Provide Sampler Object functionality (texture parameters stored independently from texture itself).
//! Available since OpenGL 3.3+ (GL_ARB_sampler_objects extension) and OpenGL ES 3.0+.
struct OpenGl_ArbSamplerObject : protected OpenGl_GlFunctions
{
  using OpenGl_GlFunctions::glGenSamplers;
  using OpenGl_GlFunctions::glDeleteSamplers;
  using OpenGl_GlFunctions::glIsSampler;
  using OpenGl_GlFunctions::glBindSampler;
  using OpenGl_GlFunctions::glSamplerParameteri;
  using OpenGl_GlFunctions::glSamplerParameteriv;
  using OpenGl_GlFunctions::glSamplerParameterf;
  using OpenGl_GlFunctions::glSamplerParameterfv;
  using OpenGl_GlFunctions::glGetSamplerParameteriv;
  using OpenGl_GlFunctions::glGetSamplerParameterfv;

#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glSamplerParameterIiv;
  using OpenGl_GlFunctions::glSamplerParameterIuiv;
  using OpenGl_GlFunctions::glGetSamplerParameterIiv;
  using OpenGl_GlFunctions::glGetSamplerParameterIuiv;
#endif

};

#endif // _OpenGl_ArbSamplerObject_Header
