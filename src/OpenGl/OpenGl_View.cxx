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
  myBackfacing     (Graphic3d_TOBM_AUTOMATIC),
  myToShowGradTrihedron  (false),
  myZLayers        (Structure_MAX_PRIORITY - Structure_MIN_PRIORITY + 1),
  myStateCounter         (theCounter),
  myCurrLightSourceState (theCounter->Increment()),
  myLightsRevision       (0),
  myLastLightSourceState (0, 0),
  mySRgbState            (-1),
  myFboColorFormat       (GL_SRGB8_ALPHA8), // note that GL_SRGB8 is not required to be renderable, unlike GL_RGB8, GL_RGBA8, GL_SRGB8_ALPHA8
  myFboDepthFormat       (GL_DEPTH24_STENCIL8),
  myToFlipOutput         (Standard_False),
  //
  myFrameCounter         (0),
  myHasFboBlit           (Standard_True),
  myToDisableOIT         (Standard_False),
  myToDisableOITMSAA     (Standard_False),
  myToDisableMSAA        (Standard_False),
  myTransientDrawToFront (Standard_True),
  myBackBufferRestored   (Standard_False),
  myIsImmediateDrawn     (Standard_False),
  myTextureParams   (new OpenGl_Aspects()),
  myCubeMapParams   (new OpenGl_Aspects()),
  myBackgroundType  (Graphic3d_TOB_NONE),
  myPBREnvState     (OpenGl_PBREnvState_NONEXISTENT),
  myPBREnvRequest   (OpenGl_PBREnvRequest_NONE),
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
  for (int i = 0; i < Graphic3d_TypeOfBackground_NB; ++i)
  {
    myBackgrounds[i] = new OpenGl_BackgroundArray(Graphic3d_TypeOfBackground(i));
  }

  myWorkspace = new OpenGl_Workspace (this, NULL);

  Handle(Graphic3d_CLight) aLight = new Graphic3d_CLight (Graphic3d_TOLS_AMBIENT);
  aLight->SetHeadlight (false);
  aLight->SetColor (Quantity_NOC_WHITE);
  myLights = new Graphic3d_LightSet();
  myNoShadingLight = new Graphic3d_LightSet();
  myNoShadingLight->Add (aLight);

  myMainSceneFbos[0]         = new OpenGl_FrameBuffer();
  myMainSceneFbos[1]         = new OpenGl_FrameBuffer();
  myMainSceneFbosOit[0]      = new OpenGl_FrameBuffer();
  myMainSceneFbosOit[1]      = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[0]    = new OpenGl_FrameBuffer();
  myImmediateSceneFbos[1]    = new OpenGl_FrameBuffer();
  myImmediateSceneFbosOit[0] = new OpenGl_FrameBuffer();
  myImmediateSceneFbosOit[1] = new OpenGl_FrameBuffer();
  myXrSceneFbo               = new OpenGl_FrameBuffer();
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
  for (int i = 0; i < Graphic3d_TypeOfBackground_NB; ++i)
  {
    OpenGl_Element::Destroy(NULL, myBackgrounds[i]);
  }

  OpenGl_Element::Destroy (NULL, myTextureParams);
  OpenGl_Element::Destroy (NULL, myCubeMapParams);
}

