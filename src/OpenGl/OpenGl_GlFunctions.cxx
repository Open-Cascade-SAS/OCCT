// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <OpenGl_GlNative.hxx>

#include <OpenGl_Context.hxx>

#include <OpenGl_ArbTBO.hxx>
#include <OpenGl_ArbIns.hxx>
#include <OpenGl_ArbDbg.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_ExtGS.hxx>
#include <OpenGl_ArbSamplerObject.hxx>
#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_GlCore46.hxx>

#if !defined(HAVE_EGL) && defined(HAVE_XLIB)
  #include <GL/glx.h>
#endif

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
  #include <emscripten/html5.h>
#endif

// This debug macros can be enabled to help debugging OpenGL implementations
// without solid / working debugging capabilities.
// #define TO_TRACE

#define WrapProxyProc(theFunc) this->theFunc = opencascade::theFunc
#define WrapProxyProc5(theFunc1, theFunc2, theFunc3, theFunc4, theFunc5)                           \
  WrapProxyProc(theFunc1);                                                                         \
  WrapProxyProc(theFunc2);                                                                         \
  WrapProxyProc(theFunc3);                                                                         \
  WrapProxyProc(theFunc4);                                                                         \
  WrapProxyProc(theFunc5)

#ifdef TO_TRACE
  #define OpenGl_TRACE(theName)                                                                    \
    {                                                                                              \
      OpenGl_GlFunctions::debugPrintError(#theName);                                               \
    }
  #define WrapProxyDef(theFunc) this->theFunc = opencascade::theFunc
#else
  #define OpenGl_TRACE(theName)
  // skip wrapper and set pointer to global function
  #define WrapProxyDef(theFunc)                                                                    \
    this->theFunc = opencascade::theFunc;                                                          \
    this->theFunc = ::theFunc;
#endif

#define WrapProxyDef5(theFunc1, theFunc2, theFunc3, theFunc4, theFunc5)                            \
  WrapProxyDef(theFunc1);                                                                          \
  WrapProxyDef(theFunc2);                                                                          \
  WrapProxyDef(theFunc3);                                                                          \
  WrapProxyDef(theFunc4);                                                                          \
  WrapProxyDef(theFunc5)

namespace opencascade
{
static void APIENTRY glReadBuffer(GLenum mode)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF); // added to OpenGL ES 3.0
  (void)mode;
#else
  ::glReadBuffer(mode);
#endif
  OpenGl_TRACE(glReadBuffer)
}

static void APIENTRY glGetTexLevelParameteriv(GLenum target,
                                              GLint  level,
                                              GLenum pname,
                                              GLint* params)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF); // added to OpenGL ES 3.1
  (void)target;
  (void)level;
  (void)pname;
  (void)params;
#else
  ::glGetTexLevelParameteriv(target, level, pname, params);
#endif
  OpenGl_TRACE(glGetTexLevelParameteriv)
}

static void APIENTRY glGetTexLevelParameterfv(GLenum   target,
                                              GLint    level,
                                              GLenum   pname,
                                              GLfloat* params)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF); // added to OpenGL ES 3.1
  (void)target;
  (void)level;
  (void)pname;
  (void)params;
#else
  ::glGetTexLevelParameterfv(target, level, pname, params);
#endif
  OpenGl_TRACE(glGetTexLevelParameterfv)
}

static void APIENTRY glGetPointerv(GLenum name, GLvoid** params)
{
#if defined(GL_ES_VERSION_2_0)
  *params = NULL;
  ::glEnable(0xFFFF); // added to OpenGL ES 3.2
  (void)name;
#else
  ::glGetPointerv(name, params);
#endif
  OpenGl_TRACE(glGetPointerv)
}

static void APIENTRY glDrawBuffer(GLenum mode)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)mode;
#else
  ::glDrawBuffer(mode);
#endif
  OpenGl_TRACE(glDrawBuffer)
}

// Miscellaneous

static void APIENTRY glClearColor(GLclampf theRed,
                                  GLclampf theGreen,
                                  GLclampf theBlue,
                                  GLclampf theAlpha)
{
  ::glClearColor(theRed, theGreen, theBlue, theAlpha);
  OpenGl_TRACE(glClearColor)
}

static void APIENTRY glClear(GLbitfield theMask)
{
  ::glClear(theMask);
  OpenGl_TRACE(glClear)
}

static void APIENTRY glColorMask(GLboolean theRed,
                                 GLboolean theGreen,
                                 GLboolean theBlue,
                                 GLboolean theAlpha)
{
  ::glColorMask(theRed, theGreen, theBlue, theAlpha);
  OpenGl_TRACE(glColorMask)
}

static void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
  ::glBlendFunc(sfactor, dfactor);
  OpenGl_TRACE(glBlendFunc)
}

static void APIENTRY glCullFace(GLenum theMode)
{
  ::glCullFace(theMode);
  OpenGl_TRACE(glCullFace)
}

static void APIENTRY glFrontFace(GLenum theMode)
{
  ::glFrontFace(theMode);
  OpenGl_TRACE(glFrontFace)
}

static void APIENTRY glLineWidth(GLfloat theWidth)
{
  ::glLineWidth(theWidth);
  OpenGl_TRACE(glLineWidth)
}

static void APIENTRY glPolygonOffset(GLfloat theFactor, GLfloat theUnits)
{
  ::glPolygonOffset(theFactor, theUnits);
  OpenGl_TRACE(glPolygonOffset)
}

static void APIENTRY glScissor(GLint theX, GLint theY, GLsizei theWidth, GLsizei theHeight)
{
  ::glScissor(theX, theY, theWidth, theHeight);
  OpenGl_TRACE(glScissor)
}

static void APIENTRY glEnable(GLenum theCap)
{
  ::glEnable(theCap);
  OpenGl_TRACE(glEnable)
}

static void APIENTRY glDisable(GLenum theCap)
{
  ::glDisable(theCap);
  OpenGl_TRACE(glDisable)
}

static GLboolean APIENTRY glIsEnabled(GLenum theCap)
{
  return ::glIsEnabled(theCap);
}

static void APIENTRY glGetBooleanv(GLenum theParamName, GLboolean* theValues)
{
  ::glGetBooleanv(theParamName, theValues);
  OpenGl_TRACE(glGetBooleanv)
}

static void APIENTRY glGetFloatv(GLenum theParamName, GLfloat* theValues)
{
  ::glGetFloatv(theParamName, theValues);
  OpenGl_TRACE(glGetFloatv)
}

static void APIENTRY glGetIntegerv(GLenum theParamName, GLint* theValues)
{
  ::glGetIntegerv(theParamName, theValues);
  OpenGl_TRACE(glGetIntegerv)
}

static GLenum APIENTRY glGetError()
{
  return ::glGetError();
}

static const GLubyte* APIENTRY glGetString(GLenum theName)
{
  const GLubyte* aRes = ::glGetString(theName);
  OpenGl_TRACE(glGetString) return aRes;
}

static void APIENTRY glFinish()
{
  ::glFinish();
  OpenGl_TRACE(glFinish)
}

static void APIENTRY glFlush()
{
  ::glFlush();
  OpenGl_TRACE(glFlush)
}

static void APIENTRY glHint(GLenum theTarget, GLenum theMode)
{
  ::glHint(theTarget, theMode);
  OpenGl_TRACE(glHint)
}

// Depth Buffer

static void APIENTRY glClearDepth(GLclampd theDepth)
{
#if defined(GL_ES_VERSION_2_0)
  ::glClearDepthf((GLfloat)theDepth);
#else
  ::glClearDepth(theDepth);
#endif
  OpenGl_TRACE(glClearDepth)
}

static void APIENTRY glClearDepthf(GLfloat theDepth)
{
#if defined(GL_ES_VERSION_2_0)
  ::glClearDepthf(theDepth);
#else
  ::glClearDepth(theDepth);
#endif
  OpenGl_TRACE(glClearDepthf)
}

static void APIENTRY glDepthFunc(GLenum theFunc)
{
  ::glDepthFunc(theFunc);
  OpenGl_TRACE(glDepthFunc)
}

static void APIENTRY glDepthMask(GLboolean theFlag)
{
  ::glDepthMask(theFlag);
  OpenGl_TRACE(glDepthMask)
}

static void APIENTRY glDepthRange(GLclampd theNearValue, GLclampd theFarValue)
{
#if defined(GL_ES_VERSION_2_0)
  ::glDepthRangef((GLfloat)theNearValue, (GLfloat)theFarValue);
#else
  ::glDepthRange(theNearValue, theFarValue);
#endif
  OpenGl_TRACE(glDepthRange)
}

static void APIENTRY glDepthRangef(GLfloat theNearValue, GLfloat theFarValue)
{
#if defined(GL_ES_VERSION_2_0)
  ::glDepthRangef(theNearValue, theFarValue);
#else
  ::glDepthRange(theNearValue, theFarValue);
#endif
  OpenGl_TRACE(glDepthRangef)
}

// Transformation

static void APIENTRY glViewport(GLint theX, GLint theY, GLsizei theWidth, GLsizei theHeight)
{
  ::glViewport(theX, theY, theWidth, theHeight);
  OpenGl_TRACE(glViewport)
}

// Vertex Arrays

static void APIENTRY glDrawArrays(GLenum theMode, GLint theFirst, GLsizei theCount)
{
  ::glDrawArrays(theMode, theFirst, theCount);
  OpenGl_TRACE(glDrawArrays)
}

static void APIENTRY glDrawElements(GLenum        theMode,
                                    GLsizei       theCount,
                                    GLenum        theType,
                                    const GLvoid* theIndices)
{
  ::glDrawElements(theMode, theCount, theType, theIndices);
  OpenGl_TRACE(glDrawElements)
}

// Raster functions

static void APIENTRY glPixelStorei(GLenum theParamName, GLint theParam)
{
  ::glPixelStorei(theParamName, theParam);
  OpenGl_TRACE(glPixelStorei)
}

static void APIENTRY glReadPixels(GLint   x,
                                  GLint   y,
                                  GLsizei width,
                                  GLsizei height,
                                  GLenum  format,
                                  GLenum  type,
                                  GLvoid* pixels)
{
  ::glReadPixels(x, y, width, height, format, type, pixels);
  OpenGl_TRACE(glReadPixels)
}

// Stenciling

static void APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
  ::glStencilFunc(func, ref, mask);
  OpenGl_TRACE(glStencilFunc)
}

static void APIENTRY glStencilMask(GLuint mask)
{
  ::glStencilMask(mask);
  OpenGl_TRACE(glStencilMask)
}

static void APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
  ::glStencilOp(fail, zfail, zpass);
  OpenGl_TRACE(glStencilOp)
}

static void APIENTRY glClearStencil(GLint s)
{
  ::glClearStencil(s);
  OpenGl_TRACE(glClearStencil)
}

// Texture mapping

static void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
  ::glTexParameterf(target, pname, param);
  OpenGl_TRACE(glTexParameterf)
}

static void APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param)
{
  ::glTexParameteri(target, pname, param);
  OpenGl_TRACE(glTexParameteri)
}

static void APIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
  ::glTexParameterfv(target, pname, params);
  OpenGl_TRACE(glTexParameterfv)
}

static void APIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
  ::glTexParameteriv(target, pname, params);
  OpenGl_TRACE(glTexParameteriv)
}

static void APIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
  ::glGetTexParameterfv(target, pname, params);
  OpenGl_TRACE(glGetTexParameterfv)
}

static void APIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
  ::glGetTexParameteriv(target, pname, params);
  OpenGl_TRACE(glGetTexParameteriv)
}

static void APIENTRY glTexImage2D(GLenum        target,
                                  GLint         level,
                                  GLint         internalFormat,
                                  GLsizei       width,
                                  GLsizei       height,
                                  GLint         border,
                                  GLenum        format,
                                  GLenum        type,
                                  const GLvoid* pixels)
{
  ::glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
  OpenGl_TRACE(glTexImage2D)
}

static void APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
  ::glGenTextures(n, textures);
  OpenGl_TRACE(glGenTextures)
}

static void APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
  ::glDeleteTextures(n, textures);
  OpenGl_TRACE(glDeleteTextures)
}

static void APIENTRY glBindTexture(GLenum target, GLuint texture)
{
  ::glBindTexture(target, texture);
  OpenGl_TRACE(glBindTexture)
}

static GLboolean APIENTRY glIsTexture(GLuint texture)
{
  const GLboolean aRes = ::glIsTexture(texture);
  OpenGl_TRACE(glIsTexture) return aRes;
}

static void APIENTRY glTexSubImage2D(GLenum        target,
                                     GLint         level,
                                     GLint         xoffset,
                                     GLint         yoffset,
                                     GLsizei       width,
                                     GLsizei       height,
                                     GLenum        format,
                                     GLenum        type,
                                     const GLvoid* pixels)
{
  ::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
  OpenGl_TRACE(glTexSubImage2D)
}

static void APIENTRY glCopyTexImage2D(GLenum  target,
                                      GLint   level,
                                      GLenum  internalformat,
                                      GLint   x,
                                      GLint   y,
                                      GLsizei width,
                                      GLsizei height,
                                      GLint   border)
{
  ::glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
  OpenGl_TRACE(glCopyTexImage2D)
}

static void APIENTRY glCopyTexSubImage2D(GLenum  target,
                                         GLint   level,
                                         GLint   xoffset,
                                         GLint   yoffset,
                                         GLint   x,
                                         GLint   y,
                                         GLsizei width,
                                         GLsizei height)
{
  ::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
  OpenGl_TRACE(glCopyTexSubImage2D)
}

// desktop extensions - not supported in OpenGL ES 2.0

static void APIENTRY glTexImage1D(GLenum        target,
                                  GLint         level,
                                  GLint         internalFormat,
                                  GLsizei       width,
                                  GLint         border,
                                  GLenum        format,
                                  GLenum        type,
                                  const GLvoid* pixels)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)target;
  (void)level;
  (void)internalFormat;
  (void)width;
  (void)border;
  (void)format;
  (void)type;
  (void)pixels;
#else
  ::glTexImage1D(target, level, internalFormat, width, border, format, type, pixels);
#endif
  OpenGl_TRACE(glTexImage1D)
}

static void APIENTRY glTexSubImage1D(GLenum        target,
                                     GLint         level,
                                     GLint         xoffset,
                                     GLsizei       width,
                                     GLenum        format,
                                     GLenum        type,
                                     const GLvoid* pixels)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)target;
  (void)level;
  (void)xoffset;
  (void)width;
  (void)format;
  (void)type;
  (void)pixels;
#else
  ::glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
#endif
  OpenGl_TRACE(glTexSubImage1D)
}

static void APIENTRY glCopyTexImage1D(GLenum  target,
                                      GLint   level,
                                      GLenum  internalformat,
                                      GLint   x,
                                      GLint   y,
                                      GLsizei width,
                                      GLint   border)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)target;
  (void)level;
  (void)internalformat;
  (void)x;
  (void)y;
  (void)width;
  (void)border;
#else
  ::glCopyTexImage1D(target, level, internalformat, x, y, width, border);
#endif
  OpenGl_TRACE(glCopyTexImage1D)
}

static void APIENTRY
  glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)target;
  (void)level;
  (void)xoffset;
  (void)x;
  (void)y;
  (void)width;
#else
  ::glCopyTexSubImage1D(target, level, xoffset, x, y, width);
#endif
  OpenGl_TRACE(glCopyTexSubImage1D)
}

static void APIENTRY
  glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)target;
  (void)level;
  (void)format;
  (void)type;
  (void)pixels;
#else
  ::glGetTexImage(target, level, format, type, pixels);
#endif
  OpenGl_TRACE(glGetTexImage)
}

static void APIENTRY glAlphaFunc(GLenum theFunc, GLclampf theRef)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)theFunc;
  (void)theRef;
#else
  ::glAlphaFunc(theFunc, theRef);
#endif
  OpenGl_TRACE(glAlphaFunc)
}

static void APIENTRY glPointSize(GLfloat theSize)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)theSize;
#else
  ::glPointSize(theSize);
#endif
  OpenGl_TRACE(glPointSize)
}

static void APIENTRY glPolygonMode(GLenum face, GLenum mode)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)face;
  (void)mode;
#else
  ::glPolygonMode(face, mode);
#endif
  OpenGl_TRACE(glPolygonMode);
}

static void APIENTRY glLogicOp(GLenum opcode)
{
#if defined(GL_ES_VERSION_2_0)
  ::glEnable(0xFFFF);
  (void)opcode;
#else
  ::glLogicOp(opcode);
#endif
  OpenGl_TRACE(glLogicOp)
}

