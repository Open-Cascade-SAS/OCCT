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

#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>

#include <OpenGl_GlTypes.hxx>

#if !defined(HAVE_EGL)
  #if defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__) || defined(HAVE_GLES2)   \
    || defined(OCCT_UWP)
    #if !defined(__APPLE__)
      #define HAVE_EGL // EAGL is used instead of EGL
    #endif
  #elif !defined(_WIN32) && !defined(__APPLE__) && !defined(HAVE_XLIB)
    #define HAVE_EGL
  #endif
#endif
struct Aspect_XDisplay;

// GL version can be defined by system gl.h header
#ifdef GL_VERSION_1_2
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
#endif
#ifdef GL_COPY_READ_BUFFER_BINDING
  // suppress iOS SDK -Wmacro-redefined warnings
  #undef GL_DRAW_FRAMEBUFFER_BINDING
  #undef GL_COPY_READ_BUFFER_BINDING
  #undef GL_COPY_WRITE_BUFFER_BINDING
#endif
// include glext.h provided by Khronos group
#include <OpenGl_glext.h>

class OpenGl_Context;

//! Mega structure defines the complete list of OpenGL functions.
struct OpenGl_GlFunctions
{

  //! Check glGetError(); defined for debugging purposes.
  //! @return TRUE on error
  Standard_EXPORT static bool debugPrintError(const char* theName);

  //! Read OpenGL version.
  Standard_EXPORT static void readGlVersion(Standard_Integer& theGlVerMajor,
                                            Standard_Integer& theGlVerMinor);

  //! Load functions.
  Standard_EXPORT void load(OpenGl_Context& theCtx, Standard_Boolean theIsCoreProfile);

public: //! @name OpenGL 1.1
  typedef void(APIENTRYP glClearColor_t)(GLclampf theRed,
                                         GLclampf theGreen,
                                         GLclampf theBlue,
                                         GLclampf theAlpha);
  glClearColor_t glClearColor;

  typedef void(APIENTRYP glClear_t)(GLbitfield theMask);
  glClear_t glClear;

  typedef void(APIENTRYP glColorMask_t)(GLboolean theRed,
                                        GLboolean theGreen,
                                        GLboolean theBlue,
                                        GLboolean theAlpha);
  glColorMask_t glColorMask;

  typedef void(APIENTRYP glBlendFunc_t)(GLenum sfactor, GLenum dfactor);
  glBlendFunc_t glBlendFunc;

  typedef void(APIENTRYP glCullFace_t)(GLenum theMode);
  glCullFace_t glCullFace;

  typedef void(APIENTRYP glFrontFace_t)(GLenum theMode);
  glFrontFace_t glFrontFace;

  typedef void(APIENTRYP glLineWidth_t)(GLfloat theWidth);
  glLineWidth_t glLineWidth;

  typedef void(APIENTRYP glPolygonOffset_t)(GLfloat theFactor, GLfloat theUnits);
  glPolygonOffset_t glPolygonOffset;

  typedef void(APIENTRYP glScissor_t)(GLint theX, GLint theY, GLsizei theWidth, GLsizei theHeight);
  glScissor_t glScissor;

  typedef void(APIENTRYP glEnable_t)(GLenum theCap);
  glEnable_t glEnable;

  typedef void(APIENTRYP glDisable_t)(GLenum theCap);
  glDisable_t glDisable;

  typedef GLboolean(APIENTRYP glIsEnabled_t)(GLenum theCap);
  glIsEnabled_t glIsEnabled;

  typedef void(APIENTRYP glGetBooleanv_t)(GLenum theParamName, GLboolean* theValues);
  glGetBooleanv_t glGetBooleanv;

  typedef void(APIENTRYP glGetFloatv_t)(GLenum theParamName, GLfloat* theValues);
  glGetFloatv_t glGetFloatv;

  typedef void(APIENTRYP glGetIntegerv_t)(GLenum theParamName, GLint* theValues);
  glGetIntegerv_t glGetIntegerv;

  typedef GLenum(APIENTRYP glGetError_t)();
  glGetError_t glGetError;

  typedef const GLubyte*(APIENTRYP glGetString_t)(GLenum theName);
  glGetString_t glGetString;

  typedef void(APIENTRYP glFinish_t)();
  glFinish_t glFinish;

  typedef void(APIENTRYP glFlush_t)();
  glFlush_t glFlush;

  typedef void(APIENTRYP glHint_t)(GLenum theTarget, GLenum theMode);
  glHint_t glHint;

  typedef void(APIENTRYP glGetPointerv_t)(GLenum pname, GLvoid** params);
  glGetPointerv_t glGetPointerv;

  typedef void(APIENTRYP glReadBuffer_t)(GLenum src); // added to OpenGL ES 3.0
  glReadBuffer_t glReadBuffer;

  typedef void(APIENTRYP glDrawBuffer_t)(GLenum mode); // added to OpenGL ES 3.0
  glDrawBuffer_t glDrawBuffer;

  typedef void(APIENTRYP glPixelTransferi_t)(GLenum pname, GLint param);
  glPixelTransferi_t glPixelTransferi;

public: //! @name Depth Buffer
  typedef void(APIENTRYP glClearDepth_t)(GLclampd theDepth);
  glClearDepth_t glClearDepth;

  typedef void(APIENTRYP glDepthFunc_t)(GLenum theFunc);
  glDepthFunc_t glDepthFunc;

  typedef void(APIENTRYP glDepthMask_t)(GLboolean theFlag);
  glDepthMask_t glDepthMask;

  typedef void(APIENTRYP glDepthRange_t)(GLclampd theNearValue, GLclampd theFarValue);
  glDepthRange_t glDepthRange;

public: //! @name Transformation
  typedef void(APIENTRYP glViewport_t)(GLint theX, GLint theY, GLsizei theWidth, GLsizei theHeight);
  glViewport_t glViewport;

public: //! @name Vertex Arrays
  typedef void(APIENTRYP glDrawArrays_t)(GLenum theMode, GLint theFirst, GLsizei theCount);
  glDrawArrays_t glDrawArrays;

  typedef void(APIENTRYP glDrawElements_t)(GLenum        theMode,
                                           GLsizei       theCount,
                                           GLenum        theType,
                                           const GLvoid* theIndices);
  glDrawElements_t glDrawElements;

public: //! @name Raster functions
  typedef void(APIENTRYP glPixelStorei_t)(GLenum theParamName, GLint theParam);
  glPixelStorei_t glPixelStorei;

  typedef void(APIENTRYP glReadPixels_t)(GLint   x,
                                         GLint   y,
                                         GLsizei width,
                                         GLsizei height,
                                         GLenum  format,
                                         GLenum  type,
                                         GLvoid* pixels);
  glReadPixels_t glReadPixels;

public: //! @name Stenciling
  typedef void(APIENTRYP glStencilFunc_t)(GLenum func, GLint ref, GLuint mask);
  glStencilFunc_t glStencilFunc;

  typedef void(APIENTRYP glStencilMask_t)(GLuint mask);
  glStencilMask_t glStencilMask;

  typedef void(APIENTRYP glStencilOp_t)(GLenum fail, GLenum zfail, GLenum zpass);
  glStencilOp_t glStencilOp;

