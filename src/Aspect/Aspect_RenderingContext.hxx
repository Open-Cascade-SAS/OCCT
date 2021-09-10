// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

/*============================================================================*/
/*==== Title: Aspect_RenderingContext.hxx                                     */
/*==== Role: The header file of primitive type "RenderingContext" from package*/
/*==== "V3d"                                                                  */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/
// To manage 2D or 3D graphic context

#ifndef _Aspect_RenderingContext_HeaderFile
#define _Aspect_RenderingContext_HeaderFile

#include <Standard_Macro.hxx>

#if defined(__APPLE__) && !defined(HAVE_XLIB)
  #import <TargetConditionals.h>
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    #ifdef __OBJC__
      @class EAGLContext;
    #else
      struct EAGLContext;
    #endif
    typedef EAGLContext* Aspect_RenderingContext;
  #else
    #ifdef __OBJC__
      @class NSOpenGLContext;
    #else
      struct NSOpenGLContext;
    #endif
    Standard_DISABLE_DEPRECATION_WARNINGS
    typedef NSOpenGLContext* Aspect_RenderingContext;
    Standard_ENABLE_DEPRECATION_WARNINGS
  #endif
#else
  typedef void* Aspect_RenderingContext; // GLXContext under UNIX
#endif

#endif /* _Aspect_RenderingContext_HeaderFile */
