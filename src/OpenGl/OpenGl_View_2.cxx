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
#include <OpenGl_tgl_funcs.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Image_AlienPixMap.hxx>
#include <Visual3d_Layer.hxx>

#include <NCollection_Mat4.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_PrimitiveArray.hxx>
#include <OpenGl_PrinterContext.hxx>
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

};

extern void InitLayerProp (const int theListId); //szvgl: defined in OpenGl_GraphicDriver_Layer.cxx

/*----------------------------------------------------------------------*/
/*
* Fonctions privees
*/

#if !defined(GL_ES_VERSION_2_0)
/*-----------------------------------------------------------------*/
/*
*  Set des lumieres
*/
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

  if (theLight.Type == Visual3d_TOLS_AMBIENT)
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
    case Visual3d_TOLS_DIRECTIONAL:
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
    case Visual3d_TOLS_POSITIONAL:
    {
      // to create a realistic effect, set the GL_SPECULAR parameter to the same value as the GL_DIFFUSE
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              theLight.Position.GetData());
      glLightfv (theLightGlId, GL_SPOT_DIRECTION,        THE_DEFAULT_SPOT_DIR);
      glLightf  (theLightGlId, GL_SPOT_EXPONENT,         THE_DEFAULT_SPOT_EXPONENT);
      glLightf  (theLightGlId, GL_SPOT_CUTOFF,           THE_DEFAULT_SPOT_CUTOFF);
      glLightf  (theLightGlId, GL_CONSTANT_ATTENUATION,  theLight.ConstAttenuation());
      glLightf  (theLightGlId, GL_LINEAR_ATTENUATION,    theLight.LinearAttenuation());
      glLightf  (theLightGlId, GL_QUADRATIC_ATTENUATION, 0.0);
      break;
    }
    case Visual3d_TOLS_SPOT:
    {
      glLightfv (theLightGlId, GL_AMBIENT,               THE_DEFAULT_AMBIENT);
      glLightfv (theLightGlId, GL_DIFFUSE,               theLight.Color.GetData());
      glLightfv (theLightGlId, GL_SPECULAR,              theLight.Color.GetData());
      glLightfv (theLightGlId, GL_POSITION,              theLight.Position.GetData());
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

/*----------------------------------------------------------------------*/

void OpenGl_View::DrawBackground (const Handle(OpenGl_Workspace)& theWorkspace)
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  if ((theWorkspace->NamedStatus & OPENGL_NS_WHITEBACK) != 0 // no background
    || (!myBgTextureArray->IsDefined()                       // no texture
     && !myBgGradientArray->IsDefined()))                    // no gradient
  {
    return;
  }

  aCtx->core11fwd->glDisable (GL_DEPTH_TEST);

  aCtx->WorldViewState.Push();
  aCtx->ProjectionState.Push();
  aCtx->WorldViewState.SetIdentity();
  aCtx->ProjectionState.SetIdentity();
  aCtx->ApplyProjectionMatrix();
  aCtx->ApplyWorldViewMatrix();

  // Drawing background gradient if:
  // - gradient fill type is not Aspect_GFM_NONE and
  // - either background texture is no specified or it is drawn in Aspect_FM_CENTERED mode
  if (myBgGradientArray->IsDefined()
    && (!myTextureParams->DoTextureMap()
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

    if (myBgGradientArray->IsDataChanged())
    {
      myBgGradientArray->Init (theWorkspace);
    }

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
   && myTextureParams->DoTextureMap())
  {
    aCtx->core11fwd->glDisable (GL_BLEND);

    const OpenGl_AspectFace* anOldAspectFace = theWorkspace->SetAspectFace (myTextureParams);

    if (myBgTextureArray->IsDataChanged()
     || myBgTextureArray->IsViewSizeChanged (theWorkspace))
    {
      myBgTextureArray->Init (theWorkspace);
    }

    myBgTextureArray->Render (theWorkspace);

    // restore aspects
    theWorkspace->SetAspectFace (anOldAspectFace);
  }

  aCtx->WorldViewState.Pop();
  aCtx->ProjectionState.Pop();
  aCtx->ApplyProjectionMatrix();
  aCtx->ApplyWorldViewMatrix();

  if (theWorkspace->UseZBuffer())
  {
    aCtx->core11fwd->glEnable (GL_DEPTH_TEST);
  }
}

/*----------------------------------------------------------------------*/

//call_func_redraw_all_structs_proc
void OpenGl_View::Render (const Handle(OpenGl_PrinterContext)& thePrintContext,
                          const Handle(OpenGl_Workspace)&      theWorkspace,
                          OpenGl_FrameBuffer*                  theOutputFBO,
                          Graphic3d_Camera::Projection         theProjection,
                          const Graphic3d_CView&               theCView,
                          const Aspect_CLayer2d&               theCUnderLayer,
                          const Aspect_CLayer2d&               theCOverLayer,
                          const Standard_Boolean               theToDrawImmediate)
{
  // ==================================
  //      Step 1: Prepare for redraw
  // ==================================

  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

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
  Standard_Boolean isProjectionMatUpdateNeeded  = Standard_False;
  Standard_Boolean isOrientationMatUpdateNeeded = Standard_False;
  if (myBVHSelector.ProjectionState() != myCamera->ProjectionState())
  {
    isProjectionMatUpdateNeeded = Standard_True;
    myBVHSelector.ChangeProjectionState() = myCamera->ProjectionState();
  }
  if (myBVHSelector.ModelViewState() != myCamera->ModelViewState())
  {
    isOrientationMatUpdateNeeded = Standard_True;
    myBVHSelector.ChangeModelViewState() = myCamera->ModelViewState();
  }

  if (isProjectionMatUpdateNeeded
   || isOrientationMatUpdateNeeded)
  {
    myBVHSelector.SetViewVolume (myCamera);
  }

  const Handle(OpenGl_ShaderManager)& aManager   = aContext->ShaderManager();
  const Standard_Boolean              isSameView = aManager->IsSameView (this); // force camera state update when needed
  if (StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index()) != myLastLightSourceState)
  {
    aManager->UpdateLightSourceStateTo (&myLights);
    myLastLightSourceState = StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index());
  }

  if (myProjectionState != myCamera->ProjectionState()
  || !isSameView)
  {
    myProjectionState = myCamera->ProjectionState();
    aContext->ProjectionState.SetCurrent (myCamera->ProjectionMatrixF());
    aContext->ApplyProjectionMatrix();
  }

  if (myModelViewState != myCamera->ModelViewState()
  || !isSameView)
  {
    myModelViewState = myCamera->ModelViewState();
    aContext->WorldViewState.SetCurrent (myCamera->OrientationMatrixF());
    aContext->ApplyWorldViewMatrix();
  }

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
    DrawBackground (theWorkspace);
  }

