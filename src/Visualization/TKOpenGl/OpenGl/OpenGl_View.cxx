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

#include <Aspect_NeutralWindow.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Aspect_XRSession.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Image_AlienPixMap.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_BackgroundArray.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_DepthPeeling.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShadowMap.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Window.hxx>
#include <OpenGl_Workspace.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_CLocaleSentry.hxx>

#include "../Textures/Textures_EnvLUT.pxx"

namespace
{
//! Format Frame Buffer format for logging messages.
static TCollection_AsciiString printFboFormat(const occ::handle<OpenGl_FrameBuffer>& theFbo)
{
  return TCollection_AsciiString() + theFbo->GetInitVPSizeX() + "x" + theFbo->GetInitVPSizeY() + "@"
         + theFbo->NbSamples();
}

//! Return TRUE if Frame Buffer initialized has failed with the same parameters.
static bool checkWasFailedFbo(const occ::handle<OpenGl_FrameBuffer>& theFboToCheck,
                              int                                    theSizeX,
                              int                                    theSizeY,
                              int                                    theNbSamples)
{
  return !theFboToCheck->IsValid() && theFboToCheck->GetInitVPSizeX() == theSizeX
         && theFboToCheck->GetInitVPSizeY() == theSizeY
         && theFboToCheck->NbSamples() == theNbSamples;
}

//! Return TRUE if Frame Buffer initialized has failed with the same parameters.
static bool checkWasFailedFbo(const occ::handle<OpenGl_FrameBuffer>& theFboToCheck,
                              const occ::handle<OpenGl_FrameBuffer>& theFboRef)
{
  return checkWasFailedFbo(theFboToCheck,
                           theFboRef->GetVPSizeX(),
                           theFboRef->GetVPSizeY(),
                           theFboRef->NbSamples());
}

//! Chooses compatible internal color format for OIT frame buffer.
static bool chooseOitColorConfiguration(const occ::handle<OpenGl_Context>& theGlContext,
                                        const int                          theConfigIndex,
                                        NCollection_Vector<int>&           theFormats)
{
  theFormats.Clear();
  switch (theConfigIndex)
  {
    case 0: // choose best applicable color format combination
    {
      theFormats.Append(
        theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_RGBA16F : GL_RGBA32F);
      theFormats.Append(theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_R16F
                                                                                       : GL_R32F);
      return true;
    }
    case 1: // choose non-optimal applicable color format combination
    {
      theFormats.Append(
        theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_RGBA16F : GL_RGBA32F);
      theFormats.Append(
        theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_RGBA16F : GL_RGBA32F);
      return true;
    }
  }
  return false; // color combination does not exist
}
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_View, Graphic3d_CView)

//=================================================================================================

OpenGl_View::OpenGl_View(const occ::handle<Graphic3d_StructureManager>& theMgr,
                         const occ::handle<OpenGl_GraphicDriver>&       theDriver,
                         const occ::handle<OpenGl_Caps>&                theCaps,
                         OpenGl_StateCounter*                           theCounter)
    : Graphic3d_CView(theMgr),
      myDriver(theDriver.operator->()),
      myCaps(theCaps),
      myWasRedrawnGL(false),
      myToShowGradTrihedron(false),
      myStateCounter(theCounter),
      myCurrLightSourceState(theCounter->Increment()),
      myLightsRevision(0),
      myLastLightSourceState(0, 0),
      mySRgbState(-1),
      myFboColorFormat(GL_SRGB8_ALPHA8), // note that GL_SRGB8 is not required to be renderable,
                                         // unlike GL_RGB8, GL_RGBA8, GL_SRGB8_ALPHA8
      myFboDepthFormat(GL_DEPTH24_STENCIL8),
      myToFlipOutput(false),
      //
      myFrameCounter(0),
      myHasFboBlit(true),
      myToDisableOIT(false),
      myToDisableOITMSAA(false),
      myToDisableMSAA(false),
      myTransientDrawToFront(true),
      myBackBufferRestored(false),
      myIsImmediateDrawn(false),
      myTextureParams(new OpenGl_Aspects()),
      myCubeMapParams(new OpenGl_Aspects()),
      myColoredQuadParams(new OpenGl_Aspects()),
      myPBREnvState(OpenGl_PBREnvState_NONEXISTENT),
      myPBREnvRequest(false),
      // ray-tracing fields initialization
      myRaytraceInitStatus(OpenGl_RT_NONE),
      myIsRaytraceDataValid(false),
      myIsRaytraceWarnTextures(false),
      myRaytraceBVHBuilder(new BVH_BinnedBuilder<float, 3, BVH_Constants_NbBinsBest>(
        BVH_Constants_LeafNodeSizeAverage,
        BVH_Constants_MaxTreeDepth,
        false,
        OSD_Parallel::NbLogicalProcessors() + 1)),
      myRaytraceSceneRadius(0.0f),
      myRaytraceSceneEpsilon(1.0e-6f),
      myToUpdateEnvironmentMap(false),
      myRaytraceLayerListState(0),
      myPrevCameraApertureRadius(0.f),
      myPrevCameraFocalPlaneDist(0.f)
{
  for (int i = 0; i < Graphic3d_TypeOfBackground_NB; ++i)
  {
    myBackgrounds[i] = new OpenGl_BackgroundArray(Graphic3d_TypeOfBackground(i));
  }

  myWorkspace = new OpenGl_Workspace(this, nullptr);

  occ::handle<Graphic3d_CLight> aLight = new Graphic3d_CLight(Graphic3d_TypeOfLightSource_Ambient);
  aLight->SetColor(Quantity_NOC_WHITE);
  myLights         = new Graphic3d_LightSet();
  myNoShadingLight = new Graphic3d_LightSet();
  myNoShadingLight->Add(aLight);

  myMainSceneFbos[0]         = new OpenGl_FrameBuffer("fbo0_main");
  myMainSceneFbos[1]         = new OpenGl_FrameBuffer("fbo1_main");
  myMainSceneFbosOit[0]      = new OpenGl_FrameBuffer("fbo0_main_oit");
  myMainSceneFbosOit[1]      = new OpenGl_FrameBuffer("fbo1_main_oit");
  myImmediateSceneFbos[0]    = new OpenGl_FrameBuffer("fbo0_imm");
  myImmediateSceneFbos[1]    = new OpenGl_FrameBuffer("fbo1_imm");
  myImmediateSceneFbosOit[0] = new OpenGl_FrameBuffer("fbo0_imm_oit");
  myImmediateSceneFbosOit[1] = new OpenGl_FrameBuffer("fbo1_imm_oit");
  myXrSceneFbo               = new OpenGl_FrameBuffer("fbo_xr");
  myOpenGlFBO                = new OpenGl_FrameBuffer("fbo_gl");
  myOpenGlFBO2               = new OpenGl_FrameBuffer("fbo_gl2");
  myRaytraceFBO1[0]          = new OpenGl_FrameBuffer("fbo0_raytrace1");
  myRaytraceFBO1[1]          = new OpenGl_FrameBuffer("fbo1_raytrace1");
  myRaytraceFBO2[0]          = new OpenGl_FrameBuffer("fbo0_raytrace2");
  myRaytraceFBO2[1]          = new OpenGl_FrameBuffer("fbo1_raytrace2");
  myDepthPeelingFbos         = new OpenGl_DepthPeeling();
  myShadowMaps               = new OpenGl_ShadowMapArray();

  myXrSceneFbo->ColorTexture()->Sampler()->Parameters()->SetFilter(Graphic3d_TOTF_BILINEAR);
}

//=================================================================================================

OpenGl_View::~OpenGl_View()
{
  ReleaseGlResources(nullptr); // ensure ReleaseGlResources() was called within valid context
  for (int i = 0; i < Graphic3d_TypeOfBackground_NB; ++i)
  {
    OpenGl_Element::Destroy(nullptr, myBackgrounds[i]);
  }

  OpenGl_Element::Destroy(nullptr, myTextureParams);
  OpenGl_Element::Destroy(nullptr, myCubeMapParams);
  OpenGl_Element::Destroy(nullptr, myColoredQuadParams);
}

//=================================================================================================

void OpenGl_View::releaseSrgbResources(const occ::handle<OpenGl_Context>& theCtx)
{
  myRenderParams.RebuildRayTracingShaders = true;

  if (!myTextureEnv.IsNull())
  {
    if (!theCtx.IsNull())
    {
      for (OpenGl_TextureSet::Iterator aTextureIter(myTextureEnv); aTextureIter.More();
           aTextureIter.Next())
      {
        theCtx->DelayedRelease(aTextureIter.ChangeValue());
        aTextureIter.ChangeValue().Nullify();
      }
    }
    myTextureEnv.Nullify();
  }

  if (myTextureParams != nullptr)
  {
    myTextureParams->Release(theCtx.get());
  }

  if (myCubeMapParams != nullptr)
  {
    myCubeMapParams->Release(theCtx.get());
  }

  for (int i = 0; i < Graphic3d_TypeOfBackground_NB; ++i)
  {
    if (myBackgrounds[i] != nullptr)
    {
      myBackgrounds[i]->Release(theCtx.get());
    }
  }

  myMainSceneFbos[0]->Release(theCtx.get());
  myMainSceneFbos[1]->Release(theCtx.get());
  myMainSceneFbosOit[0]->Release(theCtx.get());
  myMainSceneFbosOit[1]->Release(theCtx.get());
  myImmediateSceneFbos[0]->Release(theCtx.get());
  myImmediateSceneFbos[1]->Release(theCtx.get());
  myImmediateSceneFbosOit[0]->Release(theCtx.get());
  myImmediateSceneFbosOit[1]->Release(theCtx.get());
  myXrSceneFbo->Release(theCtx.get());
  myDepthPeelingFbos->Release(theCtx.get());
  myOpenGlFBO->Release(theCtx.get());
  myOpenGlFBO2->Release(theCtx.get());
  myFullScreenQuad.Release(theCtx.get());
  myFullScreenQuadFlip.Release(theCtx.get());
  myShadowMaps->Release(theCtx.get());

  // Technically we should also re-initialize all sRGB/RGB8 color textures.
  // But for now consider this sRGB disabling/enabling to be done at application start-up
  // and re-create dynamically only frame buffers.
}

//=================================================================================================

void OpenGl_View::ReleaseGlResources(const occ::handle<OpenGl_Context>& theCtx)
{
  myGraduatedTrihedron.Release(theCtx.get());
  myFrameStatsPrs.Release(theCtx.get());

  releaseSrgbResources(theCtx);

  releaseRaytraceResources(theCtx);

  if (!myPBREnvironment.IsNull())
  {
    myPBREnvironment->Release(theCtx.get());
  }
  ReleaseXR();
}

//=================================================================================================

void OpenGl_View::Remove()
{
  if (IsRemoved())
  {
    return;
  }

  myDriver->RemoveView(this);
  myWindow.Nullify();

  Graphic3d_CView::Remove();
}

//=================================================================================================

void OpenGl_View::SetLocalOrigin(const gp_XYZ& theOrigin)
{
  myLocalOrigin                           = theOrigin;
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  if (!aCtx.IsNull())
  {
    aCtx->ShaderManager()->SetLocalOrigin(theOrigin);
  }
}

//=================================================================================================

void OpenGl_View::SetTextureEnv(const occ::handle<Graphic3d_TextureEnv>& theTextureEnv)
{
  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  if (!aCtx.IsNull() && !myTextureEnv.IsNull())
  {
    for (OpenGl_TextureSet::Iterator aTextureIter(myTextureEnv); aTextureIter.More();
         aTextureIter.Next())
    {
      aCtx->DelayedRelease(aTextureIter.ChangeValue());
      aTextureIter.ChangeValue().Nullify();
    }
  }

  myToUpdateEnvironmentMap = true;
  myTextureEnvData         = theTextureEnv;
  myTextureEnv.Nullify();
  initTextureEnv(aCtx);
}

//=================================================================================================

void OpenGl_View::initTextureEnv(const occ::handle<OpenGl_Context>& theContext)
{
  if (myTextureEnvData.IsNull() || theContext.IsNull() || !theContext->MakeCurrent())
  {
    return;
  }

  occ::handle<OpenGl_Texture> aTextureEnv =
    new OpenGl_Texture(myTextureEnvData->GetId(), myTextureEnvData->GetParams());
  aTextureEnv->Init(theContext, myTextureEnvData);

  myTextureEnv                         = new OpenGl_TextureSet(aTextureEnv);
  myTextureEnv->ChangeTextureSetBits() = Graphic3d_TextureSetBits_BaseColor;
}

//=================================================================================================

bool OpenGl_View::SetImmediateModeDrawToFront(const bool theDrawToFrontBuffer)
{
  const bool aPrevMode   = myTransientDrawToFront;
  myTransientDrawToFront = theDrawToFrontBuffer;
  return aPrevMode;
}

//=================================================================================================

occ::handle<Aspect_Window> OpenGl_View::Window() const
{
  return myWindow->SizeWindow();
}

//=================================================================================================

void OpenGl_View::SetWindow(const occ::handle<Graphic3d_CView>& theParentVIew,
                            const occ::handle<Aspect_Window>&   theWindow,
                            const Aspect_RenderingContext       theContext)
{
  if (theContext != nullptr && !theParentVIew.IsNull())
  {
    throw Standard_ProgramError("OpenGl_View::SetWindow(), internal error");
  }

  if (myParentView != nullptr)
  {
    myParentView->RemoveSubview(this);
    myParentView = nullptr;
  }

  OpenGl_View* aParentView = dynamic_cast<OpenGl_View*>(theParentVIew.get());
  if (!theParentVIew.IsNull())
  {
    if (aParentView == nullptr || aParentView->GlWindow().IsNull()
        || aParentView->GlWindow()->GetGlContext().IsNull())
    {
      throw Standard_ProgramError("OpenGl_View::SetWindow(), internal error");
    }

    myParentView = aParentView;
    myParentView->AddSubview(this);

    occ::handle<Aspect_NeutralWindow> aSubWindow = occ::down_cast<Aspect_NeutralWindow>(theWindow);
    SubviewResized(aSubWindow);

    const occ::handle<OpenGl_Window>& aParentGlWindow = aParentView->GlWindow();
    Aspect_RenderingContext aRendCtx = aParentGlWindow->GetGlContext()->RenderingContext();
    myWindow = myDriver->CreateRenderWindow(aParentGlWindow->PlatformWindow(), theWindow, aRendCtx);
  }
  else
  {
    myWindow = myDriver->CreateRenderWindow(theWindow, theWindow, theContext);
  }
  if (myWindow.IsNull())
  {
    throw Standard_ProgramError("OpenGl_View::SetWindow, Failed to create OpenGl window");
  }

  myWorkspace = new OpenGl_Workspace(this, myWindow);
  myWorldViewProjState.Reset();
  myToUpdateEnvironmentMap = true;
  myHasFboBlit             = true;
  Invalidate();

  // choose preferred FBO format
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  if (aCtx->IsWindowDeepColor() && aCtx->IsGlGreaterEqual(3, 0))
  {
    myFboColorFormat = GL_RGB10_A2;
  }
  else if (aCtx->HasSRGB())
  {
    // note that GL_SRGB8 is not required to be renderable, unlike GL_RGB8, GL_RGBA8,
    // GL_SRGB8_ALPHA8
    myFboColorFormat = GL_SRGB8_ALPHA8;
  }
  else
  {
    myFboColorFormat = GL_RGBA8;
  }

  // Environment texture resource does not support lazy initialization.
  initTextureEnv(aCtx);
}

//=================================================================================================

void OpenGl_View::Resized()
{
  base_type::Resized();
  if (!myWindow.IsNull())
  {
    myWindow->Resize();
  }
}

//=================================================================================================

static void SetMinMaxValuesCallback(Graphic3d_CView* theView)
{
  OpenGl_View* aView = dynamic_cast<OpenGl_View*>(theView);
  if (aView == nullptr)
    return;

  Bnd_Box aBox = theView->MinMaxValues();
  if (!aBox.IsVoid())
  {
    gp_Pnt aMin = aBox.CornerMin();
    gp_Pnt aMax = aBox.CornerMax();

    NCollection_Vec3<float> aMinVec((float)aMin.X(), (float)aMin.Y(), (float)aMin.Z());
    NCollection_Vec3<float> aMaxVec((float)aMax.X(), (float)aMax.Y(), (float)aMax.Z());
    aView->GraduatedTrihedronMinMaxValues(aMinVec, aMaxVec);
  }
}

//=================================================================================================

void OpenGl_View::GraduatedTrihedronDisplay(const Graphic3d_GraduatedTrihedron& theTrihedronData)
{
  myGTrihedronData = theTrihedronData;
  myGTrihedronData.SetCubicAxesCallback(SetMinMaxValuesCallback);
  myGraduatedTrihedron.SetValues(myGTrihedronData);
  myToShowGradTrihedron = true;
}

//=================================================================================================

