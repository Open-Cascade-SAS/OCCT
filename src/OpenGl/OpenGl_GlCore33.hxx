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

#ifndef _OpenGl_GlCore33_Header
#define _OpenGl_GlCore33_Header

#include <OpenGl_GlCore32.hxx>

//! OpenGL 3.3 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore33 : public theBaseClass_t
{

#if !defined(GL_ES_VERSION_2_0)

public: //! @name GL_ARB_blend_func_extended (added to OpenGL 3.3 core)

  using theBaseClass_t::glBindFragDataLocationIndexed;
  using theBaseClass_t::glGetFragDataIndex;

public: //! @name GL_ARB_sampler_objects (added to OpenGL 3.3 core)

  using theBaseClass_t::glGenSamplers;
  using theBaseClass_t::glDeleteSamplers;
  using theBaseClass_t::glIsSampler;
  using theBaseClass_t::glBindSampler;
  using theBaseClass_t::glSamplerParameteri;
  using theBaseClass_t::glSamplerParameteriv;
  using theBaseClass_t::glSamplerParameterf;
  using theBaseClass_t::glSamplerParameterfv;
  using theBaseClass_t::glSamplerParameterIiv;
  using theBaseClass_t::glSamplerParameterIuiv;
  using theBaseClass_t::glGetSamplerParameteriv;
  using theBaseClass_t::glGetSamplerParameterIiv;
  using theBaseClass_t::glGetSamplerParameterfv;
  using theBaseClass_t::glGetSamplerParameterIuiv;

public: //! @name GL_ARB_timer_query (added to OpenGL 3.3 core)

  using theBaseClass_t::glQueryCounter;
  using theBaseClass_t::glGetQueryObjecti64v;
  using theBaseClass_t::glGetQueryObjectui64v;

public: //! @name GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)

  using theBaseClass_t::glVertexAttribP1ui;
  using theBaseClass_t::glVertexAttribP1uiv;
  using theBaseClass_t::glVertexAttribP2ui;
  using theBaseClass_t::glVertexAttribP2uiv;
  using theBaseClass_t::glVertexAttribP3ui;
  using theBaseClass_t::glVertexAttribP3uiv;
  using theBaseClass_t::glVertexAttribP4ui;
  using theBaseClass_t::glVertexAttribP4uiv;

public: //! @name OpenGL 3.3 additives to 3.2

  using theBaseClass_t::glVertexAttribDivisor;

#endif

};

//! OpenGL 3.3 compatibility profile.
typedef OpenGl_TmplCore33<OpenGl_GlCore32Back> OpenGl_GlCore33Back;

//! OpenGL 3.3 core profile.
typedef OpenGl_TmplCore33<OpenGl_GlCore32>     OpenGl_GlCore33;

#endif // _OpenGl_GlCore33_Header