static void APIENTRY glMultiDrawElements(GLenum             theMode,
                                         const GLsizei*     theCount,
                                         GLenum             theType,
                                         const void* const* theIndices,
                                         GLsizei            theDrawCount)
{
  if (theCount == NULL || theIndices == NULL)
  {
    return;
  }

  for (GLsizei aBatchIter = 0; aBatchIter < theDrawCount; ++aBatchIter)
  {
    ::glDrawElements(theMode, theCount[aBatchIter], theType, theIndices[aBatchIter]);
  }
  OpenGl_TRACE(glMultiDrawElements)
}

#if defined(GL_ES_VERSION_2_0)

// OpenGL ES 1.1

static void APIENTRY glActiveTexture(GLenum texture)
{
  ::glActiveTexture(texture);
  OpenGl_TRACE(glActiveTexture)
}

static void APIENTRY glCompressedTexImage2D(GLenum        target,
                                            GLint         level,
                                            GLenum        internalformat,
                                            GLsizei       width,
                                            GLsizei       height,
                                            GLint         border,
                                            GLsizei       imageSize,
                                            const GLvoid* data)
{
  ::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
  OpenGl_TRACE(glCompressedTexImage2D)
}

static void APIENTRY glCompressedTexSubImage2D(GLenum        target,
                                               GLint         level,
                                               GLint         xoffset,
                                               GLint         yoffset,
                                               GLsizei       width,
                                               GLsizei       height,
                                               GLenum        format,
                                               GLsizei       imageSize,
                                               const GLvoid* data)
{
  ::glCompressedTexSubImage2D(target,
                              level,
                              xoffset,
                              yoffset,
                              width,
                              height,
                              format,
                              imageSize,
                              data);
  OpenGl_TRACE(glCompressedTexSubImage2D)
}

static void APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
  ::glBindBuffer(target, buffer);
  OpenGl_TRACE(glBindBuffer)
}

static void APIENTRY glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
  ::glBufferData(target, size, data, usage);
  OpenGl_TRACE(glBufferData)
}

static void APIENTRY glBufferSubData(GLenum      target,
                                     GLintptr    offset,
                                     GLsizeiptr  size,
                                     const void* data)
{
  ::glBufferSubData(target, offset, size, data);
  OpenGl_TRACE(glBufferSubData)
}

static void APIENTRY glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
  ::glDeleteBuffers(n, buffers);
  OpenGl_TRACE(glDeleteBuffers)
}

static void APIENTRY glGenBuffers(GLsizei n, GLuint* buffers)
{
  ::glGenBuffers(n, buffers);
  OpenGl_TRACE(glGenBuffers)
}

static void APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
  ::glGetBufferParameteriv(target, pname, params);
  OpenGl_TRACE(glGetBufferParameteriv)
}

static GLboolean APIENTRY glIsBuffer(GLuint buffer)
{
  return ::glIsBuffer(buffer);
}

static void APIENTRY glSampleCoverage(GLfloat value, GLboolean invert)
{
  ::glSampleCoverage(value, invert);
  OpenGl_TRACE(glSampleCoverage)
}

// OpenGL ES 2.0

static void APIENTRY glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
  ::glBlendColor(red, green, blue, alpha);
  OpenGl_TRACE(glBlendColor)
}

static void APIENTRY glBlendEquation(GLenum mode)
{
  ::glBlendEquation(mode);
  OpenGl_TRACE(glBlendEquation)
}

static void APIENTRY glBlendFuncSeparate(GLenum sfactorRGB,
                                         GLenum dfactorRGB,
                                         GLenum sfactorAlpha,
                                         GLenum dfactorAlpha)
{
  ::glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
  OpenGl_TRACE(glBlendFuncSeparate)
}

static void APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
  ::glBlendEquationSeparate(modeRGB, modeAlpha);
  OpenGl_TRACE(glBlendEquationSeparate)
}

static void APIENTRY glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
  ::glStencilOpSeparate(face, sfail, dpfail, dppass);
  OpenGl_TRACE(glStencilOpSeparate)
}

static void APIENTRY glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
  ::glStencilFuncSeparate(face, func, ref, mask);
  OpenGl_TRACE(glStencilFuncSeparate)
}

static void APIENTRY glStencilMaskSeparate(GLenum face, GLuint mask)
{
  ::glStencilMaskSeparate(face, mask);
  OpenGl_TRACE(glStencilMaskSeparate)
}

static void APIENTRY glAttachShader(GLuint program, GLuint shader)
{
  ::glAttachShader(program, shader);
  OpenGl_TRACE(glAttachShader)
}

static void APIENTRY glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)
{
  ::glBindAttribLocation(program, index, name);
  OpenGl_TRACE(glBindAttribLocation)
}

static void APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
  ::glBindFramebuffer(target, framebuffer);
  OpenGl_TRACE(glBindFramebuffer)
}

static void APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
  ::glBindRenderbuffer(target, renderbuffer);
  OpenGl_TRACE(glBindRenderbuffer)
}

static GLenum APIENTRY glCheckFramebufferStatus(GLenum target)
{
  return ::glCheckFramebufferStatus(target);
}

static void APIENTRY glCompileShader(GLuint shader)
{
  ::glCompileShader(shader);
  OpenGl_TRACE(glCompileShader)
}

static GLuint APIENTRY glCreateProgram()
{
  return ::glCreateProgram();
}

static GLuint APIENTRY glCreateShader(GLenum type)
{
  return ::glCreateShader(type);
}

static void APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
  ::glDeleteFramebuffers(n, framebuffers);
  OpenGl_TRACE(glDeleteFramebuffers)
}

static void APIENTRY glDeleteProgram(GLuint program)
{
  ::glDeleteProgram(program);
  OpenGl_TRACE(glDeleteProgram)
}

static void APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
  ::glDeleteRenderbuffers(n, renderbuffers);
  OpenGl_TRACE(glDeleteRenderbuffers)
}

static void APIENTRY glDeleteShader(GLuint shader)
{
  ::glDeleteShader(shader);
  OpenGl_TRACE(glDeleteShader)
}

static void APIENTRY glDetachShader(GLuint program, GLuint shader)
{
  ::glDetachShader(program, shader);
  OpenGl_TRACE(glDetachShader)
}

static void APIENTRY glDisableVertexAttribArray(GLuint index)
{
  ::glDisableVertexAttribArray(index);
  OpenGl_TRACE(glDisableVertexAttribArray)
}

static void APIENTRY glEnableVertexAttribArray(GLuint index)
{
  ::glEnableVertexAttribArray(index);
  OpenGl_TRACE(glEnableVertexAttribArray)
}

static void APIENTRY glFramebufferRenderbuffer(GLenum target,
                                               GLenum attachment,
                                               GLenum renderbuffertarget,
                                               GLuint renderbuffer)
{
  ::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  OpenGl_TRACE(glFramebufferRenderbuffer)
}

static void APIENTRY glFramebufferTexture2D(GLenum target,
                                            GLenum attachment,
                                            GLenum textarget,
                                            GLuint texture,
                                            GLint  level)
{
  ::glFramebufferTexture2D(target, attachment, textarget, texture, level);
  OpenGl_TRACE(glFramebufferTexture2D)
}

static void APIENTRY glGenerateMipmap(GLenum target)
{
  ::glGenerateMipmap(target);
  OpenGl_TRACE(glGenerateMipmap)
}

static void APIENTRY glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
  ::glGenFramebuffers(n, framebuffers);
  OpenGl_TRACE(glGenFramebuffers)
}

static void APIENTRY glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
  ::glGenRenderbuffers(n, renderbuffers);
  OpenGl_TRACE(glGenRenderbuffers)
}

static void APIENTRY glGetActiveAttrib(GLuint   program,
                                       GLuint   index,
                                       GLsizei  bufSize,
                                       GLsizei* length,
                                       GLint*   size,
                                       GLenum*  type,
                                       GLchar*  name)
{
  ::glGetActiveAttrib(program, index, bufSize, length, size, type, name);
  OpenGl_TRACE(glGetActiveAttrib)
}

static void APIENTRY glGetActiveUniform(GLuint   program,
                                        GLuint   index,
                                        GLsizei  bufSize,
                                        GLsizei* length,
                                        GLint*   size,
                                        GLenum*  type,
                                        GLchar*  name)
{
  ::glGetActiveUniform(program, index, bufSize, length, size, type, name);
  OpenGl_TRACE(glGetActiveUniform)
}

static void APIENTRY glGetAttachedShaders(GLuint   program,
                                          GLsizei  maxCount,
                                          GLsizei* count,
                                          GLuint*  shaders)
{
  ::glGetAttachedShaders(program, maxCount, count, shaders);
  OpenGl_TRACE(glGetAttachedShaders)
}

static GLint APIENTRY glGetAttribLocation(GLuint program, const GLchar* name)
{
  const GLint aRes = ::glGetAttribLocation(program, name);
  OpenGl_TRACE(glGetAttribLocation) return aRes;
}

static void APIENTRY glGetFramebufferAttachmentParameteriv(GLenum target,
                                                           GLenum attachment,
                                                           GLenum pname,
                                                           GLint* params)
{
  ::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
  OpenGl_TRACE(glGetFramebufferAttachmentParameteriv)
}

static void APIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
  ::glGetProgramiv(program, pname, params);
  OpenGl_TRACE(glGetProgramiv)
}

static void APIENTRY glGetProgramInfoLog(GLuint   program,
                                         GLsizei  bufSize,
                                         GLsizei* length,
                                         GLchar*  infoLog)
{
  ::glGetProgramInfoLog(program, bufSize, length, infoLog);
  OpenGl_TRACE(glGetProgramInfoLog)
}

static void APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
  ::glGetRenderbufferParameteriv(target, pname, params);
  OpenGl_TRACE(glGetRenderbufferParameteriv)
}

static void APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
  ::glGetShaderiv(shader, pname, params);
  OpenGl_TRACE(glGetShaderiv)
}

static void APIENTRY glGetShaderInfoLog(GLuint   shader,
                                        GLsizei  bufSize,
                                        GLsizei* length,
                                        GLchar*  infoLog)
{
  ::glGetShaderInfoLog(shader, bufSize, length, infoLog);
  OpenGl_TRACE(glGetShaderInfoLog)
}

static void APIENTRY glGetShaderPrecisionFormat(GLenum shadertype,
                                                GLenum precisiontype,
                                                GLint* range,
                                                GLint* precision)
{
  ::glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
  OpenGl_TRACE(glGetShaderPrecisionFormat)
}

static void APIENTRY glGetShaderSource(GLuint   shader,
                                       GLsizei  bufSize,
                                       GLsizei* length,
                                       GLchar*  source)
{
  ::glGetShaderSource(shader, bufSize, length, source);
  OpenGl_TRACE(glGetShaderSource)
}

static void APIENTRY glGetUniformfv(GLuint program, GLint location, GLfloat* params)
{
  ::glGetUniformfv(program, location, params);
  OpenGl_TRACE(glGetUniformfv)
}

static void APIENTRY glGetUniformiv(GLuint program, GLint location, GLint* params)
{
  ::glGetUniformiv(program, location, params);
  OpenGl_TRACE(glGetUniformiv)
}

static GLint APIENTRY glGetUniformLocation(GLuint program, const GLchar* name)
{
  const GLint aRes = ::glGetUniformLocation(program, name);
  OpenGl_TRACE(glGetUniformLocation) return aRes;
}

static void APIENTRY glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
  ::glGetVertexAttribfv(index, pname, params);
  OpenGl_TRACE(glGetVertexAttribfv)
}

static void APIENTRY glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
  ::glGetVertexAttribiv(index, pname, params);
  OpenGl_TRACE(glGetVertexAttribiv)
}

static void APIENTRY glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
{
  ::glGetVertexAttribPointerv(index, pname, pointer);
  OpenGl_TRACE(glGetVertexAttribPointerv)
}

static GLboolean APIENTRY glIsFramebuffer(GLuint framebuffer)
{
  return ::glIsFramebuffer(framebuffer);
}

static GLboolean APIENTRY glIsProgram(GLuint program)
{
  return ::glIsProgram(program);
}

static GLboolean APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
  return ::glIsRenderbuffer(renderbuffer);
}

static GLboolean APIENTRY glIsShader(GLuint shader)
{
  return ::glIsShader(shader);
}

static void APIENTRY glLinkProgram(GLuint program)
{
  ::glLinkProgram(program);
  OpenGl_TRACE(glLinkProgram)
}

static void APIENTRY glReleaseShaderCompiler()
{
  ::glReleaseShaderCompiler();
  OpenGl_TRACE(glReleaseShaderCompiler)
}

static void APIENTRY glRenderbufferStorage(GLenum  target,
                                           GLenum  internalformat,
                                           GLsizei width,
                                           GLsizei height)
{
  ::glRenderbufferStorage(target, internalformat, width, height);
  OpenGl_TRACE(glRenderbufferStorage)
}

static void APIENTRY glShaderBinary(GLsizei       count,
                                    const GLuint* shaders,
                                    GLenum        binaryformat,
                                    const void*   binary,
                                    GLsizei       length)
{
  ::glShaderBinary(count, shaders, binaryformat, binary, length);
  OpenGl_TRACE(glShaderBinary)
}

static void APIENTRY glShaderSource(GLuint               shader,
                                    GLsizei              count,
                                    const GLchar* const* string,
                                    const GLint*         length)
{
  ::glShaderSource(shader, count, string, length);
  OpenGl_TRACE(glShaderSource)
}

static void APIENTRY glUniform1f(GLint location, GLfloat v0)
{
  ::glUniform1f(location, v0);
  OpenGl_TRACE(glUniform1f)
}

static void APIENTRY glUniform1fv(GLint location, GLsizei count, const GLfloat* value)
{
  ::glUniform1fv(location, count, value);
  OpenGl_TRACE(glUniform1fv)
}

static void APIENTRY glUniform1i(GLint location, GLint v0)
{
  ::glUniform1i(location, v0);
  OpenGl_TRACE(glUniform1i)
}

static void APIENTRY glUniform1iv(GLint location, GLsizei count, const GLint* value)
{
  ::glUniform1iv(location, count, value);
  OpenGl_TRACE(glUniform1iv)
}

static void APIENTRY glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
  ::glUniform2f(location, v0, v1);
  OpenGl_TRACE(glUniform2f)
}

static void APIENTRY glUniform2fv(GLint location, GLsizei count, const GLfloat* value)
{
  ::glUniform2fv(location, count, value);
  OpenGl_TRACE(glUniform2fv)
}

static void APIENTRY glUniform2i(GLint location, GLint v0, GLint v1)
{
  ::glUniform2i(location, v0, v1);
  OpenGl_TRACE(glUniform2i)
}

static void APIENTRY glUniform2iv(GLint location, GLsizei count, const GLint* value)
{
  ::glUniform2iv(location, count, value);
  OpenGl_TRACE(glUniform2iv)
}

static void APIENTRY glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
  ::glUniform3f(location, v0, v1, v2);
  OpenGl_TRACE(glUniform3f)
}

static void APIENTRY glUniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
  ::glUniform3fv(location, count, value);
  OpenGl_TRACE(glUniform3fv)
}

static void APIENTRY glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
{
  ::glUniform3i(location, v0, v1, v2);
  OpenGl_TRACE(glUniform3i)
}

static void APIENTRY glUniform3iv(GLint location, GLsizei count, const GLint* value)
{
  ::glUniform3iv(location, count, value);
  OpenGl_TRACE(glUniform3iv)
}

static void APIENTRY glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  ::glUniform4f(location, v0, v1, v2, v3);
  OpenGl_TRACE(glUniform4f)
}

static void APIENTRY glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
  ::glUniform4fv(location, count, value);
  OpenGl_TRACE(glUniform4fv)
}

static void APIENTRY glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
  ::glUniform4i(location, v0, v1, v2, v3);
  OpenGl_TRACE(glUniform4i)
}

static void APIENTRY glUniform4iv(GLint location, GLsizei count, const GLint* value)
{
  ::glUniform4iv(location, count, value);
  OpenGl_TRACE(glUniform4iv)
}

static void APIENTRY glUniformMatrix2fv(GLint          location,
                                        GLsizei        count,
                                        GLboolean      transpose,
                                        const GLfloat* value)
{
  ::glUniformMatrix2fv(location, count, transpose, value);
  OpenGl_TRACE(glUniformMatrix2fv)
}

static void APIENTRY glUniformMatrix3fv(GLint          location,
                                        GLsizei        count,
                                        GLboolean      transpose,
                                        const GLfloat* value)
{
  ::glUniformMatrix3fv(location, count, transpose, value);
  OpenGl_TRACE(glUniformMatrix3fv)
}