void OpenGl_View::GraduatedTrihedronErase()
{
  myGraduatedTrihedron.Release(myWorkspace->GetGlContext().operator->());
  myToShowGradTrihedron = false;
}

//=================================================================================================

void OpenGl_View::GraduatedTrihedronMinMaxValues(const NCollection_Vec3<float> theMin,
                                                 const NCollection_Vec3<float> theMax)
{
  myGraduatedTrihedron.SetMinMax(theMin, theMax);
}

//=================================================================================================

bool OpenGl_View::BufferDump(Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType)
{
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  if (theBufferType != Graphic3d_BT_RGB_RayTraceHdrLeft)
  {
    return myWorkspace->BufferDump(myFBO, theImage, theBufferType);
  }

  if (!myRaytraceParameters.AdaptiveScreenSampling)
  {
    return myWorkspace->BufferDump(myAccumFrames % 2 ? myRaytraceFBO2[0] : myRaytraceFBO1[0],
                                   theImage,
                                   theBufferType);
  }

  if (aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
  {
    return false;
  }
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
    aValues.resize(aW * aH);
  }
  catch (const std::bad_alloc&)
  {
    return false;
  }

  aCtx->core11fwd->glBindTexture(GL_TEXTURE_RECTANGLE, myRaytraceOutputTexture[0]->TextureId());
  aCtx->core11fwd->glGetTexImage(GL_TEXTURE_RECTANGLE,
                                 0,
                                 OpenGl_TextureFormat::Create<GLfloat, 1>().Format(),
                                 GL_FLOAT,
                                 &aValues[0]);
  aCtx->core11fwd->glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  for (unsigned int aRow = 0; aRow < aH; aRow += 2)
  {
    for (unsigned int aCol = 0; aCol < aW; aCol += 3)
    {
      float* anImageValue  = theImage.ChangeValue<float[3]>((aH - aRow) / 2 - 1, aCol / 3);
      float  aInvNbSamples = 1.f / aValues[aRow * aW + aCol + aW];
      anImageValue[0]      = aValues[aRow * aW + aCol] * aInvNbSamples;
      anImageValue[1]      = aValues[aRow * aW + aCol + 1] * aInvNbSamples;
      anImageValue[2]      = aValues[aRow * aW + aCol + 1 + aW] * aInvNbSamples;
    }
  }

  return true;
}

//=================================================================================================

bool OpenGl_View::ShadowMapDump(Image_PixMap& theImage, const TCollection_AsciiString& theLightName)
{
  if (!myShadowMaps->IsValid())
  {
    return false;
  }

  const occ::handle<OpenGl_Context>& aGlCtx = myWorkspace->GetGlContext();
  for (int aShadowIter = 0; aShadowIter < myShadowMaps->Size(); ++aShadowIter)
  {
    occ::handle<OpenGl_ShadowMap>& aShadow = myShadowMaps->ChangeValue(aShadowIter);
    if (!aShadow.IsNull() && aShadow->LightSource()->Name() == theLightName)
    {
      const occ::handle<OpenGl_FrameBuffer>& aShadowFbo = aShadow->FrameBuffer();
      if (aShadowFbo->GetVPSizeX() == myRenderParams.ShadowMapResolution)
      {
        if ((int)theImage.Width() != aShadowFbo->GetVPSizeX()
            || (int)theImage.Height() != aShadowFbo->GetVPSizeY())
        {
          theImage.InitZero(Image_Format_GrayF, aShadowFbo->GetVPSizeX(), aShadowFbo->GetVPSizeY());
        }
        GLint aReadBufferPrev = GL_BACK;
        // Bind FBO if used.
        if (!aShadowFbo.IsNull() && aShadowFbo->IsValid())
        {
          aShadowFbo->BindBuffer(aGlCtx);
        }
        else if (aGlCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
        {
          aGlCtx->core11fwd->glGetIntegerv(GL_READ_BUFFER, &aReadBufferPrev);
          GLint aDrawBufferPrev = GL_BACK;
          aGlCtx->core11fwd->glGetIntegerv(GL_DRAW_BUFFER, &aDrawBufferPrev);
          aGlCtx->core11fwd->glReadBuffer(aDrawBufferPrev);
        }
        // Setup alignment.
        // clang-format off
        const GLint anAligment = std::min(GLint(theImage.MaxRowAligmentBytes()), 8); // limit to 8 bytes for OpenGL.
        // clang-format on
        aGlCtx->core11fwd->glPixelStorei(GL_PACK_ALIGNMENT, anAligment);
        // Read data.
        aGlCtx->core11fwd->glReadPixels(0,
                                        0,
                                        GLsizei(theImage.SizeX()),
                                        GLsizei(theImage.SizeY()),
                                        GL_DEPTH_COMPONENT,
                                        GL_FLOAT,
                                        theImage.ChangeData());
        aGlCtx->core11fwd->glPixelStorei(GL_PACK_ALIGNMENT, 1);
        if (aGlCtx->hasPackRowLength)
        {
          aGlCtx->core11fwd->glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        }
        // Unbind FBO.
        if (!aShadowFbo.IsNull() && aShadowFbo->IsValid())
        {
          aShadowFbo->UnbindBuffer(aGlCtx);
        }
        else if (aGlCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
        {
          aGlCtx->core11fwd->glReadBuffer(aReadBufferPrev);
        }
        // Check for errors.
        const bool hasErrors = aGlCtx->ResetErrors(true);
        return !hasErrors;
      }
    }
  }
  return false;
}

//=================================================================================================

Aspect_GradientBackground OpenGl_View::GradientBackground() const
{
  Quantity_Color aColor1, aColor2;
  aColor1.SetValues(myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor(0).r(),
                    myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor(0).g(),
                    myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor(0).b(),
                    Quantity_TOC_RGB);
  aColor2.SetValues(myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor(1).r(),
                    myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor(1).g(),
                    myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientColor(1).b(),
                    Quantity_TOC_RGB);
  return Aspect_GradientBackground(aColor1,
                                   aColor2,
                                   myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientFillMethod());
}

//=================================================================================================

void OpenGl_View::SetGradientBackground(const Aspect_GradientBackground& theBackground)
{
  Quantity_Color aColor1, aColor2;
  theBackground.Colors(aColor1, aColor2);
  myBackgrounds[Graphic3d_TOB_GRADIENT]->SetGradientParameters(
    aColor1,
    aColor2,
    theBackground.BgGradientFillMethod());
  if (theBackground.BgGradientFillMethod() >= Aspect_GradientFillMethod_Corner1
      && theBackground.BgGradientFillMethod() <= Aspect_GradientFillMethod_Corner4)
  {
    if (const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext())
    {
      myColoredQuadParams->Aspect()->SetShaderProgram(
        aCtx->ShaderManager()->GetColoredQuadProgram());
      myColoredQuadParams->Aspect()->ShaderProgram()->PushVariableVec3("uColor1", aColor1.Rgb());
      myColoredQuadParams->Aspect()->ShaderProgram()->PushVariableVec3("uColor2", aColor2.Rgb());
    }
  }
  myBackgroundType = Graphic3d_TOB_GRADIENT;
}

//=================================================================================================

void OpenGl_View::SetBackgroundImage(const occ::handle<Graphic3d_TextureMap>& theTextureMap,
                                     bool                                     theToUpdatePBREnv)
{
  occ::handle<Graphic3d_TextureMap> aNewMap = theTextureMap;
  if (theTextureMap.IsNull() || !theTextureMap->IsDone())
  {
    if (!theTextureMap.IsNull())
    {
      Message::SendFail("Error: unable to set image background");
    }
    aNewMap.Nullify();
  }

  occ::handle<Graphic3d_CubeMap> aCubeMap = occ::down_cast<Graphic3d_CubeMap>(aNewMap);
  if (theToUpdatePBREnv)
  {
    // update PBR environment
    const TCollection_AsciiString anIdOld =
      !myCubeMapIBL.IsNull() ? myCubeMapIBL->GetId() : TCollection_AsciiString();
    const TCollection_AsciiString anIdNew =
      !aCubeMap.IsNull() ? aCubeMap->GetId() : TCollection_AsciiString();
    if (anIdNew != anIdOld)
    {
      myPBREnvRequest = true;
    }
    myCubeMapIBL = aCubeMap;
  }

  if (aNewMap.IsNull())
  {
    if (myBackgroundType == Graphic3d_TOB_TEXTURE || myBackgroundType == Graphic3d_TOB_CUBEMAP)
    {
      myBackgroundType = Graphic3d_TOB_NONE;
    }
    return;
  }

  occ::handle<Graphic3d_AspectFillArea3d> anAspect    = new Graphic3d_AspectFillArea3d();
  occ::handle<Graphic3d_TextureSet>       aTextureSet = new Graphic3d_TextureSet(aNewMap);
  anAspect->SetInteriorStyle(Aspect_IS_SOLID);
  anAspect->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_DoubleSided);
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  anAspect->SetTextureSet(aTextureSet);
  anAspect->SetTextureMapOn(true);

  if (occ::handle<Graphic3d_Texture2D> aTextureMap = occ::down_cast<Graphic3d_Texture2D>(aNewMap))
  {
    myTextureParams->SetAspect(anAspect);
    myBackgroundType  = Graphic3d_TOB_TEXTURE;
    myBackgroundImage = aTextureMap;
    return;
  }

  if (!aCubeMap.IsNull())
  {
    aCubeMap->SetMipmapsGeneration(true);
    if (const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext())
    {
      anAspect->SetShaderProgram(aCtx->ShaderManager()->GetBgCubeMapProgram());
    }

    myCubeMapParams->SetAspect(anAspect);

    const OpenGl_Aspects* anAspectsBackup = myWorkspace->SetAspects(myCubeMapParams);
    myWorkspace->ApplyAspects();
    myWorkspace->SetAspects(anAspectsBackup);
    myWorkspace->ApplyAspects();

    myBackgroundType    = Graphic3d_TOB_CUBEMAP;
    myCubeMapBackground = aCubeMap;
    return;
  }

  throw Standard_ProgramError(
    "OpenGl_View::SetBackgroundImage() - invalid texture map set for background");
}

//=================================================================================================

void OpenGl_View::SetImageBasedLighting(bool theToEnableIBL)
{
  if (!theToEnableIBL || myBackgroundType != Graphic3d_TOB_CUBEMAP)
  {
    if (!myCubeMapIBL.IsNull())
    {
      myPBREnvRequest = true;
      myCubeMapIBL.Nullify();
    }
    return;
  }

  const TCollection_AsciiString anIdOld =
    !myCubeMapIBL.IsNull() ? myCubeMapIBL->GetId() : TCollection_AsciiString();
  const TCollection_AsciiString anIdNew =
    !myCubeMapBackground.IsNull() ? myCubeMapBackground->GetId() : TCollection_AsciiString();
  if (anIdNew != anIdOld)
  {
    myPBREnvRequest = true;
  }
  myCubeMapIBL = myCubeMapBackground;
}

//=================================================================================================

Aspect_FillMethod OpenGl_View::BackgroundImageStyle() const
{
  return myBackgrounds[Graphic3d_TOB_TEXTURE]->TextureFillMethod();
}

//=================================================================================================

void OpenGl_View::SetBackgroundImageStyle(const Aspect_FillMethod theFillStyle)
{
  myBackgrounds[Graphic3d_TOB_TEXTURE]->SetTextureFillMethod(theFillStyle);
}

//=================================================================================================

unsigned int OpenGl_View::SpecIBLMapLevels() const
{
  return myPBREnvironment.IsNull() ? 0 : myPBREnvironment->SpecMapLevelsNumber();
}

//=================================================================================================

void OpenGl_View::InsertLayerBefore(const Graphic3d_ZLayerId        theLayerId,
                                    const Graphic3d_ZLayerSettings& theSettings,
                                    const Graphic3d_ZLayerId        theLayerAfter)
{
  myZLayers.InsertLayerBefore(theLayerId, theSettings, theLayerAfter);
}

//=================================================================================================

void OpenGl_View::InsertLayerAfter(const Graphic3d_ZLayerId        theLayerId,
                                   const Graphic3d_ZLayerSettings& theSettings,
                                   const Graphic3d_ZLayerId        theLayerBefore)
{
  myZLayers.InsertLayerAfter(theLayerId, theSettings, theLayerBefore);
}

//=================================================================================================

void OpenGl_View::RemoveZLayer(const Graphic3d_ZLayerId theLayerId)
{
  myZLayers.RemoveLayer(theLayerId);
}

//=================================================================================================

void OpenGl_View::SetZLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                                    const Graphic3d_ZLayerSettings& theSettings)
{
  myZLayers.SetLayerSettings(theLayerId, theSettings);
}

//=================================================================================================

int OpenGl_View::ZLayerMax() const
{
  int aLayerMax = Graphic3d_ZLayerId_Default;
  for (NCollection_List<occ::handle<Graphic3d_Layer>>::Iterator aLayerIter(myZLayers.Layers());
       aLayerIter.More();
       aLayerIter.Next())
  {
    aLayerMax = std::max(aLayerMax, aLayerIter.Value()->LayerId());
  }
  return aLayerMax;
}

//=================================================================================================

const NCollection_List<occ::handle<Graphic3d_Layer>>& OpenGl_View::Layers() const
{
  return myZLayers.Layers();
}

//=================================================================================================

occ::handle<Graphic3d_Layer> OpenGl_View::Layer(const Graphic3d_ZLayerId theLayerId) const
{
  occ::handle<Graphic3d_Layer> aLayer;
  if (theLayerId != Graphic3d_ZLayerId_UNKNOWN)
  {
    myZLayers.LayerIDs().Find(theLayerId, aLayer);
  }
  return aLayer;
}

//=================================================================================================

Bnd_Box OpenGl_View::MinMaxValues(const bool theToIncludeAuxiliary) const
{
  if (!IsDefined())
  {
    return Bnd_Box();
  }

  Bnd_Box aBox = base_type::MinMaxValues(theToIncludeAuxiliary);

  // make sure that stats overlay isn't clamped on hardware with unavailable depth clamping
  if (theToIncludeAuxiliary && myRenderParams.ToShowStats
      && !myWorkspace->GetGlContext()->arbDepthClamp)
  {
    Bnd_Box aStatsBox(gp_Pnt(float(myWindow->Width() / 2.0), float(myWindow->Height() / 2.0), 0.0),
                      gp_Pnt(float(myWindow->Width() / 2.0), float(myWindow->Height() / 2.0), 0.0));
    myRenderParams.StatsPosition->Apply(myCamera,
                                        myCamera->ProjectionMatrix(),
                                        myCamera->OrientationMatrix(),
                                        myWindow->Width(),
                                        myWindow->Height(),
                                        aStatsBox);
    aBox.Add(aStatsBox);
  }
  return aBox;
}

//=================================================================================================

occ::handle<Standard_Transient> OpenGl_View::FBO() const
{
  return occ::handle<Standard_Transient>(myFBO);
}

//=================================================================================================

void OpenGl_View::SetFBO(const occ::handle<Standard_Transient>& theFbo)
{
  myFBO = occ::down_cast<OpenGl_FrameBuffer>(theFbo);
}

//=================================================================================================

occ::handle<Standard_Transient> OpenGl_View::FBOCreate(const int theWidth, const int theHeight)
{
  return myWorkspace->FBOCreate(theWidth, theHeight);
}

//=================================================================================================

