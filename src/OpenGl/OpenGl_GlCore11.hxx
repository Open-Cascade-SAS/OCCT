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

#ifndef OpenGl_GlCore11_HeaderFile
#define OpenGl_GlCore11_HeaderFile

#include <OpenGl_GlCore11Fwd.hxx>

//! OpenGL 1.1 core.
//! Notice that all functions within this structure are actually exported by system GL library.
//! The main purpose for these hint - to control visibility of functions per GL version
//! (global functions should not be used directly to achieve this effect!).
struct OpenGl_GlCore11 : protected OpenGl_GlFunctions
{

#if !defined(GL_ES_VERSION_2_0)
public:

  inline void glTexEnvi (GLenum target, GLenum pname, GLint param)
  {
    ::glTexEnvi (target, pname, param);
    OpenGl_TRACE(glTexEnvi)
  }

  inline void glGetTexEnviv (GLenum target, GLenum pname, GLint *params)
  {
    ::glGetTexEnviv (target, pname, params);
    OpenGl_TRACE(glGetTexEnviv)
  }

  inline void glLogicOp (GLenum opcode)
  {
    ::glLogicOp (opcode);
    OpenGl_TRACE(glLogicOp)
  }

public: //! @name Begin/End primitive specification (removed since 3.1)

  inline void glColor4fv (const GLfloat*   theVec)
  {
    ::glColor4fv (theVec);
    OpenGl_TRACE(glColor4fv)
  }

public: //! @name Matrix operations (removed since 3.1)

  inline void glMatrixMode (GLenum theMode)
  {
    ::glMatrixMode (theMode);
    OpenGl_TRACE(glMatrixMode)
  }

  inline void glLoadIdentity()
  {
    ::glLoadIdentity();
    OpenGl_TRACE(glLoadIdentity)
  }

  inline void glLoadMatrixf (const GLfloat*  theMatrix)
  {
    ::glLoadMatrixf (theMatrix);
    OpenGl_TRACE(glLoadMatrixf)
  }

public: //! @name Line and Polygon stipple (removed since 3.1)

  inline void glLineStipple (GLint theFactor, GLushort thePattern)
  {
    ::glLineStipple (theFactor, thePattern);
    OpenGl_TRACE(glLineStipple)
  }

  inline void glPolygonStipple (const GLubyte* theMask)
  {
    ::glPolygonStipple (theMask);
    OpenGl_TRACE(glPolygonStipple)
  }

public: //! @name Fixed pipeline lighting (removed since 3.1)

  inline void glShadeModel (GLenum theMode)
  {
    ::glShadeModel (theMode);
    OpenGl_TRACE(glShadeModel)
  }

  inline void glLightf (GLenum theLight, GLenum pname, GLfloat param)
  {
    ::glLightf (theLight, pname, param);
    OpenGl_TRACE(glLightf)
  }

  inline void glLightfv (GLenum theLight, GLenum pname, const GLfloat* params)
  {
    ::glLightfv (theLight, pname, params);
    OpenGl_TRACE(glLightfv)
  }

  inline void glLightModeli (GLenum pname, GLint param)
  {
    ::glLightModeli(pname, param);
    OpenGl_TRACE(glLightModeli)
  }

  inline void glLightModelfv (GLenum pname, const GLfloat* params)
  {
    ::glLightModelfv(pname, params);
    OpenGl_TRACE(glLightModelfv)
  }

  inline void glMaterialf (GLenum face, GLenum pname, GLfloat param)
  {
    ::glMaterialf(face, pname, param);
    OpenGl_TRACE(glMaterialf)
  }

  inline void glMaterialfv (GLenum face, GLenum pname, const GLfloat* params)
  {
    ::glMaterialfv(face, pname, params);
    OpenGl_TRACE(glMaterialfv)
  }

  inline void glColorMaterial (GLenum face, GLenum mode)
  {
    ::glColorMaterial(face, mode);
    OpenGl_TRACE(glColorMaterial)
  }

public: //! @name clipping plane (removed since 3.1)

  inline void glClipPlane (GLenum thePlane, const GLdouble* theEquation)
  {
    ::glClipPlane (thePlane, theEquation);
    OpenGl_TRACE(glClipPlane)
  }

public: //! @name Display lists (removed since 3.1)

  inline void glDeleteLists (GLuint theList, GLsizei theRange)
  {
    ::glDeleteLists (theList, theRange);
    OpenGl_TRACE(glDeleteLists)
  }

