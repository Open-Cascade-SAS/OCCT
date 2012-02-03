// File:      OpenGl_ExtVBO.hxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_ExtFBO_H__
#define _OpenGl_ExtFBO_H__

#if (defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
#endif
#include <GL/gl.h>

//! FBO is part of OpenGL since 2.0
struct OpenGl_ExtFBO
{
  #ifndef GL_FRAMEBUFFER_EXT
    #define GL_FRAMEBUFFER_EXT 0x8D40
  #endif
  #ifndef GL_COLOR_ATTACHMENT0_EXT
    #define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
  #endif
  #ifndef GL_FRAMEBUFFER_COMPLETE_EXT
    #define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
  #endif
  #ifndef GL_RENDERBUFFER_EXT
    #define GL_RENDERBUFFER_EXT 0x8D41
  #endif
  #ifndef GL_DEPTH_ATTACHMENT_EXT
    #define GL_DEPTH_ATTACHMENT_EXT 0x8D00
  #endif

public:

  typedef void (APIENTRY *glGenFramebuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (APIENTRY *glDeleteFramebuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (APIENTRY *glBindFramebufferEXT_t) (GLenum target, GLuint id);
  typedef void (APIENTRY *glFramebufferTexture2DEXT_t) (GLenum target, GLenum attachmentPoint,
                                                        GLenum textureTarget, GLuint textureId,
                                                        GLint level);
  typedef GLenum (APIENTRY *glCheckFramebufferStatusEXT_t) (GLenum target);
  typedef void (APIENTRY *glGenRenderbuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (APIENTRY *glDeleteRenderbuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (APIENTRY *glBindRenderbufferEXT_t) (GLenum target, GLuint id);
  typedef void (APIENTRY *glRenderbufferStorageEXT_t) (GLenum target, GLenum internalFormat,
                                                       GLsizei width, GLsizei height);

  typedef void (APIENTRY *glFramebufferRenderbufferEXT_t) (GLenum target,
                                                           GLenum attachmentPoint,
                                                           GLenum renderbufferTarget,
                                                           GLuint renderbufferId);

public:

  glGenFramebuffersEXT_t         glGenFramebuffersEXT;
  glDeleteFramebuffersEXT_t      glDeleteFramebuffersEXT;
  glBindFramebufferEXT_t         glBindFramebufferEXT;
  glFramebufferTexture2DEXT_t    glFramebufferTexture2DEXT;
  glCheckFramebufferStatusEXT_t  glCheckFramebufferStatusEXT;
  glGenRenderbuffersEXT_t        glGenRenderbuffersEXT;
  glDeleteRenderbuffersEXT_t     glDeleteRenderbuffersEXT;
  glBindRenderbufferEXT_t        glBindRenderbufferEXT;
  glRenderbufferStorageEXT_t     glRenderbufferStorageEXT;
  glFramebufferRenderbufferEXT_t glFramebufferRenderbufferEXT;

};

#endif // _OpenGl_ExtFBO_H__
