// Created on: 2012-11-12
// Created by: Kirill Gavrilov
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

#include <InterfaceGraphic.hxx>

#include <OpenGl_Window.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_Display.hxx>
#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Cocoa_LocalPool.hxx>
#include <TCollection_AsciiString.hxx>

namespace
{
  static const TEL_COLOUR THE_DEFAULT_BG_COLOR = { { 0.F, 0.F, 0.F, 1.F } };
  static const NSOpenGLPixelFormatAttribute THE_DOUBLE_BUFF[] = {
    //NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute )32,
    NSOpenGLPFADepthSize,   (NSOpenGLPixelFormatAttribute )24,
    NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute )8,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    0
  };

};

// =======================================================================
// function : OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::OpenGl_Window (const Handle(OpenGl_Display)& theDisplay,
                              const CALL_DEF_WINDOW&        theCWindow,
                              Aspect_RenderingContext       theGContext,
                              const Handle(OpenGl_Context)& theShareCtx)
: myDisplay (theDisplay),
  myGlContext (new OpenGl_Context()),
  myOwnGContext (theGContext == 0),
  myWidth ((Standard_Integer )theCWindow.dx),
  myHeight ((Standard_Integer )theCWindow.dy),
  myBgColor (THE_DEFAULT_BG_COLOR),
  myDither (theDisplay->Dither()),
  myBackDither (theDisplay->BackDither())
{
  myBgColor.rgb[0] = theCWindow.Background.r;
  myBgColor.rgb[1] = theCWindow.Background.g;
  myBgColor.rgb[2] = theCWindow.Background.b;

  Cocoa_LocalPool aLocalPool;
  //NSOpenGLContext* aGContext = (NSOpenGLContext* )theGContext;

  // all GL context within one OpenGl_GraphicDriver should be shared!
  NSOpenGLContext*     aGLCtxShare = theShareCtx.IsNull() ? NULL : (NSOpenGLContext* )theShareCtx->myGContext;
  NSOpenGLPixelFormat* aGLFormat   = [[[NSOpenGLPixelFormat alloc] initWithAttributes: THE_DOUBLE_BUFF] autorelease];
  NSOpenGLContext*     aGLContext  = [[NSOpenGLContext alloc] initWithFormat: aGLFormat
                                                                shareContext: aGLCtxShare];
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
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  if (aDisp == NULL)
  {
    return;
  }

  // If the size is not changed - do nothing
  if ((myWidth == theCWindow.dx) && (myHeight == theCWindow.dy))
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
