// File:      OpenGl_ArbVBO.hxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

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
