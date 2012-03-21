// Created on: 2012-03-06
// Created by: Kirill GAVRILOV
// Copyright (c) -1999 Matra Datavision
// Copyright (c) 2012-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
