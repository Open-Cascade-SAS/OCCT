// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_View.hxx>

#include <Aspect_RenderingContext.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Mat4d.hxx>
#include <NCollection_Mat4.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_Window.hxx>
#include <OpenGl_Workspace.hxx>
#include <Standard_CLocaleSentry.hxx>

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

/*----------------------------------------------------------------------*/

namespace
{
  static const OPENGL_ZCLIP myDefaultZClip = { { Standard_False, 0.F }, { Standard_False, 1.F } };
  static const OPENGL_FOG   myDefaultFog   = { Standard_False, 0.F, 1.F, { { 0.F, 0.F, 0.F, 1.F } } };
  static const TEL_COLOUR   myDefaultBg    = { { 0.F, 0.F, 0.F, 1.F } };
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
OpenGl_View::OpenGl_View (const Handle(Graphic3d_StructureManager)& theMgr,
                          const Handle(OpenGl_GraphicDriver)& theDriver,
                          const Handle(OpenGl_Caps)& theCaps,
                          Standard_Boolean& theDeviceLostFlag,
                          OpenGl_StateCounter* theCounter)
: Graphic3d_CView  (theMgr),
  myDriver         (theDriver.operator->()),
  myCaps           (theCaps),
  myDeviceLostFlag (theDeviceLostFlag),
  myWasRedrawnGL   (Standard_False),
  myAntiAliasing   (Standard_False),
  myCulling        (Standard_True),
  myShadingModel   (Graphic3d_TOSM_FACET),
  mySurfaceDetail  (Graphic3d_TOD_ALL),
  myBackfacing     (Graphic3d_TOBM_AUTOMATIC),
  myBgColor        (myDefaultBg),
  myFog            (myDefaultFog),
  myZClip          (myDefaultZClip),
  myCamera         (new Graphic3d_Camera()),
  myFBO            (NULL),
  myUseGLLight     (Standard_True),
  myToShowTrihedron      (false),
  myToShowGradTrihedron  (false),
  myStateCounter         (theCounter),
  myLastLightSourceState (0, 0),
  myFrameCounter         (0),
  myHasFboBlit           (Standard_True),
  myTransientDrawToFront (Standard_True),
  myBackBufferRestored   (Standard_False),
  myIsImmediateDrawn     (Standard_False),
  myTextureParams   (new OpenGl_AspectFace()),
  myBgGradientArray (new OpenGl_BackgroundArray (Graphic3d_TOB_GRADIENT)),
  myBgTextureArray  (new OpenGl_BackgroundArray (Graphic3d_TOB_TEXTURE)),
  // ray-tracing fields initialization
  myRaytraceInitStatus     (OpenGl_RT_NONE),
  myIsRaytraceDataValid    (Standard_False),
  myIsRaytraceWarnTextures (Standard_False),
  myToUpdateEnvironmentMap (Standard_False),
  myLayerListState (0)
{
  myWorkspace = new OpenGl_Workspace (this, NULL);

  // AA mode
  const char* anAaEnv = ::getenv ("CALL_OPENGL_ANTIALIASING_MODE");
  if (anAaEnv != NULL)
  {
    int v;
    if (sscanf (anAaEnv, "%d", &v) > 0) myAntiAliasing = v;
  }

  myCurrLightSourceState  = myStateCounter->Increment();
  myMainSceneFbos[0]      = new OpenGl_FrameBuffer();
  myMainSceneFbos[1]      = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[0] = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[1] = new OpenGl_FrameBuffer();
}

// =======================================================================
// function : Destructor
// purpose  :
// =======================================================================
OpenGl_View::~OpenGl_View()
{
  ReleaseGlResources (NULL); // ensure ReleaseGlResources() was called within valid context
  OpenGl_Element::Destroy (NULL, myBgGradientArray);
  OpenGl_Element::Destroy (NULL, myBgTextureArray);
  OpenGl_Element::Destroy (NULL, myTextureParams);
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
void OpenGl_View::ReleaseGlResources (const Handle(OpenGl_Context)& theCtx)
{
  myTrihedron         .Release (theCtx.operator->());
  myGraduatedTrihedron.Release (theCtx.operator->());

  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }

  if (myTextureParams != NULL)
  {
    myTextureParams->Release (theCtx.operator->());
  }
  if (myBgGradientArray != NULL)
  {
    myBgGradientArray->Release (theCtx.operator->());
  }
  if (myBgTextureArray != NULL)
  {
    myBgTextureArray->Release (theCtx.operator->());
  }

  myMainSceneFbos[0]     ->Release (theCtx.operator->());
  myMainSceneFbos[1]     ->Release (theCtx.operator->());
  myImmediateSceneFbos[0]->Release (theCtx.operator->());
  myImmediateSceneFbos[1]->Release (theCtx.operator->());
  myFullScreenQuad        .Release (theCtx.operator->());
  myFullScreenQuadFlip    .Release (theCtx.operator->());

  releaseRaytraceResources (theCtx);
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void OpenGl_View::Remove()
{
  if (IsRemoved())
  {
    return;
  }

  myDriver->RemoveView (this);
  myWindow.Nullify();

  Graphic3d_CView::Remove();
}

// =======================================================================
// function : SetTextureEnv
// purpose  :
// =======================================================================
void OpenGl_View::SetTextureEnv (const Handle(Graphic3d_TextureEnv)& theTextureEnv)
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  if (!aCtx.IsNull() && !myTextureEnv.IsNull())
  {
    aCtx->DelayedRelease (myTextureEnv);
  }

  myToUpdateEnvironmentMap = Standard_True;
  myTextureEnvData = theTextureEnv;
  myTextureEnv.Nullify();
  initTextureEnv (aCtx);
}

// =======================================================================
// function : initTextureEnv
// purpose  :
// =======================================================================
void OpenGl_View::initTextureEnv (const Handle(OpenGl_Context)& theContext)
{
  if (myTextureEnvData.IsNull()
    ||  theContext.IsNull()
    || !theContext->MakeCurrent())
  {
    return;
  }

  myTextureEnv = new OpenGl_Texture (myTextureEnvData->GetParams());
  Handle(Image_PixMap) anImage = myTextureEnvData->GetImage();
  if (!anImage.IsNull())
  {
    myTextureEnv->Init (theContext, *anImage.operator->(), myTextureEnvData->Type());
  }
}

// =======================================================================
// function : SetImmediateModeDrawToFront
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer)
{
  const Standard_Boolean aPrevMode = myTransientDrawToFront;
  myTransientDrawToFront = theDrawToFrontBuffer;
  return aPrevMode;
}

