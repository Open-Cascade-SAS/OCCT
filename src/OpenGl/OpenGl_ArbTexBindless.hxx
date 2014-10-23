// Created on: 2014-10-07
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

#ifndef _OpenGl_ArbTexBindless_H__
#define _OpenGl_ArbTexBindless_H__

#include <OpenGl_GlFunctions.hxx>

//! Provides bindless textures.
//! This extension allows OpenGL applications to access texture objects in
//! shaders without first binding each texture to one of a limited number of
//! texture image units.
struct OpenGl_ArbTexBindless : protected OpenGl_GlFunctions
{
#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glGetTextureHandleARB;
  using OpenGl_GlFunctions::glGetTextureSamplerHandleARB;
  using OpenGl_GlFunctions::glMakeTextureHandleResidentARB;
  using OpenGl_GlFunctions::glMakeTextureHandleNonResidentARB;
  using OpenGl_GlFunctions::glGetImageHandleARB;
  using OpenGl_GlFunctions::glMakeImageHandleResidentARB;
  using OpenGl_GlFunctions::glMakeImageHandleNonResidentARB;
  using OpenGl_GlFunctions::glUniformHandleui64ARB;
  using OpenGl_GlFunctions::glUniformHandleui64vARB;
  using OpenGl_GlFunctions::glProgramUniformHandleui64ARB;
  using OpenGl_GlFunctions::glProgramUniformHandleui64vARB;
  using OpenGl_GlFunctions::glIsTextureHandleResidentARB;
  using OpenGl_GlFunctions::glIsImageHandleResidentARB;
  using OpenGl_GlFunctions::glVertexAttribL1ui64ARB;
  using OpenGl_GlFunctions::glVertexAttribL1ui64vARB;
  using OpenGl_GlFunctions::glGetVertexAttribLui64vARB;
#endif
};

#endif // _OpenGl_ArbTexBindless_H__
