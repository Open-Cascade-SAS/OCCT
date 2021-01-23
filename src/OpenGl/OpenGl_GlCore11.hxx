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

#ifndef _OpenGl_GlCore11_Header
#define _OpenGl_GlCore11_Header

#include <OpenGl_GlCore11Fwd.hxx>

//! OpenGL 1.1 core.
//! Notice that all functions within this structure are actually exported by system GL library.
//! The main purpose for these hint - to control visibility of functions per GL version
//! (global functions should not be used directly to achieve this effect!).
struct OpenGl_GlCore11 : protected OpenGl_GlFunctions
{

#if !defined(GL_ES_VERSION_2_0)
  inline void glRotated (GLdouble theAngleDegrees,
                         GLdouble theX, GLdouble theY, GLdouble theZ)
  {
    ::glRotated (theAngleDegrees, theX, theY, theZ);
    OpenGl_TRACE(glRotated)
  }

  inline void glScaled (GLdouble theX, GLdouble theY, GLdouble theZ)
  {
    ::glScaled (theX, theY, theZ);
    OpenGl_TRACE(glScaled)
  }

  inline void glTranslated (GLdouble theX, GLdouble theY, GLdouble theZ)
  {
    ::glTranslated (theX, theY, theZ);
    OpenGl_TRACE(glTranslated)
  }

public: //! @name Begin/End primitive specification (removed since 3.1)

  inline void glBegin (GLenum theMode)
  {
    ::glBegin (theMode);
    OpenGl_TRACE(glBegin)
  }

  inline void glEnd()
  {
    ::glEnd();
    OpenGl_TRACE(glEnd)
  }

  inline void glVertex2d (GLdouble theX, GLdouble theY)
  {
    ::glVertex2d (theX, theY);
    OpenGl_TRACE(glVertex2d)
  }

  inline void glVertex2f (GLfloat  theX, GLfloat  theY)
  {
    ::glVertex2f (theX, theY);
    OpenGl_TRACE(glVertex2f)
  }

  inline void glVertex2i (GLint    theX, GLint    theY)
  {
    ::glVertex2i (theX, theY);
    OpenGl_TRACE(glVertex2i)
  }

  inline void glVertex2s (GLshort  theX, GLshort  theY)
  {
    ::glVertex2s (theX, theY);
    OpenGl_TRACE(glVertex2s)
  }

  inline void glVertex3d (GLdouble theX, GLdouble theY, GLdouble theZ)
  {
    ::glVertex3d (theX, theY, theZ);
    OpenGl_TRACE(glVertex3d)
  }

  inline void glVertex3f (GLfloat  theX, GLfloat  theY, GLfloat  theZ)
  {
    ::glVertex3f (theX, theY, theZ);
    OpenGl_TRACE(glVertex3f)
  }

  inline void glVertex3i (GLint    theX, GLint    theY, GLint    theZ)
  {
    ::glVertex3i (theX, theY, theZ);
    OpenGl_TRACE(glVertex3i)
  }

  inline void glVertex3s (GLshort  theX, GLshort  theY, GLshort  theZ)
  {
    ::glVertex3s (theX, theY, theZ);
    OpenGl_TRACE(glVertex3s)
  }

  inline void glVertex4d (GLdouble theX, GLdouble theY, GLdouble theZ, GLdouble theW)
  {
    ::glVertex4d (theX, theY, theZ, theW);
    OpenGl_TRACE(glVertex4d)
  }

  inline void glVertex4f (GLfloat  theX, GLfloat  theY, GLfloat  theZ, GLfloat  theW)
  {
    ::glVertex4f (theX, theY, theZ, theW);
    OpenGl_TRACE(glVertex4f)
  }

  inline void glVertex4i (GLint    theX, GLint    theY, GLint    theZ, GLint    theW)
  {
    ::glVertex4i (theX, theY, theZ, theW);
    OpenGl_TRACE(glVertex4i)
  }

  inline void glVertex4s (GLshort  theX, GLshort  theY, GLshort  theZ, GLshort  theW)
  {
    ::glVertex4s (theX, theY, theZ, theW);
    OpenGl_TRACE(glVertex4s)
  }

  inline void glVertex2dv (const GLdouble* theVec2)
  {
    ::glVertex2dv (theVec2);
    OpenGl_TRACE(glVertex2dv)
  }

  inline void glVertex2fv (const GLfloat*  theVec2)
  {
    ::glVertex2fv (theVec2);
    OpenGl_TRACE(glVertex2fv)
  }

  inline void glVertex2iv (const GLint*    theVec2)
  {
    ::glVertex2iv (theVec2);
    OpenGl_TRACE(glVertex2iv)
  }

  inline void glVertex2sv (const GLshort*  theVec2)
  {
    ::glVertex2sv (theVec2);
    OpenGl_TRACE(glVertex2sv)
  }

  inline void glVertex3dv (const GLdouble* theVec3)
  {
    ::glVertex3dv (theVec3);
    OpenGl_TRACE(glVertex3dv)
  }

  inline void glVertex3fv (const GLfloat*  theVec3)
  {
    ::glVertex3fv (theVec3);
    OpenGl_TRACE(glVertex3fv)
  }

  inline void glVertex3iv (const GLint*    theVec3)
  {
    ::glVertex3iv (theVec3);
    OpenGl_TRACE(glVertex3iv)
  }

  inline void glVertex3sv (const GLshort*  theVec3)
  {
    ::glVertex3sv (theVec3);
    OpenGl_TRACE(glVertex3sv)
  }

