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
#include <OpenGl_Window.hxx>
#include <OpenGl_Workspace.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_CLocaleSentry.hxx>

#include "../Graphic3d/Graphic3d_Structure.pxx"

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_View,Graphic3d_CView)

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
OpenGl_View::OpenGl_View (const Handle(Graphic3d_StructureManager)& theMgr,
                          const Handle(OpenGl_GraphicDriver)& theDriver,
                          const Handle(OpenGl_Caps)& theCaps,
                          OpenGl_StateCounter* theCounter)
: Graphic3d_CView  (theMgr),
  myDriver         (theDriver.operator->()),
  myCaps           (theCaps),
  myWasRedrawnGL   (Standard_False),
  myCulling        (Standard_True),
  myShadingModel   (Graphic3d_TOSM_FACET),
  myBackfacing     (Graphic3d_TOBM_AUTOMATIC),
  myBgColor        (Quantity_NOC_BLACK),
  myCamera         (new Graphic3d_Camera()),
  myToShowGradTrihedron  (false),
  myZLayers        (Structure_MAX_PRIORITY - Structure_MIN_PRIORITY + 1),
  myStateCounter         (theCounter),
  myLastLightSourceState (0, 0),
#if !defined(GL_ES_VERSION_2_0)
  myFboColorFormat       (GL_RGBA8),
#else
  myFboColorFormat       (GL_RGBA),
