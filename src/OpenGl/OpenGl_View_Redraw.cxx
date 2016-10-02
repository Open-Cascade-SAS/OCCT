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

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <Image_AlienPixMap.hxx>

#include <NCollection_Mat4.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_ArbFBO.hxx>

#define EPSI 0.0001

namespace
{
  static const GLfloat THE_DEFAULT_AMBIENT[4]    = { 0.0f, 0.0f, 0.0f, 1.0f };
  static const GLfloat THE_DEFAULT_SPOT_DIR[3]   = { 0.0f, 0.0f, -1.0f };
  static const GLfloat THE_DEFAULT_SPOT_EXPONENT = 0.0f;
  static const GLfloat THE_DEFAULT_SPOT_CUTOFF   = 180.0f;
}

extern void InitLayerProp (const int theListId); //szvgl: defined in OpenGl_GraphicDriver_Layer.cxx

#if !defined(GL_ES_VERSION_2_0)

//=======================================================================
//function : bindLight
//purpose  :
//=======================================================================
static void bindLight (const OpenGl_Light&             theLight,
                       GLenum&                         theLightGlId,
                       Graphic3d_Vec4&                 theAmbientColor,
                       const Handle(OpenGl_Workspace)& theWorkspace)
{
  // Only 8 lights in OpenGL...
  if (theLightGlId > GL_LIGHT7)
  {
    return;
  }

  if (theLight.Type == Graphic3d_TOLS_AMBIENT)
  {
    // add RGBA intensity of the ambient light
    theAmbientColor += theLight.Color;
    return;
  }

  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // the light is a headlight?
  if (theLight.IsHeadlight)
  {
    aContext->WorldViewState.Push();
    aContext->WorldViewState.SetIdentity();

    aContext->ApplyWorldViewMatrix();
  }

  // setup light type
  switch (theLight.Type)
  {
    case Graphic3d_TOLS_AMBIENT    : break; // handled by separate if-clause at beginning of method
    case Graphic3d_TOLS_DIRECTIONAL:
    {
      // if the last parameter of GL_POSITION, is zero, the corresponding light source is a Directional one
      const OpenGl_Vec4 anInfDir = -theLight.Direction;

      // to create a realistic effect,  set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE.
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              anInfDir.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
      break;
    }
    case Graphic3d_TOLS_POSITIONAL:
    {
      // to create a realistic effect, set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE
      const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position.x()), static_cast<float>(theLight.Position.y()), static_cast<float>(theLight.Position.z()), 1.0f);
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
      glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
      glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
      glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0);
      break;
    }
    case Graphic3d_TOLS_SPOT:
    {
      const OpenGl_Vec4 aPosition (static_cast<float>(theLight.Position.x()), static_cast<float>(theLight.Position.y()), static_cast<float>(theLight.Position.z()), 1.0f);
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              aPosition.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        theLight.Direction.GetData());
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         theLight.Concentration() * 128.0f);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,          (theLight.Angle() * 180.0f) / GLfloat(M_PI));
      glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
      glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
      glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0f);
      break;
    }
  }

  // restore matrix in case of headlight
  if (theLight.IsHeadlight)
  {
    aContext->WorldViewState.Pop();
  }

  glEnable (theLightGlId++);
}
#endif

