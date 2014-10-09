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
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Structure.hxx>

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

struct OPENGL_CLIP_PLANE
{
  GLboolean isEnabled;
  GLdouble Equation[4];
  DEFINE_STANDARD_ALLOC
};

/*----------------------------------------------------------------------*/
/*
* Fonctions privees
*/

#if !defined(GL_ES_VERSION_2_0)
/*-----------------------------------------------------------------*/
/*
*  Set des lumieres
*/
static void bind_light (const OpenGl_Light& theLight,
                        GLenum&             theLightGlId,
                        Graphic3d_Vec4&     theAmbientColor)
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

  // the light is a headlight?
  GLint aMatrixModeOld = 0;
  if (theLight.IsHeadlight)
  {
    glGetIntegerv (GL_MATRIX_MODE, &aMatrixModeOld);
    glMatrixMode  (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
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
    glPopMatrix();
    glMatrixMode (aMatrixModeOld);
  }

  glEnable (theLightGlId++);
}
#endif

/*----------------------------------------------------------------------*/

void OpenGl_View::DrawBackground (OpenGl_Workspace& theWorkspace)
{
#if !defined(GL_ES_VERSION_2_0)
  if ( (theWorkspace.NamedStatus & OPENGL_NS_WHITEBACK) == 0 &&
       ( myBgTexture.TexId != 0 || myBgGradient.type != Aspect_GFM_NONE ) )
  {
    const Standard_Integer aViewWidth = theWorkspace.Width();
    const Standard_Integer aViewHeight = theWorkspace.Height();

    glPushAttrib( GL_ENABLE_BIT | GL_TEXTURE_BIT );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    if ( glIsEnabled( GL_DEPTH_TEST ) )
      glDisable( GL_DEPTH_TEST ); //push GL_ENABLE_BIT

    // drawing bg gradient if:
    // - gradient fill type is not Aspect_GFM_NONE and
    // - either background texture is no specified or it is drawn in Aspect_FM_CENTERED mode
    if ( ( myBgGradient.type != Aspect_GFM_NONE ) &&
      ( myBgTexture.TexId == 0 || myBgTexture.Style == Aspect_FM_CENTERED ||
      myBgTexture.Style == Aspect_FM_NONE ) )
    {
      Tfloat* corner1 = 0;/* -1,-1*/
      Tfloat* corner2 = 0;/*  1,-1*/
      Tfloat* corner3 = 0;/*  1, 1*/
      Tfloat* corner4 = 0;/* -1, 1*/
      Tfloat dcorner1[3];
      Tfloat dcorner2[3];

      switch( myBgGradient.type )
      {
      case Aspect_GFM_HOR:
        corner1 = myBgGradient.color1.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color1.rgb;
        break;
      case Aspect_GFM_VER:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color1.rgb;
        corner4 = myBgGradient.color1.rgb;
        break;
      case Aspect_GFM_DIAG1:
        corner2 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color1.rgb;
        dcorner1 [0] = dcorner2[0] = 0.5F * (corner2[0] + corner4[0]);
        dcorner1 [1] = dcorner2[1] = 0.5F * (corner2[1] + corner4[1]);
        dcorner1 [2] = dcorner2[2] = 0.5F * (corner2[2] + corner4[2]);
        corner1 = dcorner1;
        corner3 = dcorner2;
        break;
      case Aspect_GFM_DIAG2:
        corner1 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color1.rgb;
        dcorner1 [0] = dcorner2[0] = 0.5F * (corner1[0] + corner3[0]);
        dcorner1 [1] = dcorner2[1] = 0.5F * (corner1[1] + corner3[1]);
        dcorner1 [2] = dcorner2[2] = 0.5F * (corner1[2] + corner3[2]);
        corner2 = dcorner1;
        corner4 = dcorner2;
        break;
      case Aspect_GFM_CORNER1:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color1.rgb;
        break;
      case Aspect_GFM_CORNER2:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color1.rgb;
        corner4 = myBgGradient.color2.rgb;
        break;
      case Aspect_GFM_CORNER3:
        corner1 = myBgGradient.color2.rgb;
        corner2 = myBgGradient.color1.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color2.rgb;
        break;
      case Aspect_GFM_CORNER4:
        corner1 = myBgGradient.color1.rgb;
        corner2 = myBgGradient.color2.rgb;
        corner3 = myBgGradient.color2.rgb;
        corner4 = myBgGradient.color2.rgb;
        break;
      default:
        //printf("gradient background type not right\n");
        break;
      }

      // Save GL parameters
      glDisable( GL_LIGHTING ); //push GL_ENABLE_BIT

      GLint curSM;
      glGetIntegerv( GL_SHADE_MODEL, &curSM );
      if ( curSM != GL_SMOOTH )
        glShadeModel( GL_SMOOTH ); //push GL_LIGHTING_BIT

      glBegin(GL_TRIANGLE_FAN);
      if( myBgGradient.type != Aspect_GFM_CORNER1 && myBgGradient.type != Aspect_GFM_CORNER3 )
      {
        glColor3f(corner1[0],corner1[1],corner1[2]); glVertex2f(-1.,-1.);
        glColor3f(corner2[0],corner2[1],corner2[2]); glVertex2f( 1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]); glVertex2f( 1., 1.);
        glColor3f(corner4[0],corner4[1],corner4[2]); glVertex2f(-1., 1.);
      }
      else //if ( myBgGradient.type == Aspect_GFM_CORNER1 || myBgGradient.type == Aspect_GFM_CORNER3 )
      {
        glColor3f(corner2[0],corner2[1],corner2[2]); glVertex2f( 1.,-1.);
        glColor3f(corner3[0],corner3[1],corner3[2]); glVertex2f( 1., 1.);
        glColor3f(corner4[0],corner4[1],corner4[2]); glVertex2f(-1., 1.);
        glColor3f(corner1[0],corner1[1],corner1[2]); glVertex2f(-1.,-1.);
      }
      glEnd();

      // Restore GL parameters
      if ( curSM != GL_SMOOTH )
        glShadeModel( curSM );
    }
    // drawing bg image if:
    // - it is defined and
    // - fill type is not Aspect_FM_NONE
    if ( myBgTexture.TexId != 0 && myBgTexture.Style != Aspect_FM_NONE )
    {
      GLfloat texX_range = 1.F; // texture <s> coordinate
      GLfloat texY_range = 1.F; // texture <t> coordinate

      // Set up for stretching or tiling
      GLfloat x_offset, y_offset;
      if ( myBgTexture.Style == Aspect_FM_CENTERED )
      {
        x_offset = (GLfloat)myBgTexture.Width / (GLfloat)aViewWidth;
        y_offset = (GLfloat)myBgTexture.Height / (GLfloat)aViewHeight;
      }
      else
      {
        x_offset = 1.F;
        y_offset = 1.F;
        if ( myBgTexture.Style == Aspect_FM_TILED )
        {
          texX_range = (GLfloat)aViewWidth / (GLfloat)myBgTexture.Width;
          texY_range = (GLfloat)aViewHeight / (GLfloat)myBgTexture.Height;
        }
      }

      // OCCT issue 0023000: Improve the way the gradient and textured
      // background is managed in 3d viewer (note 0020339)
      // Setting this coefficient to -1.F allows to tile textures relatively
      // to the top-left corner of the view (value 1.F corresponds to the
      // initial behaviour - tiling from the bottom-left corner)
      GLfloat aCoef = -1.F;

      glEnable( GL_TEXTURE_2D ); //push GL_ENABLE_BIT
      glBindTexture( GL_TEXTURE_2D, myBgTexture.TexId ); //push GL_TEXTURE_BIT

      glDisable( GL_BLEND ); //push GL_ENABLE_BIT

      glColor3fv (theWorkspace.BackgroundColor().rgb);
      glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //push GL_TEXTURE_BIT

      // Note that texture is mapped using GL_REPEAT wrapping mode so integer part
      // is simply ignored, and negative multiplier is here for convenience only
      // and does not result e.g. in texture mirroring
      glBegin( GL_QUADS );
      glTexCoord2f(0.F, 0.F); glVertex2f( -x_offset, -aCoef * y_offset );
      glTexCoord2f(texX_range, 0.F); glVertex2f( x_offset, -aCoef * y_offset );
      glTexCoord2f(texX_range, aCoef * texY_range); glVertex2f( x_offset, aCoef * y_offset );
      glTexCoord2f(0.F, aCoef * texY_range); glVertex2f( -x_offset, aCoef * y_offset );
      glEnd();
    }

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glPopAttrib(); //GL_ENABLE_BIT | GL_TEXTURE_BIT

    if (theWorkspace.UseZBuffer())
    {
      glEnable (GL_DEPTH_TEST);
    }
  }