  inline void glVertex4dv (const GLdouble* theVec4)
  {
    ::glVertex4dv (theVec4);
    OpenGl_TRACE(glVertex4dv)
  }

  inline void glVertex4fv (const GLfloat*  theVec4)
  {
    ::glVertex4fv (theVec4);
    OpenGl_TRACE(glVertex4fv)
  }

  inline void glVertex4iv (const GLint*    theVec4)
  {
    ::glVertex4iv (theVec4);
    OpenGl_TRACE(glVertex4iv)
  }

  inline void glVertex4sv (const GLshort*  theVec4)
  {
    ::glVertex4sv (theVec4);
    OpenGl_TRACE(glVertex4sv)
  }

  inline void glNormal3b (GLbyte   nx, GLbyte   ny, GLbyte   nz)
  {
    ::glNormal3b(nx, ny, nz);
    OpenGl_TRACE(glNormal3b)
  }

  inline void glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
  {
    ::glNormal3d(nx, ny, nz);
    OpenGl_TRACE(glNormal3d)
  }

  inline void glNormal3f (GLfloat  nx, GLfloat  ny, GLfloat  nz)
  {
    ::glNormal3f(nx, ny, nz);
    OpenGl_TRACE(glNormal3f)
  }

  inline void glNormal3i (GLint    nx, GLint    ny, GLint    nz)
  {
    ::glNormal3i(nx, ny, nz);
    OpenGl_TRACE(glNormal3i)
  }

  inline void glNormal3s (GLshort nx, GLshort ny, GLshort nz)
  {
    ::glNormal3s(nx, ny, nz);
    OpenGl_TRACE(glNormal3s)
  }

  inline void glNormal3bv (const GLbyte*   theVec)
  {
    ::glNormal3bv (theVec);
    OpenGl_TRACE(glNormal3bv)
  }

  inline void glNormal3dv (const GLdouble* theVec)
  {
    ::glNormal3dv (theVec);
    OpenGl_TRACE(glNormal3dv)
  }

  inline void glNormal3fv (const GLfloat*  theVec)
  {
    ::glNormal3fv (theVec);
    OpenGl_TRACE(glNormal3fv)
  }

  inline void glNormal3iv (const GLint*    theVec)
  {
    ::glNormal3iv (theVec);
    OpenGl_TRACE(glNormal3iv)
  }

  inline void glNormal3sv (const GLshort*  theVec)
  {
    ::glNormal3sv (theVec);
    OpenGl_TRACE(glNormal3sv)
  }

  inline void glIndexd (GLdouble c)
  {
    ::glIndexd(c);
    OpenGl_TRACE(glIndexd)
  }

  inline void glIndexf (GLfloat c)
  {
    ::glIndexf(c);
    OpenGl_TRACE(glIndexf)
  }

  inline void glIndexi (GLint c)
  {
    ::glIndexi(c);
    OpenGl_TRACE(glIndexi)
  }

  inline void glIndexs (GLshort c)
  {
    ::glIndexs(c);
    OpenGl_TRACE(glIndexs)
  }

  inline void glIndexub (GLubyte c)
  {
    ::glIndexub(c);
    OpenGl_TRACE(glIndexub)
  }

  inline void glIndexdv (const GLdouble* c)
  {
    ::glIndexdv(c);
    OpenGl_TRACE(glIndexdv)
  }

  inline void glIndexfv (const GLfloat* c)
  {
    ::glIndexfv(c);
    OpenGl_TRACE(glIndexfv)
  }

  inline void glIndexiv (const GLint* c)
  {
    ::glIndexiv(c);
    OpenGl_TRACE(glIndexiv)
  }

  inline void glIndexsv (const GLshort* c)
  {
    ::glIndexsv(c);
    OpenGl_TRACE(glIndexsv)
  }

  inline void glIndexubv (const GLubyte* c)
  {
    ::glIndexubv(c);
    OpenGl_TRACE(glIndexubv)
  }

  inline void glColor3b (GLbyte theRed, GLbyte theGreen, GLbyte theBlue)
  {
    ::glColor3b (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3b)
  }

  inline void glColor3d (GLdouble theRed, GLdouble theGreen, GLdouble theBlue)
  {
    ::glColor3d (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3d)
  }

  inline void glColor3f (GLfloat theRed, GLfloat theGreen, GLfloat theBlue)
  {
    ::glColor3f (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3f)
  }

  inline void glColor3i (GLint theRed, GLint theGreen, GLint theBlue)
  {
    ::glColor3i (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3i)
  }

  inline void glColor3s (GLshort theRed, GLshort theGreen, GLshort theBlue)
  {
    ::glColor3s (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3s)
  }

  inline void glColor3ub (GLubyte theRed, GLubyte theGreen, GLubyte theBlue)
  {
    ::glColor3ub (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3ub)
  }

  inline void glColor3ui (GLuint theRed, GLuint theGreen, GLuint theBlue)
  {
    ::glColor3ui (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3ui)
  }

  inline void glColor3us (GLushort theRed, GLushort theGreen, GLushort theBlue)
  {
    ::glColor3us (theRed, theGreen, theBlue);
    OpenGl_TRACE(glColor3us)
  }

  inline void glColor4b (GLbyte theRed, GLbyte theGreen, GLbyte theBlue, GLbyte theAlpha)
  {
    ::glColor4b (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4b)
  }

