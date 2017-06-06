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

#include <AndroidQt_Window.h>
#include <AndroidQt.h>
#include <AndroidQt_UserInteractionParameters.h>

#include <AIS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Quantity_Color.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>

#include <EGL/egl.h>
#include <QFileInfo>

// =======================================================================
// function : AndroidQt
// purpose  :
// =======================================================================
AndroidQt::AndroidQt()
: myFitAllAction (false)
{
  connect (this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));

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

  connect(theWin, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

  theWin->setClearBeforeRendering (false);
}

// =======================================================================
// function : sync
// purpose  :
// =======================================================================
void AndroidQt::sync()
{
  myViewportSize = window()->size() * window()->devicePixelRatio();

  if (myViewer.IsNull())
  {
    initViewer();
    connect (window(), SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
  }
  else
  {
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myViewer->Driver());
    if (aDriver->getRawGlContext() != eglGetCurrentContext())
    {
      initViewer();
    }
    else
    {
      Handle(AndroidQt_Window) aWindow = Handle(AndroidQt_Window)::DownCast (myView->Window());
      aWindow->SetSize (myViewportSize.width(), myViewportSize.height());
      //myView->MustBeResized(); // can be used instead of SetWindow() when EGLsurface has not been changed

      EGLContext anEglContext = eglGetCurrentContext();
      myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext, NULL, NULL);
    }
  }
}

// =======================================================================
// function : paint
// purpose  :
// =======================================================================
void AndroidQt::paint()
{
  myMutex.lock();

  if (Abs(myTouchPoint.DevX()) + Abs(myTouchPoint.DevY()) > 1)
  {
    myView->StartRotation (myTouchPoint.X().first,  myTouchPoint.Y().first);
    myView->Rotation      (myTouchPoint.X().second, myTouchPoint.Y().second);

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
bool AndroidQt::initViewer()
{
  EGLint aCfgId = 0;
  int aWidth = 0, aHeight = 0;
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
    release();
    return false;
  }

  if (!myViewer.IsNull())
  {
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myViewer->Driver());
    Handle(AndroidQt_Window)     aWindow = Handle(AndroidQt_Window)::DownCast (myView->Window());
    if (!aDriver->InitEglContext (anEglDisplay, anEglContext, anEglConfig))
    {
      release();
      return false;
    }

    aWindow->SetSize (aWidth, aHeight);
    myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext, NULL, NULL);
  }

  Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver (NULL, Standard_False);
  aDriver->ChangeOptions().buffersNoSwap = Standard_True;
  //aDriver->ChangeOptions().glslWarnings  = Standard_True; // for GLSL shaders debug

  if (!aDriver->InitEglContext (anEglDisplay, anEglContext, anEglConfig))
  {
    release();
    return false;
  }

  // create viewer
  myViewer = new V3d_Viewer (aDriver);
  myViewer->SetDefaultBackgroundColor (AndroidQt_UserInteractionParameters::BgColor.Name());
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  // create AIS context
  myContext = new AIS_InteractiveContext (myViewer);
  myContext->SetDisplayMode (AIS_Shaded, false);

  Handle(AndroidQt_Window) aWindow = new AndroidQt_Window (aWidth, aHeight);
  myView = myViewer->CreateView();

  myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext, NULL, NULL);
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
