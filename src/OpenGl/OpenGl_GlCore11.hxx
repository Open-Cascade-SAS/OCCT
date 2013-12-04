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

#ifndef _OpenGl_GlCore11_H__
#define _OpenGl_GlCore11_H__

// required for correct APIENTRY definition
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifndef APIENTRY
  #define APIENTRY
#endif
#ifndef APIENTRYP
  #define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
  #define GLAPI extern
#endif

// exclude modern definitions and system-provided glext.h, should be defined before gl.h inclusion
#define GL_GLEXT_LEGACY

// include main OpenGL header provided with system
#if defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #define __X_GL_H // prevent chaotic gl.h inclusions to avoid compile errors
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif

#include <InterfaceGraphic.hxx>
#include <InterfaceGraphic_tgl_all.hxx>
#include <InterfaceGraphic_telem.hxx>

#endif // _OpenGl_GlCore11_H__