#if !defined(GL_ES_VERSION_2_0)
  // Switch off lighting by default
  if (aContext->core11 != NULL)
  {
    glDisable(GL_LIGHTING);
  }
#endif

  // =================================
  //      Step 3: Draw underlayer
  // =================================
  if (!theToDrawImmediate)
  {
    RedrawLayer2d (thePrintContext, theWorkspace, theCView, theCUnderLayer);
  }

  // =================================
  //      Step 4: Redraw main plane
  // =================================

  // Setup face culling
  GLboolean isCullFace = GL_FALSE;
  if ( myBackfacing )
  {
    isCullFace = glIsEnabled( GL_CULL_FACE );
    if ( myBackfacing < 0 )
    {
      glEnable( GL_CULL_FACE );
      glCullFace( GL_BACK );
    }
    else
      glDisable( GL_CULL_FACE );
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

  // Apply Fog
  if (myFog.IsOn
   && aContext->core11 != NULL)
  {
    Standard_Real aFogFrontConverted = (Standard_Real )myFog.Front + myCamera->Distance();
    if (myCamera->ZFar() < aFogFrontConverted)
    {
      aFogFrontConverted = myCamera->ZFar();
      myFog.Front = (Standard_ShortReal )(aFogFrontConverted - myCamera->Distance());
    }

    Standard_Real aFogBackConverted = (Standard_Real )myFog.Back + myCamera->Distance();
    if (myCamera->ZFar() < aFogFrontConverted)
    {
      aFogBackConverted = myCamera->ZFar();
      myFog.Back = (Standard_ShortReal )(aFogBackConverted - myCamera->Distance());
    }

    if (aFogFrontConverted > aFogBackConverted)
    {
      myFog.Front = (Standard_ShortReal )(aFogFrontConverted - myCamera->Distance());
      myFog.Back = (Standard_ShortReal )(aFogBackConverted - myCamera->Distance());
    }

    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, (Standard_ShortReal )aFogFrontConverted);
    glFogf(GL_FOG_END, (Standard_ShortReal )aFogBackConverted);
    glFogfv(GL_FOG_COLOR, myFog.Color.rgb);
    glEnable(GL_FOG);
  }
  else if (aContext->core11 != NULL)
  {
    glDisable (GL_FOG);
  }

  // Apply InteriorShadingMethod
  if (aContext->core11 != NULL)
  {
    aContext->core11->glShadeModel (myShadingModel == Visual3d_TOM_FACET
                                 || myShadingModel == Visual3d_TOM_NONE ? GL_FLAT : GL_SMOOTH);
  }