#endif
}

/*----------------------------------------------------------------------*/

//call_func_redraw_all_structs_proc
void OpenGl_View::Render (const Handle(OpenGl_PrinterContext)& thePrintContext,
                          const Handle(OpenGl_Workspace) &theWorkspace,
                          const Graphic3d_CView& theCView,
                          const Aspect_CLayer2d& theCUnderLayer,
                          const Aspect_CLayer2d& theCOverLayer)
{
  // ==================================
  //      Step 1: Prepare for redraw
  // ==================================

  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

#if !defined(GL_ES_VERSION_2_0)
  // store and disable current clipping planes
  const Standard_Integer aMaxPlanes = aContext->MaxClipPlanes();
  NCollection_Array1<OPENGL_CLIP_PLANE> aOldPlanes (GL_CLIP_PLANE0, GL_CLIP_PLANE0 + aMaxPlanes - 1);
  if (aContext->core11 != NULL)
  {
    for (Standard_Integer aClipPlaneId = aOldPlanes.Lower(); aClipPlaneId <= aOldPlanes.Upper(); ++aClipPlaneId)
    {
      OPENGL_CLIP_PLANE& aPlane = aOldPlanes.ChangeValue (aClipPlaneId);
      aContext->core11->glGetClipPlane (aClipPlaneId, aPlane.Equation);
      if (aPlane.isEnabled)
      {
        aContext->core11fwd->glDisable (aClipPlaneId);
        aPlane.isEnabled = GL_TRUE;
      }
      else
      {
        aPlane.isEnabled = GL_FALSE;
      }
    }
  }
#endif

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

  // Set OCCT state uniform variables
  const Handle(OpenGl_ShaderManager) aManager   = aContext->ShaderManager();
  const Standard_Boolean             isSameView = aManager->IsSameView (this); // force camera state update when needed
  if (!aManager->IsEmpty())
  {
    if (StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index()) != myLastLightSourceState)
    {
      aManager->UpdateLightSourceStateTo (&myLights);
      myLastLightSourceState = StateInfo (myCurrLightSourceState, aManager->LightSourceState().Index());
    }

    if (myProjectionState != myCamera->ProjectionState()
    || !isSameView)
    {
      myProjectionState = myCamera->ProjectionState();
      aManager->UpdateProjectionStateTo ((const Tmatrix3*)myCamera->ProjectionMatrixF().GetData());
    }

    if (myModelViewState != myCamera->ModelViewState()
    || !isSameView)
    {
      myModelViewState = myCamera->ModelViewState();
      aManager->UpdateWorldViewStateTo ((const Tmatrix3*)myCamera->OrientationMatrixF().GetData());
    }

    if (aManager->ModelWorldState().Index() == 0)
    {
      Tmatrix3 aModelWorldState = { { 1.f, 0.f, 0.f, 0.f },
                                    { 0.f, 1.f, 0.f, 0.f },
                                    { 0.f, 0.f, 1.f, 0.f },
                                    { 0.f, 0.f, 0.f, 1.f } };

      aContext->ShaderManager()->UpdateModelWorldStateTo (&aModelWorldState);
    }
  }

  if (isProjectionMatUpdateNeeded
   || isOrientationMatUpdateNeeded)
  {
    myBVHSelector.SetViewVolume (myCamera);
  }

  // ====================================
  //      Step 2: Redraw background
  // ====================================

  // Render background
  if (theWorkspace->ToRedrawGL())
  {
    DrawBackground (*theWorkspace);
  }