  inline void glColor4d (GLdouble theRed, GLdouble theGreen, GLdouble theBlue, GLdouble theAlpha)
  {
    ::glColor4d (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4d)
  }

  inline void glColor4f (GLfloat theRed, GLfloat theGreen, GLfloat theBlue, GLfloat theAlpha)
  {
    ::glColor4f (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4f)
  }

  inline void glColor4i (GLint theRed, GLint theGreen, GLint theBlue, GLint theAlpha)
  {
    ::glColor4i (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4i)
  }

  inline void glColor4s (GLshort theRed, GLshort theGreen, GLshort theBlue, GLshort theAlpha)
  {
    ::glColor4s (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4s)
  }

  inline void glColor4ub (GLubyte theRed, GLubyte theGreen, GLubyte theBlue, GLubyte theAlpha)
  {
    ::glColor4ub (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4ub)
  }

  inline void glColor4ui (GLuint theRed, GLuint theGreen, GLuint theBlue, GLuint theAlpha)
  {
    ::glColor4ui (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4ui)
  }

  inline void glColor4us (GLushort theRed, GLushort theGreen, GLushort theBlue, GLushort theAlpha)
  {
    ::glColor4us (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glColor4us)
  }

  inline void glColor3bv (const GLbyte*    theVec)
  {
    ::glColor3bv (theVec);
    OpenGl_TRACE(glColor3bv)
  }

  inline void glColor3dv (const GLdouble*  theVec)
  {
    ::glColor3dv (theVec);
    OpenGl_TRACE(glColor3dv)
  }

  inline void glColor3fv (const GLfloat*   theVec)
  {
    ::glColor3fv (theVec);
    OpenGl_TRACE(glColor3fv)
  }

  inline void glColor3iv (const GLint*     theVec)
  {
    ::glColor3iv (theVec);
    OpenGl_TRACE(glColor3iv)
  }

  inline void glColor3sv (const GLshort*   theVec)
  {
    ::glColor3sv (theVec);
    OpenGl_TRACE(glColor3sv)
  }

  inline void glColor3ubv (const GLubyte*  theVec)
  {
    ::glColor3ubv (theVec);
    OpenGl_TRACE(glColor3ubv)
  }

  inline void glColor3uiv (const GLuint*   theVec)
  {
    ::glColor3uiv (theVec);
    OpenGl_TRACE(glColor3uiv)
  }

  inline void glColor3usv (const GLushort* theVec)
  {
    ::glColor3usv (theVec);
    OpenGl_TRACE(glColor3usv)
  }

  inline void glColor4bv (const GLbyte*    theVec)
  {
    ::glColor4bv (theVec);
    OpenGl_TRACE(glColor4bv)
  }

  inline void glColor4dv (const GLdouble*  theVec)
  {
    ::glColor4dv (theVec);
    OpenGl_TRACE(glColor4dv)
  }

  inline void glColor4fv (const GLfloat*   theVec)
  {
    ::glColor4fv (theVec);
    OpenGl_TRACE(glColor4fv)
  }

  inline void glColor4iv (const GLint*     theVec)
  {
    ::glColor4iv (theVec);
    OpenGl_TRACE(glColor4iv)
  }

  inline void glColor4sv (const GLshort*   theVec)
  {
    ::glColor4sv (theVec);
    OpenGl_TRACE(glColor4sv)
  }

  inline void glColor4ubv (const GLubyte*  theVec)
  {
    ::glColor4ubv (theVec);
    OpenGl_TRACE(glColor4ubv)
  }

  inline void glColor4uiv (const GLuint*   theVec)
  {
    ::glColor4uiv (theVec);
    OpenGl_TRACE(glColor4uiv)
  }

  inline void glColor4usv (const GLushort* theVec)
  {
    ::glColor4usv (theVec);
    OpenGl_TRACE(glColor4usv)
  }

  inline void glTexCoord1d (GLdouble s)
  {
    ::glTexCoord1d(s);
    OpenGl_TRACE(glTexCoord1d);
  }

  inline void glTexCoord1f (GLfloat s)
  {
    ::glTexCoord1f(s);
    OpenGl_TRACE(glTexCoord1f)
  }

  inline void glTexCoord1i (GLint s)
  {
    ::glTexCoord1i(s);
    OpenGl_TRACE(glTexCoord1i)
  }

  inline void glTexCoord1s (GLshort s)
  {
    ::glTexCoord1s(s);
    OpenGl_TRACE(glTexCoord1s)
  }

  inline void glTexCoord2d (GLdouble s, GLdouble t)
  {
    ::glTexCoord2d(s, t);
    OpenGl_TRACE(glTexCoord2d)
  }

  inline void glTexCoord2f (GLfloat s, GLfloat t)
  {
    ::glTexCoord2f(s, t);
    OpenGl_TRACE(glTexCoord2f)
  }

  inline void glTexCoord2i (GLint s, GLint t)
  {
    ::glTexCoord2i(s, t);
    OpenGl_TRACE(glTexCoord2i)
  }

  inline void glTexCoord2s (GLshort s, GLshort t)
  {
    ::glTexCoord2s(s, t);
    OpenGl_TRACE(glTexCoord2s)
  }

  inline void glTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
  {
    ::glTexCoord3d(s, t, r);
    OpenGl_TRACE(glTexCoord3d)
  }

  inline void glTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
  {
    ::glTexCoord3f(s, t, r);
    OpenGl_TRACE(glTexCoord3f)
  }

