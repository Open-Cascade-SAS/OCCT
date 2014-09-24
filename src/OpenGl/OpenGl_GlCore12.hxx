// Created on: 2012-03-06
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_GlCore12_Header
#define _OpenGl_GlCore12_Header

#include <OpenGl_GlCore11.hxx>

//! OpenGL 1.2 core based on 1.1 version.
template<typename theBaseClass_t>
struct OpenGl_TmplCore12 : public theBaseClass_t
{

public: //! @name OpenGL 1.2 additives to 1.1

  using theBaseClass_t::glBlendColor;
  using theBaseClass_t::glBlendEquation;

#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glDrawRangeElements;
  using theBaseClass_t::glTexImage3D;
  using theBaseClass_t::glTexSubImage3D;
  using theBaseClass_t::glCopyTexSubImage3D;
#endif

};

//! OpenGL 1.2 core based on 1.1 version.
typedef OpenGl_TmplCore12<OpenGl_GlCore11>    OpenGl_GlCore12;

//! OpenGL 1.2 without deprecated entry points.
typedef OpenGl_TmplCore12<OpenGl_GlCore11Fwd> OpenGl_GlCore12Fwd;

#endif // _OpenGl_GlCore12_Header