#endif
  myFboDepthFormat       (GL_DEPTH24_STENCIL8),
  myToFlipOutput         (Standard_False),
  myFrameCounter         (0),
  myHasFboBlit           (Standard_True),
  myToDisableOIT         (Standard_False),
  myToDisableOITMSAA     (Standard_False),
  myToDisableMSAA        (Standard_False),
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
  myRaytraceBVHBuilder (new BVH_BinnedBuilder<Standard_ShortReal, 3, BVH_Constants_NbBinsBest> (BVH_Constants_LeafNodeSizeAverage,
                                                                                                BVH_Constants_MaxTreeDepth,
                                                                                                Standard_False,
                                                                                                OSD_Parallel::NbLogicalProcessors() + 1)),
  myRaytraceSceneRadius  (0.0f),
  myRaytraceSceneEpsilon (1.0e-6f),
  myToUpdateEnvironmentMap (Standard_False),
  myRaytraceLayerListState (0),
  myPrevCameraApertureRadius(0.f),
  myPrevCameraFocalPlaneDist(0.f)
{
  myWorkspace = new OpenGl_Workspace (this, NULL);

  OpenGl_Light       aLight;
  aLight.Type        = Graphic3d_TOLS_AMBIENT;
  aLight.IsHeadlight = Standard_False;
  aLight.Color.r()   = 1.;
  aLight.Color.g()   = 1.;
  aLight.Color.b()   = 1.;
  myNoShadingLight.Append (aLight);

  myCurrLightSourceState     = myStateCounter->Increment();
  myMainSceneFbos[0]         = new OpenGl_FrameBuffer();
  myMainSceneFbos[1]         = new OpenGl_FrameBuffer();
  myMainSceneFbosOit[0]      = new OpenGl_FrameBuffer();
  myMainSceneFbosOit[1]      = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[0]    = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[1]    = new OpenGl_FrameBuffer();
  myImmediateSceneFbosOit[0] = new OpenGl_FrameBuffer();
  myImmediateSceneFbosOit[1] = new OpenGl_FrameBuffer();
  myOpenGlFBO                = new OpenGl_FrameBuffer();
  myOpenGlFBO2               = new OpenGl_FrameBuffer();
  myRaytraceFBO1[0]          = new OpenGl_FrameBuffer();
  myRaytraceFBO1[1]          = new OpenGl_FrameBuffer();
  myRaytraceFBO2[0]          = new OpenGl_FrameBuffer();
  myRaytraceFBO2[1]          = new OpenGl_FrameBuffer();
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
  myGraduatedTrihedron.Release (theCtx.operator->());

  if (!myTextureEnv.IsNull())
  {
    for (OpenGl_TextureSet::Iterator aTextureIter (myTextureEnv); aTextureIter.More(); aTextureIter.Next())
    {
      theCtx->DelayedRelease (aTextureIter.ChangeValue());
      aTextureIter.ChangeValue().Nullify();
    }
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

  myMainSceneFbos[0]        ->Release (theCtx.operator->());
  myMainSceneFbos[1]        ->Release (theCtx.operator->());
  myMainSceneFbosOit[0]     ->Release (theCtx.operator->());
  myMainSceneFbosOit[1]     ->Release (theCtx.operator->());
  myImmediateSceneFbos[0]   ->Release (theCtx.operator->());
  myImmediateSceneFbos[1]   ->Release (theCtx.operator->());
  myImmediateSceneFbosOit[0]->Release (theCtx.operator->());
  myImmediateSceneFbosOit[1]->Release (theCtx.operator->());
  myOpenGlFBO               ->Release (theCtx.operator->());
  myOpenGlFBO2              ->Release (theCtx.operator->());
  myFullScreenQuad           .Release (theCtx.operator->());
  myFullScreenQuadFlip       .Release (theCtx.operator->());

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
void OpenGl_View::SetCamera(const Handle(Graphic3d_Camera)& theCamera)
{
  myCamera = theCamera;
}

// =======================================================================
// function : SetLocalOrigin
// purpose  :
// =======================================================================
void OpenGl_View::SetLocalOrigin (const gp_XYZ& theOrigin)
{
  myLocalOrigin = theOrigin;
  const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext();
  if (!aCtx.IsNull())
  {
    aCtx->ShaderManager()->SetLocalOrigin (theOrigin);
  }
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
    for (OpenGl_TextureSet::Iterator aTextureIter (myTextureEnv); aTextureIter.More(); aTextureIter.Next())
    {
      aCtx->DelayedRelease (aTextureIter.ChangeValue());
      aTextureIter.ChangeValue().Nullify();
    }
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

  myTextureEnv = new OpenGl_TextureSet (1);
  Handle(OpenGl_Texture)& aTextureEnv = myTextureEnv->ChangeFirst();
  aTextureEnv = new OpenGl_Texture (myTextureEnvData->GetId(), myTextureEnvData->GetParams());
  Handle(Image_PixMap) anImage = myTextureEnvData->GetImage();
  if (!anImage.IsNull())
  {
    aTextureEnv->Init (theContext, *anImage.operator->(), myTextureEnvData->Type());
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
// function : BufferDump
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::BufferDump (Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType)
{
  if (theBufferType != Graphic3d_BT_RGB_RayTraceHdrLeft)
  {
    return myWorkspace->BufferDump(myFBO, theImage, theBufferType);
  }

  if (!myRaytraceParameters.AdaptiveScreenSampling)
  {
    return myWorkspace->BufferDump(myAccumFrames % 2 ? myRaytraceFBO2[0] : myRaytraceFBO1[0], theImage, theBufferType);
  }

#if defined(GL_ES_VERSION_2_0)
  return false;
#else
  if (theImage.Format() != Image_Format_RGBF)
  {
    return false;
  }

  const GLuint aW = myRaytraceOutputTexture[0]->SizeX();
  const GLuint aH = myRaytraceOutputTexture[0]->SizeY();
  if (aW / 3 != theImage.SizeX() || aH / 2 != theImage.SizeY())
  {
    return false;
  }

  std::vector<GLfloat> aValues;
  try
  {
    aValues.resize (aW * aH);
  }
  catch (const std::bad_alloc&)
  {
    return false;
  }

  glBindTexture (GL_TEXTURE_RECTANGLE, myRaytraceOutputTexture[0]->TextureId());
  glGetTexImage (GL_TEXTURE_RECTANGLE, 0, OpenGl_TextureFormat::Create<GLfloat, 1>().Format(), GL_FLOAT, &aValues[0]);
  glBindTexture (GL_TEXTURE_RECTANGLE, 0);
  for (unsigned int aRow = 0; aRow < aH; aRow += 2)
  {
    for (unsigned int aCol = 0; aCol < aW; aCol += 3)
    {
      float* anImageValue = theImage.ChangeValue<float[3]> ((aH - aRow) / 2 - 1, aCol / 3);
      float aInvNbSamples = 1.f / aValues[aRow * aW + aCol + aW];
      anImageValue[0] = aValues[aRow * aW + aCol] * aInvNbSamples;
      anImageValue[1] = aValues[aRow * aW + aCol + 1] * aInvNbSamples;
      anImageValue[2] = aValues[aRow * aW + aCol + 1 + aW] * aInvNbSamples;
    }
  }

  return true;
#endif
}

// =======================================================================
// function : Background
// purpose  :
// =======================================================================
Aspect_Background OpenGl_View::Background() const
{
  return Aspect_Background (myBgColor.GetRGB());
}

// =======================================================================
// function : SetBackground
// purpose  :
// =======================================================================
void OpenGl_View::SetBackground (const Aspect_Background& theBackground)
{
  myBgColor.SetRGB (theBackground.Color());
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
  anAspect->SetSuppressBackFaces (false);
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
//function : ZLayerMax
//purpose  :
//=======================================================================
Standard_Integer OpenGl_View::ZLayerMax() const
{
  Standard_Integer aLayerMax = Graphic3d_ZLayerId_Default;
  for (OpenGl_LayerSeqIds::Iterator aMapIt(myZLayers.LayerIDs()); aMapIt.More(); aMapIt.Next())
  {
    aLayerMax = Max (aLayerMax, aMapIt.Value());
  }

  return aLayerMax;
}

//=======================================================================
//function : InvalidateZLayerBoundingBox
//purpose  :
//=======================================================================
void OpenGl_View::InvalidateZLayerBoundingBox (const Graphic3d_ZLayerId theLayerId) const
{
  if (myZLayers.LayerIDs().IsBound (theLayerId))
  {
    myZLayers.Layer (theLayerId).InvalidateBoundingBox();
  }
  else
  {
    const Standard_Integer aLayerMax = ZLayerMax();
    for (Standard_Integer aLayerId = Graphic3d_ZLayerId_Default; aLayerId < aLayerMax; ++aLayerId)
    {
      if (myZLayers.LayerIDs().IsBound (aLayerId))
      {
        const OpenGl_Layer& aLayer = myZLayers.Layer (aLayerId);
        if (aLayer.NbOfTransformPersistenceObjects() > 0)
        {
          aLayer.InvalidateBoundingBox();
        }
      }
    }
  }
}

//=======================================================================
//function : ZLayerBoundingBox
//purpose  :
//=======================================================================
Bnd_Box OpenGl_View::ZLayerBoundingBox (const Graphic3d_ZLayerId        theLayerId,
                                        const Handle(Graphic3d_Camera)& theCamera,
                                        const Standard_Integer          theWindowWidth,
                                        const Standard_Integer          theWindowHeight,
                                        const Standard_Boolean          theToIncludeAuxiliary) const
{
  Bnd_Box aBox;
  if (myZLayers.LayerIDs().IsBound (theLayerId))
  {
    aBox = myZLayers.Layer (theLayerId).BoundingBox (Identification(),
                                                     theCamera,
                                                     theWindowWidth,
                                                     theWindowHeight,
                                                     theToIncludeAuxiliary);
  }

  // add bounding box of gradient/texture background for proper Z-fit
  if (theToIncludeAuxiliary
   && theLayerId == Graphic3d_ZLayerId_BotOSD
   && (myBgTextureArray->IsDefined()
    || myBgGradientArray->IsDefined()))
  {
    // Background is drawn using 2D transformation persistence
    // (e.g. it is actually placed in 3D coordinates within active camera position).
    // We add here full-screen plane with 2D transformation persistence
    // for simplicity (myBgTextureArray might define a little bit different options
    // but it is updated within ::Render())
    const Graphic3d_Mat4d& aProjectionMat = theCamera->ProjectionMatrix();
    const Graphic3d_Mat4d& aWorldViewMat  = theCamera->OrientationMatrix();
    Graphic3d_BndBox3d aBox2d (Graphic3d_Vec3d (0.0, 0.0, 0.0),
                               Graphic3d_Vec3d (double(theWindowWidth), double(theWindowHeight), 0.0));

    Graphic3d_TransformPers aTrsfPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER);
    aTrsfPers.Apply (theCamera,
                     aProjectionMat,
                     aWorldViewMat,
                     theWindowWidth,
                     theWindowHeight,
                     aBox2d);
    aBox.Add (gp_Pnt (aBox2d.CornerMin().x(), aBox2d.CornerMin().y(), aBox2d.CornerMin().z()));
    aBox.Add (gp_Pnt (aBox2d.CornerMax().x(), aBox2d.CornerMax().y(), aBox2d.CornerMax().z()));
  }

  return aBox;
}

//=======================================================================
//function : considerZoomPersistenceObjects
//purpose  :
//=======================================================================
Standard_Real OpenGl_View::considerZoomPersistenceObjects (const Graphic3d_ZLayerId        theLayerId,
                                                           const Handle(Graphic3d_Camera)& theCamera,
                                                           const Standard_Integer          theWindowWidth,
                                                           const Standard_Integer          theWindowHeight) const
{
  if (myZLayers.LayerIDs().IsBound (theLayerId))
  {
    return myZLayers.Layer (theLayerId).considerZoomPersistenceObjects (Identification(),
                                                                        theCamera,
                                                                        theWindowWidth,
                                                                        theWindowHeight);
  }

  return 1.0;
}

//=======================================================================
//function : FBO
//purpose  :
//=======================================================================
Handle(Standard_Transient) OpenGl_View::FBO() const
{
  return Handle(Standard_Transient)(myFBO);
}

//=======================================================================
//function : SetFBO
//purpose  :
//=======================================================================
void OpenGl_View::SetFBO (const Handle(Standard_Transient)& theFbo)
{
  myFBO = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
}

//=======================================================================
//function : FBOCreate
//purpose  :
//=======================================================================
Handle(Standard_Transient) OpenGl_View::FBOCreate (const Standard_Integer theWidth,
                                                   const Standard_Integer theHeight)
{
  return myWorkspace->FBOCreate (theWidth, theHeight);
}

//=======================================================================
//function : FBORelease
//purpose  :
//=======================================================================
void OpenGl_View::FBORelease (Handle(Standard_Transient)& theFbo)
{
  Handle(OpenGl_FrameBuffer) aFrameBuffer = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  myWorkspace->FBORelease (aFrameBuffer);
  theFbo.Nullify();
}

//=======================================================================
//function : FBOGetDimensions
//purpose  :
//=======================================================================
void OpenGl_View::FBOGetDimensions (const Handle(Standard_Transient)& theFbo,
                                    Standard_Integer& theWidth,
                                    Standard_Integer& theHeight,
                                    Standard_Integer& theWidthMax,
                                    Standard_Integer& theHeightMax)
{
  const Handle(OpenGl_FrameBuffer) aFrameBuffer = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  theWidth     = aFrameBuffer->GetVPSizeX(); // current viewport size
  theHeight    = aFrameBuffer->GetVPSizeY();
  theWidthMax  = aFrameBuffer->GetSizeX(); // texture size
  theHeightMax = aFrameBuffer->GetSizeY();
}

//=======================================================================
//function : FBOChangeViewport
//purpose  :
//=======================================================================
void OpenGl_View::FBOChangeViewport (const Handle(Standard_Transient)& theFbo,
                                     const Standard_Integer theWidth,
                                     const Standard_Integer theHeight)
{
  const Handle(OpenGl_FrameBuffer) aFrameBuffer = Handle(OpenGl_FrameBuffer)::DownCast (theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

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
  Update (anOldLayer);
  Update (theNewLayerId);
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

//=======================================================================
//function : DiagnosticInformation
//purpose  :
//=======================================================================
void OpenGl_View::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                         Graphic3d_DiagnosticInfo theFlags) const
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  if (!myWorkspace->Activate()
   || aCtx.IsNull())
  {
    return;
  }

  aCtx->DiagnosticInformation (theDict, theFlags);
  if ((theFlags & Graphic3d_DiagnosticInfo_FrameBuffer) != 0)
  {
    TCollection_AsciiString aResRatio (myRenderParams.ResolutionRatio());
    theDict.ChangeFromIndex (theDict.Add ("ResolutionRatio", aResRatio)) = aResRatio;
  }
}
