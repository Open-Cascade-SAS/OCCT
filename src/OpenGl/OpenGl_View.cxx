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

#include <NCollection_Mat4.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_transform_persistence.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Utils.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Mat4d.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_View,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_View,MMgt_TShared)

/*----------------------------------------------------------------------*/

static const OPENGL_BG_TEXTURE myDefaultBgTexture = { 0, 0, 0, Aspect_FM_CENTERED };
static const OPENGL_BG_GRADIENT myDefaultBgGradient = { {{ 0.F, 0.F, 0.F, 1.F }}, {{ 0.F, 0.F, 0.F, 1.F }}, Aspect_GFM_NONE };
static const Tmatrix3 myDefaultMatrix = { { 1.F, 0.F, 0.F, 0.F }, { 0.F, 1.F, 0.F, 0.F }, { 0.F, 0.F, 1.F, 0.F }, { 0.F, 0.F, 0.F, 1.F } };
static const OPENGL_ZCLIP myDefaultZClip = { { Standard_True, 0.F }, { Standard_True, 1.F } };

static const OPENGL_FOG myDefaultFog = { Standard_False, 0.F, 1.F, { { 0.F, 0.F, 0.F, 1.F } } };
static const TEL_TRANSFORM_PERSISTENCE myDefaultTransPers = { 0, 0.F, 0.F, 0.F };
static const GLdouble THE_IDENTITY_MATRIX[4][4] =
{
  {1.0, 0.0, 0.0, 0.0},
  {0.0, 1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0, 0.0},
  {0.0, 0.0, 0.0, 1.0}
};

/*----------------------------------------------------------------------*/

OpenGl_View::OpenGl_View (const CALL_DEF_VIEWCONTEXT &AContext,
                          OpenGl_StateCounter*       theCounter)
: mySurfaceDetail(Visual3d_TOD_NONE),
  myBackfacing(0),
  myBgTexture(myDefaultBgTexture),
  myBgGradient(myDefaultBgGradient),
  //shield_indicator = TOn,
  //shield_colour = { { 0.F, 0.F, 0.F, 1.F } },
  //border_indicator = TOff,
  //border_colour = { { 0.F, 0.F, 0.F, 1.F } },
  //active_status = TOn,
  myZClip(myDefaultZClip),
  myCamera(AContext.Camera),
  myFog(myDefaultFog),
  myTrihedron(NULL),
  myGraduatedTrihedron(NULL),
  myVisualization(AContext.Visualization),
  myShadingModel ((Visual3d_TypeOfModel )AContext.Model),
  myAntiAliasing(Standard_False),
  myTransPers(&myDefaultTransPers),
  myIsTransPers(Standard_False),
  myProjectionState (0),
  myModelViewState (0),
  myStateCounter (theCounter),
  myLastLightSourceState (0, 0)
{
  myCurrLightSourceState = myStateCounter->Increment();
  myModificationState = 1; // initial state
}

/*----------------------------------------------------------------------*/

OpenGl_View::~OpenGl_View ()
{
  ReleaseGlResources (NULL); // ensure ReleaseGlResources() was called within valid context
  OpenGl_Element::Destroy (NULL, myTrihedron);
  OpenGl_Element::Destroy (NULL, myGraduatedTrihedron);
}

void OpenGl_View::ReleaseGlResources (const Handle(OpenGl_Context)& theCtx)
{
  if (myTrihedron != NULL)
  {
    myTrihedron->Release (theCtx.operator->());
  }
  if (myGraduatedTrihedron != NULL)
  {
    myGraduatedTrihedron->Release (theCtx.operator->());
  }

  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }
  if (myBgTexture.TexId != 0)
  {
    glDeleteTextures (1, (GLuint*)&(myBgTexture.TexId));
    myBgTexture.TexId = 0;
  }
}

