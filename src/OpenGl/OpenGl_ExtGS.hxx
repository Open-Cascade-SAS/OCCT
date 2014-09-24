// Created on: 2012-09-26
// Created by: Olga SURYANINOVA
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

#ifndef _OpenGl_ExtGS_H__
#define _OpenGl_ExtGS_H__

#include <OpenGl_GlFunctions.hxx>

//! Geometry shader as extension is available on OpenGL 2.0+
struct OpenGl_ExtGS : protected OpenGl_GlFunctions
{
#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glProgramParameteriEXT;
#endif
};

#endif // _OpenGl_ExtGS_H__