static void APIENTRY glUniformMatrix4fv(GLint          location,
                                        GLsizei        count,
                                        GLboolean      transpose,
                                        const GLfloat* value)
{
  ::glUniformMatrix4fv(location, count, transpose, value);
  OpenGl_TRACE(glUniformMatrix4fv)
}

static void APIENTRY glUseProgram(GLuint program)
{
  ::glUseProgram(program);
  OpenGl_TRACE(glUseProgram)
}

static void APIENTRY glValidateProgram(GLuint program)
{
  ::glValidateProgram(program);
  OpenGl_TRACE(glValidateProgram)
}

static void APIENTRY glVertexAttrib1f(GLuint index, GLfloat x)
{
  ::glVertexAttrib1f(index, x);
  OpenGl_TRACE(glVertexAttrib1f)
}

static void APIENTRY glVertexAttrib1fv(GLuint index, const GLfloat* v)
{
  ::glVertexAttrib1fv(index, v);
  OpenGl_TRACE(glVertexAttrib1fv)
}

static void APIENTRY glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
  ::glVertexAttrib2f(index, x, y);
  OpenGl_TRACE(glVertexAttrib2f)
}

static void APIENTRY glVertexAttrib2fv(GLuint index, const GLfloat* v)
{
  ::glVertexAttrib2fv(index, v);
  OpenGl_TRACE(glVertexAttrib2fv)
}

static void APIENTRY glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
  ::glVertexAttrib3f(index, x, y, z);
  OpenGl_TRACE(glVertexAttrib3f)
}

static void APIENTRY glVertexAttrib3fv(GLuint index, const GLfloat* v)
{
  ::glVertexAttrib3fv(index, v);
  OpenGl_TRACE(glVertexAttrib3fv)
}

static void APIENTRY glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  ::glVertexAttrib4f(index, x, y, z, w);
  OpenGl_TRACE(glVertexAttrib4f)
}

static void APIENTRY glVertexAttrib4fv(GLuint index, const GLfloat* v)
{
  ::glVertexAttrib4fv(index, v);
  OpenGl_TRACE(glVertexAttrib4fv)
}

static void APIENTRY glVertexAttribPointer(GLuint      index,
                                           GLint       size,
                                           GLenum      type,
                                           GLboolean   normalized,
                                           GLsizei     stride,
                                           const void* pointer)
{
  ::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  OpenGl_TRACE(glVertexAttribPointer)
}

#else
// legacy OpenGL 1.1 FFP

static void APIENTRY glTexEnvi(GLenum target, GLenum pname, GLint param)
{
  ::glTexEnvi(target, pname, param);
  OpenGl_TRACE(glTexEnvi)
}

static void APIENTRY glGetTexEnviv(GLenum target, GLenum pname, GLint* params)
{
  ::glGetTexEnviv(target, pname, params);
  OpenGl_TRACE(glGetTexEnviv)
}

// Begin/End primitive specification (removed since 3.1)

static void APIENTRY glColor4fv(const GLfloat* theVec)
{
  ::glColor4fv(theVec);
  OpenGl_TRACE(glColor4fv)
}

// Matrix operations (removed since 3.1)

static void APIENTRY glMatrixMode(GLenum theMode)
{
  ::glMatrixMode(theMode);
  OpenGl_TRACE(glMatrixMode)
}

static void APIENTRY glLoadIdentity()
{
  ::glLoadIdentity();
  OpenGl_TRACE(glLoadIdentity)
}

static void APIENTRY glLoadMatrixf(const GLfloat* theMatrix)
{
  ::glLoadMatrixf(theMatrix);
  OpenGl_TRACE(glLoadMatrixf)
}

// Line and Polygon stipple (removed since 3.1)

static void APIENTRY glLineStipple(GLint theFactor, GLushort thePattern)
{
  ::glLineStipple(theFactor, thePattern);
  OpenGl_TRACE(glLineStipple)
}

static void APIENTRY glPolygonStipple(const GLubyte* theMask)
{
  ::glPolygonStipple(theMask);
  OpenGl_TRACE(glPolygonStipple)
}

// Fixed pipeline lighting (removed since 3.1)

static void APIENTRY glShadeModel(GLenum theMode)
{
  ::glShadeModel(theMode);
  OpenGl_TRACE(glShadeModel)
}

static void APIENTRY glLightf(GLenum theLight, GLenum pname, GLfloat param)
{
  ::glLightf(theLight, pname, param);
  OpenGl_TRACE(glLightf)
}

static void APIENTRY glLightfv(GLenum theLight, GLenum pname, const GLfloat* params)
{
  ::glLightfv(theLight, pname, params);
  OpenGl_TRACE(glLightfv)
}

static void APIENTRY glLightModeli(GLenum pname, GLint param)
{
  ::glLightModeli(pname, param);
  OpenGl_TRACE(glLightModeli)
}

static void APIENTRY glLightModelfv(GLenum pname, const GLfloat* params)
{
  ::glLightModelfv(pname, params);
  OpenGl_TRACE(glLightModelfv)
}

static void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
  ::glMaterialf(face, pname, param);
  OpenGl_TRACE(glMaterialf)
}

static void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
  ::glMaterialfv(face, pname, params);
  OpenGl_TRACE(glMaterialfv)
}

static void APIENTRY glColorMaterial(GLenum face, GLenum mode)
{
  ::glColorMaterial(face, mode);
  OpenGl_TRACE(glColorMaterial)
}

// clipping plane (removed since 3.1)

static void APIENTRY glClipPlane(GLenum thePlane, const GLdouble* theEquation)
{
  ::glClipPlane(thePlane, theEquation);
  OpenGl_TRACE(glClipPlane)
}

// Display lists (removed since 3.1)

static void APIENTRY glDeleteLists(GLuint theList, GLsizei theRange)
{
  ::glDeleteLists(theList, theRange);
  OpenGl_TRACE(glDeleteLists)
}

static GLuint APIENTRY glGenLists(GLsizei theRange)
{
  const GLuint aRes = ::glGenLists(theRange);
  OpenGl_TRACE(glGenLists) return aRes;
}

static void APIENTRY glNewList(GLuint theList, GLenum theMode)
{
  ::glNewList(theList, theMode);
  OpenGl_TRACE(glNewList)
}

static void APIENTRY glEndList()
{
  ::glEndList();
  OpenGl_TRACE(glEndList)
}

static void APIENTRY glCallList(GLuint theList)
{
  ::glCallList(theList);
  OpenGl_TRACE(glCallList)
}

static void APIENTRY glCallLists(GLsizei theNb, GLenum theType, const GLvoid* theLists)
{
  ::glCallLists(theNb, theType, theLists);
  OpenGl_TRACE(glCallLists)
}

static void APIENTRY glListBase(GLuint theBase)
{
  ::glListBase(theBase);
  OpenGl_TRACE(glListBase)
}

// Current raster position and Rectangles (removed since 3.1)

static void APIENTRY glRasterPos2i(GLint x, GLint y)
{
  ::glRasterPos2i(x, y);
  OpenGl_TRACE(glRasterPos2i)
}

static void APIENTRY glRasterPos3fv(const GLfloat* theVec)
{
  ::glRasterPos3fv(theVec);
  OpenGl_TRACE(glRasterPos3fv)
}

// Texture mapping (removed since 3.1)

static void APIENTRY glTexGeni(GLenum coord, GLenum pname, GLint param)
{
  ::glTexGeni(coord, pname, param);
  OpenGl_TRACE(glTexGeni)
}

static void APIENTRY glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params)
{
  ::glTexGenfv(coord, pname, params);
  OpenGl_TRACE(glTexGenfv)
}

// Pixel copying (removed since 3.1)

static void APIENTRY
  glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
  ::glDrawPixels(width, height, format, type, pixels);
  OpenGl_TRACE(glDrawPixels)
}

static void APIENTRY glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
  ::glCopyPixels(x, y, width, height, type);
  OpenGl_TRACE(glCopyPixels)
}

static void APIENTRY glBitmap(GLsizei        width,
                              GLsizei        height,
                              GLfloat        xorig,
                              GLfloat        yorig,
                              GLfloat        xmove,
                              GLfloat        ymove,
                              const GLubyte* bitmap)
{
  ::glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
  OpenGl_TRACE(glBitmap)
}

// Edge flags and fixed-function vertex processing (removed since 3.1)

static void APIENTRY glIndexPointer(GLenum theType, GLsizei theStride, const GLvoid* thePtr)
{
  ::glIndexPointer(theType, theStride, thePtr);
  OpenGl_TRACE(glIndexPointer)
}

static void APIENTRY glVertexPointer(GLint         theSize,
                                     GLenum        theType,
                                     GLsizei       theStride,
                                     const GLvoid* thePtr)
{
  ::glVertexPointer(theSize, theType, theStride, thePtr);
  OpenGl_TRACE(glVertexPointer)
}

static void APIENTRY glNormalPointer(GLenum theType, GLsizei theStride, const GLvoid* thePtr)
{
  ::glNormalPointer(theType, theStride, thePtr);
  OpenGl_TRACE(glNormalPointer)
}

static void APIENTRY glColorPointer(GLint         theSize,
                                    GLenum        theType,
                                    GLsizei       theStride,
                                    const GLvoid* thePtr)
{
  ::glColorPointer(theSize, theType, theStride, thePtr);
  OpenGl_TRACE(glColorPointer)
}

static void APIENTRY glTexCoordPointer(GLint         theSize,
                                       GLenum        theType,
                                       GLsizei       theStride,
                                       const GLvoid* thePtr)
{
  ::glTexCoordPointer(theSize, theType, theStride, thePtr);
  OpenGl_TRACE(glTexCoordPointer)
}

static void APIENTRY glEnableClientState(GLenum theCap)
{
  ::glEnableClientState(theCap);
  OpenGl_TRACE(glEnableClientState)
}

static void APIENTRY glDisableClientState(GLenum theCap)
{
  ::glDisableClientState(theCap);
  OpenGl_TRACE(glDisableClientState)
}

static void APIENTRY glPixelTransferi(GLenum pname, GLint param)
{
  ::glPixelTransferi(pname, param);
  OpenGl_TRACE(glPixelTransferi)
}
#endif
} // namespace opencascade

// =======================================================================
// function : debugPrintError
// purpose  :
// =======================================================================
bool OpenGl_GlFunctions::debugPrintError(const char* theName)
{
  const int anErr = ::glGetError();
  if (anErr != GL_NO_ERROR)
  {
    Message::SendFail() << theName
                        << "(), unhandled GL error: " << OpenGl_Context::FormatGlError(anErr);
    // there is no glSetError(), just emulate non-clear state
    switch (anErr)
    {
      case GL_INVALID_VALUE: {
        ::glLineWidth(-1.0f);
        ::glLineWidth(1.0f);
        break;
      }
      default:
      case GL_INVALID_ENUM: {
        ::glEnable(0xFFFF);
        break;
      }
    }
  }
  return anErr != GL_NO_ERROR;
}

// =======================================================================
// function : readGlVersion
// purpose  :
// =======================================================================
void OpenGl_GlFunctions::readGlVersion(Standard_Integer& theGlVerMajor,
                                       Standard_Integer& theGlVerMinor)
{
  // reset values
  theGlVerMajor = 0;
  theGlVerMinor = 0;

  bool toCheckVer3 = true;
#if defined(__EMSCRIPTEN__)
  // WebGL 1.0 prints annoying invalid enumeration warnings to console.
  toCheckVer3 = false;
  if (EMSCRIPTEN_WEBGL_CONTEXT_HANDLE aWebGlCtx = emscripten_webgl_get_current_context())
  {
    EmscriptenWebGLContextAttributes anAttribs = {};
    if (emscripten_webgl_get_context_attributes(aWebGlCtx, &anAttribs) == EMSCRIPTEN_RESULT_SUCCESS)
    {
      toCheckVer3 = anAttribs.majorVersion >= 2;
    }
  }
#endif

  // Available since OpenGL 3.0 and OpenGL ES 3.0.
  if (toCheckVer3)
  {
    GLint aMajor = 0, aMinor = 0;
    ::glGetIntegerv(GL_MAJOR_VERSION, &aMajor);
    ::glGetIntegerv(GL_MINOR_VERSION, &aMinor);
    // glGetError() sometimes does not report an error here even if
    // GL does not know GL_MAJOR_VERSION and GL_MINOR_VERSION constants.
    // This happens on some renderers like e.g. Cygwin MESA.
    // Thus checking additionally if GL has put anything to
    // the output variables.
    if (::glGetError() == GL_NO_ERROR && aMajor != 0 && aMinor != 0)
    {
      theGlVerMajor = aMajor;
      theGlVerMinor = aMinor;
      return;
    }
    for (GLenum anErr = ::glGetError(), aPrevErr = GL_NO_ERROR;;
         aPrevErr = anErr, anErr = ::glGetError())
    {
      if (anErr == GL_NO_ERROR || anErr == aPrevErr)
      {
        break;
      }
    }
  }

  // Read version string.
  // Notice that only first two numbers split by point '2.1 XXXXX' are significant.
  // Following trash (after space) is vendor-specific.
  // New drivers also returns micro version of GL like '3.3.0' which has no meaning
  // and should be considered as vendor-specific too.
  const char* aVerStr = (const char*)::glGetString(GL_VERSION);
  if (aVerStr == NULL || *aVerStr == '\0')
  {
    // invalid GL context
    return;
  }

  // #if defined(GL_ES_VERSION_2_0)
  //  skip "OpenGL ES-** " section
  for (; *aVerStr != '\0'; ++aVerStr)
  {
    if (*aVerStr >= '0' && *aVerStr <= '9')
    {
      break;
    }
  }
  // #endif

  // parse string for major number
  char   aMajorStr[32];
  char   aMinorStr[32];
  size_t aMajIter = 0;
  while (aVerStr[aMajIter] >= '0' && aVerStr[aMajIter] <= '9')
  {
    ++aMajIter;
  }
  if (aMajIter == 0 || aMajIter >= sizeof(aMajorStr))
  {
    return;
  }
  memcpy(aMajorStr, aVerStr, aMajIter);
  aMajorStr[aMajIter] = '\0';

  // parse string for minor number
  aVerStr += aMajIter + 1;
  size_t aMinIter = 0;
  while (aVerStr[aMinIter] >= '0' && aVerStr[aMinIter] <= '9')
  {
    ++aMinIter;
  }
  if (aMinIter == 0 || aMinIter >= sizeof(aMinorStr))
  {
    return;
  }
  memcpy(aMinorStr, aVerStr, aMinIter);
  aMinorStr[aMinIter] = '\0';

  // read numbers
  theGlVerMajor = atoi(aMajorStr);
  theGlVerMinor = atoi(aMinorStr);
#if defined(__EMSCRIPTEN__)
  if (theGlVerMajor >= 3)
  {
    if (!toCheckVer3 || ::strstr(aVerStr, "WebGL 1.0") != NULL)
    {
      Message::SendWarning()
        << "Warning! OpenGL context reports version " << theGlVerMajor << "." << theGlVerMinor
        << " but WebGL 2.0 was unavailable\n"
        << "Fallback to OpenGL ES 2.0 will be used instead of reported version";
      theGlVerMajor = 2;
      theGlVerMinor = 0;
    }
  }
#endif

  if (theGlVerMajor <= 0)
  {
    theGlVerMajor = 0;
    theGlVerMinor = 0;
  }
}

// =======================================================================
// function : load
// purpose  :
// =======================================================================
void OpenGl_GlFunctions::load(OpenGl_Context& theCtx, Standard_Boolean theIsCoreProfile)
{
#if !defined(GL_ES_VERSION_2_0)
  bool isCoreProfile = false;
  if (theCtx.GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGL && theCtx.IsGlGreaterEqual(3, 2))
  {
    isCoreProfile = (theIsCoreProfile == true);

    // detect Core profile
    if (!isCoreProfile)
    {
      GLint aProfile = 0;
      ::glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &aProfile);
      isCoreProfile = (aProfile & GL_CONTEXT_CORE_PROFILE_BIT) != 0;
    }
  }
#else
  (void)theIsCoreProfile;
