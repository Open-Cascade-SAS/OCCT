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

#include <stdio.h>
#include <stdlib.h>

#include <OpenGl_GlCore11.hxx>

#include <Aspect_XRSession.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <Image_AlienPixMap.hxx>

#include <NCollection_Mat4.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_FrameStats.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_ArbFBO.hxx>

#include "../Textures/Textures_EnvLUT.pxx"

namespace
{
  //! Format Frame Buffer format for logging messages.
  static TCollection_AsciiString printFboFormat (const Handle(OpenGl_FrameBuffer)& theFbo)
  {
    return TCollection_AsciiString() + theFbo->GetInitVPSizeX() + "x" + theFbo->GetInitVPSizeY() + "@" + theFbo->NbSamples();
  }

  //! Return TRUE if Frame Buffer initialized has failed with the same parameters.
  static bool checkWasFailedFbo (const Handle(OpenGl_FrameBuffer)& theFboToCheck,
                                 Standard_Integer theSizeX,
                                 Standard_Integer theSizeY,
                                 Standard_Integer theNbSamples)
  {
    return !theFboToCheck->IsValid()
        &&  theFboToCheck->GetInitVPSizeX() == theSizeX
        &&  theFboToCheck->GetInitVPSizeY() == theSizeY
        &&  theFboToCheck->NbSamples()      == theNbSamples;
  }

  //! Return TRUE if Frame Buffer initialized has failed with the same parameters.
  static bool checkWasFailedFbo (const Handle(OpenGl_FrameBuffer)& theFboToCheck,
                                 const Handle(OpenGl_FrameBuffer)& theFboRef)
  {
    return checkWasFailedFbo (theFboToCheck, theFboRef->GetVPSizeX(), theFboRef->GetVPSizeY(), theFboRef->NbSamples());
  }
}

//=======================================================================
//function : drawBackground
//purpose  :
//=======================================================================
void OpenGl_View::drawBackground (const Handle(OpenGl_Workspace)& theWorkspace,
                                  Graphic3d_Camera::Projection theProjection)
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();
  const Standard_Boolean wasUsedZBuffer = theWorkspace->SetUseZBuffer (Standard_False);
  if (wasUsedZBuffer)
  {
    aCtx->core11fwd->glDisable (GL_DEPTH_TEST);
  }

  if (myBackgroundType == Graphic3d_TOB_CUBEMAP)
  {
    myCubeMapParams->Aspect()->ShaderProgram()->PushVariableInt ("uZCoeff", myBackgroundCubeMap->ZIsInverted() ? -1 : 1);
    myCubeMapParams->Aspect()->ShaderProgram()->PushVariableInt ("uYCoeff", myBackgroundCubeMap->IsTopDown() ? 1 : -1);
    const OpenGl_Aspects* anOldAspectFace = theWorkspace->SetAspects (myCubeMapParams);

    myBackgrounds[Graphic3d_TOB_CUBEMAP]->Render (theWorkspace, theProjection);

    theWorkspace->SetAspects (anOldAspectFace);
  }
  else if (myBackgroundType == Graphic3d_TOB_GRADIENT
        || myBackgroundType == Graphic3d_TOB_TEXTURE)
  {
    // Drawing background gradient if:
    // - gradient fill type is not Aspect_GFM_NONE and
    // - either background texture is no specified or it is drawn in Aspect_FM_CENTERED mode
    if (myBackgrounds[Graphic3d_TOB_GRADIENT]->IsDefined()
      && (!myTextureParams->Aspect()->ToMapTexture()
        || myBackgrounds[Graphic3d_TOB_TEXTURE]->TextureFillMethod() == Aspect_FM_CENTERED
        || myBackgrounds[Graphic3d_TOB_TEXTURE]->TextureFillMethod() == Aspect_FM_NONE))
    {
      myBackgrounds[Graphic3d_TOB_GRADIENT]->Render(theWorkspace, theProjection);
    }

    // Drawing background image if it is defined
    // (texture is defined and fill type is not Aspect_FM_NONE)
    if (myBackgrounds[Graphic3d_TOB_TEXTURE]->IsDefined()
      && myTextureParams->Aspect()->ToMapTexture())
    {
      aCtx->core11fwd->glDisable (GL_BLEND);

      const OpenGl_Aspects* anOldAspectFace = theWorkspace->SetAspects (myTextureParams);
      myBackgrounds[Graphic3d_TOB_TEXTURE]->Render (theWorkspace, theProjection);
      theWorkspace->SetAspects (anOldAspectFace);
    }
  }

  if (wasUsedZBuffer)
  {
    theWorkspace->SetUseZBuffer (Standard_True);
    aCtx->core11fwd->glEnable (GL_DEPTH_TEST);
  }
}