  inline GLuint glGenLists (GLsizei theRange)
  {
    const GLuint aRes = ::glGenLists (theRange);
    OpenGl_TRACE(glGenLists)
    return aRes;
  }

  inline void glNewList (GLuint theList, GLenum theMode)
  {
    ::glNewList (theList, theMode);
    OpenGl_TRACE(glNewList)
  }

  inline void glEndList()
  {
    ::glEndList();
    OpenGl_TRACE(glEndList)
  }

  inline void glCallList (GLuint theList)
  {
    ::glCallList (theList);
    OpenGl_TRACE(glCallList)
  }

  inline void glCallLists (GLsizei theNb, GLenum theType, const GLvoid* theLists)
  {
    ::glCallLists (theNb, theType, theLists);
    OpenGl_TRACE(glCallLists)
  }

  inline void glListBase (GLuint theBase)
  {
    ::glListBase (theBase);
    OpenGl_TRACE(glListBase)
  }

public: //! @name Current raster position and Rectangles (removed since 3.1)

  inline void glRasterPos2i (GLint    x, GLint    y)
  {
    ::glRasterPos2i (x, y);
    OpenGl_TRACE(glRasterPos2i)
  }

  inline void glRasterPos3fv (const GLfloat*  theVec)
  {
    ::glRasterPos3fv (theVec);
    OpenGl_TRACE(glRasterPos3fv)
  }

public: //! @name Texture mapping (removed since 3.1)

  inline void glTexGeni (GLenum coord, GLenum pname, GLint param)
  {
    ::glTexGeni (coord, pname, param);
    OpenGl_TRACE(glTexGeni)
  }

  inline void glTexGenfv (GLenum coord, GLenum pname, const GLfloat* params)
  {
    ::glTexGenfv (coord, pname, params);
    OpenGl_TRACE(glTexGenfv)
  }

public: //! @name Pixel copying (removed since 3.1)

  inline void glDrawPixels (GLsizei width, GLsizei height,
                            GLenum format, GLenum type,
                            const GLvoid* pixels)
  {
    ::glDrawPixels (width, height, format, type, pixels);
    OpenGl_TRACE(glDrawPixels)
  }

  inline void glCopyPixels (GLint x, GLint y,
                    GLsizei width, GLsizei height,
                    GLenum type)
  {
    ::glCopyPixels (x, y, width, height, type);
    OpenGl_TRACE(glCopyPixels)
  }

  inline void glBitmap (GLsizei width, GLsizei height,
                        GLfloat xorig, GLfloat yorig,
                        GLfloat xmove, GLfloat ymove,
                        const GLubyte* bitmap)
  {
    ::glBitmap (width, height, xorig, yorig, xmove, ymove, bitmap);
    OpenGl_TRACE(glBitmap)
  }

public: //! @name Edge flags and fixed-function vertex processing (removed since 3.1)

  inline void glIndexPointer (GLenum theType, GLsizei theStride, const GLvoid* thePtr)
  {
    ::glIndexPointer (theType, theStride, thePtr);
    OpenGl_TRACE(glIndexPointer)
  }

  inline void glVertexPointer (GLint theSize, GLenum theType, GLsizei theStride, const GLvoid* thePtr)
  {
    ::glVertexPointer (theSize, theType, theStride, thePtr);
    OpenGl_TRACE(glVertexPointer)
  }

  inline void glNormalPointer (GLenum theType, GLsizei theStride, const GLvoid* thePtr)
  {
    ::glNormalPointer (theType, theStride, thePtr);
    OpenGl_TRACE(glNormalPointer)
  }

  inline void glColorPointer (GLint theSize, GLenum theType, GLsizei theStride, const GLvoid* thePtr)
  {
    ::glColorPointer (theSize, theType, theStride, thePtr);
    OpenGl_TRACE(glColorPointer)
  }

  inline void glTexCoordPointer (GLint theSize, GLenum theType, GLsizei theStride, const GLvoid* thePtr)
  {
    ::glTexCoordPointer (theSize, theType, theStride, thePtr);
    OpenGl_TRACE(glTexCoordPointer)
  }

  inline void glEnableClientState (GLenum theCap)
  {
    ::glEnableClientState (theCap);
    OpenGl_TRACE(glEnableClientState)
  }

  inline void glDisableClientState (GLenum theCap)
  {
    ::glDisableClientState (theCap);
    OpenGl_TRACE(glDisableClientState)
  }

#endif

};

#endif // _OpenGl_GlCore11_Header
