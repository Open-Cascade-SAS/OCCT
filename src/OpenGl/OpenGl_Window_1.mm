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

  const NSOpenGLPixelFormatAttribute aDummyAttrib = NSOpenGLPFACompliant;
  NSOpenGLPixelFormatAttribute anAttribs[] = {
    theCaps->contextStereo ? NSOpenGLPFAStereo : aDummyAttrib,
    //NSOpenGLPFAColorSize,  32,
    NSOpenGLPFADepthSize,    24,
    NSOpenGLPFAStencilSize,  8,
    NSOpenGLPFADoubleBuffer,
    theCaps->contextNoAccel ? NSOpenGLPFARendererID : NSOpenGLPFAAccelerated,
    theCaps->contextNoAccel ? (NSOpenGLPixelFormatAttribute )kCGLRendererGenericFloatID : 0,
    0
  };

  // all GL context within one OpenGl_GraphicDriver should be shared!
  NSOpenGLContext*     aGLCtxShare = theShareCtx.IsNull() ? NULL : (NSOpenGLContext* )theShareCtx->myGContext;
  NSOpenGLPixelFormat* aGLFormat   = [[[NSOpenGLPixelFormat alloc] initWithAttributes: anAttribs] autorelease];
  NSOpenGLContext*     aGLContext  = [[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                                                shareContext: aGLCtxShare];
  if (aGLContext == NULL
   && theCaps->contextStereo)
  {
    anAttribs[0] = aDummyAttrib;
    aGLFormat    = [[[NSOpenGLPixelFormat alloc] initWithAttributes: anAttribs] autorelease];
    aGLContext   = [[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                              shareContext: aGLCtxShare];
  }
  if (aGLContext == NULL)
  {
    TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: NSOpenGLContext creation failed");
    Aspect_GraphicDeviceDefinitionError::Raise (aMsg.ToCString());
    return;
  }

  NSView* aView = (NSView* )theCWindow.XWindow;
  [aGLContext setView: aView];

  myGlContext->Init (aGLContext);
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