void OpenGl_View::SetTextureEnv (const Handle(OpenGl_Context)&       theCtx,
                                 const Handle(Graphic3d_TextureEnv)& theTexture)
{
  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }

  if (theTexture.IsNull())
  {
    return;
  }

  myTextureEnv = new OpenGl_Texture (theTexture->GetParams());
  Handle(Image_PixMap) anImage = theTexture->GetImage();
  if (!anImage.IsNull())
    myTextureEnv->Init (theCtx, *anImage.operator->(), theTexture->Type());

  myModificationState++;
}

void OpenGl_View::SetSurfaceDetail (const Visual3d_TypeOfSurfaceDetail theMode)
{
  mySurfaceDetail = theMode;

  myModificationState++;
}

// =======================================================================
// function : SetBackfacing
// purpose  :
// =======================================================================
void OpenGl_View::SetBackfacing (const Standard_Integer theMode)
{
  myBackfacing = theMode;
}

// =======================================================================
// function : SetLights
// purpose  :
// =======================================================================
void OpenGl_View::SetLights (const CALL_DEF_VIEWCONTEXT& theViewCtx)
{
  myLights.Clear();
  for (Standard_Integer aLightIt = 0; aLightIt < theViewCtx.NbActiveLight; ++aLightIt)
  {
    myLights.Append (theViewCtx.ActiveLight[aLightIt]);
  }
  myCurrLightSourceState = myStateCounter->Increment();
}

/*----------------------------------------------------------------------*/

//call_togl_setvisualisation
void OpenGl_View::SetVisualisation (const CALL_DEF_VIEWCONTEXT &AContext)
{
  myVisualization = AContext.Visualization;
  myShadingModel  = (Visual3d_TypeOfModel )AContext.Model;
}

/*----------------------------------------------------------------------*/