// =======================================================================
// function : SetWindow
// purpose  :
// =======================================================================
void OpenGl_View::SetWindow (const Handle(Aspect_Window)& theWindow,
                             const Aspect_RenderingContext theContext)
{
  myWindow = myDriver->CreateRenderWindow (theWindow, theContext);
  Standard_ASSERT_RAISE (!myWindow.IsNull(),
                         "OpenGl_View::SetWindow, "
                         "Failed to create OpenGl window.");

  myWorkspace = new OpenGl_Workspace (this, myWindow);
  myWorldViewProjState.Reset();
  myToUpdateEnvironmentMap = Standard_True;
  myHasFboBlit = Standard_True;
  Invalidate();

  // Environment texture resource does not support lazy initialization.
  initTextureEnv (myWorkspace->GetGlContext());
}

// =======================================================================
// function : Resized
// purpose  :
// =======================================================================
void OpenGl_View::Resized()
{
  if (myWindow.IsNull())
    return;

  myWindow->Resize();
}

// =======================================================================
// function : TriedronDisplay
// purpose  :
// =======================================================================
void OpenGl_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                                   const Quantity_NameOfColor          theColor,
                                   const Standard_Real                 theScale,
                                   const Standard_Boolean              theAsWireframe)
{
  myToShowTrihedron = true;
  myTrihedron.SetWireframe   (theAsWireframe);
  myTrihedron.SetPosition    (thePosition);
  myTrihedron.SetScale       (theScale);
  myTrihedron.SetLabelsColor (theColor);
}

