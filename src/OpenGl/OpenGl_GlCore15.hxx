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

#ifndef _OpenGl_GlCore15_H__
#define _OpenGl_GlCore15_H__

#include <OpenGl_GlCore14.hxx>

//! Function list for GL1.5 core functionality.
struct OpenGl_GlCore15 : public OpenGl_GlCore14
{

  PFNGLGENQUERIESPROC           glGenQueries;
  PFNGLDELETEQUERIESPROC        glDeleteQueries;
  PFNGLISQUERYPROC              glIsQuery;
  PFNGLBEGINQUERYPROC           glBeginQuery;
  PFNGLENDQUERYPROC             glEndQuery;
  PFNGLGETQUERYIVPROC           glGetQueryiv;
  PFNGLGETQUERYOBJECTIVPROC     glGetQueryObjectiv;
  PFNGLGETQUERYOBJECTUIVPROC    glGetQueryObjectuiv;
  PFNGLBINDBUFFERPROC           glBindBuffer;
  PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
  PFNGLGENBUFFERSPROC           glGenBuffers;
  PFNGLISBUFFERPROC             glIsBuffer;
  PFNGLBUFFERDATAPROC           glBufferData;
  PFNGLBUFFERSUBDATAPROC        glBufferSubData;
  PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData;
  PFNGLMAPBUFFERPROC            glMapBuffer;
  PFNGLUNMAPBUFFERPROC          glUnmapBuffer;
  PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
  PFNGLGETBUFFERPOINTERVPROC    glGetBufferPointerv;

};

#endif // _OpenGl_GlCore15_H__