#endif

  // set built-in functions
  WrapProxyDef5(glGetIntegerv, glClearColor, glClear, glColorMask, glBlendFunc);
  WrapProxyDef5(glCullFace, glFrontFace, glLineWidth, glPolygonOffset, glScissor);
  WrapProxyDef5(glEnable, glDisable, glIsEnabled, glGetBooleanv, glGetFloatv);
  WrapProxyDef5(glGetIntegerv, glGetError, glGetString, glFinish, glFlush);
  WrapProxyDef5(glHint, glDepthFunc, glDepthMask, glPixelStorei, glClearStencil);
  WrapProxyDef5(glReadPixels, glStencilFunc, glStencilMask, glStencilOp, glTexParameterf);
  WrapProxyDef5(glTexParameterf,
                glTexParameteri,
                glTexParameterfv,
                glTexParameteriv,
                glGetTexParameterfv);
  WrapProxyDef5(glGetTexParameteriv, glTexImage2D, glGenTextures, glDeleteTextures, glBindTexture);
  WrapProxyDef5(glIsTexture, glTexSubImage2D, glCopyTexImage2D, glCopyTexSubImage2D, glViewport);
  WrapProxyProc5(glDrawArrays, glDrawElements, glMultiDrawElements, glClearDepth, glClearDepthf);
  WrapProxyProc5(glReadBuffer, glDrawBuffer, glGetPointerv, glDepthRange, glDepthRangef);
  WrapProxyProc5(glTexImage1D,
                 glTexSubImage1D,
                 glCopyTexImage1D,
                 glCopyTexSubImage1D,
                 glGetTexImage);

#if defined(GL_ES_VERSION_2_0)
  WrapProxyDef5(glActiveTexture,
                glCompressedTexImage2D,
                glCompressedTexSubImage2D,
                glBindBuffer,
                glBufferData);
  WrapProxyDef5(glBufferSubData,
                glDeleteBuffers,
                glDepthRangef,
                glGenBuffers,
                glGetBufferParameteriv);
  WrapProxyDef5(glIsBuffer, glSampleCoverage, glBlendColor, glBlendEquation, glBlendFuncSeparate);
  WrapProxyDef5(glBlendEquationSeparate,
                glStencilOpSeparate,
                glStencilFuncSeparate,
                glStencilMaskSeparate,
                glAttachShader);
  WrapProxyDef5(glBindAttribLocation,
                glBindFramebuffer,
                glBindRenderbuffer,
                glCheckFramebufferStatus,
                glCompileShader);
  WrapProxyDef5(glCreateProgram,
                glCreateShader,
                glDeleteFramebuffers,
                glDeleteProgram,
                glDeleteRenderbuffers);
  WrapProxyDef5(glDeleteShader,
                glDetachShader,
                glDisableVertexAttribArray,
                glEnableVertexAttribArray,
                glFramebufferRenderbuffer);
  WrapProxyDef5(glFramebufferTexture2D,
                glGenerateMipmap,
                glGenFramebuffers,
                glGenRenderbuffers,
                glGetActiveAttrib);
  WrapProxyDef5(glGetActiveUniform,
                glGetAttachedShaders,
                glGetAttribLocation,
                glGetFramebufferAttachmentParameteriv,
                glGetProgramiv);
  WrapProxyDef5(glGetProgramInfoLog,
                glGetRenderbufferParameteriv,
                glGetShaderiv,
                glGetShaderInfoLog,
                glGetShaderPrecisionFormat);
  WrapProxyDef5(glGetShaderSource,
                glGetUniformfv,
                glGetUniformiv,
                glGetUniformLocation,
                glGetVertexAttribfv);
  WrapProxyDef5(glGetVertexAttribiv,
                glGetVertexAttribPointerv,
                glIsFramebuffer,
                glIsProgram,
                glIsRenderbuffer);
  WrapProxyDef5(glIsShader,
                glLinkProgram,
                glReleaseShaderCompiler,
                glRenderbufferStorage,
                glShaderBinary);
  WrapProxyDef5(glShaderSource, glUniform1f, glUniform1fv, glUniform1i, glUniform1iv);
  WrapProxyDef5(glUniform2f, glUniform2fv, glUniform2i, glUniform2iv, glUniform3f);
  WrapProxyDef5(glUniform3fv, glUniform3i, glUniform3iv, glUniform4f, glUniform4fv);
  WrapProxyDef5(glUniform4i,
                glUniform4iv,
                glUniformMatrix2fv,
                glUniformMatrix3fv,
                glUniformMatrix4fv);
  WrapProxyDef5(glUseProgram,
                glValidateProgram,
                glVertexAttrib1f,
                glVertexAttrib1fv,
                glVertexAttrib2f);
  WrapProxyDef5(glVertexAttrib2fv,
                glVertexAttrib3f,
                glVertexAttrib3fv,
                glVertexAttrib4f,
                glVertexAttrib4fv);
  WrapProxyDef(glVertexAttribPointer);
  // empty fallbacks
  WrapProxyProc5(glAlphaFunc, glPointSize, glLogicOp, glPolygonMode, glGetTexLevelParameteriv);
  WrapProxyProc(glGetTexLevelParameterfv);
#else
  WrapProxyDef5(glAlphaFunc, glPointSize, glLogicOp, glPolygonMode, glGetTexLevelParameteriv);
  WrapProxyDef(glGetTexLevelParameterfv);
  if (!isCoreProfile)
  {
    WrapProxyDef5(glTexEnvi, glGetTexEnviv, glColor4fv, glMatrixMode, glLoadIdentity);
    WrapProxyDef5(glLoadMatrixf, glLineStipple, glPolygonStipple, glShadeModel, glLightf);
    WrapProxyDef5(glLightfv, glLightModeli, glLightModelfv, glMaterialf, glMaterialfv);
    WrapProxyDef5(glColorMaterial, glClipPlane, glDeleteLists, glGenLists, glNewList);
    WrapProxyDef5(glEndList, glCallList, glCallLists, glListBase, glRasterPos2i);
    WrapProxyDef5(glRasterPos3fv, glTexGeni, glTexGenfv, glDrawPixels, glCopyPixels);
    WrapProxyDef5(glBitmap, glIndexPointer, glVertexPointer, glNormalPointer, glColorPointer);
    WrapProxyDef(glTexCoordPointer);
    WrapProxyDef(glEnableClientState);
    WrapProxyDef(glDisableClientState);
    WrapProxyDef(glPixelTransferi);
  }
#endif

  if (theCtx.IsGlGreaterEqual(3, 0))
  {
    // retrieve auxiliary function in advance
    theCtx.FindProc("glGetStringi", theCtx.myFuncs->glGetStringi);
  }

#if defined(GL_ES_VERSION_2_0)
  theCtx.core11ffp = NULL;
#else
  theCtx.core11ffp = !isCoreProfile ? (OpenGl_GlCore11*)this : NULL;
#endif
  theCtx.core11fwd      = (OpenGl_GlCore11Fwd*)this;
  theCtx.core15         = NULL;
  theCtx.core15fwd      = NULL;
  theCtx.core20         = NULL;
  theCtx.core20fwd      = NULL;
  theCtx.core30         = NULL;
  theCtx.core32         = NULL;
  theCtx.core33         = NULL;
  theCtx.core41         = NULL;
  theCtx.core42         = NULL;
  theCtx.core43         = NULL;
  theCtx.core44         = NULL;
  theCtx.core45         = NULL;
  theCtx.core46         = NULL;
  theCtx.arbTBO         = NULL;
  theCtx.arbTboRGB32    = false;
  theCtx.arbClipControl = false;
  theCtx.arbIns         = NULL;
  theCtx.arbDbg         = NULL;
  theCtx.arbFBO         = NULL;
  theCtx.arbFBOBlit     = NULL;
  theCtx.extGS          = NULL;

  //! Make record shorter to retrieve function pointer using variable with same name
  const char* aLastFailedProc = NULL;
#define FindProcShort(theFunc) theCtx.FindProcVerbose(aLastFailedProc, #theFunc, this->theFunc)
#define checkExtensionShort theCtx.CheckExtension
#define isGlGreaterEqualShort(theMaj, theMin) theCtx.IsGlGreaterEqual(theMaj, theMin)

