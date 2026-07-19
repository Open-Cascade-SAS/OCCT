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

#ifndef OpenGl_GlCore31_HeaderFile
#define OpenGl_GlCore31_HeaderFile

#include <OpenGl_GlCore30.hxx>

//! OpenGL 3.1 definition.
struct OpenGl_GlCore31 : public OpenGl_GlCore30
{
private:
  typedef OpenGl_GlCore30 theBaseClass_t;

public: //! @name GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)
  using theBaseClass_t::glGetActiveUniformBlockiv;
  using theBaseClass_t::glGetActiveUniformBlockName;
  using theBaseClass_t::glGetActiveUniformsiv;
  using theBaseClass_t::glGetUniformBlockIndex;
  using theBaseClass_t::glGetUniformIndices;
  using theBaseClass_t::glUniformBlockBinding;
#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glGetActiveUniformName; // undefined in OpenGL ES
#endif

public: //! @name GL_ARB_copy_buffer (added to OpenGL 3.1 core)
  using theBaseClass_t::glCopyBufferSubData;

public: //! @name OpenGL 3.1 additives to 3.0
  using theBaseClass_t::glDrawArraysInstanced;
  using theBaseClass_t::glDrawElementsInstanced;
#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glPrimitiveRestartIndex; // undefined in OpenGL ES
  using theBaseClass_t::glTexBuffer;             // added in OpenGL ES 3.2
#endif
};

#endif // _OpenGl_GlCore31_Header
