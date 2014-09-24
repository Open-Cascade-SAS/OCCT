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

#ifndef _OpenGl_GlCore32_Header
#define _OpenGl_GlCore32_Header

#include <OpenGl_GlCore31.hxx>

//! OpenGL 3.2 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore32 : public theBaseClass_t
{

#if !defined(GL_ES_VERSION_2_0)

public: //! @name GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)

  using theBaseClass_t::glDrawElementsBaseVertex;
  using theBaseClass_t::glDrawRangeElementsBaseVertex;
  using theBaseClass_t::glDrawElementsInstancedBaseVertex;
  using theBaseClass_t::glMultiDrawElementsBaseVertex;

public: //! @name GL_ARB_provoking_vertex (added to OpenGL 3.2 core)

  using theBaseClass_t::glProvokingVertex;

public: //! @name GL_ARB_sync (added to OpenGL 3.2 core)

  using theBaseClass_t::glFenceSync;
  using theBaseClass_t::glIsSync;
  using theBaseClass_t::glDeleteSync;
  using theBaseClass_t::glClientWaitSync;
  using theBaseClass_t::glWaitSync;
  using theBaseClass_t::glGetInteger64v;
  using theBaseClass_t::glGetSynciv;

public: //! @name GL_ARB_texture_multisample (added to OpenGL 3.2 core)

  using theBaseClass_t::glTexImage2DMultisample;
  using theBaseClass_t::glTexImage3DMultisample;
  using theBaseClass_t::glGetMultisamplefv;
  using theBaseClass_t::glSampleMaski;

public: //! @name OpenGL 3.2 additives to 3.1

  using theBaseClass_t::glGetInteger64i_v;
  using theBaseClass_t::glGetBufferParameteri64v;
  using theBaseClass_t::glFramebufferTexture;

#endif

};

//! OpenGL 3.2 compatibility profile.
typedef OpenGl_TmplCore32<OpenGl_GlCore31Back> OpenGl_GlCore32Back;

//! OpenGL 3.2 core profile.
typedef OpenGl_TmplCore32<OpenGl_GlCore31>     OpenGl_GlCore32;

#endif // _OpenGl_GlCore32_Header