#if !defined(GL_ES_VERSION_2_0)
  // Switch off lighting by default
  glDisable(GL_LIGHTING);
#endif

  // =================================
  //      Step 3: Draw underlayer
  // =================================

  RedrawLayer2d (thePrintContext, theCView, theCUnderLayer);

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
  if(anAxialScale.X() != 1.F ||
     anAxialScale.Y() != 1.F ||
     anAxialScale.Z() != 1.F)
    glEnable(GL_NORMALIZE);
  else if(glIsEnabled(GL_NORMALIZE))
    glDisable(GL_NORMALIZE);

  // Apply Fog
  if ( myFog.IsOn )
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
  else
    glDisable(GL_FOG);

  // Apply InteriorShadingMethod
  aContext->core11->glShadeModel (myShadingModel == Visual3d_TOM_FACET
                               || myShadingModel == Visual3d_TOM_NONE ? GL_FLAT : GL_SMOOTH);
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
  if (!myCamera->IsStereo() || !aContext->HasStereoBuffers())
  {
    // single-pass monographic rendering
    const OpenGl_Matrix* aProj = (const OpenGl_Matrix*) &myCamera->ProjectionMatrixF();

    const OpenGl_Matrix* aOrient = (const OpenGl_Matrix*) &myCamera->OrientationMatrixF();

    // redraw scene with normal orientation and projection
    RedrawScene (thePrintContext, theWorkspace, aProj, aOrient);
  }
  else
  {
    // two stereographic passes
    const OpenGl_Matrix* aLProj  = (const OpenGl_Matrix*) &myCamera->ProjectionStereoLeftF();
    const OpenGl_Matrix* aRProj  = (const OpenGl_Matrix*) &myCamera->ProjectionStereoRightF();
    const OpenGl_Matrix* aOrient = (const OpenGl_Matrix*) &myCamera->OrientationMatrixF();

    // safely switch to left Eye buffer
    aContext->SetDrawBufferLeft();

    // redraw left Eye
    RedrawScene (thePrintContext, theWorkspace, aLProj, aOrient);

    // reset depth buffer of first rendering pass
    if (theWorkspace->UseDepthTest())
    {
      glClear (GL_DEPTH_BUFFER_BIT);
    }
    // safely switch to right Eye buffer
    aContext->SetDrawBufferRight();

    // redraw right Eye
    RedrawScene (thePrintContext, theWorkspace, aRProj, aOrient);

    // switch back to monographic rendering
    aContext->SetDrawBufferMono();
  }

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
  if (theWorkspace->ToRedrawGL())
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

  const int aMode = 0;
  theWorkspace->DisplayCallback (theCView, (aMode | OCC_PRE_OVERLAY));

  RedrawLayer2d (thePrintContext, theCView, theCOverLayer);

  theWorkspace->DisplayCallback (theCView, aMode);

  // ===============================
  //      Step 7: Finalize
  // ===============================