void OpenGl_View::FBORelease(occ::handle<Standard_Transient>& theFbo)
{
  occ::handle<OpenGl_FrameBuffer> aFrameBuffer = occ::down_cast<OpenGl_FrameBuffer>(theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  myWorkspace->FBORelease(aFrameBuffer);
  theFbo.Nullify();
}

//=================================================================================================

void OpenGl_View::FBOGetDimensions(const occ::handle<Standard_Transient>& theFbo,
                                   int&                                   theWidth,
                                   int&                                   theHeight,
                                   int&                                   theWidthMax,
                                   int&                                   theHeightMax)
{
  const occ::handle<OpenGl_FrameBuffer> aFrameBuffer = occ::down_cast<OpenGl_FrameBuffer>(theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  theWidth     = aFrameBuffer->GetVPSizeX(); // current viewport size
  theHeight    = aFrameBuffer->GetVPSizeY();
  theWidthMax  = aFrameBuffer->GetSizeX(); // texture size
  theHeightMax = aFrameBuffer->GetSizeY();
}

//=================================================================================================

void OpenGl_View::FBOChangeViewport(const occ::handle<Standard_Transient>& theFbo,
                                    const int                              theWidth,
                                    const int                              theHeight)
{
  const occ::handle<OpenGl_FrameBuffer> aFrameBuffer = occ::down_cast<OpenGl_FrameBuffer>(theFbo);
  if (aFrameBuffer.IsNull())
  {
    return;
  }

  aFrameBuffer->ChangeViewport(theWidth, theHeight);
}

//=================================================================================================

void OpenGl_View::displayStructure(const occ::handle<Graphic3d_CStructure>& theStructure,
                                   const Graphic3d_DisplayPriority          thePriority)
{
  const OpenGl_Structure*  aStruct = static_cast<const OpenGl_Structure*>(theStructure.get());
  const Graphic3d_ZLayerId aZLayer = aStruct->ZLayer();
  myZLayers.AddStructure(aStruct, aZLayer, thePriority);
}

//=================================================================================================

void OpenGl_View::eraseStructure(const occ::handle<Graphic3d_CStructure>& theStructure)
{
  const OpenGl_Structure* aStruct = static_cast<const OpenGl_Structure*>(theStructure.get());
  myZLayers.RemoveStructure(aStruct);
}

//=================================================================================================

void OpenGl_View::changeZLayer(const occ::handle<Graphic3d_CStructure>& theStructure,
                               const Graphic3d_ZLayerId                 theNewLayerId)
{
  const Graphic3d_ZLayerId anOldLayer = theStructure->ZLayer();
  const OpenGl_Structure*  aStruct    = static_cast<const OpenGl_Structure*>(theStructure.get());
  myZLayers.ChangeLayer(aStruct, anOldLayer, theNewLayerId);
  Update(anOldLayer);
  Update(theNewLayerId);
}

//=================================================================================================

void OpenGl_View::changePriority(const occ::handle<Graphic3d_CStructure>& theStructure,
                                 const Graphic3d_DisplayPriority          theNewPriority)
{
  const Graphic3d_ZLayerId aLayerId = theStructure->ZLayer();
  const OpenGl_Structure*  aStruct  = static_cast<const OpenGl_Structure*>(theStructure.get());
  myZLayers.ChangePriority(aStruct, aLayerId, theNewPriority);
}

//=================================================================================================

void OpenGl_View::DiagnosticInformation(
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict,
  Graphic3d_DiagnosticInfo                                                      theFlags) const
{
  base_type::DiagnosticInformation(theDict, theFlags);
  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  if (!myWorkspace->Activate() || aCtx.IsNull())
  {
    return;
  }

  aCtx->DiagnosticInformation(theDict, theFlags);
  if ((theFlags & Graphic3d_DiagnosticInfo_FrameBuffer) != 0)
  {
    TCollection_AsciiString aResRatio(myRenderParams.ResolutionRatio());
    theDict.ChangeFromIndex(theDict.Add("ResolutionRatio", aResRatio)) = aResRatio;
    if (myMainSceneFbos[0]->IsValid())
    {
      TCollection_AsciiString anFboInfo;
      if (const occ::handle<OpenGl_Texture>& aColorTex = myMainSceneFbos[0]->ColorTexture())
      {
        anFboInfo += OpenGl_TextureFormat::FormatFormat(aColorTex->SizedFormat());
      }
      if (const occ::handle<OpenGl_Texture>& aDepthTex = myMainSceneFbos[0]->DepthStencilTexture())
      {
        anFboInfo = anFboInfo + " " + OpenGl_TextureFormat::FormatFormat(aDepthTex->SizedFormat());
      }
      theDict.ChangeFromIndex(theDict.Add("FBO buffer", anFboInfo)) = anFboInfo;
    }
  }
}

//=================================================================================================

void OpenGl_View::StatisticInformation(
  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict) const
{
  if (const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext())
  {
    const occ::handle<OpenGl_FrameStats>& aStats      = aCtx->FrameStats();
    const Graphic3d_RenderingParams&      aRendParams = myWorkspace->View()->RenderingParams();
    aStats->FormatStats(theDict, aRendParams.CollectedStats);
  }
}

//=================================================================================================

TCollection_AsciiString OpenGl_View::StatisticInformation() const
{
  if (const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext())
  {
    const occ::handle<OpenGl_FrameStats>& aStats      = aCtx->FrameStats();
    const Graphic3d_RenderingParams&      aRendParams = myWorkspace->View()->RenderingParams();
    return aStats->FormatStats(aRendParams.CollectedStats);
  }
  return TCollection_AsciiString();
}

//=================================================================================================

void OpenGl_View::drawBackground(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                 Graphic3d_Camera::Projection         theProjection)
{
  const occ::handle<OpenGl_Context>& aCtx           = theWorkspace->GetGlContext();
  const bool                         wasUsedZBuffer = theWorkspace->SetUseZBuffer(false);
  if (wasUsedZBuffer)
  {
    aCtx->core11fwd->glDisable(GL_DEPTH_TEST);
  }

#ifdef GL_DEPTH_CLAMP
  const bool wasDepthClamped = aCtx->arbDepthClamp && aCtx->core11fwd->glIsEnabled(GL_DEPTH_CLAMP);
  if (aCtx->arbDepthClamp && !wasDepthClamped)
  {
    // make sure background is always drawn (workaround skybox rendering on some hardware)
    aCtx->core11fwd->glEnable(GL_DEPTH_CLAMP);
  }
#endif

  if (myBackgroundType == Graphic3d_TOB_CUBEMAP)
  {
    updateSkydomeBg(aCtx);
    if (!myCubeMapParams->Aspect()->ShaderProgram().IsNull())
    {
      myCubeMapParams->Aspect()->ShaderProgram()->PushVariableInt(
        "uZCoeff",
        myCubeMapBackground->ZIsInverted() ? -1 : 1);
      myCubeMapParams->Aspect()->ShaderProgram()->PushVariableInt(
        "uYCoeff",
        myCubeMapBackground->IsTopDown() ? 1 : -1);
      const OpenGl_Aspects* anOldAspectFace = theWorkspace->SetAspects(myCubeMapParams);

      myBackgrounds[Graphic3d_TOB_CUBEMAP]->Render(theWorkspace, theProjection);

      theWorkspace->SetAspects(anOldAspectFace);
    }
  }
  else if (myBackgroundType == Graphic3d_TOB_GRADIENT || myBackgroundType == Graphic3d_TOB_TEXTURE)
  {
    // Drawing background gradient if:
    // - gradient fill type is not Aspect_GradientFillMethod_None and
    // - either background texture is no specified or it is drawn in Aspect_FM_CENTERED mode
    if (myBackgrounds[Graphic3d_TOB_GRADIENT]->IsDefined()
        && (!myTextureParams->Aspect()->ToMapTexture()
            || myBackgrounds[Graphic3d_TOB_TEXTURE]->TextureFillMethod() == Aspect_FM_CENTERED
            || myBackgrounds[Graphic3d_TOB_TEXTURE]->TextureFillMethod() == Aspect_FM_NONE))
    {
      if (myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientFillMethod()
            >= Aspect_GradientFillMethod_Corner1
          && myBackgrounds[Graphic3d_TOB_GRADIENT]->GradientFillMethod()
               <= Aspect_GradientFillMethod_Corner4)
      {
        const OpenGl_Aspects* anOldAspectFace = theWorkspace->SetAspects(myColoredQuadParams);

        myBackgrounds[Graphic3d_TOB_GRADIENT]->Render(theWorkspace, theProjection);

        theWorkspace->SetAspects(anOldAspectFace);
      }
      else
      {
        myBackgrounds[Graphic3d_TOB_GRADIENT]->Render(theWorkspace, theProjection);
      }
    }

    // Drawing background image if it is defined
    // (texture is defined and fill type is not Aspect_FM_NONE)
    if (myBackgrounds[Graphic3d_TOB_TEXTURE]->IsDefined()
        && myTextureParams->Aspect()->ToMapTexture())
    {
      aCtx->core11fwd->glDisable(GL_BLEND);

      const OpenGl_Aspects* anOldAspectFace = theWorkspace->SetAspects(myTextureParams);
      myBackgrounds[Graphic3d_TOB_TEXTURE]->Render(theWorkspace, theProjection);
      theWorkspace->SetAspects(anOldAspectFace);
    }
  }

  if (wasUsedZBuffer)
  {
    theWorkspace->SetUseZBuffer(true);
    aCtx->core11fwd->glEnable(GL_DEPTH_TEST);
  }
#ifdef GL_DEPTH_CLAMP
  if (aCtx->arbDepthClamp && !wasDepthClamped)
  {
    aCtx->core11fwd->glDisable(GL_DEPTH_CLAMP);
  }
#endif
}

//=================================================================================================

bool OpenGl_View::prepareFrameBuffers(Graphic3d_Camera::Projection& theProj)
{
  theProj                                 = myCamera->ProjectionType();
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();

  int                 aSizeX = 0, aSizeY = 0;
  OpenGl_FrameBuffer* aFrameBuffer = myFBO.get();
  if (aFrameBuffer != nullptr)
  {
    aSizeX = aFrameBuffer->GetVPSizeX();
    aSizeY = aFrameBuffer->GetVPSizeY();
  }
  else if (IsActiveXR())
  {
    aSizeX = myXRSession->RecommendedViewport().x();
    aSizeY = myXRSession->RecommendedViewport().y();
  }
  else
  {
    aSizeX = myWindow->Width();
    aSizeY = myWindow->Height();
  }

  const NCollection_Vec2<int> aRendSize(int(myRenderParams.RenderResolutionScale * aSizeX + 0.5f),
                                        int(myRenderParams.RenderResolutionScale * aSizeY + 0.5f));
  if (aSizeX < 1 || aSizeY < 1 || aRendSize.x() < 1 || aRendSize.y() < 1)
  {
    myBackBufferRestored = false;
    myIsImmediateDrawn   = false;
    return false;
  }

  // determine multisampling parameters
  int aNbSamples = !myToDisableMSAA && aSizeX == aRendSize.x()
                     ? std::max(std::min(myRenderParams.NbMsaaSamples, aCtx->MaxMsaaSamples()), 0)
                     : 0;
  if (aNbSamples != 0)
  {
    aNbSamples = OpenGl_Context::GetPowerOfTwo(aNbSamples, aCtx->MaxMsaaSamples());
  }
  // Only MSAA textures can be blit into MSAA target,
  // while render buffers could be resolved only into non-MSAA targets.
  // As result, within obsolete OpenGL ES 3.0 context, we may create only one MSAA render buffer for
  // main scene content and blit it into non-MSAA immediate FBO.
  const bool hasTextureMsaa = aCtx->HasTextureMultisampling();

  bool toUseOit = myRenderParams.TransparencyMethod != Graphic3d_RTM_BLEND_UNORDERED
                  && !myIsSubviewComposer && checkOitCompatibility(aCtx, aNbSamples > 0);

  const bool toInitImmediateFbo =
    myTransientDrawToFront && !myIsSubviewComposer
    && (!aCtx->caps->useSystemBuffer || (toUseOit && HasImmediateStructures()));

  if (aFrameBuffer == nullptr && !aCtx->DefaultFrameBuffer().IsNull()
      && aCtx->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = aCtx->DefaultFrameBuffer().operator->();
  }

  if (myHasFboBlit
      && (myTransientDrawToFront || theProj == Graphic3d_Camera::Projection_Stereo
          || aNbSamples != 0 || toUseOit || aSizeX != aRendSize.x()))
  {
    if (myMainSceneFbos[0]->GetVPSize() != aRendSize
        || myMainSceneFbos[0]->NbSamples() != aNbSamples)
    {
      if (!myTransientDrawToFront)
      {
        myImmediateSceneFbos[0]->Release(aCtx.operator->());
        myImmediateSceneFbos[1]->Release(aCtx.operator->());
        myImmediateSceneFbos[0]->ChangeViewport(0, 0);
        myImmediateSceneFbos[1]->ChangeViewport(0, 0);
      }

      // prepare FBOs containing main scene
      // for further blitting and rendering immediate presentations on top
      if (aCtx->core20fwd != nullptr)
      {
        const bool wasFailedMain0 =
          checkWasFailedFbo(myMainSceneFbos[0], aRendSize.x(), aRendSize.y(), aNbSamples);
        if (!myMainSceneFbos[0]
               ->Init(aCtx, aRendSize, myFboColorFormat, myFboDepthFormat, aNbSamples)
            && !wasFailedMain0)
        {
          TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Main FBO "
                                            + printFboFormat(myMainSceneFbos[0])
                                            + " initialization has failed";
          aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            aMsg);
        }
      }
    }

    if (myMainSceneFbos[0]->IsValid() && (toInitImmediateFbo || myImmediateSceneFbos[0]->IsValid()))
    {
      const bool wasFailedImm0 = checkWasFailedFbo(myImmediateSceneFbos[0], myMainSceneFbos[0]);
      if (!myImmediateSceneFbos[0]->InitLazy(aCtx, *myMainSceneFbos[0], hasTextureMsaa)
          && !wasFailedImm0)
      {
        TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Immediate FBO "
                                          + printFboFormat(myImmediateSceneFbos[0])
                                          + " initialization has failed";
        aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          aMsg);
      }
    }
  }
  else
  {
    myMainSceneFbos[0]->Release(aCtx.operator->());
    myMainSceneFbos[1]->Release(aCtx.operator->());
    myImmediateSceneFbos[0]->Release(aCtx.operator->());
    myImmediateSceneFbos[1]->Release(aCtx.operator->());
    myXrSceneFbo->Release(aCtx.operator->());
    myMainSceneFbos[0]->ChangeViewport(0, 0);
    myMainSceneFbos[1]->ChangeViewport(0, 0);
    myImmediateSceneFbos[0]->ChangeViewport(0, 0);
    myImmediateSceneFbos[1]->ChangeViewport(0, 0);
    myXrSceneFbo->ChangeViewport(0, 0);
  }

  bool hasXRBlitFbo = false;
  if (theProj == Graphic3d_Camera::Projection_Stereo && IsActiveXR()
      && myMainSceneFbos[0]->IsValid())
  {
    if (aNbSamples != 0 || aSizeX != aRendSize.x())
    {
      hasXRBlitFbo = myXrSceneFbo->InitLazy(aCtx,
                                            NCollection_Vec2<int>(aSizeX, aSizeY),
                                            myFboColorFormat,
                                            myFboDepthFormat,
                                            0);
      if (!hasXRBlitFbo)
      {
        TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! VR FBO "
                                          + printFboFormat(myXrSceneFbo)
                                          + " initialization has failed";
        aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          aMsg);
      }
    }
  }
  else if (theProj == Graphic3d_Camera::Projection_Stereo && myMainSceneFbos[0]->IsValid())
  {
    const bool wasFailedMain1 = checkWasFailedFbo(myMainSceneFbos[1], myMainSceneFbos[0]);
    if (!myMainSceneFbos[1]->InitLazy(aCtx, *myMainSceneFbos[0], true) && !wasFailedMain1)
    {
      TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Main FBO (second) "
                                        + printFboFormat(myMainSceneFbos[1])
                                        + " initialization has failed";
      aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        aMsg);
    }
    if (!myMainSceneFbos[1]->IsValid())
    {
      // no enough memory?
      theProj = Graphic3d_Camera::Projection_Perspective;
    }
    else if (!myTransientDrawToFront)
    {
      //
    }
    else if (!aCtx->HasStereoBuffers()
             || myRenderParams.StereoMode != Graphic3d_StereoMode_QuadBuffer)
    {
      const bool wasFailedImm0 = checkWasFailedFbo(myImmediateSceneFbos[0], myMainSceneFbos[0]);
      const bool wasFailedImm1 = checkWasFailedFbo(myImmediateSceneFbos[1], myMainSceneFbos[0]);
      if (!myImmediateSceneFbos[0]->InitLazy(aCtx, *myMainSceneFbos[0], hasTextureMsaa)
          && !wasFailedImm0)
      {
        TCollection_ExtendedString aMsg =
          TCollection_ExtendedString() + "Error! Immediate FBO (first) "
          + printFboFormat(myImmediateSceneFbos[0]) + " initialization has failed";
        aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          aMsg);
      }
      if (!myImmediateSceneFbos[1]->InitLazy(aCtx, *myMainSceneFbos[0], hasTextureMsaa)
          && !wasFailedImm1)
      {
        TCollection_ExtendedString aMsg =
          TCollection_ExtendedString() + "Error! Immediate FBO (first) "
          + printFboFormat(myImmediateSceneFbos[1]) + " initialization has failed";
        aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          aMsg);
      }
      if (!myImmediateSceneFbos[0]->IsValid() || !myImmediateSceneFbos[1]->IsValid())
      {
        theProj = Graphic3d_Camera::Projection_Perspective;
      }
    }
  }
  if (!hasXRBlitFbo)
  {
    myXrSceneFbo->Release(aCtx.get());
    myXrSceneFbo->ChangeViewport(0, 0);
  }

  // process PBR environment
  if (myRenderParams.ShadingModel == Graphic3d_TypeOfShadingModel_Pbr
      || myRenderParams.ShadingModel == Graphic3d_TypeOfShadingModel_PbrFacet)
  {
    if (!myPBREnvironment.IsNull()
        && myPBREnvironment->SizesAreDifferent(myRenderParams.PbrEnvPow2Size,
                                               myRenderParams.PbrEnvSpecMapNbLevels))
    {
      myPBREnvironment->Release(aCtx.get());
      myPBREnvironment.Nullify();
      myPBREnvState   = OpenGl_PBREnvState_NONEXISTENT;
      myPBREnvRequest = true;
      ++myLightsRevision;
    }

    if (myPBREnvState == OpenGl_PBREnvState_NONEXISTENT && aCtx->HasPBR())
    {
      myPBREnvironment = OpenGl_PBREnvironment::Create(aCtx,
                                                       myRenderParams.PbrEnvPow2Size,
                                                       myRenderParams.PbrEnvSpecMapNbLevels);
      myPBREnvState =
        myPBREnvironment.IsNull() ? OpenGl_PBREnvState_UNAVAILABLE : OpenGl_PBREnvState_CREATED;
      if (myPBREnvState == OpenGl_PBREnvState_CREATED)
      {
        occ::handle<OpenGl_Texture>          anEnvLUT;
        static const TCollection_AsciiString THE_SHARED_ENV_LUT_KEY("EnvLUT");
        if (!aCtx->GetResource(THE_SHARED_ENV_LUT_KEY, anEnvLUT))
        {
          bool toConvertHalfFloat = false;

          // GL_RG32F is not texture-filterable format in OpenGL ES without OES_texture_float_linear
          // extension. GL_RG16F is texture-filterable since OpenGL ES 3.0 or OpenGL ES 2.0 +
          // OES_texture_half_float_linear. OpenGL ES 3.0 allows initialization of GL_RG16F from
          // 32-bit float data, but OpenGL ES 2.0 + OES_texture_half_float does not. Note that it is
          // expected that GL_RG16F has enough precision for this table, so that it can be used also
          // on desktop OpenGL.
          const bool hasHalfFloat =
            aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
            && (aCtx->IsGlGreaterEqual(3, 0)
                || aCtx->CheckExtension("GL_OES_texture_half_float_linear"));
          if (aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
          {
            toConvertHalfFloat = !aCtx->IsGlGreaterEqual(3, 0) && hasHalfFloat;
          }

          Image_Format anImgFormat = Image_Format_UNKNOWN;
          if (aCtx->arbTexRG)
          {
            anImgFormat = toConvertHalfFloat ? Image_Format_RGF_half : Image_Format_RGF;
          }
          else
          {
            anImgFormat = toConvertHalfFloat ? Image_Format_RGBAF_half : Image_Format_RGBAF;
          }

          occ::handle<Image_PixMap> aPixMap = new Image_PixMap();
          if (anImgFormat == Image_Format_RGF)
          {
            aPixMap->InitWrapper(Image_Format_RGF,
                                 (uint8_t*)Textures_EnvLUT,
                                 Textures_EnvLUTSize,
                                 Textures_EnvLUTSize);
          }
          else
          {
            aPixMap->InitZero(anImgFormat, Textures_EnvLUTSize, Textures_EnvLUTSize);
            Image_PixMap aPixMapRG;
            aPixMapRG.InitWrapper(Image_Format_RGF,
                                  (uint8_t*)Textures_EnvLUT,
                                  Textures_EnvLUTSize,
                                  Textures_EnvLUTSize);
            for (size_t aRowIter = 0; aRowIter < aPixMapRG.SizeY(); ++aRowIter)
            {
              for (size_t aColIter = 0; aColIter < aPixMapRG.SizeX(); ++aColIter)
              {
                const Image_ColorRGF& aPixelRG =
                  aPixMapRG.Value<Image_ColorRGF>(aRowIter, aColIter);
                if (toConvertHalfFloat)
                {
                  NCollection_Vec2<uint16_t>& aPixelRGBA =
                    aPixMap->ChangeValue<NCollection_Vec2<uint16_t>>(aRowIter, aColIter);
                  aPixelRGBA.x() = Image_PixMap::ConvertToHalfFloat(aPixelRG.r());
                  aPixelRGBA.y() = Image_PixMap::ConvertToHalfFloat(aPixelRG.g());
                }
                else
                {
                  Image_ColorRGBAF& aPixelRGBA =
                    aPixMap->ChangeValue<Image_ColorRGBAF>(aRowIter, aColIter);
                  aPixelRGBA.r() = aPixelRG.r();
                  aPixelRGBA.g() = aPixelRG.g();
                }
              }
            }
          }

          OpenGl_TextureFormat aTexFormat =
            OpenGl_TextureFormat::FindFormat(aCtx, aPixMap->Format(), false);
          if (aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES && aTexFormat.IsValid()
              && hasHalfFloat)
          {
            aTexFormat.SetInternalFormat(aCtx->arbTexRG ? GL_RG16F : GL_RGBA16F);
          }

          occ::handle<Graphic3d_TextureParams> aParams = new Graphic3d_TextureParams();
          aParams->SetFilter(Graphic3d_TOTF_BILINEAR);
          aParams->SetRepeat(false);
          aParams->SetTextureUnit(aCtx->PBREnvLUTTexUnit());
          anEnvLUT = new OpenGl_Texture(THE_SHARED_ENV_LUT_KEY, aParams);
          if (!aTexFormat.IsValid()
              || !anEnvLUT->Init(aCtx,
                                 aTexFormat,
                                 NCollection_Vec2<int>((int)Textures_EnvLUTSize),
                                 Graphic3d_TypeOfTexture_2D,
                                 aPixMap.get()))
          {
            aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                              GL_DEBUG_TYPE_ERROR,
                              0,
                              GL_DEBUG_SEVERITY_HIGH,
                              "Failed allocation of LUT for PBR");
            anEnvLUT.Nullify();
          }
          aCtx->ShareResource(THE_SHARED_ENV_LUT_KEY, anEnvLUT);
        }
        if (!anEnvLUT.IsNull())
        {
          anEnvLUT->Bind(aCtx);
        }
        myWorkspace->ApplyAspects();
      }
    }
    updatePBREnvironment(aCtx);
  }

  // create color and coverage accumulation buffers required for OIT algorithm
  if (toUseOit && myRenderParams.TransparencyMethod == Graphic3d_RTM_DEPTH_PEELING_OIT)
  {
    if (myDepthPeelingFbos->BlendBackFboOit()->GetSize() != aRendSize)
    {
      if (myDepthPeelingFbos->BlendBackFboOit()->Init(aCtx, aRendSize, GL_RGBA16F, 0))
      {
        for (int aPairIter = 0; aPairIter < 2; ++aPairIter)
        {
          NCollection_Vector<int> aColorFormats;
          aColorFormats.Append(GL_RG32F);
          aColorFormats.Append(GL_RGBA16F);
          aColorFormats.Append(GL_RGBA16F);
          myDepthPeelingFbos->DepthPeelFbosOit()[aPairIter]->Init(aCtx,
                                                                  aRendSize,
                                                                  aColorFormats,
                                                                  0);

          NCollection_Sequence<occ::handle<OpenGl_Texture>> anAttachments;
          anAttachments.Append(myDepthPeelingFbos->DepthPeelFbosOit()[aPairIter]->ColorTexture(1));
          anAttachments.Append(myDepthPeelingFbos->DepthPeelFbosOit()[aPairIter]->ColorTexture(2));
          myDepthPeelingFbos->FrontBackColorFbosOit()[aPairIter]->InitWrapper(aCtx, anAttachments);
        }
      }
      else
      {
        toUseOit = false;
        aCtx->PushMessage(
          GL_DEBUG_SOURCE_APPLICATION,
          GL_DEBUG_TYPE_ERROR,
          0,
          GL_DEBUG_SEVERITY_HIGH,
          "Initialization of float texture framebuffer for use with\n"
          "  Depth-Peeling order-independent transparency rendering algorithm has failed.");
      }
    }
  }
  if (!toUseOit)
  {
    myDepthPeelingFbos->Release(aCtx.operator->());
  }

  if (toUseOit && myRenderParams.TransparencyMethod == Graphic3d_RTM_BLEND_OIT)
  {
    int anFboIt = 0;
    for (; anFboIt < 2; ++anFboIt)
    {
      occ::handle<OpenGl_FrameBuffer>& aMainSceneFbo          = myMainSceneFbos[anFboIt];
      occ::handle<OpenGl_FrameBuffer>& aMainSceneFboOit       = myMainSceneFbosOit[anFboIt];
      occ::handle<OpenGl_FrameBuffer>& anImmediateSceneFbo    = myImmediateSceneFbos[anFboIt];
      occ::handle<OpenGl_FrameBuffer>& anImmediateSceneFboOit = myImmediateSceneFbosOit[anFboIt];
      if (aMainSceneFbo->IsValid()
          && (aMainSceneFboOit->GetVPSize() != aRendSize
              || aMainSceneFboOit->NbSamples() != aNbSamples))
      {
        int aColorConfig = 0;
        for (;;) // seemly responding to driver limitation (GL_FRAMEBUFFER_UNSUPPORTED)
        {
          if (myFboOitColorConfig.IsEmpty())
          {
            if (!chooseOitColorConfiguration(aCtx, aColorConfig++, myFboOitColorConfig))
            {
              break;
            }
          }
          if (aMainSceneFboOit->Init(aCtx,
                                     aRendSize,
                                     myFboOitColorConfig,
                                     aMainSceneFbo->DepthStencilTexture(),
                                     aNbSamples))
          {
            break;
          }
          myFboOitColorConfig.Clear();
        }
        if (!aMainSceneFboOit->IsValid())
        {
          break;
        }
      }
      else if (!aMainSceneFbo->IsValid())
      {
        aMainSceneFboOit->Release(aCtx.operator->());
        aMainSceneFboOit->ChangeViewport(0, 0);
      }

      if (anImmediateSceneFbo->IsValid()
          && (anImmediateSceneFboOit->GetVPSize() != aRendSize
              || anImmediateSceneFboOit->NbSamples() != aNbSamples))
      {
        if (!anImmediateSceneFboOit->Init(aCtx,
                                          aRendSize,
                                          myFboOitColorConfig,
                                          anImmediateSceneFbo->DepthStencilTexture(),
                                          aNbSamples))
        {
          break;
        }
      }
      else if (!anImmediateSceneFbo->IsValid())
      {
        anImmediateSceneFboOit->Release(aCtx.operator->());
        anImmediateSceneFboOit->ChangeViewport(0, 0);
      }
    }
    if (anFboIt == 0) // only the first OIT framebuffer is mandatory
    {
      aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        "Initialization of float texture framebuffer for use with\n"
                        "  blended order-independent transparency rendering algorithm has failed.\n"
                        "  Blended order-independent transparency will not be available.\n");
      if (aNbSamples > 0)
      {
        myToDisableOITMSAA = true;
      }
      else
      {
        myToDisableOIT = true;
      }
      toUseOit = false;
    }
  }
  if (!toUseOit && myMainSceneFbosOit[0]->IsValid())
  {
    myDepthPeelingFbos->Release(aCtx.operator->());
    myMainSceneFbosOit[0]->Release(aCtx.operator->());
    myMainSceneFbosOit[1]->Release(aCtx.operator->());
    myImmediateSceneFbosOit[0]->Release(aCtx.operator->());
    myImmediateSceneFbosOit[1]->Release(aCtx.operator->());
    myMainSceneFbosOit[0]->ChangeViewport(0, 0);
    myMainSceneFbosOit[1]->ChangeViewport(0, 0);
    myImmediateSceneFbosOit[0]->ChangeViewport(0, 0);
    myImmediateSceneFbosOit[1]->ChangeViewport(0, 0);
  }

  // allocate shadow maps
  const occ::handle<Graphic3d_LightSet>& aLights =
    myRenderParams.ShadingModel == Graphic3d_TypeOfShadingModel_Unlit ? myNoShadingLight : myLights;
  if (!aLights.IsNull())
  {
    aLights->UpdateRevision();
  }
  bool toUseShadowMap = myRenderParams.IsShadowEnabled && myRenderParams.ShadowMapResolution > 0
                        && !myLights.IsNull() && myLights->NbCastShadows() > 0
                        && myRenderParams.Method != Graphic3d_RM_RAYTRACING;
  if (toUseShadowMap)
  {
    if (myShadowMaps->Size() != myLights->NbCastShadows())
    {
      myShadowMaps->Release(aCtx.get());
      myShadowMaps->Resize(0, myLights->NbCastShadows() - 1, true);
    }

    const GLint aSamplFrom = GLint(aCtx->ShadowMapTexUnit()) - myLights->NbCastShadows() + 1;
    for (int aShadowIter = 0; aShadowIter < myShadowMaps->Size(); ++aShadowIter)
    {
      occ::handle<OpenGl_ShadowMap>& aShadow = myShadowMaps->ChangeValue(aShadowIter);
      if (aShadow.IsNull())
      {
        aShadow = new OpenGl_ShadowMap();
      }
      aShadow->SetShadowMapBias(myRenderParams.ShadowMapBias);
      aShadow->Texture()->Sampler()->Parameters()->SetTextureUnit(
        (Graphic3d_TextureUnit)(aSamplFrom + aShadowIter));

      const occ::handle<OpenGl_FrameBuffer>& aShadowFbo = aShadow->FrameBuffer();
      if (aShadowFbo->GetVPSizeX() != myRenderParams.ShadowMapResolution && toUseShadowMap)
      {
        NCollection_Vector<int> aDummy;
        if (!aShadowFbo->Init(aCtx,
                              NCollection_Vec2<int>(myRenderParams.ShadowMapResolution),
                              aDummy,
                              myFboDepthFormat,
                              0))
        {
          toUseShadowMap = false;
        }
      }
    }
  }
  if (!toUseShadowMap && myShadowMaps->IsValid())
  {
    myShadowMaps->Release(aCtx.get());
  }

  return true;
}