  typedef void(APIENTRYP glClearStencil_t)(GLint s);
  glClearStencil_t glClearStencil;

public: //! @name Texture mapping
  typedef void(APIENTRYP glTexParameterf_t)(GLenum target, GLenum pname, GLfloat param);
  glTexParameterf_t glTexParameterf;

  typedef void(APIENTRYP glTexParameteri_t)(GLenum target, GLenum pname, GLint param);
  glTexParameteri_t glTexParameteri;

  typedef void(APIENTRYP glTexParameterfv_t)(GLenum target, GLenum pname, const GLfloat* params);
  glTexParameterfv_t glTexParameterfv;

  typedef void(APIENTRYP glTexParameteriv_t)(GLenum target, GLenum pname, const GLint* params);
  glTexParameteriv_t glTexParameteriv;

  typedef void(APIENTRYP glGetTexParameterfv_t)(GLenum target, GLenum pname, GLfloat* params);
  glGetTexParameterfv_t glGetTexParameterfv;

  typedef void(APIENTRYP glGetTexParameteriv_t)(GLenum target, GLenum pname, GLint* params);
  glGetTexParameteriv_t glGetTexParameteriv;

  typedef void(APIENTRYP glTexImage2D_t)(GLenum        target,
                                         GLint         level,
                                         GLint         internalFormat,
                                         GLsizei       width,
                                         GLsizei       height,
                                         GLint         border,
                                         GLenum        format,
                                         GLenum        type,
                                         const GLvoid* pixels);
  glTexImage2D_t glTexImage2D;

  typedef void(APIENTRYP glGenTextures_t)(GLsizei n, GLuint* textures);
  glGenTextures_t glGenTextures;

  typedef void(APIENTRYP glDeleteTextures_t)(GLsizei n, const GLuint* textures);
  glDeleteTextures_t glDeleteTextures;

  typedef void(APIENTRYP glBindTexture_t)(GLenum target, GLuint texture);
  glBindTexture_t glBindTexture;

  typedef GLboolean(APIENTRYP glIsTexture_t)(GLuint texture);
  glIsTexture_t glIsTexture;

  typedef void(APIENTRYP glTexSubImage2D_t)(GLenum        target,
                                            GLint         level,
                                            GLint         xoffset,
                                            GLint         yoffset,
                                            GLsizei       width,
                                            GLsizei       height,
                                            GLenum        format,
                                            GLenum        type,
                                            const GLvoid* pixels);
  glTexSubImage2D_t glTexSubImage2D;

  typedef void(APIENTRYP glCopyTexImage2D_t)(GLenum  target,
                                             GLint   level,
                                             GLenum  internalformat,
                                             GLint   x,
                                             GLint   y,
                                             GLsizei width,
                                             GLsizei height,
                                             GLint   border);
  glCopyTexImage2D_t glCopyTexImage2D;

  typedef void(APIENTRYP glCopyTexSubImage2D_t)(GLenum  target,
                                                GLint   level,
                                                GLint   xoffset,
                                                GLint   yoffset,
                                                GLint   x,
                                                GLint   y,
                                                GLsizei width,
                                                GLsizei height);
  glCopyTexSubImage2D_t glCopyTexSubImage2D;

public: // not part of OpenGL ES 2.0
  typedef void(APIENTRYP glTexImage1D_t)(GLenum        target,
                                         GLint         level,
                                         GLint         internalFormat,
                                         GLsizei       width,
                                         GLint         border,
                                         GLenum        format,
                                         GLenum        type,
                                         const GLvoid* pixels);
  glTexImage1D_t glTexImage1D;

  typedef void(APIENTRYP glTexSubImage1D_t)(GLenum        target,
                                            GLint         level,
                                            GLint         xoffset,
                                            GLsizei       width,
                                            GLenum        format,
                                            GLenum        type,
                                            const GLvoid* pixels);
  glTexSubImage1D_t glTexSubImage1D;

  typedef void(APIENTRYP glCopyTexImage1D_t)(GLenum  target,
                                             GLint   level,
                                             GLenum  internalformat,
                                             GLint   x,
                                             GLint   y,
                                             GLsizei width,
                                             GLint   border);
  glCopyTexImage1D_t glCopyTexImage1D;

  typedef void(APIENTRYP glCopyTexSubImage1D_t)(GLenum  target,
                                                GLint   level,
                                                GLint   xoffset,
                                                GLint   x,
                                                GLint   y,
                                                GLsizei width);
  glCopyTexSubImage1D_t glCopyTexSubImage1D;

  typedef void(APIENTRYP glGetTexImage_t)(GLenum  target,
                                          GLint   level,
                                          GLenum  format,
                                          GLenum  type,
                                          GLvoid* pixels);
  glGetTexImage_t glGetTexImage;

  typedef void(APIENTRYP glAlphaFunc_t)(GLenum theFunc, GLclampf theRef);
  glAlphaFunc_t glAlphaFunc;

  typedef void(APIENTRYP glPointSize_t)(GLfloat theSize);
  glPointSize_t glPointSize;

public: //! @name OpenGL 1.1 FFP (obsolete, removed since 3.1)
  typedef void(APIENTRYP glTexEnvi_t)(GLenum target, GLenum pname, GLint param);
  glTexEnvi_t glTexEnvi;

  typedef void(APIENTRYP glGetTexEnviv_t)(GLenum target, GLenum pname, GLint* params);
  glGetTexEnviv_t glGetTexEnviv;

  typedef void(APIENTRYP glLogicOp_t)(GLenum opcode);
  glLogicOp_t glLogicOp;

public: //! @name Begin/End primitive specification (removed since 3.1)
  typedef void(APIENTRYP glColor4fv_t)(const GLfloat* theVec);
  glColor4fv_t glColor4fv;

public: //! @name Matrix operations (removed since 3.1)
  typedef void(APIENTRYP glMatrixMode_t)(GLenum theMode);
  glMatrixMode_t glMatrixMode;

  typedef void(APIENTRYP glLoadIdentity_t)();
  glLoadIdentity_t glLoadIdentity;

  typedef void(APIENTRYP glLoadMatrixf_t)(const GLfloat* theMatrix);
  glLoadMatrixf_t glLoadMatrixf;

public: //! @name Line and Polygon stipple (removed since 3.1)
  typedef void(APIENTRYP glLineStipple_t)(GLint theFactor, GLushort thePattern);
  glLineStipple_t glLineStipple;

  typedef void(APIENTRYP glPolygonStipple_t)(const GLubyte* theMask);
  glPolygonStipple_t glPolygonStipple;

public: //! @name Fixed pipeline lighting (removed since 3.1)
  typedef void(APIENTRYP glShadeModel_t)(GLenum theMode);
  glShadeModel_t glShadeModel;

  typedef void(APIENTRYP glLightf_t)(GLenum theLight, GLenum pname, GLfloat param);
  glLightf_t glLightf;

  typedef void(APIENTRYP glLightfv_t)(GLenum theLight, GLenum pname, const GLfloat* params);
  glLightfv_t glLightfv;

  typedef void(APIENTRYP glLightModeli_t)(GLenum pname, GLint param);
  glLightModeli_t glLightModeli;

  typedef void(APIENTRYP glLightModelfv_t)(GLenum pname, const GLfloat* params);
  glLightModelfv_t glLightModelfv;

  typedef void(APIENTRYP glMaterialf_t)(GLenum face, GLenum pname, GLfloat param);
  glMaterialf_t glMaterialf;

