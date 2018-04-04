// Copyright (c) 2014 OPEN CASCADE SAS
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

#if defined(_WIN32)
  #include <windows.h>
#endif

#include "AndroidQt.h"
#include "AndroidQt_UserInteractionParameters.h"
#include "AndroidQt_Window.h"

#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Quantity_Color.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>
#include <WNT_Window.hxx>

#include <EGL/egl.h>

#include <Standard_WarningsDisable.hxx>
#include <QFileInfo>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : AndroidQt
// purpose  :
// =======================================================================
AndroidQt::AndroidQt()
: myFitAllAction (false)
{
  connect (this, SIGNAL (windowChanged (QQuickWindow*)), this, SLOT (handleWindowChanged (QQuickWindow*)));

  // set shaders location variable
  QByteArray aDataRoot = "/data/data/org.qtproject.example.AndroidQt/files/opencascade/shared";
  qputenv ("CSF_ShadersDirectory", aDataRoot + "/Shaders");
}

// =======================================================================
// function : ReadShapeFromFile
// purpose  :
// =======================================================================
bool AndroidQt::ReadShapeFromFile (QString theFilePath)
{
  QUrl    aFileUrl   (theFilePath);
  QString aFilePath = theFilePath;
  if (aFileUrl.isLocalFile())
  {
    aFilePath = QUrl (theFilePath).toLocalFile();
  }

  if (!QFile (aFilePath).exists())
  {
    return false;
  }

  TopoDS_Shape aShape;
  BRep_Builder aBuildTool;
  try
  {
    OCC_CATCH_SIGNALS

    if (!BRepTools::Read (aShape, aFilePath.toStdString().c_str(), aBuildTool))
    {
      return false;
    }

    if (!myContext.IsNull())
    {
      myContext->EraseAll (Standard_False);

      Handle(AIS_Shape) aShapePrs = new AIS_Shape (aShape);
      aShapePrs->SetColor (Quantity_Color(1.0, 0.73, 0.2, Quantity_TOC_RGB));

      myContext->Display        (aShapePrs, Standard_False);
      myContext->SetDisplayMode (aShapePrs, AIS_Shaded, Standard_False);
    }

    myMutex.lock();
    myFitAllAction = true;
    myMutex.unlock();

    if (window())
    {
      window()->update();
    }
  }
  catch (Standard_Failure)
  {
    return false;
  }

  return true;
}

// =======================================================================
// function : InitTouch
// purpose  :
// =======================================================================
void AndroidQt::InitTouch (const double theX,
                           const double theY)
{
  myMutex.lock();
  myTouchPoint.SetStarts (theX, theY);
  myMutex.unlock();
}

// =======================================================================
// function : UpdateTouch
// purpose  :
// =======================================================================
void AndroidQt::UpdateTouch (const double theX,
                             const double theY)
{
  myMutex.lock();
  myTouchPoint.SetEnds (theX, theY);
  myMutex.unlock();

  if (window())
    window()->update();
}

// =======================================================================
// function : handleWindowChanged
// purpose  :
// =======================================================================
void AndroidQt::handleWindowChanged (QQuickWindow* theWin)
{
  if (theWin == NULL)
  {
    return;
  }

  connect (theWin, SIGNAL (beforeSynchronizing()), this, SLOT (sync()), Qt::DirectConnection);

  theWin->setClearBeforeRendering (false);
}

// =======================================================================
// function : sync
// purpose  :
// =======================================================================
void AndroidQt::sync()
{
  myViewportSize = window()->size() * window()->devicePixelRatio();

  Graphic3d_Vec2i aWinTopLeft (window()->x(), window()->y());
  Graphic3d_Vec2i aWinSize (myViewportSize.width(), myViewportSize.height());
  const bool isChangedLeft = (myWinTopLeft.x() != aWinTopLeft.x());
  const bool isChangedTop = (myWinTopLeft.y() != aWinTopLeft.y());
  myWinTopLeft = aWinTopLeft;

  if (myViewer.IsNull())
  {
    initViewer (Aspect_Drawable (window()->winId()));
    connect (window(), SIGNAL (beforeRendering()), this, SLOT (paint()), Qt::DirectConnection);
  }
  else
  {
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myViewer->Driver());
  #ifdef __ANDROID__
    if (aDriver->getRawGlContext() != eglGetCurrentContext())
    {
      initViewer (Aspect_Drawable (window()->winId()));
    }
    else
  #endif
    {
    #ifdef __ANDROID__
      Handle(AndroidQt_Window) aWindow = Handle(AndroidQt_Window)::DownCast(myView->Window());
      aWindow->SetSize (myViewportSize.width(), myViewportSize.height());
      //myView->MustBeResized(); // can be used instead of SetWindow() when EGLsurface has not been changed

      EGLContext anEglContext = eglGetCurrentContext();
      myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext);
    #else
      if (aWinSize.x() != myWinSize.x()
       || aWinSize.y() != myWinSize.y())
      {
        myView->MustBeResized();
        myView->Invalidate();
      }
      else if (isChangedTop)
      {
        myView->MustBeResized();
      }
      else if (isChangedLeft)
      {
        myView->MustBeResized();
      }
    #endif
    }
  }
  myWinSize = aWinSize;
}