//=================================================================================================

void OpenGl_View::Redraw()
{
  const bool wasDisabledMSAA = myToDisableMSAA;
  const bool hadFboBlit      = myHasFboBlit;
  if (myRenderParams.Method == Graphic3d_RM_RAYTRACING && !myCaps->vboDisable
      && !myCaps->keepArrayData)
  {
    // caps are shared across all views, thus we need to invalidate all of them
    // if (myWasRedrawnGL) { myStructureManager->SetDeviceLost(); }
    myDriver->setDeviceLost();
    myCaps->keepArrayData = true;
  }

  if (!myWorkspace->Activate())
  {
    return;
  }

  // implicitly disable VSync when using HMD composer (can be mirrored in window for debugging)
  myWindow->SetSwapInterval(IsActiveXR());

  ++myFrameCounter;
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  aCtx->FrameStats()->FrameStart(myWorkspace->View(), false);
  aCtx->SetLineFeather(myRenderParams.LineFeather);

  const int anSRgbState = aCtx->ToRenderSRGB() ? 1 : 0;
  if (mySRgbState != -1 && mySRgbState != anSRgbState)
  {
    releaseSrgbResources(aCtx);
    initTextureEnv(aCtx);
  }
  mySRgbState = anSRgbState;
  aCtx->ShaderManager()->UpdateSRgbState();

  // release pending GL resources
  aCtx->ReleaseDelayed();

  // fetch OpenGl context state
  aCtx->FetchState();

  const Graphic3d_StereoMode   aStereoMode  = myRenderParams.StereoMode;
  Graphic3d_Camera::Projection aProjectType = myCamera->ProjectionType();
  if (!prepareFrameBuffers(aProjectType))
  {
    myBackBufferRestored = false;
    myIsImmediateDrawn   = false;
    return;
  }

  // draw shadow maps
  if (myShadowMaps->IsValid())
  {
    int aShadowIndex = myShadowMaps->Lower();
    for (Graphic3d_LightSet::Iterator aLightIter(
           myLights,
           Graphic3d_LightSet::IterationFilter_ActiveShadowCasters);
         aLightIter.More();
         aLightIter.Next())
    {
      const occ::handle<Graphic3d_CLight>& aLight = aLightIter.Value();
      if (aLight->ToCastShadows())
      {
        const occ::handle<OpenGl_ShadowMap>& aShadowMap = myShadowMaps->ChangeValue(aShadowIndex);
        aShadowMap->SetLightSource(aLight);
        renderShadowMap(aShadowMap);
        ++aShadowIndex;
      }
    }
    for (; aShadowIndex <= myShadowMaps->Upper(); ++aShadowIndex)
    {
      myShadowMaps->ChangeValue(aShadowIndex)->SetLightSource(occ::handle<Graphic3d_CLight>());
    }
  }

  OpenGl_FrameBuffer* aFrameBuffer = myFBO.get();
  bool toSwap = aCtx->IsRender() && !aCtx->caps->buffersNoSwap && aFrameBuffer == nullptr
                && (!IsActiveXR() || myRenderParams.ToMirrorComposer);
  if (aFrameBuffer == nullptr && !aCtx->DefaultFrameBuffer().IsNull()
      && aCtx->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = aCtx->DefaultFrameBuffer().operator->();
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    OpenGl_FrameBuffer* aMainFbos[2] = {
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : nullptr,
      myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : nullptr};
    OpenGl_FrameBuffer* aMainFbosOit[2] = {
      myMainSceneFbosOit[0]->IsValid() ? myMainSceneFbosOit[0].operator->() : nullptr,
      myMainSceneFbosOit[1]->IsValid()   ? myMainSceneFbosOit[1].operator->()
      : myMainSceneFbosOit[0]->IsValid() ? myMainSceneFbosOit[0].operator->()
                                         : nullptr};

    OpenGl_FrameBuffer* anImmFbos[2] = {
      myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : nullptr,
      myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : nullptr};
    OpenGl_FrameBuffer* anImmFbosOit[2] = {
      myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : nullptr,
      myImmediateSceneFbosOit[1]->IsValid()   ? myImmediateSceneFbosOit[1].operator->()
      : myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->()
                                              : nullptr};

    if (IsActiveXR())
    {
      // use single frame for both views - caching main scene content makes no sense
      // when head position is expected to be updated each frame redraw with high accuracy
      aMainFbos[1]    = aMainFbos[0];
      aMainFbosOit[1] = aMainFbosOit[0];
      anImmFbos[0]    = aMainFbos[0];
      anImmFbos[1]    = aMainFbos[1];
      anImmFbosOit[0] = aMainFbosOit[0];
      anImmFbosOit[1] = aMainFbosOit[1];
    }
    else if (!myTransientDrawToFront)
    {
      anImmFbos[0]    = aMainFbos[0];
      anImmFbos[1]    = aMainFbos[1];
      anImmFbosOit[0] = aMainFbosOit[0];
      anImmFbosOit[1] = aMainFbosOit[1];
    }
    else if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
             || aStereoMode == Graphic3d_StereoMode_QuadBuffer)
    {
      anImmFbos[0]    = nullptr;
      anImmFbos[1]    = nullptr;
      anImmFbosOit[0] = nullptr;
      anImmFbosOit[1] = nullptr;
    }

    aCtx->SetReadDrawBuffer(aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT
                                                                           : GL_BACK);
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        aMainFbos[0] != nullptr ? myRenderParams.RenderResolutionScale : 1.0f);

    redraw(Graphic3d_Camera::Projection_MonoLeftEye, aMainFbos[0], aMainFbosOit[0]);
    myBackBufferRestored = true;
    myIsImmediateDrawn   = false;
    aCtx->SetReadDrawBuffer(aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT
                                                                           : GL_BACK);
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        anImmFbos[0] != nullptr ? myRenderParams.RenderResolutionScale : 1.0f);
    if (!redrawImmediate(Graphic3d_Camera::Projection_MonoLeftEye,
                         aMainFbos[0],
                         anImmFbos[0],
                         anImmFbosOit[0]))
    {
      toSwap = false;
    }
    else if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip && toSwap && myParentView == nullptr)
    {
      aCtx->SwapBuffers();
    }

    if (IsActiveXR())
    {
      // push Left frame to HMD display composer
      OpenGl_FrameBuffer* anXRFbo = myXrSceneFbo->IsValid() ? myXrSceneFbo.get() : aMainFbos[0];
      if (anXRFbo != aMainFbos[0])
      {
        blitBuffers(aMainFbos[0], anXRFbo); // resize or resolve MSAA samples
      }
      const Aspect_GraphicsLibrary aGraphicsLib = aCtx->GraphicsLibrary();
      myXRSession->SubmitEye((void*)(size_t)anXRFbo->ColorTexture()->TextureId(),
                             aGraphicsLib,
                             Aspect_ColorSpace_sRGB,
                             Aspect_Eye_Left);
    }

    if (aCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
    {
      aCtx->SetReadDrawBuffer(aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_RIGHT
                                                                             : GL_BACK);
    }
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        aMainFbos[1] != nullptr ? myRenderParams.RenderResolutionScale : 1.0f);

    redraw(Graphic3d_Camera::Projection_MonoRightEye, aMainFbos[1], aMainFbosOit[1]);
    myBackBufferRestored = true;
    myIsImmediateDrawn   = false;
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        anImmFbos[1] != nullptr ? myRenderParams.RenderResolutionScale : 1.0f);
    if (!redrawImmediate(Graphic3d_Camera::Projection_MonoRightEye,
                         aMainFbos[1],
                         anImmFbos[1],
                         anImmFbosOit[1]))
    {
      toSwap = false;
    }

    if (IsActiveXR())
    {
      // push Right frame to HMD display composer
      OpenGl_FrameBuffer* anXRFbo = myXrSceneFbo->IsValid() ? myXrSceneFbo.get() : aMainFbos[1];
      if (anXRFbo != aMainFbos[1])
      {
        blitBuffers(aMainFbos[1], anXRFbo); // resize or resolve MSAA samples
      }

      const Aspect_GraphicsLibrary aGraphicsLib = aCtx->GraphicsLibrary();
      myXRSession->SubmitEye((void*)(size_t)anXRFbo->ColorTexture()->TextureId(),
                             aGraphicsLib,
                             Aspect_ColorSpace_sRGB,
                             Aspect_Eye_Right);
      aCtx->core11fwd->glFinish();

      if (myRenderParams.ToMirrorComposer)
      {
        blitBuffers(anXRFbo, aFrameBuffer, myToFlipOutput);
      }
    }
    else if (anImmFbos[0] != nullptr)
    {
      aCtx->SetResolution(myRenderParams.Resolution, myRenderParams.ResolutionRatio(), 1.0f);
      drawStereoPair(aFrameBuffer);
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo =
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : aFrameBuffer;
    OpenGl_FrameBuffer* aMainFboOit =
      myMainSceneFbosOit[0]->IsValid() ? myMainSceneFbosOit[0].operator->() : nullptr;
    OpenGl_FrameBuffer* anImmFbo    = aFrameBuffer;
    OpenGl_FrameBuffer* anImmFboOit = nullptr;
    if (!myTransientDrawToFront)
    {
      anImmFbo    = aMainFbo;
      anImmFboOit = aMainFboOit;
    }
    else if (myImmediateSceneFbos[0]->IsValid())
    {
      anImmFbo = myImmediateSceneFbos[0].operator->();
      anImmFboOit =
        myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : nullptr;
    }

    if (aMainFbo == nullptr)
    {
      aCtx->SetReadDrawBuffer(GL_BACK);
    }
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        aMainFbo != aFrameBuffer ? myRenderParams.RenderResolutionScale : 1.0f);

    redraw(aProjectType, aMainFbo, aMainFboOit);
    myBackBufferRestored = true;
    myIsImmediateDrawn   = false;
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        anImmFbo != aFrameBuffer ? myRenderParams.RenderResolutionScale : 1.0f);
    if (!redrawImmediate(aProjectType, aMainFbo, anImmFbo, anImmFboOit))
    {
      toSwap = false;
    }

    if (anImmFbo != nullptr && anImmFbo != aFrameBuffer)
    {
      blitBuffers(anImmFbo, aFrameBuffer, myToFlipOutput);
    }
  }

  if (myRenderParams.Method == Graphic3d_RM_RAYTRACING
      && myRenderParams.IsGlobalIlluminationEnabled)
  {
    myAccumFrames++;
  }

  // bind default FBO
  bindDefaultFbo();

  if (wasDisabledMSAA != myToDisableMSAA || hadFboBlit != myHasFboBlit)
  {
    // retry on error
    Redraw();
  }

  // reset state for safety
  aCtx->BindProgram(occ::handle<OpenGl_ShaderProgram>());
  if (aCtx->caps->ffpEnable)
  {
    aCtx->ShaderManager()->PushState(occ::handle<OpenGl_ShaderProgram>());
  }

  // Swap the buffers
  if (toSwap && myParentView == nullptr)
  {
    aCtx->SwapBuffers();
    if (!myMainSceneFbos[0]->IsValid())
    {
      myBackBufferRestored = false;
    }
  }
  else
  {
    aCtx->core11fwd->glFlush();
  }

  // reset render mode state
  aCtx->FetchState();
  aCtx->FrameStats()->FrameEnd(myWorkspace->View(), false);

  myWasRedrawnGL = true;
}