  typedef void(APIENTRYP glMaterialfv_t)(GLenum face, GLenum pname, const GLfloat* params);
  glMaterialfv_t glMaterialfv;

  typedef void(APIENTRYP glColorMaterial_t)(GLenum face, GLenum mode);
  glColorMaterial_t glColorMaterial;

public: //! @name clipping plane (removed since 3.1)
  typedef void(APIENTRYP glClipPlane_t)(GLenum thePlane, const GLdouble* theEquation);
  glClipPlane_t glClipPlane;

public: //! @name Display lists (removed since 3.1)
  typedef void(APIENTRYP glDeleteLists_t)(GLuint theList, GLsizei theRange);
  glDeleteLists_t glDeleteLists;

  typedef GLuint(APIENTRYP glGenLists_t)(GLsizei theRange);
  glGenLists_t glGenLists;

  typedef void(APIENTRYP glNewList_t)(GLuint theList, GLenum theMode);
  glNewList_t glNewList;

  typedef void(APIENTRYP glEndList_t)();
  glEndList_t glEndList;

  typedef void(APIENTRYP glCallList_t)(GLuint theList);
  glCallList_t glCallList;

  typedef void(APIENTRYP glCallLists_t)(GLsizei theNb, GLenum theType, const GLvoid* theLists);
  glCallLists_t glCallLists;

  typedef void(APIENTRYP glListBase_t)(GLuint theBase);
  glListBase_t glListBase;

public: //! @name Current raster position and Rectangles (removed since 3.1)
  typedef void(APIENTRYP glRasterPos2i_t)(GLint x, GLint y);
  glRasterPos2i_t glRasterPos2i;

  typedef void(APIENTRYP glRasterPos3fv_t)(const GLfloat* theVec);
  glRasterPos3fv_t glRasterPos3fv;

public: //! @name Texture mapping (removed since 3.1)
  typedef void(APIENTRYP glTexGeni_t)(GLenum coord, GLenum pname, GLint param);
  glTexGeni_t glTexGeni;

  typedef void(APIENTRYP glTexGenfv_t)(GLenum coord, GLenum pname, const GLfloat* params);
  glTexGenfv_t glTexGenfv;

public: //! @name Pixel copying (removed since 3.1)
  typedef void(APIENTRYP glDrawPixels_t)(GLsizei       width,
                                         GLsizei       height,
                                         GLenum        format,
                                         GLenum        type,
                                         const GLvoid* pixels);
  glDrawPixels_t glDrawPixels;

  typedef void(
    APIENTRYP glCopyPixels_t)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
  glCopyPixels_t glCopyPixels;

  typedef void(APIENTRYP glBitmap_t)(GLsizei        width,
                                     GLsizei        height,
                                     GLfloat        xorig,
                                     GLfloat        yorig,
                                     GLfloat        xmove,
                                     GLfloat        ymove,
                                     const GLubyte* bitmap);
  glBitmap_t glBitmap;

public: //! @name Edge flags and fixed-function vertex processing (removed since 3.1)
  typedef void(APIENTRYP glIndexPointer_t)(GLenum theType, GLsizei theStride, const GLvoid* thePtr);
  glIndexPointer_t glIndexPointer;

  typedef void(APIENTRYP glVertexPointer_t)(GLint         theSize,
                                            GLenum        theType,
                                            GLsizei       theStride,
                                            const GLvoid* thePtr);
  glVertexPointer_t glVertexPointer;

  typedef void(APIENTRYP glNormalPointer_t)(GLenum        theType,
                                            GLsizei       theStride,
                                            const GLvoid* thePtr);
  glNormalPointer_t glNormalPointer;

  typedef void(APIENTRYP glColorPointer_t)(GLint         theSize,
                                           GLenum        theType,
                                           GLsizei       theStride,
                                           const GLvoid* thePtr);
  glColorPointer_t glColorPointer;

  typedef void(APIENTRYP glTexCoordPointer_t)(GLint         theSize,
                                              GLenum        theType,
                                              GLsizei       theStride,
                                              const GLvoid* thePtr);
  glTexCoordPointer_t glTexCoordPointer;

  typedef void(APIENTRYP glEnableClientState_t)(GLenum theCap);
  glEnableClientState_t glEnableClientState;

  typedef void(APIENTRYP glDisableClientState_t)(GLenum theCap);
  glDisableClientState_t glDisableClientState;

  typedef void(APIENTRYP glGetTexLevelParameterfv_t)(GLenum   target,
                                                     GLint    level,
                                                     GLenum   pname,
                                                     GLfloat* params);
  glGetTexLevelParameterfv_t glGetTexLevelParameterfv;

  typedef void(APIENTRYP glGetTexLevelParameteriv_t)(GLenum target,
                                                     GLint  level,
                                                     GLenum pname,
                                                     GLint* params);
  glGetTexLevelParameteriv_t glGetTexLevelParameteriv;

  typedef void(APIENTRYP glPolygonMode_t)(GLenum face, GLenum mode);
  glPolygonMode_t glPolygonMode;

public: //! @name OpenGL ES 3.2
  typedef void(APIENTRYP glBlendBarrier_t)(void);
  glBlendBarrier_t glBlendBarrier;