//=======================================================================
//function : Redraw
//purpose  :
//=======================================================================
void OpenGl_View::Redraw()
{
  const Standard_Boolean wasDisabledMSAA = myToDisableMSAA;
  const Standard_Boolean hadFboBlit      = myHasFboBlit;
  if (myRenderParams.Method == Graphic3d_RM_RAYTRACING
  && !myCaps->vboDisable
  && !myCaps->keepArrayData)
  {
    // caps are shared across all views, thus we need to invalidate all of them
    // if (myWasRedrawnGL) { myStructureManager->SetDeviceLost(); }
    myDriver->setDeviceLost();
    myCaps->keepArrayData = Standard_True;
  }

  if (!myWorkspace->Activate())
  {
    return;
  }

  // implicitly disable VSync when using HMD composer (can be mirrored in window for debugging)
  myWindow->SetSwapInterval (IsActiveXR());

  ++myFrameCounter;
  const Graphic3d_StereoMode   aStereoMode  = myRenderParams.StereoMode;
  Graphic3d_Camera::Projection aProjectType = myCamera->ProjectionType();
  const Handle(OpenGl_Context)& aCtx        = myWorkspace->GetGlContext();
  aCtx->FrameStats()->FrameStart (myWorkspace->View(), false);
  aCtx->SetLineFeather (myRenderParams.LineFeather);

  const Standard_Integer anSRgbState = aCtx->ToRenderSRGB() ? 1 : 0;
  if (mySRgbState != -1
   && mySRgbState != anSRgbState)
  {
    releaseSrgbResources (aCtx);
    initTextureEnv (aCtx);
  }
  mySRgbState = anSRgbState;
  aCtx->ShaderManager()->UpdateSRgbState();

  // release pending GL resources
  aCtx->ReleaseDelayed();

  // fetch OpenGl context state
  aCtx->FetchState();

  OpenGl_FrameBuffer* aFrameBuffer = myFBO.get();
  bool toSwap = aCtx->IsRender()
            && !aCtx->caps->buffersNoSwap
            &&  aFrameBuffer == NULL
            &&  (!IsActiveXR() || myRenderParams.ToMirrorComposer);

  Standard_Integer aSizeX = myWindow->Width();
  Standard_Integer aSizeY = myWindow->Height();
  if (aFrameBuffer != NULL)
  {
    aSizeX = aFrameBuffer->GetVPSizeX();
    aSizeY = aFrameBuffer->GetVPSizeY();
  }
  else if (IsActiveXR())
  {
    aSizeX = myXRSession->RecommendedViewport().x();
    aSizeY = myXRSession->RecommendedViewport().y();
  }

  const Standard_Integer aRendSizeX = Standard_Integer(myRenderParams.RenderResolutionScale * aSizeX + 0.5f);
  const Standard_Integer aRendSizeY = Standard_Integer(myRenderParams.RenderResolutionScale * aSizeY + 0.5f);
  if (aSizeX < 1
   || aSizeY < 1
   || aRendSizeX < 1
   || aRendSizeY < 1)
  {
    myBackBufferRestored = Standard_False;
    myIsImmediateDrawn   = Standard_False;
    return;
  }

  // determine multisampling parameters
  Standard_Integer aNbSamples = !myToDisableMSAA && aSizeX == aRendSizeX
                              ? Max (Min (myRenderParams.NbMsaaSamples, aCtx->MaxMsaaSamples()), 0)
                              : 0;
  if (aNbSamples != 0)
  {
    aNbSamples = OpenGl_Context::GetPowerOfTwo (aNbSamples, aCtx->MaxMsaaSamples());
  }

  bool toUseOit = myRenderParams.TransparencyMethod == Graphic3d_RTM_BLEND_OIT
               && checkOitCompatibility (aCtx, aNbSamples > 0);

  const bool toInitImmediateFbo = myTransientDrawToFront
                               && (!aCtx->caps->useSystemBuffer || (toUseOit && HasImmediateStructures()));

  if ( aFrameBuffer == NULL
   && !aCtx->DefaultFrameBuffer().IsNull()
   &&  aCtx->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = aCtx->DefaultFrameBuffer().operator->();
  }

  if (myHasFboBlit
   && (myTransientDrawToFront
    || aProjectType == Graphic3d_Camera::Projection_Stereo
    || aNbSamples != 0
    || toUseOit
    || aSizeX != aRendSizeX))
  {
    if (myMainSceneFbos[0]->GetVPSizeX() != aRendSizeX
     || myMainSceneFbos[0]->GetVPSizeY() != aRendSizeY
     || myMainSceneFbos[0]->NbSamples()  != aNbSamples)
    {
      if (!myTransientDrawToFront)
      {
        myImmediateSceneFbos[0]->Release (aCtx.operator->());
        myImmediateSceneFbos[1]->Release (aCtx.operator->());
        myImmediateSceneFbos[0]->ChangeViewport (0, 0);
        myImmediateSceneFbos[1]->ChangeViewport (0, 0);
      }

      // prepare FBOs containing main scene
      // for further blitting and rendering immediate presentations on top
      if (aCtx->core20fwd != NULL)
      {
        const bool wasFailedMain0 = checkWasFailedFbo (myMainSceneFbos[0], aRendSizeX, aRendSizeY, aNbSamples);
        if (!myMainSceneFbos[0]->Init (aCtx, aRendSizeX, aRendSizeY, myFboColorFormat, myFboDepthFormat, aNbSamples)
         && !wasFailedMain0)
        {
          TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Main FBO "
                                          + printFboFormat (myMainSceneFbos[0]) + " initialization has failed";
          aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
        }
      }
    }
    if (myMainSceneFbos[0]->IsValid() && (toInitImmediateFbo || myImmediateSceneFbos[0]->IsValid()))
    {
      const bool wasFailedImm0 = checkWasFailedFbo (myImmediateSceneFbos[0], myMainSceneFbos[0]);
      if (!myImmediateSceneFbos[0]->InitLazy (aCtx, *myMainSceneFbos[0])
       && !wasFailedImm0)
      {
        TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Immediate FBO "
                                        + printFboFormat (myImmediateSceneFbos[0]) + " initialization has failed";
        aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
      }
    }
  }
  else
  {
    myMainSceneFbos     [0]->Release (aCtx.operator->());
    myMainSceneFbos     [1]->Release (aCtx.operator->());
    myImmediateSceneFbos[0]->Release (aCtx.operator->());
    myImmediateSceneFbos[1]->Release (aCtx.operator->());
    myXrSceneFbo           ->Release (aCtx.operator->());
    myMainSceneFbos     [0]->ChangeViewport (0, 0);
    myMainSceneFbos     [1]->ChangeViewport (0, 0);
    myImmediateSceneFbos[0]->ChangeViewport (0, 0);
    myImmediateSceneFbos[1]->ChangeViewport (0, 0);
    myXrSceneFbo           ->ChangeViewport (0, 0);
  }

  bool hasXRBlitFbo = false;
  if (aProjectType == Graphic3d_Camera::Projection_Stereo
   && IsActiveXR()
   && myMainSceneFbos[0]->IsValid())
  {
    if (aNbSamples != 0
     || aSizeX != aRendSizeX)
    {
      hasXRBlitFbo = myXrSceneFbo->InitLazy (aCtx, aSizeX, aSizeY, myFboColorFormat, myFboDepthFormat, 0);
      if (!hasXRBlitFbo)
      {
        TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! VR FBO "
                                        + printFboFormat (myXrSceneFbo) + " initialization has failed";
        aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
      }
    }
  }
  else if (aProjectType == Graphic3d_Camera::Projection_Stereo
        && myMainSceneFbos[0]->IsValid())
  {
    const bool wasFailedMain1 = checkWasFailedFbo (myMainSceneFbos[1], myMainSceneFbos[0]);
    if (!myMainSceneFbos[1]->InitLazy (aCtx, *myMainSceneFbos[0])
     && !wasFailedMain1)
    {
      TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Main FBO (second) "
                                      + printFboFormat (myMainSceneFbos[1]) + " initialization has failed";
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
    }
    if (!myMainSceneFbos[1]->IsValid())
    {
      // no enough memory?
      aProjectType = Graphic3d_Camera::Projection_Perspective;
    }
    else if (!myTransientDrawToFront)
    {
      //
    }
    else if (!aCtx->HasStereoBuffers() || aStereoMode != Graphic3d_StereoMode_QuadBuffer)
    {
      const bool wasFailedImm0 = checkWasFailedFbo (myImmediateSceneFbos[0], myMainSceneFbos[0]);
      const bool wasFailedImm1 = checkWasFailedFbo (myImmediateSceneFbos[1], myMainSceneFbos[0]);
      if (!myImmediateSceneFbos[0]->InitLazy (aCtx, *myMainSceneFbos[0])
       && !wasFailedImm0)
      {
        TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Immediate FBO (first) "
                                        + printFboFormat (myImmediateSceneFbos[0]) + " initialization has failed";
        aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
      }
      if (!myImmediateSceneFbos[1]->InitLazy (aCtx, *myMainSceneFbos[0])
       && !wasFailedImm1)
      {
        TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "Error! Immediate FBO (first) "
                                        + printFboFormat (myImmediateSceneFbos[1]) + " initialization has failed";
        aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
      }
      if (!myImmediateSceneFbos[0]->IsValid()
       || !myImmediateSceneFbos[1]->IsValid())
      {
        aProjectType = Graphic3d_Camera::Projection_Perspective;
      }
    }
  }
  if (!hasXRBlitFbo)
  {
    myXrSceneFbo->Release (aCtx.get());
    myXrSceneFbo->ChangeViewport (0, 0);
  }

  // process PBR environment
  if (myShadingModel == Graphic3d_TOSM_PBR
   || myShadingModel == Graphic3d_TOSM_PBR_FACET)
  {
    if (!myPBREnvironment.IsNull()
      && myPBREnvironment->SizesAreDifferent (myRenderParams.PbrEnvPow2Size,
                                              myRenderParams.PbrEnvSpecMapNbLevels))
    {
      myPBREnvironment->Release (aCtx.get());
      myPBREnvironment.Nullify();
      myPBREnvState = OpenGl_PBREnvState_NONEXISTENT;
      myPBREnvRequest = OpenGl_PBREnvRequest_BAKE;
      ++myLightsRevision;
    }

    if (myPBREnvState == OpenGl_PBREnvState_NONEXISTENT
     && aCtx->HasPBR())
    {
      myPBREnvironment = OpenGl_PBREnvironment::Create (aCtx, myRenderParams.PbrEnvPow2Size, myRenderParams.PbrEnvSpecMapNbLevels);
      myPBREnvState = myPBREnvironment.IsNull() ? OpenGl_PBREnvState_UNAVAILABLE : OpenGl_PBREnvState_CREATED;
      if (myPBREnvState == OpenGl_PBREnvState_CREATED)
      {
        Handle(OpenGl_Texture) anEnvLUT;
        static const TCollection_AsciiString THE_SHARED_ENV_LUT_KEY("EnvLUT");
        if (!aCtx->GetResource (THE_SHARED_ENV_LUT_KEY, anEnvLUT))
        {
          Handle(Graphic3d_TextureParams) aParams = new Graphic3d_TextureParams();
          aParams->SetFilter (Graphic3d_TOTF_BILINEAR);
          aParams->SetRepeat (Standard_False);
          aParams->SetTextureUnit (aCtx->PBREnvLUTTexUnit());
          anEnvLUT = new OpenGl_Texture(THE_SHARED_ENV_LUT_KEY, aParams);
          Handle(Image_PixMap) aPixMap = new Image_PixMap();
          aPixMap->InitWrapper (Image_Format_RGF, (Standard_Byte*)Textures_EnvLUT, Textures_EnvLUTSize, Textures_EnvLUTSize);
          OpenGl_TextureFormat aTexFormat = OpenGl_TextureFormat::FindFormat (aCtx, aPixMap->Format(), false);
        #if defined(GL_ES_VERSION_2_0)
          // GL_RG32F is not texture-filterable format on OpenGL ES without OES_texture_float_linear extension.
          // GL_RG16F is texture-filterable since OpenGL ES 3.0 and can be initialized from 32-bit floats.
          // Note that it is expected that GL_RG16F has enough precision for this table, so that it can be used also on desktop OpenGL.
          //if (!aCtx->hasTexFloatLinear)
          aTexFormat.SetInternalFormat (GL_RG16F);
        #endif
          if (!aTexFormat.IsValid()
           || !anEnvLUT->Init (aCtx, aTexFormat, Graphic3d_Vec2i((Standard_Integer)Textures_EnvLUTSize), Graphic3d_TOT_2D, aPixMap.get()))
          {
            aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, "Failed allocation of LUT for PBR");
            anEnvLUT.Nullify();
          }
          aCtx->ShareResource (THE_SHARED_ENV_LUT_KEY, anEnvLUT);
        }
        if (!anEnvLUT.IsNull())
        {
          anEnvLUT->Bind (aCtx);
        }
        myWorkspace->ApplyAspects();
      }
    }
    processPBREnvRequest (aCtx);
  }

  // create color and coverage accumulation buffers required for OIT algorithm
  if (toUseOit)
  {
    Standard_Integer anFboIt = 0;
    for (; anFboIt < 2; ++anFboIt)
    {
      Handle(OpenGl_FrameBuffer)& aMainSceneFbo          = myMainSceneFbos        [anFboIt];
      Handle(OpenGl_FrameBuffer)& aMainSceneFboOit       = myMainSceneFbosOit     [anFboIt];
      Handle(OpenGl_FrameBuffer)& anImmediateSceneFbo    = myImmediateSceneFbos   [anFboIt];
      Handle(OpenGl_FrameBuffer)& anImmediateSceneFboOit = myImmediateSceneFbosOit[anFboIt];
      if (aMainSceneFbo->IsValid()
       && (aMainSceneFboOit->GetVPSizeX() != aRendSizeX
        || aMainSceneFboOit->GetVPSizeY() != aRendSizeY
        || aMainSceneFboOit->NbSamples()  != aNbSamples))
      {
        Standard_Integer aColorConfig = 0;
        for (;;) // seemly responding to driver limitation (GL_FRAMEBUFFER_UNSUPPORTED)
        {
          if (myFboOitColorConfig.IsEmpty())
          {
            if (!chooseOitColorConfiguration (aCtx, aColorConfig++, myFboOitColorConfig))
            {
              break;
            }
          }
          if (aMainSceneFboOit->Init (aCtx, aRendSizeX, aRendSizeY, myFboOitColorConfig, aMainSceneFbo->DepthStencilTexture(), aNbSamples))
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
        aMainSceneFboOit->Release (aCtx.operator->());
        aMainSceneFboOit->ChangeViewport (0, 0);
      }

      if (anImmediateSceneFbo->IsValid()
       && (anImmediateSceneFboOit->GetVPSizeX() != aRendSizeX
        || anImmediateSceneFboOit->GetVPSizeY() != aRendSizeY
        || anImmediateSceneFboOit->NbSamples()  != aNbSamples))
      {
        if (!anImmediateSceneFboOit->Init (aCtx, aRendSizeX, aRendSizeY, myFboOitColorConfig,
                                           anImmediateSceneFbo->DepthStencilTexture(), aNbSamples))
        {
          break;
        }
      }
      else if (!anImmediateSceneFbo->IsValid())
      {
        anImmediateSceneFboOit->Release (aCtx.operator->());
        anImmediateSceneFboOit->ChangeViewport (0, 0);
      }
    }
    if (anFboIt == 0) // only the first OIT framebuffer is mandatory
    {
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "Initialization of float texture framebuffer for use with\n"
                         "  blended order-independent transparency rendering algorithm has failed.\n"
                         "  Blended order-independent transparency will not be available.\n");
      if (aNbSamples > 0)
      {
        myToDisableOITMSAA = Standard_True;
      }
      else
      {
        myToDisableOIT     = Standard_True;
      }
      toUseOit = false;
    }
  }
  if (!toUseOit && myMainSceneFbosOit[0]->IsValid())
  {
    myMainSceneFbosOit     [0]->Release (aCtx.operator->());
    myMainSceneFbosOit     [1]->Release (aCtx.operator->());
    myImmediateSceneFbosOit[0]->Release (aCtx.operator->());
    myImmediateSceneFbosOit[1]->Release (aCtx.operator->());
    myMainSceneFbosOit     [0]->ChangeViewport (0, 0);
    myMainSceneFbosOit     [1]->ChangeViewport (0, 0);
    myImmediateSceneFbosOit[0]->ChangeViewport (0, 0);
    myImmediateSceneFbosOit[1]->ChangeViewport (0, 0);
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    OpenGl_FrameBuffer* aMainFbos[2] =
    {
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL,
      myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : NULL
    };
    OpenGl_FrameBuffer* aMainFbosOit[2] =
    {
      myMainSceneFbosOit[0]->IsValid() ? myMainSceneFbosOit[0].operator->() : NULL,
      myMainSceneFbosOit[1]->IsValid() ? myMainSceneFbosOit[1].operator->() :
        myMainSceneFbosOit[0]->IsValid() ? myMainSceneFbosOit[0].operator->() : NULL
    };

    OpenGl_FrameBuffer* anImmFbos[2] =
    {
      myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : NULL,
      myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : NULL
    };
    OpenGl_FrameBuffer* anImmFbosOit[2] =
    {
      myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : NULL,
      myImmediateSceneFbosOit[1]->IsValid() ? myImmediateSceneFbosOit[1].operator->() :
        myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : NULL
    };

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
      anImmFbos   [0] = aMainFbos   [0];
      anImmFbos   [1] = aMainFbos   [1];
      anImmFbosOit[0] = aMainFbosOit[0];
      anImmFbosOit[1] = aMainFbosOit[1];
    }
    else if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
          || aStereoMode == Graphic3d_StereoMode_QuadBuffer)
    {
      anImmFbos   [0] = NULL;
      anImmFbos   [1] = NULL;
      anImmFbosOit[0] = NULL;
      anImmFbosOit[1] = NULL;
    }

  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT : GL_BACK);
  #endif
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         aMainFbos[0] != NULL ? myRenderParams.RenderResolutionScale : 1.0f);

    redraw (Graphic3d_Camera::Projection_MonoLeftEye, aMainFbos[0], aMainFbosOit[0]);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT : GL_BACK);
  #endif
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         anImmFbos[0] != NULL ? myRenderParams.RenderResolutionScale : 1.0f);
    if (!redrawImmediate (Graphic3d_Camera::Projection_MonoLeftEye, aMainFbos[0], anImmFbos[0], anImmFbosOit[0]))
    {
      toSwap = false;
    }
    else if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip && toSwap)
    {
      aCtx->SwapBuffers();
    }

    if (IsActiveXR())
    {
      // push Left frame to HMD display composer
      OpenGl_FrameBuffer* anXRFbo = hasXRBlitFbo ? myXrSceneFbo.get() : aMainFbos[0];
      if (anXRFbo != aMainFbos[0])
      {
        blitBuffers (aMainFbos[0], anXRFbo); // resize or resolve MSAA samples
      }
    #if !defined(GL_ES_VERSION_2_0)
      const Aspect_GraphicsLibrary aGraphicsLib = Aspect_GraphicsLibrary_OpenGL;
    #else
      const Aspect_GraphicsLibrary aGraphicsLib = Aspect_GraphicsLibrary_OpenGLES;
    #endif
      myXRSession->SubmitEye ((void* )(size_t )anXRFbo->ColorTexture()->TextureId(),
                              aGraphicsLib, Aspect_ColorSpace_sRGB, Aspect_Eye_Left);
    }

  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_RIGHT : GL_BACK);
  #endif
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         aMainFbos[1] != NULL ? myRenderParams.RenderResolutionScale : 1.0f);

    redraw (Graphic3d_Camera::Projection_MonoRightEye, aMainFbos[1], aMainFbosOit[1]);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         anImmFbos[1] != NULL ? myRenderParams.RenderResolutionScale : 1.0f);
    if (!redrawImmediate (Graphic3d_Camera::Projection_MonoRightEye, aMainFbos[1], anImmFbos[1], anImmFbosOit[1]))
    {
      toSwap = false;
    }

    if (IsActiveXR())
    {
      // push Right frame to HMD display composer
      OpenGl_FrameBuffer* anXRFbo = hasXRBlitFbo ? myXrSceneFbo.get() : aMainFbos[1];
      if (anXRFbo != aMainFbos[1])
      {
        blitBuffers (aMainFbos[1], anXRFbo); // resize or resolve MSAA samples
      }
    #if !defined(GL_ES_VERSION_2_0)
      const Aspect_GraphicsLibrary aGraphicsLib = Aspect_GraphicsLibrary_OpenGL;
    #else
      const Aspect_GraphicsLibrary aGraphicsLib = Aspect_GraphicsLibrary_OpenGLES;
    #endif
      myXRSession->SubmitEye ((void* )(size_t )anXRFbo->ColorTexture()->TextureId(),
                              aGraphicsLib, Aspect_ColorSpace_sRGB, Aspect_Eye_Right);
      ::glFinish();

      if (myRenderParams.ToMirrorComposer)
      {
        blitBuffers (anXRFbo, aFrameBuffer, myToFlipOutput);
      }
    }
    else if (anImmFbos[0] != NULL)
    {
      aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(), 1.0f);
      drawStereoPair (aFrameBuffer);
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo    = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : aFrameBuffer;
    OpenGl_FrameBuffer* aMainFboOit = myMainSceneFbosOit[0]->IsValid() ? myMainSceneFbosOit[0].operator->() : NULL;
    OpenGl_FrameBuffer* anImmFbo    = aFrameBuffer;
    OpenGl_FrameBuffer* anImmFboOit = NULL;
    if (!myTransientDrawToFront)
    {
      anImmFbo    = aMainFbo;
      anImmFboOit = aMainFboOit;
    }
    else if (myImmediateSceneFbos[0]->IsValid())
    {
      anImmFbo    = myImmediateSceneFbos[0].operator->();
      anImmFboOit = myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : NULL;
    }

  #if !defined(GL_ES_VERSION_2_0)
    if (aMainFbo == NULL)
    {
      aCtx->SetReadDrawBuffer (GL_BACK);
    }
  #endif
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         aMainFbo != aFrameBuffer ? myRenderParams.RenderResolutionScale : 1.0f);

    redraw (aProjectType, aMainFbo, aMainFboOit);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         anImmFbo != aFrameBuffer ? myRenderParams.RenderResolutionScale : 1.0f);
    if (!redrawImmediate (aProjectType, aMainFbo, anImmFbo, anImmFboOit))
    {
      toSwap = false;
    }

    if (anImmFbo != NULL
     && anImmFbo != aFrameBuffer)
    {
      blitBuffers (anImmFbo, aFrameBuffer, myToFlipOutput);
    }
  }

  if (myRenderParams.Method == Graphic3d_RM_RAYTRACING
   && myRenderParams.IsGlobalIlluminationEnabled)
  {
    myAccumFrames++;
  }

  // bind default FBO
  bindDefaultFbo();

  if (wasDisabledMSAA != myToDisableMSAA
   || hadFboBlit      != myHasFboBlit)
  {
    // retry on error
    Redraw();
  }

  // reset state for safety
  aCtx->BindProgram (Handle(OpenGl_ShaderProgram)());
  if (aCtx->caps->ffpEnable)
  {
    aCtx->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
  }

  // Swap the buffers
  if (toSwap)
  {
    aCtx->SwapBuffers();
    if (!myMainSceneFbos[0]->IsValid())
    {
      myBackBufferRestored = Standard_False;
    }
  }
  else
  {
    aCtx->core11fwd->glFlush();
  }

  // reset render mode state
  aCtx->FetchState();
  aCtx->FrameStats()->FrameEnd (myWorkspace->View(), false);

  myWasRedrawnGL = Standard_True;
}