// =======================================================================
// function : TriedronErase
// purpose  :
// =======================================================================
void OpenGl_View::TriedronErase()
{
  myToShowTrihedron = false;
  myTrihedron.Release (myWorkspace->GetGlContext().operator->());
}

// =======================================================================
// function : ZBufferTriedronSetup
// purpose  :
// =======================================================================
void OpenGl_View::ZBufferTriedronSetup (const Quantity_NameOfColor theXColor,
                                        const Quantity_NameOfColor theYColor,
                                        const Quantity_NameOfColor theZColor,
                                        const Standard_Real theSizeRatio,
                                        const Standard_Real theAxisDiametr,
                                        const Standard_Integer theNbFacettes)
{
  myTrihedron.SetArrowsColors  (theXColor, theYColor, theZColor);
  myTrihedron.SetSizeRatio     (theSizeRatio);
  myTrihedron.SetNbFacets      (theNbFacettes);
  myTrihedron.SetArrowDiameter (theAxisDiametr);
}

// =======================================================================
// function : TriedronEcho
// purpose  :
// =======================================================================
void OpenGl_View::TriedronEcho (const Aspect_TypeOfTriedronEcho /*theType*/)
{
  // do nothing
}

// =======================================================================
// function : SetMinMaxValuesCallback
// purpose  :
// =======================================================================
static void SetMinMaxValuesCallback (Graphic3d_CView* theView)
{
  OpenGl_View* aView = dynamic_cast<OpenGl_View*>(theView);
  if (aView == NULL)
    return;

  Bnd_Box aBox = theView->MinMaxValues();
  if (!aBox.IsVoid())
  {
    gp_Pnt aMin = aBox.CornerMin();
    gp_Pnt aMax = aBox.CornerMax();

    Graphic3d_Vec3 aMinVec ((Standard_ShortReal )aMin.X(), (Standard_ShortReal )aMin.Y(), (Standard_ShortReal )aMin.Z());
    Graphic3d_Vec3 aMaxVec ((Standard_ShortReal )aMax.X(), (Standard_ShortReal )aMax.Y(), (Standard_ShortReal )aMax.Z());
    aView->GraduatedTrihedronMinMaxValues (aMinVec, aMaxVec);
  }
}

// =======================================================================
// function : GraduatedTrihedronDisplay
// purpose  :
// =======================================================================
void OpenGl_View::GraduatedTrihedronDisplay (const Graphic3d_GraduatedTrihedron& theTrihedronData)
{
  myGTrihedronData = theTrihedronData;
  myGTrihedronData.PtrView = this;
  myGTrihedronData.CubicAxesCallback = SetMinMaxValuesCallback;
  myGraduatedTrihedron.SetValues (myGTrihedronData);
  myToShowGradTrihedron = true;
}

// =======================================================================
// function : GraduatedTrihedronErase
// purpose  :
// =======================================================================
void OpenGl_View::GraduatedTrihedronErase()
{
  myGTrihedronData.PtrView = NULL;
  myGraduatedTrihedron.Release (myWorkspace->GetGlContext().operator->());
  myToShowGradTrihedron = false;
}

// =======================================================================
// function : GraduatedTrihedronMinMaxValues
// purpose  :
// =======================================================================
void OpenGl_View::GraduatedTrihedronMinMaxValues (const Graphic3d_Vec3 theMin, const Graphic3d_Vec3 theMax)
{
  myGraduatedTrihedron.SetMinMax (theMin, theMax);
}

// =======================================================================
// function : ReadDepths
// purpose  :
// =======================================================================
void OpenGl_View::ReadDepths (const Standard_Integer theX,
                              const Standard_Integer theY,
                              const Standard_Integer theWidth,
                              const Standard_Integer theHeight,
                              const Standard_Address theBuffer) const
{
  if (myWindow.IsNull())
    return;

  myWindow->ReadDepths (theX, theY, theWidth, theHeight, (float*)theBuffer);
}

