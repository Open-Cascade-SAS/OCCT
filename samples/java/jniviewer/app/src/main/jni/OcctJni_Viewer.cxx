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

#include "OcctJni_Viewer.hxx"
#include "OcctJni_MsgPrinter.hxx"

#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <Image_AlienPixMap.hxx>
#include <BRepTools.hxx>
#include <Message_Messenger.hxx>
#include <Message_MsgFile.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Timer.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Standard_Version.hxx>

#include <BRepPrimAPI_MakeBox.hxx>

#include <STEPControl_Reader.hxx>
#include <IGESControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>

#include <EGL/egl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <jni.h>

//! @return true if file exists
static bool isFileExist (const TCollection_AsciiString& thePath)
{
  struct stat64 aStatBuffer;
  return stat64 (thePath.ToCString(), &aStatBuffer) == 0;
}

//! Cut-off the last split character from the path and everything after it.
static TCollection_AsciiString getParentDir (const TCollection_AsciiString& thePath)
{
  TCollection_AsciiString aPath = thePath;
  char* aSplitter = (char* )aPath.ToCString();
  for (char* anIter = aSplitter; *anIter != '\0'; ++anIter)
  {
    if (*anIter == '\\'
     || *anIter == '/')
    {
      aSplitter = anIter;
    }
  }
  *aSplitter = '\0'; // cut off file name or trailing folder
  return TCollection_AsciiString (aPath.ToCString());
}

//! Set environment variable theVarName indicating location of resource
//! file theFile so as to correspond to actual location of this file.
//!
//! The resource file is searched in directory where Test.Draw.dll is located,
//! and if not found - also in subdirectory ../res from there.
//! If file is found, environment variable is set for C subsystem.
//! Otherwise, environment is not changed.
//!
//! If theToAddFileName is true, complete file name is set as value of the variable,
//! if theToAddFileName is false, only path is set.
Standard_Boolean setResourceEnv (const TCollection_AsciiString& theVarName,
                                 const TCollection_AsciiString& theRoot,
                                 const TCollection_AsciiString& theFile,
                                 const Standard_Boolean         theToAddFileName)
{
  // use location of current assembly to figure out possible location of resource
  TCollection_AsciiString aBaseDir = theRoot;

  // check the same directory where binary is located
  if (!isFileExist (aBaseDir + "/" + theFile))
  {
    // check subdirectory ../res
    aBaseDir = getParentDir (aBaseDir) + "/res";
    if (!isFileExist (aBaseDir + "/" + theFile))
    {
      return Standard_False;
    }
  }

  // set C library environment
  if (theToAddFileName)
  {
    aBaseDir = aBaseDir + "/" + theFile;
  }

  OSD_Environment anEnv (theVarName, aBaseDir);
  anEnv.Build();
  return Standard_True;
}

// =======================================================================
// function : OcctJni_Viewer
// purpose  :
// =======================================================================
OcctJni_Viewer::OcctJni_Viewer (float theDispDensity)
: myDevicePixelRatio (theDispDensity),
  myIsJniMoreFrames (false)
{
  SetTouchToleranceScale (theDispDensity);
#ifndef NDEBUG
  // Register printer for logging messages into global Android log.
  // Should never be used in production (or specify higher gravity for logging only failures).
  Handle(Message_Messenger) aMsgMgr = Message::DefaultMessenger();
  aMsgMgr->RemovePrinters (STANDARD_TYPE (Message_PrinterSystemLog));
  aMsgMgr->AddPrinter (new Message_PrinterSystemLog ("OcctJni_Viewer"));
#endif

  // prepare necessary environment
  TCollection_AsciiString aResRoot = "/data/data/com.opencascade.jnisample/files";

  setResourceEnv ("CSF_XSMessage", aResRoot + "/XSMessage", "XSTEP.us", Standard_False);
  setResourceEnv ("CSF_SHMessage", aResRoot + "/XSMessage", "SHAPE.us", Standard_False);
}

