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

#if defined(__APPLE__) && !defined(HAVE_XLIB)

#ifndef GL_GLEXT_LEGACY
#define GL_GLEXT_LEGACY // To prevent inclusion of system glext.h on Mac OS X 10.6.8
#endif

// macOS 10.4 deprecated OpenGL framework - suppress useless warnings
#define GL_SILENCE_DEPRECATION

#import <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  #import <UIKit/UIKit.h>
#else
  #import <Cocoa/Cocoa.h>

#if !defined(MAC_OS_X_VERSION_10_7) || (MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_7)
@interface NSView (LionAPI)
- (NSSize )convertSizeToBacking: (NSSize )theSize;
@end
#endif

#endif

#include <OpenGl_Window.hxx>
#include <OpenGl_FrameBuffer.hxx>

#include <OpenGl_Context.hxx>
#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Cocoa_LocalPool.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //
#else
  #include <OpenGL/CGLRenderers.h>
#endif

// =======================================================================
// function : OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::OpenGl_Window()
: myOwnGContext (false),
  mySwapInterval (0)
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Window::Init (const Handle(OpenGl_GraphicDriver)& theDriver,
                          const Handle(Aspect_Window)&  thePlatformWindow,
                          const Handle(Aspect_Window)&  theSizeWindow,
                          Aspect_RenderingContext       theGContext,
                          const Handle(OpenGl_Caps)&    theCaps,
                          const Handle(OpenGl_Context)& theShareCtx)
{
  myGlContext = new OpenGl_Context (theCaps);
  myOwnGContext = (theGContext == 0);
  myPlatformWindow = thePlatformWindow;
  mySizeWindow = theSizeWindow;
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  myUIView = NULL;
#endif
  mySwapInterval = theCaps->swapInterval;

  (void )theDriver;
  mySizeWindow->Size (mySize.x(), mySize.y());

#if defined(__APPLE__)
  mySizePt = mySize;
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  EAGLContext* aGLContext = theGContext;
  if (aGLContext == NULL)
  {
    void* aViewPtr = (void* )myPlatformWindow->NativeHandle();

    myUIView = (__bridge UIView* )aViewPtr;
    CAEAGLLayer* anEaglLayer = (CAEAGLLayer* )myUIView.layer;
    anEaglLayer.opaque = TRUE;
    anEaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool: FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8,            kEAGLDrawablePropertyColorFormat,
                                        NULL];

    aGLContext = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES3];
    if (aGLContext == NULL
    || ![EAGLContext setCurrentContext: aGLContext])
    {
      aGLContext = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];

      if (aGLContext == NULL
      || ![EAGLContext setCurrentContext: aGLContext])
      {
        TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: EAGLContext creation failed");
        throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
      }
    }

    myGlContext->Init (aGLContext, Standard_False);
  }
  else
  {
    if (![EAGLContext setCurrentContext: aGLContext])
    {
      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: EAGLContext can not be assigned");
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
    }

    myGlContext->Init (aGLContext, Standard_False);
  }
#else

  Cocoa_LocalPool aLocalPool;

  // all GL context within one OpenGl_GraphicDriver should be shared!
  NSOpenGLContext* aGLCtxShare = theShareCtx.IsNull() ? NULL : theShareCtx->myGContext;
  NSOpenGLContext* aGLContext  = theGContext;
  bool isCore = false;
  if (aGLContext == NULL)
  {
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

          // deprecated since macOS 10.12 without replacement
          Standard_DISABLE_DEPRECATION_WARNINGS
          anAttribs[aLastAttrib++] = NSOpenGLPFAStereo;
          Standard_ENABLE_DEPRECATION_WARNINGS
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
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
    }

    if (aTryStereo == 0
     && theCaps->contextStereo)
    {
      TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: QuadBuffer is unavailable!");
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
    }
    if (aTryCore == 0
    && !theCaps->contextCompatible)
    {
      TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: core profile creation failed.");
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
    }

    NSView* aView = (NSView* )myPlatformWindow->NativeHandle();
  Standard_DISABLE_DEPRECATION_WARNINGS
    [aGLContext setView: aView];
  Standard_ENABLE_DEPRECATION_WARNINGS
    isCore = (aTryCore == 1);
  }

  myGlContext->Init (aGLContext, isCore);
#endif

  myGlContext->Share (theShareCtx);
  myGlContext->SetSwapInterval (mySwapInterval);
  init();
}

