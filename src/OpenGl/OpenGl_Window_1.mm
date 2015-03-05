// Created on: 2012-11-12
// Created by: Kirill Gavrilov
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

#include <InterfaceGraphic.hxx>

#include <OpenGl_Window.hxx>

#include <OpenGl_Context.hxx>
#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Cocoa_LocalPool.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <OpenGL/CGLRenderers.h>

namespace
{
  static const TEL_COLOUR THE_DEFAULT_BG_COLOR = { { 0.F, 0.F, 0.F, 1.F } };
};

// =======================================================================
// function : OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::OpenGl_Window (const Handle(OpenGl_GraphicDriver)& theDriver,
                              const CALL_DEF_WINDOW&        theCWindow,
                              Aspect_RenderingContext       theGContext,
                              const Handle(OpenGl_Caps)&    theCaps,
                              const Handle(OpenGl_Context)& theShareCtx)
: myGlContext (new OpenGl_Context (theCaps)),
  myOwnGContext (theGContext == 0),
  myWidth  ((Standard_Integer )theCWindow.dx),
  myHeight ((Standard_Integer )theCWindow.dy),
  myBgColor (THE_DEFAULT_BG_COLOR)
{
  myBgColor.rgb[0] = theCWindow.Background.r;
  myBgColor.rgb[1] = theCWindow.Background.g;
  myBgColor.rgb[2] = theCWindow.Background.b;

  Cocoa_LocalPool aLocalPool;
  //NSOpenGLContext* aGContext = (NSOpenGLContext* )theGContext;

  // all GL context within one OpenGl_GraphicDriver should be shared!
  NSOpenGLContext* aGLCtxShare = theShareCtx.IsNull() ? NULL : (NSOpenGLContext* )theShareCtx->myGContext;
  NSOpenGLContext* aGLContext  = NULL;

  NSOpenGLPixelFormatAttribute anAttribs[32] = {};
  Standard_Integer aLastAttrib = 0;
  //anAttribs[aLastAttrib++] = NSOpenGLPFAColorSize;    anAttribs[aLastAttrib++] = 32,
  anAttribs[aLastAttrib++] = NSOpenGLPFADepthSize;    anAttribs[aLastAttrib++] = 24;
  anAttribs[aLastAttrib++] = NSOpenGLPFAStencilSize;  anAttribs[aLastAttrib++] = 8;
  anAttribs[aLastAttrib++] = NSOpenGLPFADoubleBuffer;
  if (theCaps->contextNoAccel)
  {
    anAttribs[aLastAttrib++] = NSOpenGLPFARendererID;
    anAttribs[aLastAttrib++] = (NSOpenGLPixelFormatAttribute )kCGLRendererGenericFloatID;
  }
  else
  {
    anAttribs[aLastAttrib++] = NSOpenGLPFAAccelerated;
  }
  anAttribs[aLastAttrib] = 0;
  const Standard_Integer aLastMainAttrib = aLastAttrib;
  Standard_Integer aTryCore   = 0;
  Standard_Integer aTryStereo = 0;
  for (aTryCore = 1; aTryCore >= 0; --aTryCore)
  {
    aLastAttrib = aLastMainAttrib;
    if (aTryCore == 1)
    {
      if (theCaps->contextCompatible)
      {
        continue;
      }

      // supported since OS X 10.7+
      anAttribs[aLastAttrib++] = 99;     // NSOpenGLPFAOpenGLProfile
      anAttribs[aLastAttrib++] = 0x3200; // NSOpenGLProfileVersion3_2Core
    }

    for (aTryStereo = 1; aTryStereo >= 0; --aTryStereo)
    {
      if (aTryStereo == 1)
      {
        if (!theCaps->contextStereo)
        {
          continue;
        }
        anAttribs[aLastAttrib++] = NSOpenGLPFAStereo;
      }

      anAttribs[aLastAttrib] = 0;

      NSOpenGLPixelFormat* aGLFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes: anAttribs] autorelease];
      aGLContext = [[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                              shareContext: aGLCtxShare];
      if (aGLContext != NULL)
      {
        break;
      }
    }

    if (aGLContext != NULL)
    {
      break;
    }
  }

  if (aGLContext == NULL)
  {
    TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: NSOpenGLContext creation failed");
    Aspect_GraphicDeviceDefinitionError::Raise (aMsg.ToCString());
    return;
  }

  if (aTryStereo == 0
   && theCaps->contextStereo)
  {
    TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: QuadBuffer is unavailable!");
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_OTHER_ARB, 0, GL_DEBUG_SEVERITY_LOW_ARB, aMsg);
  }
  if (aTryCore == 0
  && !theCaps->contextCompatible)
  {
    TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: core profile creation failed.");
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_PORTABILITY_ARB, 0, GL_DEBUG_SEVERITY_LOW_ARB, aMsg);
  }

  NSView* aView = (NSView* )theCWindow.XWindow;
  [aGLContext setView: aView];

  myGlContext->Init (aGLContext, aTryCore == 1);
  myGlContext->Share (theShareCtx);
  Init();
}

// =======================================================================
// function : ~OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::~OpenGl_Window()
{
  NSOpenGLContext* aGLCtx = (NSOpenGLContext* )myGlContext->myGContext;
  myGlContext.Nullify();

  [NSOpenGLContext clearCurrentContext];
  if (myOwnGContext)
  {
    [aGLCtx clearDrawable];
    [aGLCtx release];
  }
}

// =======================================================================
// function : Resize
// purpose  : call_subr_resize
// =======================================================================
void OpenGl_Window::Resize (const CALL_DEF_WINDOW& theCWindow)
{
  // If the size is not changed - do nothing
  if (myWidth  == (Standard_Integer )theCWindow.dx
   && myHeight == (Standard_Integer )theCWindow.dy)
  {
    return;
  }

  myWidth  = (Standard_Integer )theCWindow.dx;
  myHeight = (Standard_Integer )theCWindow.dy;

  Init();
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Window::Init()
{
  if (!Activate())
  {
    return;
  }

  NSOpenGLContext* aGLCtx = (NSOpenGLContext* )myGlContext->myGContext;
  NSRect aBounds = [[aGLCtx view] bounds];

  // we should call this method each time when window is resized
  [aGLCtx update];

  myWidth  = Standard_Integer(aBounds.size.width);
  myHeight = Standard_Integer(aBounds.size.height);

  glMatrixMode (GL_MODELVIEW);
  glViewport (0, 0, myWidth, myHeight);

  glDisable (GL_SCISSOR_TEST);
  glDrawBuffer (GL_BACK);
}

#endif // __APPLE__
