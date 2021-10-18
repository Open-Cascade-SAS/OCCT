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

#ifndef OpenGl_GlFunctions_HeaderFile
#define OpenGl_GlFunctions_HeaderFile

// required for correct APIENTRY definition
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>

#ifndef APIENTRY
  #define APIENTRY
#endif
#ifndef APIENTRYP
  #define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
  #define GLAPI extern
#endif

// exclude modern definitions and system-provided glext.h, should be defined before gl.h inclusion
#ifndef GL_GLEXT_LEGACY
  #define GL_GLEXT_LEGACY
#endif
#ifndef GLX_GLXEXT_LEGACY
  #define GLX_GLXEXT_LEGACY
#endif

// include main OpenGL header provided with system
#if defined(__APPLE__)
  #import <TargetConditionals.h>
  // macOS 10.4 deprecated OpenGL framework - suppress useless warnings
  #define GL_SILENCE_DEPRECATION
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    #include <OpenGLES/ES3/gl.h>
  #else
    #include <OpenGL/gl.h>
  #endif
  #define __X_GL_H // prevent chaotic gl.h inclusions to avoid compile errors
#elif defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
  #if defined(_WIN32)
    // Angle OpenGL ES headers do not define function prototypes even for core functions,
    // however OCCT is expected to be linked against libGLESv2
    #define GL_GLEXT_PROTOTYPES
  #endif
  #include <GLES3/gl3.h>
#else
  #include <GL/gl.h>
#endif

#if !defined(HAVE_EGL)
#if defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__) || defined(HAVE_GLES2) || defined(OCCT_UWP)
  #define HAVE_EGL
#elif !defined(_WIN32) && !defined(__APPLE__) && !defined(HAVE_XLIB)
  #define HAVE_EGL
#endif
#endif
struct Aspect_XDisplay;

#if defined(GL_ES_VERSION_2_0)
  #include <OpenGl_GLESExtensions.hxx>
#else
  // GL version can be defined by system gl.h header
  #undef GL_VERSION_1_2
  #undef GL_VERSION_1_3
  #undef GL_VERSION_1_4
  #undef GL_VERSION_1_5
  #undef GL_VERSION_2_0
  #undef GL_VERSION_2_1
  #undef GL_VERSION_3_0
  #undef GL_VERSION_3_1
  #undef GL_VERSION_3_2
  #undef GL_VERSION_3_3
  #undef GL_VERSION_4_0
  #undef GL_VERSION_4_1
  #undef GL_VERSION_4_2
  #undef GL_VERSION_4_3
  #undef GL_VERSION_4_4
  #undef GL_VERSION_4_5

  // include glext.h provided by Khronos group
  #include <glext.h>
#endif

class OpenGl_Context;

//! Mega structure defines the complete list of OpenGL functions.
struct OpenGl_GlFunctions
{

  //! Check glGetError(); defined for debugging purposes.
  //! @return TRUE on error
  Standard_EXPORT bool debugPrintError (const char* theName) const;

  //! Load functions.
  Standard_EXPORT void load (OpenGl_Context& theCtx,
                             Standard_Boolean theIsCoreProfile);

  // This debug macros can be enabled to help debugging OpenGL implementations
  // without solid / working debugging capabilities.
  //#define OpenGl_TRACE(theName) {OpenGl_GlFunctions::debugPrintError(#theName);}
  #define OpenGl_TRACE(theName)

public: //! @name OpenGL ES 1.1

#if defined(GL_ES_VERSION_2_0)

  inline void glActiveTexture (GLenum texture) const
  {
    ::glActiveTexture (texture);
    OpenGl_TRACE(glActiveTexture)
  }

  inline void glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) const
  {
    ::glCompressedTexImage2D (target, level, internalformat, width, height, border, imageSize, data);
    OpenGl_TRACE(glCompressedTexImage2D)
  }

  inline void glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) const
  {
    ::glCompressedTexSubImage2D (target, level, xoffset, yoffset, width, height, format, imageSize, data);
    OpenGl_TRACE(glCompressedTexSubImage2D)
  }

  inline void glBindBuffer (GLenum target, GLuint buffer) const
  {
    ::glBindBuffer (target, buffer);
    OpenGl_TRACE(glBindBuffer)
  }

  inline void glBufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage) const
  {
    ::glBufferData (target, size, data, usage);
    OpenGl_TRACE(glBufferData)
  }

  inline void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void* data) const
  {
    ::glBufferSubData (target, offset, size, data);
    OpenGl_TRACE(glBufferSubData)
  }

  inline void glDeleteBuffers (GLsizei n, const GLuint *buffers) const
  {
    ::glDeleteBuffers (n, buffers);
    OpenGl_TRACE(glDeleteBuffers)
  }

  inline void glDeleteTextures (GLsizei n, const GLuint *textures) const
  {
    ::glDeleteTextures (n, textures);
    OpenGl_TRACE(glDeleteTextures)
  }

  inline void glDepthFunc (GLenum func) const
  {
    ::glDepthFunc (func);
    OpenGl_TRACE(glDepthFunc)
  }

  inline void glDepthMask (GLboolean flag) const
  {
    ::glDepthMask (flag);
    OpenGl_TRACE(glDepthMask)
  }

  inline void glDepthRangef (GLfloat n, GLfloat f) const
  {
    ::glDepthRangef (n, f);
    OpenGl_TRACE(glDepthRangef)
  }

  inline void glGenBuffers (GLsizei n, GLuint *buffers) const
  {
    ::glGenBuffers (n, buffers);
    OpenGl_TRACE(glGenBuffers)
  }

  inline void glGenTextures (GLsizei n, GLuint *textures) const
  {
    ::glGenTextures (n, textures);
    OpenGl_TRACE(glGenTextures)
  }

  inline void glGetBufferParameteriv (GLenum target, GLenum pname, GLint* params) const
  {
    ::glGetBufferParameteriv (target, pname, params);
    OpenGl_TRACE(glGetBufferParameteriv)
  }

  inline GLboolean glIsBuffer (GLuint buffer) const
  {
    return ::glIsBuffer (buffer);
  }

  inline void glSampleCoverage (GLfloat value, GLboolean invert) const
  {
    ::glSampleCoverage (value, invert);
    OpenGl_TRACE(glSampleCoverage)
  }

  inline void glMultiDrawElements (GLenum theMode, const GLsizei* theCount, GLenum theType, const void* const* theIndices, GLsizei theDrawCount) const
  {
    if (theCount   == NULL
     || theIndices == NULL)
    {
      return;
    }

    for (GLsizei aBatchIter = 0; aBatchIter < theDrawCount; ++aBatchIter)
    {
      ::glDrawElements (theMode, theCount[aBatchIter], theType, theIndices[aBatchIter]);
    }
    OpenGl_TRACE(glMultiDrawElements)
  }

#endif

public: //! @name OpenGL ES 2.0