#if defined(GL_ES_VERSION_2_0)

  theCtx.hasTexRGBA8 = isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_OES_rgb8_rgba8");
  theCtx.hasTexSRGB  = isGlGreaterEqualShort(3, 0);
  theCtx.hasFboSRGB  = isGlGreaterEqualShort(3, 0);
  if (!isGlGreaterEqualShort(3, 0) && checkExtensionShort("GL_EXT_sRGB"))
  {
    // limited support
    theCtx.hasTexSRGB = true;
    theCtx.hasFboSRGB = true;
  }
  theCtx.hasFboRenderMipmap =
    isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_OES_fbo_render_mipmap");
  theCtx.hasSRGBControl   = checkExtensionShort("GL_EXT_sRGB_write_control");
  theCtx.hasPackRowLength = isGlGreaterEqualShort(3, 0);
  // clang-format off
  theCtx.hasUnpackRowLength = isGlGreaterEqualShort (3, 0); // || checkExtensionShort ("GL_EXT_unpack_subimage");
  // NPOT textures has limited support within OpenGL ES 2.0
  // which are relaxed by OpenGL ES 3.0 or some extensions
  //theCtx.arbNPTW = isGlGreaterEqualShort (3, 0)
  //           || checkExtensionShort ("GL_OES_texture_npot")
  //           || checkExtensionShort ("GL_NV_texture_npot_2D_mipmap");
  // clang-format on
  theCtx.arbNPTW   = true;
  theCtx.arbTexRG  = isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_EXT_texture_rg");
  theCtx.extBgra   = checkExtensionShort("GL_EXT_texture_format_BGRA8888");
  theCtx.extTexR16 = checkExtensionShort("GL_EXT_texture_norm16");
  theCtx.extAnis   = checkExtensionShort("GL_EXT_texture_filter_anisotropic");
  theCtx.extPDS = isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_OES_packed_depth_stencil");

  theCtx.core11fwd = (OpenGl_GlCore11Fwd*)this;
  if (isGlGreaterEqualShort(2, 0))
  {
    // enable compatible functions
    theCtx.core20    = (OpenGl_GlCore20*)this;
    theCtx.core20fwd = (OpenGl_GlCore20*)this;
    theCtx.core15    = (OpenGl_GlCore15*)this;
    theCtx.core15fwd = (OpenGl_GlCore15*)this;
    theCtx.arbFBO    = (OpenGl_ArbFBO*)this;
  }
  if (isGlGreaterEqualShort(3, 0) && FindProcShort(glBlitFramebuffer))
  {
    theCtx.arbFBOBlit = (OpenGl_ArbFBOBlit*)this;
  }
  if (isGlGreaterEqualShort(3, 0) && FindProcShort(glGenSamplers) && FindProcShort(glDeleteSamplers)
      && FindProcShort(glIsSampler) && FindProcShort(glBindSampler)
      && FindProcShort(glSamplerParameteri) && FindProcShort(glSamplerParameteriv)
      && FindProcShort(glSamplerParameterf) && FindProcShort(glSamplerParameterfv)
      && FindProcShort(glGetSamplerParameteriv) && FindProcShort(glGetSamplerParameterfv))
  //&& FindProcShort (glSamplerParameterIiv) // only on Desktop or with extensions
  // GL_OES_texture_border_clamp/GL_EXT_texture_border_clamp
  //&& FindProcShort (glSamplerParameterIuiv)
  //&& FindProcShort (glGetSamplerParameterIiv)
  //&& FindProcShort (glGetSamplerParameterIuiv))
  {
    theCtx.arbSamplerObject = (OpenGl_ArbSamplerObject*)this;
  }
  theCtx.extFragDepth = !isGlGreaterEqualShort(3, 0) && checkExtensionShort("GL_EXT_frag_depth");
  if (isGlGreaterEqualShort(3, 1) && FindProcShort(glTexStorage2DMultisample))
  {
    //
  }

  theCtx.hasUintIndex =
    isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_OES_element_index_uint");
  theCtx.hasHighp = checkExtensionShort("GL_OES_fragment_precision_high");
  GLint aRange[2] = {0, 0};
  GLint aPrec     = 0;
  ::glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, aRange, &aPrec);
  if (aPrec != 0)
  {
    theCtx.hasHighp = Standard_True;
  }

  theCtx.arbTexFloat = (isGlGreaterEqualShort(3, 0) && FindProcShort(glTexImage3D))
                       || checkExtensionShort("GL_OES_texture_float");
  theCtx.hasTexFloatLinear =
    theCtx.arbTexFloat && checkExtensionShort("GL_OES_texture_float_linear");

  const bool hasTexBuffer32  = isGlGreaterEqualShort(3, 2) && FindProcShort(glTexBuffer);
  const bool hasExtTexBuffer = checkExtensionShort("GL_EXT_texture_buffer")
                               && theCtx.FindProc("glTexBufferEXT", this->glTexBuffer);
  if (hasTexBuffer32 || hasExtTexBuffer)
  {
    theCtx.arbTBO = reinterpret_cast<OpenGl_ArbTBO*>(this);
  }

  bool hasInstanced = isGlGreaterEqualShort(3, 0) && FindProcShort(glVertexAttribDivisor)
                      && FindProcShort(glDrawArraysInstanced)
                      && FindProcShort(glDrawElementsInstanced);
  if (!hasInstanced)
  {
    hasInstanced =
      checkExtensionShort("GL_ANGLE_instanced_arrays")
      && theCtx.FindProc("glVertexAttribDivisorANGLE", this->glVertexAttribDivisor)
      && theCtx.FindProc("glDrawArraysInstancedANGLE", this->glDrawArraysInstanced)
      && theCtx.FindProc("glDrawElementsInstancedANGLE", this->glDrawElementsInstanced);
  }
  if (hasInstanced)
  {
    theCtx.arbIns = (OpenGl_ArbIns*)this;
  }

  const bool hasVAO = isGlGreaterEqualShort(3, 0) && FindProcShort(glBindVertexArray)
                      && FindProcShort(glDeleteVertexArrays) && FindProcShort(glGenVertexArrays)
                      && FindProcShort(glIsVertexArray);
  #ifndef __EMSCRIPTEN__ // latest Emscripten does not pretend having / simulating mapping buffer
                         // functions
  const bool hasMapBufferRange =
    isGlGreaterEqualShort(3, 0) && FindProcShort(glMapBufferRange) && FindProcShort(glUnmapBuffer)
    && FindProcShort(glGetBufferPointerv) && FindProcShort(glFlushMappedBufferRange);
  #endif

  // load OpenGL ES 3.0 new functions
  const bool has30es =
    isGlGreaterEqualShort(3, 0) && hasVAO
  #ifndef __EMSCRIPTEN__
    && hasMapBufferRange
  #endif
    && hasInstanced && theCtx.arbSamplerObject != NULL && theCtx.arbFBOBlit != NULL
    && FindProcShort(glReadBuffer) && FindProcShort(glDrawRangeElements)
    && FindProcShort(glTexImage3D) && FindProcShort(glTexSubImage3D)
    && FindProcShort(glCopyTexSubImage3D) && FindProcShort(glCompressedTexImage3D)
    && FindProcShort(glCompressedTexSubImage3D) && FindProcShort(glGenQueries)
    && FindProcShort(glDeleteQueries) && FindProcShort(glIsQuery) && FindProcShort(glBeginQuery)
    && FindProcShort(glEndQuery) && FindProcShort(glGetQueryiv)
    && FindProcShort(glGetQueryObjectuiv) && FindProcShort(glDrawBuffers)
    && FindProcShort(glUniformMatrix2x3fv) && FindProcShort(glUniformMatrix3x2fv)
    && FindProcShort(glUniformMatrix2x4fv) && FindProcShort(glUniformMatrix4x2fv)
    && FindProcShort(glUniformMatrix3x4fv) && FindProcShort(glUniformMatrix4x3fv)
    && FindProcShort(glRenderbufferStorageMultisample) && FindProcShort(glFramebufferTextureLayer)
    && FindProcShort(glGetIntegeri_v) && FindProcShort(glBeginTransformFeedback)
    && FindProcShort(glEndTransformFeedback) && FindProcShort(glBindBufferRange)
    && FindProcShort(glBindBufferBase) && FindProcShort(glTransformFeedbackVaryings)
    && FindProcShort(glGetTransformFeedbackVarying) && FindProcShort(glVertexAttribIPointer)
    && FindProcShort(glGetVertexAttribIiv) && FindProcShort(glGetVertexAttribIuiv)
    && FindProcShort(glVertexAttribI4i) && FindProcShort(glVertexAttribI4ui)
    && FindProcShort(glVertexAttribI4iv) && FindProcShort(glVertexAttribI4uiv)
    && FindProcShort(glGetUniformuiv) && FindProcShort(glGetFragDataLocation)
    && FindProcShort(glUniform1ui) && FindProcShort(glUniform2ui) && FindProcShort(glUniform3ui)
    && FindProcShort(glUniform4ui) && FindProcShort(glUniform1uiv) && FindProcShort(glUniform2uiv)
    && FindProcShort(glUniform3uiv) && FindProcShort(glUniform4uiv)
    && FindProcShort(glClearBufferiv) && FindProcShort(glClearBufferuiv)
    && FindProcShort(glClearBufferfv) && FindProcShort(glClearBufferfi)
    && FindProcShort(glGetStringi) && FindProcShort(glCopyBufferSubData)
    && FindProcShort(glGetUniformIndices) && FindProcShort(glGetActiveUniformsiv)
    && FindProcShort(glGetUniformBlockIndex) && FindProcShort(glGetActiveUniformBlockiv)
    && FindProcShort(glGetActiveUniformBlockName) && FindProcShort(glUniformBlockBinding)
    && FindProcShort(glFenceSync) && FindProcShort(glIsSync) && FindProcShort(glDeleteSync)
    && FindProcShort(glClientWaitSync) && FindProcShort(glWaitSync)
    && FindProcShort(glGetInteger64v) && FindProcShort(glGetSynciv)
    && FindProcShort(glGetInteger64i_v) && FindProcShort(glGetBufferParameteri64v)
    && FindProcShort(glBindTransformFeedback) && FindProcShort(glDeleteTransformFeedbacks)
    && FindProcShort(glGenTransformFeedbacks) && FindProcShort(glIsTransformFeedback)
    && FindProcShort(glPauseTransformFeedback) && FindProcShort(glResumeTransformFeedback)
    && FindProcShort(glGetProgramBinary) && FindProcShort(glProgramBinary)
    && FindProcShort(glProgramParameteri) && FindProcShort(glInvalidateFramebuffer)
    && FindProcShort(glInvalidateSubFramebuffer) && FindProcShort(glTexStorage2D)
    && FindProcShort(glTexStorage3D) && FindProcShort(glGetInternalformativ);
  if (!has30es)
  {
    theCtx.checkWrongVersion(3, 0, aLastFailedProc);
  }
  else
  {
    theCtx.core30           = (OpenGl_GlCore30*)this;
    theCtx.hasGetBufferData = true;
  }

  // load OpenGL ES 3.1 new functions
  const bool has31es =
    isGlGreaterEqualShort(3, 1) && has30es && FindProcShort(glDispatchCompute)
    && FindProcShort(glDispatchComputeIndirect) && FindProcShort(glDrawArraysIndirect)
    && FindProcShort(glDrawElementsIndirect) && FindProcShort(glFramebufferParameteri)
    && FindProcShort(glGetFramebufferParameteriv) && FindProcShort(glGetProgramInterfaceiv)
    && FindProcShort(glGetProgramResourceIndex) && FindProcShort(glGetProgramResourceName)
    && FindProcShort(glGetProgramResourceiv) && FindProcShort(glGetProgramResourceLocation)
    && FindProcShort(glUseProgramStages) && FindProcShort(glActiveShaderProgram)
    && FindProcShort(glCreateShaderProgramv) && FindProcShort(glBindProgramPipeline)
    && FindProcShort(glDeleteProgramPipelines) && FindProcShort(glGenProgramPipelines)
    && FindProcShort(glIsProgramPipeline) && FindProcShort(glGetProgramPipelineiv)
    && FindProcShort(glProgramUniform1i) && FindProcShort(glProgramUniform2i)
    && FindProcShort(glProgramUniform3i) && FindProcShort(glProgramUniform4i)
    && FindProcShort(glProgramUniform1ui) && FindProcShort(glProgramUniform2ui)
    && FindProcShort(glProgramUniform3ui) && FindProcShort(glProgramUniform4ui)
    && FindProcShort(glProgramUniform1f) && FindProcShort(glProgramUniform2f)
    && FindProcShort(glProgramUniform3f) && FindProcShort(glProgramUniform4f)
    && FindProcShort(glProgramUniform1iv) && FindProcShort(glProgramUniform2iv)
    && FindProcShort(glProgramUniform3iv) && FindProcShort(glProgramUniform4iv)
    && FindProcShort(glProgramUniform1uiv) && FindProcShort(glProgramUniform2uiv)
    && FindProcShort(glProgramUniform3uiv) && FindProcShort(glProgramUniform4uiv)
    && FindProcShort(glProgramUniform1fv) && FindProcShort(glProgramUniform2fv)
    && FindProcShort(glProgramUniform3fv) && FindProcShort(glProgramUniform4fv)
    && FindProcShort(glProgramUniformMatrix2fv) && FindProcShort(glProgramUniformMatrix3fv)
    && FindProcShort(glProgramUniformMatrix4fv) && FindProcShort(glProgramUniformMatrix2x3fv)
    && FindProcShort(glProgramUniformMatrix3x2fv) && FindProcShort(glProgramUniformMatrix2x4fv)
    && FindProcShort(glProgramUniformMatrix4x2fv) && FindProcShort(glProgramUniformMatrix3x4fv)
    && FindProcShort(glProgramUniformMatrix4x3fv) && FindProcShort(glValidateProgramPipeline)
    && FindProcShort(glGetProgramPipelineInfoLog) && FindProcShort(glBindImageTexture)
    && FindProcShort(glGetBooleani_v) && FindProcShort(glMemoryBarrier)
    && FindProcShort(glMemoryBarrierByRegion) && FindProcShort(glTexStorage2DMultisample)
    && FindProcShort(glGetMultisamplefv) && FindProcShort(glSampleMaski)
    && FindProcShort(glGetTexLevelParameteriv) && FindProcShort(glGetTexLevelParameterfv)
    && FindProcShort(glBindVertexBuffer) && FindProcShort(glVertexAttribFormat)
    && FindProcShort(glVertexAttribIFormat) && FindProcShort(glVertexAttribBinding)
    && FindProcShort(glVertexBindingDivisor);
  if (!has31es)
  {
    theCtx.checkWrongVersion(3, 1, aLastFailedProc);
  }

  // initialize debug context extension
  if (isGlGreaterEqualShort(3, 2) || checkExtensionShort("GL_KHR_debug"))
  {
    // this functionality become a part of OpenGL ES 3.2
    theCtx.arbDbg = NULL;
    if (isGlGreaterEqualShort(3, 2) && FindProcShort(glDebugMessageControl)
        && FindProcShort(glDebugMessageInsert) && FindProcShort(glDebugMessageCallback)
        && FindProcShort(glGetDebugMessageLog))
    {
      theCtx.arbDbg = (OpenGl_ArbDbg*)this;
    }
    // According to GL_KHR_debug spec, all functions should have KHR suffix.
    // However, some implementations can export these functions without suffix.
    else if (!isGlGreaterEqualShort(3, 2)
             && theCtx.FindProc("glDebugMessageControlKHR", this->glDebugMessageControl)
             && theCtx.FindProc("glDebugMessageInsertKHR", this->glDebugMessageInsert)
             && theCtx.FindProc("glDebugMessageCallbackKHR", this->glDebugMessageCallback)
             && theCtx.FindProc("glGetDebugMessageLogKHR", this->glGetDebugMessageLog))
    {
      theCtx.arbDbg = (OpenGl_ArbDbg*)this;
    }
  }

  // load OpenGL ES 3.2 new functions
  const bool has32es =
    isGlGreaterEqualShort(3, 2) && has31es && hasTexBuffer32 && theCtx.arbDbg != NULL
    && FindProcShort(glBlendBarrier) && FindProcShort(glCopyImageSubData)
    && FindProcShort(glPushDebugGroup) && FindProcShort(glPopDebugGroup)
    && FindProcShort(glObjectLabel) && FindProcShort(glGetObjectLabel)
    && FindProcShort(glObjectPtrLabel) && FindProcShort(glGetObjectPtrLabel)
    && FindProcShort(glGetPointerv) && FindProcShort(glEnablei) && FindProcShort(glDisablei)
    && FindProcShort(glBlendEquationi) && FindProcShort(glBlendEquationSeparatei)
    && FindProcShort(glBlendFunci) && FindProcShort(glBlendFuncSeparatei)
    && FindProcShort(glColorMaski) && FindProcShort(glIsEnabledi)
    && FindProcShort(glDrawElementsBaseVertex) && FindProcShort(glDrawRangeElementsBaseVertex)
    && FindProcShort(glDrawElementsInstancedBaseVertex) && FindProcShort(glFramebufferTexture)
    && FindProcShort(glPrimitiveBoundingBox) && FindProcShort(glGetGraphicsResetStatus)
    && FindProcShort(glReadnPixels) && FindProcShort(glGetnUniformfv)
    && FindProcShort(glGetnUniformiv) && FindProcShort(glGetnUniformuiv)
    && FindProcShort(glMinSampleShading) && FindProcShort(glPatchParameteri)
    && FindProcShort(glTexParameterIiv) && FindProcShort(glTexParameterIuiv)
    && FindProcShort(glGetTexParameterIiv) && FindProcShort(glGetTexParameterIuiv)
    && FindProcShort(glSamplerParameterIiv) && FindProcShort(glSamplerParameterIuiv)
    && FindProcShort(glGetSamplerParameterIiv) && FindProcShort(glGetSamplerParameterIuiv)
    && FindProcShort(glTexBufferRange) && FindProcShort(glTexStorage3DMultisample);
  if (!has32es)
  {
    theCtx.checkWrongVersion(3, 2, aLastFailedProc);
  }

  // clang-format off
  theCtx.arbTboRGB32 = isGlGreaterEqualShort (3, 2); // OpenGL ES 3.2 introduces TBO already supporting RGB32 format
  // clang-format on
  theCtx.extDrawBuffers = checkExtensionShort("GL_EXT_draw_buffers")
                          && theCtx.FindProc("glDrawBuffersEXT", this->glDrawBuffers);
  theCtx.arbDrawBuffers = checkExtensionShort("GL_ARB_draw_buffers")
                          && theCtx.FindProc("glDrawBuffersARB", this->glDrawBuffers);

  if (isGlGreaterEqualShort(3, 0) && FindProcShort(glDrawBuffers))
  {
    theCtx.hasDrawBuffers = OpenGl_FeatureInCore;
  }
  else if (theCtx.extDrawBuffers || theCtx.arbDrawBuffers)
  {
    theCtx.hasDrawBuffers = OpenGl_FeatureInExtensions;
  }

  // float textures available since OpenGL ES 3.0+,
  // but renderable only since 3.2+ or with extension
  theCtx.hasFloatBuffer = theCtx.hasHalfFloatBuffer = OpenGl_FeatureNotAvailable;
  if (isGlGreaterEqualShort(3, 2))
  {
    theCtx.hasFloatBuffer = theCtx.hasHalfFloatBuffer = OpenGl_FeatureInCore;
  }
  else
  {
    if (checkExtensionShort("GL_EXT_color_buffer_float"))
    {
      theCtx.hasFloatBuffer =
        isGlGreaterEqualShort(3, 0) ? OpenGl_FeatureInCore : OpenGl_FeatureInExtensions;
    }
    if (checkExtensionShort("GL_EXT_color_buffer_half_float"))
    {
      // GL_HALF_FLOAT_OES for OpenGL ES 2.0 and GL_HALF_FLOAT for OpenGL ES 3.0+
      theCtx.hasHalfFloatBuffer =
        isGlGreaterEqualShort(3, 0) ? OpenGl_FeatureInCore : OpenGl_FeatureInExtensions;
    }
  }

  theCtx.oesSampleVariables = checkExtensionShort("GL_OES_sample_variables");
  theCtx.oesStdDerivatives  = checkExtensionShort("GL_OES_standard_derivatives");
  theCtx.hasSampleVariables = isGlGreaterEqualShort(3, 2) ? OpenGl_FeatureInCore
                              : theCtx.oesSampleVariables ? OpenGl_FeatureInExtensions
                                                          : OpenGl_FeatureNotAvailable;
  theCtx.hasGlslBitwiseOps =
    isGlGreaterEqualShort(3, 0) ? OpenGl_FeatureInCore : OpenGl_FeatureNotAvailable;
  // without hasHighp, dFdx/dFdy precision is considered too low for flat shading (visual artifacts)
  theCtx.hasFlatShading = isGlGreaterEqualShort(3, 0) ? OpenGl_FeatureInCore
                                                      : (theCtx.oesStdDerivatives && theCtx.hasHighp
                                                           ? OpenGl_FeatureInExtensions
                                                           : OpenGl_FeatureNotAvailable);
  if (!isGlGreaterEqualShort(3, 1) && theCtx.Vendor().Search("qualcomm") != -1)
  {
    // dFdx/dFdy are completely broken on tested Adreno devices with versions below OpenGl ES 3.1
    theCtx.hasFlatShading = OpenGl_FeatureNotAvailable;
  }

  theCtx.hasGeometryStage = isGlGreaterEqualShort(3, 2)
                              ? OpenGl_FeatureInCore
                              : (checkExtensionShort("GL_EXT_geometry_shader")
                                     && checkExtensionShort("GL_EXT_shader_io_blocks")
                                   ? OpenGl_FeatureInExtensions
                                   : OpenGl_FeatureNotAvailable);