//call_togl_cliplimit
void OpenGl_View::SetClipLimit (const Graphic3d_CView& theCView)
{
  myZClip.Back.Limit = theCView.Context.ZClipBackPlane;
  myZClip.Front.Limit = theCView.Context.ZClipFrontPlane;

  myZClip.Back.IsOn  = (theCView.Context.BackZClipping  != 0);
  myZClip.Front.IsOn = (theCView.Context.FrontZClipping != 0);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::SetFog (const Graphic3d_CView& theCView,
                          const Standard_Boolean theFlag)
{
  if (!theFlag)
  {
    myFog.IsOn = Standard_False;
  }
  else
  {
    myFog.IsOn = Standard_True;

    myFog.Front = theCView.Context.DepthFrontPlane;
    myFog.Back = theCView.Context.DepthBackPlane;

    myFog.Color.rgb[0] = theCView.DefWindow.Background.r;
    myFog.Color.rgb[1] = theCView.DefWindow.Background.g;
    myFog.Color.rgb[2] = theCView.DefWindow.Background.b;
    myFog.Color.rgb[3] = 1.0f;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronDisplay (const Handle(OpenGl_Context)&       theCtx,
                                   const Aspect_TypeOfTriedronPosition thePosition,
                                   const Quantity_NameOfColor          theColor,
                                   const Standard_Real                 theScale,
                                   const Standard_Boolean              theAsWireframe)
{
  OpenGl_Element::Destroy (theCtx.operator->(), myTrihedron);
  myTrihedron = new OpenGl_Trihedron (thePosition, theColor, theScale, theAsWireframe);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronErase (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Element::Destroy (theCtx.operator->(), myTrihedron);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronDisplay (const Handle(OpenGl_Context)&        theCtx,
                                             const Graphic3d_CGraduatedTrihedron& theData)
{
  OpenGl_Element::Destroy (theCtx.operator->(), myGraduatedTrihedron);
  myGraduatedTrihedron = new OpenGl_GraduatedTrihedron (theData);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronErase (const Handle(OpenGl_Context)& theCtx)
{
  OpenGl_Element::Destroy (theCtx.operator->(), myGraduatedTrihedron);
}

/*----------------------------------------------------------------------*/

//transform_persistence_end
void OpenGl_View::EndTransformPersistence(const Handle(OpenGl_Context)& theCtx)
{
  if (myIsTransPers)
  {
    theCtx->WorldViewState.Pop();
    theCtx->ProjectionState.Pop();

    theCtx->ApplyProjectionMatrix();

    myIsTransPers = Standard_False;
  }
}

/*----------------------------------------------------------------------*/

//transform_persistence_begin
const TEL_TRANSFORM_PERSISTENCE* OpenGl_View::BeginTransformPersistence (const Handle(OpenGl_Context)& theCtx,
                                                                         const TEL_TRANSFORM_PERSISTENCE* theTransPers)
{
  const TEL_TRANSFORM_PERSISTENCE* aTransPersPrev = myTransPers;
  myTransPers = theTransPers;
  if (theTransPers->mode == 0)
  {
    EndTransformPersistence (theCtx);
    return aTransPersPrev;
  }

  GLint aViewport[4];
  OpenGl_Mat4d aModelMatrix, aProjMatrix;
  theCtx->core11fwd->glGetIntegerv (GL_VIEWPORT, aViewport);
  aModelMatrix.Convert (theCtx->ModelWorldState.Current() * theCtx->WorldViewState.Current());
  aProjMatrix .Convert (theCtx->ProjectionState.Current());

  const GLdouble aViewportW = (GLdouble )aViewport[2];
  const GLdouble aViewportH = (GLdouble )aViewport[3];
  if (myIsTransPers)
  {
    // pop matrix stack - it will be overridden later
    theCtx->WorldViewState.Pop();
    theCtx->ProjectionState.Pop();
  }
  else
  {
    myIsTransPers = Standard_True;
  }

  // push matrices into stack and reset them
  theCtx->WorldViewState.Push();
  theCtx->ProjectionState.Push();

  // get the window's (fixed) coordinates for theTransPers->point before matrixes modifications
  GLdouble aWinX = 0.0, aWinY = 0.0, aWinZ = 0.0;
  if ((theTransPers->mode & TPF_PAN) != TPF_PAN)
  {
    OpenGl_Utils::Project<Standard_Real> (theTransPers->pointX,
                                          theTransPers->pointY,
                                          theTransPers->pointZ,
                                          aModelMatrix,
                                          aProjMatrix,
                                          aViewport,
                                          aWinZ,
                                          aWinY,
                                          aWinZ);
  }

  // prevent zooming
  if ((theTransPers->mode & TPF_ZOOM)
   || (theTransPers->mode == TPF_TRIEDRON))
  {
    // compute fixed-zoom multiplier
    // actually function works ugly with TelPerspective!
    const GLdouble aDet2 = 0.002 / (aViewportW > aViewportH ? aProjMatrix.GetValue (1, 1) : aProjMatrix.GetValue (0, 0));
    aProjMatrix.ChangeValue (0, 0) *= aDet2;
    aProjMatrix.ChangeValue (1, 1) *= aDet2;
    aProjMatrix.ChangeValue (2, 2) *= aDet2;
  }

  // prevent translation - annulate translate matrix
  if ((theTransPers->mode & TPF_PAN)
   || (theTransPers->mode == TPF_TRIEDRON))
  {
    aModelMatrix.SetValue (0, 3, 0.0);
    aModelMatrix.SetValue (1, 3, 0.0);
    aModelMatrix.SetValue (2, 3, 0.0);
    aProjMatrix .SetValue (0, 3, 0.0);
    aProjMatrix .SetValue (1, 3, 0.0);
    aProjMatrix .SetValue (2, 3, 0.0);
  }

  // prevent scaling-on-axis
  if (theTransPers->mode & TPF_ZOOM)
  {
    const gp_Pnt anAxialScale = myCamera->AxialScale();
    const double aScaleX = anAxialScale.X();
    const double aScaleY = anAxialScale.Y();
    const double aScaleZ = anAxialScale.Z();
    for (int i = 0; i < 3; ++i)
    {
      aModelMatrix.ChangeValue (0, i) /= aScaleX;
      aModelMatrix.ChangeValue (1, i) /= aScaleY;
      aModelMatrix.ChangeValue (2, i) /= aScaleZ;
    }
  }

  // prevent rotating - annulate rotate matrix
  if (theTransPers->mode & TPF_ROTATE)
  {
    aModelMatrix.SetValue (0, 0, 1.0);
    aModelMatrix.SetValue (1, 1, 1.0);
    aModelMatrix.SetValue (2, 2, 1.0);

    aModelMatrix.SetValue (1, 0, 0.0);
    aModelMatrix.SetValue (2, 0, 0.0);
    aModelMatrix.SetValue (0, 1, 0.0);
    aModelMatrix.SetValue (2, 1, 0.0);
    aModelMatrix.SetValue (0, 2, 0.0);
    aModelMatrix.SetValue (1, 2, 0.0);
  }

  // load computed matrices
  theCtx->ModelWorldState.SetIdentity();
  theCtx->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  theCtx->ProjectionState.SetCurrent<Standard_Real> (aProjMatrix);

  if (theTransPers->mode == TPF_TRIEDRON)
  {
    // move to the window corner
    if (theTransPers->pointX != 0.0
     && theTransPers->pointY != 0.0)
    {
      GLdouble aW1, aH1, aW2, aH2, aDummy;

      OpenGl_Mat4d anIdentity;

      OpenGl_Utils::UnProject<Standard_Real> (0.5 * aViewportW,
                                              0.5 * aViewportH,
                                              0.0,
                                              anIdentity,
                                              aProjMatrix,
                                              aViewport,
                                              aW1,
                                              aH1,
                                              aDummy);

      OpenGl_Utils::UnProject<Standard_Real> (-0.5 * aViewportW,
                                              -0.5 * aViewportH,
                                              0.0,
                                              anIdentity,
                                              aProjMatrix,
                                              aViewport,
                                              aW2,
                                              aH2,
                                              aDummy);

      GLdouble aMoveX = 0.5 * (aW1 - aW2 - theTransPers->pointZ);
      GLdouble aMoveY = 0.5 * (aH1 - aH2 - theTransPers->pointZ);
      aMoveX = (theTransPers->pointX > 0.0) ? aMoveX : -aMoveX;
      aMoveY = (theTransPers->pointY > 0.0) ? aMoveY : -aMoveY;

      OpenGl_Utils::Translate<Standard_Real> (aProjMatrix, aMoveX, aMoveY, 0.0);
      theCtx->ProjectionState.SetCurrent<Standard_Real> (aProjMatrix);
    }
  }
  else if ((theTransPers->mode & TPF_PAN) != TPF_PAN)
  {
    // move to thePoint using saved win-coordinates ('marker-behaviour')
    GLdouble aMoveX, aMoveY, aMoveZ;

    OpenGl_Utils::UnProject<Standard_Real> (aWinX,
                                            aWinY,
                                            aWinZ,
                                            aModelMatrix,
                                            aProjMatrix,
                                            aViewport,
                                            aMoveX,
                                            aMoveY,
                                            aMoveZ);

    OpenGl_Utils::Translate<Standard_Real> (aModelMatrix, aMoveX, aMoveY, aMoveZ);
    theCtx->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  }

  theCtx->ApplyProjectionMatrix();
  return aTransPersPrev;
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GetMatrices (OpenGl_Mat4& theOrientation,
                               OpenGl_Mat4& theViewMapping) const
{
  theViewMapping = myCamera->ProjectionMatrixF();
  theOrientation = myCamera->OrientationMatrixF();
}
/*----------------------------------------------------------------------*/