// ================================================================
// Function : dumpGlInfo
// Purpose  :
// ================================================================
void OcctJni_Viewer::dumpGlInfo (bool theIsBasic)
{
  TColStd_IndexedDataMapOfStringString aGlCapsDict;
  myView->DiagnosticInformation (aGlCapsDict, Graphic3d_DiagnosticInfo_Basic); //theIsBasic ? Graphic3d_DiagnosticInfo_Basic : Graphic3d_DiagnosticInfo_Complete);
  if (theIsBasic)
  {
    TCollection_AsciiString aViewport;
    aGlCapsDict.FindFromKey ("Viewport", aViewport);
    aGlCapsDict.Clear();
    aGlCapsDict.Add ("Viewport", aViewport);
  }
  aGlCapsDict.Add ("Display scale", TCollection_AsciiString(myDevicePixelRatio));

  // beautify output
  {
    TCollection_AsciiString* aGlVer   = aGlCapsDict.ChangeSeek ("GLversion");
    TCollection_AsciiString* aGlslVer = aGlCapsDict.ChangeSeek ("GLSLversion");
    if (aGlVer   != NULL
     && aGlslVer != NULL)
    {
      *aGlVer = *aGlVer + " [GLSL: " + *aGlslVer + "]";
      aGlslVer->Clear();
    }
  }

  TCollection_AsciiString anInfo;
  for (TColStd_IndexedDataMapOfStringString::Iterator aValueIter (aGlCapsDict); aValueIter.More(); aValueIter.Next())
  {
    if (!aValueIter.Value().IsEmpty())
    {
      if (!anInfo.IsEmpty())
      {
        anInfo += "\n";
      }
      anInfo += aValueIter.Key() + ": " + aValueIter.Value();
    }
  }

  Message::Send (anInfo, Message_Warning);
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
bool OcctJni_Viewer::init()
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
    Message::DefaultMessenger()->Send ("Error: No active EGL context!", Message_Fail);
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
    Message::DefaultMessenger()->Send ("Error: EGL does not provide compatible configurations!", Message_Fail);
    release();
    return false;
  }

  if (!myViewer.IsNull())
  {
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myViewer->Driver());
    Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast (myView->Window());
    if (!aDriver->InitEglContext (anEglDisplay, anEglContext, anEglConfig))
    {
      Message::DefaultMessenger()->Send ("Error: OpenGl_GraphicDriver can not be initialized!", Message_Fail);
      release();
      return false;
    }

    aWindow->SetSize (aWidth, aHeight);
    myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext);
    dumpGlInfo (true);
    return true;
  }

  Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver (NULL, Standard_False);
  aDriver->ChangeOptions().buffersNoSwap = Standard_True;