#if !defined(GL_ES_VERSION_2_0)
  // restore clipping planes
  if (aContext->core11 != NULL)
  {
    for (Standard_Integer aClipPlaneId = aOldPlanes.Lower(); aClipPlaneId <= aOldPlanes.Upper(); ++aClipPlaneId)
    {
      const OPENGL_CLIP_PLANE& aPlane = aOldPlanes.ChangeValue (aClipPlaneId);
      aContext->core11->glClipPlane (aClipPlaneId, aPlane.Equation);
      if (aPlane.isEnabled)
        aContext->core11fwd->glEnable (aClipPlaneId);
      else
        aContext->core11fwd->glDisable (aClipPlaneId);
    }
  }
#endif

  // ==============================================================
  //      Step 8: Keep shader manager informed about last View
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
void OpenGl_View::InvalidateBVHData (const Standard_Integer theLayerId)
{
  myZLayers.InvalidateBVHData (theLayerId);
}

/*----------------------------------------------------------------------*/

//ExecuteViewDisplay
void OpenGl_View::RenderStructs (const Handle(OpenGl_Workspace) &AWorkspace)
{
  if ( myZLayers.NbStructures() <= 0 )
    return;

#if !defined(GL_ES_VERSION_2_0)
  glPushAttrib ( GL_DEPTH_BUFFER_BIT );
#endif

  //TsmPushAttri(); /* save previous graphics context */

  if ( (AWorkspace->NamedStatus & OPENGL_NS_2NDPASSNEED) == 0 )
  {
  #if !defined(GL_ES_VERSION_2_0)
    const int antiAliasingMode = AWorkspace->AntiAliasingMode();
  #endif

    if ( !myAntiAliasing )
    {
    #if !defined(GL_ES_VERSION_2_0)
      glDisable(GL_POINT_SMOOTH);
      glDisable(GL_LINE_SMOOTH);
      if( antiAliasingMode & 2 ) glDisable(GL_POLYGON_SMOOTH);
    #endif
      glBlendFunc (GL_ONE, GL_ZERO);
      glDisable (GL_BLEND);
    }
    else
    {
    #if !defined(GL_ES_VERSION_2_0)
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_LINE_SMOOTH);
      if( antiAliasingMode & 2 ) glEnable(GL_POLYGON_SMOOTH);
    #endif
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable (GL_BLEND);
    }
  }

  myZLayers.Render (AWorkspace);