  typedef void(APIENTRYP glPrimitiveBoundingBox_t)(GLfloat minX,
                                                   GLfloat minY,
                                                   GLfloat minZ,
                                                   GLfloat minW,
                                                   GLfloat maxX,
                                                   GLfloat maxY,
                                                   GLfloat maxZ,
                                                   GLfloat maxW);
  glPrimitiveBoundingBox_t glPrimitiveBoundingBox;

public: //! @name OpenGL 1.2
  PFNGLBLENDCOLORPROC        glBlendColor;
  PFNGLBLENDEQUATIONPROC     glBlendEquation;
  PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
  PFNGLTEXIMAGE3DPROC        glTexImage3D;
  PFNGLTEXSUBIMAGE3DPROC     glTexSubImage3D;
  PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

public: //! @name OpenGL 1.3
  PFNGLACTIVETEXTUREPROC           glActiveTexture;
  PFNGLSAMPLECOVERAGEPROC          glSampleCoverage;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC    glCompressedTexImage3D;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D;
  PFNGLCOMPRESSEDTEXIMAGE1DPROC    glCompressedTexImage1D;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage;

public: //! @name OpenGL 1.4
  PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
  PFNGLMULTIDRAWARRAYSPROC   glMultiDrawArrays;
  PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
  PFNGLPOINTPARAMETERFPROC   glPointParameterf;
  PFNGLPOINTPARAMETERFVPROC  glPointParameterfv;
  PFNGLPOINTPARAMETERIPROC   glPointParameteri;
  PFNGLPOINTPARAMETERIVPROC  glPointParameteriv;

public: //! @name OpenGL 1.5
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
  PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
  PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
  PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
  PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
  PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
  PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

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
  PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
  PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
  PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
  PFNGLISVERTEXARRAYPROC      glIsVertexArray;

public: //! @name GL_ARB_map_buffer_range (added to OpenGL 3.0 core)
  PFNGLMAPBUFFERRANGEPROC         glMapBufferRange;
  PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;

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
  PFNGLGETUNIFORMINDICESPROC         glGetUniformIndices;
  PFNGLGETACTIVEUNIFORMSIVPROC       glGetActiveUniformsiv;
  PFNGLGETACTIVEUNIFORMNAMEPROC      glGetActiveUniformName;
  PFNGLGETUNIFORMBLOCKINDEXPROC      glGetUniformBlockIndex;
  PFNGLGETACTIVEUNIFORMBLOCKIVPROC   glGetActiveUniformBlockiv;
  PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
  PFNGLUNIFORMBLOCKBINDINGPROC       glUniformBlockBinding;

public: //! @name GL_ARB_copy_buffer (added to OpenGL 3.1 core)
  PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;

public: //! @name OpenGL 3.1
  PFNGLDRAWARRAYSINSTANCEDPROC   glDrawArraysInstanced;
  PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
  PFNGLTEXBUFFERPROC             glTexBuffer;
  PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;

public: //! @name GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)
  PFNGLDRAWELEMENTSBASEVERTEXPROC          glDrawElementsBaseVertex;
  PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC     glDrawRangeElementsBaseVertex;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
  PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC     glMultiDrawElementsBaseVertex;

public: //! @name GL_ARB_provoking_vertex (added to OpenGL 3.2 core)
  PFNGLPROVOKINGVERTEXPROC glProvokingVertex;

public: //! @name GL_ARB_sync (added to OpenGL 3.2 core)
  PFNGLFENCESYNCPROC      glFenceSync;
  PFNGLISSYNCPROC         glIsSync;
  PFNGLDELETESYNCPROC     glDeleteSync;
  PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
  PFNGLWAITSYNCPROC       glWaitSync;
  PFNGLGETINTEGER64VPROC  glGetInteger64v;
  PFNGLGETSYNCIVPROC      glGetSynciv;

public: //! @name GL_ARB_texture_multisample (added to OpenGL 3.2 core)
  PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
  PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
  PFNGLGETMULTISAMPLEFVPROC      glGetMultisamplefv;
  PFNGLSAMPLEMASKIPROC           glSampleMaski;

public: //! @name OpenGL 3.2
  PFNGLGETINTEGER64I_VPROC        glGetInteger64i_v;
  PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
  PFNGLFRAMEBUFFERTEXTUREPROC     glFramebufferTexture;

public: //! @name GL_ARB_blend_func_extended (added to OpenGL 3.3 core)
  PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
  PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex;

public: //! @name GL_ARB_sampler_objects (added to OpenGL 3.3 core)
  PFNGLGENSAMPLERSPROC             glGenSamplers;
  PFNGLDELETESAMPLERSPROC          glDeleteSamplers;
  PFNGLISSAMPLERPROC               glIsSampler;
  PFNGLBINDSAMPLERPROC             glBindSampler;
  PFNGLSAMPLERPARAMETERIPROC       glSamplerParameteri;
  PFNGLSAMPLERPARAMETERIVPROC      glSamplerParameteriv;
  PFNGLSAMPLERPARAMETERFPROC       glSamplerParameterf;
  PFNGLSAMPLERPARAMETERFVPROC      glSamplerParameterfv;
  PFNGLSAMPLERPARAMETERIIVPROC     glSamplerParameterIiv;
  PFNGLSAMPLERPARAMETERIUIVPROC    glSamplerParameterIuiv;
  PFNGLGETSAMPLERPARAMETERIVPROC   glGetSamplerParameteriv;
  PFNGLGETSAMPLERPARAMETERIIVPROC  glGetSamplerParameterIiv;
  PFNGLGETSAMPLERPARAMETERFVPROC   glGetSamplerParameterfv;
  PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;

public: //! @name GL_ARB_timer_query (added to OpenGL 3.3 core)
  PFNGLQUERYCOUNTERPROC        glQueryCounter;
  PFNGLGETQUERYOBJECTI64VPROC  glGetQueryObjecti64v;
  PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;

public: //! @name GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)
  PFNGLVERTEXATTRIBP1UIPROC  glVertexAttribP1ui;
  PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
  PFNGLVERTEXATTRIBP2UIPROC  glVertexAttribP2ui;
  PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
  PFNGLVERTEXATTRIBP3UIPROC  glVertexAttribP3ui;
  PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
  PFNGLVERTEXATTRIBP4UIPROC  glVertexAttribP4ui;
  PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;

public: //! @name OpenGL 3.3
  PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;

public: //! @name GL_ARB_draw_indirect (added to OpenGL 4.0 core)
  PFNGLDRAWARRAYSINDIRECTPROC   glDrawArraysIndirect;
  PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;

public: //! @name GL_ARB_gpu_shader_fp64 (added to OpenGL 4.0 core)
  PFNGLUNIFORM1DPROC          glUniform1d;
  PFNGLUNIFORM2DPROC          glUniform2d;
  PFNGLUNIFORM3DPROC          glUniform3d;
  PFNGLUNIFORM4DPROC          glUniform4d;
  PFNGLUNIFORM1DVPROC         glUniform1dv;
  PFNGLUNIFORM2DVPROC         glUniform2dv;
  PFNGLUNIFORM3DVPROC         glUniform3dv;
  PFNGLUNIFORM4DVPROC         glUniform4dv;
  PFNGLUNIFORMMATRIX2DVPROC   glUniformMatrix2dv;
  PFNGLUNIFORMMATRIX3DVPROC   glUniformMatrix3dv;
  PFNGLUNIFORMMATRIX4DVPROC   glUniformMatrix4dv;
  PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
  PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
  PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
  PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
  PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
  PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
  PFNGLGETUNIFORMDVPROC       glGetUniformdv;

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
  PFNGLPATCHPARAMETERIPROC  glPatchParameteri;
  PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;

public: //! @name GL_ARB_transform_feedback2 (added to OpenGL 4.0 core)
  PFNGLBINDTRANSFORMFEEDBACKPROC    glBindTransformFeedback;
  PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
  PFNGLGENTRANSFORMFEEDBACKSPROC    glGenTransformFeedbacks;
  PFNGLISTRANSFORMFEEDBACKPROC      glIsTransformFeedback;
  PFNGLPAUSETRANSFORMFEEDBACKPROC   glPauseTransformFeedback;
  PFNGLRESUMETRANSFORMFEEDBACKPROC  glResumeTransformFeedback;
  PFNGLDRAWTRANSFORMFEEDBACKPROC    glDrawTransformFeedback;

public: //! @name GL_ARB_transform_feedback3 (added to OpenGL 4.0 core)
  PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
  PFNGLBEGINQUERYINDEXEDPROC           glBeginQueryIndexed;
  PFNGLENDQUERYINDEXEDPROC             glEndQueryIndexed;
  PFNGLGETQUERYINDEXEDIVPROC           glGetQueryIndexediv;

public: //! @name OpenGL 4.0
  PFNGLMINSAMPLESHADINGPROC       glMinSampleShading;
  PFNGLBLENDEQUATIONIPROC         glBlendEquationi;
  PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
  PFNGLBLENDFUNCIPROC             glBlendFunci;
  PFNGLBLENDFUNCSEPARATEIPROC     glBlendFuncSeparatei;

public: //! @name GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)
  PFNGLRELEASESHADERCOMPILERPROC    glReleaseShaderCompiler;
  PFNGLSHADERBINARYPROC             glShaderBinary;
  PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
  PFNGLDEPTHRANGEFPROC              glDepthRangef;
  PFNGLCLEARDEPTHFPROC              glClearDepthf;

public: //! @name GL_ARB_get_program_binary (added to OpenGL 4.1 core)
  PFNGLGETPROGRAMBINARYPROC  glGetProgramBinary;
  PFNGLPROGRAMBINARYPROC     glProgramBinary;
  PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;

public: //! @name GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)
  PFNGLUSEPROGRAMSTAGESPROC          glUseProgramStages;
  PFNGLACTIVESHADERPROGRAMPROC       glActiveShaderProgram;
  PFNGLCREATESHADERPROGRAMVPROC      glCreateShaderProgramv;
  PFNGLBINDPROGRAMPIPELINEPROC       glBindProgramPipeline;
  PFNGLDELETEPROGRAMPIPELINESPROC    glDeleteProgramPipelines;
  PFNGLGENPROGRAMPIPELINESPROC       glGenProgramPipelines;
  PFNGLISPROGRAMPIPELINEPROC         glIsProgramPipeline;
  PFNGLGETPROGRAMPIPELINEIVPROC      glGetProgramPipelineiv;
  PFNGLPROGRAMUNIFORM1IPROC          glProgramUniform1i;
  PFNGLPROGRAMUNIFORM1IVPROC         glProgramUniform1iv;
  PFNGLPROGRAMUNIFORM1FPROC          glProgramUniform1f;
  PFNGLPROGRAMUNIFORM1FVPROC         glProgramUniform1fv;
  PFNGLPROGRAMUNIFORM1DPROC          glProgramUniform1d;
  PFNGLPROGRAMUNIFORM1DVPROC         glProgramUniform1dv;
  PFNGLPROGRAMUNIFORM1UIPROC         glProgramUniform1ui;
  PFNGLPROGRAMUNIFORM1UIVPROC        glProgramUniform1uiv;
  PFNGLPROGRAMUNIFORM2IPROC          glProgramUniform2i;
  PFNGLPROGRAMUNIFORM2IVPROC         glProgramUniform2iv;
  PFNGLPROGRAMUNIFORM2FPROC          glProgramUniform2f;
  PFNGLPROGRAMUNIFORM2FVPROC         glProgramUniform2fv;
  PFNGLPROGRAMUNIFORM2DPROC          glProgramUniform2d;
  PFNGLPROGRAMUNIFORM2DVPROC         glProgramUniform2dv;
  PFNGLPROGRAMUNIFORM2UIPROC         glProgramUniform2ui;
  PFNGLPROGRAMUNIFORM2UIVPROC        glProgramUniform2uiv;
  PFNGLPROGRAMUNIFORM3IPROC          glProgramUniform3i;
  PFNGLPROGRAMUNIFORM3IVPROC         glProgramUniform3iv;
  PFNGLPROGRAMUNIFORM3FPROC          glProgramUniform3f;
  PFNGLPROGRAMUNIFORM3FVPROC         glProgramUniform3fv;
  PFNGLPROGRAMUNIFORM3DPROC          glProgramUniform3d;
  PFNGLPROGRAMUNIFORM3DVPROC         glProgramUniform3dv;
  PFNGLPROGRAMUNIFORM3UIPROC         glProgramUniform3ui;
  PFNGLPROGRAMUNIFORM3UIVPROC        glProgramUniform3uiv;
  PFNGLPROGRAMUNIFORM4IPROC          glProgramUniform4i;
  PFNGLPROGRAMUNIFORM4IVPROC         glProgramUniform4iv;
  PFNGLPROGRAMUNIFORM4FPROC          glProgramUniform4f;
  PFNGLPROGRAMUNIFORM4FVPROC         glProgramUniform4fv;
  PFNGLPROGRAMUNIFORM4DPROC          glProgramUniform4d;
  PFNGLPROGRAMUNIFORM4DVPROC         glProgramUniform4dv;
  PFNGLPROGRAMUNIFORM4UIPROC         glProgramUniform4ui;
  PFNGLPROGRAMUNIFORM4UIVPROC        glProgramUniform4uiv;
  PFNGLPROGRAMUNIFORMMATRIX2FVPROC   glProgramUniformMatrix2fv;
  PFNGLPROGRAMUNIFORMMATRIX3FVPROC   glProgramUniformMatrix3fv;
  PFNGLPROGRAMUNIFORMMATRIX4FVPROC   glProgramUniformMatrix4fv;
  PFNGLPROGRAMUNIFORMMATRIX2DVPROC   glProgramUniformMatrix2dv;
  PFNGLPROGRAMUNIFORMMATRIX3DVPROC   glProgramUniformMatrix3dv;
  PFNGLPROGRAMUNIFORMMATRIX4DVPROC   glProgramUniformMatrix4dv;
  PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
  PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
  PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
  PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
  PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
  PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
  PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
  PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
  PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
  PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
  PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
  PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
  PFNGLVALIDATEPROGRAMPIPELINEPROC   glValidateProgramPipeline;
  PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;

public: //! @name GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)
  PFNGLVERTEXATTRIBL1DPROC      glVertexAttribL1d;
  PFNGLVERTEXATTRIBL2DPROC      glVertexAttribL2d;
  PFNGLVERTEXATTRIBL3DPROC      glVertexAttribL3d;
  PFNGLVERTEXATTRIBL4DPROC      glVertexAttribL4d;
  PFNGLVERTEXATTRIBL1DVPROC     glVertexAttribL1dv;
  PFNGLVERTEXATTRIBL2DVPROC     glVertexAttribL2dv;
  PFNGLVERTEXATTRIBL3DVPROC     glVertexAttribL3dv;
  PFNGLVERTEXATTRIBL4DVPROC     glVertexAttribL4dv;
  PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
  PFNGLGETVERTEXATTRIBLDVPROC   glGetVertexAttribLdv;

public: //! @name GL_ARB_viewport_array (added to OpenGL 4.1 core)
  PFNGLVIEWPORTARRAYVPROC    glViewportArrayv;
  PFNGLVIEWPORTINDEXEDFPROC  glViewportIndexedf;
  PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
  PFNGLSCISSORARRAYVPROC     glScissorArrayv;
  PFNGLSCISSORINDEXEDPROC    glScissorIndexed;
  PFNGLSCISSORINDEXEDVPROC   glScissorIndexedv;
  PFNGLDEPTHRANGEARRAYVPROC  glDepthRangeArrayv;
  PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
  PFNGLGETFLOATI_VPROC       glGetFloati_v;
  PFNGLGETDOUBLEI_VPROC      glGetDoublei_v;

public: //! @name OpenGL 4.1
        //
public: //! @name GL_ARB_base_instance (added to OpenGL 4.2 core)
  PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC   glDrawArraysInstancedBaseInstance;
  PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance;
  PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC
  glDrawElementsInstancedBaseVertexBaseInstance;

public: //! @name GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)
  PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC       glDrawTransformFeedbackInstanced;
  PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced;

public: //! @name GL_ARB_internalformat_query (added to OpenGL 4.2 core)
  PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;

public: //! @name GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)
  PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv;

public: //! @name GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)
  PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
  PFNGLMEMORYBARRIERPROC    glMemoryBarrier;

public: //! @name GL_ARB_texture_storage (added to OpenGL 4.2 core)
  PFNGLTEXSTORAGE1DPROC glTexStorage1D;
  PFNGLTEXSTORAGE2DPROC glTexStorage2D;
  PFNGLTEXSTORAGE3DPROC glTexStorage3D;

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
  PFNGLCLIPCONTROLPROC                              glClipControl;
  PFNGLCREATETRANSFORMFEEDBACKSPROC                 glCreateTransformFeedbacks;
  PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC              glTransformFeedbackBufferBase;
  PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC             glTransformFeedbackBufferRange;
  PFNGLGETTRANSFORMFEEDBACKIVPROC                   glGetTransformFeedbackiv;
  PFNGLGETTRANSFORMFEEDBACKI_VPROC                  glGetTransformFeedbacki_v;
  PFNGLGETTRANSFORMFEEDBACKI64_VPROC                glGetTransformFeedbacki64_v;
  PFNGLCREATEBUFFERSPROC                            glCreateBuffers;
  PFNGLNAMEDBUFFERSTORAGEPROC                       glNamedBufferStorage;
  PFNGLNAMEDBUFFERDATAPROC                          glNamedBufferData;
  PFNGLNAMEDBUFFERSUBDATAPROC                       glNamedBufferSubData;
  PFNGLCOPYNAMEDBUFFERSUBDATAPROC                   glCopyNamedBufferSubData;
  PFNGLCLEARNAMEDBUFFERDATAPROC                     glClearNamedBufferData;
  PFNGLCLEARNAMEDBUFFERSUBDATAPROC                  glClearNamedBufferSubData;
  PFNGLMAPNAMEDBUFFERPROC                           glMapNamedBuffer;
  PFNGLMAPNAMEDBUFFERRANGEPROC                      glMapNamedBufferRange;
  PFNGLUNMAPNAMEDBUFFERPROC                         glUnmapNamedBuffer;
  PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC              glFlushMappedNamedBufferRange;
  PFNGLGETNAMEDBUFFERPARAMETERIVPROC                glGetNamedBufferParameteriv;
  PFNGLGETNAMEDBUFFERPARAMETERI64VPROC              glGetNamedBufferParameteri64v;
  PFNGLGETNAMEDBUFFERPOINTERVPROC                   glGetNamedBufferPointerv;
  PFNGLGETNAMEDBUFFERSUBDATAPROC                    glGetNamedBufferSubData;
  PFNGLCREATEFRAMEBUFFERSPROC                       glCreateFramebuffers;
  PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC             glNamedFramebufferRenderbuffer;
  PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC               glNamedFramebufferParameteri;
  PFNGLNAMEDFRAMEBUFFERTEXTUREPROC                  glNamedFramebufferTexture;
  PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC             glNamedFramebufferTextureLayer;
  PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC               glNamedFramebufferDrawBuffer;
  PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC              glNamedFramebufferDrawBuffers;
  PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC               glNamedFramebufferReadBuffer;
  PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC           glInvalidateNamedFramebufferData;
  PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC        glInvalidateNamedFramebufferSubData;
  PFNGLCLEARNAMEDFRAMEBUFFERIVPROC                  glClearNamedFramebufferiv;
  PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC                 glClearNamedFramebufferuiv;
  PFNGLCLEARNAMEDFRAMEBUFFERFVPROC                  glClearNamedFramebufferfv;
  PFNGLCLEARNAMEDFRAMEBUFFERFIPROC                  glClearNamedFramebufferfi;
  PFNGLBLITNAMEDFRAMEBUFFERPROC                     glBlitNamedFramebuffer;
  PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC              glCheckNamedFramebufferStatus;
  PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC           glGetNamedFramebufferParameteriv;
  PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv;
  PFNGLCREATERENDERBUFFERSPROC                      glCreateRenderbuffers;
  PFNGLNAMEDRENDERBUFFERSTORAGEPROC                 glNamedRenderbufferStorage;
  PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC      glNamedRenderbufferStorageMultisample;
  PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC          glGetNamedRenderbufferParameteriv;
  PFNGLCREATETEXTURESPROC                           glCreateTextures;
  PFNGLTEXTUREBUFFERPROC                            glTextureBuffer;
  PFNGLTEXTUREBUFFERRANGEPROC                       glTextureBufferRange;
  PFNGLTEXTURESTORAGE1DPROC                         glTextureStorage1D;
  PFNGLTEXTURESTORAGE2DPROC                         glTextureStorage2D;
  PFNGLTEXTURESTORAGE3DPROC                         glTextureStorage3D;
  PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC              glTextureStorage2DMultisample;
  PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC              glTextureStorage3DMultisample;
  PFNGLTEXTURESUBIMAGE1DPROC                        glTextureSubImage1D;
  PFNGLTEXTURESUBIMAGE2DPROC                        glTextureSubImage2D;
  PFNGLTEXTURESUBIMAGE3DPROC                        glTextureSubImage3D;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC              glCompressedTextureSubImage1D;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC              glCompressedTextureSubImage2D;
  PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC              glCompressedTextureSubImage3D;
  PFNGLCOPYTEXTURESUBIMAGE1DPROC                    glCopyTextureSubImage1D;
  PFNGLCOPYTEXTURESUBIMAGE2DPROC                    glCopyTextureSubImage2D;
  PFNGLCOPYTEXTURESUBIMAGE3DPROC                    glCopyTextureSubImage3D;
  PFNGLTEXTUREPARAMETERFPROC                        glTextureParameterf;
  PFNGLTEXTUREPARAMETERFVPROC                       glTextureParameterfv;
  PFNGLTEXTUREPARAMETERIPROC                        glTextureParameteri;
  PFNGLTEXTUREPARAMETERIIVPROC                      glTextureParameterIiv;
  PFNGLTEXTUREPARAMETERIUIVPROC                     glTextureParameterIuiv;
  PFNGLTEXTUREPARAMETERIVPROC                       glTextureParameteriv;
  PFNGLGENERATETEXTUREMIPMAPPROC                    glGenerateTextureMipmap;
  PFNGLBINDTEXTUREUNITPROC                          glBindTextureUnit;
  PFNGLGETTEXTUREIMAGEPROC                          glGetTextureImage;
  PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC                glGetCompressedTextureImage;
  PFNGLGETTEXTURELEVELPARAMETERFVPROC               glGetTextureLevelParameterfv;
  PFNGLGETTEXTURELEVELPARAMETERIVPROC               glGetTextureLevelParameteriv;
  PFNGLGETTEXTUREPARAMETERFVPROC                    glGetTextureParameterfv;
  PFNGLGETTEXTUREPARAMETERIIVPROC                   glGetTextureParameterIiv;
  PFNGLGETTEXTUREPARAMETERIUIVPROC                  glGetTextureParameterIuiv;
  PFNGLGETTEXTUREPARAMETERIVPROC                    glGetTextureParameteriv;
  PFNGLCREATEVERTEXARRAYSPROC                       glCreateVertexArrays;
  PFNGLDISABLEVERTEXARRAYATTRIBPROC                 glDisableVertexArrayAttrib;
  PFNGLENABLEVERTEXARRAYATTRIBPROC                  glEnableVertexArrayAttrib;
  PFNGLVERTEXARRAYELEMENTBUFFERPROC                 glVertexArrayElementBuffer;
  PFNGLVERTEXARRAYVERTEXBUFFERPROC                  glVertexArrayVertexBuffer;
  PFNGLVERTEXARRAYVERTEXBUFFERSPROC                 glVertexArrayVertexBuffers;
  PFNGLVERTEXARRAYATTRIBBINDINGPROC                 glVertexArrayAttribBinding;
  PFNGLVERTEXARRAYATTRIBFORMATPROC                  glVertexArrayAttribFormat;
  PFNGLVERTEXARRAYATTRIBIFORMATPROC                 glVertexArrayAttribIFormat;
  PFNGLVERTEXARRAYATTRIBLFORMATPROC                 glVertexArrayAttribLFormat;
  PFNGLVERTEXARRAYBINDINGDIVISORPROC                glVertexArrayBindingDivisor;
  PFNGLGETVERTEXARRAYIVPROC                         glGetVertexArrayiv;
  PFNGLGETVERTEXARRAYINDEXEDIVPROC                  glGetVertexArrayIndexediv;
  PFNGLGETVERTEXARRAYINDEXED64IVPROC                glGetVertexArrayIndexed64iv;
  PFNGLCREATESAMPLERSPROC                           glCreateSamplers;
  PFNGLCREATEPROGRAMPIPELINESPROC                   glCreateProgramPipelines;
  PFNGLCREATEQUERIESPROC                            glCreateQueries;
  PFNGLGETQUERYBUFFEROBJECTI64VPROC                 glGetQueryBufferObjecti64v;
  PFNGLGETQUERYBUFFEROBJECTIVPROC                   glGetQueryBufferObjectiv;
  PFNGLGETQUERYBUFFEROBJECTUI64VPROC                glGetQueryBufferObjectui64v;
  PFNGLGETQUERYBUFFEROBJECTUIVPROC                  glGetQueryBufferObjectuiv;
  PFNGLMEMORYBARRIERBYREGIONPROC                    glMemoryBarrierByRegion;
  PFNGLGETTEXTURESUBIMAGEPROC                       glGetTextureSubImage;
  PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC             glGetCompressedTextureSubImage;
  PFNGLGETGRAPHICSRESETSTATUSPROC                   glGetGraphicsResetStatus;
  PFNGLGETNCOMPRESSEDTEXIMAGEPROC                   glGetnCompressedTexImage;
  PFNGLGETNTEXIMAGEPROC                             glGetnTexImage;
  PFNGLGETNUNIFORMDVPROC                            glGetnUniformdv;
  PFNGLGETNUNIFORMFVPROC                            glGetnUniformfv;
  PFNGLGETNUNIFORMIVPROC                            glGetnUniformiv;
  PFNGLGETNUNIFORMUIVPROC                           glGetnUniformuiv;
  PFNGLREADNPIXELSPROC                              glReadnPixels;
  PFNGLTEXTUREBARRIERPROC                           glTextureBarrier;

public: //! @name OpenGL 4.6
  PFNGLSPECIALIZESHADERPROC               glSpecializeShader;
  PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC   glMultiDrawArraysIndirectCount;
  PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC glMultiDrawElementsIndirectCount;
  PFNGLPOLYGONOFFSETCLAMPPROC             glPolygonOffsetClamp;

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
  typedef const char*(WINAPI* wglGetExtensionsStringARB_t)(HDC theDeviceContext);
  wglGetExtensionsStringARB_t wglGetExtensionsStringARB;

