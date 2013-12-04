// Created on: 2014-03-06
// Created by: Kirill GAVRILOV
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 2014-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_GlCore13_H__
#define _OpenGl_GlCore13_H__

#include <OpenGl_GlCore12.hxx>

//! Function list for GL1.3 core functionality.
struct OpenGl_GlCore13 : public OpenGl_GlCore12
{

  PFNGLACTIVETEXTUREPROC           glActiveTexture;
  PFNGLSAMPLECOVERAGEPROC          glSampleCoverage;
  PFNGLCOMPRESSEDTEXIMAGE3DPROC    glCompressedTexImage3D;
  PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D;
  PFNGLCOMPRESSEDTEXIMAGE1DPROC    glCompressedTexImage1D;
  PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
  PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage;

  // deprecated functions

  PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
  PFNGLMULTITEXCOORD1DPROC  glMultiTexCoord1d;
  PFNGLMULTITEXCOORD1DVPROC glMultiTexCoord1dv;
  PFNGLMULTITEXCOORD1FPROC  glMultiTexCoord1f;
  PFNGLMULTITEXCOORD1FVPROC glMultiTexCoord1fv;
  PFNGLMULTITEXCOORD1IPROC  glMultiTexCoord1i;
  PFNGLMULTITEXCOORD1IVPROC glMultiTexCoord1iv;
  PFNGLMULTITEXCOORD1SPROC  glMultiTexCoord1s;
  PFNGLMULTITEXCOORD1SVPROC glMultiTexCoord1sv;
  PFNGLMULTITEXCOORD2DPROC  glMultiTexCoord2d;
  PFNGLMULTITEXCOORD2DVPROC glMultiTexCoord2dv;
  PFNGLMULTITEXCOORD2FPROC  glMultiTexCoord2f;
  PFNGLMULTITEXCOORD2FVPROC glMultiTexCoord2fv;
  PFNGLMULTITEXCOORD2IPROC  glMultiTexCoord2i;
  PFNGLMULTITEXCOORD2IVPROC glMultiTexCoord2iv;
  PFNGLMULTITEXCOORD2SPROC  glMultiTexCoord2s;
  PFNGLMULTITEXCOORD2SVPROC glMultiTexCoord2sv;
  PFNGLMULTITEXCOORD3DPROC  glMultiTexCoord3d;
  PFNGLMULTITEXCOORD3DVPROC glMultiTexCoord3dv;
  PFNGLMULTITEXCOORD3FPROC  glMultiTexCoord3f;
  PFNGLMULTITEXCOORD3FVPROC glMultiTexCoord3fv;
  PFNGLMULTITEXCOORD3IPROC  glMultiTexCoord3i;
  PFNGLMULTITEXCOORD3IVPROC glMultiTexCoord3iv;
  PFNGLMULTITEXCOORD3SPROC  glMultiTexCoord3s;
  PFNGLMULTITEXCOORD3SVPROC glMultiTexCoord3sv;
  PFNGLMULTITEXCOORD4DPROC  glMultiTexCoord4d;
  PFNGLMULTITEXCOORD4DVPROC glMultiTexCoord4dv;
  PFNGLMULTITEXCOORD4FPROC  glMultiTexCoord4f;
  PFNGLMULTITEXCOORD4FVPROC glMultiTexCoord4fv;
  PFNGLMULTITEXCOORD4IPROC  glMultiTexCoord4i;
  PFNGLMULTITEXCOORD4IVPROC glMultiTexCoord4iv;
  PFNGLMULTITEXCOORD4SPROC  glMultiTexCoord4s;
  PFNGLMULTITEXCOORD4SVPROC glMultiTexCoord4sv;
  PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;
  PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;
  PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;
  PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;

};

#endif // _OpenGl_GlCore13_H__