//aDriver->ChangeOptions().glslWarnings  = Standard_True; /// for debug only!
  if (!aDriver->InitEglContext (anEglDisplay, anEglContext, anEglConfig))
  {
    Message::DefaultMessenger()->Send ("Error: OpenGl_GraphicDriver can not be initialized!", Message_Fail);
    release();
    return false;
  }

  myTextStyle = new Prs3d_TextAspect();
  myTextStyle->SetFont (Font_NOF_ASCII_MONO);
  myTextStyle->SetHeight (12);
  myTextStyle->Aspect()->SetColor (Quantity_NOC_GRAY95);
  myTextStyle->Aspect()->SetColorSubTitle (Quantity_NOC_BLACK);
  myTextStyle->Aspect()->SetDisplayType (Aspect_TODT_SHADOW);
  myTextStyle->Aspect()->SetTextFontAspect (Font_FA_Bold);
  myTextStyle->Aspect()->SetTextZoomable (false);
  myTextStyle->SetHorizontalJustification (Graphic3d_HTA_LEFT);
  myTextStyle->SetVerticalJustification (Graphic3d_VTA_BOTTOM);

  // create viewer
  myViewer = new V3d_Viewer (aDriver);
  myViewer->SetDefaultBackgroundColor (Quantity_NOC_BLACK);
  myViewer->SetDefaultLights();
  myViewer->SetLightOn();

  // create AIS context
  myContext = new AIS_InteractiveContext (myViewer);
  myContext->SetPixelTolerance (int(myDevicePixelRatio * 6.0)); // increase tolerance and adjust to hi-dpi screens
  myContext->SetDisplayMode (AIS_Shaded, false);

  Handle(Aspect_NeutralWindow) aWindow = new Aspect_NeutralWindow();
  aWindow->SetSize (aWidth, aHeight);
  myView = myViewer->CreateView();
  myView->SetImmediateUpdate (false);
  myView->ChangeRenderingParams().Resolution = (unsigned int )(96.0 * myDevicePixelRatio + 0.5);
  myView->ChangeRenderingParams().ToShowStats = true;
  myView->ChangeRenderingParams().CollectedStats = (Graphic3d_RenderingParams::PerfCounters ) (Graphic3d_RenderingParams::PerfCounters_FrameRate | Graphic3d_RenderingParams::PerfCounters_Triangles);
  myView->ChangeRenderingParams().StatsTextAspect = myTextStyle->Aspect();
  myView->ChangeRenderingParams().StatsTextHeight = (int )myTextStyle->Height();

  myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext);
  dumpGlInfo (false);
  //myView->TriedronDisplay (Aspect_TOTP_RIGHT_LOWER, Quantity_NOC_WHITE, 0.08 * myDevicePixelRatio, V3d_ZBUFFER);

  initContent();
  return true;
}

// =======================================================================
// function : release
// purpose  :
// =======================================================================
void OcctJni_Viewer::release()
{
  myContext.Nullify();
  myView.Nullify();
  myViewer.Nullify();
}

// =======================================================================
// function : resize
// purpose  :
// =======================================================================
void OcctJni_Viewer::resize (int theWidth,
                             int theHeight)
{
  if (myContext.IsNull())
  {
    Message::DefaultMessenger()->Send ("Resize failed - view is unavailable", Message_Fail);
    return;
  }

  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myViewer->Driver());
  Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast (myView->Window());
  aWindow->SetSize (theWidth, theHeight);
  //myView->MustBeResized(); // can be used instead of SetWindow() when EGLsurface has not been changed

  EGLContext anEglContext = eglGetCurrentContext();
  myView->SetWindow (aWindow, (Aspect_RenderingContext )anEglContext);
  dumpGlInfo (true);
  //saveSnapshot ("/sdcard/Download/tt.png", theWidth, theHeight);
}

// =======================================================================
// function : initContent
// purpose  :
// =======================================================================
void OcctJni_Viewer::initContent()
{
  myContext->RemoveAll (Standard_False);

  if (myViewCube.IsNull())
  {
    myViewCube = new AIS_ViewCube();
    {
      // setup view cube size
      static const double THE_CUBE_SIZE = 60.0;
      myViewCube->SetSize (myDevicePixelRatio * THE_CUBE_SIZE, false);
      myViewCube->SetBoxFacetExtension (myViewCube->Size() * 0.15);
      myViewCube->SetAxesPadding (myViewCube->Size() * 0.10);
      myViewCube->SetFontHeight  (THE_CUBE_SIZE * 0.16);
    }
    // presentation parameters
    myViewCube->SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_TriedronPers, Aspect_TOTP_RIGHT_LOWER, Graphic3d_Vec2i (200, 200)));
    myViewCube->Attributes()->SetDatumAspect (new Prs3d_DatumAspect());
    myViewCube->Attributes()->DatumAspect()->SetTextAspect (myTextStyle);
    // animation parameters
    myViewCube->SetViewAnimation (myViewAnimation);
    myViewCube->SetFixedAnimationLoop (false);
    myViewCube->SetAutoStartAnimation (true);
  }
  myContext->Display (myViewCube, false);

  OSD_Timer aTimer;
  aTimer.Start();
  if (!myShape.IsNull())
  {
    Handle(AIS_Shape) aShapePrs = new AIS_Shape (myShape);
    myContext->Display (aShapePrs, Standard_False);
  }
  else
  {
    BRepPrimAPI_MakeBox aBuilder (1.0, 2.0, 3.0);
    Handle(AIS_Shape) aShapePrs = new AIS_Shape (aBuilder.Shape());
    myContext->Display (aShapePrs, Standard_False);
  }
  myView->FitAll();

  aTimer.Stop();
  Message::DefaultMessenger()->Send (TCollection_AsciiString() + "Presentation computed in " + aTimer.ElapsedTime() + " seconds", Message_Info);
}