// =======================================================================
// function : releaseSrgbResources
// purpose  :
// =======================================================================
void OpenGl_View::releaseSrgbResources (const Handle(OpenGl_Context)& theCtx)
{
  myRenderParams.RebuildRayTracingShaders = true;

  if (!myTextureEnv.IsNull())
  {
    if (!theCtx.IsNull())
    {
      for (OpenGl_TextureSet::Iterator aTextureIter (myTextureEnv); aTextureIter.More(); aTextureIter.Next())
      {
        theCtx->DelayedRelease (aTextureIter.ChangeValue());
        aTextureIter.ChangeValue().Nullify();
      }
    }
    myTextureEnv.Nullify();
  }

  if (myTextureParams != NULL)
  {
    myTextureParams->Release (theCtx.get());
  }

  if (myCubeMapParams != NULL)
  {
    myCubeMapParams->Release (theCtx.get());
  }

  for (int i = 0; i < Graphic3d_TypeOfBackground_NB; ++i)
  {
    if (myBackgrounds[i] != NULL)
    {
      myBackgrounds[i]->Release (theCtx.get());
    }
  }

  myMainSceneFbos[0]        ->Release (theCtx.get());
  myMainSceneFbos[1]        ->Release (theCtx.get());
  myMainSceneFbosOit[0]     ->Release (theCtx.get());
  myMainSceneFbosOit[1]     ->Release (theCtx.get());
  myImmediateSceneFbos[0]   ->Release (theCtx.get());
  myImmediateSceneFbos[1]   ->Release (theCtx.get());
  myImmediateSceneFbosOit[0]->Release (theCtx.get());
  myImmediateSceneFbosOit[1]->Release (theCtx.get());
  myXrSceneFbo              ->Release (theCtx.get());
  myOpenGlFBO               ->Release (theCtx.get());
  myOpenGlFBO2              ->Release (theCtx.get());
  myFullScreenQuad           .Release (theCtx.get());
  myFullScreenQuadFlip       .Release (theCtx.get());

  // Technically we should also re-initialize all sRGB/RGB8 color textures.
  // But for now consider this sRGB disabling/enabling to be done at application start-up
  // and re-create dynamically only frame buffers.
}