//=================================================================================================

void OpenGl_View::RedrawImmediate()
{
  if (!myWorkspace->Activate())
    return;

  // no special handling of HMD display, since it will force full Redraw() due to no frame caching
  // (myBackBufferRestored)
  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  if (!myTransientDrawToFront || !myBackBufferRestored
      || (aCtx->caps->buffersNoSwap && !myMainSceneFbos[0]->IsValid()))
  {
    Redraw();
    return;
  }

  const Graphic3d_StereoMode   aStereoMode  = myRenderParams.StereoMode;
  Graphic3d_Camera::Projection aProjectType = myCamera->ProjectionType();
  OpenGl_FrameBuffer*          aFrameBuffer = myFBO.get();
  aCtx->FrameStats()->FrameStart(myWorkspace->View(), true);

  if (aFrameBuffer == nullptr && !aCtx->DefaultFrameBuffer().IsNull()
      && aCtx->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = aCtx->DefaultFrameBuffer().operator->();
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    if (myMainSceneFbos[0]->IsValid() && !myMainSceneFbos[1]->IsValid())
    {
      aProjectType = Graphic3d_Camera::Projection_Perspective;
    }
  }

  bool toSwap = false;
  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    OpenGl_FrameBuffer* aMainFbos[2] = {
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : nullptr,
      myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : nullptr};
    OpenGl_FrameBuffer* anImmFbos[2] = {
      myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : nullptr,
      myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : nullptr};
    OpenGl_FrameBuffer* anImmFbosOit[2] = {
      myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : nullptr,
      myImmediateSceneFbosOit[1]->IsValid()   ? myImmediateSceneFbosOit[1].operator->()
      : myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->()
                                              : nullptr};
    if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
        || aStereoMode == Graphic3d_StereoMode_QuadBuffer)
    {
      anImmFbos[0]    = nullptr;
      anImmFbos[1]    = nullptr;
      anImmFbosOit[0] = nullptr;
      anImmFbosOit[1] = nullptr;
    }

    if (aCtx->arbFBO != nullptr)
    {
      aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
    if (anImmFbos[0] == nullptr)
    {
      aCtx->SetReadDrawBuffer(aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT
                                                                             : GL_BACK);
    }

    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        anImmFbos[0] != nullptr ? myRenderParams.RenderResolutionScale : 1.0f);
    toSwap = redrawImmediate(Graphic3d_Camera::Projection_MonoLeftEye,
                             aMainFbos[0],
                             anImmFbos[0],
                             anImmFbosOit[0],
                             true)
             || toSwap;
    if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip && toSwap && myFBO.get() == nullptr
        && !aCtx->caps->buffersNoSwap && myParentView == nullptr)
    {
      aCtx->SwapBuffers();
    }

    if (aCtx->arbFBO != nullptr)
    {
      aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
    if (anImmFbos[1] == nullptr)
    {
      aCtx->SetReadDrawBuffer(aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_RIGHT
                                                                             : GL_BACK);
    }
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        anImmFbos[1] != nullptr ? myRenderParams.RenderResolutionScale : 1.0f);
    toSwap = redrawImmediate(Graphic3d_Camera::Projection_MonoRightEye,
                             aMainFbos[1],
                             anImmFbos[1],
                             anImmFbosOit[1],
                             true)
             || toSwap;
    if (anImmFbos[0] != nullptr)
    {
      drawStereoPair(aFrameBuffer);
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo =
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : nullptr;
    OpenGl_FrameBuffer* anImmFbo    = aFrameBuffer;
    OpenGl_FrameBuffer* anImmFboOit = nullptr;
    if (myImmediateSceneFbos[0]->IsValid())
    {
      anImmFbo = myImmediateSceneFbos[0].operator->();
      anImmFboOit =
        myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : nullptr;
    }
    if (aMainFbo == nullptr)
    {
      aCtx->SetReadDrawBuffer(GL_BACK);
    }
    aCtx->SetResolution(myRenderParams.Resolution,
                        myRenderParams.ResolutionRatio(),
                        anImmFbo != aFrameBuffer ? myRenderParams.RenderResolutionScale : 1.0f);
    toSwap = redrawImmediate(aProjectType, aMainFbo, anImmFbo, anImmFboOit, true) || toSwap;
    if (anImmFbo != nullptr && anImmFbo != aFrameBuffer)
    {
      blitBuffers(anImmFbo, aFrameBuffer, myToFlipOutput);
    }
  }

  // bind default FBO
  bindDefaultFbo();

  // reset state for safety
  aCtx->BindProgram(occ::handle<OpenGl_ShaderProgram>());
  if (aCtx->caps->ffpEnable)
  {
    aCtx->ShaderManager()->PushState(occ::handle<OpenGl_ShaderProgram>());
  }

  if (toSwap && myFBO.get() == nullptr && !aCtx->caps->buffersNoSwap && myParentView == nullptr)
  {
    aCtx->SwapBuffers();
  }
  else
  {
    aCtx->core11fwd->glFlush();
  }
  aCtx->FrameStats()->FrameEnd(myWorkspace->View(), true);

  myWasRedrawnGL = true;
}

//=================================================================================================

void OpenGl_View::redraw(const Graphic3d_Camera::Projection theProjection,
                         OpenGl_FrameBuffer*                theReadDrawFbo,
                         OpenGl_FrameBuffer*                theOitAccumFbo)
{
  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  if (theReadDrawFbo != nullptr)
  {
    theReadDrawFbo->BindBuffer(aCtx);
    theReadDrawFbo->SetupViewport(aCtx);
  }
  else
  {
    const int aViewport[4] = {0, 0, myWindow->Width(), myWindow->Height()};
    aCtx->ResizeViewport(aViewport);
  }

  // request reset of material
  aCtx->ShaderManager()->UpdateMaterialState();

  myWorkspace->UseZBuffer()    = true;
  myWorkspace->UseDepthWrite() = true;
  GLbitfield toClear           = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
  aCtx->core11fwd->glDepthFunc(GL_LEQUAL);
  aCtx->core11fwd->glDepthMask(GL_TRUE);
  aCtx->core11fwd->glEnable(GL_DEPTH_TEST);

  aCtx->core11fwd->glClearDepth(1.0);

  const NCollection_Vec4<float> aBgColor = aCtx->Vec4FromQuantityColor(myBgColor);
  // clang-format off
  aCtx->SetColorMaskRGBA (NCollection_Vec4<bool> (true)); // force writes into all components, including alpha
  // clang-format on
  aCtx->core11fwd->glClearColor(aBgColor.r(),
                                aBgColor.g(),
                                aBgColor.b(),
                                aCtx->caps->buffersOpaqueAlpha ? 1.0f : 0.0f);
  aCtx->core11fwd->glClear(toClear);
  aCtx->SetColorMask(true); // restore default alpha component write state

  render(theProjection, theReadDrawFbo, theOitAccumFbo, false);
}

//=================================================================================================

