// Created on: 2012-11-12
// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)

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