  inline void glTexCoord3i (GLint s, GLint t, GLint r)
  {
    ::glTexCoord3i(s, t, r);
    OpenGl_TRACE(glTexCoord3i)
  }

  inline void glTexCoord3s (GLshort s, GLshort t, GLshort r)
  {
    ::glTexCoord3s(s, t, r);
    OpenGl_TRACE(glTexCoord3s)
  }

  inline void glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
  {
    ::glTexCoord4d(s, t, r, q);
    OpenGl_TRACE(glTexCoord4d)
  }

  inline void glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
  {
    ::glTexCoord4f(s, t, r, q);
    OpenGl_TRACE(glTexCoord4f)
  }

  inline void glTexCoord4i (GLint s, GLint t, GLint r, GLint q)
  {
    ::glTexCoord4i(s, t, r, q);
    OpenGl_TRACE(glTexCoord4i)
  }

  inline void glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
  {
    ::glTexCoord4s(s, t, r, q);
    OpenGl_TRACE(glTexCoord4s)
  }

  inline void glTexCoord1dv (const GLdouble* theVec1)
  {
    ::glTexCoord1dv (theVec1);
    OpenGl_TRACE(glTexCoord1dv)
  }

  inline void glTexCoord1fv (const GLfloat*  theVec1)
  {
    ::glTexCoord1fv (theVec1);
    OpenGl_TRACE(glTexCoord1fv)
  }

  inline void glTexCoord1iv (const GLint*    theVec1)
  {
    ::glTexCoord1iv (theVec1);
    OpenGl_TRACE(glTexCoord1iv)
  }

  inline void glTexCoord1sv (const GLshort*  theVec1)
  {
    ::glTexCoord1sv (theVec1);
    OpenGl_TRACE(glTexCoord1sv)
  }

  inline void glTexCoord2dv (const GLdouble* theVec2)
  {
    ::glTexCoord2dv (theVec2);
    OpenGl_TRACE(glTexCoord2dv)
  }

  inline void glTexCoord2fv (const GLfloat*  theVec2)
  {
    ::glTexCoord2fv (theVec2);
    OpenGl_TRACE(glTexCoord2fv)
  }

  inline void glTexCoord2iv (const GLint*    theVec2)
  {
    ::glTexCoord2iv (theVec2);
    OpenGl_TRACE(glTexCoord2iv)
  }

  inline void glTexCoord2sv (const GLshort*  theVec)
  {
    ::glTexCoord2sv (theVec);
    OpenGl_TRACE(glTexCoord2sv)
  }

  inline void glTexCoord3dv (const GLdouble* theVec3)
  {
    ::glTexCoord3dv (theVec3);
    OpenGl_TRACE(glTexCoord3dv)
  }

  inline void glTexCoord3fv (const GLfloat*  theVec3)
  {
    ::glTexCoord3fv (theVec3);
    OpenGl_TRACE(glTexCoord3fv)
  }

  inline void glTexCoord3iv (const GLint*    theVec3)
  {
    ::glTexCoord3iv (theVec3);
    OpenGl_TRACE(glTexCoord3iv)
  }

  inline void glTexCoord3sv (const GLshort*  theVec3)
  {
    ::glTexCoord3sv (theVec3);
    OpenGl_TRACE(glTexCoord3sv)
  }

  inline void glTexCoord4dv (const GLdouble* theVec4)
  {
    ::glTexCoord4dv (theVec4);
    OpenGl_TRACE(glTexCoord4dv)
  }

  inline void glTexCoord4fv (const GLfloat*  theVec4)
  {
    ::glTexCoord4fv (theVec4);
    OpenGl_TRACE(glTexCoord4fv)
  }

  inline void glTexCoord4iv (const GLint*    theVec4)
  {
    ::glTexCoord4iv (theVec4);
    OpenGl_TRACE(glTexCoord4iv)
  }

  inline void glTexCoord4sv (const GLshort*  theVec4)
  {
    ::glTexCoord4sv (theVec4);
    OpenGl_TRACE(glTexCoord4sv)
  }

public: //! @name Matrix operations (removed since 3.1)

  inline void glMatrixMode (GLenum theMode)
  {
    ::glMatrixMode (theMode);
    OpenGl_TRACE(glMatrixMode)
  }

  inline void glOrtho (GLdouble theLeft,    GLdouble theRight,
                       GLdouble theBottom,  GLdouble theTop,
                       GLdouble theNearVal, GLdouble theFarVal)
  {
    ::glOrtho (theLeft, theRight, theBottom, theTop, theNearVal, theFarVal);
    OpenGl_TRACE(glOrtho)
  }

  inline void glFrustum (GLdouble theLeft,    GLdouble theRight,
                         GLdouble theBottom,  GLdouble theTop,
                         GLdouble theNearVal, GLdouble theFarVal)
  {
    ::glFrustum (theLeft, theRight, theBottom, theTop, theNearVal, theFarVal);
    OpenGl_TRACE(glFrustum)
  }

  inline void glPushMatrix()
  {
    ::glPushMatrix();
    OpenGl_TRACE(glPushMatrix)
  }

  inline void glPopMatrix()
  {
    ::glPopMatrix();
    OpenGl_TRACE(glPopMatrix)
  }

  inline void glLoadIdentity()
  {
    ::glLoadIdentity();
    OpenGl_TRACE(glLoadIdentity)
  }