bool OpenGl_View::redrawImmediate(const Graphic3d_Camera::Projection theProjection,
                                  OpenGl_FrameBuffer*                theReadFbo,
                                  OpenGl_FrameBuffer*                theDrawFbo,
                                  OpenGl_FrameBuffer*                theOitAccumFbo,
                                  const bool                         theIsPartialUpdate)
{
  const occ::handle<OpenGl_Context>& aCtx              = myWorkspace->GetGlContext();
  GLboolean                          toCopyBackToFront = GL_FALSE;
  if (theDrawFbo == theReadFbo && theDrawFbo != nullptr && theDrawFbo->IsValid())
  {
    myBackBufferRestored = false;
    theDrawFbo->BindBuffer(aCtx);
  }
  else if (theReadFbo != nullptr && theReadFbo->IsValid() && aCtx->IsRender())
  {
    if (!blitBuffers(theReadFbo, theDrawFbo))
    {
      return true;
    }
  }
  else if (theDrawFbo == nullptr)
  {
    if (aCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
    {
      aCtx->core11fwd->glGetBooleanv(GL_DOUBLEBUFFER, &toCopyBackToFront);
    }
    if (toCopyBackToFront && myTransientDrawToFront)
    {
      if (!HasImmediateStructures() && !theIsPartialUpdate)
      {
        // prefer Swap Buffers within Redraw in compatibility mode (without FBO)
        return true;
      }
      if (!copyBackToFront())
      {
        toCopyBackToFront    = GL_FALSE;
        myBackBufferRestored = false;
      }
    }
    else
    {
      toCopyBackToFront    = GL_FALSE;
      myBackBufferRestored = false;
    }
  }
  else
  {
    myBackBufferRestored = false;
  }
  myIsImmediateDrawn = true;

  myWorkspace->UseZBuffer()    = true;
  myWorkspace->UseDepthWrite() = true;
  aCtx->core11fwd->glDepthFunc(GL_LEQUAL);
  aCtx->core11fwd->glDepthMask(GL_TRUE);
  aCtx->core11fwd->glEnable(GL_DEPTH_TEST);
  aCtx->core11fwd->glClearDepth(1.0);

  render(theProjection, theDrawFbo, theOitAccumFbo, true);

  blitSubviews(theProjection, theDrawFbo);

  return !toCopyBackToFront;
}

//=================================================================================================

bool OpenGl_View::blitSubviews(const Graphic3d_Camera::Projection, OpenGl_FrameBuffer* theDrawFbo)
{
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  if (aCtx->arbFBOBlit == nullptr)
  {
    return false;
  }

  bool isChanged = false;
  for (const occ::handle<Graphic3d_CView>& aChildIter : mySubviews)
  {
    OpenGl_View* aSubView = dynamic_cast<OpenGl_View*>(aChildIter.get());
    if (!aSubView->IsActive())
    {
      continue;
    }

    const occ::handle<OpenGl_FrameBuffer>& aChildFbo = !aSubView->myImmediateSceneFbos[0].IsNull()
                                                         ? aSubView->myImmediateSceneFbos[0]
                                                         : aSubView->myMainSceneFbos[0];
    if (aChildFbo.IsNull() || !aChildFbo->IsValid())
    {
      continue;
    }

    aChildFbo->BindReadBuffer(aCtx);
    if (theDrawFbo != nullptr && theDrawFbo->IsValid())
    {
      theDrawFbo->BindDrawBuffer(aCtx);
    }
    else
    {
      aCtx->arbFBO->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
      aCtx->SetFrameBufferSRGB(false);
    }

    // clang-format off
    NCollection_Vec2<int> aWinSize (aCtx->Viewport()[2], aCtx->Viewport()[3]); //aSubView->GlWindow()->PlatformWindow()->Dimensions();
    // clang-format on
    NCollection_Vec2<int> aSubViewSize = aChildFbo->GetVPSize();
    NCollection_Vec2<int> aSubViewPos  = aSubView->SubviewTopLeft();
    NCollection_Vec2<int> aDestSize    = aSubViewSize;
    if (aSubView->RenderingParams().RenderResolutionScale != 1.0f)
    {
      aDestSize = NCollection_Vec2<int>(
        NCollection_Vec2<double>(aDestSize)
        / NCollection_Vec2<double>(aSubView->RenderingParams().RenderResolutionScale));
    }
    aSubViewPos.y() = aWinSize.y() - aDestSize.y() - aSubViewPos.y();

    const GLint aFilterGl = aDestSize == aSubViewSize ? GL_NEAREST : GL_LINEAR;
    aCtx->arbFBOBlit->glBlitFramebuffer(0,
                                        0,
                                        aSubViewSize.x(),
                                        aSubViewSize.y(),
                                        aSubViewPos.x(),
                                        aSubViewPos.y(),
                                        aSubViewPos.x() + aDestSize.x(),
                                        aSubViewPos.y() + aDestSize.y(),
                                        GL_COLOR_BUFFER_BIT,
                                        aFilterGl);
    const int anErr = aCtx->core11fwd->glGetError();
    if (anErr != GL_NO_ERROR)
    {
      TCollection_ExtendedString aMsg =
        TCollection_ExtendedString() + "FBO blitting has failed [Error "
        + OpenGl_Context::FormatGlError(anErr) + "]\n"
        + "  Please check your graphics driver settings or try updating driver.";
      if (aChildFbo->NbSamples() != 0)
      {
        myToDisableMSAA = true;
        aMsg += "\n  MSAA settings should not be overridden by driver!";
      }
      aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        aMsg);
    }

    if (theDrawFbo != nullptr && theDrawFbo->IsValid())
    {
      theDrawFbo->BindBuffer(aCtx);
    }
    else
    {
      aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
      aCtx->SetFrameBufferSRGB(false);
    }
    isChanged = true;
  }

  return isChanged;
}

//=================================================================================================

void OpenGl_View::renderShadowMap(const occ::handle<OpenGl_ShadowMap>& theShadowMap)
{
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  if (!theShadowMap->UpdateCamera(*this))
  {
    return;
  }

  myBVHSelector.SetViewVolume(theShadowMap->Camera());
  myBVHSelector.SetViewportSize(myWindow->Width(),
                                myWindow->Height(),
                                myRenderParams.ResolutionRatio());
  myBVHSelector.CacheClipPtsProjections();

  myLocalOrigin.SetCoord(0.0, 0.0, 0.0);
  aCtx->SetCamera(theShadowMap->Camera());
  aCtx->ProjectionState.SetCurrent(theShadowMap->Camera()->ProjectionMatrixF());
  aCtx->ApplyProjectionMatrix();

  aCtx->ShaderManager()->UpdateMaterialState();
  aCtx->ShaderManager()->UpdateModelWorldStateTo(NCollection_Mat4<float>());
  aCtx->ShaderManager()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);

  const occ::handle<OpenGl_FrameBuffer>& aShadowBuffer = theShadowMap->FrameBuffer();
  aShadowBuffer->BindBuffer(aCtx);
  aShadowBuffer->SetupViewport(aCtx);

  aCtx->SetColorMask(false);
  aCtx->SetAllowSampleAlphaToCoverage(false);
  aCtx->SetSampleAlphaToCoverage(false);

  myWorkspace->UseZBuffer()    = true;
  myWorkspace->UseDepthWrite() = true;
  aCtx->core11fwd->glDepthFunc(GL_LEQUAL);
  aCtx->core11fwd->glDepthMask(GL_TRUE);
  aCtx->core11fwd->glEnable(GL_DEPTH_TEST);
  aCtx->core11fwd->glClearDepth(1.0);
  aCtx->core11fwd->glClear(GL_DEPTH_BUFFER_BIT);

  Graphic3d_Camera::Projection aProjection =
    theShadowMap->LightSource()->Type() == Graphic3d_TypeOfLightSource_Directional
      ? Graphic3d_Camera::Projection_Orthographic
      : Graphic3d_Camera::Projection_Perspective;
  myWorkspace->SetRenderFilter(myWorkspace->RenderFilter()
                               | OpenGl_RenderFilter_SkipTrsfPersistence);
  renderScene(aProjection, aShadowBuffer.get(), nullptr, false);
  myWorkspace->SetRenderFilter(myWorkspace->RenderFilter()
                               & ~(int)OpenGl_RenderFilter_SkipTrsfPersistence);

  aCtx->SetColorMask(true);
  myWorkspace->ResetAppliedAspect();
  aCtx->BindProgram(occ::handle<OpenGl_ShaderProgram>());

  // Image_AlienPixMap anImage; anImage.InitZero (Image_Format_Gray, aShadowBuffer->GetVPSizeX(),
  // aShadowBuffer->GetVPSizeY()); OpenGl_FrameBuffer::BufferDump (aCtx, aShadowBuffer, anImage,
  // Graphic3d_BT_Depth); anImage.Save (TCollection_AsciiString ("shadow") +
  // theShadowMap->Texture()->Sampler()->Parameters()->TextureUnit() + ".png");

  bindDefaultFbo();
}

//=================================================================================================

void OpenGl_View::render(Graphic3d_Camera::Projection theProjection,
                         OpenGl_FrameBuffer*          theOutputFBO,
                         OpenGl_FrameBuffer*          theOitAccumFbo,
                         const bool                   theToDrawImmediate)
{
  // ==================================
  //      Step 1: Prepare for render
  // ==================================

  const occ::handle<OpenGl_Context>& aContext = myWorkspace->GetGlContext();
  aContext->SetAllowSampleAlphaToCoverage(myRenderParams.ToEnableAlphaToCoverage
                                          && theOutputFBO != nullptr
                                          && theOutputFBO->NbSamples() != 0);

  // Disable current clipping planes
  if (aContext->core11ffp != nullptr)
  {
    const int aMaxPlanes = aContext->MaxClipPlanes();
    for (int aClipPlaneId = GL_CLIP_PLANE0; aClipPlaneId < GL_CLIP_PLANE0 + aMaxPlanes;
         ++aClipPlaneId)
    {
      aContext->core11fwd->glDisable(aClipPlaneId);
    }
  }

  // update states of OpenGl_BVHTreeSelector (frustum culling algorithm);
  // note that we pass here window dimensions ignoring
  // Graphic3d_RenderingParams::RenderResolutionScale
  myBVHSelector.SetViewVolume(myCamera);
  myBVHSelector.SetViewportSize(myWindow->Width(),
                                myWindow->Height(),
                                myRenderParams.ResolutionRatio());
  myBVHSelector.CacheClipPtsProjections();

  const occ::handle<OpenGl_ShaderManager>& aManager = aContext->ShaderManager();
  const occ::handle<Graphic3d_LightSet>&   aLights =
    myRenderParams.ShadingModel == Graphic3d_TypeOfShadingModel_Unlit ? myNoShadingLight : myLights;
  size_t aLightsRevision = 0;
  if (!aLights.IsNull())
  {
    aLightsRevision = aLights->UpdateRevision();
  }
  if (StateInfo(myCurrLightSourceState, aManager->LightSourceState().Index())
        != myLastLightSourceState
      || aLightsRevision != myLightsRevision)
  {
    myLightsRevision = aLightsRevision;
    aManager->UpdateLightSourceStateTo(
      aLights,
      SpecIBLMapLevels(),
      myShadowMaps->IsValid() ? myShadowMaps : occ::handle<OpenGl_ShadowMapArray>());
    myLastLightSourceState =
      StateInfo(myCurrLightSourceState, aManager->LightSourceState().Index());
  }

  // Update matrices if camera has changed.
  Graphic3d_WorldViewProjState aWVPState = myCamera->WorldViewProjState();
  if (myWorldViewProjState != aWVPState)
  {
    myAccumFrames        = 0;
    myWorldViewProjState = aWVPState;
  }

  myLocalOrigin.SetCoord(0.0, 0.0, 0.0);
  aContext->SetCamera(myCamera);
  if (aManager->ModelWorldState().Index() == 0)
  {
    aContext->ShaderManager()->UpdateModelWorldStateTo(NCollection_Mat4<float>());
  }

  // ====================================
  //      Step 2: Redraw background
  // ====================================

  // Render background
  if (!theToDrawImmediate)
  {
    drawBackground(myWorkspace, theProjection);
  }

  // Switch off lighting by default
  if (aContext->core11ffp != nullptr && aContext->caps->ffpEnable)
  {
    aContext->core11fwd->glDisable(GL_LIGHTING);
  }

  // =================================
  //      Step 3: Redraw main plane
  // =================================

  // if the view is scaled normal vectors are scaled to unit
  // length for correct displaying of shaded objects
  const gp_Pnt anAxialScale = aContext->Camera()->AxialScale();
  if (anAxialScale.X() != 1.F || anAxialScale.Y() != 1.F || anAxialScale.Z() != 1.F)
  {
    aContext->SetGlNormalizeEnabled(true);
  }
  else
  {
    aContext->SetGlNormalizeEnabled(false);
  }

  aManager->SetShadingModel(
    OpenGl_ShaderManager::PBRShadingModelFallback(myRenderParams.ShadingModel,
                                                  checkPBRAvailability()));

  // Redraw 3d scene
  if (theProjection == Graphic3d_Camera::Projection_MonoLeftEye)
  {
    aContext->ProjectionState.SetCurrent(aContext->Camera()->ProjectionStereoLeftF());
    aContext->ApplyProjectionMatrix();
  }
  else if (theProjection == Graphic3d_Camera::Projection_MonoRightEye)
  {
    aContext->ProjectionState.SetCurrent(aContext->Camera()->ProjectionStereoRightF());
    aContext->ApplyProjectionMatrix();
  }

  myWorkspace->SetEnvironmentTexture(myTextureEnv);

  const bool hasShadowMap = aContext->ShaderManager()->LightSourceState().HasShadowMaps();
  if (hasShadowMap)
  {
    for (int aShadowIter = myShadowMaps->Lower(); aShadowIter <= myShadowMaps->Upper();
         ++aShadowIter)
    {
      const occ::handle<OpenGl_ShadowMap>& aShadow = myShadowMaps->Value(aShadowIter);
      aShadow->Texture()->Bind(aContext);
    }
  }

  renderScene(theProjection, theOutputFBO, theOitAccumFbo, theToDrawImmediate);

  if (hasShadowMap)
  {
    for (int aShadowIter = myShadowMaps->Lower(); aShadowIter <= myShadowMaps->Upper();
         ++aShadowIter)
    {
      const occ::handle<OpenGl_ShadowMap>& aShadow = myShadowMaps->Value(aShadowIter);
      aShadow->Texture()->Unbind(aContext);
    }
    if (aContext->core15fwd != nullptr)
    {
      aContext->core15fwd->glActiveTexture(GL_TEXTURE0);
    }
  }

  myWorkspace->SetEnvironmentTexture(occ::handle<OpenGl_TextureSet>());

  // ===============================
  //      Step 4: Trihedron
  // ===============================

  // Resetting GL parameters according to the default aspects
  // in order to synchronize GL state with the graphic driver state
  // before drawing auxiliary stuff (trihedrons, overlayer)
  myWorkspace->ResetAppliedAspect();

  // Render trihedron
  if (!theToDrawImmediate)
  {
    renderTrihedron(myWorkspace);
  }
  else
  {
    renderFrameStats();
  }

  myWorkspace->ResetAppliedAspect();
  aContext->SetAllowSampleAlphaToCoverage(false);
  aContext->SetSampleAlphaToCoverage(false);

  // reset FFP state for safety
  aContext->BindProgram(occ::handle<OpenGl_ShaderProgram>());
  if (aContext->caps->ffpEnable)
  {
    aContext->ShaderManager()->PushState(occ::handle<OpenGl_ShaderProgram>());
  }
}

//=================================================================================================

void OpenGl_View::InvalidateBVHData(const Graphic3d_ZLayerId theLayerId)
{
  myZLayers.InvalidateBVHData(theLayerId);
}

//=================================================================================================

void OpenGl_View::renderStructs(Graphic3d_Camera::Projection theProjection,
                                OpenGl_FrameBuffer*          theReadDrawFbo,
                                OpenGl_FrameBuffer*          theOitAccumFbo,
                                const bool                   theToDrawImmediate)
{
  if (myIsSubviewComposer)
  {
    return;
  }

  myZLayers.UpdateCulling(myWorkspace, theToDrawImmediate);
  if (myZLayers.NbStructures() <= 0)
  {
    return;
  }

  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  bool toRenderGL = theToDrawImmediate || myRenderParams.Method != Graphic3d_RM_RAYTRACING
                    || myRaytraceInitStatus == OpenGl_RT_FAIL || aCtx->IsFeedback();

  if (!toRenderGL)
  {
    const NCollection_Vec2<int> aSizeXY =
      theReadDrawFbo != nullptr ? theReadDrawFbo->GetVPSize()
                                : NCollection_Vec2<int>(myWindow->Width(), myWindow->Height());

    toRenderGL = !initRaytraceResources(aSizeXY.x(), aSizeXY.y(), aCtx)
                 || !updateRaytraceGeometry(OpenGl_GUM_CHECK, myId, aCtx);

    toRenderGL |= !myIsRaytraceDataValid; // if no ray-trace data use OpenGL

    if (!toRenderGL)
    {
      myOpenGlFBO->InitLazy(aCtx, aSizeXY, myFboColorFormat, myFboDepthFormat, 0);
      if (theReadDrawFbo != nullptr)
      {
        theReadDrawFbo->UnbindBuffer(aCtx);
      }

      // Prepare preliminary OpenGL output
      if (aCtx->arbFBOBlit != nullptr)
      {
        // Render bottom OSD layer
        myZLayers.Render(myWorkspace,
                         theToDrawImmediate,
                         OpenGl_LF_Bottom,
                         myZLayerTarget,
                         theReadDrawFbo,
                         theOitAccumFbo);

        const int aPrevFilter =
          myWorkspace->RenderFilter() & ~(int)(OpenGl_RenderFilter_NonRaytraceableOnly);
        myWorkspace->SetRenderFilter(aPrevFilter | OpenGl_RenderFilter_NonRaytraceableOnly);
        {
          if (theReadDrawFbo != nullptr)
          {
            theReadDrawFbo->BindDrawBuffer(aCtx);
          }
          else
          {
            aCtx->arbFBO->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            aCtx->SetFrameBufferSRGB(false);
          }

          // Render non-polygonal elements in default layer
          myZLayers.Render(myWorkspace,
                           theToDrawImmediate,
                           OpenGl_LF_RayTracable,
                           myZLayerTarget,
                           theReadDrawFbo,
                           theOitAccumFbo);
        }
        myWorkspace->SetRenderFilter(aPrevFilter);
      }

      if (theReadDrawFbo != nullptr)
      {
        theReadDrawFbo->BindBuffer(aCtx);
      }
      else
      {
        aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, 0);
        aCtx->SetFrameBufferSRGB(false);
      }

      // Reset OpenGl aspects state to default to avoid enabling of
      // backface culling which is not supported in ray-tracing.
      myWorkspace->ResetAppliedAspect();

      // Ray-tracing polygonal primitive arrays
      raytrace(aSizeXY.x(), aSizeXY.y(), theProjection, theReadDrawFbo, aCtx);

      // Render upper (top and topmost) OpenGL layers
      myZLayers.Render(myWorkspace,
                       theToDrawImmediate,
                       OpenGl_LF_Upper,
                       myZLayerTarget,
                       theReadDrawFbo,
                       theOitAccumFbo);
    }
  }

  // Redraw 3D scene using OpenGL in standard
  // mode or in case of ray-tracing failure
  if (toRenderGL)
  {
    // check if only a single layer is to be dumped
    OpenGl_LayerFilter aFilter = myZLayerRedrawMode ? OpenGl_LF_Single : OpenGl_LF_All;
    myZLayers.Render(myWorkspace,
                     theToDrawImmediate,
                     aFilter,
                     myZLayerTarget,
                     theReadDrawFbo,
                     theOitAccumFbo);

    // Set flag that scene was redrawn by standard pipeline
    myWasRedrawnGL = true;
  }
}

