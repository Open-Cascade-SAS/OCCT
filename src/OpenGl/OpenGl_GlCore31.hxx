// Created on: 2014-03-17
// Created by: Kirill GAVRILOV
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

#ifndef _OpenGl_GlCore31_Header
#define _OpenGl_GlCore31_Header

#include <OpenGl_GlCore30.hxx>

//! OpenGL 3.1 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore31 : public theBaseClass_t
{

#if !defined(GL_ES_VERSION_2_0)

public: //! @name GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)

  using theBaseClass_t::glGetUniformIndices;
  using theBaseClass_t::glGetActiveUniformsiv;
  using theBaseClass_t::glGetActiveUniformName;
  using theBaseClass_t::glGetUniformBlockIndex;
  using theBaseClass_t::glGetActiveUniformBlockiv;
  using theBaseClass_t::glGetActiveUniformBlockName;
  using theBaseClass_t::glUniformBlockBinding;

public: //! @name GL_ARB_copy_buffer (added to OpenGL 3.1 core)

  using theBaseClass_t::glCopyBufferSubData;

public: //! @name OpenGL 3.1 additives to 3.0

  using theBaseClass_t::glDrawArraysInstanced;
  using theBaseClass_t::glDrawElementsInstanced;
  using theBaseClass_t::glTexBuffer;
  using theBaseClass_t::glPrimitiveRestartIndex;

#endif

};

//! OpenGL 3.1 compatibility profile.
typedef OpenGl_TmplCore31<OpenGl_GlCore30>    OpenGl_GlCore31Back;

//! OpenGL 3.1 core profile (without removed entry points marked as deprecated in 3.0).
//! Notice that GLSL versions 1.10 and 1.20 also removed in 3.1!
typedef OpenGl_TmplCore31<OpenGl_GlCore30Fwd> OpenGl_GlCore31;

#endif // _OpenGl_GlCore31_Header