#if defined(GL_ES_VERSION_2_0)
  inline void glBlendColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) const
  {
    ::glBlendColor (red, green, blue, alpha);
    OpenGl_TRACE(glBlendColor)
  }

  inline void glBlendEquation (GLenum mode) const
  {
    ::glBlendEquation (mode);
    OpenGl_TRACE(glBlendEquation)
  }

  inline void glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) const
  {
    ::glBlendFuncSeparate (sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    OpenGl_TRACE(glBlendFuncSeparate)
  }

  inline void glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha) const
  {
    ::glBlendEquationSeparate (modeRGB, modeAlpha);
    OpenGl_TRACE(glBlendEquationSeparate)
  }

  inline void glStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) const
  {
    ::glStencilOpSeparate (face, sfail, dpfail, dppass);
    OpenGl_TRACE(glStencilOpSeparate)
  }

  inline void glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask) const
  {
    ::glStencilFuncSeparate (face, func, ref, mask);
    OpenGl_TRACE(glStencilFuncSeparate)
  }

  inline void glStencilMaskSeparate (GLenum face, GLuint mask) const
  {
    ::glStencilMaskSeparate (face, mask);
    OpenGl_TRACE(glStencilMaskSeparate)
  }

  inline void glAttachShader (GLuint program, GLuint shader) const
  {
    ::glAttachShader (program, shader);
    OpenGl_TRACE(glAttachShader)
  }

  inline void glBindAttribLocation (GLuint program, GLuint index, const GLchar *name) const
  {
    ::glBindAttribLocation (program, index, name);
    OpenGl_TRACE(glBindAttribLocation)
  }

  inline void glBindFramebuffer (GLenum target, GLuint framebuffer) const
  {
    ::glBindFramebuffer (target, framebuffer);
    OpenGl_TRACE(glBindFramebuffer)
  }

  inline void glBindRenderbuffer (GLenum target, GLuint renderbuffer) const
  {
    ::glBindRenderbuffer (target, renderbuffer);
    OpenGl_TRACE(glBindRenderbuffer)
  }

  inline GLenum glCheckFramebufferStatus (GLenum target) const
  {
    return ::glCheckFramebufferStatus (target);
  }

  inline void glCompileShader (GLuint shader) const
  {
    ::glCompileShader (shader);
    OpenGl_TRACE(glCompileShader)
  }

  inline GLuint glCreateProgram() const
  {
    return ::glCreateProgram();
  }

  inline GLuint glCreateShader (GLenum type) const
  {
    return ::glCreateShader (type);
  }

  inline void glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers) const
  {
    ::glDeleteFramebuffers (n, framebuffers);
    OpenGl_TRACE(glDeleteFramebuffers)
  }

  inline void glDeleteProgram (GLuint program) const
  {
    ::glDeleteProgram (program);
    OpenGl_TRACE(glDeleteProgram)
  }

  inline void glDeleteRenderbuffers (GLsizei n, const GLuint *renderbuffers) const
  {
    ::glDeleteRenderbuffers (n, renderbuffers);
    OpenGl_TRACE(glDeleteRenderbuffers)
  }

  inline void glDeleteShader (GLuint shader) const
  {
    ::glDeleteShader (shader);
    OpenGl_TRACE(glDeleteShader)
  }

  inline void glDetachShader (GLuint program, GLuint shader) const
  {
    ::glDetachShader (program, shader);
    OpenGl_TRACE(glDetachShader)
  }

  inline void glDisableVertexAttribArray (GLuint index) const
  {
    ::glDisableVertexAttribArray (index);
    OpenGl_TRACE(glDisableVertexAttribArray)
  }

  inline void glEnableVertexAttribArray (GLuint index) const
  {
    ::glEnableVertexAttribArray (index);
    OpenGl_TRACE(glEnableVertexAttribArray)
  }

  inline void glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) const
  {
    ::glFramebufferRenderbuffer (target, attachment, renderbuffertarget, renderbuffer);
    OpenGl_TRACE(glFramebufferRenderbuffer)
  }

  inline void glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) const
  {
    ::glFramebufferTexture2D (target, attachment, textarget, texture, level);
    OpenGl_TRACE(glFramebufferTexture2D)
  }

  inline void glGenerateMipmap (GLenum target) const
  {
    ::glGenerateMipmap (target);
    OpenGl_TRACE(glGenerateMipmap)
  }

  inline void glGenFramebuffers (GLsizei n, GLuint *framebuffers) const
  {
    ::glGenFramebuffers (n, framebuffers);
    OpenGl_TRACE(glGenFramebuffers)
  }

  inline void glGenRenderbuffers (GLsizei n, GLuint *renderbuffers) const
  {
    ::glGenRenderbuffers (n, renderbuffers);
    OpenGl_TRACE(glGenRenderbuffers)
  }

  inline void glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint* size, GLenum *type, GLchar *name) const
  {
    ::glGetActiveAttrib (program, index, bufSize, length, size, type, name);
    OpenGl_TRACE(glGetActiveAttrib)
  }

  inline void glGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint* size, GLenum *type, GLchar *name) const
  {
    ::glGetActiveUniform (program, index, bufSize, length, size, type, name);
    OpenGl_TRACE(glGetActiveUniform)
  }

  inline void glGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders) const
  {
    ::glGetAttachedShaders (program, maxCount, count, shaders);
    OpenGl_TRACE(glGetAttachedShaders)
  }

  inline GLint glGetAttribLocation (GLuint program, const GLchar *name) const
  {
    const GLint aRes = ::glGetAttribLocation (program, name);
    OpenGl_TRACE(glGetAttribLocation)
    return aRes;
  }

  inline void glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params) const
  {
    ::glGetFramebufferAttachmentParameteriv (target, attachment, pname, params);
    OpenGl_TRACE(glGetFramebufferAttachmentParameteriv)
  }

  inline void glGetProgramiv (GLuint program, GLenum pname, GLint* params) const
  {
    ::glGetProgramiv (program, pname, params);
    OpenGl_TRACE(glGetProgramiv)
  }

  inline void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) const
  {
    ::glGetProgramInfoLog (program, bufSize, length, infoLog);
    OpenGl_TRACE(glGetProgramInfoLog)
  }

  inline void glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params) const
  {
    ::glGetRenderbufferParameteriv (target, pname, params);
    OpenGl_TRACE(glGetRenderbufferParameteriv)
  }

  inline void glGetShaderiv (GLuint shader, GLenum pname, GLint* params) const
  {
    ::glGetShaderiv (shader, pname, params);
    OpenGl_TRACE(glGetShaderiv)
  }

  inline void glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) const
  {
    ::glGetShaderInfoLog (shader, bufSize, length, infoLog);
    OpenGl_TRACE(glGetShaderInfoLog)
  }

  inline void glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) const
  {
    ::glGetShaderPrecisionFormat (shadertype, precisiontype, range, precision);
    OpenGl_TRACE(glGetShaderPrecisionFormat)
  }

  inline void glGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source) const
  {
    ::glGetShaderSource (shader, bufSize, length, source);
    OpenGl_TRACE(glGetShaderSource)
  }

  inline void glGetUniformfv (GLuint program, GLint location, GLfloat* params) const
  {
    ::glGetUniformfv (program, location, params);
    OpenGl_TRACE(glGetUniformfv)
  }

  inline void glGetUniformiv (GLuint program, GLint location, GLint* params) const
  {
    ::glGetUniformiv (program, location, params);
    OpenGl_TRACE(glGetUniformiv)
  }

  GLint glGetUniformLocation (GLuint program, const GLchar *name) const
  {
    const GLint aRes = ::glGetUniformLocation (program, name);
    OpenGl_TRACE(glGetUniformLocation)
    return aRes;
  }

  inline void glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params) const
  {
    ::glGetVertexAttribfv (index, pname, params);
    OpenGl_TRACE(glGetVertexAttribfv)
  }

  inline void glGetVertexAttribiv (GLuint index, GLenum pname, GLint* params) const
  {
    ::glGetVertexAttribiv (index, pname, params);
    OpenGl_TRACE(glGetVertexAttribiv)
  }

  inline void glGetVertexAttribPointerv (GLuint index, GLenum pname, void* *pointer) const
  {
    ::glGetVertexAttribPointerv (index, pname, pointer);
    OpenGl_TRACE(glGetVertexAttribPointerv)
  }

  inline GLboolean glIsFramebuffer (GLuint framebuffer) const
  {
    return ::glIsFramebuffer (framebuffer);
  }

  inline GLboolean glIsProgram (GLuint program) const
  {
    return ::glIsProgram (program);
  }

  inline GLboolean glIsRenderbuffer (GLuint renderbuffer) const
  {
    return ::glIsRenderbuffer (renderbuffer);
  }

  inline GLboolean glIsShader (GLuint shader) const
  {
    return ::glIsShader (shader);
  }

  inline void glLinkProgram (GLuint program) const
  {
    ::glLinkProgram (program);
    OpenGl_TRACE(glLinkProgram)
  }

  inline void glReleaseShaderCompiler() const
  {
    ::glReleaseShaderCompiler();
    OpenGl_TRACE(glReleaseShaderCompiler)
  }

  inline void glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height) const
  {
    ::glRenderbufferStorage (target, internalformat, width, height);
    OpenGl_TRACE(glRenderbufferStorage)
  }

  inline void glShaderBinary (GLsizei count, const GLuint *shaders, GLenum binaryformat, const void* binary, GLsizei length) const
  {
    ::glShaderBinary (count, shaders, binaryformat, binary, length);
    OpenGl_TRACE(glShaderBinary)
  }

  inline void glShaderSource (GLuint shader, GLsizei count, const GLchar** string, const GLint* length) const
  {
    ::glShaderSource (shader, count, string, length);
    OpenGl_TRACE(glShaderSource)
  }

  inline void glUniform1f (GLint location, GLfloat v0) const
  {
    ::glUniform1f (location, v0);
    OpenGl_TRACE(glUniform1f)
  }

  inline void glUniform1fv (GLint location, GLsizei count, const GLfloat* value) const
  {
    ::glUniform1fv (location, count, value);
    OpenGl_TRACE(glUniform1fv)
  }

  inline void glUniform1i (GLint location, GLint v0) const
  {
    ::glUniform1i (location, v0);
    OpenGl_TRACE(glUniform1i)
  }

  inline void glUniform1iv (GLint location, GLsizei count, const GLint* value) const
  {
    ::glUniform1iv (location, count, value);
    OpenGl_TRACE(glUniform1iv)
  }

  inline void glUniform2f (GLint location, GLfloat v0, GLfloat v1) const
  {
    ::glUniform2f (location, v0, v1);
    OpenGl_TRACE(glUniform2f)
  }

  inline void glUniform2fv (GLint location, GLsizei count, const GLfloat* value) const
  {
    ::glUniform2fv (location, count, value);
    OpenGl_TRACE(glUniform2fv)
  }

  inline void glUniform2i (GLint location, GLint v0, GLint v1) const
  {
    ::glUniform2i (location, v0, v1);
    OpenGl_TRACE(glUniform2i)
  }

  inline void glUniform2iv (GLint location, GLsizei count, const GLint* value) const
  {
    ::glUniform2iv (location, count, value);
    OpenGl_TRACE(glUniform2iv)
  }

  inline void glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2) const
  {
    ::glUniform3f (location, v0, v1, v2);
    OpenGl_TRACE(glUniform3f)
  }

  inline void glUniform3fv (GLint location, GLsizei count, const GLfloat* value) const
  {
    ::glUniform3fv (location, count, value);
    OpenGl_TRACE(glUniform3fv)
  }

  inline void glUniform3i (GLint location, GLint v0, GLint v1, GLint v2) const
  {
    ::glUniform3i (location, v0, v1, v2);
    OpenGl_TRACE(glUniform3i)
  }

  inline void glUniform3iv (GLint location, GLsizei count, const GLint* value) const
  {
    ::glUniform3iv (location, count, value);
    OpenGl_TRACE(glUniform3iv)
  }

  inline void glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
  {
    ::glUniform4f (location, v0, v1, v2, v3);
    OpenGl_TRACE(glUniform4f)
  }

  inline void glUniform4fv (GLint location, GLsizei count, const GLfloat* value) const
  {
    ::glUniform4fv (location, count, value);
    OpenGl_TRACE(glUniform4fv)
  }

  inline void glUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3) const
  {
    ::glUniform4i (location, v0, v1, v2, v3);
    OpenGl_TRACE(glUniform4i)
  }

  inline void glUniform4iv (GLint location, GLsizei count, const GLint* value) const
  {
    ::glUniform4iv (location, count, value);
    OpenGl_TRACE(glUniform4iv)
  }

  inline void glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) const
  {
    ::glUniformMatrix2fv (location, count, transpose, value);
    OpenGl_TRACE(glUniformMatrix2fv)
  }

  inline void glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) const
  {
    ::glUniformMatrix3fv (location, count, transpose, value);
    OpenGl_TRACE(glUniformMatrix3fv)
  }

  inline void glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) const
  {
    ::glUniformMatrix4fv (location, count, transpose, value);
    OpenGl_TRACE(glUniformMatrix4fv)
  }

  inline void glUseProgram (GLuint program) const
  {
    ::glUseProgram (program);
    OpenGl_TRACE(glUseProgram)
  }

  inline void glValidateProgram (GLuint program) const
  {
    ::glValidateProgram (program);
    OpenGl_TRACE(glValidateProgram)
  }

  inline void glVertexAttrib1f (GLuint index, GLfloat x) const
  {
    ::glVertexAttrib1f (index, x);
    OpenGl_TRACE(glVertexAttrib1f)
  }

  inline void glVertexAttrib1fv (GLuint index, const GLfloat* v) const
  {
    ::glVertexAttrib1fv (index, v);
    OpenGl_TRACE(glVertexAttrib1fv)
  }

  inline void glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y) const
  {
    ::glVertexAttrib2f (index, x, y);
    OpenGl_TRACE(glVertexAttrib2f)
  }

  inline void glVertexAttrib2fv (GLuint index, const GLfloat* v) const
  {
    ::glVertexAttrib2fv (index, v);
    OpenGl_TRACE(glVertexAttrib2fv)
  }

  inline void glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z) const
  {
    ::glVertexAttrib3f (index, x, y, z);
    OpenGl_TRACE(glVertexAttrib3f)
  }

  inline void glVertexAttrib3fv (GLuint index, const GLfloat* v) const
  {
    ::glVertexAttrib3fv (index, v);
    OpenGl_TRACE(glVertexAttrib3fv)
  }

  inline void glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const
  {
    ::glVertexAttrib4f (index, x, y, z, w);
    OpenGl_TRACE(glVertexAttrib4f)
  }

  inline void glVertexAttrib4fv (GLuint index, const GLfloat* v) const
  {
    ::glVertexAttrib4fv (index, v);
    OpenGl_TRACE(glVertexAttrib4fv)
  }

  inline void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) const
  {
    ::glVertexAttribPointer (index, size, type, normalized, stride, pointer);
    OpenGl_TRACE(glVertexAttribPointer)
  }