//=================================================================================================

void OpenGl_View::renderTrihedron(const occ::handle<OpenGl_Workspace>& theWorkspace)
{
  if (myToShowGradTrihedron)
  {
    myGraduatedTrihedron.Render(theWorkspace);
  }
}

//=================================================================================================

void OpenGl_View::renderFrameStats()
{
  if (myRenderParams.ToShowStats
      && myRenderParams.CollectedStats != Graphic3d_RenderingParams::PerfCounters_NONE)
  {
    myFrameStatsPrs.Update(myWorkspace);
    myFrameStatsPrs.Render(myWorkspace);
  }
}

//=================================================================================================

void OpenGl_View::Invalidate()
{
  myBackBufferRestored = false;
}

//=================================================================================================

void OpenGl_View::renderScene(Graphic3d_Camera::Projection theProjection,
                              OpenGl_FrameBuffer*          theReadDrawFbo,
                              OpenGl_FrameBuffer*          theOitAccumFbo,
                              const bool                   theToDrawImmediate)
{
  const occ::handle<OpenGl_Context>& aContext = myWorkspace->GetGlContext();

  // Specify clipping planes in view transformation space
  aContext->ChangeClipping().Reset(myClipPlanes);
  if (!myClipPlanes.IsNull() && !myClipPlanes->IsEmpty())
  {
    aContext->ShaderManager()->UpdateClippingState();
  }

  renderStructs(theProjection, theReadDrawFbo, theOitAccumFbo, theToDrawImmediate);
  aContext->BindTextures(occ::handle<OpenGl_TextureSet>(), occ::handle<OpenGl_ShaderProgram>());

  // Apply restored view matrix.
  aContext->ApplyWorldViewMatrix();

  aContext->ChangeClipping().Reset(occ::handle<Graphic3d_SequenceOfHClipPlane>());
  if (!myClipPlanes.IsNull() && !myClipPlanes->IsEmpty())
  {
    aContext->ShaderManager()->RevertClippingState();
  }
}

//=================================================================================================

void OpenGl_View::bindDefaultFbo(OpenGl_FrameBuffer* theCustomFbo)
{
  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  OpenGl_FrameBuffer*         anFbo =
    (theCustomFbo != nullptr && theCustomFbo->IsValid())
              ? theCustomFbo
              : (!aCtx->DefaultFrameBuffer().IsNull() && aCtx->DefaultFrameBuffer()->IsValid()
                   ? aCtx->DefaultFrameBuffer().operator->()
                   : nullptr);
  if (anFbo != nullptr)
  {
    anFbo->BindBuffer(aCtx);
    anFbo->SetupViewport(aCtx);
  }
  else
  {
    if (aCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
    {
      aCtx->SetReadDrawBuffer(GL_BACK);
    }
    else if (aCtx->arbFBO != nullptr)
    {
      aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }

    const int aViewport[4] = {0, 0, myWindow->Width(), myWindow->Height()};
    aCtx->ResizeViewport(aViewport);
  }
}

//=================================================================================================

OpenGl_VertexBuffer* OpenGl_View::initBlitQuad(const bool theToFlip)
{
  OpenGl_VertexBuffer* aVerts = nullptr;
  if (!theToFlip)
  {
    aVerts = &myFullScreenQuad;
    if (!aVerts->IsValid())
    {
      NCollection_Vec4<float> aQuad[4] = {NCollection_Vec4<float>(1.0f, -1.0f, 1.0f, 0.0f),
                                          NCollection_Vec4<float>(1.0f, 1.0f, 1.0f, 1.0f),
                                          NCollection_Vec4<float>(-1.0f, -1.0f, 0.0f, 0.0f),
                                          NCollection_Vec4<float>(-1.0f, 1.0f, 0.0f, 1.0f)};
      aVerts->Init(myWorkspace->GetGlContext(), 4, 4, aQuad[0].GetData());
    }
  }
  else
  {
    aVerts = &myFullScreenQuadFlip;
    if (!aVerts->IsValid())
    {
      NCollection_Vec4<float> aQuad[4] = {NCollection_Vec4<float>(1.0f, -1.0f, 1.0f, 1.0f),
                                          NCollection_Vec4<float>(1.0f, 1.0f, 1.0f, 0.0f),
                                          NCollection_Vec4<float>(-1.0f, -1.0f, 0.0f, 1.0f),
                                          NCollection_Vec4<float>(-1.0f, 1.0f, 0.0f, 0.0f)};
      aVerts->Init(myWorkspace->GetGlContext(), 4, 4, aQuad[0].GetData());
    }
  }
  return aVerts;
}

//=================================================================================================

bool OpenGl_View::blitBuffers(OpenGl_FrameBuffer* theReadFbo,
                              OpenGl_FrameBuffer* theDrawFbo,
                              const bool          theToFlip)
{
  occ::handle<OpenGl_Context> aCtx = myWorkspace->GetGlContext();
  const int aReadSizeX = theReadFbo != nullptr ? theReadFbo->GetVPSizeX() : myWindow->Width();
  const int aReadSizeY = theReadFbo != nullptr ? theReadFbo->GetVPSizeY() : myWindow->Height();
  const int aDrawSizeX = theDrawFbo != nullptr ? theDrawFbo->GetVPSizeX() : myWindow->Width();
  const int aDrawSizeY = theDrawFbo != nullptr ? theDrawFbo->GetVPSizeY() : myWindow->Height();
  if (theReadFbo == nullptr || aCtx->IsFeedback())
  {
    return false;
  }
  else if (theReadFbo == theDrawFbo)
  {
    return true;
  }

  // clear destination before blitting
  if (theDrawFbo != nullptr && theDrawFbo->IsValid())
  {
    theDrawFbo->BindBuffer(aCtx);
  }
  else
  {
    aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    aCtx->SetFrameBufferSRGB(false);
  }
  const int aViewport[4] = {0, 0, aDrawSizeX, aDrawSizeY};
  aCtx->ResizeViewport(aViewport);

  // clang-format off
  aCtx->SetColorMaskRGBA (NCollection_Vec4<bool> (true)); // force writes into all components, including alpha
  // clang-format on
  aCtx->core20fwd->glClearDepth(1.0);
  aCtx->core20fwd->glClearColor(0.0f, 0.0f, 0.0f, aCtx->caps->buffersOpaqueAlpha ? 1.0f : 0.0f);
  aCtx->core20fwd->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  aCtx->SetColorMask(true); // restore default alpha component write state

  const bool toApplyGamma  = aCtx->ToRenderSRGB() != aCtx->IsFrameBufferSRGB();
  bool       toDrawTexture = true;
  if (aCtx->arbFBOBlit != nullptr)
  {
    if (!toApplyGamma && theReadFbo->NbSamples() != 0)
    {
      toDrawTexture = false;
    }
    if (theReadFbo->IsColorRenderBuffer())
    {
      // render buffers could be resolved only via glBlitFramebuffer()
      toDrawTexture = false;
    }
  }

  if (!toDrawTexture)
  {
    GLbitfield aCopyMask = 0;
    theReadFbo->BindReadBuffer(aCtx);
    if (theDrawFbo != nullptr && theDrawFbo->IsValid())
    {
      theDrawFbo->BindDrawBuffer(aCtx);
      if (theDrawFbo->HasColor() && theReadFbo->HasColor())
      {
        aCopyMask |= GL_COLOR_BUFFER_BIT;
      }
      if (theDrawFbo->HasDepth() && theReadFbo->HasDepth())
      {
        aCopyMask |= GL_DEPTH_BUFFER_BIT;
      }
    }
    else
    {
      if (theReadFbo->HasColor())
      {
        aCopyMask |= GL_COLOR_BUFFER_BIT;
      }
      if (theReadFbo->HasDepth())
      {
        aCopyMask |= GL_DEPTH_BUFFER_BIT;
      }
      aCtx->arbFBO->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
      aCtx->SetFrameBufferSRGB(false);
    }

    // we don't copy stencil buffer here... does it matter for performance?
    aCtx->arbFBOBlit->glBlitFramebuffer(0,
                                        0,
                                        aReadSizeX,
                                        aReadSizeY,
                                        0,
                                        0,
                                        aDrawSizeX,
                                        aDrawSizeY,
                                        aCopyMask,
                                        GL_NEAREST);
    const int anErr = aCtx->core11fwd->glGetError();
    if (anErr != GL_NO_ERROR)
    {
      // glBlitFramebuffer() might fail in several cases:
      // - Both FBOs have MSAA and they are samples number does not match.
      //   OCCT checks that this does not happen,
      //   however some graphics drivers provide an option for overriding MSAA.
      //   In this case window MSAA might be non-zero (and application can not check it)
      //   and might not match MSAA of our offscreen FBOs.
      // - Pixel formats of FBOs do not match.
      //   This also might happen with window has pixel format,
      //   e.g. Mesa fails blitting RGBA8 -> RGB8 while other drivers support this conversion.
      TCollection_ExtendedString aMsg =
        TCollection_ExtendedString() + "FBO blitting has failed [Error "
        + OpenGl_Context::FormatGlError(anErr) + "]\n"
        + "  Please check your graphics driver settings or try updating driver.";
      if (theReadFbo->NbSamples() != 0)
      {
        myToDisableMSAA = true;
        aMsg += "\n  MSAA settings should not be overridden by driver!";
      }
      aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        aMsg);
    }

    if (theDrawFbo != nullptr && theDrawFbo->IsValid())
    {
      theDrawFbo->BindBuffer(aCtx);
    }
    else
    {
      aCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
      aCtx->SetFrameBufferSRGB(false);
    }
  }
  else
  {
    aCtx->core20fwd->glDepthFunc(GL_ALWAYS);
    aCtx->core20fwd->glDepthMask(GL_TRUE);
    aCtx->core20fwd->glEnable(GL_DEPTH_TEST);
    if (aCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES && !aCtx->IsGlGreaterEqual(3, 0)
        && !aCtx->extFragDepth)
    {
      aCtx->core20fwd->glDisable(GL_DEPTH_TEST);
    }

    aCtx->BindTextures(occ::handle<OpenGl_TextureSet>(), occ::handle<OpenGl_ShaderProgram>());

    const Graphic3d_TypeOfTextureFilter aFilter =
      (aDrawSizeX == aReadSizeX && aDrawSizeY == aReadSizeY) ? Graphic3d_TOTF_NEAREST
                                                             : Graphic3d_TOTF_BILINEAR;
    const GLint aFilterGl = aFilter == Graphic3d_TOTF_NEAREST ? GL_NEAREST : GL_LINEAR;

    OpenGl_VertexBuffer*                     aVerts   = initBlitQuad(theToFlip);
    const occ::handle<OpenGl_ShaderManager>& aManager = aCtx->ShaderManager();
    if (aVerts->IsValid()
        && aManager->BindFboBlitProgram(theReadFbo != nullptr ? theReadFbo->NbSamples() : 0,
                                        toApplyGamma))
    {
      aCtx->SetSampleAlphaToCoverage(false);
      theReadFbo->ColorTexture()->Bind(aCtx, Graphic3d_TextureUnit_0);
      if (theReadFbo->ColorTexture()->Sampler()->Parameters()->Filter() != aFilter)
      {
        theReadFbo->ColorTexture()->Sampler()->Parameters()->SetFilter(aFilter);
        aCtx->core20fwd->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterGl);
        aCtx->core20fwd->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilterGl);
      }

      theReadFbo->DepthStencilTexture()->Bind(aCtx, Graphic3d_TextureUnit_1);
      if (theReadFbo->DepthStencilTexture()->Sampler()->Parameters()->Filter() != aFilter)
      {
        theReadFbo->DepthStencilTexture()->Sampler()->Parameters()->SetFilter(aFilter);
        aCtx->core20fwd->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterGl);
        aCtx->core20fwd->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilterGl);
      }

      aVerts->BindVertexAttrib(aCtx, Graphic3d_TOA_POS);

      aCtx->core20fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      aVerts->UnbindVertexAttrib(aCtx, Graphic3d_TOA_POS);
      theReadFbo->DepthStencilTexture()->Unbind(aCtx, Graphic3d_TextureUnit_1);
      theReadFbo->ColorTexture()->Unbind(aCtx, Graphic3d_TextureUnit_0);
      aCtx->BindProgram(nullptr);
    }
    else
    {
      TCollection_ExtendedString aMsg =
        TCollection_ExtendedString() + "Error! FBO blitting has failed";
      aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        aMsg);
      myHasFboBlit = false;
      theReadFbo->Release(aCtx.operator->());
      return true;
    }
  }
  return true;
}

//=================================================================================================

