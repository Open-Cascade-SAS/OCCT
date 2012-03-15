// File:      OpenGl_GlCore11.hxx
// Created:   06 March 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

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

// current TKOpenGl implementation is incompatible with native OpenGL on MacOS X
#define MACOSX_USE_GLX

// include main OpenGL header provided with system
#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #define GL_GLEXT_LEGACY // exclude modern definitions
  #include <OpenGL/OpenGL.h>
#else
  #include <GL/gl.h>
#endif

#include <InterfaceGraphic.hxx>
#include <InterfaceGraphic_tgl_all.hxx>
#include <InterfaceGraphic_telem.hxx>

#endif // _OpenGl_GlCore11_H__