public: //! @name OpenGL ES 3.0

  opencascade::PFNGLREADBUFFERPROC glReadBuffer;
  opencascade::PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
  opencascade::PFNGLTEXIMAGE3DPROC glTexImage3D;
  opencascade::PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
  opencascade::PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;
  opencascade::PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
  opencascade::PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
  opencascade::PFNGLGENQUERIESPROC glGenQueries;
  opencascade::PFNGLDELETEQUERIESPROC glDeleteQueries;
  opencascade::PFNGLISQUERYPROC    glIsQuery;
  opencascade::PFNGLBEGINQUERYPROC glBeginQuery;
  opencascade::PFNGLENDQUERYPROC   glEndQuery;
  opencascade::PFNGLGETQUERYIVPROC glGetQueryiv;
  opencascade::PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
  opencascade::PFNGLUNMAPBUFFERPROC glUnmapBuffer;
  opencascade::PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;
  opencascade::PFNGLDRAWBUFFERSPROC glDrawBuffers;
  opencascade::PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
  opencascade::PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
  opencascade::PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
  opencascade::PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
  opencascade::PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
  opencascade::PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;
  opencascade::PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
  opencascade::PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
  opencascade::PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
  opencascade::PFNGLMAPBUFFERRANGEPROC  glMapBufferRange;
  opencascade::PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
  opencascade::PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
  opencascade::PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
  opencascade::PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
  opencascade::PFNGLISVERTEXARRAYPROC   glIsVertexArray;
  opencascade::PFNGLGETINTEGERI_VPROC   glGetIntegeri_v;
  opencascade::PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
  opencascade::PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
  opencascade::PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
  opencascade::PFNGLBINDBUFFERBASEPROC  glBindBufferBase;
  opencascade::PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
  opencascade::PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
  opencascade::PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
  opencascade::PFNGLGETVERTEXATTRIBIIVPROC   glGetVertexAttribIiv;
  opencascade::PFNGLGETVERTEXATTRIBIUIVPROC  glGetVertexAttribIuiv;
  opencascade::PFNGLVERTEXATTRIBI4IPROC   glVertexAttribI4i;
  opencascade::PFNGLVERTEXATTRIBI4UIPROC  glVertexAttribI4ui;
  opencascade::PFNGLVERTEXATTRIBI4IVPROC  glVertexAttribI4iv;
  opencascade::PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
  opencascade::PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
  opencascade::PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
  opencascade::PFNGLUNIFORM1UIPROC glUniform1ui;
  opencascade::PFNGLUNIFORM2UIPROC glUniform2ui;
  opencascade::PFNGLUNIFORM3UIPROC glUniform3ui;
  opencascade::PFNGLUNIFORM4UIPROC glUniform4ui;
  opencascade::PFNGLUNIFORM1UIVPROC glUniform1uiv;
  opencascade::PFNGLUNIFORM2UIVPROC glUniform2uiv;
  opencascade::PFNGLUNIFORM3UIVPROC glUniform3uiv;
  opencascade::PFNGLUNIFORM4UIVPROC glUniform4uiv;
  opencascade::PFNGLCLEARBUFFERIVPROC  glClearBufferiv;
  opencascade::PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
  opencascade::PFNGLCLEARBUFFERFVPROC  glClearBufferfv;
  opencascade::PFNGLCLEARBUFFERFIPROC  glClearBufferfi;
  opencascade::PFNGLGETSTRINGIPROC glGetStringi;
  opencascade::PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
  opencascade::PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
  opencascade::PFNGLGETACTIVEUNIFORMSIVPROC  glGetActiveUniformsiv;
  opencascade::PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
  opencascade::PFNGLGETACTIVEUNIFORMBLOCKIVPROC   glGetActiveUniformBlockiv;
  opencascade::PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
  opencascade::PFNGLUNIFORMBLOCKBINDINGPROC   glUniformBlockBinding;
  opencascade::PFNGLDRAWARRAYSINSTANCEDPROC   glDrawArraysInstanced;
  opencascade::PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
  opencascade::PFNGLFENCESYNCPROC      glFenceSync;
  opencascade::PFNGLISSYNCPROC         glIsSync;
  opencascade::PFNGLDELETESYNCPROC     glDeleteSync;
  opencascade::PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
  opencascade::PFNGLWAITSYNCPROC       glWaitSync;
  opencascade::PFNGLGETINTEGER64VPROC  glGetInteger64v;
  opencascade::PFNGLGETSYNCIVPROC      glGetSynciv;
  opencascade::PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
  opencascade::PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
  opencascade::PFNGLGENSAMPLERSPROC glGenSamplers;
  opencascade::PFNGLDELETESAMPLERSPROC glDeleteSamplers;
  opencascade::PFNGLISSAMPLERPROC   glIsSampler;
  opencascade::PFNGLBINDSAMPLERPROC glBindSampler;
  opencascade::PFNGLSAMPLERPARAMETERIPROC  glSamplerParameteri;
  opencascade::PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
  opencascade::PFNGLSAMPLERPARAMETERFPROC  glSamplerParameterf;
  opencascade::PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
  opencascade::PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
  opencascade::PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
  opencascade::PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
  opencascade::PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback;
  opencascade::PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
  opencascade::PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks;
  opencascade::PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback;
  opencascade::PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback;
  opencascade::PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback;
  opencascade::PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
  opencascade::PFNGLPROGRAMBINARYPROC glProgramBinary;
  opencascade::PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
  opencascade::PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;
  opencascade::PFNGLINVALIDATESUBFRAMEBUFFERPROC glInvalidateSubFramebuffer;
  opencascade::PFNGLTEXSTORAGE2DPROC glTexStorage2D;
  opencascade::PFNGLTEXSTORAGE3DPROC glTexStorage3D;
  opencascade::PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;

public: //! @name OpenGL ES 3.1

  opencascade::PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
  opencascade::PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
  opencascade::PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
  opencascade::PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
  opencascade::PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri;
  opencascade::PFNGLGETFRAMEBUFFERPARAMETERIVPROC glGetFramebufferParameteriv;
  opencascade::PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
  opencascade::PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
  opencascade::PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
  opencascade::PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv;
  opencascade::PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation;
  opencascade::PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
  opencascade::PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram;
  opencascade::PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
  opencascade::PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
  opencascade::PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
  opencascade::PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
  opencascade::PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline;
  opencascade::PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv;
  opencascade::PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i;
  opencascade::PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i;
  opencascade::PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i;
  opencascade::PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i;
  opencascade::PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui;
  opencascade::PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui;
  opencascade::PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui;
  opencascade::PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui;
  opencascade::PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f;
  opencascade::PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f;
  opencascade::PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f;
  opencascade::PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f;
  opencascade::PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv;
  opencascade::PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv;
  opencascade::PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv;
  opencascade::PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv;
  opencascade::PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv;
  opencascade::PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv;
  opencascade::PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv;
  opencascade::PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv;
  opencascade::PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv;
  opencascade::PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv;
  opencascade::PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv;
  opencascade::PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
  opencascade::PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
  opencascade::PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline;
  opencascade::PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;
  opencascade::PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
  opencascade::PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
  opencascade::PFNGLMEMORYBARRIERPROC glMemoryBarrier;
  opencascade::PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion;
  opencascade::PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
  opencascade::PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
  opencascade::PFNGLSAMPLEMASKIPROC glSampleMaski;
  opencascade::PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
  opencascade::PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
  opencascade::PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
  opencascade::PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
  opencascade::PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
  opencascade::PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
  opencascade::PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;

public: //! @name OpenGL ES 3.2

  opencascade::PFNGLBLENDBARRIERPROC glBlendBarrier;
  opencascade::PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData;
  opencascade::PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
  opencascade::PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
  opencascade::PFNGLOBJECTLABELPROC glObjectLabel;
  opencascade::PFNGLGETOBJECTLABELPROC glGetObjectLabel;
  opencascade::PFNGLOBJECTPTRLABELPROC glObjectPtrLabel;
  opencascade::PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel;
  opencascade::PFNGLGETPOINTERVPROC glGetPointerv;
  opencascade::PFNGLENABLEIPROC  glEnablei;
  opencascade::PFNGLDISABLEIPROC glDisablei;
  opencascade::PFNGLBLENDEQUATIONIPROC glBlendEquationi;
  opencascade::PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
  opencascade::PFNGLBLENDFUNCIPROC glBlendFunci;
  opencascade::PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
  opencascade::PFNGLCOLORMASKIPROC glColorMaski;
  opencascade::PFNGLISENABLEDIPROC glIsEnabledi;
  opencascade::PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
  opencascade::PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
  opencascade::PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
  opencascade::PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
  opencascade::PFNGLPRIMITIVEBOUNDINGBOXPROC glPrimitiveBoundingBox;
  opencascade::PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus;
  opencascade::PFNGLREADNPIXELSPROC glReadnPixels;
  opencascade::PFNGLGETNUNIFORMFVPROC glGetnUniformfv;
  opencascade::PFNGLGETNUNIFORMIVPROC glGetnUniformiv;
  opencascade::PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv;
  opencascade::PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
  opencascade::PFNGLPATCHPARAMETERIPROC glPatchParameteri;
  opencascade::PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
  opencascade::PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
  opencascade::PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
  opencascade::PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
  opencascade::PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
  opencascade::PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
  opencascade::PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
  opencascade::PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
  opencascade::PFNGLTEXBUFFERPROC glTexBuffer;
  opencascade::PFNGLTEXBUFFERRANGEPROC glTexBufferRange;
  opencascade::PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;

public: //! @name GL_KHR_debug (optional) or OpenGL ES 3.2+

  opencascade::PFNGLDEBUGMESSAGECONTROLPROC  glDebugMessageControl;
  opencascade::PFNGLDEBUGMESSAGEINSERTPROC   glDebugMessageInsert;
  opencascade::PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
  opencascade::PFNGLGETDEBUGMESSAGELOGPROC   glGetDebugMessageLog;

#else // OpenGL ES vs. desktop

public: //! @name OpenGL 1.2

  PFNGLBLENDCOLORPROC               glBlendColor;
  PFNGLBLENDEQUATIONPROC            glBlendEquation;
  PFNGLDRAWRANGEELEMENTSPROC        glDrawRangeElements;
  PFNGLTEXIMAGE3DPROC               glTexImage3D;
  PFNGLTEXSUBIMAGE3DPROC            glTexSubImage3D;
  PFNGLCOPYTEXSUBIMAGE3DPROC        glCopyTexSubImage3D;

public: //! @name OpenGL 1.3

  PFNGLACTIVETEXTUREPROC            glActiveTexture;
  PFNGLSAMPLECOVERAGEPROC           glSampleCoverage;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC     glCompressedTexImage3D;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC     glCompressedTexImage2D;
  PFNGLCOMPRESSEDTEXIMAGE1DPROC     glCompressedTexImage1D;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC  glCompressedTexSubImage3D;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC  glCompressedTexSubImage2D;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC  glCompressedTexSubImage1D;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC    glGetCompressedTexImage;

