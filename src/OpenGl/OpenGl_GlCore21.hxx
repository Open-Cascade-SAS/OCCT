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

#ifndef _OpenGl_GlCore21_Header
#define _OpenGl_GlCore21_Header

#include <OpenGl_GlCore20.hxx>

//! OpenGL 2.1 core based on 2.0 version.
template<typename theBaseClass_t>
struct OpenGl_TmplCore21 : public theBaseClass_t
{

public: //! @name OpenGL 2.1 additives to 2.0

#if !defined(GL_ES_VERSION_2_0)

  using theBaseClass_t::glUniformMatrix2x3fv;
  using theBaseClass_t::glUniformMatrix3x2fv;
  using theBaseClass_t::glUniformMatrix2x4fv;
  using theBaseClass_t::glUniformMatrix4x2fv;
  using theBaseClass_t::glUniformMatrix3x4fv;
  using theBaseClass_t::glUniformMatrix4x3fv;

#endif

};

//! OpenGL 2.1 core based on 2.0 version.
typedef OpenGl_TmplCore21<OpenGl_GlCore20>    OpenGl_GlCore21;

//! OpenGL 2.1 without deprecated entry points.
typedef OpenGl_TmplCore21<OpenGl_GlCore20Fwd> OpenGl_GlCore21Fwd;

#endif // _OpenGl_GlCore21_Header