  inline void glLoadMatrixd (const GLdouble* theMatrix)
  {
    ::glLoadMatrixd (theMatrix);
    OpenGl_TRACE(glLoadMatrixd)
  }

  inline void glLoadMatrixf (const GLfloat*  theMatrix)
  {
    ::glLoadMatrixf (theMatrix);
    OpenGl_TRACE(glLoadMatrixf)
  }

  inline void glMultMatrixd (const GLdouble* theMatrix)
  {
    ::glMultMatrixd (theMatrix);
    OpenGl_TRACE(glMultMatrixd)
  }

  inline void glMultMatrixf (const GLfloat*  theMatrix)
  {
    ::glMultMatrixf (theMatrix);
    OpenGl_TRACE(glMultMatrixf)
  }

public: //! @name Line and Polygon stripple (removed since 3.1)

  //void glLineWidth (GLfloat theWidth) { ::glLineWidth (theWidth); }

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

  inline void glGetPolygonStipple (GLubyte* theMask)
  {
    ::glGetPolygonStipple (theMask);
    OpenGl_TRACE(glGetPolygonStipple)
  }

public: //! @name Attribute stacks (removed since 3.1)

  inline void glPushAttrib (GLbitfield theMask)
  {
    ::glPushAttrib (theMask);
    OpenGl_TRACE(glPushAttrib)
  }

  inline void glPopAttrib()
  {
    ::glPopAttrib();
    OpenGl_TRACE(glPopAttrib)
  }

  inline void glPushClientAttrib (GLbitfield theMask)
  {
    ::glPushClientAttrib (theMask);
    OpenGl_TRACE(glPushClientAttrib)
  }