void OpenGl_View::drawStereoPair(OpenGl_FrameBuffer* theDrawFbo)
{
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  bindDefaultFbo(theDrawFbo);
  OpenGl_FrameBuffer* aPair[2] = {
    myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : nullptr,
    myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : nullptr};
  if (aPair[0] == nullptr || aPair[1] == nullptr || !myTransientDrawToFront)
  {
    aPair[0] = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : nullptr;
    aPair[1] = myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : nullptr;
  }

  if (aPair[0] == nullptr || aPair[1] == nullptr)
  {
    return;
  }

  if (aPair[0]->NbSamples() != 0)
  {
    // resolve MSAA buffers before drawing
    if (!myOpenGlFBO->InitLazy(aCtx, aPair[0]->GetVPSize(), myFboColorFormat, myFboDepthFormat, 0)
        || !myOpenGlFBO2->InitLazy(aCtx, aPair[0]->GetVPSize(), myFboColorFormat, 0, 0))
    {
      aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                        GL_DEBUG_TYPE_ERROR,
                        0,
                        GL_DEBUG_SEVERITY_HIGH,
                        "Error! Unable to allocate FBO for blitting stereo pair");
      bindDefaultFbo(theDrawFbo);
      return;
    }

    if (!blitBuffers(aPair[0], myOpenGlFBO.operator->(), false)
        || !blitBuffers(aPair[1], myOpenGlFBO2.operator->(), false))
    {
      bindDefaultFbo(theDrawFbo);
      return;
    }

    aPair[0] = myOpenGlFBO.operator->();
    aPair[1] = myOpenGlFBO2.operator->();
    bindDefaultFbo(theDrawFbo);
  }

  struct
  {
    int left;
    int top;
    int right;
    int bottom;

    int dx() { return right - left; }

    int dy() { return bottom - top; }
  } aGeom;

  myWindow->PlatformWindow()->Position(aGeom.left, aGeom.top, aGeom.right, aGeom.bottom);

  bool       toReverse = myRenderParams.ToReverseStereo;
  const bool isOddY    = (aGeom.top + aGeom.dy()) % 2 == 1;
  const bool isOddX    = aGeom.left % 2 == 1;
  if (isOddY
      && (myRenderParams.StereoMode == Graphic3d_StereoMode_RowInterlaced
          || myRenderParams.StereoMode == Graphic3d_StereoMode_ChessBoard))
  {
    toReverse = !toReverse;
  }
  if (isOddX
      && (myRenderParams.StereoMode == Graphic3d_StereoMode_ColumnInterlaced
          || myRenderParams.StereoMode == Graphic3d_StereoMode_ChessBoard))
  {
    toReverse = !toReverse;
  }

  if (toReverse)
  {
    std::swap(aPair[0], aPair[1]);
  }

  aCtx->core20fwd->glDepthFunc(GL_ALWAYS);
  aCtx->core20fwd->glDepthMask(GL_TRUE);
  aCtx->core20fwd->glEnable(GL_DEPTH_TEST);

  aCtx->BindTextures(occ::handle<OpenGl_TextureSet>(), occ::handle<OpenGl_ShaderProgram>());
  OpenGl_VertexBuffer* aVerts = initBlitQuad(myToFlipOutput);

  const occ::handle<OpenGl_ShaderManager>& aManager = aCtx->ShaderManager();
  if (!aVerts->IsValid() || !aManager->BindStereoProgram(myRenderParams.StereoMode))
  {
    aCtx->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                      GL_DEBUG_TYPE_ERROR,
                      0,
                      GL_DEBUG_SEVERITY_HIGH,
                      "Error! Anaglyph has failed");
    return;
  }

  switch (myRenderParams.StereoMode)
  {
    case Graphic3d_StereoMode_Anaglyph: {
      NCollection_Mat4<float> aFilterL, aFilterR;
      aFilterL.SetDiagonal(NCollection_Vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
      aFilterR.SetDiagonal(NCollection_Vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
      switch (myRenderParams.AnaglyphFilter)
      {
        case Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple: {
          aFilterL.SetRow(0, NCollection_Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f));
          aFilterR.SetRow(1, NCollection_Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f));
          aFilterR.SetRow(2, NCollection_Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized: {
          aFilterL.SetRow(0, NCollection_Vec4<float>(0.4154f, 0.4710f, 0.16666667f, 0.0f));
          aFilterL.SetRow(1, NCollection_Vec4<float>(-0.0458f, -0.0484f, -0.0257f, 0.0f));
          aFilterL.SetRow(2, NCollection_Vec4<float>(-0.0547f, -0.0615f, 0.0128f, 0.0f));
          aFilterL.SetRow(3, NCollection_Vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
          aFilterR.SetRow(0,
                          NCollection_Vec4<float>(-0.01090909f, -0.03636364f, -0.00606061f, 0.0f));
          aFilterR.SetRow(1, NCollection_Vec4<float>(0.37560000f, 0.73333333f, 0.01111111f, 0.0f));
          aFilterR.SetRow(2,
                          NCollection_Vec4<float>(-0.06510000f, -0.12870000f, 1.29710000f, 0.0f));
          aFilterR.SetRow(3, NCollection_Vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple: {
          aFilterL.SetRow(0, NCollection_Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f));
          aFilterL.SetRow(1, NCollection_Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f));
          aFilterR.SetRow(2, NCollection_Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized: {
          aFilterL.SetRow(0, NCollection_Vec4<float>(1.062f, -0.205f, 0.299f, 0.0f));
          aFilterL.SetRow(1, NCollection_Vec4<float>(-0.026f, 0.908f, 0.068f, 0.0f));
          aFilterL.SetRow(2, NCollection_Vec4<float>(-0.038f, -0.173f, 0.022f, 0.0f));
          aFilterL.SetRow(3, NCollection_Vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
          aFilterR.SetRow(0, NCollection_Vec4<float>(-0.016f, -0.123f, -0.017f, 0.0f));
          aFilterR.SetRow(1, NCollection_Vec4<float>(0.006f, 0.062f, -0.017f, 0.0f));
          aFilterR.SetRow(2, NCollection_Vec4<float>(0.094f, 0.185f, 0.911f, 0.0f));
          aFilterR.SetRow(3, NCollection_Vec4<float>(0.0f, 0.0f, 0.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple: {
          aFilterR.SetRow(0, NCollection_Vec4<float>(1.0f, 0.0f, 0.0f, 0.0f));
          aFilterL.SetRow(1, NCollection_Vec4<float>(0.0f, 1.0f, 0.0f, 0.0f));
          aFilterR.SetRow(2, NCollection_Vec4<float>(0.0f, 0.0f, 1.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_UserDefined: {
          aFilterL = myRenderParams.AnaglyphLeft;
          aFilterR = myRenderParams.AnaglyphRight;
          break;
        }
      }
      aCtx->ActiveProgram()->SetUniform(aCtx, "uMultL", aFilterL);
      aCtx->ActiveProgram()->SetUniform(aCtx, "uMultR", aFilterR);
      break;
    }
    case Graphic3d_StereoMode_RowInterlaced: {
      NCollection_Vec2<float> aTexOffset =
        myRenderParams.ToSmoothInterlacing
          ? NCollection_Vec2<float>(0.0f, -0.5f / float(aPair[0]->GetSizeY()))
          : NCollection_Vec2<float>();
      aCtx->ActiveProgram()->SetUniform(aCtx, "uTexOffset", aTexOffset);
      break;
    }
    case Graphic3d_StereoMode_ColumnInterlaced: {
      NCollection_Vec2<float> aTexOffset =
        myRenderParams.ToSmoothInterlacing
          ? NCollection_Vec2<float>(0.5f / float(aPair[0]->GetSizeX()), 0.0f)
          : NCollection_Vec2<float>();
      aCtx->ActiveProgram()->SetUniform(aCtx, "uTexOffset", aTexOffset);
      break;
    }
    case Graphic3d_StereoMode_ChessBoard: {
      NCollection_Vec2<float> aTexOffset =
        myRenderParams.ToSmoothInterlacing
          ? NCollection_Vec2<float>(0.5f / float(aPair[0]->GetSizeX()),
                                    -0.5f / float(aPair[0]->GetSizeY()))
          : NCollection_Vec2<float>();
      aCtx->ActiveProgram()->SetUniform(aCtx, "uTexOffset", aTexOffset);
      break;
    }
    default:
      break;
  }

  for (int anEyeIter = 0; anEyeIter < 2; ++anEyeIter)
  {
    OpenGl_FrameBuffer* anEyeFbo = aPair[anEyeIter];
    anEyeFbo->ColorTexture()->Bind(aCtx,
                                   (Graphic3d_TextureUnit)(Graphic3d_TextureUnit_0 + anEyeIter));
    if (anEyeFbo->ColorTexture()->Sampler()->Parameters()->Filter() != Graphic3d_TOTF_BILINEAR)
    {
      // force filtering
      anEyeFbo->ColorTexture()->Sampler()->Parameters()->SetFilter(Graphic3d_TOTF_BILINEAR);
      aCtx->core20fwd->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      aCtx->core20fwd->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
  }
  aVerts->BindVertexAttrib(aCtx, 0);

  aCtx->core20fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  aVerts->UnbindVertexAttrib(aCtx, 0);
  aPair[1]->ColorTexture()->Unbind(aCtx, Graphic3d_TextureUnit_1);
  aPair[0]->ColorTexture()->Unbind(aCtx, Graphic3d_TextureUnit_0);
}

//=================================================================================================

bool OpenGl_View::copyBackToFront()
{
  myIsImmediateDrawn                      = false;
  const occ::handle<OpenGl_Context>& aCtx = myWorkspace->GetGlContext();
  if (aCtx->core11ffp == nullptr)
  {
    return false;
  }

  NCollection_Mat4<float> aProjectMat;
  Graphic3d_TransformUtils::Ortho2D(aProjectMat,
                                    0.0f,
                                    static_cast<GLfloat>(myWindow->Width()),
                                    0.0f,
                                    static_cast<GLfloat>(myWindow->Height()));

  aCtx->WorldViewState.Push();
  aCtx->ProjectionState.Push();

  aCtx->WorldViewState.SetIdentity();
  aCtx->ProjectionState.SetCurrent(aProjectMat);

  aCtx->ApplyProjectionMatrix();
  aCtx->ApplyWorldViewMatrix();

  // synchronize FFP state before copying pixels
  aCtx->BindProgram(occ::handle<OpenGl_ShaderProgram>());
  aCtx->ShaderManager()->PushState(occ::handle<OpenGl_ShaderProgram>());
  aCtx->DisableFeatures();

  switch (aCtx->DrawBuffer())
  {
    case GL_BACK_LEFT: {
      aCtx->SetReadBuffer(GL_BACK_LEFT);
      aCtx->SetDrawBuffer(GL_FRONT_LEFT);
      break;
    }
    case GL_BACK_RIGHT: {
      aCtx->SetReadBuffer(GL_BACK_RIGHT);
      aCtx->SetDrawBuffer(GL_FRONT_RIGHT);
      break;
    }
    default: {
      aCtx->SetReadBuffer(GL_BACK);
      aCtx->SetDrawBuffer(GL_FRONT);
      break;
    }
  }

  aCtx->core11ffp->glRasterPos2i(0, 0);
  aCtx->core11ffp->glCopyPixels(0, 0, myWindow->Width() + 1, myWindow->Height() + 1, GL_COLOR);
  // aCtx->core11ffp->glCopyPixels  (0, 0, myWidth + 1, myHeight + 1, GL_DEPTH);

  aCtx->EnableFeatures();

  aCtx->WorldViewState.Pop();
  aCtx->ProjectionState.Pop();
  aCtx->ApplyProjectionMatrix();

  // read/write from front buffer now
  aCtx->SetReadBuffer(aCtx->DrawBuffer());
  return true;
}

//=================================================================================================

bool OpenGl_View::checkOitCompatibility(const occ::handle<OpenGl_Context>& theGlContext,
                                        const bool                         theMSAA)
{
  // determine if OIT is supported by current OpenGl context
  bool& aToDisableOIT = theMSAA ? myToDisableMSAA : myToDisableOIT;
  if (aToDisableOIT)
  {
    return false;
  }

  TCollection_ExtendedString aCompatibilityMsg;
  if (theGlContext->hasFloatBuffer == OpenGl_FeatureNotAvailable
      && theGlContext->hasHalfFloatBuffer == OpenGl_FeatureNotAvailable)
  {
    aCompatibilityMsg +=
      "OpenGL context does not support floating-point RGBA color buffer format.\n";
  }
  if (theMSAA && theGlContext->hasSampleVariables == OpenGl_FeatureNotAvailable)
  {
    aCompatibilityMsg += "Current version of GLSL does not support built-in sample variables.\n";
  }
  if (theGlContext->hasDrawBuffers == OpenGl_FeatureNotAvailable)
  {
    aCompatibilityMsg += "OpenGL context does not support multiple draw buffers.\n";
  }
  if (aCompatibilityMsg.IsEmpty())
  {
    return true;
  }

  aCompatibilityMsg += "  Blended order-independent transparency will not be available.\n";
  theGlContext->PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                            GL_DEBUG_TYPE_ERROR,
                            0,
                            GL_DEBUG_SEVERITY_HIGH,
                            aCompatibilityMsg);

  aToDisableOIT = true;
  return false;
}

//=================================================================================================

void OpenGl_View::updateSkydomeBg(const occ::handle<OpenGl_Context>& theCtx)
{
  if (!myToUpdateSkydome)
  {
    return;
  }

  myToUpdateSkydome = false;

  // Set custom shader
  occ::handle<OpenGl_ShaderProgram>    aProg;
  occ::handle<Graphic3d_ShaderProgram> aProxy = theCtx->ShaderManager()->GetBgSkydomeProgram();
  TCollection_AsciiString              anUnused;
  theCtx->ShaderManager()->Create(aProxy, anUnused, aProg);
  occ::handle<OpenGl_ShaderProgram> aPrevProgram = theCtx->ActiveProgram();
  theCtx->BindProgram(aProg);

  // Setup uniforms
  aProg->SetUniform(theCtx,
                    "uSunDir",
                    NCollection_Vec3<float>((float)mySkydomeAspect.SunDirection().X(),
                                            (float)mySkydomeAspect.SunDirection().Y(),
                                            (float)mySkydomeAspect.SunDirection().Z()));
  aProg->SetUniform(theCtx, "uCloudy", mySkydomeAspect.Cloudiness());
  aProg->SetUniform(theCtx, "uTime", mySkydomeAspect.TimeParameter());
  aProg->SetUniform(theCtx, "uFog", mySkydomeAspect.Fogginess());

  // Create and prepare framebuffer
  GLint aPrevFBO = 0;
  theCtx->core11fwd->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &aPrevFBO);
  GLuint anFBO = 0;
  theCtx->arbFBO->glGenFramebuffers(1, &anFBO);
  theCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, anFBO);

  const int anOldViewport[4] = {theCtx->Viewport()[0],
                                theCtx->Viewport()[1],
                                theCtx->Viewport()[2],
                                theCtx->Viewport()[3]};
  const int aViewport[4]     = {0, 0, mySkydomeAspect.Size(), mySkydomeAspect.Size()};
  theCtx->ResizeViewport(aViewport);

  // Fullscreen triangle
  occ::handle<OpenGl_VertexBuffer> aVBO        = new OpenGl_VertexBuffer();
  const float                      aTriangle[] = {-1.0, -1.0, 3.0, -1.0, -1.0, 3.0};
  aVBO->Init(theCtx, 2, 3, aTriangle);
  aVBO->BindAttribute(theCtx, Graphic3d_TypeOfAttribute::Graphic3d_TOA_POS);
  aVBO->Bind(theCtx);

  if (mySkydomeTexture.IsNull())
  {
    mySkydomeTexture = new OpenGl_Texture();
    mySkydomeTexture->Sampler()->Parameters()->SetFilter(Graphic3d_TOTF_BILINEAR);
  }
  if (mySkydomeTexture->SizeX() != mySkydomeAspect.Size())
  {
    mySkydomeTexture->Release(theCtx.get());
    mySkydomeTexture
      ->InitCubeMap(theCtx, nullptr, mySkydomeAspect.Size(), Image_Format_RGB, false, false);
  }

  // init aspects if needed
  if (myCubeMapParams->TextureSet(theCtx).IsNull())
  {
    myCubeMapParams->Aspect()->SetInteriorStyle(Aspect_IS_SOLID);
    myCubeMapParams->Aspect()->SetFaceCulling(Graphic3d_TypeOfBackfacingModel_DoubleSided);
    myCubeMapParams->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
    myCubeMapParams->Aspect()->SetShaderProgram(theCtx->ShaderManager()->GetBgCubeMapProgram());
    occ::handle<Graphic3d_TextureSet> aTextureSet = new Graphic3d_TextureSet(1);
    myCubeMapParams->Aspect()->SetTextureSet(aTextureSet);
    myCubeMapParams->Aspect()->SetTextureMapOn(true);
    myCubeMapParams->SynchronizeAspects();
  }

  myCubeMapParams->Aspect()->ShaderProgram()->PushVariableInt("uZCoeff", 1);
  myCubeMapParams->Aspect()->ShaderProgram()->PushVariableInt("uYCoeff", 1);

  for (int aSideIter = 0; aSideIter < 6; aSideIter++)
  {
    aProg->SetUniform(theCtx, "uSide", aSideIter);
    theCtx->arbFBO->glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_COLOR_ATTACHMENT0,
                                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + aSideIter,
                                           mySkydomeTexture->TextureId(),
                                           0);
    theCtx->core15->glDrawArrays(GL_TRIANGLES, 0, 3);
  }
  theCtx->arbFBO->glDeleteFramebuffers(1, &anFBO);
  aVBO->Release(theCtx.get());

  myCubeMapParams->TextureSet(theCtx)->ChangeFirst() = mySkydomeTexture;
  theCtx->BindProgram(aPrevProgram);
  theCtx->ResizeViewport(anOldViewport);
  theCtx->arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, aPrevFBO);
}

//=================================================================================================

bool OpenGl_View::checkPBRAvailability() const
{
  return myWorkspace->GetGlContext()->HasPBR() && !myPBREnvironment.IsNull();
}

//=================================================================================================

void OpenGl_View::updatePBREnvironment(const occ::handle<OpenGl_Context>& theCtx)
{
  if (myBackgroundType == Graphic3d_TOB_CUBEMAP && myToUpdateSkydome)
  {
    updateSkydomeBg(theCtx);
  }

  if (myPBREnvState != OpenGl_PBREnvState_CREATED || !myPBREnvRequest)
  {
    myPBREnvRequest = false;
    return;
  }

  myPBREnvRequest = false;

  occ::handle<OpenGl_TextureSet> aGlTextureSet;
  OpenGl_Aspects*                aTmpGlAspects = nullptr;
  if (!myCubeMapIBL.IsNull() && myCubeMapIBL == myCubeMapBackground)
  {
    aGlTextureSet = myCubeMapParams->TextureSet(theCtx);
  }
  else if (!myCubeMapIBL.IsNull())
  {
    myCubeMapIBL->SetMipmapsGeneration(true);

    occ::handle<Graphic3d_AspectFillArea3d> anAspect = new Graphic3d_AspectFillArea3d();
    {
      occ::handle<Graphic3d_TextureSet> aTextureSet = new Graphic3d_TextureSet(myCubeMapIBL);
      anAspect->SetInteriorStyle(Aspect_IS_SOLID);
      anAspect->SetTextureSet(aTextureSet);
      anAspect->SetTextureMapOn(true);
    }

    aTmpGlAspects = new OpenGl_Aspects();
    aTmpGlAspects->SetAspect(anAspect);
    aGlTextureSet = aTmpGlAspects->TextureSet(theCtx);
  }

  if (!aGlTextureSet.IsNull() && !aGlTextureSet->IsEmpty())
  {
    myPBREnvironment->Bake(theCtx,
                           aGlTextureSet->First(),
                           myCubeMapIBL->ZIsInverted(),
                           myCubeMapIBL->IsTopDown(),
                           myRenderParams.PbrEnvBakingDiffNbSamples,
                           myRenderParams.PbrEnvBakingSpecNbSamples,
                           myRenderParams.PbrEnvBakingProbability);
  }
  else
  {
    myPBREnvironment->Clear(theCtx);
  }
  aGlTextureSet.Nullify();
  OpenGl_Element::Destroy(theCtx.get(), aTmpGlAspects);
}