#if !defined(GL_ES_VERSION_2_0)
  //TsmPopAttri(); /* restore previous graphics context; before update lights */
  glPopAttrib();
#endif
}

/*----------------------------------------------------------------------*/

//call_togl_redraw_layer2d
void OpenGl_View::RedrawLayer2d (const Handle(OpenGl_PrinterContext)& thePrintContext,
                                 const Graphic3d_CView&               ACView,
                                 const Aspect_CLayer2d&               ACLayer)
{
#if !defined(GL_ES_VERSION_2_0)
  if (&ACLayer == NULL
   || ACLayer.ptrLayer == NULL
   || ACLayer.ptrLayer->listIndex == 0) return;

  GLsizei dispWidth  = (GLsizei )ACLayer.viewport[0];
  GLsizei dispHeight = (GLsizei )ACLayer.viewport[1];

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix ();
  glLoadIdentity ();

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();

  if (!ACLayer.sizeDependent)
    glViewport (0, 0, dispWidth, dispHeight);

  float left = ACLayer.ortho[0];
  float right = ACLayer.ortho[1];
  float bottom = ACLayer.ortho[2];
  float top = ACLayer.ortho[3];

  int attach = ACLayer.attach;

  float ratio;
  if (!ACLayer.sizeDependent)
    ratio = (float) dispWidth/dispHeight;
  else
    ratio = ACView.DefWindow.dx/ACView.DefWindow.dy;

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
    thePrintContext->LoadProjTransformation();

    // printing operation also assumes other viewport dimension
    // to comply with transformation matrix or graphics scaling
    // factors for tiling for layer redraw
    GLsizei anViewportX = 0;
    GLsizei anViewportY = 0;
    thePrintContext->GetLayerViewport (anViewportX, anViewportY);
    if (anViewportX != 0 && anViewportY != 0)
      glViewport (0, 0, anViewportX, anViewportY);
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

  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix ();

  if (!ACLayer.sizeDependent)
    glViewport (0, 0, (GLsizei) ACView.DefWindow.dx, (GLsizei) ACView.DefWindow.dy);

  glFlush ();
#endif
}

/*----------------------------------------------------------------------*/

void OpenGl_View::RedrawTrihedron (const Handle(OpenGl_Workspace) &theWorkspace)
{
  // display global trihedron
  if (myTrihedron != NULL)
  {
    myTrihedron->Render (theWorkspace);
  }
  if (myGraduatedTrihedron != NULL)
  {
    myGraduatedTrihedron->Render (theWorkspace);
  }
}

/*----------------------------------------------------------------------*/