// =======================================================================
// function : RedrawImmediate
// purpose  :
// =======================================================================
void OpenGl_View::RedrawImmediate()
{
  if (!myWorkspace->Activate())
    return;

  // no special handling of HMD display, since it will force full Redraw() due to no frame caching (myBackBufferRestored)
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  if (!myTransientDrawToFront
   || !myBackBufferRestored
   || (aCtx->caps->buffersNoSwap && !myMainSceneFbos[0]->IsValid()))
  {
    Redraw();
    return;
  }

  const Graphic3d_StereoMode   aStereoMode  = myRenderParams.StereoMode;
  Graphic3d_Camera::Projection aProjectType = myCamera->ProjectionType();
  OpenGl_FrameBuffer*          aFrameBuffer = myFBO.get();
  aCtx->FrameStats()->FrameStart (myWorkspace->View(), true);

  if ( aFrameBuffer == NULL
   && !aCtx->DefaultFrameBuffer().IsNull()
   &&  aCtx->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = aCtx->DefaultFrameBuffer().operator->();
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    if (myMainSceneFbos[0]->IsValid()
    && !myMainSceneFbos[1]->IsValid())
    {
      aProjectType = Graphic3d_Camera::Projection_Perspective;
    }
  }

  bool toSwap = false;
  if (aProjectType == Graphic3d_Camera::Projection_Stereo)
  {
    OpenGl_FrameBuffer* aMainFbos[2] =
    {
      myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL,
      myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : NULL
    };
    OpenGl_FrameBuffer* anImmFbos[2] =
    {
      myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : NULL,
      myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : NULL
    };
    OpenGl_FrameBuffer* anImmFbosOit[2] =
    {
      myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : NULL,
      myImmediateSceneFbosOit[1]->IsValid() ? myImmediateSceneFbosOit[1].operator->() :
        myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : NULL
    };
    if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
     || aStereoMode == Graphic3d_StereoMode_QuadBuffer)
    {
      anImmFbos[0]    = NULL;
      anImmFbos[1]    = NULL;
      anImmFbosOit[0] = NULL;
      anImmFbosOit[1] = NULL;
    }

    if (aCtx->arbFBO != NULL)
    {
      aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (anImmFbos[0] == NULL)
    {
      aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT : GL_BACK);
    }
  #endif

    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         anImmFbos[0] != NULL ? myRenderParams.RenderResolutionScale : 1.0f);
    toSwap = redrawImmediate (Graphic3d_Camera::Projection_MonoLeftEye,
                              aMainFbos[0],
                              anImmFbos[0],
                              anImmFbosOit[0],
                              Standard_True) || toSwap;
    if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
    &&  toSwap
    && !aCtx->caps->buffersNoSwap)
    {
      aCtx->SwapBuffers();
    }

    if (aCtx->arbFBO != NULL)
    {
      aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (anImmFbos[1] == NULL)
    {
      aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_RIGHT : GL_BACK);
    }
  #endif
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         anImmFbos[1] != NULL ? myRenderParams.RenderResolutionScale : 1.0f);
    toSwap = redrawImmediate (Graphic3d_Camera::Projection_MonoRightEye,
                              aMainFbos[1],
                              anImmFbos[1],
                              anImmFbosOit[1],
                              Standard_True) || toSwap;
    if (anImmFbos[0] != NULL)
    {
      drawStereoPair (aFrameBuffer);
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL;
    OpenGl_FrameBuffer* anImmFbo = aFrameBuffer;
    OpenGl_FrameBuffer* anImmFboOit = NULL;
    if (myImmediateSceneFbos[0]->IsValid())
    {
      anImmFbo    = myImmediateSceneFbos[0].operator->();
      anImmFboOit = myImmediateSceneFbosOit[0]->IsValid() ? myImmediateSceneFbosOit[0].operator->() : NULL;
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (aMainFbo == NULL)
    {
      aCtx->SetReadDrawBuffer (GL_BACK);
    }
  #endif
    aCtx->SetResolution (myRenderParams.Resolution, myRenderParams.ResolutionRatio(),
                         anImmFbo != aFrameBuffer ? myRenderParams.RenderResolutionScale : 1.0f);
    toSwap = redrawImmediate (aProjectType,
                              aMainFbo,
                              anImmFbo,
                              anImmFboOit,
                              Standard_True) || toSwap;
    if (anImmFbo != NULL
     && anImmFbo != aFrameBuffer)
    {
      blitBuffers (anImmFbo, aFrameBuffer, myToFlipOutput);
    }
  }

  // bind default FBO
  bindDefaultFbo();

  // reset state for safety
  aCtx->BindProgram (Handle(OpenGl_ShaderProgram)());
  if (aCtx->caps->ffpEnable)
  {
    aCtx->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
  }

  if (toSwap && !aCtx->caps->buffersNoSwap)
  {
    aCtx->SwapBuffers();
  }
  else
  {
    aCtx->core11fwd->glFlush();
  }
  aCtx->FrameStats()->FrameEnd (myWorkspace->View(), true);

  myWasRedrawnGL = Standard_True;
}