  typedef BOOL(WINAPI* wglSwapIntervalEXT_t)(int theInterval);
  wglSwapIntervalEXT_t wglSwapIntervalEXT;

    // WGL_ARB_pixel_format

  #ifndef WGL_NUMBER_PIXEL_FORMATS_ARB
    #define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
    #define WGL_DRAW_TO_WINDOW_ARB 0x2001
    #define WGL_DRAW_TO_BITMAP_ARB 0x2002
    #define WGL_ACCELERATION_ARB 0x2003
    #define WGL_NEED_PALETTE_ARB 0x2004
    #define WGL_NEED_SYSTEM_PALETTE_ARB 0x2005
    #define WGL_SWAP_LAYER_BUFFERS_ARB 0x2006
    #define WGL_SWAP_METHOD_ARB 0x2007
    #define WGL_NUMBER_OVERLAYS_ARB 0x2008
    #define WGL_NUMBER_UNDERLAYS_ARB 0x2009
    #define WGL_TRANSPARENT_ARB 0x200A
    #define WGL_TRANSPARENT_RED_VALUE_ARB 0x2037
    #define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
    #define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
    #define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
    #define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
    #define WGL_SHARE_DEPTH_ARB 0x200C
    #define WGL_SHARE_STENCIL_ARB 0x200D
    #define WGL_SHARE_ACCUM_ARB 0x200E
    #define WGL_SUPPORT_GDI_ARB 0x200F
    #define WGL_SUPPORT_OPENGL_ARB 0x2010
    #define WGL_DOUBLE_BUFFER_ARB 0x2011
    #define WGL_STEREO_ARB 0x2012
    #define WGL_PIXEL_TYPE_ARB 0x2013
    #define WGL_COLOR_BITS_ARB 0x2014
    #define WGL_RED_BITS_ARB 0x2015
    #define WGL_RED_SHIFT_ARB 0x2016
    #define WGL_GREEN_BITS_ARB 0x2017
    #define WGL_GREEN_SHIFT_ARB 0x2018
    #define WGL_BLUE_BITS_ARB 0x2019
    #define WGL_BLUE_SHIFT_ARB 0x201A
    #define WGL_ALPHA_BITS_ARB 0x201B
    #define WGL_ALPHA_SHIFT_ARB 0x201C
    #define WGL_ACCUM_BITS_ARB 0x201D
    #define WGL_ACCUM_RED_BITS_ARB 0x201E
    #define WGL_ACCUM_GREEN_BITS_ARB 0x201F
    #define WGL_ACCUM_BLUE_BITS_ARB 0x2020
    #define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
    #define WGL_DEPTH_BITS_ARB 0x2022
    #define WGL_STENCIL_BITS_ARB 0x2023
    #define WGL_AUX_BUFFERS_ARB 0x2024