//! Load shape from IGES file
static TopoDS_Shape loadIGES (const TCollection_AsciiString& thePath)
{
  TopoDS_Shape          aShape;
  IGESControl_Reader    aReader;
  IFSelect_ReturnStatus aReadStatus = IFSelect_RetFail;
  try
  {
    aReadStatus = aReader.ReadFile (thePath.ToCString());
  }
  catch (Standard_Failure)
  {
    Message::DefaultMessenger()->Send ("Error: IGES reader, computation error", Message_Fail);
    return aShape;
  }

  if (aReadStatus != IFSelect_RetDone)
  {
    Message::DefaultMessenger()->Send ("Error: IGES reader, bad file format", Message_Fail);
    return aShape;
  }

  // now perform the translation
  aReader.TransferRoots();
  if (aReader.NbShapes() <= 0)
  {
    Handle(XSControl_WorkSession) aWorkSession = new XSControl_WorkSession();
    aWorkSession->SelectNorm ("IGES");
    aReader.SetWS (aWorkSession, Standard_True);
    aReader.SetReadVisible (Standard_False);
    aReader.TransferRoots();
  }
  if (aReader.NbShapes() <= 0)
  {
    Message::DefaultMessenger()->Send ("Error: IGES reader, no shapes has been found", Message_Fail);
    return aShape;
  }
  return aReader.OneShape();
  /*TopoDS_Shape anImportedShape = aReader.OneShape();

  // apply sewing on the imported shape
  BRepBuilderAPI_Sewing aTool (0.0);
  aTool.SetNonManifoldMode  (Standard_False);
  aTool.SetFloatingEdgesMode(Standard_True);
  aTool.Load (anImportedShape);
  aTool.Perform();
  TopoDS_Shape aSewedShape = aTool.SewedShape();

  if (aSewedShape.IsNull())
  {
    Message::DefaultMessenger()->Send ("Error: Sewing result is empty", Message_Fail);
    return aShape;
  }
  if (aSewedShape.IsSame(anImportedShape))
  {
    aShape = anImportedShape;
  }
  else
  {
    // apply shape healing
    ShapeFix_Shape aShapeFixer(aSewedShape);
    aShapeFixer.FixSolidMode() = 1;
    aShapeFixer.FixFreeShellMode() = 1;
    aShapeFixer.FixFreeFaceMode() = 1;
    aShapeFixer.FixFreeWireMode() = 0;
    aShapeFixer.FixSameParameterMode() = 0;
    aShapeFixer.FixVertexPositionMode() = 0;
    aShape = aShapeFixer.Perform() ? aShapeFixer.Shape() : aSewedShape;
  }
  return aShape;*/
}

//! Load shape from STEP file
static TopoDS_Shape loadSTEP (const TCollection_AsciiString& thePath)
{
  STEPControl_Reader    aReader;
  IFSelect_ReturnStatus aReadStatus = IFSelect_RetFail;
  try
  {
    aReadStatus = aReader.ReadFile (thePath.ToCString());
  }
  catch (Standard_Failure)
  {
    Message::DefaultMessenger()->Send ("Error: STEP reader, computation error", Message_Fail);
    return TopoDS_Shape();
  }

  if (aReadStatus != IFSelect_RetDone)
  {
    Message::DefaultMessenger()->Send ("Error: STEP reader, bad file format", Message_Fail);
    return TopoDS_Shape();
  }
  else if (aReader.NbRootsForTransfer() <= 0)
  {
    Message::DefaultMessenger()->Send ("Error: STEP reader, shape is empty", Message_Fail);
    return TopoDS_Shape();
  }

  // now perform the translation
  aReader.TransferRoots();
  return aReader.OneShape();
}