//=======================================================================
//function : drawBackground
//purpose  :
//=======================================================================
void OpenGl_View::drawBackground (const Handle(OpenGl_Workspace)& theWorkspace)
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  if ((theWorkspace->NamedStatus & OPENGL_NS_WHITEBACK) != 0 // no background
    || (!myBgTextureArray->IsDefined()                       // no texture
     && !myBgGradientArray->IsDefined()))                    // no gradient
  {
    return;
  }

  const Standard_Boolean wasUsedZBuffer = theWorkspace->SetUseZBuffer (Standard_False);
  if (wasUsedZBuffer)
  {
    aCtx->core11fwd->glDisable (GL_DEPTH_TEST);
  }

  // Drawing background gradient if:
  // - gradient fill type is not Aspect_GFM_NONE and
  // - either background texture is no specified or it is drawn in Aspect_FM_CENTERED mode
  if (myBgGradientArray->IsDefined()
    && (!myTextureParams->Aspect()->ToMapTexture()
      || myBgTextureArray->TextureFillMethod() == Aspect_FM_CENTERED
      || myBgTextureArray->TextureFillMethod() == Aspect_FM_NONE))
  {
  #if !defined(GL_ES_VERSION_2_0)
    GLint aShadingModelOld = GL_SMOOTH;
    if (aCtx->core11 != NULL)
    {
      aCtx->core11fwd->glDisable (GL_LIGHTING);
      aCtx->core11fwd->glGetIntegerv (GL_SHADE_MODEL, &aShadingModelOld);
      aCtx->core11->glShadeModel (GL_SMOOTH);
    }
  #endif

    myBgGradientArray->Render (theWorkspace);

  #if !defined(GL_ES_VERSION_2_0)
    if (aCtx->core11 != NULL)
    {
      aCtx->core11->glShadeModel (aShadingModelOld);
    }
  #endif
  }

  // Drawing background image if it is defined
  // (texture is defined and fill type is not Aspect_FM_NONE)
  if (myBgTextureArray->IsDefined()
   && myTextureParams->Aspect()->ToMapTexture())
  {
    aCtx->core11fwd->glDisable (GL_BLEND);

    const OpenGl_AspectFace* anOldAspectFace = theWorkspace->SetAspectFace (myTextureParams);
    myBgTextureArray->Render (theWorkspace);
    theWorkspace->SetAspectFace (anOldAspectFace);
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
    if (myWasRedrawnGL)
    {
      myDeviceLostFlag = Standard_True;
    }

    myCaps->keepArrayData = Standard_True;
  }

  if (!myWorkspace->Activate())
  {
    return;
  }

  myWindow->SetSwapInterval();

  ++myFrameCounter;
  const Graphic3d_StereoMode   aStereoMode  = myRenderParams.StereoMode;
  Graphic3d_Camera::Projection aProjectType = myCamera->ProjectionType();
  Handle(OpenGl_Context)       aCtx         = myWorkspace->GetGlContext();

  // release pending GL resources
  aCtx->ReleaseDelayed();

  // fetch OpenGl context state
  aCtx->FetchState();

  // set resolution ratio
  aCtx->SetResolutionRatio (RenderingParams().ResolutionRatio());

  OpenGl_FrameBuffer* aFrameBuffer = myFBO.operator->();
  bool toSwap = aCtx->IsRender()
            && !aCtx->caps->buffersNoSwap
            &&  aFrameBuffer == NULL;

  Standard_Integer aSizeX = aFrameBuffer != NULL ? aFrameBuffer->GetVPSizeX() : myWindow->Width();
  Standard_Integer aSizeY = aFrameBuffer != NULL ? aFrameBuffer->GetVPSizeY() : myWindow->Height();

  // determine multisampling parameters
  Standard_Integer aNbSamples = !myToDisableMSAA
                              ? Max (Min (myRenderParams.NbMsaaSamples, aCtx->MaxMsaaSamples()), 0)
                              : 0;
  if (aNbSamples != 0)
  {
    aNbSamples = OpenGl_Context::GetPowerOfTwo (aNbSamples, aCtx->MaxMsaaSamples());
  }

  if ( aFrameBuffer == NULL
   && !aCtx->DefaultFrameBuffer().IsNull()
   &&  aCtx->DefaultFrameBuffer()->IsValid())
  {
    aFrameBuffer = aCtx->DefaultFrameBuffer().operator->();
  }

  if (myHasFboBlit
   && (myTransientDrawToFront
    || aProjectType == Graphic3d_Camera::Projection_Stereo
    || aNbSamples != 0))
  {
    if (myMainSceneFbos[0]->GetVPSizeX() != aSizeX
     || myMainSceneFbos[0]->GetVPSizeY() != aSizeY
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
        myMainSceneFbos[0]->Init (aCtx, aSizeX, aSizeY, myFboColorFormat, myFboDepthFormat, aNbSamples);
      }
      if (myTransientDrawToFront
       && !aCtx->caps->useSystemBuffer
       && myMainSceneFbos[0]->IsValid())
      {
        myImmediateSceneFbos[0]->InitLazy (aCtx, *myMainSceneFbos[0]);
      }
    }
  }
  else
  {
    myMainSceneFbos     [0]->Release (aCtx.operator->());
    myMainSceneFbos     [1]->Release (aCtx.operator->());
    myImmediateSceneFbos[0]->Release (aCtx.operator->());
    myImmediateSceneFbos[1]->Release (aCtx.operator->());
    myMainSceneFbos     [0]->ChangeViewport (0, 0);
    myMainSceneFbos     [1]->ChangeViewport (0, 0);
    myImmediateSceneFbos[0]->ChangeViewport (0, 0);
    myImmediateSceneFbos[1]->ChangeViewport (0, 0);
  }

  if (aProjectType == Graphic3d_Camera::Projection_Stereo
   && myMainSceneFbos[0]->IsValid())
  {
    myMainSceneFbos[1]->InitLazy (aCtx, *myMainSceneFbos[0]);
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
      myImmediateSceneFbos[0]->InitLazy (aCtx, *myMainSceneFbos[0]);
      myImmediateSceneFbos[1]->InitLazy (aCtx, *myMainSceneFbos[0]);
      if (!myImmediateSceneFbos[0]->IsValid()
       || !myImmediateSceneFbos[1]->IsValid())
      {
        aProjectType = Graphic3d_Camera::Projection_Perspective;
      }
    }
  }

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

    if (!myTransientDrawToFront)
    {
      anImmFbos[0] = aMainFbos[0];
      anImmFbos[1] = aMainFbos[1];
    }
    else if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
          || aStereoMode == Graphic3d_StereoMode_QuadBuffer)
    {
      anImmFbos[0] = NULL;
      anImmFbos[1] = NULL;
    }

  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT : GL_BACK);
  #endif
    redraw (Graphic3d_Camera::Projection_MonoLeftEye, aMainFbos[0]);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_LEFT : GL_BACK);
  #endif
    if (!redrawImmediate (Graphic3d_Camera::Projection_MonoLeftEye, aMainFbos[0], anImmFbos[0]))
    {
      toSwap = false;
    }
    else if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip && toSwap)
    {
      aCtx->SwapBuffers();
    }

  #if !defined(GL_ES_VERSION_2_0)
    aCtx->SetReadDrawBuffer (aStereoMode == Graphic3d_StereoMode_QuadBuffer ? GL_BACK_RIGHT : GL_BACK);
  #endif
    redraw (Graphic3d_Camera::Projection_MonoRightEye, aMainFbos[1]);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
    if (!redrawImmediate (Graphic3d_Camera::Projection_MonoRightEye, aMainFbos[1], anImmFbos[1]))
    {
      toSwap = false;
    }

    if (anImmFbos[0] != NULL)
    {
      drawStereoPair (aFrameBuffer);
    }
  }
  else
  {
    OpenGl_FrameBuffer* aMainFbo = myMainSceneFbos[0]->IsValid() ? myMainSceneFbos[0].operator->() : aFrameBuffer;
    OpenGl_FrameBuffer* anImmFbo = aFrameBuffer;
    if (!aCtx->caps->useSystemBuffer && myImmediateSceneFbos[0]->IsValid())
    {
      anImmFbo = myImmediateSceneFbos[0].operator->();
    }
    if (!myTransientDrawToFront)
    {
      anImmFbo = aMainFbo;
    }

  #if !defined(GL_ES_VERSION_2_0)
    if (aMainFbo == NULL)
    {
      aCtx->SetReadDrawBuffer (GL_BACK);
    }
  #endif
    redraw (aProjectType, aMainFbo);
    myBackBufferRestored = Standard_True;
    myIsImmediateDrawn   = Standard_False;
    if (!redrawImmediate (aProjectType, aMainFbo, anImmFbo))
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
  OpenGl_FrameBuffer*          aFrameBuffer = myFBO.operator->();

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
    if (aStereoMode == Graphic3d_StereoMode_SoftPageFlip
     || aStereoMode == Graphic3d_StereoMode_QuadBuffer)
    {
      anImmFbos[0] = NULL;
      anImmFbos[1] = NULL;
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
    toSwap = redrawImmediate (Graphic3d_Camera::Projection_MonoLeftEye,
                              aMainFbos[0],
                              anImmFbos[0],
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
    toSwap = redrawImmediate (Graphic3d_Camera::Projection_MonoRightEye,
                              aMainFbos[1],
                              anImmFbos[1],
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
    if (!aCtx->caps->useSystemBuffer && myImmediateSceneFbos[0]->IsValid())
    {
      anImmFbo = myImmediateSceneFbos[0].operator->();
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (aMainFbo == NULL)
    {
      aCtx->SetReadDrawBuffer (GL_BACK);
    }
  #endif
    toSwap = redrawImmediate (aProjectType,
                              aMainFbo,
                              anImmFbo,
                              Standard_True) || toSwap;
    if (anImmFbo != NULL
     && anImmFbo != aFrameBuffer)
    {
      blitBuffers (anImmFbo, aFrameBuffer, myToFlipOutput);
    }
  }

  // bind default FBO
  bindDefaultFbo();

  if (toSwap && !aCtx->caps->buffersNoSwap)
  {
    aCtx->SwapBuffers();
  }
  else
  {
    aCtx->core11fwd->glFlush();
  }

  myWasRedrawnGL = Standard_True;
}

// =======================================================================
// function : redraw
// purpose  :
// =======================================================================
void OpenGl_View::redraw (const Graphic3d_Camera::Projection theProjection, OpenGl_FrameBuffer* theReadDrawFbo)
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
  myWorkspace->NamedStatus    |= OPENGL_NS_RESMAT;
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

  if (myWorkspace->NamedStatus & OPENGL_NS_WHITEBACK)
  {
    // set background to white
    glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
  }
  else
  {
    const OpenGl_Vec4& aBgColor = myBgColor;
    glClearColor (aBgColor.r(), aBgColor.g(), aBgColor.b(), 0.0f);
  }

  glClear (toClear);

  render (theProjection, theReadDrawFbo, Standard_False);
}

// =======================================================================
// function : redrawMonoImmediate
// purpose  :
// =======================================================================
bool OpenGl_View::redrawImmediate (const Graphic3d_Camera::Projection theProjection,
                                   OpenGl_FrameBuffer*    theReadFbo,
                                   OpenGl_FrameBuffer*    theDrawFbo,
                                   const Standard_Boolean theIsPartialUpdate)
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  GLboolean toCopyBackToFront = GL_FALSE;
  if (theDrawFbo == theReadFbo
   && theDrawFbo != NULL)
  {
    myBackBufferRestored = Standard_False;
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
    if (toCopyBackToFront)
    {
      if (!HasImmediateStructures()
       && !theIsPartialUpdate)
      {
        // prefer Swap Buffers within Redraw in compatibility mode (without FBO)
        return true;
      }
      copyBackToFront();
    }
    else
    {
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

  render (theProjection, theDrawFbo, Standard_True);

  return !toCopyBackToFront;
}

//=======================================================================
//function : Render
//purpose  :
//=======================================================================
void OpenGl_View::render (Graphic3d_Camera::Projection theProjection,
                          OpenGl_FrameBuffer*          theOutputFBO,
                          const Standard_Boolean       theToDrawImmediate)
{
  // ==================================
  //      Step 1: Prepare for render
  // ==================================

  const Handle(OpenGl_Context)& aContext = myWorkspace->GetGlContext();

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

  // Update states of OpenGl_BVHTreeSelector (frustum culling algorithm).
  myBVHSelector.SetViewVolume (myCamera);
  myBVHSelector.SetViewportSize (myWindow->Width(), myWindow->Height());

  const Handle(OpenGl_ShaderManager)& aManager   = aContext->ShaderManager();
  if (StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index()) != myLastLightSourceState)
  {
    aManager->UpdateLightSourceStateTo (myShadingModel == Graphic3d_TOSM_NONE ? &myNoShadingLight : &myLights);
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
  aContext->ProjectionState.SetCurrent (myCamera->ProjectionMatrixF());
  aContext->WorldViewState .SetCurrent (myCamera->OrientationMatrixF());
  aContext->ApplyProjectionMatrix();
  aContext->ApplyWorldViewMatrix();
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
    drawBackground (myWorkspace);
  }

#if !defined(GL_ES_VERSION_2_0)
  // Switch off lighting by default
  if (aContext->core11 != NULL)
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
  const gp_Pnt anAxialScale = myCamera->AxialScale();
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

  // Apply InteriorShadingMethod
  if (aContext->core11 != NULL)
  {
    aContext->core11->glShadeModel (myShadingModel == Graphic3d_TOSM_FACET
                                 || myShadingModel == Graphic3d_TOSM_NONE ? GL_FLAT : GL_SMOOTH);
  }
#endif

  aManager->SetShadingModel (myShadingModel);

  // Redraw 3d scene
  if (theProjection == Graphic3d_Camera::Projection_MonoLeftEye)
  {
    aContext->ProjectionState.SetCurrent (myCamera->ProjectionStereoLeftF());
    aContext->ApplyProjectionMatrix();
  }
  else if (theProjection == Graphic3d_Camera::Projection_MonoRightEye)
  {
    aContext->ProjectionState.SetCurrent (myCamera->ProjectionStereoRightF());
    aContext->ApplyProjectionMatrix();
  }

  myWorkspace->SetEnvironmentTexture (myTextureEnv);

  renderScene (theProjection, theOutputFBO, theToDrawImmediate);

  myWorkspace->SetEnvironmentTexture (Handle(OpenGl_Texture)());

  // ===============================
  //      Step 4: Trihedron
  // ===============================

  // Resetting GL parameters according to the default aspects
  // in order to synchronize GL state with the graphic driver state
  // before drawing auxiliary stuff (trihedrons, overlayer)
  myWorkspace->ResetAppliedAspect();


  // We need to disable (unbind) all shaders programs to ensure
  // that all objects without specified aspect will be drawn
  // correctly (such as background)
  aContext->BindProgram (NULL);

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
                                 const Standard_Boolean       theToDrawImmediate)
{
  if ( myZLayers.NbStructures() <= 0 )
    return;

  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  Standard_Boolean toRenderGL = theToDrawImmediate ||
    myRenderParams.Method != Graphic3d_RM_RAYTRACING ||
    myRaytraceInitStatus == OpenGl_RT_FAIL ||
    aCtx->IsFeedback();

  if (!toRenderGL)
  {
    toRenderGL = !initRaytraceResources (aCtx) ||
      !updateRaytraceGeometry (OpenGl_GUM_CHECK, myId, aCtx);

    toRenderGL |= !myIsRaytraceDataValid; // if no ray-trace data use OpenGL

    if (!toRenderGL)
    {
      const Standard_Integer aSizeX = theReadDrawFbo != NULL ? theReadDrawFbo->GetVPSizeX() : myWindow->Width();
      const Standard_Integer aSizeY = theReadDrawFbo != NULL ? theReadDrawFbo->GetVPSizeY() : myWindow->Height();
      myOpenGlFBO ->InitLazy (aCtx, aSizeX, aSizeY, myFboColorFormat, myFboDepthFormat, 0);

      if (myRaytraceFilter.IsNull())
        myRaytraceFilter = new OpenGl_RaytraceFilter;

      myRaytraceFilter->SetPrevRenderFilter (myWorkspace->GetRenderFilter());

      if (theReadDrawFbo != NULL)
        theReadDrawFbo->UnbindBuffer (aCtx);

      // Prepare preliminary OpenGL output
      if (aCtx->arbFBOBlit != NULL)
      {
        // Render bottom OSD layer
        myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_Bottom);

        myWorkspace->SetRenderFilter (myRaytraceFilter);
        {
          if (theReadDrawFbo != NULL)
          {
            theReadDrawFbo->BindDrawBuffer (aCtx);
          }
          else
          {
            aCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
          }

          // Render non-polygonal elements in default layer
          myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_Default);
        }
        myWorkspace->SetRenderFilter (myRaytraceFilter->PrevRenderFilter());
      }

      if (theReadDrawFbo != NULL)
      {
        theReadDrawFbo->BindBuffer (aCtx);
      }
      else
      {
        aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, 0);
      }

      // Reset OpenGl aspects state to default to avoid enabling of
      // backface culling which is not supported in ray-tracing.
      myWorkspace->ResetAppliedAspect();

      // Ray-tracing polygonal primitive arrays
      raytrace (aSizeX, aSizeY, theProjection, theReadDrawFbo, aCtx);

      // Render upper (top and topmost) OpenGL layers
      myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_Upper);
    }
  }

  // Redraw 3D scene using OpenGL in standard
  // mode or in case of ray-tracing failure
  if (toRenderGL)
  {
    myZLayers.Render (myWorkspace, theToDrawImmediate, OpenGl_LF_All);

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
                               const Standard_Boolean       theToDrawImmediate)
{
  const Handle(OpenGl_Context)& aContext = myWorkspace->GetGlContext();

  // Specify clipping planes in view transformation space
  aContext->ChangeClipping().Reset (aContext, myClipPlanes);
  if (!myClipPlanes.IsNull()
   && !myClipPlanes->IsEmpty())
  {
    aContext->ShaderManager()->UpdateClippingState();
  }

#if !defined(GL_ES_VERSION_2_0)
  // Apply Lights
  if (aContext->core11 != NULL)
  {
    // setup lights
    Graphic3d_Vec4 anAmbientColor (THE_DEFAULT_AMBIENT[0],
                                   THE_DEFAULT_AMBIENT[1],
                                   THE_DEFAULT_AMBIENT[2],
                                   THE_DEFAULT_AMBIENT[3]);
    GLenum aLightGlId = GL_LIGHT0;

    OpenGl_ListOfLight::Iterator aLightIt (myShadingModel == Graphic3d_TOSM_NONE ? myNoShadingLight : myLights);
    for (; aLightIt.More(); aLightIt.Next())
    {
      bindLight (aLightIt.Value(), aLightGlId, anAmbientColor, myWorkspace);
    }

    // apply accumulated ambient color
    anAmbientColor.a() = 1.0f;
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, anAmbientColor.GetData());

    if (aLightGlId != GL_LIGHT0)
    {
      glEnable (GL_LIGHTING);
    }
    // switch off unused lights
    for (; aLightGlId <= GL_LIGHT7; ++aLightGlId)
    {
      glDisable (aLightGlId);
    }
  }
