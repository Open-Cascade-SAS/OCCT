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
struct OpenGl_GlCore46 : public OpenGl_GlCore45
{
private:
  typedef OpenGl_GlCore45 theBaseClass_t;

public: //! @name OpenGL 4.6 additives to 4.5
  using theBaseClass_t::glMultiDrawArraysIndirectCount;
  using theBaseClass_t::glMultiDrawElementsIndirectCount;
  using theBaseClass_t::glPolygonOffsetClamp;
  using theBaseClass_t::glSpecializeShader;
};

#endif // _OpenGl_GlCore46_Header