public: //! @name OpenGL 1.4

  PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate;
  PFNGLMULTIDRAWARRAYSPROC          glMultiDrawArrays;
  PFNGLMULTIDRAWELEMENTSPROC        glMultiDrawElements;
  PFNGLPOINTPARAMETERFPROC          glPointParameterf;
  PFNGLPOINTPARAMETERFVPROC         glPointParameterfv;
  PFNGLPOINTPARAMETERIPROC          glPointParameteri;
  PFNGLPOINTPARAMETERIVPROC         glPointParameteriv;

public: //! @name OpenGL 1.5

  PFNGLGENQUERIESPROC               glGenQueries;
  PFNGLDELETEQUERIESPROC            glDeleteQueries;
  PFNGLISQUERYPROC                  glIsQuery;
  PFNGLBEGINQUERYPROC               glBeginQuery;
  PFNGLENDQUERYPROC                 glEndQuery;
  PFNGLGETQUERYIVPROC               glGetQueryiv;
  PFNGLGETQUERYOBJECTIVPROC         glGetQueryObjectiv;
  PFNGLGETQUERYOBJECTUIVPROC        glGetQueryObjectuiv;
  PFNGLBINDBUFFERPROC               glBindBuffer;
  PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
  PFNGLGENBUFFERSPROC               glGenBuffers;
  PFNGLISBUFFERPROC                 glIsBuffer;
  PFNGLBUFFERDATAPROC               glBufferData;
  PFNGLBUFFERSUBDATAPROC            glBufferSubData;
  PFNGLGETBUFFERSUBDATAPROC         glGetBufferSubData;
  PFNGLMAPBUFFERPROC                glMapBuffer;
  PFNGLUNMAPBUFFERPROC              glUnmapBuffer;
  PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv;
  PFNGLGETBUFFERPOINTERVPROC        glGetBufferPointerv;

public: //! @name OpenGL 2.0

  PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate;
  PFNGLDRAWBUFFERSPROC              glDrawBuffers;
  PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
  PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
  PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate;
  PFNGLATTACHSHADERPROC             glAttachShader;
  PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
  PFNGLCOMPILESHADERPROC            glCompileShader;
  PFNGLCREATEPROGRAMPROC            glCreateProgram;
  PFNGLCREATESHADERPROC             glCreateShader;
  PFNGLDELETEPROGRAMPROC            glDeleteProgram;
  PFNGLDELETESHADERPROC             glDeleteShader;
  PFNGLDETACHSHADERPROC             glDetachShader;
  PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
  PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
  PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib;
  PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
  PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders;
  PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
  PFNGLGETPROGRAMIVPROC             glGetProgramiv;
  PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
  PFNGLGETSHADERIVPROC              glGetShaderiv;
  PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
  PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
  PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
  PFNGLGETUNIFORMFVPROC             glGetUniformfv;
  PFNGLGETUNIFORMIVPROC             glGetUniformiv;
  PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv;
  PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv;
  PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv;
  PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv;
  PFNGLISPROGRAMPROC                glIsProgram;
  PFNGLISSHADERPROC                 glIsShader;
  PFNGLLINKPROGRAMPROC              glLinkProgram;
  PFNGLSHADERSOURCEPROC             glShaderSource;
  PFNGLUSEPROGRAMPROC               glUseProgram;
  PFNGLUNIFORM1FPROC                glUniform1f;
  PFNGLUNIFORM2FPROC                glUniform2f;
  PFNGLUNIFORM3FPROC                glUniform3f;
  PFNGLUNIFORM4FPROC                glUniform4f;
  PFNGLUNIFORM1IPROC                glUniform1i;
  PFNGLUNIFORM2IPROC                glUniform2i;
  PFNGLUNIFORM3IPROC                glUniform3i;
  PFNGLUNIFORM4IPROC                glUniform4i;
  PFNGLUNIFORM1FVPROC               glUniform1fv;
  PFNGLUNIFORM2FVPROC               glUniform2fv;
  PFNGLUNIFORM3FVPROC               glUniform3fv;
  PFNGLUNIFORM4FVPROC               glUniform4fv;
  PFNGLUNIFORM1IVPROC               glUniform1iv;
  PFNGLUNIFORM2IVPROC               glUniform2iv;
  PFNGLUNIFORM3IVPROC               glUniform3iv;
  PFNGLUNIFORM4IVPROC               glUniform4iv;
  PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
  PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
  PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
  PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
  PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d;
  PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv;
  PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
  PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
  PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s;
  PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv;
  PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d;
  PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv;
  PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f;
  PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
  PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s;
  PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv;
  PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d;
  PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv;
  PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f;
  PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
  PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s;
  PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv;
  PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv;
  PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv;
  PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv;
  PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub;
  PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv;
  PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv;
  PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv;
  PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv;
  PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d;
  PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv;
  PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
  PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
  PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv;
  PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s;
  PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv;
  PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv;
  PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv;
  PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv;
  PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;

public: //! @name OpenGL 2.1

  PFNGLUNIFORMMATRIX2X3FVPROC       glUniformMatrix2x3fv;
  PFNGLUNIFORMMATRIX3X2FVPROC       glUniformMatrix3x2fv;
  PFNGLUNIFORMMATRIX2X4FVPROC       glUniformMatrix2x4fv;
  PFNGLUNIFORMMATRIX4X2FVPROC       glUniformMatrix4x2fv;
  PFNGLUNIFORMMATRIX3X4FVPROC       glUniformMatrix3x4fv;
  PFNGLUNIFORMMATRIX4X3FVPROC       glUniformMatrix4x3fv;

public: //! @name GL_ARB_framebuffer_object (added to OpenGL 3.0 core)

  PFNGLISRENDERBUFFERPROC                      glIsRenderbuffer;
  PFNGLBINDRENDERBUFFERPROC                    glBindRenderbuffer;
  PFNGLDELETERENDERBUFFERSPROC                 glDeleteRenderbuffers;
  PFNGLGENRENDERBUFFERSPROC                    glGenRenderbuffers;
  PFNGLRENDERBUFFERSTORAGEPROC                 glRenderbufferStorage;
  PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv;
  PFNGLISFRAMEBUFFERPROC                       glIsFramebuffer;
  PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer;
  PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers;
  PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers;
  PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus;
  PFNGLFRAMEBUFFERTEXTURE1DPROC                glFramebufferTexture1D;
  PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D;
  PFNGLFRAMEBUFFERTEXTURE3DPROC                glFramebufferTexture3D;
  PFNGLFRAMEBUFFERRENDERBUFFERPROC             glFramebufferRenderbuffer;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
  PFNGLGENERATEMIPMAPPROC                      glGenerateMipmap;
  PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer;
  PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC      glRenderbufferStorageMultisample;
  PFNGLFRAMEBUFFERTEXTURELAYERPROC             glFramebufferTextureLayer;

public: //! @name GL_ARB_vertex_array_object (added to OpenGL 3.0 core)

  PFNGLBINDVERTEXARRAYPROC             glBindVertexArray;
  PFNGLDELETEVERTEXARRAYSPROC          glDeleteVertexArrays;
  PFNGLGENVERTEXARRAYSPROC             glGenVertexArrays;
  PFNGLISVERTEXARRAYPROC               glIsVertexArray;

public: //! @name GL_ARB_map_buffer_range (added to OpenGL 3.0 core)

  PFNGLMAPBUFFERRANGEPROC              glMapBufferRange;
  PFNGLFLUSHMAPPEDBUFFERRANGEPROC      glFlushMappedBufferRange;

public: //! @name OpenGL 3.0

  PFNGLCOLORMASKIPROC                  glColorMaski;
  PFNGLGETBOOLEANI_VPROC               glGetBooleani_v;
  PFNGLGETINTEGERI_VPROC               glGetIntegeri_v;
  PFNGLENABLEIPROC                     glEnablei;
  PFNGLDISABLEIPROC                    glDisablei;
  PFNGLISENABLEDIPROC                  glIsEnabledi;
  PFNGLBEGINTRANSFORMFEEDBACKPROC      glBeginTransformFeedback;
  PFNGLENDTRANSFORMFEEDBACKPROC        glEndTransformFeedback;
  PFNGLBINDBUFFERRANGEPROC             glBindBufferRange;
  PFNGLBINDBUFFERBASEPROC              glBindBufferBase;
  PFNGLTRANSFORMFEEDBACKVARYINGSPROC   glTransformFeedbackVaryings;
  PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
  PFNGLCLAMPCOLORPROC                  glClampColor;
  PFNGLBEGINCONDITIONALRENDERPROC      glBeginConditionalRender;
  PFNGLENDCONDITIONALRENDERPROC        glEndConditionalRender;
  PFNGLVERTEXATTRIBIPOINTERPROC        glVertexAttribIPointer;
  PFNGLGETVERTEXATTRIBIIVPROC          glGetVertexAttribIiv;
  PFNGLGETVERTEXATTRIBIUIVPROC         glGetVertexAttribIuiv;
  PFNGLVERTEXATTRIBI1IPROC             glVertexAttribI1i;
  PFNGLVERTEXATTRIBI2IPROC             glVertexAttribI2i;
  PFNGLVERTEXATTRIBI3IPROC             glVertexAttribI3i;
  PFNGLVERTEXATTRIBI4IPROC             glVertexAttribI4i;
  PFNGLVERTEXATTRIBI1UIPROC            glVertexAttribI1ui;
  PFNGLVERTEXATTRIBI2UIPROC            glVertexAttribI2ui;
  PFNGLVERTEXATTRIBI3UIPROC            glVertexAttribI3ui;
  PFNGLVERTEXATTRIBI4UIPROC            glVertexAttribI4ui;
  PFNGLVERTEXATTRIBI1IVPROC            glVertexAttribI1iv;
  PFNGLVERTEXATTRIBI2IVPROC            glVertexAttribI2iv;
  PFNGLVERTEXATTRIBI3IVPROC            glVertexAttribI3iv;
  PFNGLVERTEXATTRIBI4IVPROC            glVertexAttribI4iv;
  PFNGLVERTEXATTRIBI1UIVPROC           glVertexAttribI1uiv;
  PFNGLVERTEXATTRIBI2UIVPROC           glVertexAttribI2uiv;
  PFNGLVERTEXATTRIBI3UIVPROC           glVertexAttribI3uiv;
  PFNGLVERTEXATTRIBI4UIVPROC           glVertexAttribI4uiv;
  PFNGLVERTEXATTRIBI4BVPROC            glVertexAttribI4bv;
  PFNGLVERTEXATTRIBI4SVPROC            glVertexAttribI4sv;
  PFNGLVERTEXATTRIBI4UBVPROC           glVertexAttribI4ubv;
  PFNGLVERTEXATTRIBI4USVPROC           glVertexAttribI4usv;
  PFNGLGETUNIFORMUIVPROC               glGetUniformuiv;
  PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation;
  PFNGLGETFRAGDATALOCATIONPROC         glGetFragDataLocation;
  PFNGLUNIFORM1UIPROC                  glUniform1ui;
  PFNGLUNIFORM2UIPROC                  glUniform2ui;
  PFNGLUNIFORM3UIPROC                  glUniform3ui;
  PFNGLUNIFORM4UIPROC                  glUniform4ui;
  PFNGLUNIFORM1UIVPROC                 glUniform1uiv;
  PFNGLUNIFORM2UIVPROC                 glUniform2uiv;
  PFNGLUNIFORM3UIVPROC                 glUniform3uiv;
  PFNGLUNIFORM4UIVPROC                 glUniform4uiv;
  PFNGLTEXPARAMETERIIVPROC             glTexParameterIiv;
  PFNGLTEXPARAMETERIUIVPROC            glTexParameterIuiv;
  PFNGLGETTEXPARAMETERIIVPROC          glGetTexParameterIiv;
  PFNGLGETTEXPARAMETERIUIVPROC         glGetTexParameterIuiv;
  PFNGLCLEARBUFFERIVPROC               glClearBufferiv;
  PFNGLCLEARBUFFERUIVPROC              glClearBufferuiv;
  PFNGLCLEARBUFFERFVPROC               glClearBufferfv;
  PFNGLCLEARBUFFERFIPROC               glClearBufferfi;
  PFNGLGETSTRINGIPROC                  glGetStringi;