#endif

  aManager->SetShadingModel (myShadingModel);

  // Apply AntiAliasing
  if (myAntiAliasing)
    theWorkspace->NamedStatus |= OPENGL_NS_ANTIALIASING;
  else
    theWorkspace->NamedStatus &= ~OPENGL_NS_ANTIALIASING;

  if (!aManager->IsEmpty())
  {
    aManager->UpdateClippingState();
  }

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
  RedrawScene (thePrintContext, theWorkspace, theOutputFBO, theCView, theToDrawImmediate);

  // ===============================
  //      Step 5: Trihedron
  // ===============================

  // Resetting GL parameters according to the default aspects
  // in order to synchronize GL state with the graphic driver state
  // before drawing auxiliary stuff (trihedrons, overlayer)
  // and invoking optional callbacks
  theWorkspace->ResetAppliedAspect();

  aContext->ChangeClipping().RemoveAll();

  if (!aManager->IsEmpty())
  {
    aManager->ResetMaterialStates();
    aManager->RevertClippingState();

    // We need to disable (unbind) all shaders programs to ensure
    // that all objects without specified aspect will be drawn
    // correctly (such as background)
    aContext->BindProgram (NULL);
  }

  // Render trihedron
  if (!theToDrawImmediate)
  {
    RedrawTrihedron (theWorkspace);

    // Restore face culling
    if ( myBackfacing )
    {
      if ( isCullFace )
      {
        glEnable   ( GL_CULL_FACE );
        glCullFace ( GL_BACK      );
      }
      else
        glDisable ( GL_CULL_FACE );
    }
  }

  // ===============================
  //      Step 6: Redraw overlay
  // ===============================
  if (!theToDrawImmediate)
  {
    const int aMode = 0;
    theWorkspace->DisplayCallback (theCView, (aMode | OCC_PRE_OVERLAY));

    RedrawLayer2d (thePrintContext, theWorkspace, theCView, theCOverLayer);

    theWorkspace->DisplayCallback (theCView, aMode);
  }

  // ==============================================================
  //      Step 7: Keep shader manager informed about last View
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

/*----------------------------------------------------------------------*/

