// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _OpenGl_GlCore46_Header
#define _OpenGl_GlCore46_Header

#include <OpenGl_GlCore45.hxx>

//! OpenGL 4.6 definition.
template<typename theBaseClass_t>
struct OpenGl_TmplCore46 : public theBaseClass_t
{

public: //! @name OpenGL 4.6 additives to 4.5

#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glSpecializeShader;
  using theBaseClass_t::glMultiDrawArraysIndirectCount;
  using theBaseClass_t::glMultiDrawElementsIndirectCount;
  using theBaseClass_t::glPolygonOffsetClamp;
#endif

};

//! OpenGL 4.6 compatibility profile.
typedef OpenGl_TmplCore46<OpenGl_GlCore45Back> OpenGl_GlCore46Back;

//! OpenGL 4.6 core profile.
typedef OpenGl_TmplCore46<OpenGl_GlCore45>     OpenGl_GlCore46;

#endif // _OpenGl_GlCore46_Header