    #define WGL_NO_ACCELERATION_ARB 0x2025
    #define WGL_GENERIC_ACCELERATION_ARB 0x2026
    #define WGL_FULL_ACCELERATION_ARB 0x2027

    #define WGL_SWAP_EXCHANGE_ARB 0x2028
    #define WGL_SWAP_COPY_ARB 0x2029
    #define WGL_SWAP_UNDEFINED_ARB 0x202A

    #define WGL_TYPE_RGBA_ARB 0x202B
    #define WGL_TYPE_COLORINDEX_ARB 0x202C

  #endif // WGL_NUMBER_PIXEL_FORMATS_ARB

    // WGL_ARB_multisample
  #ifndef WGL_SAMPLE_BUFFERS_ARB
    #define WGL_SAMPLE_BUFFERS_ARB 0x2041
    #define WGL_SAMPLES_ARB 0x2042
  #endif

    // WGL_ARB_create_context_robustness
  #ifndef WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
    #define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
    #define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
    #define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
    #define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
  #endif

  typedef BOOL(WINAPI* wglChoosePixelFormatARB_t)(HDC           theDevCtx,
                                                  const int*    theIntAttribs,
                                                  const float*  theFloatAttribs,
                                                  unsigned int  theMaxFormats,
                                                  int*          theFormatsOut,
                                                  unsigned int* theNumFormatsOut);
  wglChoosePixelFormatARB_t wglChoosePixelFormatARB;