// =======================================================================
// function : redraw
// purpose  :
// =======================================================================
void OpenGl_View::redraw (const Graphic3d_Camera::Projection theProjection,
                          OpenGl_FrameBuffer*                theReadDrawFbo,
                          OpenGl_FrameBuffer*                theOitAccumFbo)
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  if (theReadDrawFbo != NULL)
  {
    theReadDrawFbo->BindBuffer    (aCtx);
    theReadDrawFbo->SetupViewport (aCtx);
  }
  else
  {
    const Standard_Integer aViewport[4] = { 0, 0, myWindow->Width(), myWindow->Height() };
    aCtx->ResizeViewport (aViewport);
  }

  // request reset of material
  aCtx->ShaderManager()->UpdateMaterialState();

  myWorkspace->UseZBuffer()    = Standard_True;
  myWorkspace->UseDepthWrite() = Standard_True;
  GLbitfield toClear = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
  glDepthFunc (GL_LEQUAL);
  glDepthMask (GL_TRUE);
  glEnable (GL_DEPTH_TEST);

#if !defined(GL_ES_VERSION_2_0)
  glClearDepth (1.0);
#else
  glClearDepthf (1.0f);
#endif

  const OpenGl_Vec4 aBgColor = aCtx->Vec4FromQuantityColor (myBgColor);
  glClearColor (aBgColor.r(), aBgColor.g(), aBgColor.b(), 0.0f);

  glClear (toClear);

  render (theProjection, theReadDrawFbo, theOitAccumFbo, Standard_False);
}

// =======================================================================
// function : redrawMonoImmediate
// purpose  :
// =======================================================================
bool OpenGl_View::redrawImmediate (const Graphic3d_Camera::Projection theProjection,
                                   OpenGl_FrameBuffer*                theReadFbo,
                                   OpenGl_FrameBuffer*                theDrawFbo,
                                   OpenGl_FrameBuffer*                theOitAccumFbo,
                                   const Standard_Boolean             theIsPartialUpdate)
{
  const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext();
  GLboolean toCopyBackToFront = GL_FALSE;
  if (theDrawFbo == theReadFbo
   && theDrawFbo != NULL
   && theDrawFbo->IsValid())
  {
    myBackBufferRestored = Standard_False;
    theDrawFbo->BindBuffer (aCtx);
  }
  else if (theReadFbo != NULL
        && theReadFbo->IsValid()
        && aCtx->IsRender())
  {
    if (!blitBuffers (theReadFbo, theDrawFbo))
    {
      return true;
    }
  }
  else if (theDrawFbo == NULL)
  {
  #if !defined(GL_ES_VERSION_2_0)
    aCtx->core11fwd->glGetBooleanv (GL_DOUBLEBUFFER, &toCopyBackToFront);
  #endif
    if (toCopyBackToFront
     && myTransientDrawToFront)
    {
      if (!HasImmediateStructures()
       && !theIsPartialUpdate)
      {
        // prefer Swap Buffers within Redraw in compatibility mode (without FBO)
        return true;
      }
      if (!copyBackToFront())
      {
        toCopyBackToFront    = GL_FALSE;
        myBackBufferRestored = Standard_False;
      }
    }
    else
    {
      toCopyBackToFront    = GL_FALSE;
      myBackBufferRestored = Standard_False;
    }
  }
  else
  {
    myBackBufferRestored = Standard_False;
  }
  myIsImmediateDrawn = Standard_True;

  myWorkspace->UseZBuffer()    = Standard_True;
  myWorkspace->UseDepthWrite() = Standard_True;
  glDepthFunc (GL_LEQUAL);
  glDepthMask (GL_TRUE);
  glEnable (GL_DEPTH_TEST);
#if !defined(GL_ES_VERSION_2_0)
  glClearDepth (1.0);
#else
  glClearDepthf (1.0f);
#endif

  render (theProjection, theDrawFbo, theOitAccumFbo, Standard_True);

  return !toCopyBackToFront;
}