// =======================================================================
// function : paint
// purpose  :
// =======================================================================
void AndroidQt::paint()
{
  myMutex.lock();

  if (Abs (myTouchPoint.DevX()) + Abs (myTouchPoint.DevY()) > 1)
  {
    myView->StartRotation ((Standard_Integer)myTouchPoint.X().first,  (Standard_Integer)myTouchPoint.Y().first);
    myView->Rotation      ((Standard_Integer)myTouchPoint.X().second, (Standard_Integer)myTouchPoint.Y().second);

    myTouchPoint.ClearDev();
  }

  if (myFitAllAction)
  {
    myView->FitAll();
    myFitAllAction = false;
  }

  myMutex.unlock();

  myView->Redraw();
}

// =======================================================================
// function : initViewer
// purpose  :
// =======================================================================
bool AndroidQt::initViewer (Aspect_Drawable theWin)
{
  int aWidth = 0, aHeight = 0;
  Handle(Aspect_DisplayConnection) aDisplayConnection;
#ifdef __ANDROID__
  EGLint aCfgId = 0;
  EGLDisplay anEglDisplay = eglGetCurrentDisplay();
  EGLContext anEglContext = eglGetCurrentContext();
  EGLSurface anEglSurf    = eglGetCurrentSurface (EGL_DRAW);

  if (anEglDisplay == EGL_NO_DISPLAY
   || anEglContext == EGL_NO_CONTEXT
   || anEglSurf    == EGL_NO_SURFACE)
  {
    release();
    return false;
  }

  eglQuerySurface (anEglDisplay, anEglSurf, EGL_WIDTH,     &aWidth);
  eglQuerySurface (anEglDisplay, anEglSurf, EGL_HEIGHT,    &aHeight);
  eglQuerySurface (anEglDisplay, anEglSurf, EGL_CONFIG_ID, &aCfgId);

  const EGLint aConfigAttribs[] = { EGL_CONFIG_ID, aCfgId, EGL_NONE };
  EGLint       aNbConfigs = 0;
  void*        anEglConfig = NULL;

  if (eglChooseConfig (anEglDisplay, aConfigAttribs, &anEglConfig, 1, &aNbConfigs) != EGL_TRUE)
  {
    Message::DefaultMessenger()->Send ("Error: EGL does not provide compatible configurations", Message_Fail);
    release();
    return false;
  }

  if (!myViewer.IsNull())
  {
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myViewer->Driver());
    Handle(AndroidQt_Window)     aWindow = Handle(AndroidQt_Window)::DownCast (myView->Window());
    if (!aDriver->InitEglContext (anEglDisplay, anEglContext, anEglConfig))
    {
      Message::DefaultMessenger()->Send ("Error: OpenGl_GraphicDriver can not be initialized", Message_Fail);
      release();
      return false;
    }

    aWindow->SetSize (aWidth, aHeight);
    myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext);
  }

  Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver (NULL, Standard_False);
#elif defined(_WIN32)
  HWND  aWinHandle = (HWND)theWin;
  HDC   aWindowDC = wglGetCurrentDC();
  HGLRC aRendCtx   = wglGetCurrentContext();
  if (aWinHandle == NULL
   || aWindowDC  == NULL
   || aRendCtx   == NULL)
  {
    Message::DefaultMessenger()->Send ("Error: No active WGL context!", Message_Fail);
    release();
    return false;
  }

  RECT aRect;
  ::GetClientRect (aWinHandle, &aRect);
  aWidth  = aRect.right - aRect.left;
  aHeight = aRect.bottom - aRect.top;
  myWinSize.x() = aWidth;
  myWinSize.y() = aHeight;
  if (!myViewer.IsNull())
  {
    Handle(WNT_Window) aWindow = new WNT_Window (aWinHandle);
    myView->SetWindow (aWindow, (Aspect_RenderingContext)aRendCtx);
    return true;
  }
  Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver (aDisplayConnection, Standard_False);
#endif

  aDriver->ChangeOptions().buffersNoSwap = Standard_True;
  //aDriver->ChangeOptions().glslWarnings  = Standard_True; // for GLSL shaders debug

#ifdef __ANDROID__
  if (!aDriver->InitEglContext (anEglDisplay, anEglContext, anEglConfig))
  {
    Message::DefaultMessenger()->Send ("Error: OpenGl_GraphicDriver can not be initialized", Message_Fail);
    release();
    return false;
  }
#endif

  // create viewer
  myViewer = new V3d_Viewer (aDriver);
  myViewer->SetDefaultBackgroundColor (AndroidQt_UserInteractionParameters::BgColor.Name());
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  // create AIS context
  myContext = new AIS_InteractiveContext (myViewer);
  myContext->SetDisplayMode (AIS_Shaded, false);

#ifdef __ANDROID__
  Handle(AndroidQt_Window) aWindow = new AndroidQt_Window (aWidth, aHeight);
#elif defined(_WIN32)
  Handle(WNT_Window)       aWindow = new WNT_Window (aWinHandle);
#endif

  myView = myViewer->CreateView();
  myView->SetImmediateUpdate (Standard_False);

#ifdef __ANDROID__
  myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext);
#else
  myView->SetWindow (aWindow, (Aspect_RenderingContext )aRendCtx);
#endif
  myView->TriedronDisplay (Aspect_TOTP_RIGHT_LOWER, Quantity_NOC_WHITE, 0.08, V3d_ZBUFFER);

  return true;
}

// =======================================================================
// function : release
// purpose  :
// =======================================================================
void AndroidQt::release()
{
  myContext.Nullify();
  myView.Nullify();
  myViewer.Nullify();
}