// =======================================================================
// function : ReleaseGlResources
// purpose  :
// =======================================================================
void OpenGl_View::ReleaseGlResources (const Handle(OpenGl_Context)& theCtx)
{
  myGraduatedTrihedron.Release (theCtx.get());
  myFrameStatsPrs.Release (theCtx.get());

  releaseSrgbResources (theCtx);

  releaseRaytraceResources (theCtx);

  if (!myPBREnvironment.IsNull())
  {
    myPBREnvironment->Release (theCtx.get());
  }
  ReleaseXR();
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

  Handle(OpenGl_Texture) aTextureEnv = new OpenGl_Texture (myTextureEnvData->GetId(), myTextureEnvData->GetParams());
  if (Handle(Image_PixMap) anImage = myTextureEnvData->GetImage (theContext->SupportedTextureFormats()))
  {
    aTextureEnv->Init (theContext, *anImage, myTextureEnvData->Type(), true);
  }
  myTextureEnv = new OpenGl_TextureSet (aTextureEnv);
  myTextureEnv->ChangeTextureSetBits() = Graphic3d_TextureSetBits_BaseColor;
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
// function : GradientBackground
// purpose  :
// =======================================================================
Aspect_GradientBackground OpenGl_View::GradientBackground() const
{
  Quantity_Color aColor1, aColor2;
  aColor1.SetValues (myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (0).r(),
                     myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (0).g(),
                     myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (0).b(), Quantity_TOC_RGB);
  aColor2.SetValues (myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (1).r(),
                     myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (1).g(),
                     myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor (1).b(), Quantity_TOC_RGB);
  return Aspect_GradientBackground (aColor1, aColor2, myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientFillMethod());
}

// =======================================================================
// function : SetGradientBackground
// purpose  :
// =======================================================================
void OpenGl_View::SetGradientBackground (const Aspect_GradientBackground& theBackground)
{
  Quantity_Color aColor1, aColor2;
  theBackground.Colors (aColor1, aColor2);
  myBackgrounds[Graphic3d_TOB_GRADIENT]->SetGradientParameters (aColor1, aColor2, theBackground.BgGradientFillMethod());

  myBackgroundType = Graphic3d_TOB_GRADIENT;
}

// =======================================================================
// function : SetBackgroundImage
// purpose  :
// =======================================================================
void OpenGl_View::SetBackgroundImage (const Handle(Graphic3d_TextureMap)& theTextureMap,
                                      Standard_Boolean theToUpdatePBREnv)
{
  if (theTextureMap.IsNull()
  || !theTextureMap->IsDone())
  {
    if (myBackgroundType == Graphic3d_TOB_TEXTURE
     || myBackgroundType == Graphic3d_TOB_CUBEMAP)
    {
      myBackgroundType = Graphic3d_TOB_NONE;
      if (theToUpdatePBREnv)
      {
        myPBREnvRequest = OpenGl_PBREnvRequest_CLEAR;
      }
    }
    return;
  }

  Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
  Handle(Graphic3d_TextureSet) aTextureSet = new Graphic3d_TextureSet (theTextureMap);
  anAspect->SetInteriorStyle (Aspect_IS_SOLID);
  anAspect->SetSuppressBackFaces (false);
  anAspect->SetShadingModel (Graphic3d_TOSM_UNLIT);
  anAspect->SetTextureSet (aTextureSet);
  anAspect->SetTextureMapOn (true);

  if (Handle(Graphic3d_Texture2D) aTextureMap = Handle(Graphic3d_Texture2D)::DownCast (theTextureMap))
  {
    if (theToUpdatePBREnv && myBackgroundType == Graphic3d_TOB_CUBEMAP)
    {
      myPBREnvRequest = OpenGl_PBREnvRequest_CLEAR;
    }

    myTextureParams->SetAspect (anAspect);
    myBackgroundType  = Graphic3d_TOB_TEXTURE;
    myBackgroundImage = aTextureMap;
    return;
  }

  if (Handle(Graphic3d_CubeMap) aCubeMap = Handle(Graphic3d_CubeMap)::DownCast (theTextureMap))
  {
    if (theToUpdatePBREnv)
    {
      myPBREnvRequest = OpenGl_PBREnvRequest_BAKE;
    }

    aCubeMap->SetMipmapsGeneration (Standard_True);
    if (const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext())
    {
      anAspect->SetShaderProgram (aCtx->ShaderManager()->GetBgCubeMapProgram());
    }

    myCubeMapParams->SetAspect (anAspect);

    const OpenGl_Aspects* anAspectsBackup = myWorkspace->SetAspects (myCubeMapParams);
    myWorkspace->ApplyAspects();
    myWorkspace->SetAspects (anAspectsBackup);
    myWorkspace->ApplyAspects();

    myBackgroundType = Graphic3d_TOB_CUBEMAP;
    myBackgroundCubeMap = aCubeMap;
    return;
  }

  throw Standard_ProgramError ("OpenGl_View::SetBackgroundImage() - invalid texture map set for background");
}

// =======================================================================
// function : BackgroundImageStyle
// purpose  :
// =======================================================================
Aspect_FillMethod OpenGl_View::BackgroundImageStyle() const
{
  return myBackgrounds[Graphic3d_TOB_TEXTURE]->TextureFillMethod();
}

// =======================================================================
// function : SetBackgroundImageStyle
// purpose  :
// =======================================================================
void OpenGl_View::SetBackgroundImageStyle (const Aspect_FillMethod theFillStyle)
{
  myBackgrounds[Graphic3d_TOB_TEXTURE]->SetTextureFillMethod (theFillStyle);
}

// =======================================================================
// function : BackgroundCubeMap
// purpose  :
// =======================================================================
Handle(Graphic3d_CubeMap) OpenGl_View::BackgroundCubeMap() const
{
  return myBackgroundCubeMap;
}

// =======================================================================
// function : SpecIBLMapLevels
// purpose  :
// =======================================================================
unsigned int OpenGl_View::SpecIBLMapLevels() const
{
  return myPBREnvironment.IsNull() ? 0 : myPBREnvironment->SpecMapLevelsNumber();
}

//=======================================================================
//function : InsertLayerBefore
//purpose  :
//=======================================================================
void OpenGl_View::InsertLayerBefore (const Graphic3d_ZLayerId theLayerId,
  const Graphic3d_ZLayerSettings& theSettings,
  const Graphic3d_ZLayerId theLayerAfter)
{
  myZLayers.InsertLayerBefore (theLayerId, theSettings, theLayerAfter);
}

//=======================================================================
//function : InsertLayerAfter
//purpose  :
//=======================================================================
void OpenGl_View::InsertLayerAfter (const Graphic3d_ZLayerId theLayerId,
  const Graphic3d_ZLayerSettings& theSettings,
  const Graphic3d_ZLayerId theLayerBefore)
{
  myZLayers.InsertLayerAfter (theLayerId, theSettings, theLayerBefore);
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
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (myZLayers.Layers()); aLayerIter.More(); aLayerIter.Next())
  {
    aLayerMax = Max (aLayerMax, aLayerIter.Value()->LayerId());
  }
  return aLayerMax;
}

