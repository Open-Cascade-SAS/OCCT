// File:      OpenGl_ArbVBO.hxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#ifndef _OpenGl_ArbVBO_H__
#define _OpenGl_ArbVBO_H__

#if (defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
#endif
#include <GL/gl.h>

typedef ptrdiff_t GLsizeiptr;

//! VBO is part of OpenGL since 1.5
struct OpenGl_ArbVBO
{
  #ifndef GL_ARRAY_BUFFER_ARB
    #define GL_ARRAY_BUFFER_ARB          0x8892
  #endif
  #ifndef GL_STATIC_DRAW_ARB
    #define GL_STATIC_DRAW_ARB           0x88E4
  #endif
  #ifndef GL_ELEMENTS_ARRAY_BUFFER_ARB
    #define GL_ELEMENTS_ARRAY_BUFFER_ARB 0x8893
  #endif

public:

  typedef void (APIENTRY *glBindBuffer_t)    (GLenum target, GLuint buffer);
  typedef void (APIENTRY *glDeleteBuffers_t) (GLsizei n, const GLuint* buffers);
  typedef void (APIENTRY *glGenBuffers_t)    (GLsizei n, GLuint* buffers);
  typedef void (APIENTRY *glBufferData_t)    (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

public:

  glGenBuffers_t    glGenBuffersARB;
  glBindBuffer_t    glBindBufferARB;
  glBufferData_t    glBufferDataARB;
  glDeleteBuffers_t glDeleteBuffersARB;

};

#endif // _OpenGl_ArbVBO_H__