// =======================================================================
// function : open
// purpose  :
// =======================================================================
bool OcctJni_Viewer::open (const TCollection_AsciiString& thePath)
{
  myShape.Nullify();
  if (!myContext.IsNull())
  {
    myContext->RemoveAll (Standard_False);
    if (!myViewCube.IsNull())
    {
      myContext->Display (myViewCube, false);
    }
  }
  if (thePath.IsEmpty())
  {
    return false;
  }

  OSD_Timer aTimer;
  aTimer.Start();
  TCollection_AsciiString aFormatStr;
  const Standard_Integer  aLen = thePath.Length();
  if (aLen >= 5
  && thePath.Value (aLen - 4) == '.')
  {
    aFormatStr = thePath.SubString (aLen - 3, aLen);
  }
  else if (aLen >= 4
   && thePath.Value (aLen - 3) == '.')
  {
    aFormatStr = thePath.SubString (aLen - 2, aLen);
  }
  else if (aLen >= 3
        && thePath.Value (aLen - 2) == '.')
  {
    aFormatStr = thePath.SubString (aLen - 1, aLen);
  }
  aFormatStr.LowerCase();

  TopoDS_Shape aShape;
  if (aFormatStr == "stp"
   || aFormatStr == "step")
  {
    aShape = loadSTEP (thePath);
  }
  else if (aFormatStr == "igs"
        || aFormatStr == "iges")
  {
    aShape = loadIGES (thePath);
  }
  else
      // if (aFormatStr == "brep"
      //  || aFormatStr == "rle")
  {
    BRep_Builder aBuilder;
    if (!BRepTools::Read (aShape, thePath.ToCString(), aBuilder))
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString() + "Error: file '" + thePath + "' can not be opened!", Message_Info);
      return false;
    }
  }
  if (aShape.IsNull())
  {
    return false;
  }
  aTimer.Stop();
  Message::DefaultMessenger()->Send (TCollection_AsciiString() + "File '" + thePath + "' loaded in " + aTimer.ElapsedTime() + " seconds", Message_Info);

  myShape = aShape;
  if (myContext.IsNull())
  {
    return true;
  }

  aTimer.Reset();
  aTimer.Start();

  Handle(AIS_Shape) aShapePrs = new AIS_Shape (aShape);
  myContext->Display (aShapePrs, Standard_False);
  myView->FitAll();

  aTimer.Stop();
  Message::DefaultMessenger()->Send (TCollection_AsciiString() + "Presentation computed in " + aTimer.ElapsedTime() + " seconds", Message_Info);
  return true;
}