//call_togl_create_bg_texture
void OpenGl_View::CreateBackgroundTexture (const Standard_CString  theFilePath,
                                           const Aspect_FillMethod theFillStyle)
{
  if (myBgTexture.TexId != 0)
  {
    // delete existing texture
    glDeleteTextures (1, (GLuint* )&(myBgTexture.TexId));
    myBgTexture.TexId = 0;
  }

  // load image from file
  Image_AlienPixMap anImageLoaded;
  if (!anImageLoaded.Load (theFilePath))
  {
    return;
  }

  Image_PixMap anImage;
  if (anImageLoaded.RowExtraBytes() == 0 &&
      (anImageLoaded.Format() == Image_PixMap::ImgRGB
    || anImageLoaded.Format() == Image_PixMap::ImgRGB32
    || anImageLoaded.Format() == Image_PixMap::ImgRGBA))
  {
    anImage.InitWrapper (anImageLoaded.Format(), anImageLoaded.ChangeData(),
                         anImageLoaded.SizeX(), anImageLoaded.SizeY(), anImageLoaded.SizeRowBytes());
  }
  else
  {
    // convert image to RGB format
    if (!anImage.InitTrash (Image_PixMap::ImgRGB, anImageLoaded.SizeX(), anImageLoaded.SizeY()))
    {
      return;
    }

    anImage.SetTopDown (false);
    Quantity_Color aSrcColor;
    for (Standard_Size aRow = 0; aRow < anImage.SizeY(); ++aRow)
    {
      for (Standard_Size aCol = 0; aCol < anImage.SizeX(); ++aCol)
      {
        aSrcColor = anImageLoaded.PixelColor ((Standard_Integer )aCol, (Standard_Integer )aRow);
        Image_ColorRGB& aColor = anImage.ChangeValue<Image_ColorRGB> (aRow, aCol);
        aColor.r() = Standard_Byte(255.0 * aSrcColor.Red());
        aColor.g() = Standard_Byte(255.0 * aSrcColor.Green());
        aColor.b() = Standard_Byte(255.0 * aSrcColor.Blue());
      }
    }
    anImageLoaded.Clear();
  }

  // create MipMapped texture
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  GLuint aTextureId = 0;
  glGenTextures (1, &aTextureId);
  glBindTexture (GL_TEXTURE_2D, aTextureId);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

  const GLenum aDataFormat = (anImage.Format() == Image_PixMap::ImgRGB) ? GL_RGB : GL_RGBA;

#if !defined(GL_ES_VERSION_2_0)
  gluBuild2DMipmaps (GL_TEXTURE_2D, 3/*4*/,
                     GLint(anImage.SizeX()), GLint(anImage.SizeY()),
                     aDataFormat, GL_UNSIGNED_BYTE, anImage.Data());
#endif

  myBgTexture.TexId  = aTextureId;
  myBgTexture.Width  = (Standard_Integer )anImage.SizeX();
  myBgTexture.Height = (Standard_Integer )anImage.SizeY();
  myBgTexture.Style  = theFillStyle;
}

/*----------------------------------------------------------------------*/

//call_togl_set_bg_texture_style
void OpenGl_View::SetBackgroundTextureStyle (const Aspect_FillMethod AFillStyle)
{
  myBgTexture.Style = AFillStyle;
}

/*----------------------------------------------------------------------*/

//call_togl_gradient_background
void OpenGl_View::SetBackgroundGradient (const Quantity_Color& AColor1,
                                        const Quantity_Color& AColor2,
                                        const Aspect_GradientFillMethod AType)
{
  Standard_Real R,G,B;
  AColor1.Values( R, G, B, Quantity_TOC_RGB );
  myBgGradient.color1.rgb[0] = ( Tfloat )R;
  myBgGradient.color1.rgb[1] = ( Tfloat )G;
  myBgGradient.color1.rgb[2] = ( Tfloat )B;
  myBgGradient.color1.rgb[3] = 0.F;

  AColor2.Values( R, G, B, Quantity_TOC_RGB );
  myBgGradient.color2.rgb[0] = ( Tfloat )R;
  myBgGradient.color2.rgb[1] = ( Tfloat )G;
  myBgGradient.color2.rgb[2] = ( Tfloat )B;
  myBgGradient.color2.rgb[3] = 0.F;

  myBgGradient.type = AType;
}

/*----------------------------------------------------------------------*/

