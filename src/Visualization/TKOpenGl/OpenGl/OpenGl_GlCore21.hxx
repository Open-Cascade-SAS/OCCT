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

#ifndef OpenGl_GlCore21_HeaderFile
#define OpenGl_GlCore21_HeaderFile

#include <OpenGl_GlCore20.hxx>

//! OpenGL 2.1 core based on 2.0 version.
struct OpenGl_GlCore21 : public OpenGl_GlCore20
{
private:
  typedef OpenGl_GlCore20 theBaseClass_t;

public: //! @name OpenGL 2.1 additives to 2.0
#if !defined(GL_ES_VERSION_2_0)

  using theBaseClass_t::glUniformMatrix2x3fv;
  using theBaseClass_t::glUniformMatrix2x4fv;
  using theBaseClass_t::glUniformMatrix3x2fv;
  using theBaseClass_t::glUniformMatrix3x4fv;
  using theBaseClass_t::glUniformMatrix4x2fv;
  using theBaseClass_t::glUniformMatrix4x3fv;

#endif
};

#endif // _OpenGl_GlCore21_Header