//=======================================================================
//function : Render
//purpose  :
//=======================================================================
void OpenGl_View::render (Graphic3d_Camera::Projection theProjection,
                          OpenGl_FrameBuffer*          theOutputFBO,
                          OpenGl_FrameBuffer*          theOitAccumFbo,
                          const Standard_Boolean       theToDrawImmediate)
{
  // ==================================
  //      Step 1: Prepare for render
  // ==================================

  const Handle(OpenGl_Context)& aContext = myWorkspace->GetGlContext();
  aContext->SetAllowSampleAlphaToCoverage (myRenderParams.ToEnableAlphaToCoverage
                                        && theOutputFBO != NULL
                                        && theOutputFBO->NbSamples() != 0);

#if !defined(GL_ES_VERSION_2_0)
  // Disable current clipping planes
  if (aContext->core11 != NULL)
  {
    const Standard_Integer aMaxPlanes = aContext->MaxClipPlanes();
    for (Standard_Integer aClipPlaneId = GL_CLIP_PLANE0; aClipPlaneId < GL_CLIP_PLANE0 + aMaxPlanes; ++aClipPlaneId)
    {
      aContext->core11fwd->glDisable (aClipPlaneId);
    }
  }
#endif

  // update states of OpenGl_BVHTreeSelector (frustum culling algorithm);
  // note that we pass here window dimensions ignoring Graphic3d_RenderingParams::RenderResolutionScale
  myBVHSelector.SetViewVolume (myCamera);
  myBVHSelector.SetViewportSize (myWindow->Width(), myWindow->Height(), myRenderParams.ResolutionRatio());
  myBVHSelector.CacheClipPtsProjections();

  const Handle(OpenGl_ShaderManager)& aManager = aContext->ShaderManager();
  const Handle(Graphic3d_LightSet)&   aLights  = myShadingModel == Graphic3d_TOSM_UNLIT ? myNoShadingLight : myLights;
  Standard_Size aLightsRevision = 0;
  if (!aLights.IsNull())
  {
    aLightsRevision = aLights->UpdateRevision();
  }
  if (StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index()) != myLastLightSourceState
   || aLightsRevision != myLightsRevision)
  {
    myLightsRevision = aLightsRevision;
    aManager->UpdateLightSourceStateTo (aLights, SpecIBLMapLevels());
    myLastLightSourceState = StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index());
  }

  // Update matrices if camera has changed.
  Graphic3d_WorldViewProjState aWVPState = myCamera->WorldViewProjState();
  if (myWorldViewProjState != aWVPState)
  {
    myAccumFrames = 0;
    myWorldViewProjState = aWVPState;
  }

  myLocalOrigin.SetCoord (0.0, 0.0, 0.0);
  aContext->SetCamera (myCamera);
  if (aManager->ModelWorldState().Index() == 0)
  {
    aContext->ShaderManager()->UpdateModelWorldStateTo (OpenGl_Mat4());
  }

  // ====================================
  //      Step 2: Redraw background
  // ====================================

  // Render background
  if (!theToDrawImmediate)
  {
    drawBackground (myWorkspace, theProjection);
  }

#if !defined(GL_ES_VERSION_2_0)
  // Switch off lighting by default
  if (aContext->core11 != NULL
   && aContext->caps->ffpEnable)
  {
    glDisable(GL_LIGHTING);
  }
#endif

  // =================================
  //      Step 3: Redraw main plane
  // =================================

  // Setup face culling
  GLboolean isCullFace = GL_FALSE;
  if (myBackfacing != Graphic3d_TOBM_AUTOMATIC)
  {
    isCullFace = glIsEnabled (GL_CULL_FACE);
    if (myBackfacing == Graphic3d_TOBM_DISABLE)
    {
      glEnable (GL_CULL_FACE);
      glCullFace (GL_BACK);
    }
    else
      glDisable (GL_CULL_FACE);
  }

#if !defined(GL_ES_VERSION_2_0)
  // if the view is scaled normal vectors are scaled to unit
  // length for correct displaying of shaded objects
  const gp_Pnt anAxialScale = aContext->Camera()->AxialScale();
  if (anAxialScale.X() != 1.F ||
      anAxialScale.Y() != 1.F ||
      anAxialScale.Z() != 1.F)
  {
    aContext->SetGlNormalizeEnabled (Standard_True);
  }
  else
  {
    aContext->SetGlNormalizeEnabled (Standard_False);
  }
#endif

  aManager->SetShadingModel (OpenGl_ShaderManager::PBRShadingModelFallback (myShadingModel, checkPBRAvailability()));

  // Redraw 3d scene
  if (theProjection == Graphic3d_Camera::Projection_MonoLeftEye)
  {
    aContext->ProjectionState.SetCurrent (aContext->Camera()->ProjectionStereoLeftF());
    aContext->ApplyProjectionMatrix();
  }
  else if (theProjection == Graphic3d_Camera::Projection_MonoRightEye)
  {
    aContext->ProjectionState.SetCurrent (aContext->Camera()->ProjectionStereoRightF());
    aContext->ApplyProjectionMatrix();
  }

  myWorkspace->SetEnvironmentTexture (myTextureEnv);

  renderScene (theProjection, theOutputFBO, theOitAccumFbo, theToDrawImmediate);

  myWorkspace->SetEnvironmentTexture (Handle(OpenGl_TextureSet)());

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
    renderTrihedron (myWorkspace);

    // Restore face culling
    if (myBackfacing != Graphic3d_TOBM_AUTOMATIC)
    {
      if (isCullFace)
      {
        glEnable (GL_CULL_FACE);
        glCullFace (GL_BACK);
      }
      else
        glDisable (GL_CULL_FACE);
    }
  }
  else
  {
    renderFrameStats();
  }

  myWorkspace->ResetAppliedAspect();
  aContext->SetAllowSampleAlphaToCoverage (false);
  aContext->SetSampleAlphaToCoverage (false);

  // reset FFP state for safety
  aContext->BindProgram (Handle(OpenGl_ShaderProgram)());
  if (aContext->caps->ffpEnable)
  {
    aContext->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
  }

  // ==============================================================
  //      Step 6: Keep shader manager informed about last View
  // ==============================================================

  if (!aManager.IsNull())
  {
    aManager->SetLastView (this);
  }
}

// =======================================================================
// function : InvalidateBVHData
// purpose  :
// =======================================================================
void OpenGl_View::InvalidateBVHData (const Graphic3d_ZLayerId theLayerId)
{
  myZLayers.InvalidateBVHData (theLayerId);
}

//=======================================================================
//function : renderStructs
//purpose  :
//=======================================================================
void OpenGl_View::renderStructs (Graphic3d_Camera::Projection theProjection,
                                 OpenGl_FrameBuffer*          theReadDrawFbo,
                                 OpenGl_FrameBuffer*          theOitAccumFbo,
                                 const Standard_Boolean       theToDrawImmediate)
{
  myZLayers.UpdateCulling (myWorkspace, theToDrawImmediate);
  if ( myZLayers.NbStructures() <= 0 )
    return;

  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  Standard_Boolean toRenderGL = theToDrawImmediate ||
    myRenderParams.Method != Graphic3d_RM_RAYTRACING ||
    myRaytraceInitStatus == OpenGl_RT_FAIL ||
    aCtx->IsFeedback();

  if (!toRenderGL)
  {
    const Standard_Integer aSizeX = theReadDrawFbo != NULL ? theReadDrawFbo->GetVPSizeX() : myWindow->Width();
    const Standard_Integer aSizeY = theReadDrawFbo != NULL ? theReadDrawFbo->GetVPSizeY() : myWindow->Height();

    toRenderGL = !initRaytraceResources (aSizeX, aSizeY, aCtx)
              || !updateRaytraceGeometry (OpenGl_GUM_CHECK, myId, aCtx);

    toRenderGL |= !myIsRaytraceDataValid; // if no ray-trace data use OpenGL

    if (!toRenderGL)
    {
      myOpenGlFBO ->InitLazy (aCtx, aSizeX, aSizeY, myFboColorFormat, myFboDepthFormat, 0);

      if (theReadDrawFbo != NULL)
        theReadDrawFbo->UnbindBuffer (aCtx);

      // Prepare preliminary OpenGL output
      if (aCtx->arbFBOBlit != NULL)
      {
        // Render bottom OSD layer
        myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_Bottom, theReadDrawFbo, theOitAccumFbo);

        const Standard_Integer aPrevFilter = myWorkspace->RenderFilter() & ~(Standard_Integer )(OpenGl_RenderFilter_NonRaytraceableOnly);
        myWorkspace->SetRenderFilter (aPrevFilter | OpenGl_RenderFilter_NonRaytraceableOnly);
        {
          if (theReadDrawFbo != NULL)
          {
            theReadDrawFbo->BindDrawBuffer (aCtx);
          }
          else
          {
            aCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
            aCtx->SetFrameBufferSRGB (false);
          }

          // Render non-polygonal elements in default layer
          myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_RayTracable, theReadDrawFbo, theOitAccumFbo);
        }
        myWorkspace->SetRenderFilter (aPrevFilter);
      }

      if (theReadDrawFbo != NULL)
      {
        theReadDrawFbo->BindBuffer (aCtx);
      }
      else
      {
        aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, 0);
        aCtx->SetFrameBufferSRGB (false);
      }

      // Reset OpenGl aspects state to default to avoid enabling of
      // backface culling which is not supported in ray-tracing.
      myWorkspace->ResetAppliedAspect();

      // Ray-tracing polygonal primitive arrays
      raytrace (aSizeX, aSizeY, theProjection, theReadDrawFbo, aCtx);

      // Render upper (top and topmost) OpenGL layers
      myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_Upper, theReadDrawFbo, theOitAccumFbo);
    }
  }

  // Redraw 3D scene using OpenGL in standard
  // mode or in case of ray-tracing failure
  if (toRenderGL)
  {
    myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_All, theReadDrawFbo, theOitAccumFbo);

    // Set flag that scene was redrawn by standard pipeline
    myWasRedrawnGL = Standard_True;
  }
}