#endif

  // Clear status bitfields
  myWorkspace->NamedStatus &= ~(OPENGL_NS_2NDPASSNEED | OPENGL_NS_2NDPASSDO);

  // First pass
  renderStructs (theProjection, theReadDrawFbo, theToDrawImmediate);
  myWorkspace->DisableTexture();

  // Second pass
  if (myWorkspace->NamedStatus & OPENGL_NS_2NDPASSNEED)
  {
    myWorkspace->NamedStatus |= OPENGL_NS_2NDPASSDO;

    // Remember OpenGl properties
    GLint aSaveBlendDst = GL_ONE_MINUS_SRC_ALPHA, aSaveBlendSrc = GL_SRC_ALPHA;
    GLint aSaveZbuffFunc;
    GLboolean aSaveZbuffWrite;
    glGetBooleanv (GL_DEPTH_WRITEMASK, &aSaveZbuffWrite);
    glGetIntegerv (GL_DEPTH_FUNC, &aSaveZbuffFunc);
  #if !defined(GL_ES_VERSION_2_0)
    glGetIntegerv (GL_BLEND_DST, &aSaveBlendDst);
    glGetIntegerv (GL_BLEND_SRC, &aSaveBlendSrc);
  #endif
    GLboolean wasZbuffEnabled = glIsEnabled (GL_DEPTH_TEST);
    GLboolean wasBlendEnabled = glIsEnabled (GL_BLEND);

    // Change the properties for second rendering pass
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);

    glDepthFunc (GL_EQUAL);
    glDepthMask (GL_FALSE);
    glEnable (GL_DEPTH_TEST);

    // Render the view
    renderStructs (theProjection, theReadDrawFbo, theToDrawImmediate);
    myWorkspace->DisableTexture();

    // Restore properties back
    glBlendFunc (aSaveBlendSrc, aSaveBlendDst);
    if (!wasBlendEnabled)
      glDisable (GL_BLEND);

    glDepthFunc (aSaveZbuffFunc);
    glDepthMask (aSaveZbuffWrite);
    if (!wasZbuffEnabled)
      glDisable (GL_DEPTH_FUNC);
  }

  // Apply restored view matrix.
  aContext->ApplyWorldViewMatrix();

  aContext->ChangeClipping().Reset (aContext, Handle(Graphic3d_SequenceOfHClipPlane)());
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
  }
