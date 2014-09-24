// Created on: 2013-08-25
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_ArbDbg_H__
#define _OpenGl_ArbDbg_H__

#include <OpenGl_GlFunctions.hxx>

//! Debug context routines
struct OpenGl_ArbDbg : protected OpenGl_GlFunctions
{
#if !defined(GL_ES_VERSION_2_0)
  using OpenGl_GlFunctions::glDebugMessageControlARB;
  using OpenGl_GlFunctions::glDebugMessageInsertARB;
  using OpenGl_GlFunctions::glDebugMessageCallbackARB;
  using OpenGl_GlFunctions::glGetDebugMessageLogARB;
#endif
};

#endif // _OpenGl_ArbDbg_H__