//=======================================================================
//function : renderTrihedron
//purpose  :
//=======================================================================
void OpenGl_View::renderTrihedron (const Handle(OpenGl_Workspace) &theWorkspace)
{
  if (myToShowGradTrihedron)
  {
    myGraduatedTrihedron.Render (theWorkspace);
  }
}

//=======================================================================
//function : renderFrameStats
//purpose  :
//=======================================================================
void OpenGl_View::renderFrameStats()
{
  if (myRenderParams.ToShowStats
   && myRenderParams.CollectedStats != Graphic3d_RenderingParams::PerfCounters_NONE)
  {
    myFrameStatsPrs.Update (myWorkspace);
    myFrameStatsPrs.Render (myWorkspace);
  }
}

// =======================================================================
// function : Invalidate
// purpose  :
// =======================================================================
void OpenGl_View::Invalidate()
{
  myBackBufferRestored = Standard_False;
}

//=======================================================================
//function : renderScene
//purpose  :
//=======================================================================
void OpenGl_View::renderScene (Graphic3d_Camera::Projection theProjection,
                               OpenGl_FrameBuffer*          theReadDrawFbo,
                               OpenGl_FrameBuffer*          theOitAccumFbo,
                               const Standard_Boolean       theToDrawImmediate)
{
  const Handle(OpenGl_Context)& aContext = myWorkspace->GetGlContext();

  // Specify clipping planes in view transformation space
  aContext->ChangeClipping().Reset (myClipPlanes);
  if (!myClipPlanes.IsNull()
   && !myClipPlanes->IsEmpty())
  {
    aContext->ShaderManager()->UpdateClippingState();
  }

  renderStructs (theProjection, theReadDrawFbo, theOitAccumFbo, theToDrawImmediate);
  aContext->BindTextures (Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());

  // Apply restored view matrix.
  aContext->ApplyWorldViewMatrix();

  aContext->ChangeClipping().Reset (Handle(Graphic3d_SequenceOfHClipPlane)());
  if (!myClipPlanes.IsNull()
   && !myClipPlanes->IsEmpty())
  {
    aContext->ShaderManager()->RevertClippingState();
  }
}

// =======================================================================
// function : bindDefaultFbo
// purpose  :
// =======================================================================
void OpenGl_View::bindDefaultFbo (OpenGl_FrameBuffer* theCustomFbo)
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  OpenGl_FrameBuffer* anFbo = (theCustomFbo != NULL && theCustomFbo->IsValid())
                            ?  theCustomFbo
                            : (!aCtx->DefaultFrameBuffer().IsNull()
                             && aCtx->DefaultFrameBuffer()->IsValid()
                              ? aCtx->DefaultFrameBuffer().operator->()
                              : NULL);
  if (anFbo != NULL)
  {
    anFbo->BindBuffer (aCtx);
    anFbo->SetupViewport (aCtx);
  }
  else
  {
  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (GL_BACK);
  #else
    if (aCtx->arbFBO != NULL)
    {
      aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    }
  #endif
    const Standard_Integer aViewport[4] = { 0, 0, myWindow->Width(), myWindow->Height() };
    aCtx->ResizeViewport (aViewport);
  }
}

// =======================================================================
// function : initBlitQuad
// purpose  :
// =======================================================================
OpenGl_VertexBuffer* OpenGl_View::initBlitQuad (const Standard_Boolean theToFlip)
{
  OpenGl_VertexBuffer* aVerts = NULL;
  if (!theToFlip)
  {
    aVerts = &myFullScreenQuad;
    if (!aVerts->IsValid())
    {
      OpenGl_Vec4 aQuad[4] =
      {
        OpenGl_Vec4( 1.0f, -1.0f, 1.0f, 0.0f),
        OpenGl_Vec4( 1.0f,  1.0f, 1.0f, 1.0f),
        OpenGl_Vec4(-1.0f, -1.0f, 0.0f, 0.0f),
        OpenGl_Vec4(-1.0f,  1.0f, 0.0f, 1.0f)
      };
      aVerts->Init (myWorkspace->GetGlContext(), 4, 4, aQuad[0].GetData());
    }
  }
  else
  {
    aVerts = &myFullScreenQuadFlip;
    if (!aVerts->IsValid())
    {
      OpenGl_Vec4 aQuad[4] =
      {
        OpenGl_Vec4( 1.0f, -1.0f, 1.0f, 1.0f),
        OpenGl_Vec4( 1.0f,  1.0f, 1.0f, 0.0f),
        OpenGl_Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
        OpenGl_Vec4(-1.0f,  1.0f, 0.0f, 0.0f)
      };
      aVerts->Init (myWorkspace->GetGlContext(), 4, 4, aQuad[0].GetData());
    }
  }
  return aVerts;
}

// =======================================================================
// function : blitBuffers
// purpose  :
// =======================================================================
bool OpenGl_View::blitBuffers (OpenGl_FrameBuffer*    theReadFbo,
                               OpenGl_FrameBuffer*    theDrawFbo,
                               const Standard_Boolean theToFlip)
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  const Standard_Integer aReadSizeX = theReadFbo != NULL ? theReadFbo->GetVPSizeX() : myWindow->Width();
  const Standard_Integer aReadSizeY = theReadFbo != NULL ? theReadFbo->GetVPSizeY() : myWindow->Height();
  const Standard_Integer aDrawSizeX = theDrawFbo != NULL ? theDrawFbo->GetVPSizeX() : myWindow->Width();
  const Standard_Integer aDrawSizeY = theDrawFbo != NULL ? theDrawFbo->GetVPSizeY() : myWindow->Height();
  if (theReadFbo == NULL || aCtx->IsFeedback())
  {
    return false;
  }
  else if (theReadFbo == theDrawFbo)
  {
    return true;
  }

  // clear destination before blitting
  if (theDrawFbo != NULL
  &&  theDrawFbo->IsValid())
  {
    theDrawFbo->BindBuffer (aCtx);
  }
  else
  {
    aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
    aCtx->SetFrameBufferSRGB (false);
  }
  const Standard_Integer aViewport[4] = { 0, 0, aDrawSizeX, aDrawSizeY };
  aCtx->ResizeViewport (aViewport);

#if !defined(GL_ES_VERSION_2_0)
  aCtx->core20fwd->glClearDepth  (1.0);
#else
  aCtx->core20fwd->glClearDepthf (1.0f);