//=======================================================================
//function : Layers
//purpose  :
//=======================================================================
const NCollection_List<Handle(Graphic3d_Layer)>& OpenGl_View::Layers() const
{
  return myZLayers.Layers();
}

//=======================================================================
//function : Layer
//purpose  :
//=======================================================================
Handle(Graphic3d_Layer) OpenGl_View::Layer (const Graphic3d_ZLayerId theLayerId) const
{
  Handle(Graphic3d_Layer) aLayer;
  if (theLayerId != Graphic3d_ZLayerId_UNKNOWN)
  {
    myZLayers.LayerIDs().Find (theLayerId, aLayer);
  }
  return aLayer;
}

//=======================================================================
//function : MinMaxValues
//purpose  :
//=======================================================================
Bnd_Box OpenGl_View::MinMaxValues (const Standard_Boolean theToIncludeAuxiliary) const
{
  if (!IsDefined())
  {
    return Bnd_Box();
  }

  Bnd_Box aBox = base_type::MinMaxValues (theToIncludeAuxiliary);

  return aBox;
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

//=======================================================================
//function : displayStructure
//purpose  :
//=======================================================================
void OpenGl_View::displayStructure (const Handle(Graphic3d_CStructure)& theStructure,
                                    const Standard_Integer              thePriority)
{
  const OpenGl_Structure*  aStruct = static_cast<const OpenGl_Structure*> (theStructure.get());
  const Graphic3d_ZLayerId aZLayer = aStruct->ZLayer();
  myZLayers.AddStructure (aStruct, aZLayer, thePriority);
}

//=======================================================================
//function : eraseStructure
//purpose  :
//=======================================================================
void OpenGl_View::eraseStructure (const Handle(Graphic3d_CStructure)& theStructure)
{
  const OpenGl_Structure* aStruct = static_cast<const OpenGl_Structure*> (theStructure.get());
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
  const OpenGl_Structure* aStruct = static_cast<const OpenGl_Structure*> (theStructure.get());
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
  const OpenGl_Structure* aStruct = static_cast<const OpenGl_Structure*> (theStructure.get());
  myZLayers.ChangePriority (aStruct, aLayerId, theNewPriority);
}

//=======================================================================
//function : DiagnosticInformation
//purpose  :
//=======================================================================
void OpenGl_View::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                         Graphic3d_DiagnosticInfo theFlags) const
{
  base_type::DiagnosticInformation (theDict, theFlags);
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

//=======================================================================
//function : StatisticInformation
//purpose  :
//=======================================================================
void OpenGl_View::StatisticInformation (TColStd_IndexedDataMapOfStringString& theDict) const
{
  if (const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext())
  {
    const Handle(OpenGl_FrameStats)& aStats = aCtx->FrameStats();
    const Graphic3d_RenderingParams& aRendParams = myWorkspace->View()->RenderingParams();
    aStats->FormatStats (theDict, aRendParams.CollectedStats);
  }
}

//=======================================================================
//function : StatisticInformation
//purpose  :
//=======================================================================
TCollection_AsciiString OpenGl_View::StatisticInformation() const
{
  if (const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext())
  {
    const Handle(OpenGl_FrameStats)& aStats = aCtx->FrameStats();
    const Graphic3d_RenderingParams& aRendParams = myWorkspace->View()->RenderingParams();
    return aStats->FormatStats (aRendParams.CollectedStats);
  }
  return TCollection_AsciiString();
}
