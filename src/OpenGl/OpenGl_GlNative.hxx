// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef OpenGl_GlNative_HeaderFile
#define OpenGl_GlNative_HeaderFile

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

#ifndef GL_APICALL
  #define GL_APICALL GLAPI
#endif

// exclude modern definitions and system-provided glext.h, should be defined before gl.h inclusion
#ifndef GL_GLEXT_LEGACY
  #define GL_GLEXT_LEGACY
#endif
#ifndef GLX_GLXEXT_LEGACY
  #define GLX_GLXEXT_LEGACY
#endif

// include main OpenGL header provided with system
#if defined(__APPLE__)
  #import <TargetConditionals.h>
  // macOS 10.4 deprecated OpenGL framework - suppress useless warnings
  #define GL_SILENCE_DEPRECATION
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    #include <OpenGLES/ES3/gl.h>
  #else
    #include <OpenGL/gl.h>
  #endif
  #define __X_GL_H // prevent chaotic gl.h inclusions to avoid compile errors
#elif defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
  #if defined(_WIN32)
    // Angle OpenGL ES headers do not define function prototypes even for core functions,
    // however OCCT is expected to be linked against libGLESv2
    #define GL_GLEXT_PROTOTYPES
  #endif
  #include <GLES3/gl3.h>
#else
  #include <GL/gl.h>
#endif

#endif // OpenGl_GlNative_HeaderFile