    // WGL_ARB_create_context_profile

  #ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
    #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
    #define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
    #define WGL_CONTEXT_FLAGS_ARB 0x2094
    #define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

    // WGL_CONTEXT_FLAGS bits
    #define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
    #define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

    // WGL_CONTEXT_PROFILE_MASK_ARB bits
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
    #define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
  #endif // WGL_CONTEXT_MAJOR_VERSION_ARB

  typedef HGLRC(WINAPI* wglCreateContextAttribsARB_t)(HDC        theDevCtx,
                                                      HGLRC      theShareContext,
                                                      const int* theAttribs);
  wglCreateContextAttribsARB_t wglCreateContextAttribsARB;

  // WGL_NV_DX_interop

  typedef BOOL(WINAPI* wglDXSetResourceShareHandleNV_t)(void* theObjectD3d, HANDLE theShareHandle);
  typedef HANDLE(WINAPI* wglDXOpenDeviceNV_t)(void* theDeviceD3d);
  typedef BOOL(WINAPI* wglDXCloseDeviceNV_t)(HANDLE theDeviceIOP);
  typedef HANDLE(WINAPI* wglDXRegisterObjectNV_t)(HANDLE theDeviceIOP,
                                                  void*  theObjectD3d,
                                                  GLuint theName,
                                                  GLenum theType,
                                                  GLenum theAccess);
  typedef BOOL(WINAPI* wglDXUnregisterObjectNV_t)(HANDLE theDeviceIOP, HANDLE theObject);
  typedef BOOL(WINAPI* wglDXObjectAccessNV_t)(HANDLE theObject, GLenum theAccess);
  typedef BOOL(WINAPI* wglDXLockObjectsNV_t)(HANDLE  theDeviceIOP,
                                             GLint   theCount,
                                             HANDLE* theObjects);
  typedef BOOL(WINAPI* wglDXUnlockObjectsNV_t)(HANDLE  theDeviceIOP,
                                               GLint   theCount,
                                               HANDLE* theObjects);

