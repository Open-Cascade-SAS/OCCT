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

#ifndef OpenGl_GlCore12_HeaderFile
#define OpenGl_GlCore12_HeaderFile

#include <OpenGl_GlCore11.hxx>

//! OpenGL 1.2 core based on 1.1 version.
struct OpenGl_GlCore12 : public OpenGl_GlCore11Fwd
{
private:
  typedef OpenGl_GlCore11Fwd theBaseClass_t;

public: //! @name OpenGL 1.2 additives to 1.1
  using theBaseClass_t::glBlendColor;
  using theBaseClass_t::glBlendEquation;

#if !defined(GL_ES_VERSION_2_0)
  using theBaseClass_t::glCopyTexSubImage3D;
  using theBaseClass_t::glDrawRangeElements;
  using theBaseClass_t::glTexImage3D;
  using theBaseClass_t::glTexSubImage3D;
#endif
};

#endif // _OpenGl_GlCore12_Header