public: //! @name GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)

  PFNGLGETUNIFORMINDICESPROC           glGetUniformIndices;
  PFNGLGETACTIVEUNIFORMSIVPROC         glGetActiveUniformsiv;
  PFNGLGETACTIVEUNIFORMNAMEPROC        glGetActiveUniformName;
  PFNGLGETUNIFORMBLOCKINDEXPROC        glGetUniformBlockIndex;
  PFNGLGETACTIVEUNIFORMBLOCKIVPROC     glGetActiveUniformBlockiv;
  PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC   glGetActiveUniformBlockName;
  PFNGLUNIFORMBLOCKBINDINGPROC         glUniformBlockBinding;

public: //! @name GL_ARB_copy_buffer (added to OpenGL 3.1 core)

  PFNGLCOPYBUFFERSUBDATAPROC           glCopyBufferSubData;

public: //! @name OpenGL 3.1

  PFNGLDRAWARRAYSINSTANCEDPROC         glDrawArraysInstanced;
  PFNGLDRAWELEMENTSINSTANCEDPROC       glDrawElementsInstanced;
  PFNGLTEXBUFFERPROC                   glTexBuffer;
  PFNGLPRIMITIVERESTARTINDEXPROC       glPrimitiveRestartIndex;

public: //! @name GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)

  PFNGLDRAWELEMENTSBASEVERTEXPROC      glDrawElementsBaseVertex;
  PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
  PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;

public: //! @name GL_ARB_provoking_vertex (added to OpenGL 3.2 core)

  PFNGLPROVOKINGVERTEXPROC             glProvokingVertex;

public: //! @name GL_ARB_sync (added to OpenGL 3.2 core)

  PFNGLFENCESYNCPROC                   glFenceSync;
  PFNGLISSYNCPROC                      glIsSync;
  PFNGLDELETESYNCPROC                  glDeleteSync;
  PFNGLCLIENTWAITSYNCPROC              glClientWaitSync;
  PFNGLWAITSYNCPROC                    glWaitSync;
  PFNGLGETINTEGER64VPROC               glGetInteger64v;
  PFNGLGETSYNCIVPROC                   glGetSynciv;

public: //! @name GL_ARB_texture_multisample (added to OpenGL 3.2 core)

  PFNGLTEXIMAGE2DMULTISAMPLEPROC       glTexImage2DMultisample;
  PFNGLTEXIMAGE3DMULTISAMPLEPROC       glTexImage3DMultisample;
  PFNGLGETMULTISAMPLEFVPROC            glGetMultisamplefv;
  PFNGLSAMPLEMASKIPROC                 glSampleMaski;

public: //! @name OpenGL 3.2

  PFNGLGETINTEGER64I_VPROC             glGetInteger64i_v;
  PFNGLGETBUFFERPARAMETERI64VPROC      glGetBufferParameteri64v;
  PFNGLFRAMEBUFFERTEXTUREPROC          glFramebufferTexture;

public: //! @name GL_ARB_blend_func_extended (added to OpenGL 3.3 core)

  PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
  PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex;

public: //! @name GL_ARB_sampler_objects (added to OpenGL 3.3 core)

  PFNGLGENSAMPLERSPROC                 glGenSamplers;
  PFNGLDELETESAMPLERSPROC              glDeleteSamplers;
  PFNGLISSAMPLERPROC                   glIsSampler;
  PFNGLBINDSAMPLERPROC                 glBindSampler;
  PFNGLSAMPLERPARAMETERIPROC           glSamplerParameteri;
  PFNGLSAMPLERPARAMETERIVPROC          glSamplerParameteriv;
  PFNGLSAMPLERPARAMETERFPROC           glSamplerParameterf;
  PFNGLSAMPLERPARAMETERFVPROC          glSamplerParameterfv;
  PFNGLSAMPLERPARAMETERIIVPROC         glSamplerParameterIiv;
  PFNGLSAMPLERPARAMETERIUIVPROC        glSamplerParameterIuiv;
  PFNGLGETSAMPLERPARAMETERIVPROC       glGetSamplerParameteriv;
  PFNGLGETSAMPLERPARAMETERIIVPROC      glGetSamplerParameterIiv;
  PFNGLGETSAMPLERPARAMETERFVPROC       glGetSamplerParameterfv;
  PFNGLGETSAMPLERPARAMETERIUIVPROC     glGetSamplerParameterIuiv;

public: //! @name GL_ARB_timer_query (added to OpenGL 3.3 core)

  PFNGLQUERYCOUNTERPROC                glQueryCounter;
  PFNGLGETQUERYOBJECTI64VPROC          glGetQueryObjecti64v;
  PFNGLGETQUERYOBJECTUI64VPROC         glGetQueryObjectui64v;

public: //! @name GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)

  PFNGLVERTEXATTRIBP1UIPROC            glVertexAttribP1ui;
  PFNGLVERTEXATTRIBP1UIVPROC           glVertexAttribP1uiv;
  PFNGLVERTEXATTRIBP2UIPROC            glVertexAttribP2ui;
  PFNGLVERTEXATTRIBP2UIVPROC           glVertexAttribP2uiv;
  PFNGLVERTEXATTRIBP3UIPROC            glVertexAttribP3ui;
  PFNGLVERTEXATTRIBP3UIVPROC           glVertexAttribP3uiv;
  PFNGLVERTEXATTRIBP4UIPROC            glVertexAttribP4ui;
  PFNGLVERTEXATTRIBP4UIVPROC           glVertexAttribP4uiv;

public: //! @name OpenGL 3.3

  PFNGLVERTEXATTRIBDIVISORPROC         glVertexAttribDivisor;

public: //! @name GL_ARB_draw_indirect (added to OpenGL 4.0 core)

  PFNGLDRAWARRAYSINDIRECTPROC          glDrawArraysIndirect;
  PFNGLDRAWELEMENTSINDIRECTPROC        glDrawElementsIndirect;

public: //! @name GL_ARB_gpu_shader_fp64 (added to OpenGL 4.0 core)

  PFNGLUNIFORM1DPROC                   glUniform1d;
  PFNGLUNIFORM2DPROC                   glUniform2d;
  PFNGLUNIFORM3DPROC                   glUniform3d;
  PFNGLUNIFORM4DPROC                   glUniform4d;
  PFNGLUNIFORM1DVPROC                  glUniform1dv;
  PFNGLUNIFORM2DVPROC                  glUniform2dv;
  PFNGLUNIFORM3DVPROC                  glUniform3dv;
  PFNGLUNIFORM4DVPROC                  glUniform4dv;
  PFNGLUNIFORMMATRIX2DVPROC            glUniformMatrix2dv;
  PFNGLUNIFORMMATRIX3DVPROC            glUniformMatrix3dv;
  PFNGLUNIFORMMATRIX4DVPROC            glUniformMatrix4dv;
  PFNGLUNIFORMMATRIX2X3DVPROC          glUniformMatrix2x3dv;
  PFNGLUNIFORMMATRIX2X4DVPROC          glUniformMatrix2x4dv;
  PFNGLUNIFORMMATRIX3X2DVPROC          glUniformMatrix3x2dv;
  PFNGLUNIFORMMATRIX3X4DVPROC          glUniformMatrix3x4dv;
  PFNGLUNIFORMMATRIX4X2DVPROC          glUniformMatrix4x2dv;
  PFNGLUNIFORMMATRIX4X3DVPROC          glUniformMatrix4x3dv;
  PFNGLGETUNIFORMDVPROC                glGetUniformdv;

public: //! @name GL_ARB_shader_subroutine (added to OpenGL 4.0 core)

  PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC   glGetSubroutineUniformLocation;
  PFNGLGETSUBROUTINEINDEXPROC             glGetSubroutineIndex;
  PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC   glGetActiveSubroutineUniformiv;
  PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
  PFNGLGETACTIVESUBROUTINENAMEPROC        glGetActiveSubroutineName;
  PFNGLUNIFORMSUBROUTINESUIVPROC          glUniformSubroutinesuiv;
  PFNGLGETUNIFORMSUBROUTINEUIVPROC        glGetUniformSubroutineuiv;
  PFNGLGETPROGRAMSTAGEIVPROC              glGetProgramStageiv;

public: //! @name GL_ARB_tessellation_shader (added to OpenGL 4.0 core)

  PFNGLPATCHPARAMETERIPROC             glPatchParameteri;
  PFNGLPATCHPARAMETERFVPROC            glPatchParameterfv;

public: //! @name GL_ARB_transform_feedback2 (added to OpenGL 4.0 core)

  PFNGLBINDTRANSFORMFEEDBACKPROC       glBindTransformFeedback;
  PFNGLDELETETRANSFORMFEEDBACKSPROC    glDeleteTransformFeedbacks;
  PFNGLGENTRANSFORMFEEDBACKSPROC       glGenTransformFeedbacks;
  PFNGLISTRANSFORMFEEDBACKPROC         glIsTransformFeedback;
  PFNGLPAUSETRANSFORMFEEDBACKPROC      glPauseTransformFeedback;
  PFNGLRESUMETRANSFORMFEEDBACKPROC     glResumeTransformFeedback;
  PFNGLDRAWTRANSFORMFEEDBACKPROC       glDrawTransformFeedback;

public: //! @name GL_ARB_transform_feedback3 (added to OpenGL 4.0 core)

  PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
  PFNGLBEGINQUERYINDEXEDPROC           glBeginQueryIndexed;
  PFNGLENDQUERYINDEXEDPROC             glEndQueryIndexed;
  PFNGLGETQUERYINDEXEDIVPROC           glGetQueryIndexediv;

public: //! @name OpenGL 4.0

  PFNGLMINSAMPLESHADINGPROC            glMinSampleShading;
  PFNGLBLENDEQUATIONIPROC              glBlendEquationi;
  PFNGLBLENDEQUATIONSEPARATEIPROC      glBlendEquationSeparatei;
  PFNGLBLENDFUNCIPROC                  glBlendFunci;
  PFNGLBLENDFUNCSEPARATEIPROC          glBlendFuncSeparatei;