// =======================================================================
// function : saveSnapshot
// purpose  :
// =======================================================================
bool OcctJni_Viewer::saveSnapshot (const TCollection_AsciiString& thePath,
                                   int theWidth,
                                   int theHeight)
{
  if (myContext.IsNull()
   || thePath.IsEmpty())
  {
    Message::DefaultMessenger()->Send ("Image dump failed - view is unavailable", Message_Fail);
    return false;
  }

  if (theWidth  < 1
   || theHeight < 1)
  {
    myView->Window()->Size (theWidth, theHeight);
  }
  if (theWidth  < 1
   || theHeight < 1)
  {
    Message::DefaultMessenger()->Send ("Image dump failed - view is unavailable", Message_Fail);
    return false;
  }

  Image_AlienPixMap anAlienImage;
  if (!anAlienImage.InitTrash (Image_Format_BGRA, theWidth, theHeight))
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString() + "RGBA image " + theWidth + "x" + theHeight + " allocation failed", Message_Fail);
    return false;
  }

  // OpenGL ES does not support fetching data in BGRA format
  // while FreeImage does not support RGBA format.
  Image_PixMap anImage;
  anImage.InitWrapper (Image_Format_RGBA,
                       anAlienImage.ChangeData(),
                       anAlienImage.SizeX(),
                       anAlienImage.SizeY(),
                       anAlienImage.SizeRowBytes());
  if (!myView->ToPixMap (anImage, theWidth, theHeight, Graphic3d_BT_RGBA))
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString() + "View dump to the image " + theWidth + "x" + theHeight + " failed", Message_Fail);
  }

  for (Standard_Size aRow = 0; aRow < anAlienImage.SizeY(); ++aRow)
  {
    for (Standard_Size aCol = 0; aCol < anAlienImage.SizeX(); ++aCol)
    {
      Image_ColorRGBA& aPixel = anAlienImage.ChangeValue<Image_ColorRGBA> (aRow, aCol);
      std::swap (aPixel.r(), aPixel.b());
      //aPixel.a() = 1.0;
    }
  }

  if (!anAlienImage.Save (thePath))
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString() + "Image saving to path '" + thePath + "' failed", Message_Fail);
    return false;
  }
  Message::DefaultMessenger()->Send (TCollection_AsciiString() + "View " + theWidth + "x" + theHeight + " dumped to image '" + thePath + "'", Message_Info);
  return true;
}

// ================================================================
// Function : handleViewRedraw
// Purpose  :
// ================================================================
void OcctJni_Viewer::handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                       const Handle(V3d_View)& theView)
{
  AIS_ViewController::handleViewRedraw (theCtx, theView);
  myIsJniMoreFrames = myToAskNextFrame;
}

// =======================================================================
// function : redraw
// purpose  :
// =======================================================================
bool OcctJni_Viewer::redraw()
{
  if (myView.IsNull())
  {
    return false;
  }

  // handle user input
  myIsJniMoreFrames = false;
  myView->InvalidateImmediate();
  FlushViewEvents (myContext, myView, true);
  return myIsJniMoreFrames;
}

// =======================================================================
// function : fitAll
// purpose  :
// =======================================================================
void OcctJni_Viewer::fitAll()
{
  if (myView.IsNull())
  {
    return;
  }

  myView->FitAll (0.01, Standard_False);
  myView->Invalidate();
}

#define jexp extern "C" JNIEXPORT

