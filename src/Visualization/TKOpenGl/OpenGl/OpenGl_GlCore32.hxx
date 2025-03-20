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

#ifndef OpenGl_GlCore32_HeaderFile
#define OpenGl_GlCore32_HeaderFile

#include <OpenGl_GlCore31.hxx>

//! OpenGL 3.2 definition.
struct OpenGl_GlCore32 : public OpenGl_GlCore31
{
private:
  typedef OpenGl_GlCore31 theBaseClass_t;

public: //! @name GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)
  using theBaseClass_t::glDrawElementsBaseVertex;
  using theBaseClass_t::glDrawElementsInstancedBaseVertex;
  using theBaseClass_t::glDrawRangeElementsBaseVertex;
  using theBaseClass_t::glMultiDrawElementsBaseVertex;

public: //! @name GL_ARB_provoking_vertex (added to OpenGL 3.2 core)
  using theBaseClass_t::glProvokingVertex;

public: //! @name GL_ARB_sync (added to OpenGL 3.2 core)
  using theBaseClass_t::glClientWaitSync;
  using theBaseClass_t::glDeleteSync;
  using theBaseClass_t::glFenceSync;
  using theBaseClass_t::glGetInteger64v;
  using theBaseClass_t::glGetSynciv;
  using theBaseClass_t::glIsSync;
  using theBaseClass_t::glWaitSync;

public: //! @name GL_ARB_texture_multisample (added to OpenGL 3.2 core)
  using theBaseClass_t::glGetMultisamplefv;
  using theBaseClass_t::glSampleMaski;
  using theBaseClass_t::glTexImage2DMultisample;
  using theBaseClass_t::glTexImage3DMultisample;

public: //! @name OpenGL 3.2 additives to 3.1
  using theBaseClass_t::glFramebufferTexture;
  using theBaseClass_t::glGetBufferParameteri64v;
  using theBaseClass_t::glGetInteger64i_v;
};

#endif // _OpenGl_GlCore32_Header