public: //! @name GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)

  PFNGLRELEASESHADERCOMPILERPROC       glReleaseShaderCompiler;
  PFNGLSHADERBINARYPROC                glShaderBinary;
  PFNGLGETSHADERPRECISIONFORMATPROC    glGetShaderPrecisionFormat;
  PFNGLDEPTHRANGEFPROC                 glDepthRangef;
  PFNGLCLEARDEPTHFPROC                 glClearDepthf;

public: //! @name GL_ARB_get_program_binary (added to OpenGL 4.1 core)

  PFNGLGETPROGRAMBINARYPROC            glGetProgramBinary;
  PFNGLPROGRAMBINARYPROC               glProgramBinary;
  PFNGLPROGRAMPARAMETERIPROC           glProgramParameteri;

public: //! @name GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)

  PFNGLUSEPROGRAMSTAGESPROC            glUseProgramStages;
  PFNGLACTIVESHADERPROGRAMPROC         glActiveShaderProgram;
  PFNGLCREATESHADERPROGRAMVPROC        glCreateShaderProgramv;
  PFNGLBINDPROGRAMPIPELINEPROC         glBindProgramPipeline;
  PFNGLDELETEPROGRAMPIPELINESPROC      glDeleteProgramPipelines;
  PFNGLGENPROGRAMPIPELINESPROC         glGenProgramPipelines;
  PFNGLISPROGRAMPIPELINEPROC           glIsProgramPipeline;
  PFNGLGETPROGRAMPIPELINEIVPROC        glGetProgramPipelineiv;
  PFNGLPROGRAMUNIFORM1IPROC            glProgramUniform1i;
  PFNGLPROGRAMUNIFORM1IVPROC           glProgramUniform1iv;
  PFNGLPROGRAMUNIFORM1FPROC            glProgramUniform1f;
  PFNGLPROGRAMUNIFORM1FVPROC           glProgramUniform1fv;
  PFNGLPROGRAMUNIFORM1DPROC            glProgramUniform1d;
  PFNGLPROGRAMUNIFORM1DVPROC           glProgramUniform1dv;
  PFNGLPROGRAMUNIFORM1UIPROC           glProgramUniform1ui;
  PFNGLPROGRAMUNIFORM1UIVPROC          glProgramUniform1uiv;
  PFNGLPROGRAMUNIFORM2IPROC            glProgramUniform2i;
  PFNGLPROGRAMUNIFORM2IVPROC           glProgramUniform2iv;
  PFNGLPROGRAMUNIFORM2FPROC            glProgramUniform2f;
  PFNGLPROGRAMUNIFORM2FVPROC           glProgramUniform2fv;
  PFNGLPROGRAMUNIFORM2DPROC            glProgramUniform2d;
  PFNGLPROGRAMUNIFORM2DVPROC           glProgramUniform2dv;
  PFNGLPROGRAMUNIFORM2UIPROC           glProgramUniform2ui;
  PFNGLPROGRAMUNIFORM2UIVPROC          glProgramUniform2uiv;
  PFNGLPROGRAMUNIFORM3IPROC            glProgramUniform3i;
  PFNGLPROGRAMUNIFORM3IVPROC           glProgramUniform3iv;
  PFNGLPROGRAMUNIFORM3FPROC            glProgramUniform3f;
  PFNGLPROGRAMUNIFORM3FVPROC           glProgramUniform3fv;
  PFNGLPROGRAMUNIFORM3DPROC            glProgramUniform3d;
  PFNGLPROGRAMUNIFORM3DVPROC           glProgramUniform3dv;
  PFNGLPROGRAMUNIFORM3UIPROC           glProgramUniform3ui;
  PFNGLPROGRAMUNIFORM3UIVPROC          glProgramUniform3uiv;
  PFNGLPROGRAMUNIFORM4IPROC            glProgramUniform4i;
  PFNGLPROGRAMUNIFORM4IVPROC           glProgramUniform4iv;
  PFNGLPROGRAMUNIFORM4FPROC            glProgramUniform4f;
  PFNGLPROGRAMUNIFORM4FVPROC           glProgramUniform4fv;
  PFNGLPROGRAMUNIFORM4DPROC            glProgramUniform4d;
  PFNGLPROGRAMUNIFORM4DVPROC           glProgramUniform4dv;
  PFNGLPROGRAMUNIFORM4UIPROC           glProgramUniform4ui;
  PFNGLPROGRAMUNIFORM4UIVPROC          glProgramUniform4uiv;
  PFNGLPROGRAMUNIFORMMATRIX2FVPROC     glProgramUniformMatrix2fv;
  PFNGLPROGRAMUNIFORMMATRIX3FVPROC     glProgramUniformMatrix3fv;
  PFNGLPROGRAMUNIFORMMATRIX4FVPROC     glProgramUniformMatrix4fv;
  PFNGLPROGRAMUNIFORMMATRIX2DVPROC     glProgramUniformMatrix2dv;
  PFNGLPROGRAMUNIFORMMATRIX3DVPROC     glProgramUniformMatrix3dv;
  PFNGLPROGRAMUNIFORMMATRIX4DVPROC     glProgramUniformMatrix4dv;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC   glProgramUniformMatrix2x3fv;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC   glProgramUniformMatrix3x2fv;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC   glProgramUniformMatrix2x4fv;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC   glProgramUniformMatrix4x2fv;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC   glProgramUniformMatrix3x4fv;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC   glProgramUniformMatrix4x3fv;
  PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC   glProgramUniformMatrix2x3dv;
  PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC   glProgramUniformMatrix3x2dv;
  PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC   glProgramUniformMatrix2x4dv;
  PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC   glProgramUniformMatrix4x2dv;
  PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC   glProgramUniformMatrix3x4dv;
  PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC   glProgramUniformMatrix4x3dv;
  PFNGLVALIDATEPROGRAMPIPELINEPROC     glValidateProgramPipeline;
  PFNGLGETPROGRAMPIPELINEINFOLOGPROC   glGetProgramPipelineInfoLog;

public: //! @name GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)

  PFNGLVERTEXATTRIBL1DPROC             glVertexAttribL1d;
  PFNGLVERTEXATTRIBL2DPROC             glVertexAttribL2d;
  PFNGLVERTEXATTRIBL3DPROC             glVertexAttribL3d;
  PFNGLVERTEXATTRIBL4DPROC             glVertexAttribL4d;
  PFNGLVERTEXATTRIBL1DVPROC            glVertexAttribL1dv;
  PFNGLVERTEXATTRIBL2DVPROC            glVertexAttribL2dv;
  PFNGLVERTEXATTRIBL3DVPROC            glVertexAttribL3dv;
  PFNGLVERTEXATTRIBL4DVPROC            glVertexAttribL4dv;
  PFNGLVERTEXATTRIBLPOINTERPROC        glVertexAttribLPointer;
  PFNGLGETVERTEXATTRIBLDVPROC          glGetVertexAttribLdv;

public: //! @name GL_ARB_viewport_array (added to OpenGL 4.1 core)

  PFNGLVIEWPORTARRAYVPROC              glViewportArrayv;
  PFNGLVIEWPORTINDEXEDFPROC            glViewportIndexedf;
  PFNGLVIEWPORTINDEXEDFVPROC           glViewportIndexedfv;
  PFNGLSCISSORARRAYVPROC               glScissorArrayv;
  PFNGLSCISSORINDEXEDPROC              glScissorIndexed;
  PFNGLSCISSORINDEXEDVPROC             glScissorIndexedv;
  PFNGLDEPTHRANGEARRAYVPROC            glDepthRangeArrayv;
  PFNGLDEPTHRANGEINDEXEDPROC           glDepthRangeIndexed;
  PFNGLGETFLOATI_VPROC                 glGetFloati_v;
  PFNGLGETDOUBLEI_VPROC                glGetDoublei_v;

public: //! @name OpenGL 4.1

  //

public: //! @name GL_ARB_base_instance (added to OpenGL 4.2 core)

  PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             glDrawArraysInstancedBaseInstance;
  PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           glDrawElementsInstancedBaseInstance;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;

public: //! @name GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)

  PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC              glDrawTransformFeedbackInstanced;
  PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC        glDrawTransformFeedbackStreamInstanced;

public: //! @name GL_ARB_internalformat_query (added to OpenGL 4.2 core)

  PFNGLGETINTERNALFORMATIVPROC                         glGetInternalformativ;

public: //! @name GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)

  PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC              glGetActiveAtomicCounterBufferiv;

public: //! @name GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)

  PFNGLBINDIMAGETEXTUREPROC                            glBindImageTexture;
  PFNGLMEMORYBARRIERPROC                               glMemoryBarrier;

public: //! @name GL_ARB_texture_storage (added to OpenGL 4.2 core)

  PFNGLTEXSTORAGE1DPROC                                glTexStorage1D;
  PFNGLTEXSTORAGE2DPROC                                glTexStorage2D;
  PFNGLTEXSTORAGE3DPROC                                glTexStorage3D;

public: //! @name OpenGL 4.2

