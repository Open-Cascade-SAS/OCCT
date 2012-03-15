// File:      OpenGl_ExtVBO.hxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_ExtFBO_H__
#define _OpenGl_ExtFBO_H__

#include <OpenGl_GlCore12.hxx>

//! FBO is available on OpenGL 2.0+ hardware
struct OpenGl_ExtFBO
{

  PFNGLGENFRAMEBUFFERSEXTPROC         glGenFramebuffersEXT;
  PFNGLDELETEFRAMEBUFFERSEXTPROC      glDeleteFramebuffersEXT;
  PFNGLBINDFRAMEBUFFEREXTPROC         glBindFramebufferEXT;
  PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    glFramebufferTexture2DEXT;
  PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  glCheckFramebufferStatusEXT;
  PFNGLGENRENDERBUFFERSEXTPROC        glGenRenderbuffersEXT;
  PFNGLDELETERENDERBUFFERSEXTPROC     glDeleteRenderbuffersEXT;
  PFNGLBINDRENDERBUFFEREXTPROC        glBindRenderbufferEXT;
  PFNGLRENDERBUFFERSTORAGEEXTPROC     glRenderbufferStorageEXT;
  PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;

};

#endif // _OpenGl_ExtFBO_H__
