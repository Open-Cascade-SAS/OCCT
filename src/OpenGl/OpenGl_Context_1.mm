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

#define GL_GLEXT_LEGACY // To prevent inclusion of system glext.h on Mac OS X 10.6.8

#import <Cocoa/Cocoa.h>

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_Context.hxx>

#include <Standard_ProgramError.hxx>

// =======================================================================
// function : IsCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::IsCurrent() const
{
  return myGContext != NULL
      && [NSOpenGLContext currentContext] == (NSOpenGLContext* )myGContext;
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

  [(NSOpenGLContext* )myGContext makeCurrentContext];
  return Standard_True;
}

// =======================================================================
// function : SwapBuffers
// purpose  :
// =======================================================================
void OpenGl_Context::SwapBuffers()
{
  if (myGContext != NULL)
  {
    glFinish();
    [(NSOpenGLContext* )myGContext flushBuffer];
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::Init()
{
  if (myIsInitialized)
  {
    return Standard_True;
  }

  myGContext = [NSOpenGLContext currentContext];
  if (myGContext == NULL)
  {
    return Standard_False;
  }

  init();
  myIsInitialized = Standard_True;
  return Standard_True;
}

#endif // __APPLE__
