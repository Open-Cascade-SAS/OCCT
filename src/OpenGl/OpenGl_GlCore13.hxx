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

#ifndef _OpenGl_GlCore13_Header
#define _OpenGl_GlCore13_Header

#include <OpenGl_GlCore12.hxx>

//! OpenGL 1.3 without deprecated entry points.
struct OpenGl_GlCore13Fwd : public OpenGl_GlCore12Fwd
{

public: //! @name OpenGL 1.3 additives to 1.2

#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glCompressedTexImage3D;
  using OpenGl_GlFunctions::glCompressedTexImage1D;
  using OpenGl_GlFunctions::glCompressedTexSubImage3D;
  using OpenGl_GlFunctions::glCompressedTexSubImage1D;
  using OpenGl_GlFunctions::glGetCompressedTexImage;
#endif

  using OpenGl_GlFunctions::glActiveTexture;
  using OpenGl_GlFunctions::glSampleCoverage;
  using OpenGl_GlFunctions::glCompressedTexImage2D;
  using OpenGl_GlFunctions::glCompressedTexSubImage2D;

};

//! OpenGL 1.3 core based on 1.2 version.
struct OpenGl_GlCore13 : public OpenGl_GlCore12
{

public: //! @name OpenGL 1.3 additives to 1.2

#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glCompressedTexImage3D;
  using OpenGl_GlFunctions::glCompressedTexImage1D;
  using OpenGl_GlFunctions::glCompressedTexSubImage3D;
  using OpenGl_GlFunctions::glCompressedTexSubImage1D;
  using OpenGl_GlFunctions::glGetCompressedTexImage;
#endif

  using OpenGl_GlFunctions::glActiveTexture;
  using OpenGl_GlFunctions::glSampleCoverage;
  using OpenGl_GlFunctions::glCompressedTexImage2D;
  using OpenGl_GlFunctions::glCompressedTexSubImage2D;

#if !defined(GL_ES_VERSION_2_0)

public: //! @name Begin/End primitive specification (removed since 3.1)

  using OpenGl_GlFunctions::glMultiTexCoord1d;
  using OpenGl_GlFunctions::glMultiTexCoord1dv;
  using OpenGl_GlFunctions::glMultiTexCoord1f;
  using OpenGl_GlFunctions::glMultiTexCoord1fv;
  using OpenGl_GlFunctions::glMultiTexCoord1i;
  using OpenGl_GlFunctions::glMultiTexCoord1iv;
  using OpenGl_GlFunctions::glMultiTexCoord1s;
  using OpenGl_GlFunctions::glMultiTexCoord1sv;
  using OpenGl_GlFunctions::glMultiTexCoord2d;
  using OpenGl_GlFunctions::glMultiTexCoord2dv;
  using OpenGl_GlFunctions::glMultiTexCoord2f;
  using OpenGl_GlFunctions::glMultiTexCoord2fv;
  using OpenGl_GlFunctions::glMultiTexCoord2i;
  using OpenGl_GlFunctions::glMultiTexCoord2iv;
  using OpenGl_GlFunctions::glMultiTexCoord2s;
  using OpenGl_GlFunctions::glMultiTexCoord2sv;
  using OpenGl_GlFunctions::glMultiTexCoord3d;
  using OpenGl_GlFunctions::glMultiTexCoord3dv;
  using OpenGl_GlFunctions::glMultiTexCoord3f;
  using OpenGl_GlFunctions::glMultiTexCoord3fv;
  using OpenGl_GlFunctions::glMultiTexCoord3i;
  using OpenGl_GlFunctions::glMultiTexCoord3iv;
  using OpenGl_GlFunctions::glMultiTexCoord3s;
  using OpenGl_GlFunctions::glMultiTexCoord3sv;
  using OpenGl_GlFunctions::glMultiTexCoord4d;
  using OpenGl_GlFunctions::glMultiTexCoord4dv;
  using OpenGl_GlFunctions::glMultiTexCoord4f;
  using OpenGl_GlFunctions::glMultiTexCoord4fv;
  using OpenGl_GlFunctions::glMultiTexCoord4i;
  using OpenGl_GlFunctions::glMultiTexCoord4iv;
  using OpenGl_GlFunctions::glMultiTexCoord4s;
  using OpenGl_GlFunctions::glMultiTexCoord4sv;

  using OpenGl_GlFunctions::glClientActiveTexture;

public: //! @name Matrix operations (removed since 3.1)

  using OpenGl_GlFunctions::glLoadTransposeMatrixf;
  using OpenGl_GlFunctions::glLoadTransposeMatrixd;
  using OpenGl_GlFunctions::glMultTransposeMatrixf;
  using OpenGl_GlFunctions::glMultTransposeMatrixd;

#endif

};

#endif // _OpenGl_GlCore13_Header