#else

  theCtx.hasTexRGBA8        = true;
  theCtx.hasTexSRGB         = isGlGreaterEqualShort(2, 1);
  theCtx.hasFboSRGB         = isGlGreaterEqualShort(2, 1);
  theCtx.hasSRGBControl     = theCtx.hasFboSRGB;
  theCtx.hasFboRenderMipmap = true;
  theCtx.arbDrawBuffers     = checkExtensionShort("GL_ARB_draw_buffers");
  theCtx.arbNPTW            = checkExtensionShort("GL_ARB_texture_non_power_of_two");
  theCtx.arbTexFloat = isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_ARB_texture_float");
  theCtx.hasTexFloatLinear = theCtx.arbTexFloat;
  theCtx.arbSampleShading  = checkExtensionShort("GL_ARB_sample_shading");
  theCtx.arbDepthClamp = isGlGreaterEqualShort(3, 2) || checkExtensionShort("GL_ARB_depth_clamp")
                         || checkExtensionShort("NV_depth_clamp");
  theCtx.extBgra   = isGlGreaterEqualShort(1, 2) || checkExtensionShort("GL_EXT_bgra");
  theCtx.extTexR16 = true;
  theCtx.extAnis   = checkExtensionShort("GL_EXT_texture_filter_anisotropic");
  theCtx.extPDS    = checkExtensionShort("GL_EXT_packed_depth_stencil");
  theCtx.atiMem    = checkExtensionShort("GL_ATI_meminfo");
  theCtx.nvxMem    = checkExtensionShort("GL_NVX_gpu_memory_info");

  theCtx.hasDrawBuffers = isGlGreaterEqualShort(2, 0) ? OpenGl_FeatureInCore
                          : theCtx.arbDrawBuffers     ? OpenGl_FeatureInExtensions
                                                      : OpenGl_FeatureNotAvailable;

  theCtx.hasGlslBitwiseOps = isGlGreaterEqualShort(3, 0) ? OpenGl_FeatureInCore
                             : checkExtensionShort("GL_EXT_gpu_shader4")
                               ? OpenGl_FeatureInExtensions
                               : OpenGl_FeatureNotAvailable;

  theCtx.hasFloatBuffer = theCtx.hasHalfFloatBuffer =
    isGlGreaterEqualShort(3, 0)                        ? OpenGl_FeatureInCore
    : checkExtensionShort("GL_ARB_color_buffer_float") ? OpenGl_FeatureInExtensions
                                                       : OpenGl_FeatureNotAvailable;

  theCtx.hasGeometryStage =
    isGlGreaterEqualShort(3, 2) ? OpenGl_FeatureInCore : OpenGl_FeatureNotAvailable;

  theCtx.hasSampleVariables = isGlGreaterEqualShort(4, 0) ? OpenGl_FeatureInCore
                              : theCtx.arbSampleShading   ? OpenGl_FeatureInExtensions
                                                          : OpenGl_FeatureNotAvailable;

  bool has12 = false, has13 = false, has14 = false, has15 = false;
  bool has20 = false, has21 = false;
  bool has30 = false, has31 = false, has32 = false, has33 = false;
  bool has40 = false, has41 = false, has42 = false, has43 = false, has44 = false, has45 = false,
       has46 = false;

  // retrieve platform-dependent extensions
  #if defined(HAVE_EGL)
    //
  #elif defined(_WIN32)
  if (FindProcShort(wglGetExtensionsStringARB))
  {
    const char* aWglExts = this->wglGetExtensionsStringARB(wglGetCurrentDC());
    if (checkExtensionShort(aWglExts, "WGL_EXT_swap_control"))
    {
      FindProcShort(wglSwapIntervalEXT);
    }
    if (checkExtensionShort(aWglExts, "WGL_ARB_pixel_format"))
    {
      FindProcShort(wglChoosePixelFormatARB);
    }
    if (checkExtensionShort(aWglExts, "WGL_ARB_create_context_profile"))
    {
      FindProcShort(wglCreateContextAttribsARB);
    }
    if (checkExtensionShort(aWglExts, "WGL_NV_DX_interop"))
    {
      FindProcShort(wglDXSetResourceShareHandleNV);
      FindProcShort(wglDXOpenDeviceNV);
      FindProcShort(wglDXCloseDeviceNV);
      FindProcShort(wglDXRegisterObjectNV);
      FindProcShort(wglDXUnregisterObjectNV);
      FindProcShort(wglDXObjectAccessNV);
      FindProcShort(wglDXLockObjectsNV);
      FindProcShort(wglDXUnlockObjectsNV);
    }
    if (checkExtensionShort(aWglExts, "WGL_AMD_gpu_association"))
    {
      FindProcShort(wglGetGPUIDsAMD);
      FindProcShort(wglGetGPUInfoAMD);
      FindProcShort(wglGetContextGPUIDAMD);
    }
  }
  #elif defined(HAVE_XLIB)
  const char* aGlxExts = ::glXQueryExtensionsString((Display*)theCtx.myDisplay,
                                                    DefaultScreen((Display*)theCtx.myDisplay));
  if (checkExtensionShort(aGlxExts, "GLX_EXT_swap_control"))
  {
    FindProcShort(glXSwapIntervalEXT);
  }
  if (checkExtensionShort(aGlxExts, "GLX_SGI_swap_control"))
  {
    FindProcShort(glXSwapIntervalSGI);
  }
  if (checkExtensionShort(aGlxExts, "GLX_MESA_query_renderer"))
  {
    FindProcShort(glXQueryRendererIntegerMESA);
    FindProcShort(glXQueryCurrentRendererIntegerMESA);
    FindProcShort(glXQueryRendererStringMESA);
    FindProcShort(glXQueryCurrentRendererStringMESA);
  }
    // extSwapTear = checkExtensionShort (aGlxExts, "GLX_EXT_swap_control_tear");
  #endif

  // load OpenGL 1.2 new functions
  has12 = isGlGreaterEqualShort(1, 2) && FindProcShort(glBlendColor)
          && FindProcShort(glBlendEquation) && FindProcShort(glDrawRangeElements)
          && FindProcShort(glTexImage3D) && FindProcShort(glTexSubImage3D)
          && FindProcShort(glCopyTexSubImage3D);
  if (!has12)
  {
    theCtx.checkWrongVersion(1, 2, aLastFailedProc);
  }

  // load OpenGL 1.3 new functions
  has13 = isGlGreaterEqualShort(1, 3) && FindProcShort(glActiveTexture)
          && FindProcShort(glSampleCoverage) && FindProcShort(glCompressedTexImage3D)
          && FindProcShort(glCompressedTexImage2D) && FindProcShort(glCompressedTexImage1D)
          && FindProcShort(glCompressedTexSubImage3D) && FindProcShort(glCompressedTexSubImage2D)
          && FindProcShort(glCompressedTexSubImage1D) && FindProcShort(glGetCompressedTexImage);
  if (!has13)
  {
    theCtx.checkWrongVersion(1, 3, aLastFailedProc);
  }

  // load OpenGL 1.4 new functions
  has14 = isGlGreaterEqualShort(1, 4) && FindProcShort(glBlendFuncSeparate)
          && FindProcShort(glMultiDrawArrays) && FindProcShort(glMultiDrawElements)
          && FindProcShort(glPointParameterf) && FindProcShort(glPointParameterfv)
          && FindProcShort(glPointParameteri) && FindProcShort(glPointParameteriv);
  if (!has14)
  {
    theCtx.checkWrongVersion(1, 4, aLastFailedProc);
  }

  // load OpenGL 1.5 new functions
  has15 = isGlGreaterEqualShort(1, 5) && FindProcShort(glGenQueries)
          && FindProcShort(glDeleteQueries) && FindProcShort(glIsQuery)
          && FindProcShort(glBeginQuery) && FindProcShort(glEndQuery) && FindProcShort(glGetQueryiv)
          && FindProcShort(glGetQueryObjectiv) && FindProcShort(glGetQueryObjectuiv)
          && FindProcShort(glBindBuffer) && FindProcShort(glDeleteBuffers)
          && FindProcShort(glGenBuffers) && FindProcShort(glIsBuffer) && FindProcShort(glBufferData)
          && FindProcShort(glBufferSubData) && FindProcShort(glGetBufferSubData)
          && FindProcShort(glMapBuffer) && FindProcShort(glUnmapBuffer)
          && FindProcShort(glGetBufferParameteriv) && FindProcShort(glGetBufferPointerv);
  if (has15)
  {
    theCtx.core15           = (OpenGl_GlCore15*)this;
    theCtx.core15fwd        = (OpenGl_GlCore15*)this;
    theCtx.hasGetBufferData = true;
  }
  else
  {
    theCtx.checkWrongVersion(1, 5, aLastFailedProc);
  }

  // load OpenGL 2.0 new functions
  has20 = isGlGreaterEqualShort(2, 0) && FindProcShort(glBlendEquationSeparate)
          && FindProcShort(glDrawBuffers) && FindProcShort(glStencilOpSeparate)
          && FindProcShort(glStencilFuncSeparate) && FindProcShort(glStencilMaskSeparate)
          && FindProcShort(glAttachShader) && FindProcShort(glBindAttribLocation)
          && FindProcShort(glCompileShader) && FindProcShort(glCreateProgram)
          && FindProcShort(glCreateShader) && FindProcShort(glDeleteProgram)
          && FindProcShort(glDeleteShader) && FindProcShort(glDetachShader)
          && FindProcShort(glDisableVertexAttribArray) && FindProcShort(glEnableVertexAttribArray)
          && FindProcShort(glGetActiveAttrib) && FindProcShort(glGetActiveUniform)
          && FindProcShort(glGetAttachedShaders) && FindProcShort(glGetAttribLocation)
          && FindProcShort(glGetProgramiv) && FindProcShort(glGetProgramInfoLog)
          && FindProcShort(glGetShaderiv) && FindProcShort(glGetShaderInfoLog)
          && FindProcShort(glGetShaderSource) && FindProcShort(glGetUniformLocation)
          && FindProcShort(glGetUniformfv) && FindProcShort(glGetUniformiv)
          && FindProcShort(glGetVertexAttribdv) && FindProcShort(glGetVertexAttribfv)
          && FindProcShort(glGetVertexAttribiv) && FindProcShort(glGetVertexAttribPointerv)
          && FindProcShort(glIsProgram) && FindProcShort(glIsShader) && FindProcShort(glLinkProgram)
          && FindProcShort(glShaderSource) && FindProcShort(glUseProgram)
          && FindProcShort(glUniform1f) && FindProcShort(glUniform2f) && FindProcShort(glUniform3f)
          && FindProcShort(glUniform4f) && FindProcShort(glUniform1i) && FindProcShort(glUniform2i)
          && FindProcShort(glUniform3i) && FindProcShort(glUniform4i) && FindProcShort(glUniform1fv)
          && FindProcShort(glUniform2fv) && FindProcShort(glUniform3fv)
          && FindProcShort(glUniform4fv) && FindProcShort(glUniform1iv)
          && FindProcShort(glUniform2iv) && FindProcShort(glUniform3iv)
          && FindProcShort(glUniform4iv) && FindProcShort(glUniformMatrix2fv)
          && FindProcShort(glUniformMatrix3fv) && FindProcShort(glUniformMatrix4fv)
          && FindProcShort(glValidateProgram) && FindProcShort(glVertexAttrib1d)
          && FindProcShort(glVertexAttrib1dv) && FindProcShort(glVertexAttrib1f)
          && FindProcShort(glVertexAttrib1fv) && FindProcShort(glVertexAttrib1s)
          && FindProcShort(glVertexAttrib1sv) && FindProcShort(glVertexAttrib2d)
          && FindProcShort(glVertexAttrib2dv) && FindProcShort(glVertexAttrib2f)
          && FindProcShort(glVertexAttrib2fv) && FindProcShort(glVertexAttrib2s)
          && FindProcShort(glVertexAttrib2sv) && FindProcShort(glVertexAttrib3d)
          && FindProcShort(glVertexAttrib3dv) && FindProcShort(glVertexAttrib3f)
          && FindProcShort(glVertexAttrib3fv) && FindProcShort(glVertexAttrib3s)
          && FindProcShort(glVertexAttrib3sv) && FindProcShort(glVertexAttrib4Nbv)
          && FindProcShort(glVertexAttrib4Niv) && FindProcShort(glVertexAttrib4Nsv)
          && FindProcShort(glVertexAttrib4Nub) && FindProcShort(glVertexAttrib4Nubv)
          && FindProcShort(glVertexAttrib4Nuiv) && FindProcShort(glVertexAttrib4Nusv)
          && FindProcShort(glVertexAttrib4bv) && FindProcShort(glVertexAttrib4d)
          && FindProcShort(glVertexAttrib4dv) && FindProcShort(glVertexAttrib4f)
          && FindProcShort(glVertexAttrib4fv) && FindProcShort(glVertexAttrib4iv)
          && FindProcShort(glVertexAttrib4s) && FindProcShort(glVertexAttrib4sv)
          && FindProcShort(glVertexAttrib4ubv) && FindProcShort(glVertexAttrib4uiv)
          && FindProcShort(glVertexAttrib4usv) && FindProcShort(glVertexAttribPointer);
  if (has20)
  {
    const char* aGlslVer = (const char*)::glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (aGlslVer == NULL || *aGlslVer == '\0')
    {
      // broken context has been detected
      theCtx.checkWrongVersion(2, 0, "GLSL 1.1");
    }
    else
    {
      theCtx.core20    = (OpenGl_GlCore20*)this;
      theCtx.core20fwd = (OpenGl_GlCore20*)this;
    }
  }
  else
  {
    theCtx.checkWrongVersion(2, 0, aLastFailedProc);
  }

  // load OpenGL 2.1 new functions
  has21 = isGlGreaterEqualShort(2, 1) && FindProcShort(glUniformMatrix2x3fv)
          && FindProcShort(glUniformMatrix3x2fv) && FindProcShort(glUniformMatrix2x4fv)
          && FindProcShort(glUniformMatrix4x2fv) && FindProcShort(glUniformMatrix3x4fv)
          && FindProcShort(glUniformMatrix4x3fv);
  if (!has21)
  {
    theCtx.checkWrongVersion(2, 1, aLastFailedProc);
  }

  // load GL_ARB_framebuffer_object (added to OpenGL 3.0 core)
  const bool hasFBO =
    (isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_ARB_framebuffer_object"))
    && FindProcShort(glIsRenderbuffer) && FindProcShort(glBindRenderbuffer)
    && FindProcShort(glDeleteRenderbuffers) && FindProcShort(glGenRenderbuffers)
    && FindProcShort(glRenderbufferStorage) && FindProcShort(glGetRenderbufferParameteriv)
    && FindProcShort(glIsFramebuffer) && FindProcShort(glBindFramebuffer)
    && FindProcShort(glDeleteFramebuffers) && FindProcShort(glGenFramebuffers)
    && FindProcShort(glCheckFramebufferStatus) && FindProcShort(glFramebufferTexture1D)
    && FindProcShort(glFramebufferTexture2D) && FindProcShort(glFramebufferTexture3D)
    && FindProcShort(glFramebufferRenderbuffer)
    && FindProcShort(glGetFramebufferAttachmentParameteriv) && FindProcShort(glGenerateMipmap)
    && FindProcShort(glBlitFramebuffer) && FindProcShort(glRenderbufferStorageMultisample)
    && FindProcShort(glFramebufferTextureLayer);

  // load GL_ARB_vertex_array_object (added to OpenGL 3.0 core)
  const bool hasVAO =
    (isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_ARB_vertex_array_object"))
    && FindProcShort(glBindVertexArray) && FindProcShort(glDeleteVertexArrays)
    && FindProcShort(glGenVertexArrays) && FindProcShort(glIsVertexArray);

  // load GL_ARB_map_buffer_range (added to OpenGL 3.0 core)
  const bool hasMapBufferRange =
    (isGlGreaterEqualShort(3, 0) || checkExtensionShort("GL_ARB_map_buffer_range"))
    && FindProcShort(glMapBufferRange) && FindProcShort(glFlushMappedBufferRange);

  // load OpenGL 3.0 new functions
  has30 = isGlGreaterEqualShort(3, 0) && hasFBO && hasVAO && hasMapBufferRange
          && FindProcShort(glColorMaski) && FindProcShort(glGetBooleani_v)
          && FindProcShort(glGetIntegeri_v) && FindProcShort(glEnablei) && FindProcShort(glDisablei)
          && FindProcShort(glIsEnabledi) && FindProcShort(glBeginTransformFeedback)
          && FindProcShort(glEndTransformFeedback) && FindProcShort(glBindBufferRange)
          && FindProcShort(glBindBufferBase) && FindProcShort(glTransformFeedbackVaryings)
          && FindProcShort(glGetTransformFeedbackVarying) && FindProcShort(glClampColor)
          && FindProcShort(glBeginConditionalRender) && FindProcShort(glEndConditionalRender)
          && FindProcShort(glVertexAttribIPointer) && FindProcShort(glGetVertexAttribIiv)
          && FindProcShort(glGetVertexAttribIuiv) && FindProcShort(glVertexAttribI1i)
          && FindProcShort(glVertexAttribI2i) && FindProcShort(glVertexAttribI3i)
          && FindProcShort(glVertexAttribI4i) && FindProcShort(glVertexAttribI1ui)
          && FindProcShort(glVertexAttribI2ui) && FindProcShort(glVertexAttribI3ui)
          && FindProcShort(glVertexAttribI4ui) && FindProcShort(glVertexAttribI1iv)
          && FindProcShort(glVertexAttribI2iv) && FindProcShort(glVertexAttribI3iv)
          && FindProcShort(glVertexAttribI4iv) && FindProcShort(glVertexAttribI1uiv)
          && FindProcShort(glVertexAttribI2uiv) && FindProcShort(glVertexAttribI3uiv)
          && FindProcShort(glVertexAttribI4uiv) && FindProcShort(glVertexAttribI4bv)
          && FindProcShort(glVertexAttribI4sv) && FindProcShort(glVertexAttribI4ubv)
          && FindProcShort(glVertexAttribI4usv) && FindProcShort(glGetUniformuiv)
          && FindProcShort(glBindFragDataLocation) && FindProcShort(glGetFragDataLocation)
          && FindProcShort(glUniform1ui) && FindProcShort(glUniform2ui)
          && FindProcShort(glUniform3ui) && FindProcShort(glUniform4ui)
          && FindProcShort(glUniform1uiv) && FindProcShort(glUniform2uiv)
          && FindProcShort(glUniform3uiv) && FindProcShort(glUniform4uiv)
          && FindProcShort(glTexParameterIiv) && FindProcShort(glTexParameterIuiv)
          && FindProcShort(glGetTexParameterIiv) && FindProcShort(glGetTexParameterIuiv)
          && FindProcShort(glClearBufferiv) && FindProcShort(glClearBufferuiv)
          && FindProcShort(glClearBufferfv) && FindProcShort(glClearBufferfi)
          && FindProcShort(glGetStringi);
  if (!has30)
  {
    theCtx.checkWrongVersion(3, 0, aLastFailedProc);
  }

  // load GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)
  const bool hasUBO =
    (isGlGreaterEqualShort(3, 1) || checkExtensionShort("GL_ARB_uniform_buffer_object"))
    && FindProcShort(glGetUniformIndices) && FindProcShort(glGetActiveUniformsiv)
    && FindProcShort(glGetActiveUniformName) && FindProcShort(glGetUniformBlockIndex)
    && FindProcShort(glGetActiveUniformBlockiv) && FindProcShort(glGetActiveUniformBlockName)
    && FindProcShort(glUniformBlockBinding);

  // load GL_ARB_copy_buffer (added to OpenGL 3.1 core)
  const bool hasCopyBufSubData =
    (isGlGreaterEqualShort(3, 1) || checkExtensionShort("GL_ARB_copy_buffer"))
    && FindProcShort(glCopyBufferSubData);

  if (has30)
  {
    // NPOT textures are required by OpenGL 2.0 specifications
    // but doesn't hardware accelerated by some ancient OpenGL 2.1 hardware (GeForce FX, RadeOn 9700
    // etc.)
    theCtx.arbNPTW  = true;
    theCtx.arbTexRG = true;

    theCtx.core30 = (OpenGl_GlCore30*)this;
  }

  // load OpenGL 3.1 new functions
  has31 = isGlGreaterEqualShort(3, 1) && hasUBO && hasCopyBufSubData
          && FindProcShort(glDrawArraysInstanced) && FindProcShort(glDrawElementsInstanced)
          && FindProcShort(glTexBuffer) && FindProcShort(glPrimitiveRestartIndex);
  if (has31)
  {
    theCtx.arbTBO = (OpenGl_ArbTBO*)this;
    theCtx.arbIns = (OpenGl_ArbIns*)this;
  }
  else
  {
    theCtx.checkWrongVersion(3, 1, aLastFailedProc);

    // initialize TBO extension (ARB)
    if (checkExtensionShort("GL_ARB_texture_buffer_object")
        && theCtx.FindProc("glTexBufferARB", this->glTexBuffer))
    {
      theCtx.arbTBO = (OpenGl_ArbTBO*)this;
    }

    // initialize hardware instancing extension (ARB)
    if (checkExtensionShort("GL_ARB_draw_instanced")
        && theCtx.FindProc("glDrawArraysInstancedARB", this->glDrawArraysInstanced)
        && theCtx.FindProc("glDrawElementsInstancedARB", this->glDrawElementsInstanced))
    {
      theCtx.arbIns = (OpenGl_ArbIns*)this;
    }
  }

  theCtx.arbTboRGB32 = checkExtensionShort("GL_ARB_texture_buffer_object_rgb32");

  // load GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)
  const bool hasDrawElemsBaseVert =
    (isGlGreaterEqualShort(3, 2) || checkExtensionShort("GL_ARB_draw_elements_base_vertex"))
    && FindProcShort(glDrawElementsBaseVertex) && FindProcShort(glDrawRangeElementsBaseVertex)
    && FindProcShort(glDrawElementsInstancedBaseVertex)
    && FindProcShort(glMultiDrawElementsBaseVertex);

  // load GL_ARB_provoking_vertex (added to OpenGL 3.2 core)
  const bool hasProvokingVert =
    (isGlGreaterEqualShort(3, 2) || checkExtensionShort("GL_ARB_provoking_vertex"))
    && FindProcShort(glProvokingVertex);

  // load GL_ARB_sync (added to OpenGL 3.2 core)
  const bool hasSync = (isGlGreaterEqualShort(3, 2) || checkExtensionShort("GL_ARB_sync"))
                       && FindProcShort(glFenceSync) && FindProcShort(glIsSync)
                       && FindProcShort(glDeleteSync) && FindProcShort(glClientWaitSync)
                       && FindProcShort(glWaitSync) && FindProcShort(glGetInteger64v)
                       && FindProcShort(glGetSynciv);

  // load GL_ARB_texture_multisample (added to OpenGL 3.2 core)
  const bool hasTextureMultisample =
    (isGlGreaterEqualShort(3, 2) || checkExtensionShort("GL_ARB_texture_multisample"))
    && FindProcShort(glTexImage2DMultisample) && FindProcShort(glTexImage3DMultisample)
    && FindProcShort(glGetMultisamplefv) && FindProcShort(glSampleMaski);

  // load OpenGL 3.2 new functions
  has32 = isGlGreaterEqualShort(3, 2) && hasDrawElemsBaseVert && hasProvokingVert && hasSync
          && hasTextureMultisample && FindProcShort(glGetInteger64i_v)
          && FindProcShort(glGetBufferParameteri64v) && FindProcShort(glFramebufferTexture);
  if (has32)
  {
    theCtx.core32 = (OpenGl_GlCore32*)this;
  }
  else
  {
    theCtx.checkWrongVersion(3, 2, aLastFailedProc);
  }

  // load GL_ARB_blend_func_extended (added to OpenGL 3.3 core)
  const bool hasBlendFuncExtended =
    (isGlGreaterEqualShort(3, 3) || checkExtensionShort("GL_ARB_blend_func_extended"))
    && FindProcShort(glBindFragDataLocationIndexed) && FindProcShort(glGetFragDataIndex);

  // load GL_ARB_sampler_objects (added to OpenGL 3.3 core)
  const bool hasSamplerObjects =
    (isGlGreaterEqualShort(3, 3) || checkExtensionShort("GL_ARB_sampler_objects"))
    && FindProcShort(glGenSamplers) && FindProcShort(glDeleteSamplers) && FindProcShort(glIsSampler)
    && FindProcShort(glBindSampler) && FindProcShort(glSamplerParameteri)
    && FindProcShort(glSamplerParameteriv) && FindProcShort(glSamplerParameterf)
    && FindProcShort(glSamplerParameterfv) && FindProcShort(glSamplerParameterIiv)
    && FindProcShort(glSamplerParameterIuiv) && FindProcShort(glGetSamplerParameteriv)
    && FindProcShort(glGetSamplerParameterIiv) && FindProcShort(glGetSamplerParameterfv)
    && FindProcShort(glGetSamplerParameterIuiv);
  if (hasSamplerObjects)
  {
    theCtx.arbSamplerObject = (OpenGl_ArbSamplerObject*)this;
  }

  // load GL_ARB_timer_query (added to OpenGL 3.3 core)
  const bool hasTimerQuery =
    (isGlGreaterEqualShort(3, 3) || checkExtensionShort("GL_ARB_timer_query"))
    && FindProcShort(glQueryCounter) && FindProcShort(glGetQueryObjecti64v)
    && FindProcShort(glGetQueryObjectui64v);

  // load GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)
  const bool hasVertType21010101rev =
    (isGlGreaterEqualShort(3, 3) || checkExtensionShort("GL_ARB_vertex_type_2_10_10_10_rev"))
    && FindProcShort(glVertexAttribP1ui) && FindProcShort(glVertexAttribP1uiv)
    && FindProcShort(glVertexAttribP2ui) && FindProcShort(glVertexAttribP2uiv)
    && FindProcShort(glVertexAttribP3ui) && FindProcShort(glVertexAttribP3uiv)
    && FindProcShort(glVertexAttribP4ui) && FindProcShort(glVertexAttribP4uiv);

  // load OpenGL 3.3 extra functions
  has33 = isGlGreaterEqualShort(3, 3) && hasBlendFuncExtended && hasSamplerObjects && hasTimerQuery
          && hasVertType21010101rev && FindProcShort(glVertexAttribDivisor);
  if (has33)
  {
    theCtx.core33 = (OpenGl_GlCore33*)this;
  }
  else
  {
    theCtx.checkWrongVersion(3, 3, aLastFailedProc);
  }

  // load GL_ARB_draw_indirect (added to OpenGL 4.0 core)
  const bool hasDrawIndirect =
    (isGlGreaterEqualShort(4, 0) || checkExtensionShort("GL_ARB_draw_indirect"))
    && FindProcShort(glDrawArraysIndirect) && FindProcShort(glDrawElementsIndirect);

  // load GL_ARB_gpu_shader_fp64 (added to OpenGL 4.0 core)
  const bool hasShaderFP64 =
    (isGlGreaterEqualShort(4, 0) || checkExtensionShort("GL_ARB_gpu_shader_fp64"))
    && FindProcShort(glUniform1d) && FindProcShort(glUniform2d) && FindProcShort(glUniform3d)
    && FindProcShort(glUniform4d) && FindProcShort(glUniform1dv) && FindProcShort(glUniform2dv)
    && FindProcShort(glUniform3dv) && FindProcShort(glUniform4dv)
    && FindProcShort(glUniformMatrix2dv) && FindProcShort(glUniformMatrix3dv)
    && FindProcShort(glUniformMatrix4dv) && FindProcShort(glUniformMatrix2x3dv)
    && FindProcShort(glUniformMatrix2x4dv) && FindProcShort(glUniformMatrix3x2dv)
    && FindProcShort(glUniformMatrix3x4dv) && FindProcShort(glUniformMatrix4x2dv)
    && FindProcShort(glUniformMatrix4x3dv) && FindProcShort(glGetUniformdv);

  // load GL_ARB_shader_subroutine (added to OpenGL 4.0 core)
  const bool hasShaderSubroutine =
    (isGlGreaterEqualShort(4, 0) || checkExtensionShort("GL_ARB_shader_subroutine"))
    && FindProcShort(glGetSubroutineUniformLocation) && FindProcShort(glGetSubroutineIndex)
    && FindProcShort(glGetActiveSubroutineUniformiv)
    && FindProcShort(glGetActiveSubroutineUniformName) && FindProcShort(glGetActiveSubroutineName)
    && FindProcShort(glUniformSubroutinesuiv) && FindProcShort(glGetUniformSubroutineuiv)
    && FindProcShort(glGetProgramStageiv);

  // load GL_ARB_tessellation_shader (added to OpenGL 4.0 core)
  const bool hasTessellationShader =
    (isGlGreaterEqualShort(4, 0) || checkExtensionShort("GL_ARB_tessellation_shader"))
    && FindProcShort(glPatchParameteri) && FindProcShort(glPatchParameterfv);

  // load GL_ARB_transform_feedback2 (added to OpenGL 4.0 core)
  const bool hasTrsfFeedback2 =
    (isGlGreaterEqualShort(4, 0) || checkExtensionShort("GL_ARB_transform_feedback2"))
    && FindProcShort(glBindTransformFeedback) && FindProcShort(glDeleteTransformFeedbacks)
    && FindProcShort(glGenTransformFeedbacks) && FindProcShort(glIsTransformFeedback)
    && FindProcShort(glPauseTransformFeedback) && FindProcShort(glResumeTransformFeedback)
    && FindProcShort(glDrawTransformFeedback);

  // load GL_ARB_transform_feedback3 (added to OpenGL 4.0 core)
  const bool hasTrsfFeedback3 =
    (isGlGreaterEqualShort(4, 0) || checkExtensionShort("GL_ARB_transform_feedback3"))
    && FindProcShort(glDrawTransformFeedbackStream) && FindProcShort(glBeginQueryIndexed)
    && FindProcShort(glEndQueryIndexed) && FindProcShort(glGetQueryIndexediv);

  // load OpenGL 4.0 new functions
  has40 = isGlGreaterEqualShort(4, 0) && hasDrawIndirect && hasShaderFP64 && hasShaderSubroutine
          && hasTessellationShader && hasTrsfFeedback2 && hasTrsfFeedback3
          && FindProcShort(glMinSampleShading) && FindProcShort(glBlendEquationi)
          && FindProcShort(glBlendEquationSeparatei) && FindProcShort(glBlendFunci)
          && FindProcShort(glBlendFuncSeparatei);
  if (has40)
  {
    theCtx.arbTboRGB32 = true; // in core since OpenGL 4.0
  }
  else
  {
    theCtx.checkWrongVersion(4, 0, aLastFailedProc);
  }

  // load GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)
  const bool hasES2Compatibility =
    (isGlGreaterEqualShort(4, 1) || checkExtensionShort("GL_ARB_ES2_compatibility"))
    && FindProcShort(glReleaseShaderCompiler) && FindProcShort(glShaderBinary)
    && FindProcShort(glGetShaderPrecisionFormat) && FindProcShort(glDepthRangef)
    && FindProcShort(glClearDepthf);

  // load GL_ARB_get_program_binary (added to OpenGL 4.1 core)
  const bool hasGetProgramBinary =
    (isGlGreaterEqualShort(4, 1) || checkExtensionShort("GL_ARB_get_program_binary"))
    && FindProcShort(glGetProgramBinary) && FindProcShort(glProgramBinary)
    && FindProcShort(glProgramParameteri);

  // load GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)
  const bool hasSeparateShaderObjects =
    (isGlGreaterEqualShort(4, 1) || checkExtensionShort("GL_ARB_separate_shader_objects"))
    && FindProcShort(glUseProgramStages) && FindProcShort(glActiveShaderProgram)
    && FindProcShort(glCreateShaderProgramv) && FindProcShort(glBindProgramPipeline)
    && FindProcShort(glDeleteProgramPipelines) && FindProcShort(glGenProgramPipelines)
    && FindProcShort(glIsProgramPipeline) && FindProcShort(glGetProgramPipelineiv)
    && FindProcShort(glProgramUniform1i) && FindProcShort(glProgramUniform1iv)
    && FindProcShort(glProgramUniform1f) && FindProcShort(glProgramUniform1fv)
    && FindProcShort(glProgramUniform1d) && FindProcShort(glProgramUniform1dv)
    && FindProcShort(glProgramUniform1ui) && FindProcShort(glProgramUniform1uiv)
    && FindProcShort(glProgramUniform2i) && FindProcShort(glProgramUniform2iv)
    && FindProcShort(glProgramUniform2f) && FindProcShort(glProgramUniform2fv)
    && FindProcShort(glProgramUniform2d) && FindProcShort(glProgramUniform2dv)
    && FindProcShort(glProgramUniform2ui) && FindProcShort(glProgramUniform2uiv)
    && FindProcShort(glProgramUniform3i) && FindProcShort(glProgramUniform3iv)
    && FindProcShort(glProgramUniform3f) && FindProcShort(glProgramUniform3fv)
    && FindProcShort(glProgramUniform3d) && FindProcShort(glProgramUniform3dv)
    && FindProcShort(glProgramUniform3ui) && FindProcShort(glProgramUniform3uiv)
    && FindProcShort(glProgramUniform4i) && FindProcShort(glProgramUniform4iv)
    && FindProcShort(glProgramUniform4f) && FindProcShort(glProgramUniform4fv)
    && FindProcShort(glProgramUniform4d) && FindProcShort(glProgramUniform4dv)
    && FindProcShort(glProgramUniform4ui) && FindProcShort(glProgramUniform4uiv)
    && FindProcShort(glProgramUniformMatrix2fv) && FindProcShort(glProgramUniformMatrix3fv)
    && FindProcShort(glProgramUniformMatrix4fv) && FindProcShort(glProgramUniformMatrix2dv)
    && FindProcShort(glProgramUniformMatrix3dv) && FindProcShort(glProgramUniformMatrix4dv)
    && FindProcShort(glProgramUniformMatrix2x3fv) && FindProcShort(glProgramUniformMatrix3x2fv)
    && FindProcShort(glProgramUniformMatrix2x4fv) && FindProcShort(glProgramUniformMatrix4x2fv)
    && FindProcShort(glProgramUniformMatrix3x4fv) && FindProcShort(glProgramUniformMatrix4x3fv)
    && FindProcShort(glProgramUniformMatrix2x3dv) && FindProcShort(glProgramUniformMatrix3x2dv)
    && FindProcShort(glProgramUniformMatrix2x4dv) && FindProcShort(glProgramUniformMatrix4x2dv)
    && FindProcShort(glProgramUniformMatrix3x4dv) && FindProcShort(glProgramUniformMatrix4x3dv)
    && FindProcShort(glValidateProgramPipeline) && FindProcShort(glGetProgramPipelineInfoLog);

  // load GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)
  const bool hasVertAttrib64bit =
    (isGlGreaterEqualShort(4, 1) || checkExtensionShort("GL_ARB_vertex_attrib_64bit"))
    && FindProcShort(glVertexAttribL1d) && FindProcShort(glVertexAttribL2d)
    && FindProcShort(glVertexAttribL3d) && FindProcShort(glVertexAttribL4d)
    && FindProcShort(glVertexAttribL1dv) && FindProcShort(glVertexAttribL2dv)
    && FindProcShort(glVertexAttribL3dv) && FindProcShort(glVertexAttribL4dv)
    && FindProcShort(glVertexAttribLPointer) && FindProcShort(glGetVertexAttribLdv);

  // load GL_ARB_viewport_array (added to OpenGL 4.1 core)
  const bool hasViewportArray =
    (isGlGreaterEqualShort(4, 1) || checkExtensionShort("GL_ARB_viewport_array"))
    && FindProcShort(glViewportArrayv) && FindProcShort(glViewportIndexedf)
    && FindProcShort(glViewportIndexedfv) && FindProcShort(glScissorArrayv)
    && FindProcShort(glScissorIndexed) && FindProcShort(glScissorIndexedv)
    && FindProcShort(glDepthRangeArrayv) && FindProcShort(glDepthRangeIndexed)
    && FindProcShort(glGetFloati_v) && FindProcShort(glGetDoublei_v);

  has41 = isGlGreaterEqualShort(4, 1) && hasES2Compatibility && hasGetProgramBinary
          && hasSeparateShaderObjects && hasVertAttrib64bit && hasViewportArray;
  if (has41)
  {
    theCtx.core41 = (OpenGl_GlCore41*)this;
  }
  else
  {
    theCtx.checkWrongVersion(4, 1, aLastFailedProc);
  }

  // load GL_ARB_base_instance (added to OpenGL 4.2 core)
  const bool hasBaseInstance =
    (isGlGreaterEqualShort(4, 2) || checkExtensionShort("GL_ARB_base_instance"))
    && FindProcShort(glDrawArraysInstancedBaseInstance)
    && FindProcShort(glDrawElementsInstancedBaseInstance)
    && FindProcShort(glDrawElementsInstancedBaseVertexBaseInstance);

  // load GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)
  const bool hasTrsfFeedbackInstanced =
    (isGlGreaterEqualShort(4, 2) || checkExtensionShort("GL_ARB_transform_feedback_instanced"))
    && FindProcShort(glDrawTransformFeedbackInstanced)
    && FindProcShort(glDrawTransformFeedbackStreamInstanced);

  // load GL_ARB_internalformat_query (added to OpenGL 4.2 core)
  const bool hasInternalFormatQuery =
    (isGlGreaterEqualShort(4, 2) || checkExtensionShort("GL_ARB_internalformat_query"))
    && FindProcShort(glGetInternalformativ);

  // load GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)
  const bool hasShaderAtomicCounters =
    (isGlGreaterEqualShort(4, 2) || checkExtensionShort("GL_ARB_shader_atomic_counters"))
    && FindProcShort(glGetActiveAtomicCounterBufferiv);

  // load GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)
  const bool hasShaderImgLoadStore =
    (isGlGreaterEqualShort(4, 2) || checkExtensionShort("GL_ARB_shader_image_load_store"))
    && FindProcShort(glBindImageTexture) && FindProcShort(glMemoryBarrier);

  // load GL_ARB_texture_storage (added to OpenGL 4.2 core)
  const bool hasTextureStorage =
    (isGlGreaterEqualShort(4, 2) || checkExtensionShort("GL_ARB_texture_storage"))
    && FindProcShort(glTexStorage1D) && FindProcShort(glTexStorage2D)
    && FindProcShort(glTexStorage3D);

  has42 = isGlGreaterEqualShort(4, 2) && hasBaseInstance && hasTrsfFeedbackInstanced
          && hasInternalFormatQuery && hasShaderAtomicCounters && hasShaderImgLoadStore
          && hasTextureStorage;
  if (has42)
  {
    theCtx.core42 = (OpenGl_GlCore42*)this;
  }
  else
  {
    theCtx.checkWrongVersion(4, 2, aLastFailedProc);
  }

  has43 = isGlGreaterEqualShort(4, 3) && FindProcShort(glClearBufferData)
          && FindProcShort(glClearBufferSubData) && FindProcShort(glDispatchCompute)
          && FindProcShort(glDispatchComputeIndirect) && FindProcShort(glCopyImageSubData)
          && FindProcShort(glFramebufferParameteri) && FindProcShort(glGetFramebufferParameteriv)
          && FindProcShort(glGetInternalformati64v) && FindProcShort(glInvalidateTexSubImage)
          && FindProcShort(glInvalidateTexImage) && FindProcShort(glInvalidateBufferSubData)
          && FindProcShort(glInvalidateBufferData) && FindProcShort(glInvalidateFramebuffer)
          && FindProcShort(glInvalidateSubFramebuffer) && FindProcShort(glMultiDrawArraysIndirect)
          && FindProcShort(glMultiDrawElementsIndirect) && FindProcShort(glGetProgramInterfaceiv)
          && FindProcShort(glGetProgramResourceIndex) && FindProcShort(glGetProgramResourceName)
          && FindProcShort(glGetProgramResourceiv) && FindProcShort(glGetProgramResourceLocation)
          && FindProcShort(glGetProgramResourceLocationIndex)
          && FindProcShort(glShaderStorageBlockBinding) && FindProcShort(glTexBufferRange)
          && FindProcShort(glTexStorage2DMultisample) && FindProcShort(glTexStorage3DMultisample)
          && FindProcShort(glTextureView) && FindProcShort(glBindVertexBuffer)
          && FindProcShort(glVertexAttribFormat) && FindProcShort(glVertexAttribIFormat)
          && FindProcShort(glVertexAttribLFormat) && FindProcShort(glVertexAttribBinding)
          && FindProcShort(glVertexBindingDivisor) && FindProcShort(glDebugMessageControl)
          && FindProcShort(glDebugMessageInsert) && FindProcShort(glDebugMessageCallback)
          && FindProcShort(glGetDebugMessageLog) && FindProcShort(glPushDebugGroup)
          && FindProcShort(glPopDebugGroup) && FindProcShort(glObjectLabel)
          && FindProcShort(glGetObjectLabel) && FindProcShort(glObjectPtrLabel)
          && FindProcShort(glGetObjectPtrLabel);
  if (has43)
  {
    theCtx.core43 = (OpenGl_GlCore43*)this;
  }
  else
  {
    theCtx.checkWrongVersion(4, 3, aLastFailedProc);
  }

  // load GL_ARB_clear_texture (added to OpenGL 4.4 core)
  bool arbTexClear = (isGlGreaterEqualShort(4, 4) || checkExtensionShort("GL_ARB_clear_texture"))
                     && FindProcShort(glClearTexImage) && FindProcShort(glClearTexSubImage);

  has44 = isGlGreaterEqualShort(4, 4) && arbTexClear && FindProcShort(glBufferStorage)
          && FindProcShort(glBindBuffersBase) && FindProcShort(glBindBuffersRange)
          && FindProcShort(glBindTextures) && FindProcShort(glBindSamplers)
          && FindProcShort(glBindImageTextures) && FindProcShort(glBindVertexBuffers);
  if (has44)
  {
    theCtx.core44 = (OpenGl_GlCore44*)this;
  }
  else
  {
    theCtx.checkWrongVersion(4, 4, aLastFailedProc);
  }

  has45 =
    isGlGreaterEqualShort(4, 5) && FindProcShort(glBindVertexBuffers)
    && FindProcShort(glClipControl) && FindProcShort(glCreateTransformFeedbacks)
    && FindProcShort(glTransformFeedbackBufferBase) && FindProcShort(glTransformFeedbackBufferRange)
    && FindProcShort(glGetTransformFeedbackiv) && FindProcShort(glGetTransformFeedbacki_v)
    && FindProcShort(glGetTransformFeedbacki64_v) && FindProcShort(glCreateBuffers)
    && FindProcShort(glNamedBufferStorage) && FindProcShort(glNamedBufferData)
    && FindProcShort(glNamedBufferSubData) && FindProcShort(glCopyNamedBufferSubData)
    && FindProcShort(glClearNamedBufferData) && FindProcShort(glClearNamedBufferSubData)
    && FindProcShort(glMapNamedBuffer) && FindProcShort(glMapNamedBufferRange)
    && FindProcShort(glUnmapNamedBuffer) && FindProcShort(glFlushMappedNamedBufferRange)
    && FindProcShort(glGetNamedBufferParameteriv) && FindProcShort(glGetNamedBufferParameteri64v)
    && FindProcShort(glGetNamedBufferPointerv) && FindProcShort(glGetNamedBufferSubData)
    && FindProcShort(glCreateFramebuffers) && FindProcShort(glNamedFramebufferRenderbuffer)
    && FindProcShort(glNamedFramebufferParameteri) && FindProcShort(glNamedFramebufferTexture)
    && FindProcShort(glNamedFramebufferTextureLayer) && FindProcShort(glNamedFramebufferDrawBuffer)
    && FindProcShort(glNamedFramebufferDrawBuffers) && FindProcShort(glNamedFramebufferReadBuffer)
    && FindProcShort(glInvalidateNamedFramebufferData)
    && FindProcShort(glInvalidateNamedFramebufferSubData)
    && FindProcShort(glClearNamedFramebufferiv) && FindProcShort(glClearNamedFramebufferuiv)
    && FindProcShort(glClearNamedFramebufferfv) && FindProcShort(glClearNamedFramebufferfi)
    && FindProcShort(glBlitNamedFramebuffer) && FindProcShort(glCheckNamedFramebufferStatus)
    && FindProcShort(glGetNamedFramebufferParameteriv)
    && FindProcShort(glGetNamedFramebufferAttachmentParameteriv)
    && FindProcShort(glCreateRenderbuffers) && FindProcShort(glNamedRenderbufferStorage)
    && FindProcShort(glNamedRenderbufferStorageMultisample)
    && FindProcShort(glGetNamedRenderbufferParameteriv) && FindProcShort(glCreateTextures)
    && FindProcShort(glTextureBuffer) && FindProcShort(glTextureBufferRange)
    && FindProcShort(glTextureStorage1D) && FindProcShort(glTextureStorage2D)
    && FindProcShort(glTextureStorage3D) && FindProcShort(glTextureStorage2DMultisample)
    && FindProcShort(glTextureStorage3DMultisample) && FindProcShort(glTextureSubImage1D)
    && FindProcShort(glTextureSubImage2D) && FindProcShort(glTextureSubImage3D)
    && FindProcShort(glCompressedTextureSubImage1D) && FindProcShort(glCompressedTextureSubImage2D)
    && FindProcShort(glCompressedTextureSubImage3D) && FindProcShort(glCopyTextureSubImage1D)
    && FindProcShort(glCopyTextureSubImage2D) && FindProcShort(glCopyTextureSubImage3D)
    && FindProcShort(glTextureParameterf) && FindProcShort(glTextureParameterfv)
    && FindProcShort(glTextureParameteri) && FindProcShort(glTextureParameterIiv)
    && FindProcShort(glTextureParameterIuiv) && FindProcShort(glTextureParameteriv)
    && FindProcShort(glGenerateTextureMipmap) && FindProcShort(glBindTextureUnit)
    && FindProcShort(glGetTextureImage) && FindProcShort(glGetCompressedTextureImage)
    && FindProcShort(glGetTextureLevelParameterfv) && FindProcShort(glGetTextureLevelParameteriv)
    && FindProcShort(glGetTextureParameterfv) && FindProcShort(glGetTextureParameterIiv)
    && FindProcShort(glGetTextureParameterIuiv) && FindProcShort(glGetTextureParameteriv)
    && FindProcShort(glCreateVertexArrays) && FindProcShort(glDisableVertexArrayAttrib)
    && FindProcShort(glEnableVertexArrayAttrib) && FindProcShort(glVertexArrayElementBuffer)
    && FindProcShort(glVertexArrayVertexBuffer) && FindProcShort(glVertexArrayVertexBuffers)
    && FindProcShort(glVertexArrayAttribBinding) && FindProcShort(glVertexArrayAttribFormat)
    && FindProcShort(glVertexArrayAttribIFormat) && FindProcShort(glVertexArrayAttribLFormat)
    && FindProcShort(glVertexArrayBindingDivisor) && FindProcShort(glGetVertexArrayiv)
    && FindProcShort(glGetVertexArrayIndexediv) && FindProcShort(glGetVertexArrayIndexed64iv)
    && FindProcShort(glCreateSamplers) && FindProcShort(glCreateProgramPipelines)
    && FindProcShort(glCreateQueries) && FindProcShort(glGetQueryBufferObjecti64v)
    && FindProcShort(glGetQueryBufferObjectiv) && FindProcShort(glGetQueryBufferObjectui64v)
    && FindProcShort(glGetQueryBufferObjectuiv) && FindProcShort(glMemoryBarrierByRegion)
    && FindProcShort(glGetTextureSubImage) && FindProcShort(glGetCompressedTextureSubImage)
    && FindProcShort(glGetGraphicsResetStatus) && FindProcShort(glGetnUniformfv)
    && FindProcShort(glGetnUniformiv) && FindProcShort(glGetnUniformuiv)
    && FindProcShort(glReadnPixels) && FindProcShort(glTextureBarrier);
  bool hasGetnTexImage = has45 && FindProcShort(glGetnCompressedTexImage)
                         && FindProcShort(glGetnTexImage) && FindProcShort(glGetnUniformdv);
  if (has45 && !hasGetnTexImage)
  {
    // Intel driver exports only ARB-suffixed functions in a violation to OpenGL 4.5 specs
    hasGetnTexImage =
      checkExtensionShort("GL_ARB_robustness")
      && theCtx.FindProc("glGetnCompressedTexImageARB", this->glGetnCompressedTexImage)
      && theCtx.FindProc("glGetnTexImageARB", this->glGetnTexImage)
      && theCtx.FindProc("glGetnUniformdvARB", this->glGetnUniformdv);
    has45 = hasGetnTexImage;
    if (hasGetnTexImage)
    {
      Message::SendTrace()
        << "Warning! glGetnCompressedTexImage function required by OpenGL 4.5 specs is not found.\n"
           "A non-standard glGetnCompressedTexImageARB fallback will be used instead.\n"
           "Please report this issue to OpenGL driver vendor '"
        << theCtx.myVendor << "'.";
    }
  }
  if (has45)
  {
    theCtx.core45         = (OpenGl_GlCore45*)this;
    theCtx.arbClipControl = true;
  }
  else
  {
    theCtx.checkWrongVersion(4, 5, aLastFailedProc);
  }

  has46 = isGlGreaterEqualShort(4, 6) && FindProcShort(glSpecializeShader)
          && FindProcShort(glPolygonOffsetClamp);

  bool hasIndParams = has46 && FindProcShort(glMultiDrawArraysIndirectCount)
                      && FindProcShort(glMultiDrawElementsIndirectCount);
  if (has46 && !hasIndParams)
  {
    // Intel driver exports only ARB-suffixed functions in a violation to OpenGL 4.6 specs
    hasIndParams =
      checkExtensionShort("GL_ARB_indirect_parameters")
      && theCtx.FindProc("glMultiDrawArraysIndirectCountARB", this->glMultiDrawArraysIndirectCount)
      && theCtx.FindProc("glMultiDrawElementsIndirectCountARB",
                         this->glMultiDrawElementsIndirectCount);
    has46 = hasIndParams;
    if (hasIndParams)
    {
      Message::SendTrace()
        << "Warning! glMultiDrawArraysIndirectCount function required by OpenGL 4.6 specs is not "
           "found.\n"
           "A non-standard glMultiDrawArraysIndirectCountARB fallback will be used instead.\n"
           "Please report this issue to OpenGL driver vendor '"
        << theCtx.myVendor << "'.";
    }
  }

  if (has46)
  {
    theCtx.core46 = (OpenGl_GlCore46*)this;
  }
  else
  {
    theCtx.checkWrongVersion(4, 6, aLastFailedProc);
  }

  // initialize debug context extension
  if (checkExtensionShort("GL_ARB_debug_output"))
  {
    theCtx.arbDbg = NULL;
    if (has43)
    {
      theCtx.arbDbg = (OpenGl_ArbDbg*)this;
    }
    else if (theCtx.FindProc("glDebugMessageControlARB", this->glDebugMessageControl)
             && theCtx.FindProc("glDebugMessageInsertARB", this->glDebugMessageInsert)
             && theCtx.FindProc("glDebugMessageCallbackARB", this->glDebugMessageCallback)
             && theCtx.FindProc("glGetDebugMessageLogARB", this->glGetDebugMessageLog))
    {
      theCtx.arbDbg = (OpenGl_ArbDbg*)this;
    }
  }

  // initialize FBO extension (ARB)
  if (hasFBO)
  {
    theCtx.arbFBO     = (OpenGl_ArbFBO*)this;
    theCtx.arbFBOBlit = (OpenGl_ArbFBOBlit*)this;
    theCtx.extPDS     = Standard_True; // extension for EXT, but part of ARB
  }

  // initialize GS extension (EXT)
  if (checkExtensionShort("GL_EXT_geometry_shader4") && FindProcShort(glProgramParameteriEXT))
  {
    theCtx.extGS = (OpenGl_ExtGS*)this;
  }

  // initialize bindless texture extension (ARB)
  if (checkExtensionShort("GL_ARB_bindless_texture") && FindProcShort(glGetTextureHandleARB)
      && FindProcShort(glGetTextureSamplerHandleARB)
      && FindProcShort(glMakeTextureHandleResidentARB)
      && FindProcShort(glMakeTextureHandleNonResidentARB) && FindProcShort(glGetImageHandleARB)
      && FindProcShort(glMakeImageHandleResidentARB)
      && FindProcShort(glMakeImageHandleNonResidentARB) && FindProcShort(glUniformHandleui64ARB)
      && FindProcShort(glUniformHandleui64vARB) && FindProcShort(glProgramUniformHandleui64ARB)
      && FindProcShort(glProgramUniformHandleui64vARB)
      && FindProcShort(glIsTextureHandleResidentARB) && FindProcShort(glIsImageHandleResidentARB)
      && FindProcShort(glVertexAttribL1ui64ARB) && FindProcShort(glVertexAttribL1ui64vARB)
      && FindProcShort(glGetVertexAttribLui64vARB))
  {
    theCtx.arbTexBindless = (OpenGl_ArbTexBindless*)this;
  }

  if (!has45 && checkExtensionShort("GL_ARB_clip_control") && FindProcShort(glClipControl))
  {
    theCtx.arbClipControl = true;
  }

  if (has30)
  {
    if (!has32 && checkExtensionShort("GL_ARB_texture_multisample")
        && FindProcShort(glTexImage2DMultisample))
    {
      //
    }
    if (!has43 && checkExtensionShort("GL_ARB_texture_storage_multisample")
        && FindProcShort(glTexStorage2DMultisample))
    {
      //
    }
  }
#endif
}
