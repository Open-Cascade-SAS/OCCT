// Created on: 2012-11-12
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

#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)

#ifndef GL_GLEXT_LEGACY
#define GL_GLEXT_LEGACY // To prevent inclusion of system glext.h on Mac OS X 10.6.8
#endif

#import <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  #import <UIKit/UIKit.h>
#else
  #import <Cocoa/Cocoa.h>
#endif

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_FrameBuffer.hxx>

#include <Standard_ProgramError.hxx>

// =======================================================================
// function : IsCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::IsCurrent() const
{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  return myGContext != NULL
      && [EAGLContext     currentContext] == myGContext;
#else
  return myGContext != NULL
      && [NSOpenGLContext currentContext] == myGContext;
#endif
}

// =======================================================================
// function : MakeCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::MakeCurrent()
{
  if (myGContext == NULL)
  {
    Standard_ProgramError_Raise_if (myIsInitialized, "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  return [EAGLContext setCurrentContext: myGContext] == YES;
#else
  [myGContext makeCurrentContext];
  return Standard_True;
#endif
}

// =======================================================================
// function : SwapBuffers
// purpose  :
// =======================================================================
void OpenGl_Context::SwapBuffers()
{
  if (myGContext == NULL)
  {
    return;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  if (myDefaultFbo.IsNull()
  || !myDefaultFbo->IsValid()
  ||  myDefaultFbo->ColorRenderBuffer() == 0)
  {
    return;
  }

  ::glBindRenderbuffer (GL_RENDERBUFFER, myDefaultFbo->ColorRenderBuffer());
  [myGContext presentRenderbuffer: GL_RENDERBUFFER];
  //::glBindRenderbuffer (GL_RENDERBUFFER, 0);
#else
  glFinish();
  [myGContext flushBuffer];
#endif
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::Init (const Standard_Boolean theIsCoreProfile)
{
  if (myIsInitialized)
  {
    return Standard_True;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  myGContext = [EAGLContext     currentContext];
#else
  myGContext = [NSOpenGLContext currentContext];
#endif
  if (myGContext == NULL)
  {
    return Standard_False;
  }

  init (theIsCoreProfile);
  myIsInitialized = Standard_True;
  return Standard_True;
}

#endif // __APPLE__