//ExecuteViewDisplay
void OpenGl_View::RenderStructs (const Handle(OpenGl_Workspace)& theWorkspace,
                                 OpenGl_FrameBuffer*             theReadDrawFbo,
                                 const Graphic3d_CView&          theCView,
                                 const Standard_Boolean          theToDrawImmediate)
{
  if ( myZLayers.NbStructures() <= 0 )
    return;

  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  if ( (theWorkspace->NamedStatus & OPENGL_NS_2NDPASSNEED) == 0 )
  {
  #if !defined(GL_ES_VERSION_2_0)
    const int anAntiAliasingMode = theWorkspace->AntiAliasingMode();
  #endif

    if ( !myAntiAliasing )
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (aCtx->core11 != NULL)
      {
        glDisable (GL_POINT_SMOOTH);
      }
      glDisable(GL_LINE_SMOOTH);
      if( anAntiAliasingMode & 2 ) glDisable(GL_POLYGON_SMOOTH);
    #endif
      glBlendFunc (GL_ONE, GL_ZERO);
      glDisable (GL_BLEND);
    }
    else
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (aCtx->core11 != NULL)
      {
        glEnable(GL_POINT_SMOOTH);
      }
      glEnable(GL_LINE_SMOOTH);
      if( anAntiAliasingMode & 2 ) glEnable(GL_POLYGON_SMOOTH);
    #endif
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_BLEND);
    }
  }

  Standard_Boolean toRenderGL = theToDrawImmediate ||
    theCView.RenderParams.Method != Graphic3d_RM_RAYTRACING || myRaytraceInitStatus == OpenGl_RT_FAIL;

  if (!toRenderGL)
  {
    toRenderGL = !initRaytraceResources (theCView, aCtx) ||
      !updateRaytraceGeometry (OpenGl_GUM_CHECK, theWorkspace->ActiveViewId(), aCtx);

    toRenderGL |= !myIsRaytraceDataValid; // if no ray-trace data use OpenGL

    if (!toRenderGL)
    {
      const Standard_Integer aSizeX = theReadDrawFbo != NULL ?
        theReadDrawFbo->GetVPSizeX() : theWorkspace->Width();
      const Standard_Integer aSizeY = theReadDrawFbo != NULL ?
        theReadDrawFbo->GetVPSizeY() : theWorkspace->Height();

      if (myOpenGlFBO.IsNull())
        myOpenGlFBO = new OpenGl_FrameBuffer;

      if (myOpenGlFBO->GetVPSizeX() != aSizeX
       || myOpenGlFBO->GetVPSizeY() != aSizeY)
      {
        myOpenGlFBO->Init (aCtx, aSizeX, aSizeY);
      }

      if (myRaytraceFilter.IsNull())
        myRaytraceFilter = new OpenGl_RaytraceFilter;

      myRaytraceFilter->SetPrevRenderFilter (theWorkspace->GetRenderFilter());

      if (theReadDrawFbo != NULL)
        theReadDrawFbo->UnbindBuffer (aCtx);

      // Prepare preliminary OpenGL output
      if (aCtx->arbFBOBlit != NULL)
      {
        // Render bottom OSD layer
        myZLayers.Render (theWorkspace, theToDrawImmediate, OpenGl_LF_Bottom);

        theWorkspace->SetRenderFilter (myRaytraceFilter);
        {
          if (theReadDrawFbo != NULL)
          {
            theReadDrawFbo->BindReadBuffer (aCtx);
          }
          else
          {
            aCtx->arbFBO->glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
          }

          myOpenGlFBO->BindDrawBuffer (aCtx);

          aCtx->arbFBOBlit->glBlitFramebuffer (0, 0, aSizeX, aSizeY,
                                               0, 0, aSizeX, aSizeY,
                                               GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                                               GL_NEAREST);

          // Render non-polygonal elements in default layer
          myZLayers.Render (theWorkspace, theToDrawImmediate, OpenGl_LF_Default);
        }
        theWorkspace->SetRenderFilter (myRaytraceFilter->PrevRenderFilter());
      }

      if (theReadDrawFbo != NULL)
      {
        theReadDrawFbo->BindBuffer (aCtx);
      }
      else
      {
        aCtx->arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, 0);
      }

      // Ray-tracing polygonal primitive arrays
      raytrace (theCView, aSizeX, aSizeY, theReadDrawFbo, aCtx);

      // Render upper (top and topmost) OpenGL layers
      myZLayers.Render (theWorkspace, theToDrawImmediate, OpenGl_LF_Upper);
    }
  }

  // Redraw 3D scene using OpenGL in standard
  // mode or in case of ray-tracing failure
  if (toRenderGL)
  {
    myZLayers.Render (theWorkspace, theToDrawImmediate, OpenGl_LF_All);

    // Set flag that scene was redrawn by standard pipeline
    theCView.WasRedrawnGL = Standard_True;
  }
}

