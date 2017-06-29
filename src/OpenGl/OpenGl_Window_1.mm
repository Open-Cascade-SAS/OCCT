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

#ifndef GL_GLEXT_LEGACY
#define GL_GLEXT_LEGACY // To prevent inclusion of system glext.h on Mac OS X 10.6.8
#endif

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
OpenGl_Window::OpenGl_Window (const Handle(OpenGl_GraphicDriver)& theDriver,
                              const Handle(Aspect_Window)&  thePlatformWindow,
                              Aspect_RenderingContext       theGContext,
                              const Handle(OpenGl_Caps)&    theCaps,
                              const Handle(OpenGl_Context)& theShareCtx)
: myGlContext (new OpenGl_Context (theCaps)),
  myOwnGContext (theGContext == 0),
  myPlatformWindow (thePlatformWindow),
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  myUIView (NULL),
#endif
  mySwapInterval (theCaps->swapInterval)
{
  (void )theDriver;
  myPlatformWindow->Size (myWidth, myHeight);

#if defined(__APPLE__)
  myWidthPt  = myWidth;
  myHeightPt = myHeight;
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

    aGLContext = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    if (aGLContext == NULL
    || ![EAGLContext setCurrentContext: aGLContext])
    {
      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: EAGLContext creation failed");
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
      return;
    }

    myGlContext->Init (aGLContext, Standard_False);
  }
  else
  {
    if (![EAGLContext setCurrentContext: aGLContext])
    {
      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: EAGLContext can not be assigned");
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
      return;
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
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
      return;
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
    [aGLContext setView: aView];
    isCore = (aTryCore == 1);
  }

  myGlContext->Init (aGLContext, isCore);
#endif

  myGlContext->Share (theShareCtx);
  myGlContext->SetSwapInterval (mySwapInterval);
  Init();
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
  Standard_Integer aWidthPt  = 0;
  Standard_Integer aHeightPt = 0;
  myPlatformWindow->Size (aWidthPt, aHeightPt);
  if (myWidthPt  == aWidthPt
   && myHeightPt == aHeightPt)
  {
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    return;
  #else
    // check backing store change (moving to another screen)
    NSOpenGLContext* aGLCtx = myGlContext->myGContext;
    NSView* aView = [aGLCtx view];
    if (![aView respondsToSelector: @selector(convertSizeToBacking:)])
    {
      return;
    }

    NSRect aBounds = [aView bounds];
    NSSize aRes    = [aView convertSizeToBacking: aBounds.size];
    if (myWidth  == Standard_Integer(aRes.width)
     && myHeight == Standard_Integer(aRes.height))
    {
      return;
    }
  #endif
  }

  myWidthPt  = aWidthPt;
  myHeightPt = aHeightPt;

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
    ::glGenRenderbuffers (1, &aWinRBColor);
    ::glBindRenderbuffer (GL_RENDERBUFFER, aWinRBColor);
    [aGLCtx renderbufferStorage: GL_RENDERBUFFER fromDrawable: anEaglLayer];
    ::glGetRenderbufferParameteriv (GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,  &myWidth);
    ::glGetRenderbufferParameteriv (GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &myHeight);
    ::glBindRenderbuffer (GL_RENDERBUFFER, 0);

    if (!aDefFbo->InitWithRB (myGlContext, myWidth, myHeight, GL_RGBA8, GL_DEPTH24_STENCIL8, aWinRBColor))
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

    myWidth  = aDefFbo->GetVPSizeX();
    myHeight = aDefFbo->GetVPSizeY();
  }
  myGlContext->SetDefaultFrameBuffer (aDefFbo);
  aDefFbo->BindBuffer (myGlContext);
  aDefFbo.Nullify();
#else
  NSOpenGLContext* aGLCtx  = myGlContext->myGContext;
  NSView*          aView   = [aGLCtx view];
  NSRect           aBounds = [aView bounds];

  // we should call this method each time when window is resized
  [aGLCtx update];

  if ([aView respondsToSelector: @selector(convertSizeToBacking:)])
  {
    NSSize aRes = [aView convertSizeToBacking: aBounds.size];
    myWidth  = Standard_Integer(aRes.width);
    myHeight = Standard_Integer(aRes.height);
  }
  else
  {
    myWidth  = Standard_Integer(aBounds.size.width);
    myHeight = Standard_Integer(aBounds.size.height);
  }
  myWidthPt  = Standard_Integer(aBounds.size.width);
  myHeightPt = Standard_Integer(aBounds.size.height);
#endif

  ::glDisable (GL_DITHER);
  ::glDisable (GL_SCISSOR_TEST);
  ::glViewport (0, 0, myWidth, myHeight);
#if !defined(GL_ES_VERSION_2_0)
  ::glDrawBuffer (GL_BACK);
  if (myGlContext->core11 != NULL)
  {
    ::glMatrixMode (GL_MODELVIEW);
  }
#endif
}

// =======================================================================
// function : SetSwapInterval
// purpose  :
// =======================================================================
void OpenGl_Window::SetSwapInterval()
{
  if (mySwapInterval != myGlContext->caps->swapInterval)
  {
    mySwapInterval = myGlContext->caps->swapInterval;
    myGlContext->SetSwapInterval (mySwapInterval);
  }
}

#endif // __APPLE__