// =======================================================================
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::BufferDump (Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType)
{
  return myWorkspace->BufferDump (myFBO, theImage, theBufferType);
}

// =======================================================================
// function : Background
// purpose  :
// =======================================================================
Aspect_Background OpenGl_View::Background() const
{
  return Aspect_Background (Quantity_Color (myBgColor.rgb[0], myBgColor.rgb[1], myBgColor.rgb[2], Quantity_TOC_RGB));
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void OpenGl_View::SetBackground (const Aspect_Background& theBackground)
{
  Quantity_Color aBgColor = theBackground.Color();
  myBgColor.rgb[0] = static_cast<float> (aBgColor.Red());
  myBgColor.rgb[1] = static_cast<float> (aBgColor.Green());
  myBgColor.rgb[2] = static_cast<float> (aBgColor.Blue());
  myFog.Color      = myBgColor;
}

// =======================================================================
// function : GradientBackground
// purpose  :
// =======================================================================
Aspect_GradientBackground OpenGl_View::GradientBackground() const
{
  Quantity_Color aColor1, aColor2;
  aColor1.SetValues (myBgGradientArray->GradientColor (0).r(),
                     myBgGradientArray->GradientColor (0).g(),
                     myBgGradientArray->GradientColor (0).b(), Quantity_TOC_RGB);
  aColor2.SetValues (myBgGradientArray->GradientColor (1).r(),
                     myBgGradientArray->GradientColor (1).g(),
                     myBgGradientArray->GradientColor (1).b(), Quantity_TOC_RGB);
  return Aspect_GradientBackground (aColor1, aColor2, myBgGradientArray->GradientFillMethod());
}

// =======================================================================
// function : SetGradientBackground
// purpose  :
// =======================================================================
void OpenGl_View::SetGradientBackground (const Aspect_GradientBackground& theBackground)
{
  Quantity_Color aColor1, aColor2;
  theBackground.Colors (aColor1, aColor2);
  myBgGradientArray->SetGradientParameters (aColor1, aColor2, theBackground.BgGradientFillMethod());
}

// =======================================================================
// function : SetBackgroundImage
// purpose  :
// =======================================================================
void OpenGl_View::SetBackgroundImage (const TCollection_AsciiString& theFilePath)
{
  // Prepare aspect for texture storage
  myBackgroundImagePath = theFilePath;
  Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
  Handle(Graphic3d_Texture2Dmanual) aTextureMap = new Graphic3d_Texture2Dmanual (TCollection_AsciiString (theFilePath));
  aTextureMap->EnableRepeat();
  aTextureMap->DisableModulate();
  aTextureMap->GetParams()->SetGenMode (Graphic3d_TOTM_MANUAL,
                                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f),
                                        Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
  anAspect->SetTextureMap (aTextureMap);
  anAspect->SetInteriorStyle (Aspect_IS_SOLID);
  // Enable texture mapping
  if (aTextureMap->IsDone())
  {
    anAspect->SetTextureMapOn();
  }
  else
  {
    anAspect->SetTextureMapOff();
    return;
  }

  // Set texture parameters
  myTextureParams->SetAspect (anAspect);
}

// =======================================================================
// function : BackgroundImageStyle
// purpose  :
// =======================================================================
Aspect_FillMethod OpenGl_View::BackgroundImageStyle() const
{
  return myBgTextureArray->TextureFillMethod();
}

// =======================================================================
// function : SetBackgroundImageStyle
// purpose  :
// =======================================================================
void OpenGl_View::SetBackgroundImageStyle (const Aspect_FillMethod theFillStyle)
{
  myBgTextureArray->SetTextureFillMethod (theFillStyle);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================
void OpenGl_View::AddZLayer (const Graphic3d_ZLayerId theLayerId)
{
  myZLayers.AddLayer (theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================
void OpenGl_View::RemoveZLayer (const Graphic3d_ZLayerId theLayerId)
{
  myZLayers.RemoveLayer (theLayerId);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void OpenGl_View::SetZLayerSettings (const Graphic3d_ZLayerId        theLayerId,
                                     const Graphic3d_ZLayerSettings& theSettings)
{
  myZLayers.SetLayerSettings (theLayerId, theSettings);
}

//=======================================================================
//function : FBO
//purpose  :
//=======================================================================
Graphic3d_PtrFrameBuffer OpenGl_View::FBO() const
{
  return reinterpret_cast<Graphic3d_PtrFrameBuffer> (myFBO);
}

//=======================================================================
//function : SetFBO
//purpose  :
//=======================================================================
void OpenGl_View::SetFBO (const Graphic3d_PtrFrameBuffer theFBO)
{
  myFBO = reinterpret_cast<OpenGl_FrameBuffer*> (theFBO);
}

//=======================================================================
//function : FBOCreate
//purpose  :
//=======================================================================
Graphic3d_PtrFrameBuffer OpenGl_View::FBOCreate (const Standard_Integer theWidth,
                                                 const Standard_Integer theHeight)
{
  return myWorkspace->FBOCreate (theWidth, theHeight);
}

//=======================================================================
//function : FBORelease
//purpose  :
//=======================================================================
void OpenGl_View::FBORelease (Graphic3d_PtrFrameBuffer& theFBOPtr)
{
  myWorkspace->FBORelease (theFBOPtr);
}

//=======================================================================
//function : FBOGetDimensions
//purpose  :
//=======================================================================
void OpenGl_View::FBOGetDimensions (const Graphic3d_PtrFrameBuffer theFBOPtr,
                                    Standard_Integer& theWidth,
                                    Standard_Integer& theHeight,
                                    Standard_Integer& theWidthMax,
                                    Standard_Integer& theHeightMax)
{
  const OpenGl_FrameBuffer* aFrameBuffer = (const OpenGl_FrameBuffer* )theFBOPtr;
  theWidth     = aFrameBuffer->GetVPSizeX(); // current viewport size
  theHeight    = aFrameBuffer->GetVPSizeY();
  theWidthMax  = aFrameBuffer->GetSizeX(); // texture size
  theHeightMax = aFrameBuffer->GetSizeY();
}

//=======================================================================
//function : FBOChangeViewport
//purpose  :
//=======================================================================
void OpenGl_View::FBOChangeViewport (Graphic3d_PtrFrameBuffer& theFBOPtr,
                                     const Standard_Integer theWidth,
                                     const Standard_Integer theHeight)
{
  OpenGl_FrameBuffer* aFrameBuffer = (OpenGl_FrameBuffer* )theFBOPtr;
  aFrameBuffer->ChangeViewport (theWidth, theHeight);
}

// =======================================================================
// function : Export
// purpose  :
// =======================================================================
#ifdef HAVE_GL2PS
Standard_Boolean OpenGl_View::Export (const Standard_CString theFileName,
                                      const Graphic3d_ExportFormat theFormat,
                                      const Graphic3d_SortType theSortType)
{
  // gl2psBeginPage() will call OpenGL functions
  // so we should activate correct GL context before redraw scene call
  if (!myWorkspace->Activate())
  {
    return Standard_False;
  }

  Standard_Integer aFormat = -1;
  Standard_Integer aSortType = Graphic3d_ST_BSP_Tree;
  switch (theFormat)
  {
    case Graphic3d_EF_PostScript:
      aFormat = GL2PS_PS;
      break;
    case Graphic3d_EF_EnhPostScript:
      aFormat = GL2PS_EPS;
      break;
    case Graphic3d_EF_TEX:
      aFormat = GL2PS_TEX;
      break;
    case Graphic3d_EF_PDF:
      aFormat = GL2PS_PDF;
      break;
    case Graphic3d_EF_SVG:
      aFormat = GL2PS_SVG;
      break;
    case Graphic3d_EF_PGF:
      aFormat = GL2PS_PGF;
      break;
    case Graphic3d_EF_EMF:
      //aFormat = GL2PS_EMF;
      aFormat = GL2PS_PGF + 1; // 6
      break;
    default:
      // unsupported format
      return Standard_False;
  }

  switch (theSortType)
  {
    case Graphic3d_ST_Simple:
      aSortType = GL2PS_SIMPLE_SORT;
      break;
    case Graphic3d_ST_BSP_Tree:
      aSortType = GL2PS_BSP_SORT;
      break;
  }

  GLint aViewport[4];
  aViewport[0] = 0;
  aViewport[1] = 0;
  aViewport[2] = myWindow->Width();
  aViewport[3] = myWindow->Height();

  GLint aBufferSize = 1024 * 1024;
  GLint anErrCode = GL2PS_SUCCESS;

  // gl2ps uses standard write functions and do not check locale
  Standard_CLocaleSentry aLocaleSentry;

  while (aBufferSize > 0)
  {
    // current patch for EMF support in gl2ps uses WinAPI functions to create file
    FILE* aFileH = (theFormat != Graphic3d_EF_EMF) ? fopen (theFileName, "wb") : NULL;
    anErrCode = gl2psBeginPage ("", "", aViewport, aFormat, aSortType,
                    GL2PS_DRAW_BACKGROUND | GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT/* | GL2PS_SIMPLE_LINE_OFFSET*/,
                    GL_RGBA, 0, NULL,
                    0, 0, 0, aBufferSize, aFileH, theFileName);
    if (anErrCode != GL2PS_SUCCESS)
    {
      // initialization failed
      if (aFileH != NULL)
        fclose (aFileH);
      break;
    }
    Redraw();

    anErrCode = gl2psEndPage();
    if (aFileH != NULL)
      fclose (aFileH);

    if (anErrCode == GL2PS_OVERFLOW)
      aBufferSize *= 2;
    else
      break;
  }
  return anErrCode == GL2PS_SUCCESS;
}
#else
Standard_Boolean OpenGl_View::Export (const Standard_CString /*theFileName*/,
                                      const Graphic3d_ExportFormat /*theFormat*/,
                                      const Graphic3d_SortType /*theSortType*/)
{
    return Standard_False;
}
#endif

//=======================================================================
//function : displayStructure
//purpose  :
//=======================================================================
void OpenGl_View::displayStructure (const Handle(Graphic3d_CStructure)& theStructure,
                                    const Standard_Integer              thePriority)
{
  const OpenGl_Structure*  aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  const Graphic3d_ZLayerId aZLayer = aStruct->ZLayer();
  myZLayers.AddStructure (aStruct, aZLayer, thePriority);
}

//=======================================================================
//function : eraseStructure
//purpose  :
//=======================================================================
void OpenGl_View::eraseStructure (const Handle(Graphic3d_CStructure)& theStructure)
{
  const OpenGl_Structure*  aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  myZLayers.RemoveStructure (aStruct);
}

//=======================================================================
//function : changeZLayer
//purpose  :
//=======================================================================
void OpenGl_View::changeZLayer (const Handle(Graphic3d_CStructure)& theStructure,
                                const Graphic3d_ZLayerId theNewLayerId)
{
  const Graphic3d_ZLayerId anOldLayer = theStructure->ZLayer();
  const OpenGl_Structure* aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  myZLayers.ChangeLayer (aStruct, anOldLayer, theNewLayerId);
}

//=======================================================================
//function : changePriority
//purpose  :
//=======================================================================
void OpenGl_View::changePriority (const Handle(Graphic3d_CStructure)& theStructure,
                                  const Standard_Integer theNewPriority)
{
  const Graphic3d_ZLayerId aLayerId = theStructure->ZLayer();
  const OpenGl_Structure* aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure.operator->());
  myZLayers.ChangePriority (aStruct, aLayerId, theNewPriority);
}