//call_togl_set_gradient_type
void OpenGl_View::SetBackgroundGradientType (const Aspect_GradientFillMethod AType)
{
  myBgGradient.type = AType;
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================

void OpenGl_View::AddZLayer (const Standard_Integer theLayerId)
{
  myZLayers.AddLayer (theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================

void OpenGl_View::RemoveZLayer (const Standard_Integer theLayerId)
{
  myZLayers.RemoveLayer (theLayerId);
}

//=======================================================================
//function : DisplayStructure
//purpose  :
//=======================================================================

void OpenGl_View::DisplayStructure (const OpenGl_Structure *theStructure,
                                    const Standard_Integer  thePriority)
{
  Standard_Integer aZLayer = theStructure->GetZLayer ();
  myZLayers.AddStructure (theStructure, aZLayer, thePriority);
}

//=======================================================================
//function : DisplayImmediateStructure
//purpose  :
//=======================================================================

void OpenGl_View::DisplayImmediateStructure (const OpenGl_Structure* theStructure)
{
  for (OpenGl_SequenceOfStructure::Iterator anIter (myImmediateList);
       anIter.More(); anIter.Next())
  {
    if (anIter.Value() == theStructure)
    {
      return;
    }
  }

  myImmediateList.Append (theStructure);
}

//=======================================================================
//function : EraseStructure
//purpose  :
//=======================================================================

void OpenGl_View::EraseStructure (const OpenGl_Structure *theStructure)
{
  Standard_Integer aZLayer = theStructure->GetZLayer ();
  myZLayers.RemoveStructure (theStructure, aZLayer);
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

void OpenGl_View::ChangeZLayer (const OpenGl_Structure *theStructure,
                                const Standard_Integer  theNewLayerId)
{
  Standard_Integer anOldLayer = theStructure->GetZLayer ();
  myZLayers.ChangeLayer (theStructure, anOldLayer, theNewLayerId);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void OpenGl_View::SetZLayerSettings (const Standard_Integer theLayerId,
                                     const Graphic3d_ZLayerSettings theSettings)
{
  myZLayers.Layer (theLayerId).SetLayerSettings (theSettings);
}

//=======================================================================
//function : ChangePriority
//purpose  :
//=======================================================================
void OpenGl_View::ChangePriority (const OpenGl_Structure *theStructure,
                                  const Standard_Integer theNewPriority)
{
  Standard_Integer aLayerId = theStructure->GetZLayer();
  myZLayers.ChangePriority (theStructure, aLayerId, theNewPriority);
}

//=======================================================================
//function : RedrawScene
//purpose  :
//=======================================================================

void OpenGl_View::RedrawScene (const Handle(OpenGl_PrinterContext)& thePrintContext,
                               const Handle(OpenGl_Workspace)& theWorkspace,
                               const OpenGl_Matrix* theProjection,
                               const OpenGl_Matrix* theOrientation)
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

#if !defined(GL_ES_VERSION_2_0)
  // Setup view projection
  glMatrixMode (GL_PROJECTION);

#ifdef _WIN32
  // add printing scale/tiling transformation
  if (!thePrintContext.IsNull())
  {
    thePrintContext->LoadProjTransformation();
  }
  else
#endif
    glLoadIdentity();

  glMultMatrixf ((const GLfloat*)theProjection);

  if (!thePrintContext.IsNull())
  {
    // update shader uniform projection matrix with new data
    Tmatrix3 aResultProjection;
    glGetFloatv (GL_PROJECTION_MATRIX, *aResultProjection);
    aContext->ShaderManager()->UpdateProjectionStateTo (&aResultProjection);

    // force shader uniform restore on next frame
    myProjectionState = 0;
  }
#endif

  // Setup view orientation
  theWorkspace->SetViewMatrix (theOrientation);

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
      bind_light (aLightIt.Value(), aLightGlId, anAmbientColor);
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

  // Added PCT for handling of textures
  switch (mySurfaceDetail)
  {
    case Visual3d_TOD_NONE:
      theWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;
      theWorkspace->DisableTexture();
      // Render the view
      RenderStructs (theWorkspace);
      break;

    case Visual3d_TOD_ENVIRONMENT:
      theWorkspace->NamedStatus |= OPENGL_NS_FORBIDSETTEX;
      theWorkspace->EnableTexture (myTextureEnv);
      // Render the view
      RenderStructs (theWorkspace);
      theWorkspace->DisableTexture();
      break;

    case Visual3d_TOD_ALL:
      // First pass
      theWorkspace->NamedStatus &= ~OPENGL_NS_FORBIDSETTEX;
      // Render the view
      RenderStructs (theWorkspace);
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
        RenderStructs (theWorkspace);
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
}