public: //! @name OpenGL 4.3

  PFNGLCLEARBUFFERDATAPROC                 glClearBufferData;
  PFNGLCLEARBUFFERSUBDATAPROC              glClearBufferSubData;
  PFNGLDISPATCHCOMPUTEPROC                 glDispatchCompute;
  PFNGLDISPATCHCOMPUTEINDIRECTPROC         glDispatchComputeIndirect;
  PFNGLCOPYIMAGESUBDATAPROC                glCopyImageSubData;
  PFNGLFRAMEBUFFERPARAMETERIPROC           glFramebufferParameteri;
  PFNGLGETFRAMEBUFFERPARAMETERIVPROC       glGetFramebufferParameteriv;
  PFNGLGETINTERNALFORMATI64VPROC           glGetInternalformati64v;
  PFNGLINVALIDATETEXSUBIMAGEPROC           glInvalidateTexSubImage;
  PFNGLINVALIDATETEXIMAGEPROC              glInvalidateTexImage;
  PFNGLINVALIDATEBUFFERSUBDATAPROC         glInvalidateBufferSubData;
  PFNGLINVALIDATEBUFFERDATAPROC            glInvalidateBufferData;
  PFNGLINVALIDATEFRAMEBUFFERPROC           glInvalidateFramebuffer;
  PFNGLINVALIDATESUBFRAMEBUFFERPROC        glInvalidateSubFramebuffer;
  PFNGLMULTIDRAWARRAYSINDIRECTPROC         glMultiDrawArraysIndirect;
  PFNGLMULTIDRAWELEMENTSINDIRECTPROC       glMultiDrawElementsIndirect;
  PFNGLGETPROGRAMINTERFACEIVPROC           glGetProgramInterfaceiv;
  PFNGLGETPROGRAMRESOURCEINDEXPROC         glGetProgramResourceIndex;
  PFNGLGETPROGRAMRESOURCENAMEPROC          glGetProgramResourceName;
  PFNGLGETPROGRAMRESOURCEIVPROC            glGetProgramResourceiv;
  PFNGLGETPROGRAMRESOURCELOCATIONPROC      glGetProgramResourceLocation;
  PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex;
  PFNGLSHADERSTORAGEBLOCKBINDINGPROC       glShaderStorageBlockBinding;
  PFNGLTEXBUFFERRANGEPROC                  glTexBufferRange;
  PFNGLTEXSTORAGE2DMULTISAMPLEPROC         glTexStorage2DMultisample;
  PFNGLTEXSTORAGE3DMULTISAMPLEPROC         glTexStorage3DMultisample;
  PFNGLTEXTUREVIEWPROC                     glTextureView;
  PFNGLBINDVERTEXBUFFERPROC                glBindVertexBuffer;
  PFNGLVERTEXATTRIBFORMATPROC              glVertexAttribFormat;
  PFNGLVERTEXATTRIBIFORMATPROC             glVertexAttribIFormat;
  PFNGLVERTEXATTRIBLFORMATPROC             glVertexAttribLFormat;
  PFNGLVERTEXATTRIBBINDINGPROC             glVertexAttribBinding;
  PFNGLVERTEXBINDINGDIVISORPROC            glVertexBindingDivisor;
  PFNGLDEBUGMESSAGECONTROLPROC             glDebugMessageControl;
  PFNGLDEBUGMESSAGEINSERTPROC              glDebugMessageInsert;
  PFNGLDEBUGMESSAGECALLBACKPROC            glDebugMessageCallback;
  PFNGLGETDEBUGMESSAGELOGPROC              glGetDebugMessageLog;
  PFNGLPUSHDEBUGGROUPPROC                  glPushDebugGroup;
  PFNGLPOPDEBUGGROUPPROC                   glPopDebugGroup;
  PFNGLOBJECTLABELPROC                     glObjectLabel;
  PFNGLGETOBJECTLABELPROC                  glGetObjectLabel;
  PFNGLOBJECTPTRLABELPROC                  glObjectPtrLabel;
  PFNGLGETOBJECTPTRLABELPROC               glGetObjectPtrLabel;

public: //! @name OpenGL 4.4

  PFNGLBUFFERSTORAGEPROC     glBufferStorage;
  PFNGLCLEARTEXIMAGEPROC     glClearTexImage;
  PFNGLCLEARTEXSUBIMAGEPROC  glClearTexSubImage;
  PFNGLBINDBUFFERSBASEPROC   glBindBuffersBase;
  PFNGLBINDBUFFERSRANGEPROC  glBindBuffersRange;
  PFNGLBINDTEXTURESPROC      glBindTextures;
  PFNGLBINDSAMPLERSPROC      glBindSamplers;
  PFNGLBINDIMAGETEXTURESPROC glBindImageTextures;
  PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers;

public: //! @name OpenGL 4.5

  PFNGLCLIPCONTROLPROC glClipControl;
  PFNGLCREATETRANSFORMFEEDBACKSPROC glCreateTransformFeedbacks;
  PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glTransformFeedbackBufferBase;
  PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glTransformFeedbackBufferRange;
  PFNGLGETTRANSFORMFEEDBACKIVPROC glGetTransformFeedbackiv;
  PFNGLGETTRANSFORMFEEDBACKI_VPROC glGetTransformFeedbacki_v;
  PFNGLGETTRANSFORMFEEDBACKI64_VPROC glGetTransformFeedbacki64_v;
  PFNGLCREATEBUFFERSPROC glCreateBuffers;
  PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
  PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
  PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
  PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
  PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData;
  PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData;
  PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer;
  PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
  PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
  PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange;
  PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv;
  PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v;
  PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv;
  PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData;
  PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
  PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer;
  PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri;
  PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture;
  PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer;
  PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer;
  PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers;
  PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer;
  PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData;
  PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData;
  PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv;
  PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv;
  PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv;
  PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi;
  PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer;
  PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
  PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv;
  PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv;
  PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers;
  PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage;
  PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glNamedRenderbufferStorageMultisample;
  PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glGetNamedRenderbufferParameteriv;
  PFNGLCREATETEXTURESPROC glCreateTextures;
  PFNGLTEXTUREBUFFERPROC glTextureBuffer;
  PFNGLTEXTUREBUFFERRANGEPROC glTextureBufferRange;
  PFNGLTEXTURESTORAGE1DPROC glTextureStorage1D;
  PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
  PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D;
  PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample;
  PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample;
  PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D;
  PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D;
  PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glCompressedTextureSubImage1D;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glCompressedTextureSubImage3D;
  PFNGLCOPYTEXTURESUBIMAGE1DPROC glCopyTextureSubImage1D;
  PFNGLCOPYTEXTURESUBIMAGE2DPROC glCopyTextureSubImage2D;
  PFNGLCOPYTEXTURESUBIMAGE3DPROC glCopyTextureSubImage3D;
  PFNGLTEXTUREPARAMETERFPROC glTextureParameterf;
  PFNGLTEXTUREPARAMETERFVPROC glTextureParameterfv;
  PFNGLTEXTUREPARAMETERIPROC glTextureParameteri;
  PFNGLTEXTUREPARAMETERIIVPROC glTextureParameterIiv;
  PFNGLTEXTUREPARAMETERIUIVPROC glTextureParameterIuiv;
  PFNGLTEXTUREPARAMETERIVPROC glTextureParameteriv;
  PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap;
  PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
  PFNGLGETTEXTUREIMAGEPROC glGetTextureImage;
  PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glGetCompressedTextureImage;
  PFNGLGETTEXTURELEVELPARAMETERFVPROC glGetTextureLevelParameterfv;
  PFNGLGETTEXTURELEVELPARAMETERIVPROC glGetTextureLevelParameteriv;
  PFNGLGETTEXTUREPARAMETERFVPROC glGetTextureParameterfv;
  PFNGLGETTEXTUREPARAMETERIIVPROC glGetTextureParameterIiv;
  PFNGLGETTEXTUREPARAMETERIUIVPROC glGetTextureParameterIuiv;
  PFNGLGETTEXTUREPARAMETERIVPROC glGetTextureParameteriv;
  PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
  PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib;
  PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib;
  PFNGLVERTEXARRAYELEMENTBUFFERPROC glVertexArrayElementBuffer;
  PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
  PFNGLVERTEXARRAYVERTEXBUFFERSPROC glVertexArrayVertexBuffers;
  PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
  PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat;
  PFNGLVERTEXARRAYATTRIBIFORMATPROC glVertexArrayAttribIFormat;
  PFNGLVERTEXARRAYATTRIBLFORMATPROC glVertexArrayAttribLFormat;
  PFNGLVERTEXARRAYBINDINGDIVISORPROC glVertexArrayBindingDivisor;
  PFNGLGETVERTEXARRAYIVPROC glGetVertexArrayiv;
  PFNGLGETVERTEXARRAYINDEXEDIVPROC glGetVertexArrayIndexediv;
  PFNGLGETVERTEXARRAYINDEXED64IVPROC glGetVertexArrayIndexed64iv;
  PFNGLCREATESAMPLERSPROC glCreateSamplers;
  PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines;
  PFNGLCREATEQUERIESPROC glCreateQueries;
  PFNGLGETQUERYBUFFEROBJECTI64VPROC glGetQueryBufferObjecti64v;
  PFNGLGETQUERYBUFFEROBJECTIVPROC glGetQueryBufferObjectiv;
  PFNGLGETQUERYBUFFEROBJECTUI64VPROC glGetQueryBufferObjectui64v;
  PFNGLGETQUERYBUFFEROBJECTUIVPROC glGetQueryBufferObjectuiv;
  PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion;
  PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
  PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glGetCompressedTextureSubImage;
  PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus;
  PFNGLGETNCOMPRESSEDTEXIMAGEPROC glGetnCompressedTexImage;
  PFNGLGETNTEXIMAGEPROC glGetnTexImage;
  PFNGLGETNUNIFORMDVPROC glGetnUniformdv;
  PFNGLGETNUNIFORMFVPROC glGetnUniformfv;
  PFNGLGETNUNIFORMIVPROC glGetnUniformiv;
  PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv;
  PFNGLREADNPIXELSPROC glReadnPixels;
  PFNGLTEXTUREBARRIERPROC glTextureBarrier;

public: //! @name OpenGL 4.6

  PFNGLSPECIALIZESHADERPROC glSpecializeShader;
  PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC glMultiDrawArraysIndirectCount;
  PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC glMultiDrawElementsIndirectCount;
  PFNGLPOLYGONOFFSETCLAMPPROC glPolygonOffsetClamp;

public: //! @name GL_EXT_geometry_shader4

  PFNGLPROGRAMPARAMETERIEXTPROC glProgramParameteriEXT;

public: //! @name GL_ARB_bindless_texture

  PFNGLGETTEXTUREHANDLEARBPROC             glGetTextureHandleARB;
  PFNGLGETTEXTURESAMPLERHANDLEARBPROC      glGetTextureSamplerHandleARB;
  PFNGLMAKETEXTUREHANDLERESIDENTARBPROC    glMakeTextureHandleResidentARB;
  PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC glMakeTextureHandleNonResidentARB;
  PFNGLGETIMAGEHANDLEARBPROC               glGetImageHandleARB;
  PFNGLMAKEIMAGEHANDLERESIDENTARBPROC      glMakeImageHandleResidentARB;
  PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC   glMakeImageHandleNonResidentARB;
  PFNGLUNIFORMHANDLEUI64ARBPROC            glUniformHandleui64ARB;
  PFNGLUNIFORMHANDLEUI64VARBPROC           glUniformHandleui64vARB;
  PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC     glProgramUniformHandleui64ARB;
  PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC    glProgramUniformHandleui64vARB;
  PFNGLISTEXTUREHANDLERESIDENTARBPROC      glIsTextureHandleResidentARB;
  PFNGLISIMAGEHANDLERESIDENTARBPROC        glIsImageHandleResidentARB;
  PFNGLVERTEXATTRIBL1UI64ARBPROC           glVertexAttribL1ui64ARB;
  PFNGLVERTEXATTRIBL1UI64VARBPROC          glVertexAttribL1ui64vARB;
  PFNGLGETVERTEXATTRIBLUI64VARBPROC        glGetVertexAttribLui64vARB;

#if defined(_WIN32)
public: //! @name wgl extensions

  typedef const char* (WINAPI *wglGetExtensionsStringARB_t)(HDC theDeviceContext);
  wglGetExtensionsStringARB_t wglGetExtensionsStringARB;

  typedef BOOL        (WINAPI *wglSwapIntervalEXT_t)(int theInterval);
  wglSwapIntervalEXT_t wglSwapIntervalEXT;

  // WGL_ARB_pixel_format

