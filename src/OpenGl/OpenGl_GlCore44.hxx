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

#ifndef _OpenGl_GlCore44_Header
#define _OpenGl_GlCore44_Header

#include <OpenGl_GlCore43.hxx>

//! OpenGL 4.4 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore44 : public theBaseClass_t
{

public: //! @name OpenGL 4.4 additives to 4.3

#if !defined(GL_ES_VERSION_2_0)

  using theBaseClass_t::glBufferStorage;
  using theBaseClass_t::glClearTexImage;
  using theBaseClass_t::glClearTexSubImage;
  using theBaseClass_t::glBindBuffersBase;
  using theBaseClass_t::glBindBuffersRange;
  using theBaseClass_t::glBindTextures;
  using theBaseClass_t::glBindSamplers;
  using theBaseClass_t::glBindImageTextures;
  using theBaseClass_t::glBindVertexBuffers;

#endif

};

//! OpenGL 4.4 compatibility profile.
typedef OpenGl_TmplCore44<OpenGl_GlCore43Back> OpenGl_GlCore44Back;

//! OpenGL 4.4 core profile.
typedef OpenGl_TmplCore44<OpenGl_GlCore43>     OpenGl_GlCore44;

#endif // _OpenGl_GlCore44_Header
