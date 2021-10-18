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

#ifndef OpenGl_GlCore30_HeaderFile
#define OpenGl_GlCore30_HeaderFile

#include <OpenGl_GlCore21.hxx>

//! OpenGL 3.0 core.
//! This is first version with deprecation model introduced
//! - a lot of functionality regarding to fixed pipeline were marked deprecated.
//! Notice that nothing were actually removed in this version (unless Forward context loaded)!
struct OpenGl_GlCore30 : public OpenGl_GlCore21
{
private:
  typedef OpenGl_GlCore21 theBaseClass_t;

public: //! @name GL_ARB_framebuffer_object (added to OpenGL 3.0 core)

  using theBaseClass_t::glIsRenderbuffer;
  using theBaseClass_t::glBindRenderbuffer;
  using theBaseClass_t::glDeleteRenderbuffers;
  using theBaseClass_t::glGenRenderbuffers;
  using theBaseClass_t::glRenderbufferStorage;
  using theBaseClass_t::glGetRenderbufferParameteriv;
  using theBaseClass_t::glIsFramebuffer;
  using theBaseClass_t::glBindFramebuffer;
  using theBaseClass_t::glDeleteFramebuffers;
  using theBaseClass_t::glGenFramebuffers;
  using theBaseClass_t::glCheckFramebufferStatus;
  using theBaseClass_t::glFramebufferTexture2D;
  using theBaseClass_t::glFramebufferRenderbuffer;
  using theBaseClass_t::glGetFramebufferAttachmentParameteriv;
  using theBaseClass_t::glGenerateMipmap;
  using theBaseClass_t::glBlitFramebuffer;
  using theBaseClass_t::glRenderbufferStorageMultisample;
  using theBaseClass_t::glFramebufferTextureLayer;
#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glFramebufferTexture1D;
  using theBaseClass_t::glFramebufferTexture3D;
#endif

public: //! @name GL_ARB_vertex_array_object (added to OpenGL 3.0 core)

  using theBaseClass_t::glBindVertexArray;
  using theBaseClass_t::glDeleteVertexArrays;
  using theBaseClass_t::glGenVertexArrays;
  using theBaseClass_t::glIsVertexArray;

public: //! @name GL_ARB_map_buffer_range (added to OpenGL 3.0 core)

#ifndef __EMSCRIPTEN__
  using theBaseClass_t::glMapBufferRange;
  using theBaseClass_t::glFlushMappedBufferRange;
#endif

public: //! @name OpenGL 3.0 additives to 2.1

  using theBaseClass_t::glGetBooleani_v;
  using theBaseClass_t::glGetIntegeri_v;
  using theBaseClass_t::glBeginTransformFeedback;
  using theBaseClass_t::glEndTransformFeedback;
  using theBaseClass_t::glBindBufferRange;
  using theBaseClass_t::glBindBufferBase;
  using theBaseClass_t::glTransformFeedbackVaryings;
  using theBaseClass_t::glGetTransformFeedbackVarying;
  using theBaseClass_t::glVertexAttribIPointer;
  using theBaseClass_t::glGetVertexAttribIiv;
  using theBaseClass_t::glGetVertexAttribIuiv;
  using theBaseClass_t::glVertexAttribI4i;
  using theBaseClass_t::glVertexAttribI4ui;
  using theBaseClass_t::glVertexAttribI4iv;
  using theBaseClass_t::glVertexAttribI4uiv;
  using theBaseClass_t::glGetUniformuiv;
  using theBaseClass_t::glGetFragDataLocation;
  using theBaseClass_t::glUniform1ui;
  using theBaseClass_t::glUniform2ui;
  using theBaseClass_t::glUniform3ui;
  using theBaseClass_t::glUniform4ui;
  using theBaseClass_t::glUniform1uiv;
  using theBaseClass_t::glUniform2uiv;
  using theBaseClass_t::glUniform3uiv;
  using theBaseClass_t::glUniform4uiv;
  using theBaseClass_t::glClearBufferiv;
  using theBaseClass_t::glClearBufferuiv;
  using theBaseClass_t::glClearBufferfv;
  using theBaseClass_t::glClearBufferfi;
  using theBaseClass_t::glGetStringi;

#if !defined(GL_ES_VERSION_2_0)
  // the following have been added only in OpenGL ES 3.2
  using theBaseClass_t::glColorMaski;
  using theBaseClass_t::glEnablei;
  using theBaseClass_t::glDisablei;
  using theBaseClass_t::glIsEnabledi;

  using theBaseClass_t::glTexParameterIiv;
  using theBaseClass_t::glTexParameterIuiv;
  using theBaseClass_t::glGetTexParameterIiv;
  using theBaseClass_t::glGetTexParameterIuiv;
#endif

#if !defined(GL_ES_VERSION_2_0)
  // the following are defined only on desktop OpenGL
  using theBaseClass_t::glClampColor;
  using theBaseClass_t::glBeginConditionalRender;
  using theBaseClass_t::glEndConditionalRender;
  using theBaseClass_t::glBindFragDataLocation;
  using theBaseClass_t::glVertexAttribI1i;
  using theBaseClass_t::glVertexAttribI2i;
  using theBaseClass_t::glVertexAttribI3i;
  using theBaseClass_t::glVertexAttribI1ui;
  using theBaseClass_t::glVertexAttribI2ui;
  using theBaseClass_t::glVertexAttribI3ui;
  using theBaseClass_t::glVertexAttribI1iv;
  using theBaseClass_t::glVertexAttribI2iv;
  using theBaseClass_t::glVertexAttribI3iv;
  using theBaseClass_t::glVertexAttribI1uiv;
  using theBaseClass_t::glVertexAttribI2uiv;
  using theBaseClass_t::glVertexAttribI3uiv;
  using theBaseClass_t::glVertexAttribI4bv;
  using theBaseClass_t::glVertexAttribI4sv;
  using theBaseClass_t::glVertexAttribI4ubv;
  using theBaseClass_t::glVertexAttribI4usv;
#endif

#if defined(GL_ES_VERSION_2_0)
  // the following functions from OpenGL 1.5 have been added only in OpenGL ES 3.0
  using theBaseClass_t::glGenQueries;
  using theBaseClass_t::glDeleteQueries;
  using theBaseClass_t::glIsQuery;
  using theBaseClass_t::glBeginQuery;
  using theBaseClass_t::glEndQuery;
  using theBaseClass_t::glGetQueryiv;
  using theBaseClass_t::glGetQueryObjectuiv;
#ifndef __EMSCRIPTEN__
  using theBaseClass_t::glUnmapBuffer;
#endif
#endif
};

#endif // _OpenGl_GlCore30_Header