#if !defined(GL_ES_VERSION_2_0)
  aCtx->core20fwd->glClearDepth  (1.0);
#else
  aCtx->core20fwd->glClearDepthf (1.0f);
#endif
  aCtx->core20fwd->glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

#if !defined(GL_ES_VERSION_2_0)
  if (aCtx->arbFBOBlit != NULL
   && theReadFbo->NbSamples() != 0)
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
    }

    // we don't copy stencil buffer here... does it matter for performance?
    aCtx->arbFBOBlit->glBlitFramebuffer (0, 0, theReadFbo->GetVPSizeX(), theReadFbo->GetVPSizeY(),
                                         0, 0, theReadFbo->GetVPSizeX(), theReadFbo->GetVPSizeY(),
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
    }
  }
  else
#endif
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

    myWorkspace->DisableTexture();

    OpenGl_VertexBuffer* aVerts = initBlitQuad (theToFlip);
    const Handle(OpenGl_ShaderManager)& aManager = aCtx->ShaderManager();
    if (aVerts->IsValid()
     && aManager->BindFboBlitProgram())
    {
      theReadFbo->ColorTexture()       ->Bind   (aCtx, GL_TEXTURE0 + 0);
      theReadFbo->DepthStencilTexture()->Bind   (aCtx, GL_TEXTURE0 + 1);
      aVerts->BindVertexAttrib (aCtx, Graphic3d_TOA_POS);

      aCtx->core20fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

      aVerts->UnbindVertexAttrib (aCtx, Graphic3d_TOA_POS);
      theReadFbo->DepthStencilTexture()->Unbind (aCtx, GL_TEXTURE0 + 1);
      theReadFbo->ColorTexture()       ->Unbind (aCtx, GL_TEXTURE0 + 0);
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

  myWorkspace->DisableTexture();
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

    aPair[0]->ColorTexture()->Bind (aCtx, GL_TEXTURE0 + 0);
    aPair[1]->ColorTexture()->Bind (aCtx, GL_TEXTURE0 + 1);
    aVerts->BindVertexAttrib (aCtx, 0);

    aCtx->core20fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);

    aVerts->UnbindVertexAttrib (aCtx, 0);
    aPair[1]->ColorTexture()->Unbind (aCtx, GL_TEXTURE0 + 1);
    aPair[0]->ColorTexture()->Unbind (aCtx, GL_TEXTURE0 + 0);
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
void OpenGl_View::copyBackToFront()
{
#if !defined(GL_ES_VERSION_2_0)

  OpenGl_Mat4 aProjectMat;
  Graphic3d_TransformUtils::Ortho2D (aProjectMat,
    0.f, static_cast<GLfloat> (myWindow->Width()), 0.f, static_cast<GLfloat> (myWindow->Height()));

  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  aCtx->WorldViewState.Push();
  aCtx->ProjectionState.Push();

  aCtx->WorldViewState.SetIdentity();
  aCtx->ProjectionState.SetCurrent (aProjectMat);

  aCtx->ApplyProjectionMatrix();
  aCtx->ApplyWorldViewMatrix();

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

  glRasterPos2i (0, 0);
  glCopyPixels  (0, 0, myWindow->Width() + 1, myWindow->Height() + 1, GL_COLOR);
  //glCopyPixels  (0, 0, myWidth + 1, myHeight + 1, GL_DEPTH);

  aCtx->EnableFeatures();

  aCtx->WorldViewState.Pop();
  aCtx->ProjectionState.Pop();
  aCtx->ApplyProjectionMatrix();

  // read/write from front buffer now
  aCtx->SetReadBuffer (aCtx->DrawBuffer());
#endif
  myIsImmediateDrawn = Standard_False;
}