#endif
  aCtx->core20fwd->glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  const bool toApplyGamma = aCtx->ToRenderSRGB() != aCtx->IsFrameBufferSRGB();
  if (aCtx->arbFBOBlit != NULL
  && !toApplyGamma
  &&  theReadFbo->NbSamples() != 0)
  {
    GLbitfield aCopyMask = 0;
    theReadFbo->BindReadBuffer (aCtx);
    if (theDrawFbo != NULL
     && theDrawFbo->IsValid())
    {
      theDrawFbo->BindDrawBuffer (aCtx);
      if (theDrawFbo->HasColor()
       && theReadFbo->HasColor())
      {
        aCopyMask |= GL_COLOR_BUFFER_BIT;
      }
      if (theDrawFbo->HasDepth()
       && theReadFbo->HasDepth())
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
      aCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
      aCtx->SetFrameBufferSRGB (false);
    }

    // we don't copy stencil buffer here... does it matter for performance?
    aCtx->arbFBOBlit->glBlitFramebuffer (0, 0, aReadSizeX, aReadSizeY,
                                         0, 0, aDrawSizeX, aDrawSizeY,
                                         aCopyMask, GL_NEAREST);
    const int anErr = ::glGetError();
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
      TCollection_ExtendedString aMsg = TCollection_ExtendedString() + "FBO blitting has failed [Error #" + anErr + "]\n"
                                      + "  Please check your graphics driver settings or try updating driver.";
      if (theReadFbo->NbSamples() != 0)
      {
        myToDisableMSAA = true;
        aMsg += "\n  MSAA settings should not be overridden by driver!";
      }
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                         GL_DEBUG_TYPE_ERROR,
                         0,
                         GL_DEBUG_SEVERITY_HIGH,
                         aMsg);
    }

    if (theDrawFbo != NULL
     && theDrawFbo->IsValid())
    {
      theDrawFbo->BindBuffer (aCtx);
    }
    else
    {
      aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
      aCtx->SetFrameBufferSRGB (false);
    }
  }
  else
  {
    aCtx->core20fwd->glDepthFunc (GL_ALWAYS);
    aCtx->core20fwd->glDepthMask (GL_TRUE);
    aCtx->core20fwd->glEnable (GL_DEPTH_TEST);
  #if defined(GL_ES_VERSION_2_0)
    if (!aCtx->IsGlGreaterEqual (3, 0)
     && !aCtx->extFragDepth)
    {
      aCtx->core20fwd->glDisable (GL_DEPTH_TEST);
    }
  #endif

    aCtx->BindTextures (Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());

    const Graphic3d_TypeOfTextureFilter aFilter = (aDrawSizeX == aReadSizeX && aDrawSizeY == aReadSizeY) ? Graphic3d_TOTF_NEAREST : Graphic3d_TOTF_BILINEAR;
    const GLint aFilterGl = aFilter == Graphic3d_TOTF_NEAREST ? GL_NEAREST : GL_LINEAR;

    OpenGl_VertexBuffer* aVerts = initBlitQuad (theToFlip);
    const Handle(OpenGl_ShaderManager)& aManager = aCtx->ShaderManager();
    if (aVerts->IsValid()
     && aManager->BindFboBlitProgram (theReadFbo != NULL ? theReadFbo->NbSamples() : 0, toApplyGamma))
    {
      aCtx->SetSampleAlphaToCoverage (false);
      theReadFbo->ColorTexture()->Bind (aCtx, Graphic3d_TextureUnit_0);
      if (theReadFbo->ColorTexture()->Sampler()->Parameters()->Filter() != aFilter)
      {
        theReadFbo->ColorTexture()->Sampler()->Parameters()->SetFilter (aFilter);
        aCtx->core20fwd->glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterGl);
        aCtx->core20fwd->glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilterGl);
      }

      theReadFbo->DepthStencilTexture()->Bind (aCtx, Graphic3d_TextureUnit_1);
      if (theReadFbo->DepthStencilTexture()->Sampler()->Parameters()->Filter() != aFilter)
      {
        theReadFbo->DepthStencilTexture()->Sampler()->Parameters()->SetFilter (aFilter);
        aCtx->core20fwd->glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, aFilterGl);
        aCtx->core20fwd->glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, aFilterGl);
      }

      aVerts->BindVertexAttrib (aCtx, Graphic3d_TOA_POS);

      aCtx->core20fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

      aVerts->UnbindVertexAttrib (aCtx, Graphic3d_TOA_POS);
      theReadFbo->DepthStencilTexture()->Unbind (aCtx, Graphic3d_TextureUnit_1);
      theReadFbo->ColorTexture()       ->Unbind (aCtx, Graphic3d_TextureUnit_0);
      aCtx->BindProgram (NULL);
    }
    else
    {
      TCollection_ExtendedString aMsg = TCollection_ExtendedString()
        + "Error! FBO blitting has failed";
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                         GL_DEBUG_TYPE_ERROR,
                         0,
                         GL_DEBUG_SEVERITY_HIGH,
                         aMsg);
      myHasFboBlit = Standard_False;
      theReadFbo->Release (aCtx.operator->());
      return true;
    }
  }
  return true;
}

