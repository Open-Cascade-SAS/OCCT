// File:      OpenGl_GlCore15.hxx
// Created:   06 March 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

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