/*----------------------------------------------------------------------*/

//call_togl_redraw_layer2d
void OpenGl_View::RedrawLayer2d (const Handle(OpenGl_PrinterContext)& thePrintContext,
                                 const Handle(OpenGl_Workspace)&      theWorkspace,
                                 const Graphic3d_CView&               /*ACView*/,
                                 const Aspect_CLayer2d&               ACLayer)
{
#if !defined(GL_ES_VERSION_2_0)
  if (&ACLayer == NULL
   || ACLayer.ptrLayer == NULL
   || ACLayer.ptrLayer->listIndex == 0) return;

  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  if (aContext->core11 == NULL)
  {
    return;
  }

  GLsizei dispWidth  = (GLsizei )ACLayer.viewport[0];
  GLsizei dispHeight = (GLsizei )ACLayer.viewport[1];

  aContext->WorldViewState.Push();
  aContext->ProjectionState.Push();

  aContext->WorldViewState.SetIdentity();
  aContext->ProjectionState.SetIdentity();

  aContext->ApplyWorldViewMatrix();
  aContext->ApplyProjectionMatrix();

  if (!ACLayer.sizeDependent)
    aContext->core11fwd->glViewport (0, 0, dispWidth, dispHeight);

  float left = ACLayer.ortho[0];
  float right = ACLayer.ortho[1];
  float bottom = ACLayer.ortho[2];
  float top = ACLayer.ortho[3];

  int attach = ACLayer.attach;

  const float ratio = !ACLayer.sizeDependent
                    ? float(dispWidth) / float(dispHeight)
                    : float(theWorkspace->Width()) / float(theWorkspace->Height());

  float delta;
  if (ratio >= 1.0) {
    delta = (float )((top - bottom)/2.0);
    switch (attach) {
      case 0: /* Aspect_TOC_BOTTOM_LEFT */
        top = bottom + 2*delta/ratio;
        break;
      case 1: /* Aspect_TOC_BOTTOM_RIGHT */
        top = bottom + 2*delta/ratio;
        break;
      case 2: /* Aspect_TOC_TOP_LEFT */
        bottom = top - 2*delta/ratio;
        break;
      case 3: /* Aspect_TOC_TOP_RIGHT */
        bottom = top - 2*delta/ratio;
        break;
    }
  }
  else {
    delta = (float )((right - left)/2.0);
    switch (attach) {
      case 0: /* Aspect_TOC_BOTTOM_LEFT */
        right = left + 2*delta*ratio;
        break;
      case 1: /* Aspect_TOC_BOTTOM_RIGHT */
        left = right - 2*delta*ratio;
        break;
      case 2: /* Aspect_TOC_TOP_LEFT */
        right = left + 2*delta*ratio;
        break;
      case 3: /* Aspect_TOC_TOP_RIGHT */
        left = right - 2*delta*ratio;
        break;
    }
  }

#ifdef _WIN32
  // Check printer context that exists only for print operation
  if (!thePrintContext.IsNull())
  {
    // additional transformation matrix could be applied to
    // render only those parts of viewport that will be
    // passed to a printer as a current "frame" to provide
    // tiling; scaling of graphics by matrix helps render a
    // part of a view (frame) in same viewport, but with higher
    // resolution

    // set printing scale/tiling transformation
    aContext->ProjectionState.SetCurrent (thePrintContext->ProjTransformation());
    aContext->ApplyProjectionMatrix();

    // printing operation also assumes other viewport dimension
    // to comply with transformation matrix or graphics scaling
    // factors for tiling for layer redraw
    GLsizei anViewportX = 0;
    GLsizei anViewportY = 0;
    thePrintContext->GetLayerViewport (anViewportX, anViewportY);
    if (anViewportX != 0 && anViewportY != 0)
      aContext->core11fwd->glViewport (0, 0, anViewportX, anViewportY);
  }
#endif

  glOrtho (left, right, bottom, top, -1.0, 1.0);

  glPushAttrib (
    GL_LIGHTING_BIT | GL_LINE_BIT | GL_POLYGON_BIT |
    GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT );

  glDisable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_1D);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_LIGHTING);

  // TODO: Obsolete code, the display list is always empty now, to be removed
  glCallList (ACLayer.ptrLayer->listIndex);

  //calling dynamic render of LayerItems
  if ( ACLayer.ptrLayer->layerData )
  {
    InitLayerProp (ACLayer.ptrLayer->listIndex);
    ((Visual3d_Layer*)ACLayer.ptrLayer->layerData)->RenderLayerItems();
    InitLayerProp (0);
  }

  glPopAttrib ();

  aContext->WorldViewState.Pop();
  aContext->ProjectionState.Pop();

  aContext->ApplyProjectionMatrix();
  aContext->ApplyWorldViewMatrix();

  if (!ACLayer.sizeDependent)
    aContext->core11fwd->glViewport (0, 0, theWorkspace->Width(), theWorkspace->Height());

  glFlush ();
