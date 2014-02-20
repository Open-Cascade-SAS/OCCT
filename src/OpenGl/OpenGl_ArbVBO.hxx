// Created on: 2012-01-26
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

#ifndef _OpenGl_ArbVBO_H__
#define _OpenGl_ArbVBO_H__

#include <OpenGl_GlCore12.hxx>

//! VBO is part of OpenGL since 1.5
struct OpenGl_ArbVBO
{

  PFNGLGENBUFFERSARBPROC    glGenBuffersARB;
  PFNGLBINDBUFFERARBPROC    glBindBufferARB;
  PFNGLBUFFERDATAARBPROC    glBufferDataARB;
  PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

};

#endif // _OpenGl_ArbVBO_H__