  inline void glPopClientAttrib()
  {
    ::glPopClientAttrib();
    OpenGl_TRACE(glPopClientAttrib)
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

  inline void glLighti (GLenum theLight, GLenum pname, GLint param)
  {
    ::glLighti (theLight, pname, param);
    OpenGl_TRACE(glLighti)
  }

  inline void glLightfv (GLenum theLight, GLenum pname, const GLfloat* params)
  {
    ::glLightfv (theLight, pname, params);
    OpenGl_TRACE(glLightfv)
  }

  inline void glLightiv (GLenum theLight, GLenum pname, const GLint* params)
  {
    ::glLightiv (theLight, pname, params);
    OpenGl_TRACE(glLightiv)
  }

  inline void glGetLightfv (GLenum theLight, GLenum pname, GLfloat *params)
  {
    ::glGetLightfv (theLight, pname, params);
    OpenGl_TRACE(glGetLightfv)
  }

  inline void glGetLightiv (GLenum theLight, GLenum pname, GLint *params)
  {
    ::glGetLightiv (theLight, pname, params);
    OpenGl_TRACE(glGetLightiv)
  }

  inline void glLightModelf (GLenum pname, GLfloat param)
  {
    ::glLightModelf(pname, param);
    OpenGl_TRACE(glLightModelf)
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

  inline void glLightModeliv (GLenum pname, const GLint* params)
  {
    ::glLightModeliv(pname, params);
    OpenGl_TRACE(glLightModeliv)
  }

  inline void glMaterialf (GLenum face, GLenum pname, GLfloat param)
  {
    ::glMaterialf(face, pname, param);
    OpenGl_TRACE(glMaterialf)
  }

  inline void glMateriali (GLenum face, GLenum pname, GLint param)
  {
    ::glMateriali(face, pname, param);
    OpenGl_TRACE(glMateriali)
  }

  inline void glMaterialfv (GLenum face, GLenum pname, const GLfloat* params)
  {
    ::glMaterialfv(face, pname, params);
    OpenGl_TRACE(glMaterialfv)
  }

  inline void glMaterialiv (GLenum face, GLenum pname, const GLint* params)
  {
    ::glMaterialiv(face, pname, params);
    OpenGl_TRACE(glMaterialiv)
  }

  inline void glGetMaterialfv (GLenum face, GLenum pname, GLfloat* params)
  {
    ::glGetMaterialfv(face, pname, params);
    OpenGl_TRACE(glGetMaterialfv)
  }

  inline void glGetMaterialiv (GLenum face, GLenum pname, GLint* params)
  {
    ::glGetMaterialiv(face, pname, params);
    OpenGl_TRACE(glGetMaterialiv)
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

  inline void glGetClipPlane (GLenum thePlane, GLdouble* theEquation)
  {
    ::glGetClipPlane (thePlane, theEquation);
    OpenGl_TRACE(glGetClipPlane)
  }

public: //! @name Display lists (removed since 3.1)

  inline GLboolean glIsList (GLuint theList)
  {
    const GLboolean aRes = ::glIsList (theList);
    OpenGl_TRACE(glIsList)
    return aRes;
  }

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

  inline void glRasterPos2d (GLdouble x, GLdouble y)
  {
    ::glRasterPos2d (x, y);
    OpenGl_TRACE(glRasterPos2d)
  }

  inline void glRasterPos2f (GLfloat  x, GLfloat  y)
  {
    ::glRasterPos2f (x, y);
    OpenGl_TRACE(glRasterPos2f)
  }

  inline void glRasterPos2i (GLint    x, GLint    y)
  {
    ::glRasterPos2i (x, y);
    OpenGl_TRACE(glRasterPos2i)
  }

  inline void glRasterPos2s (GLshort  x, GLshort  y)
  {
    ::glRasterPos2s (x, y);
    OpenGl_TRACE(glRasterPos2s)
  }

  inline void glRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
  {
    ::glRasterPos3d (x, y, z);
    OpenGl_TRACE(glRasterPos3d)
  }

  inline void glRasterPos3f (GLfloat  x, GLfloat  y, GLfloat  z)
  {
    ::glRasterPos3f (x, y, z);
    OpenGl_TRACE(glRasterPos3f)
  }

  inline void glRasterPos3i (GLint    x, GLint    y, GLint    z)
  {
    ::glRasterPos3i (x, y, z);
    OpenGl_TRACE(glRasterPos3i)
  }

  inline void glRasterPos3s (GLshort  x, GLshort  y, GLshort  z)
  {
    ::glRasterPos3s (x, y, z);
    OpenGl_TRACE(glRasterPos3s)
  }

  inline void glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
  {
    ::glRasterPos4d (x, y, z, w);
    OpenGl_TRACE(glRasterPos4d)
  }

  inline void glRasterPos4f (GLfloat  x, GLfloat  y, GLfloat  z, GLfloat  w)
  {
    ::glRasterPos4f (x, y, z, w);
    OpenGl_TRACE(glRasterPos4f)
  }

  inline void glRasterPos4i (GLint    x, GLint    y, GLint    z, GLint    w)
  {
    ::glRasterPos4i (x, y, z, w);
    OpenGl_TRACE(glRasterPos4i)
  }

  inline void glRasterPos4s (GLshort  x, GLshort  y, GLshort  z, GLshort  w)
  {
    ::glRasterPos4s (x, y, z, w);
    OpenGl_TRACE(glRasterPos4s)
  }

  inline void glRasterPos2dv (const GLdouble* theVec)
  {
    ::glRasterPos2dv (theVec);
    OpenGl_TRACE(glRasterPos2dv)
  }

  inline void glRasterPos2fv (const GLfloat*  theVec)
  {
    ::glRasterPos2fv (theVec);
    OpenGl_TRACE(glRasterPos2fv)
  }

  inline void glRasterPos2iv (const GLint*    theVec)
  {
    ::glRasterPos2iv (theVec);
    OpenGl_TRACE(glRasterPos2iv)
  }

  inline void glRasterPos2sv (const GLshort*  theVec)
  {
    ::glRasterPos2sv (theVec);
    OpenGl_TRACE(glRasterPos2sv)
  }

  inline void glRasterPos3dv (const GLdouble* theVec)
  {
    ::glRasterPos3dv (theVec);
    OpenGl_TRACE(glRasterPos3dv)
  }

  inline void glRasterPos3fv (const GLfloat*  theVec)
  {
    ::glRasterPos3fv (theVec);
    OpenGl_TRACE(glRasterPos3fv)
  }

  inline void glRasterPos3iv (const GLint*    theVec)
  {
    ::glRasterPos3iv (theVec);
    OpenGl_TRACE(glRasterPos3iv)
  }

  inline void glRasterPos3sv (const GLshort*  theVec)
  {
    ::glRasterPos3sv (theVec);
    OpenGl_TRACE(glRasterPos3sv)
  }

  inline void glRasterPos4dv (const GLdouble* theVec)
  {
    ::glRasterPos4dv (theVec);
    OpenGl_TRACE(glRasterPos4dv)
  }

  inline void glRasterPos4fv (const GLfloat*  theVec)
  {
    ::glRasterPos4fv (theVec);
    OpenGl_TRACE(glRasterPos4fv)
  }

  inline void glRasterPos4iv (const GLint*    theVec)
  {
    ::glRasterPos4iv (theVec);
    OpenGl_TRACE(glRasterPos4iv)
  }

  inline void glRasterPos4sv (const GLshort*  theVec)
  {
    ::glRasterPos4sv (theVec);
    OpenGl_TRACE(glRasterPos4sv)
  }

  inline void glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
  {
    ::glRectd (x1, y1, x2, y2);
    OpenGl_TRACE(glRectd)
  }

  inline void glRectf (GLfloat  x1, GLfloat  y1, GLfloat  x2, GLfloat  y2)
  {
    ::glRectf (x1, y1, x2, y2);
    OpenGl_TRACE(glRectf)
  }

  inline void glRecti (GLint    x1, GLint    y1, GLint    x2, GLint    y2)
  {
    ::glRecti (x1, y1, x2, y2);
    OpenGl_TRACE(glRecti)
  }

  inline void glRects (GLshort  x1, GLshort  y1, GLshort  x2, GLshort  y2)
  {
    ::glRects (x1, y1, x2, y2);
    OpenGl_TRACE(glRects)
  }

  inline void glRectdv (const GLdouble* v1, const GLdouble* v2)
  {
    ::glRectdv (v1, v2);
    OpenGl_TRACE(glRectdv)
  }

  inline void glRectfv (const GLfloat*  v1, const GLfloat*  v2)
  {
    ::glRectfv (v1, v2);
    OpenGl_TRACE(glRectfv)
  }

  inline void glRectiv (const GLint*    v1, const GLint*    v2)
  {
    ::glRectiv (v1, v2);
    OpenGl_TRACE(glRectiv)
  }

  inline void glRectsv (const GLshort*  v1, const GLshort*  v2)
  {
    ::glRectsv (v1, v2);
    OpenGl_TRACE(glRectsv)
  }

public: //! @name Texture mapping (removed since 3.1)

  inline void glTexGend (GLenum coord, GLenum pname, GLdouble param)
  {
    ::glTexGend (coord, pname, param);
    OpenGl_TRACE(glTexGend)
  }

  inline void glTexGenf (GLenum coord, GLenum pname, GLfloat param)
  {
    ::glTexGenf (coord, pname, param);
    OpenGl_TRACE(glTexGenf)
  }

  inline void glTexGeni (GLenum coord, GLenum pname, GLint param)
  {
    ::glTexGeni (coord, pname, param);
    OpenGl_TRACE(glTexGeni)
  }

  inline void glTexGendv (GLenum coord, GLenum pname, const GLdouble* params)
  {
    ::glTexGendv (coord, pname, params);
    OpenGl_TRACE(glTexGendv)
  }

  inline void glTexGenfv (GLenum coord, GLenum pname, const GLfloat* params)
  {
    ::glTexGenfv (coord, pname, params);
    OpenGl_TRACE(glTexGenfv)
  }

  inline void glTexGeniv (GLenum coord, GLenum pname, const GLint* params)
  {
    ::glTexGeniv (coord, pname, params);
    OpenGl_TRACE(glTexGeniv)
  }

  inline void glGetTexGendv (GLenum coord, GLenum pname, GLdouble* params)
  {
    ::glGetTexGendv (coord, pname, params);
    OpenGl_TRACE(glGetTexGendv)
  }

  inline void glGetTexGenfv (GLenum coord, GLenum pname, GLfloat* params)
  {
    ::glGetTexGenfv (coord, pname, params);
    OpenGl_TRACE(glGetTexGenfv)
  }

  inline void glGetTexGeniv (GLenum coord, GLenum pname, GLint* params)
  {
    ::glGetTexGeniv (coord, pname, params);
    OpenGl_TRACE(glGetTexGeniv)
  }

public: //! @name Resident textures and priorities (removed since 3.1)

  inline void glPrioritizeTextures (GLsizei n, const GLuint* textures, const GLclampf* priorities)
  {
    ::glPrioritizeTextures (n, textures, priorities);
    OpenGl_TRACE(glPrioritizeTextures)
  }

  inline GLboolean glAreTexturesResident (GLsizei n, const GLuint* textures, GLboolean* residences)
  {
    const GLboolean aRes = ::glAreTexturesResident (n, textures, residences);
    OpenGl_TRACE(glAreTexturesResident)
    return aRes;
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

  inline void glPixelZoom (GLfloat xfactor, GLfloat yfactor)
  {
    ::glPixelZoom (xfactor, yfactor);
    OpenGl_TRACE(glPixelZoom)
  }

public: //! @name Fog and all associated parameters (removed since 3.1)

  inline void glFogf (GLenum pname, GLfloat param)
  {
    ::glFogf (pname, param);
    OpenGl_TRACE(glFogf)
  }

  inline void glFogi (GLenum pname, GLint param)
  {
    ::glFogi (pname, param);
    OpenGl_TRACE(glFogi)
  }

  inline void glFogfv (GLenum pname, const GLfloat* params)
  {
    ::glFogfv (pname, params);
    OpenGl_TRACE(glFogfv)
  }

  inline void glFogiv (GLenum pname, const GLint* params)
  {
    ::glFogiv (pname, params);
    OpenGl_TRACE(glFogiv)
  }

public: //! @name Evaluators (removed since 3.1)

  inline void glMap1d (GLenum target, GLdouble u1, GLdouble u2,
                       GLint stride,
                       GLint order, const GLdouble* points)
  {
    ::glMap1d (target, u1, u2, stride, order, points);
    OpenGl_TRACE(glMap1d)
  }

  inline void glMap1f (GLenum target, GLfloat u1, GLfloat u2,
                       GLint stride,
                       GLint order, const GLfloat* points)
  {
    ::glMap1f (target, u1, u2, stride, order, points);
    OpenGl_TRACE(glMap1f)
  }

  inline void glMap2d (GLenum target,
                       GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
                       GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
                       const GLdouble* points)
  {
    ::glMap2d (target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
    OpenGl_TRACE(glMap2d)
  }

  inline void glMap2f (GLenum target,
                       GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
                       GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
                       const GLfloat* points)
  {
    ::glMap2f (target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
    OpenGl_TRACE(glMap2f)
  }

  inline void glGetMapdv (GLenum target, GLenum query, GLdouble* v)
  {
    ::glGetMapdv (target, query, v);
    OpenGl_TRACE(glGetMapdv)
  }

  inline void glGetMapfv (GLenum target, GLenum query, GLfloat* v)
  {
    ::glGetMapfv (target, query, v);
    OpenGl_TRACE(glGetMapfv)
  }

  inline void glGetMapiv (GLenum target, GLenum query, GLint* v)
  {
    ::glGetMapiv (target, query, v);
    OpenGl_TRACE(glGetMapiv)
  }

  inline void glEvalCoord1d (GLdouble u)
  {
    ::glEvalCoord1d (u);
    OpenGl_TRACE(glEvalCoord1d)
  }

  inline void glEvalCoord1f (GLfloat u)
  {
    ::glEvalCoord1f (u);
    OpenGl_TRACE(glEvalCoord1f)
  }

  inline void glEvalCoord1dv (const GLdouble* u)
  {
    ::glEvalCoord1dv (u);
    OpenGl_TRACE(glEvalCoord1dv)
  }

  inline void glEvalCoord1fv (const GLfloat* u)
  {
    ::glEvalCoord1fv (u);
    OpenGl_TRACE(glEvalCoord1fv)
  }

  inline void glEvalCoord2d (GLdouble u, GLdouble v)
  {
    ::glEvalCoord2d (u, v);
    OpenGl_TRACE(glEvalCoord2d)
  }

  inline void glEvalCoord2f (GLfloat u, GLfloat v)
  {
    ::glEvalCoord2f (u, v);
    OpenGl_TRACE(glEvalCoord2f)
  }

  inline void glEvalCoord2dv (const GLdouble* u)
  {
    ::glEvalCoord2dv (u);
    OpenGl_TRACE(glEvalCoord2dv)
  }

  inline void glEvalCoord2fv (const GLfloat* u)
  {
    ::glEvalCoord2fv (u);
    OpenGl_TRACE(glEvalCoord2fv)
  }

  inline void glMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
  {
    ::glMapGrid1d (un, u1, u2);
    OpenGl_TRACE(glMapGrid1d)
  }

  inline void glMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
  {
    ::glMapGrid1f (un, u1, u2);
    OpenGl_TRACE(glMapGrid1f)
  }

  inline void glMapGrid2d (GLint un, GLdouble u1, GLdouble u2,
                           GLint vn, GLdouble v1, GLdouble v2)
  {
    ::glMapGrid2d (un, u1, u2, vn, v1, v2);
    OpenGl_TRACE(glMapGrid2d)
  }

  inline void glMapGrid2f (GLint un, GLfloat u1, GLfloat u2,
                           GLint vn, GLfloat v1, GLfloat v2)
  {
    ::glMapGrid2f (un, u1, u2, vn, v1, v2);
    OpenGl_TRACE(glMapGrid2f)
  }

  inline void glEvalPoint1 (GLint i)
  {
    ::glEvalPoint1 (i);
    OpenGl_TRACE(glEvalPoint1)
  }

  inline void glEvalPoint2 (GLint i, GLint j)
  {
    ::glEvalPoint2 (i, j);
    OpenGl_TRACE(glEvalPoint2)
  }

  inline void glEvalMesh1 (GLenum mode, GLint i1, GLint i2)
  {
    ::glEvalMesh1 (mode, i1, i2);
    OpenGl_TRACE(glEvalMesh1)
  }

  inline void glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
  {
    ::glEvalMesh2 (mode, i1, i2, j1, j2);
    OpenGl_TRACE(glEvalMesh2)
  }

public: //! @name Selection and feedback modes (removed since 3.1)

  inline void glFeedbackBuffer (GLsizei theSize, GLenum theType, GLfloat* theBuffer)
  {
    ::glFeedbackBuffer (theSize, theType, theBuffer);
    OpenGl_TRACE(glFeedbackBuffer)
  }

  inline void glPassThrough (GLfloat token)
  {
    ::glPassThrough (token);
    OpenGl_TRACE(glPassThrough)
  }

  inline void glSelectBuffer (GLsizei theSize, GLuint* theBuffer)
  {
    ::glSelectBuffer (theSize, theBuffer);
    OpenGl_TRACE(glSelectBuffer)
  }

  inline void glInitNames()
  {
    ::glInitNames();
    OpenGl_TRACE(glInitNames)
  }

  inline void glLoadName (GLuint theName)
  {
    ::glLoadName (theName);
    OpenGl_TRACE(glLoadName)
  }

  inline void glPushName (GLuint theName)
  {
    ::glPushName (theName);
    OpenGl_TRACE(glPushName)
  }

  inline void glPopName()
  {
    ::glPopName();
    OpenGl_TRACE(glPopName)
  }

public: //! @name Accumulation Buffer (removed since 3.1)

  inline void glClearAccum (GLfloat theRed, GLfloat theGreen, GLfloat theBlue, GLfloat theAlpha)
  {
    ::glClearAccum (theRed, theGreen, theBlue, theAlpha);
    OpenGl_TRACE(glClearAccum)
  }

  inline void glAccum (GLenum theOp, GLfloat theValue)
  {
    ::glAccum (theOp, theValue);
    OpenGl_TRACE(glAccum)
  }

public: //! @name Edge flags and fixed-function vertex processing (removed since 3.1)

  inline void glEdgeFlag (GLboolean theFlag)
  {
    ::glEdgeFlag (theFlag);
    OpenGl_TRACE(glEdgeFlag)
  }

  inline void glEdgeFlagv (const GLboolean* theFlag)
  {
    ::glEdgeFlagv (theFlag);
    OpenGl_TRACE(glEdgeFlagv)
  }

  inline void glIndexPointer (GLenum theType, GLsizei theStride, const GLvoid* thePtr)
  {
    ::glIndexPointer (theType, theStride, thePtr);
    OpenGl_TRACE(glIndexPointer)
  }

  inline void glEdgeFlagPointer (GLsizei theStride, const GLvoid* thePtr)
  {
    ::glEdgeFlagPointer (theStride, thePtr);
    OpenGl_TRACE(glEdgeFlagPointer)
  }

  inline void glGetPointerv (GLenum pname, GLvoid** params)
  {
    ::glGetPointerv(pname, params);
    OpenGl_TRACE(glGetPointerv)
  }

  inline void glInterleavedArrays (GLenum theFormat, GLsizei theStride, const GLvoid* thePointer)
  {
    ::glInterleavedArrays (theFormat, theStride, thePointer);
    OpenGl_TRACE(glInterleavedArrays)
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