#endif
}

/*----------------------------------------------------------------------*/

void OpenGl_View::RedrawTrihedron (const Handle(OpenGl_Workspace) &theWorkspace)
{
  // display global trihedron
  if (myToShowTrihedron)
  {
    myTrihedron.Render (theWorkspace);
  }
  if (myToShowGradTrihedron)
  {
    myGraduatedTrihedron.Render (theWorkspace);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_create_bg_texture
void OpenGl_View::CreateBackgroundTexture (const Standard_CString  theFilePath,
                                           const Aspect_FillMethod theFillStyle)
{
  // Prepare aspect for texture storage
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

  myBgTextureArray->SetTextureParameters (theFillStyle);
}

/*----------------------------------------------------------------------*/

//call_togl_set_bg_texture_style
void OpenGl_View::SetBackgroundTextureStyle (const Aspect_FillMethod theFillStyle)
{
  myBgTextureArray->SetTextureFillMethod (theFillStyle);
}

/*----------------------------------------------------------------------*/

//call_togl_gradient_background
void OpenGl_View::SetBackgroundGradient (const Quantity_Color& theColor1,
                                        const Quantity_Color& theColor2,
                                        const Aspect_GradientFillMethod theType)
{
  myBgGradientArray->SetGradientParameters (theColor1, theColor2, theType);
}

/*----------------------------------------------------------------------*/

//call_togl_set_gradient_type
void OpenGl_View::SetBackgroundGradientType (const Aspect_GradientFillMethod theType)
{
  myBgGradientArray->SetGradientFillMethod (theType);
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
//function : DisplayStructure
//purpose  :
//=======================================================================

void OpenGl_View::DisplayStructure (const Handle(Graphic3d_Structure)& theStructure,
                                    const Standard_Integer             thePriority)
{
  const OpenGl_Structure*  aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure->CStructure().operator->());
  const Graphic3d_ZLayerId aZLayer = aStruct->ZLayer();
  myZLayers.AddStructure (aStruct, aZLayer, thePriority);
}

//=======================================================================
//function : DisplayImmediateStructure
//purpose  :
//=======================================================================

void OpenGl_View::DisplayImmediateStructure (const Handle(Graphic3d_Structure)& theStructure)
{
  const OpenGl_Structure* aStruct = reinterpret_cast<const OpenGl_Structure*> (theStructure->CStructure().operator->());
  for (OpenGl_SequenceOfStructure::Iterator anIter (myImmediateList);
       anIter.More(); anIter.Next())
  {
    if (anIter.Value() == aStruct)
    {
      return;
    }
  }

  myImmediateList.Append (aStruct);
}

//=======================================================================
//function : EraseStructure
//purpose  :
//=======================================================================

void OpenGl_View::EraseStructure (const Handle(Graphic3d_Structure)& theStructure)
{
  myZLayers.RemoveStructure (theStructure);
}

//=======================================================================
//function : EraseImmediateStructure
//purpose  :
//=======================================================================

void OpenGl_View::EraseImmediateStructure (const OpenGl_Structure* theStructure)
{
  for (OpenGl_SequenceOfStructure::Iterator anIter (myImmediateList);
       anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theStructure)
    {
      myImmediateList.Remove (anIter);
      return;
    }
  }
}

//=======================================================================
//function : ChangeZLayer
//purpose  :
//=======================================================================

void OpenGl_View::ChangeZLayer (const OpenGl_Structure*  theStructure,
                                const Graphic3d_ZLayerId theNewLayerId)
{
  const Graphic3d_ZLayerId anOldLayer = theStructure->ZLayer();
  myZLayers.ChangeLayer (theStructure, anOldLayer, theNewLayerId);
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
//function : ChangePriority
//purpose  :
//=======================================================================
void OpenGl_View::ChangePriority (const OpenGl_Structure *theStructure,
                                  const Standard_Integer theNewPriority)
{
  const Graphic3d_ZLayerId aLayerId = theStructure->ZLayer();
  myZLayers.ChangePriority (theStructure, aLayerId, theNewPriority);
}

//=======================================================================
//function : RedrawScene
//purpose  :
//=======================================================================

void OpenGl_View::RedrawScene (const Handle(OpenGl_PrinterContext)& thePrintContext,
                               const Handle(OpenGl_Workspace)&      theWorkspace,
                               OpenGl_FrameBuffer*                  theReadDrawFbo,
                               const Graphic3d_CView&               theCView,
                               const Standard_Boolean               theToDrawImmediate)
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  if (myZClip.Back.IsOn || myZClip.Front.IsOn)
  {
    Handle(Graphic3d_ClipPlane) aPlaneBack;
    Handle(Graphic3d_ClipPlane) aPlaneFront;

    if (myZClip.Back.IsOn)
    {
      Standard_Real aClipBackConverted = (Standard_Real )myZClip.Front.Limit + myCamera->Distance();
      if (myCamera->ZFar() < aClipBackConverted)
      {
        aClipBackConverted = myCamera->ZFar();
        myZClip.Back.Limit = (Standard_ShortReal )(aClipBackConverted - myCamera->Distance());
      }
      const Graphic3d_ClipPlane::Equation aBackEquation (0.0, 0.0, 1.0, (Standard_ShortReal )aClipBackConverted);
      aPlaneBack = new Graphic3d_ClipPlane (aBackEquation);
    }

    if (myZClip.Front.IsOn)
    {
      Standard_Real aClipFrontConverted = (Standard_Real )myZClip.Front.Limit + myCamera->Distance();
      if (myCamera->ZNear() > aClipFrontConverted)
      {
        aClipFrontConverted = myCamera->ZNear();
        myZClip.Front.Limit = (Standard_ShortReal )(aClipFrontConverted - myCamera->Distance());
      }
      const Graphic3d_ClipPlane::Equation aFrontEquation (0.0, 0.0, -1.0, (Standard_ShortReal )-aClipFrontConverted);
      aPlaneFront = new Graphic3d_ClipPlane (aFrontEquation);
    }

    // Specify slicing planes with identity transformation
    if (!aPlaneBack.IsNull() || !aPlaneFront.IsNull())
    {
      Graphic3d_SequenceOfHClipPlane aSlicingPlanes;
      if (!aPlaneBack.IsNull())
      {
        aSlicingPlanes.Append (aPlaneBack);
      }

      if (!aPlaneFront.IsNull())
      {
        aSlicingPlanes.Append (aPlaneFront);
      }

      // add planes at loaded view matrix state
      aContext->ChangeClipping().AddView (aSlicingPlanes, theWorkspace);
    }
  }

#ifdef _WIN32
  // set printing scale/tiling transformation
  if (!thePrintContext.IsNull())
  {
    aContext->ProjectionState.Push();
    aContext->ProjectionState.SetCurrent (thePrintContext->ProjTransformation() * aContext->ProjectionState.Current());
    aContext->ApplyProjectionMatrix();
  }
#endif

  // Specify clipping planes in view transformation space
  if (!myClipPlanes.IsEmpty())
  {
    Graphic3d_SequenceOfHClipPlane aUserPlanes;
    Graphic3d_SequenceOfHClipPlane::Iterator aClippingIt (myClipPlanes);
    for (; aClippingIt.More(); aClippingIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aClipPlane = aClippingIt.Value();
      if (aClipPlane->IsOn())
      {
        aUserPlanes.Append (aClipPlane);
      }
    }

    if (!aUserPlanes.IsEmpty())
    {
      aContext->ChangeClipping().AddWorld (aUserPlanes);
    }

    if (!aContext->ShaderManager()->IsEmpty())
    {
      aContext->ShaderManager()->UpdateClippingState();
    }
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
    for (OpenGl_ListOfLight::Iterator aLightIt (myLights);
         aLightIt.More(); aLightIt.Next())
    {
      bindLight (aLightIt.Value(), aLightGlId, anAmbientColor, theWorkspace);
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
  theWorkspace->NamedStatus &= ~(OPENGL_NS_2NDPASSNEED | OPENGL_NS_2NDPASSDO);

  // Update state of surface detail level
  theWorkspace->GetGlContext()->ShaderManager()->UpdateSurfaceDetailStateTo (mySurfaceDetail);

  // Added PCT for handling of textures
  switch (mySurfaceDetail)
  {
    case Visual3d_TOD_NONE:
      theWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;
      theWorkspace->DisableTexture();
      // Render the view
      RenderStructs (theWorkspace, theReadDrawFbo, theCView, theToDrawImmediate);
      break;

    case Visual3d_TOD_ENVIRONMENT:
      theWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;
      theWorkspace->EnableTexture (myTextureEnv);
      // Render the view
      RenderStructs (theWorkspace, theReadDrawFbo, theCView, theToDrawImmediate);
      theWorkspace->DisableTexture();
      break;

    case Visual3d_TOD_ALL:
      // First pass
      theWorkspace->NamedStatus &= ~OPENGL_NS_FORBIDSETTEX;
      // Render the view
      RenderStructs (theWorkspace, theReadDrawFbo, theCView, theToDrawImmediate);
      theWorkspace->DisableTexture();

      // Second pass
      if (theWorkspace->NamedStatus & OPENGL_NS_2NDPASSNEED)
      {
        theWorkspace->NamedStatus |= OPENGL_NS_2NDPASSDO;
        theWorkspace->EnableTexture (myTextureEnv);

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

        theWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;

        // Render the view
        RenderStructs (theWorkspace, theReadDrawFbo, theCView, theToDrawImmediate);
        theWorkspace->DisableTexture();

        // Restore properties back
        glBlendFunc (aSaveBlendSrc, aSaveBlendDst);
        if (!wasBlendEnabled)
          glDisable (GL_BLEND);

        glDepthFunc (aSaveZbuffFunc);
        glDepthMask (aSaveZbuffWrite);
        if (!wasZbuffEnabled)
          glDisable (GL_DEPTH_FUNC);
      }
      break;
  }

  // Apply restored view matrix.
  aContext->ApplyWorldViewMatrix();

#ifdef _WIN32
  // set printing scale/tiling transformation
  if (!thePrintContext.IsNull())
  {
    aContext->ProjectionState.Pop();
    aContext->ApplyProjectionMatrix();
  }
#endif

}