jexp jlong JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppCreate (JNIEnv* theEnv,
                                                                             jobject theObj,
                                                                             jfloat  theDispDensity)
{
  return jlong(new OcctJni_Viewer (theDispDensity));
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppDestroy (JNIEnv* theEnv,
                                                                             jobject theObj,
                                                                             jlong   theCppPtr)
{
  delete (OcctJni_Viewer* )theCppPtr;

  Handle(Message_Messenger) aMsgMgr = Message::DefaultMessenger();
  aMsgMgr->RemovePrinters (STANDARD_TYPE (OcctJni_MsgPrinter));
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppRelease (JNIEnv* theEnv,
                                                                             jobject theObj,
                                                                             jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->release();
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppInit (JNIEnv* theEnv,
                                                                          jobject theObj,
                                                                          jlong   theCppPtr)
{
  Handle(Message_Messenger) aMsgMgr = Message::DefaultMessenger();
  aMsgMgr->RemovePrinters (STANDARD_TYPE (OcctJni_MsgPrinter));
  aMsgMgr->AddPrinter (new OcctJni_MsgPrinter (theEnv, theObj));
  ((OcctJni_Viewer* )theCppPtr)->init();
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppResize (JNIEnv* theEnv,
                                                                            jobject theObj,
                                                                            jlong   theCppPtr,
                                                                            jint    theWidth,
                                                                            jint    theHeight)
{
  ((OcctJni_Viewer* )theCppPtr)->resize (theWidth, theHeight);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppOpen (JNIEnv* theEnv,
                                                                          jobject theObj,
                                                                          jlong   theCppPtr,
                                                                          jstring thePath)
{
  const char* aPathPtr = theEnv->GetStringUTFChars (thePath, 0);
  const TCollection_AsciiString aPath (aPathPtr);
  theEnv->ReleaseStringUTFChars (thePath, aPathPtr);
  ((OcctJni_Viewer* )theCppPtr)->open (aPath);
}

jexp jboolean JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppRedraw (JNIEnv* theEnv,
                                                                                jobject theObj,
                                                                                jlong   theCppPtr)
{
  return ((OcctJni_Viewer* )theCppPtr)->redraw() ? JNI_TRUE : JNI_FALSE;
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetAxoProj (JNIEnv* theEnv,
                                                                                jobject theObj,
                                                                                jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_XposYnegZpos);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetXposProj (JNIEnv* theEnv,
                                                                                 jobject theObj,
                                                                                 jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_Xpos);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetYposProj (JNIEnv* theEnv,
                                                                                 jobject theObj,
                                                                                 jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_Ypos);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetZposProj (JNIEnv* theEnv,
                                                                                 jobject theObj,
                                                                                 jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_Zpos);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetXnegProj (JNIEnv* theEnv,
                                                                                 jobject theObj,
                                                                                 jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_Xneg);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetYnegProj (JNIEnv* theEnv,
                                                                                 jobject theObj,
                                                                                 jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_Yneg);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSetZnegProj (JNIEnv* theEnv,
                                                                                 jobject theObj,
                                                                                 jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->setProj (V3d_Zneg);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppFitAll (JNIEnv* theEnv,
                                                                            jobject theObj,
                                                                            jlong   theCppPtr)
{
  ((OcctJni_Viewer* )theCppPtr)->fitAll();
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppAddTouchPoint (JNIEnv* theEnv,
                                                                                   jobject theObj,
                                                                                   jlong   theCppPtr,
                                                                                   jint    theId,
                                                                                   jfloat  theX,
                                                                                   jfloat  theY)
{
  ((OcctJni_Viewer* )theCppPtr)->AddTouchPoint (theId, Graphic3d_Vec2d (theX, theY));
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppUpdateTouchPoint (JNIEnv* theEnv,
                                                                                   jobject theObj,
                                                                                   jlong   theCppPtr,
                                                                                   jint    theId,
                                                                                   jfloat  theX,
                                                                                   jfloat  theY)
{
  ((OcctJni_Viewer* )theCppPtr)->UpdateTouchPoint (theId, Graphic3d_Vec2d (theX, theY));
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppRemoveTouchPoint (JNIEnv* theEnv,
                                                                                   jobject theObj,
                                                                                   jlong   theCppPtr,
                                                                                   jint    theId)
{
  ((OcctJni_Viewer* )theCppPtr)->RemoveTouchPoint (theId);
}

jexp void JNICALL Java_com_opencascade_jnisample_OcctJniRenderer_cppSelectInViewer (JNIEnv* theEnv,
                                                                                    jobject theObj,
                                                                                    jlong   theCppPtr,
                                                                                    jfloat  theX,
                                                                                    jfloat  theY)
{
  ((OcctJni_Viewer* )theCppPtr)->SelectInViewer (Graphic3d_Vec2i ((int )theX, (int )theY));
}

jexp jlong JNICALL Java_com_opencascade_jnisample_OcctJniActivity_cppOcctMajorVersion (JNIEnv* theEnv,
                                                                                       jobject theObj)
{
  return OCC_VERSION_MAJOR;
}

jexp jlong JNICALL Java_com_opencascade_jnisample_OcctJniActivity_cppOcctMinorVersion (JNIEnv* theEnv,
                                                                                       jobject theObj)
{
  return OCC_VERSION_MINOR;
}

jexp jlong JNICALL Java_com_opencascade_jnisample_OcctJniActivity_cppOcctMicroVersion (JNIEnv* theEnv,
                                                                                       jobject theObj)
{
  return OCC_VERSION_MAINTENANCE;
}