// =======================================================================
// function : drawStereoPair
// purpose  :
// =======================================================================
void OpenGl_View::drawStereoPair (OpenGl_FrameBuffer* theDrawFbo)
{
  const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext();
  bindDefaultFbo (theDrawFbo);
  OpenGl_FrameBuffer* aPair[2] =
  {
    myImmediateSceneFbos[0]->IsValid() ? myImmediateSceneFbos[0].operator->() : NULL,
    myImmediateSceneFbos[1]->IsValid() ? myImmediateSceneFbos[1].operator->() : NULL
  };
  if (aPair[0] == NULL
  ||  aPair[1] == NULL
  || !myTransientDrawToFront)
  {
    aPair[0] = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : NULL;
    aPair[1] = myMainSceneFbos[1]->IsValid() ? myMainSceneFbos[1].operator->() : NULL;
  }

  if (aPair[0] == NULL
   || aPair[1] == NULL)
  {
    return;
  }

  if (aPair[0]->NbSamples() != 0)
  {
    // resolve MSAA buffers before drawing
    if (!myOpenGlFBO ->InitLazy (aCtx, aPair[0]->GetVPSizeX(), aPair[0]->GetVPSizeY(), myFboColorFormat, myFboDepthFormat, 0)
     || !myOpenGlFBO2->InitLazy (aCtx, aPair[0]->GetVPSizeX(), aPair[0]->GetVPSizeY(), myFboColorFormat, 0, 0))
    {
      aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                         GL_DEBUG_TYPE_ERROR,
                         0,
                         GL_DEBUG_SEVERITY_HIGH,
                         "Error! Unable to allocate FBO for blitting stereo pair");
      bindDefaultFbo (theDrawFbo);
      return;
    }

    if (!blitBuffers (aPair[0], myOpenGlFBO .operator->(), Standard_False)
     || !blitBuffers (aPair[1], myOpenGlFBO2.operator->(), Standard_False))
    {
      bindDefaultFbo (theDrawFbo);
      return;
    }

    aPair[0] = myOpenGlFBO .operator->();
    aPair[1] = myOpenGlFBO2.operator->();
    bindDefaultFbo (theDrawFbo);
  }

  struct
  {
    Standard_Integer left;
    Standard_Integer top;
    Standard_Integer right;
    Standard_Integer bottom;
    Standard_Integer dx() { return right  - left; }
    Standard_Integer dy() { return bottom - top; }
  } aGeom;

  myWindow->PlatformWindow()->Position (aGeom.left, aGeom.top, aGeom.right, aGeom.bottom);

  Standard_Boolean toReverse = myRenderParams.ToReverseStereo;
  const Standard_Boolean isOddY = (aGeom.top + aGeom.dy()) % 2 == 1;
  const Standard_Boolean isOddX =  aGeom.left % 2 == 1;
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
    std::swap (aPair[0], aPair[1]);
  }

  aCtx->core20fwd->glDepthFunc (GL_ALWAYS);
  aCtx->core20fwd->glDepthMask (GL_TRUE);
  aCtx->core20fwd->glEnable (GL_DEPTH_TEST);

  aCtx->BindTextures (Handle(OpenGl_TextureSet)(), Handle(OpenGl_ShaderProgram)());
  OpenGl_VertexBuffer* aVerts = initBlitQuad (myToFlipOutput);

  const Handle(OpenGl_ShaderManager)& aManager = aCtx->ShaderManager();
  if (aVerts->IsValid()
   && aManager->BindStereoProgram (myRenderParams.StereoMode))
  {
    if (myRenderParams.StereoMode == Graphic3d_StereoMode_Anaglyph)
    {
      OpenGl_Mat4 aFilterL, aFilterR;
      aFilterL.SetDiagonal (Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
      aFilterR.SetDiagonal (Graphic3d_Vec4 (0.0f, 0.0f, 0.0f, 0.0f));
      switch (myRenderParams.AnaglyphFilter)
      {
        case Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple:
        {
          aFilterL.SetRow (0, Graphic3d_Vec4 (1.0f, 0.0f, 0.0f, 0.0f));
          aFilterR.SetRow (1, Graphic3d_Vec4 (0.0f, 1.0f, 0.0f, 0.0f));
          aFilterR.SetRow (2, Graphic3d_Vec4 (0.0f, 0.0f, 1.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized:
        {
          aFilterL.SetRow (0, Graphic3d_Vec4 ( 0.4154f,      0.4710f,      0.16666667f, 0.0f));
          aFilterL.SetRow (1, Graphic3d_Vec4 (-0.0458f,     -0.0484f,     -0.0257f,     0.0f));
          aFilterL.SetRow (2, Graphic3d_Vec4 (-0.0547f,     -0.0615f,      0.0128f,     0.0f));
          aFilterL.SetRow (3, Graphic3d_Vec4 ( 0.0f,         0.0f,         0.0f,        0.0f));
          aFilterR.SetRow (0, Graphic3d_Vec4 (-0.01090909f, -0.03636364f, -0.00606061f, 0.0f));
          aFilterR.SetRow (1, Graphic3d_Vec4 ( 0.37560000f,  0.73333333f,  0.01111111f, 0.0f));
          aFilterR.SetRow (2, Graphic3d_Vec4 (-0.06510000f, -0.12870000f,  1.29710000f, 0.0f));
          aFilterR.SetRow (3, Graphic3d_Vec4 ( 0.0f,                0.0f,  0.0f,        0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple:
        {
          aFilterL.SetRow (0, Graphic3d_Vec4 (1.0f, 0.0f, 0.0f, 0.0f));
          aFilterL.SetRow (1, Graphic3d_Vec4 (0.0f, 1.0f, 0.0f, 0.0f));
          aFilterR.SetRow (2, Graphic3d_Vec4 (0.0f, 0.0f, 1.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized:
        {
          aFilterL.SetRow (0, Graphic3d_Vec4 ( 1.062f, -0.205f,  0.299f, 0.0f));
          aFilterL.SetRow (1, Graphic3d_Vec4 (-0.026f,  0.908f,  0.068f, 0.0f));
          aFilterL.SetRow (2, Graphic3d_Vec4 (-0.038f, -0.173f,  0.022f, 0.0f));
          aFilterL.SetRow (3, Graphic3d_Vec4 ( 0.0f,    0.0f,    0.0f,   0.0f));
          aFilterR.SetRow (0, Graphic3d_Vec4 (-0.016f, -0.123f, -0.017f, 0.0f));
          aFilterR.SetRow (1, Graphic3d_Vec4 ( 0.006f,  0.062f, -0.017f, 0.0f));
          aFilterR.SetRow (2, Graphic3d_Vec4 ( 0.094f,  0.185f,  0.911f, 0.0f));
          aFilterR.SetRow (3, Graphic3d_Vec4 ( 0.0f,    0.0f,    0.0f,   0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple:
        {
          aFilterR.SetRow (0, Graphic3d_Vec4 (1.0f, 0.0f, 0.0f, 0.0f));
          aFilterL.SetRow (1, Graphic3d_Vec4 (0.0f, 1.0f, 0.0f, 0.0f));
          aFilterR.SetRow (2, Graphic3d_Vec4 (0.0f, 0.0f, 1.0f, 0.0f));
          break;
        }
        case Graphic3d_RenderingParams::Anaglyph_UserDefined:
        {
          aFilterL = myRenderParams.AnaglyphLeft;
          aFilterR = myRenderParams.AnaglyphRight;
          break;
        }
      }
      aCtx->ActiveProgram()->SetUniform (aCtx, "uMultL", aFilterL);
      aCtx->ActiveProgram()->SetUniform (aCtx, "uMultR", aFilterR);
    }

    aPair[0]->ColorTexture()->Bind (aCtx, Graphic3d_TextureUnit_0);
    aPair[1]->ColorTexture()->Bind (aCtx, Graphic3d_TextureUnit_1);
    aVerts->BindVertexAttrib (aCtx, 0);

    aCtx->core20fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

    aVerts->UnbindVertexAttrib (aCtx, 0);
    aPair[1]->ColorTexture()->Unbind (aCtx, Graphic3d_TextureUnit_1);
    aPair[0]->ColorTexture()->Unbind (aCtx, Graphic3d_TextureUnit_0);
  }
  else
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString()
      + "Error! Anaglyph has failed";
    aCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                       GL_DEBUG_TYPE_ERROR,
                       0,
                       GL_DEBUG_SEVERITY_HIGH,
                       aMsg);
  }
}

// =======================================================================
// function : copyBackToFront
// purpose  :
// =======================================================================
bool OpenGl_View::copyBackToFront()
{
  myIsImmediateDrawn = Standard_False;
#if !defined(GL_ES_VERSION_2_0)
  const Handle(OpenGl_Context)& aCtx = myWorkspace->GetGlContext();
  if (aCtx->core11 == NULL)
  {
    return false;
  }

  OpenGl_Mat4 aProjectMat;
  Graphic3d_TransformUtils::Ortho2D (aProjectMat,
                                     0.0f, static_cast<GLfloat> (myWindow->Width()),
                                     0.0f, static_cast<GLfloat> (myWindow->Height()));

  aCtx->WorldViewState.Push();
  aCtx->ProjectionState.Push();

  aCtx->WorldViewState.SetIdentity();
  aCtx->ProjectionState.SetCurrent (aProjectMat);

  aCtx->ApplyProjectionMatrix();
  aCtx->ApplyWorldViewMatrix();

  // synchronize FFP state before copying pixels
  aCtx->BindProgram (Handle(OpenGl_ShaderProgram)());
  aCtx->ShaderManager()->PushState (Handle(OpenGl_ShaderProgram)());
  aCtx->DisableFeatures();

  switch (aCtx->DrawBuffer())
  {
    case GL_BACK_LEFT:
    {
      aCtx->SetReadBuffer (GL_BACK_LEFT);
      aCtx->SetDrawBuffer (GL_FRONT_LEFT);
      break;
    }
    case GL_BACK_RIGHT:
    {
      aCtx->SetReadBuffer (GL_BACK_RIGHT);
      aCtx->SetDrawBuffer (GL_FRONT_RIGHT);
      break;
    }
    default:
    {
      aCtx->SetReadBuffer (GL_BACK);
      aCtx->SetDrawBuffer (GL_FRONT);
      break;
    }
  }

  aCtx->core11->glRasterPos2i (0, 0);
  aCtx->core11->glCopyPixels  (0, 0, myWindow->Width() + 1, myWindow->Height() + 1, GL_COLOR);
  //aCtx->core11->glCopyPixels  (0, 0, myWidth + 1, myHeight + 1, GL_DEPTH);

  aCtx->EnableFeatures();

  aCtx->WorldViewState.Pop();
  aCtx->ProjectionState.Pop();
  aCtx->ApplyProjectionMatrix();

  // read/write from front buffer now
  aCtx->SetReadBuffer (aCtx->DrawBuffer());
  return true;
#else
  return false;
#endif
}

// =======================================================================
// function : checkOitCompatibility
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::checkOitCompatibility (const Handle(OpenGl_Context)& theGlContext,
                                                     const Standard_Boolean theMSAA)
{
  // determine if OIT is supported by current OpenGl context
  Standard_Boolean& aToDisableOIT = theMSAA ? myToDisableMSAA : myToDisableOIT;
  if (aToDisableOIT)
  {
    return Standard_False;
  }

  TCollection_ExtendedString aCompatibilityMsg;
  if (theGlContext->hasFloatBuffer     == OpenGl_FeatureNotAvailable
   && theGlContext->hasHalfFloatBuffer == OpenGl_FeatureNotAvailable)
  {
    aCompatibilityMsg += "OpenGL context does not support floating-point RGBA color buffer format.\n";
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
    return Standard_True;
  }

  aCompatibilityMsg += "  Blended order-independent transparency will not be available.\n";
  theGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                          GL_DEBUG_TYPE_ERROR,
                          0,
                          GL_DEBUG_SEVERITY_HIGH,
                          aCompatibilityMsg);

  aToDisableOIT = Standard_True;
  return Standard_False;
}

// =======================================================================
// function : chooseOitColorConfiguration
// purpose  :
// =======================================================================
bool OpenGl_View::chooseOitColorConfiguration (const Handle(OpenGl_Context)& theGlContext,
                                               const Standard_Integer theConfigIndex,
                                               OpenGl_ColorFormats& theFormats)
{
  theFormats.Clear();
  switch (theConfigIndex)
  {
    case 0: // choose best applicable color format combination
    {
      theFormats.Append (theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_RGBA16F : GL_RGBA32F);
      theFormats.Append (theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_R16F    : GL_R32F);
      return true;
    }
    case 1: // choose non-optimal applicable color format combination
    {
      theFormats.Append (theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_RGBA16F : GL_RGBA32F);
      theFormats.Append (theGlContext->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable ? GL_RGBA16F : GL_RGBA32F);
      return true;
    }
  }
  return false; // color combination does not exist
}

// =======================================================================
// function : checkPBRAvailability
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_View::checkPBRAvailability() const
{
  return myWorkspace->GetGlContext()->HasPBR()
      && !myPBREnvironment.IsNull();
}

// =======================================================================
// function : bakePBREnvironment
// purpose  :
// =======================================================================
void OpenGl_View::bakePBREnvironment (const Handle(OpenGl_Context)& theCtx)
{
  const Handle(OpenGl_TextureSet)& aTextureSet = myCubeMapParams->TextureSet (theCtx);
  if (!aTextureSet.IsNull()
   && !aTextureSet->IsEmpty())
  {
    myPBREnvironment->Bake (theCtx,
                            aTextureSet->First(),
                            myBackgroundCubeMap->ZIsInverted(),
                            myBackgroundCubeMap->IsTopDown(),
                            myRenderParams.PbrEnvBakingDiffNbSamples,
                            myRenderParams.PbrEnvBakingSpecNbSamples,
                            myRenderParams.PbrEnvBakingProbability);
  }
  else
  {
    myPBREnvironment->Clear (theCtx);
  }
}

// =======================================================================
// function : clearPBREnvironment
// purpose  :
// =======================================================================
void OpenGl_View::clearPBREnvironment (const Handle(OpenGl_Context)& theCtx)
{
  myPBREnvironment->Clear (theCtx);
}

// =======================================================================
// function : clearPBREnvironment
// purpose  :
// =======================================================================
void OpenGl_View::processPBREnvRequest (const Handle(OpenGl_Context)& theCtx)
{
  if (myPBREnvState == OpenGl_PBREnvState_CREATED)
  {
    switch (myPBREnvRequest)
    {
      case OpenGl_PBREnvRequest_NONE:  return;
      case OpenGl_PBREnvRequest_BAKE:  bakePBREnvironment  (theCtx); break;
      case OpenGl_PBREnvRequest_CLEAR: clearPBREnvironment (theCtx); break;
    }
  }
  myPBREnvRequest = OpenGl_PBREnvRequest_NONE;
}
