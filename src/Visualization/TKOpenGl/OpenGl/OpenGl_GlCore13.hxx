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

#ifndef OpenGl_GlCore13_HeaderFile
#define OpenGl_GlCore13_HeaderFile

#include <OpenGl_GlCore12.hxx>

//! OpenGL 1.3 without deprecated entry points.
struct OpenGl_GlCore13 : public OpenGl_GlCore12
{

public: //! @name OpenGL 1.3 additives to 1.2
#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glCompressedTexImage1D;
  using OpenGl_GlFunctions::glCompressedTexImage3D;
  using OpenGl_GlFunctions::glCompressedTexSubImage1D;
  using OpenGl_GlFunctions::glCompressedTexSubImage3D;
  using OpenGl_GlFunctions::glGetCompressedTexImage;
#endif

  using OpenGl_GlFunctions::glActiveTexture;
  using OpenGl_GlFunctions::glCompressedTexImage2D;
  using OpenGl_GlFunctions::glCompressedTexSubImage2D;
  using OpenGl_GlFunctions::glSampleCoverage;
};

#endif // _OpenGl_GlCore13_Header