// =======================================================================
// function : ~OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::~OpenGl_Window()
{
  if (!myOwnGContext
   ||  myGlContext.IsNull())
  {
    myGlContext.Nullify();
    return;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  myGlContext.Nullify();
  [EAGLContext setCurrentContext: NULL];
  myUIView = NULL;
#else
  NSOpenGLContext* aGLCtx = myGlContext->myGContext;
  myGlContext.Nullify();

  [NSOpenGLContext clearCurrentContext];
  [aGLCtx clearDrawable];
  [aGLCtx release];
#endif
}

// =======================================================================
// function : Resize
// purpose  : call_subr_resize
// =======================================================================
void OpenGl_Window::Resize()
{
  // If the size is not changed - do nothing
  Graphic3d_Vec2i aWinSize;
  mySizeWindow->Size (aWinSize.x(), aWinSize.y());
  if (myPlatformWindow->IsVirtual()
   || mySizeWindow != myPlatformWindow)
  {
    if (mySize == aWinSize)
    {
      return;
    }
    mySize = aWinSize;
  }
  else if (mySizePt == aWinSize)
  {
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    return;
  #else
    // check backing store change (moving to another screen)
    NSOpenGLContext* aGLCtx = myGlContext->myGContext;
  Standard_DISABLE_DEPRECATION_WARNINGS
    NSView* aView = [aGLCtx view];
  Standard_ENABLE_DEPRECATION_WARNINGS
    if (![aView respondsToSelector: @selector(convertSizeToBacking:)])
    {
      return;
    }

    NSRect aBounds = [aView bounds];
    NSSize aRes    = [aView convertSizeToBacking: aBounds.size];
    if (mySize.x() == Standard_Integer(aRes.width)
     && mySize.y() == Standard_Integer(aRes.height))
    {
      return;
    }
  #endif
  }

  mySizePt = aWinSize;

  init();
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void OpenGl_Window::init()
{
  if (!Activate())
  {
    return;
  }

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  Handle(OpenGl_FrameBuffer) aDefFbo = myGlContext->SetDefaultFrameBuffer (NULL);
  if (!aDefFbo.IsNull())
  {
    aDefFbo->Release (myGlContext.operator->());
  }
  else
  {
    aDefFbo = new OpenGl_FrameBuffer();
  }

  if (myOwnGContext)
  {
    EAGLContext* aGLCtx      = myGlContext->myGContext;
    CAEAGLLayer* anEaglLayer = (CAEAGLLayer* )myUIView.layer;
    GLuint aWinRBColor = 0;
    myGlContext->Functions()->glGenRenderbuffers (1, &aWinRBColor);
    myGlContext->Functions()->glBindRenderbuffer (GL_RENDERBUFFER, aWinRBColor);
    [aGLCtx renderbufferStorage: GL_RENDERBUFFER fromDrawable: anEaglLayer];
    myGlContext->Functions()->glGetRenderbufferParameteriv (GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,  &mySize.x());
    myGlContext->Functions()->glGetRenderbufferParameteriv (GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mySize.y());
    myGlContext->Functions()->glBindRenderbuffer (GL_RENDERBUFFER, 0);

    if (!aDefFbo->InitWithRB (myGlContext, mySize, GL_RGBA8, GL_DEPTH24_STENCIL8, aWinRBColor))
    {
      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: default FBO creation failed");
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
      return;
    }
  }
  else
  {
    if (!aDefFbo->InitWrapper (myGlContext))
    {
      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: default FBO wrapper creation failed");
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
      return;
    }

    mySize.x() = aDefFbo->GetVPSizeX();
    mySize.y() = aDefFbo->GetVPSizeY();
  }
  myGlContext->SetDefaultFrameBuffer (aDefFbo);
  aDefFbo->BindBuffer (myGlContext);
  aDefFbo.Nullify();
#else
  if (!myPlatformWindow->IsVirtual()
    && mySizeWindow == myPlatformWindow)
  {
    NSOpenGLContext* aGLCtx  = myGlContext->myGContext;
  Standard_DISABLE_DEPRECATION_WARNINGS
    NSView*          aView   = [aGLCtx view];
  Standard_ENABLE_DEPRECATION_WARNINGS
    NSRect           aBounds = [aView bounds];

    // we should call this method each time when window is resized
    [aGLCtx update];

    if ([aView respondsToSelector: @selector(convertSizeToBacking:)])
    {
      NSSize aRes = [aView convertSizeToBacking: aBounds.size];
      mySize.x() = Standard_Integer(aRes.width);
      mySize.y() = Standard_Integer(aRes.height);
    }
    else
    {
      mySize.x() = Standard_Integer(aBounds.size.width);
      mySize.y() = Standard_Integer(aBounds.size.height);
    }
    mySizePt.x() = Standard_Integer(aBounds.size.width);
    mySizePt.y() = Standard_Integer(aBounds.size.height);
  }
#endif

  myGlContext->core11fwd->glDisable (GL_DITHER);
  myGlContext->core11fwd->glDisable (GL_SCISSOR_TEST);
  const Standard_Integer aViewport[4] = { 0, 0, mySize.x(), mySize.y() };
  myGlContext->ResizeViewport (aViewport);
  myGlContext->SetDrawBuffer (GL_BACK);
  if (myGlContext->core11ffp != NULL)
  {
    myGlContext->core11ffp->glMatrixMode (GL_MODELVIEW);
  }
}

// =======================================================================
// function : SetSwapInterval
// purpose  :
// =======================================================================
void OpenGl_Window::SetSwapInterval (Standard_Boolean theToForceNoSync)
{
  const Standard_Integer aSwapInterval = theToForceNoSync ? 0 : myGlContext->caps->swapInterval;
  if (mySwapInterval != aSwapInterval)
  {
    mySwapInterval = aSwapInterval;
    myGlContext->SetSwapInterval (mySwapInterval);
  }
}

#endif // __APPLE__