  wglDXSetResourceShareHandleNV_t wglDXSetResourceShareHandleNV;
  wglDXOpenDeviceNV_t             wglDXOpenDeviceNV;
  wglDXCloseDeviceNV_t            wglDXCloseDeviceNV;
  wglDXRegisterObjectNV_t         wglDXRegisterObjectNV;
  wglDXUnregisterObjectNV_t       wglDXUnregisterObjectNV;
  wglDXObjectAccessNV_t           wglDXObjectAccessNV;
  wglDXLockObjectsNV_t            wglDXLockObjectsNV;
  wglDXUnlockObjectsNV_t          wglDXUnlockObjectsNV;

  #ifndef WGL_ACCESS_READ_WRITE_NV
    #define WGL_ACCESS_READ_ONLY_NV 0x0000
    #define WGL_ACCESS_READ_WRITE_NV 0x0001
    #define WGL_ACCESS_WRITE_DISCARD_NV 0x0002
  #endif

    // WGL_AMD_gpu_association

  #ifndef WGL_GPU_VENDOR_AMD
    #define WGL_GPU_VENDOR_AMD 0x1F00
    #define WGL_GPU_RENDERER_STRING_AMD 0x1F01
    #define WGL_GPU_OPENGL_VERSION_STRING_AMD 0x1F02
    #define WGL_GPU_FASTEST_TARGET_GPUS_AMD 0x21A2
    #define WGL_GPU_RAM_AMD 0x21A3
    #define WGL_GPU_CLOCK_AMD 0x21A4
    #define WGL_GPU_NUM_PIPES_AMD 0x21A5
    #define WGL_GPU_NUM_SIMD_AMD 0x21A6
    #define WGL_GPU_NUM_RB_AMD 0x21A7
    #define WGL_GPU_NUM_SPI_AMD 0x21A8
  #endif

  typedef UINT(WINAPI* wglGetGPUIDsAMD_t)(UINT theMaxCount, UINT* theIds);
  typedef INT(WINAPI* wglGetGPUInfoAMD_t)(UINT   theId,
                                          INT    theProperty,
                                          GLenum theDataType,
                                          UINT   theSize,
                                          void*  theData);
  typedef UINT(WINAPI* wglGetContextGPUIDAMD_t)(HGLRC theHglrc);
  wglGetGPUIDsAMD_t       wglGetGPUIDsAMD;
  wglGetGPUInfoAMD_t      wglGetGPUInfoAMD;
  wglGetContextGPUIDAMD_t wglGetContextGPUIDAMD;

#elif defined(__APPLE__)
public: //! @name CGL extensions

#else
public: //! @name glX extensions
  // GLX_EXT_swap_control
  // typedef int         (*glXSwapIntervalEXT_t)(Display* theDisplay, GLXDrawable theDrawable, int
  // theInterval);
  typedef int (*glXSwapIntervalEXT_t)();
  glXSwapIntervalEXT_t glXSwapIntervalEXT;

  typedef int (*glXSwapIntervalSGI_t)(int theInterval);
  glXSwapIntervalSGI_t glXSwapIntervalSGI;

    // GLX_MESA_query_renderer
  #ifndef GLX_RENDERER_VENDOR_ID_MESA
    // for glXQueryRendererIntegerMESA() and glXQueryCurrentRendererIntegerMESA()
    #define GLX_RENDERER_VENDOR_ID_MESA 0x8183
    #define GLX_RENDERER_DEVICE_ID_MESA 0x8184
    #define GLX_RENDERER_VERSION_MESA 0x8185
    #define GLX_RENDERER_ACCELERATED_MESA 0x8186
    #define GLX_RENDERER_VIDEO_MEMORY_MESA 0x8187
    #define GLX_RENDERER_UNIFIED_MEMORY_ARCHITECTURE_MESA 0x8188
    #define GLX_RENDERER_PREFERRED_PROFILE_MESA 0x8189
    #define GLX_RENDERER_OPENGL_CORE_PROFILE_VERSION_MESA 0x818A
    #define GLX_RENDERER_OPENGL_COMPATIBILITY_PROFILE_VERSION_MESA 0x818B
    #define GLX_RENDERER_OPENGL_ES_PROFILE_VERSION_MESA 0x818C
    #define GLX_RENDERER_OPENGL_ES2_PROFILE_VERSION_MESA 0x818D

    #define GLX_RENDERER_ID_MESA 0x818E
  #endif // GLX_RENDERER_VENDOR_ID_MESA

  typedef int (*glXQueryRendererIntegerMESA_t)(Aspect_XDisplay* theDisplay,
                                               int              theScreen,
                                               int              theRenderer,
                                               int              theAttribute,
                                               unsigned int*    theValue);
  typedef int (*glXQueryCurrentRendererIntegerMESA_t)(int theAttribute, unsigned int* theValue);
  typedef const char* (*glXQueryRendererStringMESA_t)(Aspect_XDisplay* theDisplay,
                                                      int              theScreen,
                                                      int              theRenderer,
                                                      int              theAttribute);
  typedef const char* (*glXQueryCurrentRendererStringMESA_t)(int theAttribute);

  glXQueryRendererIntegerMESA_t        glXQueryRendererIntegerMESA;
  glXQueryCurrentRendererIntegerMESA_t glXQueryCurrentRendererIntegerMESA;
  glXQueryRendererStringMESA_t         glXQueryRendererStringMESA;
  glXQueryCurrentRendererStringMESA_t  glXQueryCurrentRendererStringMESA;
#endif
};

#endif // _OpenGl_GlFunctions_Header