#ifndef WGL_NUMBER_PIXEL_FORMATS_ARB
  #define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
  #define WGL_DRAW_TO_WINDOW_ARB                  0x2001
  #define WGL_DRAW_TO_BITMAP_ARB                  0x2002
  #define WGL_ACCELERATION_ARB                    0x2003
  #define WGL_NEED_PALETTE_ARB                    0x2004
  #define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
  #define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
  #define WGL_SWAP_METHOD_ARB                     0x2007
  #define WGL_NUMBER_OVERLAYS_ARB                 0x2008
  #define WGL_NUMBER_UNDERLAYS_ARB                0x2009
  #define WGL_TRANSPARENT_ARB                     0x200A
  #define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
  #define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
  #define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
  #define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
  #define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
  #define WGL_SHARE_DEPTH_ARB                     0x200C
  #define WGL_SHARE_STENCIL_ARB                   0x200D
  #define WGL_SHARE_ACCUM_ARB                     0x200E
  #define WGL_SUPPORT_GDI_ARB                     0x200F
  #define WGL_SUPPORT_OPENGL_ARB                  0x2010
  #define WGL_DOUBLE_BUFFER_ARB                   0x2011
  #define WGL_STEREO_ARB                          0x2012
  #define WGL_PIXEL_TYPE_ARB                      0x2013
  #define WGL_COLOR_BITS_ARB                      0x2014
  #define WGL_RED_BITS_ARB                        0x2015
  #define WGL_RED_SHIFT_ARB                       0x2016
  #define WGL_GREEN_BITS_ARB                      0x2017
  #define WGL_GREEN_SHIFT_ARB                     0x2018
  #define WGL_BLUE_BITS_ARB                       0x2019
  #define WGL_BLUE_SHIFT_ARB                      0x201A
  #define WGL_ALPHA_BITS_ARB                      0x201B
  #define WGL_ALPHA_SHIFT_ARB                     0x201C
  #define WGL_ACCUM_BITS_ARB                      0x201D
  #define WGL_ACCUM_RED_BITS_ARB                  0x201E
  #define WGL_ACCUM_GREEN_BITS_ARB                0x201F
  #define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
  #define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
  #define WGL_DEPTH_BITS_ARB                      0x2022
  #define WGL_STENCIL_BITS_ARB                    0x2023
  #define WGL_AUX_BUFFERS_ARB                     0x2024

  #define WGL_NO_ACCELERATION_ARB                 0x2025
  #define WGL_GENERIC_ACCELERATION_ARB            0x2026
  #define WGL_FULL_ACCELERATION_ARB               0x2027

  #define WGL_SWAP_EXCHANGE_ARB                   0x2028
  #define WGL_SWAP_COPY_ARB                       0x2029
  #define WGL_SWAP_UNDEFINED_ARB                  0x202A

  #define WGL_TYPE_RGBA_ARB                       0x202B
  #define WGL_TYPE_COLORINDEX_ARB                 0x202C

#endif // WGL_NUMBER_PIXEL_FORMATS_ARB

      // WGL_ARB_multisample
#ifndef WGL_SAMPLE_BUFFERS_ARB
  #define WGL_SAMPLE_BUFFERS_ARB               0x2041
  #define WGL_SAMPLES_ARB                      0x2042
#endif

      // WGL_ARB_create_context_robustness
#ifndef WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
  #define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
  #define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
  #define WGL_NO_RESET_NOTIFICATION_ARB               0x8261
  #define WGL_LOSE_CONTEXT_ON_RESET_ARB               0x8252
#endif

  typedef BOOL (WINAPI *wglChoosePixelFormatARB_t)
      (HDC           theDevCtx,
       const int*    theIntAttribs,
       const float*  theFloatAttribs,
       unsigned int  theMaxFormats,
       int*          theFormatsOut,
       unsigned int* theNumFormatsOut);
  wglChoosePixelFormatARB_t wglChoosePixelFormatARB;

  // WGL_ARB_create_context_profile

#ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
  #define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
  #define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
  #define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
  #define WGL_CONTEXT_FLAGS_ARB                   0x2094
  #define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

  // WGL_CONTEXT_FLAGS bits
  #define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
  #define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

  // WGL_CONTEXT_PROFILE_MASK_ARB bits
  #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
  #define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif // WGL_CONTEXT_MAJOR_VERSION_ARB

  typedef HGLRC (WINAPI *wglCreateContextAttribsARB_t)(HDC        theDevCtx,
                                                       HGLRC      theShareContext,
                                                       const int* theAttribs);
  wglCreateContextAttribsARB_t wglCreateContextAttribsARB;

  // WGL_NV_DX_interop

  typedef BOOL   (WINAPI *wglDXSetResourceShareHandleNV_t)(void* theObjectD3d, HANDLE theShareHandle);
  typedef HANDLE (WINAPI *wglDXOpenDeviceNV_t      )(void*   theDeviceD3d);
  typedef BOOL   (WINAPI *wglDXCloseDeviceNV_t     )(HANDLE  theDeviceIOP);
  typedef HANDLE (WINAPI *wglDXRegisterObjectNV_t  )(HANDLE  theDeviceIOP,
                                                     void*   theObjectD3d,
                                                     GLuint  theName,
                                                     GLenum  theType,
                                                     GLenum  theAccess);
  typedef BOOL   (WINAPI *wglDXUnregisterObjectNV_t)(HANDLE  theDeviceIOP,
                                                     HANDLE  theObject);
  typedef BOOL   (WINAPI *wglDXObjectAccessNV_t    )(HANDLE  theObject,
                                                     GLenum  theAccess);
  typedef BOOL   (WINAPI *wglDXLockObjectsNV_t     )(HANDLE  theDeviceIOP,
                                                     GLint   theCount,
                                                     HANDLE* theObjects);
  typedef BOOL   (WINAPI *wglDXUnlockObjectsNV_t   )(HANDLE  theDeviceIOP,
                                                     GLint   theCount,
                                                     HANDLE* theObjects);

  wglDXSetResourceShareHandleNV_t wglDXSetResourceShareHandleNV;
  wglDXOpenDeviceNV_t       wglDXOpenDeviceNV;
  wglDXCloseDeviceNV_t      wglDXCloseDeviceNV;
  wglDXRegisterObjectNV_t   wglDXRegisterObjectNV;
  wglDXUnregisterObjectNV_t wglDXUnregisterObjectNV;
  wglDXObjectAccessNV_t     wglDXObjectAccessNV;
  wglDXLockObjectsNV_t      wglDXLockObjectsNV;
  wglDXUnlockObjectsNV_t    wglDXUnlockObjectsNV;

#ifndef WGL_ACCESS_READ_WRITE_NV
  #define WGL_ACCESS_READ_ONLY_NV     0x0000
  #define WGL_ACCESS_READ_WRITE_NV    0x0001
  #define WGL_ACCESS_WRITE_DISCARD_NV 0x0002
#endif

  // WGL_AMD_gpu_association

#ifndef WGL_GPU_VENDOR_AMD
  #define WGL_GPU_VENDOR_AMD                 0x1F00
  #define WGL_GPU_RENDERER_STRING_AMD        0x1F01
  #define WGL_GPU_OPENGL_VERSION_STRING_AMD  0x1F02
  #define WGL_GPU_FASTEST_TARGET_GPUS_AMD    0x21A2
  #define WGL_GPU_RAM_AMD                    0x21A3
  #define WGL_GPU_CLOCK_AMD                  0x21A4
  #define WGL_GPU_NUM_PIPES_AMD              0x21A5
  #define WGL_GPU_NUM_SIMD_AMD               0x21A6
  #define WGL_GPU_NUM_RB_AMD                 0x21A7
  #define WGL_GPU_NUM_SPI_AMD                0x21A8
#endif

  typedef UINT (WINAPI *wglGetGPUIDsAMD_t       )(UINT theMaxCount, UINT* theIds);
  typedef INT  (WINAPI *wglGetGPUInfoAMD_t      )(UINT theId, INT theProperty, GLenum theDataType, UINT theSize, void* theData);
  typedef UINT (WINAPI *wglGetContextGPUIDAMD_t )(HGLRC theHglrc);
  wglGetGPUIDsAMD_t       wglGetGPUIDsAMD;
  wglGetGPUInfoAMD_t      wglGetGPUInfoAMD;
  wglGetContextGPUIDAMD_t wglGetContextGPUIDAMD;

#elif defined(__APPLE__)
public: //! @name CGL extensions

#else
public: //! @name glX extensions

  // GLX_EXT_swap_control
  //typedef int         (*glXSwapIntervalEXT_t)(Display* theDisplay, GLXDrawable theDrawable, int theInterval);
  typedef int         (*glXSwapIntervalEXT_t)();
  glXSwapIntervalEXT_t glXSwapIntervalEXT;

  typedef int         (*glXSwapIntervalSGI_t)(int theInterval);
  glXSwapIntervalSGI_t glXSwapIntervalSGI;

  // GLX_MESA_query_renderer
#ifndef GLX_RENDERER_VENDOR_ID_MESA
  // for glXQueryRendererIntegerMESA() and glXQueryCurrentRendererIntegerMESA()
  #define GLX_RENDERER_VENDOR_ID_MESA                      0x8183
  #define GLX_RENDERER_DEVICE_ID_MESA                      0x8184
  #define GLX_RENDERER_VERSION_MESA                        0x8185
  #define GLX_RENDERER_ACCELERATED_MESA                    0x8186
  #define GLX_RENDERER_VIDEO_MEMORY_MESA                   0x8187
  #define GLX_RENDERER_UNIFIED_MEMORY_ARCHITECTURE_MESA    0x8188
  #define GLX_RENDERER_PREFERRED_PROFILE_MESA              0x8189
  #define GLX_RENDERER_OPENGL_CORE_PROFILE_VERSION_MESA    0x818A
  #define GLX_RENDERER_OPENGL_COMPATIBILITY_PROFILE_VERSION_MESA 0x818B
  #define GLX_RENDERER_OPENGL_ES_PROFILE_VERSION_MESA      0x818C
  #define GLX_RENDERER_OPENGL_ES2_PROFILE_VERSION_MESA     0x818D

  #define GLX_RENDERER_ID_MESA                             0x818E
#endif // GLX_RENDERER_VENDOR_ID_MESA

  typedef int (*glXQueryRendererIntegerMESA_t)(Aspect_XDisplay* theDisplay, int theScreen,
                                               int theRenderer, int theAttribute,
                                               unsigned int* theValue);
  typedef int (*glXQueryCurrentRendererIntegerMESA_t)(int theAttribute, unsigned int* theValue);
  typedef const char* (*glXQueryRendererStringMESA_t)(Aspect_XDisplay* theDisplay, int theScreen,
                                                      int theRenderer, int theAttribute);
  typedef const char* (*glXQueryCurrentRendererStringMESA_t)(int theAttribute);

  glXQueryRendererIntegerMESA_t        glXQueryRendererIntegerMESA;
  glXQueryCurrentRendererIntegerMESA_t glXQueryCurrentRendererIntegerMESA;
  glXQueryRendererStringMESA_t         glXQueryRendererStringMESA;
  glXQueryCurrentRendererStringMESA_t  glXQueryCurrentRendererStringMESA;
#endif

#endif // OpenGL ES vs. desktop

};

#endif // _OpenGl_GlFunctions_Header
