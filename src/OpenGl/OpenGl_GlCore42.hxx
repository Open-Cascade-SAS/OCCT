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

#ifndef _OpenGl_GlCore42_Header
#define _OpenGl_GlCore42_Header

#include <OpenGl_GlCore41.hxx>

//! OpenGL 4.2 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore42 : public theBaseClass_t
{

#if !defined(GL_ES_VERSION_2_0)

public: //! @name GL_ARB_base_instance (added to OpenGL 4.2 core)

  using theBaseClass_t::glDrawArraysInstancedBaseInstance;
  using theBaseClass_t::glDrawElementsInstancedBaseInstance;
  using theBaseClass_t::glDrawElementsInstancedBaseVertexBaseInstance;

public: //! @name GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)

  using theBaseClass_t::glDrawTransformFeedbackInstanced;
  using theBaseClass_t::glDrawTransformFeedbackStreamInstanced;

public: //! @name GL_ARB_internalformat_query (added to OpenGL 4.2 core)

  using theBaseClass_t::glGetInternalformativ;

public: //! @name GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)

  using theBaseClass_t::glGetActiveAtomicCounterBufferiv;

public: //! @name GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)

  using theBaseClass_t::glBindImageTexture;
  using theBaseClass_t::glMemoryBarrier;

public: //! @name GL_ARB_texture_storage (added to OpenGL 4.2 core)

  using theBaseClass_t::glTexStorage1D;
  using theBaseClass_t::glTexStorage2D;
  using theBaseClass_t::glTexStorage3D;

#endif

};

//! OpenGL 4.2 compatibility profile.
typedef OpenGl_TmplCore42<OpenGl_GlCore41Back> OpenGl_GlCore42Back;

//! OpenGL 4.2 core profile.
typedef OpenGl_TmplCore42<OpenGl_GlCore41>     OpenGl_GlCore42;


#endif // _OpenGl_GlCore42_Header
