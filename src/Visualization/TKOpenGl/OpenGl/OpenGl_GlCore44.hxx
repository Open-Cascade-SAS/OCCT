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

#ifndef OpenGl_GlCore44_HeaderFile
#define OpenGl_GlCore44_HeaderFile

#include <OpenGl_GlCore43.hxx>

//! OpenGL 4.4 definition.
struct OpenGl_GlCore44 : public OpenGl_GlCore43
{
private:
  typedef OpenGl_GlCore43 theBaseClass_t;

public: //! @name OpenGL 4.4 additives to 4.3
  using theBaseClass_t::glBindBuffersBase;
  using theBaseClass_t::glBindBuffersRange;
  using theBaseClass_t::glBindImageTextures;
  using theBaseClass_t::glBindSamplers;
  using theBaseClass_t::glBindTextures;
  using theBaseClass_t::glBindVertexBuffers;
  using theBaseClass_t::glBufferStorage;
  using theBaseClass_t::glClearTexImage;
  using theBaseClass_t::glClearTexSubImage;
};

#endif // _OpenGl_GlCore44_Header
