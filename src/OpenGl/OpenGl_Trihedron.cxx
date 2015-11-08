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

#include <OpenGl_Trihedron.hxx>

#include <OpenGl_GlCore11.hxx>

#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>
#include <Precision.hxx>

namespace
{
  static const OpenGl_TextParam THE_LABEL_PARAMS =
  {
    16, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM
  };
}

// =======================================================================
// function : resetTransformations
// purpose  :
// =======================================================================
void OpenGl_Trihedron::resetTransformations (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  const OpenGl_View*            aView    = theWorkspace->View();
  GLdouble anU = 1.0;
  GLdouble aV = 1.0;
  if (aView->Height() < aView->Width())
  {
    aV = aView->Width() / aView->Height();
  }
  else
  {
    anU = aView->Height() / aView->Width();
  }

  // Reading the transformation matrices and projection of sight
  // to cancel translations (last columns of matrices).
  OpenGl_Mat4d aModelMatrix;
  OpenGl_Mat4d aProjMatrix;
  aModelMatrix.Convert (aContext->WorldViewState.Current());

  // Cancel the translation that can be assigned to the view
  aModelMatrix.ChangeValue (0, 3) = 0.0;
  aModelMatrix.ChangeValue (1, 3) = 0.0;
  aModelMatrix.ChangeValue (2, 3) = 0.0;

  aProjMatrix.ChangeValue (0, 0) = 2.0 / anU;
  aProjMatrix.ChangeValue (1, 0) = 0.0;
  aProjMatrix.ChangeValue (2, 0) = 0.0;
  aProjMatrix.ChangeValue (3, 0) = 0.0;

  aProjMatrix.ChangeValue (0, 1) = 0.0;
  aProjMatrix.ChangeValue (1, 1) = 2.0 / aV;
  aProjMatrix.ChangeValue (2, 1) = 0.0;
  aProjMatrix.ChangeValue (3, 1) = 0.0;

  aProjMatrix.ChangeValue (0, 2) = 0.0;
  aProjMatrix.ChangeValue (1, 2) = 0.0;
  aProjMatrix.ChangeValue (2, 2) = -2.0 * 0.01;
  aProjMatrix.ChangeValue (3, 2) = 0.0;

  aProjMatrix.ChangeValue (0, 3) = 0.0;
  aProjMatrix.ChangeValue (1, 3) = 0.0;
  aProjMatrix.ChangeValue (2, 3) = 0.0;
  aProjMatrix.ChangeValue (3, 3) = 1.0;

  // Define trihedron position in the view
  switch (myPos)
  {
    case Aspect_TOTP_LEFT_LOWER:
    {
      Graphic3d_TransformUtils::Translate (aProjMatrix,
        -0.5 * anU + myScale, -0.5 * aV + myScale, 0.0);
      break;
    }
    case Aspect_TOTP_LEFT_UPPER:
    {
      Graphic3d_TransformUtils::Translate (aProjMatrix,
        -0.5 * anU + myScale, 0.5 * aV - myScale - myScale / 3.0, 0.0);
      break;
    }
    case Aspect_TOTP_RIGHT_LOWER:
    {
      Graphic3d_TransformUtils::Translate (aProjMatrix,
        0.5 * anU - myScale - myScale / 3.0, -0.5 * aV + myScale, 0.0);
      break;
    }
    case Aspect_TOTP_RIGHT_UPPER:
    {
      Graphic3d_TransformUtils::Translate (aProjMatrix,
        0.5 * anU - myScale - myScale / 3.0, 0.5 * aV - myScale - myScale / 3.0, 0.0);
      break;
    }
    //case Aspect_TOTP_CENTER:
    default:
      break;
  }

  aContext->ProjectionState.SetCurrent<Standard_Real> (aProjMatrix);
  aContext->ApplyProjectionMatrix();

  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  aContext->ApplyWorldViewMatrix();
}

// =======================================================================
// function : redraw
// purpose  :
// =======================================================================
void OpenGl_Trihedron::redraw (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  aContext->WorldViewState.Push();
  aContext->ProjectionState.Push();

  resetTransformations (theWorkspace);

  // Set trihedron size parameters
  GLdouble aScale = myScale;
  aScale *= myRatio;
  const Standard_Real aLineRatio = 0.75;
  const GLdouble aLineLength = aScale * aLineRatio;
  const GLdouble aConeDiametr     = aScale * myDiameter;
  const GLdouble aConeLength      = aScale * (1.0 - aLineRatio);
  const GLdouble aRayon = aScale / 30.0;

  // Create primitive line here for changing length
  if (!myLine.IsInitialized())
  {
    Handle(Graphic3d_ArrayOfSegments) aGraphicArray = new Graphic3d_ArrayOfSegments (2);
    aGraphicArray->AddVertex (0.0, 0.0, 0.0);
    aGraphicArray->AddVertex (0.0, 0.0, aLineLength);
    myLine.InitBuffers (aContext, Graphic3d_TOPA_SEGMENTS, aGraphicArray->Indices(),
                        aGraphicArray->Attributes(),
                        aGraphicArray->Bounds());
  }

  if (!myCircle.IsInitialized())
  {
    const Standard_Integer THE_CIRCLE_SERMENTS_NB = 24;
    Handle(Graphic3d_ArrayOfPolylines) anCircleArray = new Graphic3d_ArrayOfPolylines (THE_CIRCLE_SERMENTS_NB + 2);

    const Standard_Real THE_CIRCLE_SEGMENT_ANGLE = 2.0 * M_PI / THE_CIRCLE_SERMENTS_NB;
    for (Standard_Integer anIt = THE_CIRCLE_SERMENTS_NB; anIt >= 0; --anIt)
    {
      anCircleArray->AddVertex (aRayon * sin (anIt * THE_CIRCLE_SEGMENT_ANGLE),
                                aRayon * cos (anIt * THE_CIRCLE_SEGMENT_ANGLE), 0.0);
    }
    anCircleArray->AddVertex (aRayon * sin (THE_CIRCLE_SERMENTS_NB * THE_CIRCLE_SEGMENT_ANGLE),
                              aRayon * cos (THE_CIRCLE_SERMENTS_NB * THE_CIRCLE_SEGMENT_ANGLE), 0.0);

    myCircle.InitBuffers (aContext, Graphic3d_TOPA_POLYLINES, anCircleArray->Indices(),
                          anCircleArray->Attributes(), anCircleArray->Bounds());
  }

  if (!myDisk.IsInitialized())
  {
    myDisk.Init (0.0, static_cast<GLfloat> (aConeDiametr), myNbFacettes, 1);
  }

  if (!myCone.IsInitialized())
  {
    myCone.Init (static_cast<GLfloat> (aConeDiametr), 0.0f, static_cast<GLfloat> (aConeLength), myNbFacettes, 1);
  }

  OpenGl_AspectFace anAspectX;
  OpenGl_AspectFace anAspectY;
  OpenGl_AspectFace anAspectZ;
  OpenGl_AspectLine anAspectLine;
  memcpy (anAspectX.ChangeIntFront().matcol.rgb, myXColor.GetData(), sizeof (TEL_COLOUR));
  memcpy (anAspectY.ChangeIntFront().matcol.rgb, myYColor.GetData(), sizeof (TEL_COLOUR));
  memcpy (anAspectZ.ChangeIntFront().matcol.rgb, myZColor.GetData(), sizeof (TEL_COLOUR));
  OpenGl_Mat4d aModelMatrix;
  aModelMatrix.Convert (aContext->WorldViewState.Current());
  OpenGl_Mat4d aModelViewX (aModelMatrix);
  OpenGl_Mat4d aModelViewY (aModelMatrix);
  OpenGl_Mat4d aModelViewZ (aModelMatrix);

  // Set line aspect
  const OpenGl_AspectLine* aCurrentAspectLine = theWorkspace->AspectLine (Standard_True);
  CALL_DEF_CONTEXTLINE aLineAspect = {1, 1, { 1.F, 1.F, 1.F },  aCurrentAspectLine->Type(), aCurrentAspectLine->Width(), 0};
  aLineAspect.Color.r = myZColor.r();
  aLineAspect.Color.g = myZColor.g();
  aLineAspect.Color.b = myZColor.b();
  anAspectLine.SetAspect (aLineAspect);

  // Disable depth test and face culling
  GLboolean wasDepthMaskEnabled = GL_FALSE;
  GLint aDepthFuncBack = 0, aCullFaceModeBack = GL_BACK;
  const GLboolean wasDepthEnabled    = aContext->core11fwd->glIsEnabled (GL_DEPTH_TEST);
  const GLboolean wasCullFaceEnabled = aContext->core11fwd->glIsEnabled (GL_CULL_FACE);
  aContext->core11fwd->glGetIntegerv (GL_DEPTH_FUNC,      &aDepthFuncBack);
  aContext->core11fwd->glGetIntegerv (GL_CULL_FACE_MODE,  &aCullFaceModeBack);
  aContext->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &wasDepthMaskEnabled);
  if (!wasDepthEnabled)
  {
    aContext->core11fwd->glEnable (GL_DEPTH_TEST);
    aContext->core11fwd->glClear (GL_DEPTH_BUFFER_BIT);
  }
  if (!wasDepthMaskEnabled)
  {
    aContext->core11fwd->glDepthMask (GL_TRUE);
  }
  aContext->core11fwd->glCullFace (GL_BACK);
  if (!wasCullFaceEnabled)
  {
    aContext->core11fwd->glEnable (GL_CULL_FACE);
  }

  // Origin
  myCircle.Render (theWorkspace);

  // Z axis
  const OpenGl_AspectFace* anOldAspectFace = theWorkspace->SetAspectFace(&anAspectZ);
  theWorkspace->SetAspectLine (&anAspectLine);
  myLine.Render (theWorkspace);
  Graphic3d_TransformUtils::Translate (aModelViewZ, 0.0, 0.0, aLineLength);
  aContext->WorldViewState.SetCurrent<Standard_Real>(aModelViewZ);
  aContext->ApplyWorldViewMatrix();
  myDisk.Render (theWorkspace);
  myCone.Render (theWorkspace);

  // X axis
  theWorkspace->SetAspectFace (&anAspectX);
  Graphic3d_TransformUtils::Rotate (aModelViewX, 90.0, 0.0, aScale, 0.0);
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewX);
  aContext->ApplyWorldViewMatrix();

  aLineAspect.Color.r = myXColor.r();
  aLineAspect.Color.g = myXColor.g();
  aLineAspect.Color.b = myXColor.b();
  anAspectLine.SetAspect (aLineAspect);
  theWorkspace->SetAspectLine (&anAspectLine);
  myLine.Render (theWorkspace);
  Graphic3d_TransformUtils::Translate (aModelViewX, 0.0, 0.0, aLineLength);
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewX);
  aContext->ApplyWorldViewMatrix();
  myDisk.Render (theWorkspace);
  myCone.Render (theWorkspace);

  // Y axis
  theWorkspace->SetAspectFace (&anAspectY);
  Graphic3d_TransformUtils::Rotate (aModelViewY, -90.0, aScale, 0.0, 0.0);
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewY);
  aContext->ApplyWorldViewMatrix();

  aLineAspect.Color.r = myYColor.r();
  aLineAspect.Color.g = myYColor.g();
  aLineAspect.Color.b = myYColor.b();
  anAspectLine.SetAspect (aLineAspect);
  theWorkspace->SetAspectLine (&anAspectLine);
  myLine.Render (theWorkspace);
  Graphic3d_TransformUtils::Translate (aModelViewY, 0.0, 0.0, aLineLength);
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewY);
  aContext->ApplyWorldViewMatrix();
  myDisk.Render (theWorkspace);
  myCone.Render (theWorkspace);

  // Restore aspects
  theWorkspace->SetAspectFace (anOldAspectFace);

  if (!wasDepthEnabled)
  {
    aContext->core11fwd->glDisable (GL_DEPTH_TEST);
  }
  if (!wasDepthMaskEnabled)
  {
    aContext->core11fwd->glDepthMask (GL_FALSE);
  }
  if (!wasCullFaceEnabled)
  {
    aContext->core11fwd->glDisable (GL_CULL_FACE);
  }
  aContext->core11fwd->glCullFace (aCullFaceModeBack);

  // Always write the text
  aContext->core11fwd->glDepthFunc (GL_ALWAYS);

  // Render labels
  myLabelX.SetPosition (OpenGl_Vec3 (float (aScale + 2.0 * aRayon), 0.0f, float (-aRayon)));
  myLabelY.SetPosition (OpenGl_Vec3 (float (aRayon), float (aScale + 3.0 * aRayon), float (2.0 * aRayon)));
  myLabelZ.SetPosition (OpenGl_Vec3 (float (-2.0 * aRayon), float (0.5 * aRayon), float (aScale + 3.0 * aRayon)));
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  aContext->ApplyWorldViewMatrix();
  myLabelX.Render (theWorkspace);
  myLabelY.Render (theWorkspace);
  myLabelZ.Render (theWorkspace);

  aContext->core11fwd->glDepthFunc (aDepthFuncBack);
  aContext->WorldViewState.Pop();
  aContext->ProjectionState.Pop();
  aContext->ApplyProjectionMatrix();
}

// =======================================================================
// function : redrawZBuffer
// purpose  :
// =======================================================================
void OpenGl_Trihedron::redrawZBuffer (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  Handle(OpenGl_Context) aContext = theWorkspace->GetGlContext();
  aContext->WorldViewState.Push();
  aContext->ProjectionState.Push();

  resetTransformations (theWorkspace);

  const GLdouble aScale = myScale * myRatio;

  const OpenGl_AspectLine* anAspectLine = theWorkspace->AspectLine (Standard_True);
  const TEL_COLOUR&        aLineColor   = anAspectLine->Color();

  // Create the trihedron
  const Standard_Real THE_CYLINDER_LENGTH = 0.75;
  const GLdouble aCylinderLength  = aScale * THE_CYLINDER_LENGTH;
  const GLdouble aCylinderDiametr = aScale * myDiameter;
  const GLdouble aConeDiametr     = aCylinderDiametr * 2.0;
  const GLdouble aConeLength      = aScale * (1.0 - THE_CYLINDER_LENGTH);

  // Position des Axes
  GLdouble aTriedronAxeX[3] = { aScale, 0.0,    0.0 };
  GLdouble aTriedronAxeY[3] = { 0.0,    aScale, 0.0 };
  if (!myDisk.IsInitialized())
  {
    myDisk.Init (static_cast<GLfloat> (aCylinderDiametr),
                 static_cast<GLfloat> (aConeDiametr),
                 myNbFacettes, 1);
  }

  if (!mySphere.IsInitialized())
  {
    mySphere.Init (static_cast<GLfloat> (aCylinderDiametr * 2.0), myNbFacettes, myNbFacettes);
  }

  if (!myCone.IsInitialized())
  {
    myCone.Init (static_cast<GLfloat> (aConeDiametr), 0.0f, static_cast<GLfloat> (aConeLength), myNbFacettes, 1);
  }

  if (!myCylinder.IsInitialized())
  {
    myCylinder.Init (static_cast<GLfloat> (aCylinderDiametr),
                     static_cast<GLfloat> (aCylinderDiametr),
                     static_cast<GLfloat> (aCylinderLength),
                     myNbFacettes, 1);
  }

  GLboolean wasDepthMaskEnabled = GL_FALSE;
  GLint aDepthFuncBack = 0, aCullFaceModeBack = GL_BACK;
  const GLboolean wasDepthEnabled    = aContext->core11fwd->glIsEnabled (GL_DEPTH_TEST);
  const GLboolean wasCullFaceEnabled = aContext->core11fwd->glIsEnabled (GL_CULL_FACE);
  aContext->core11fwd->glGetIntegerv (GL_DEPTH_FUNC,      &aDepthFuncBack);
  aContext->core11fwd->glGetIntegerv (GL_CULL_FACE_MODE,  &aCullFaceModeBack);
  aContext->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &wasDepthMaskEnabled);
  if (!wasDepthEnabled)
  {
    aContext->core11fwd->glEnable (GL_DEPTH_TEST);
    aContext->core11fwd->glClear (GL_DEPTH_BUFFER_BIT);
  }
  if (!wasDepthMaskEnabled)
  {
    aContext->core11fwd->glDepthMask (GL_TRUE);
  }
  aContext->core11fwd->glCullFace (GL_BACK);
  if (!wasCullFaceEnabled)
  {
    aContext->core11fwd->glEnable (GL_CULL_FACE);
  }

  OpenGl_AspectFace anAspectC;
  OpenGl_AspectFace anAspectX;
  OpenGl_AspectFace anAspectY;
  OpenGl_AspectFace anAspectZ;
  memcpy (anAspectX.ChangeIntFront().matcol.rgb, myXColor.GetData(), sizeof (TEL_COLOUR));
  memcpy (anAspectY.ChangeIntFront().matcol.rgb, myYColor.GetData(), sizeof (TEL_COLOUR));
  memcpy (anAspectZ.ChangeIntFront().matcol.rgb, myZColor.GetData(), sizeof (TEL_COLOUR));
  memcpy (anAspectC.ChangeIntFront().matcol.rgb, aLineColor.rgb,     sizeof (TEL_COLOUR));

  OpenGl_Mat4d aModelMatrix;
  aModelMatrix.Convert (aContext->WorldViewState.Current());
  for (Standard_Integer aPass = 0; aPass < 2; ++aPass)
  {
    OpenGl_Mat4d aModelViewX (aModelMatrix);
    OpenGl_Mat4d aModelViewY (aModelMatrix);
    OpenGl_Mat4d aModelViewZ (aModelMatrix);
    aContext->core11fwd->glDepthFunc (aPass == 0 ? GL_ALWAYS : GL_LEQUAL);

    const OpenGl_AspectFace* anOldAspect = theWorkspace->SetAspectFace (&anAspectC);

    // Origin
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
    aContext->ApplyWorldViewMatrix();
    mySphere.Render (theWorkspace);

    // Z axis
    theWorkspace->SetAspectFace (&anAspectZ);
    myCylinder.Render (theWorkspace);
    Graphic3d_TransformUtils::Translate (aModelViewZ, 0.0, 0.0, aScale * THE_CYLINDER_LENGTH);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewZ);
    aContext->ApplyWorldViewMatrix();
    myDisk.Render (theWorkspace);
    myCone.Render (theWorkspace);

    // X axis
    theWorkspace->SetAspectFace (&anAspectX);
    Graphic3d_TransformUtils::Rotate (aModelViewX, 90.0, aTriedronAxeY[0], aTriedronAxeY[1], aTriedronAxeY[2]);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewX);
    aContext->ApplyWorldViewMatrix();
    myCylinder.Render (theWorkspace);
    Graphic3d_TransformUtils::Translate (aModelViewX, 0.0, 0.0, aScale * THE_CYLINDER_LENGTH);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewX);
    aContext->ApplyWorldViewMatrix();
    myDisk.Render (theWorkspace);
    myCone.Render (theWorkspace);

    // Y axis
    theWorkspace->SetAspectFace (&anAspectY);
    Graphic3d_TransformUtils::Rotate (aModelViewY, -90.0, aTriedronAxeX[0], aTriedronAxeX[1], aTriedronAxeX[2]);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewY);
    aContext->ApplyWorldViewMatrix();
    myCylinder.Render (theWorkspace);
    Graphic3d_TransformUtils::Translate (aModelViewY, 0.0, 0.0, aScale * THE_CYLINDER_LENGTH);
    aContext->WorldViewState.SetCurrent<Standard_Real> (aModelViewY);
    aContext->ApplyWorldViewMatrix();
    myDisk.Render (theWorkspace);
    myCone.Render (theWorkspace);

    theWorkspace->SetAspectFace (anOldAspect);
  }

  if (!wasDepthEnabled)
  {
    aContext->core11fwd->glDisable (GL_DEPTH_TEST);
  }
  if (!wasDepthMaskEnabled)
  {
    aContext->core11fwd->glDepthMask (GL_FALSE);
  }
  if (!wasCullFaceEnabled)
  {
    aContext->core11fwd->glDisable (GL_CULL_FACE);
  }
  aContext->core11fwd->glCullFace (aCullFaceModeBack);

  // Always write the text
  aContext->core11fwd->glDepthFunc (GL_ALWAYS);

  // draw axes labels
  const GLdouble rayon = aScale / 30.0;
  myLabelX.SetPosition (OpenGl_Vec3(float(aScale + 2.0 * rayon), 0.0f,                        float(-rayon)));
  myLabelY.SetPosition (OpenGl_Vec3(float(rayon),                float(aScale + 3.0 * rayon), float(2.0 * rayon)));
  myLabelZ.SetPosition (OpenGl_Vec3(float(-2.0 * rayon),         float(0.5 * rayon),          float(aScale + 3.0 * rayon)));
  aContext->WorldViewState.SetCurrent<Standard_Real> (aModelMatrix);
  aContext->ApplyWorldViewMatrix();
  myLabelX.Render (theWorkspace);
  myLabelY.Render (theWorkspace);
  myLabelZ.Render (theWorkspace);

  aContext->core11fwd->glDepthFunc (aDepthFuncBack);

  aContext->WorldViewState.Pop();
  aContext->ProjectionState.Pop();
  aContext->ApplyProjectionMatrix();
}

// =======================================================================
// function : OpenGl_Trihedron
// purpose  :
// =======================================================================
OpenGl_Trihedron::OpenGl_Trihedron()
: myPos (Aspect_TOTP_LEFT_LOWER),
  myScale (1.0),
  myIsWireframe (Standard_False),
  myXColor (1.0f, 0.0f, 0.0f, 0.6f),
  myYColor (0.0f, 1.0f, 0.0f, 0.6f),
  myZColor (0.0f, 0.0f, 1.0f, 0.6f),
  myRatio      (0.8f),
  myDiameter   (0.05f),
  myNbFacettes (12),
  myLabelX ("X", OpenGl_Vec3(1.0f, 0.0f, 0.0f), THE_LABEL_PARAMS),
  myLabelY ("Y", OpenGl_Vec3(0.0f, 1.0f, 0.0f), THE_LABEL_PARAMS),
  myLabelZ ("Z", OpenGl_Vec3(0.0f, 0.0f, 1.0f), THE_LABEL_PARAMS),
  myLine   (NULL), // do not register arrays UID - trihedron is not intended to be drawn by Ray Tracing engine
  myCircle (NULL)
{
  const TEL_COLOUR aWhiteColor = {{ 1.0f, 1.0f, 1.0f, 1.0f }};
  myAspectLine.ChangeColor()    = aWhiteColor;
  myAspectText.ChangeColor()    = aWhiteColor;
  myAspectText.ChangeFontName() = "Courier";
}

// =======================================================================
// function : ~OpenGl_Trihedron
// purpose  :
// =======================================================================
OpenGl_Trihedron::~OpenGl_Trihedron()
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Trihedron::Release (OpenGl_Context* theCtx)
{
  myLabelX.Release (theCtx);
  myLabelY.Release (theCtx);
  myLabelZ.Release (theCtx);
  myAspectLine.Release (theCtx);
  myAspectText.Release (theCtx);
  myCone    .Release (theCtx);
  myDisk    .Release (theCtx);
  mySphere  .Release (theCtx);
  myCylinder.Release (theCtx);
  myLine.Release (theCtx);
  myCircle.Release (theCtx);
}

// =======================================================================
// function : invalidate
// purpose  :
// =======================================================================
void OpenGl_Trihedron::invalidate()
{
  myCone    .Invalidate();
  myDisk    .Invalidate();
  mySphere  .Invalidate();
  myCylinder.Invalidate();
  myLine    .Invalidate();
  myCircle  .Invalidate();
}

// =======================================================================
// function : SetScale
// purpose  :
// =======================================================================
void OpenGl_Trihedron::SetScale (const Standard_Real theScale)
{
  if (Abs (myScale - theScale) > Precision::Confusion())
  {
    invalidate();
  }
  myScale = theScale;
}

// =======================================================================
// function : SetSizeRatio
// purpose  :
// =======================================================================
void OpenGl_Trihedron::SetSizeRatio (const Standard_Real theRatio)
{
  if (Abs (Standard_Real(myRatio) - theRatio) > Precision::Confusion())
  {
    invalidate();
  }
  myRatio = float(theRatio);
}

// =======================================================================
// function : SetArrowDiameter
// purpose  :
// =======================================================================
void OpenGl_Trihedron::SetArrowDiameter (const Standard_Real theDiam)
{
  if (Abs (Standard_Real(myDiameter) - theDiam) > Precision::Confusion())
  {
    invalidate();
  }
  myDiameter = float(theDiam);
}

// =======================================================================
// function : SetNbFacets
// purpose  :
// =======================================================================
void OpenGl_Trihedron::SetNbFacets (const Standard_Integer theNbFacets)
{
  if (Abs (myNbFacettes - theNbFacets) > 0)
  {
    invalidate();
  }
  myNbFacettes = theNbFacets;
}

// =======================================================================
// function : SetLabelsColor
// purpose  :
// =======================================================================
void OpenGl_Trihedron::SetLabelsColor (const Quantity_Color& theColor)
{
  myAspectText.ChangeColor().rgb[0] = float(theColor.Red());
  myAspectText.ChangeColor().rgb[1] = float(theColor.Green());
  myAspectText.ChangeColor().rgb[2] = float(theColor.Blue());
}

// =======================================================================
// function : SetArrowsColors
// purpose  :
// =======================================================================
void OpenGl_Trihedron::SetArrowsColors (const Quantity_Color& theColorX,
                                        const Quantity_Color& theColorY,
                                        const Quantity_Color& theColorZ)
{
  myXColor = OpenGl_Vec4 (float(theColorX.Red()), float(theColorX.Green()), float(theColorX.Blue()), 0.6f);
  myYColor = OpenGl_Vec4 (float(theColorY.Red()), float(theColorY.Green()), float(theColorY.Blue()), 0.6f);
  myZColor = OpenGl_Vec4 (float(theColorZ.Red()), float(theColorZ.Green()), float(theColorZ.Blue()), 0.6f);
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Trihedron::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const OpenGl_AspectLine* aPrevAspectLine = theWorkspace->SetAspectLine (&myAspectLine);
  const OpenGl_AspectText* aPrevAspectText = theWorkspace->SetAspectText (&myAspectText);

  /* check if GL_LIGHTING should be disabled
  no enabling 'cause it will be done (if necessary: kinda Polygon types )
  during redrawing structures
  */
  if (!theWorkspace->UseGLLight())
  {
  #if !defined(GL_ES_VERSION_2_0)
    glDisable (GL_LIGHTING);
  #endif
  }

  const Handle(OpenGl_Texture) aPrevTexture = theWorkspace->DisableTexture();

  theWorkspace->GetGlContext()->ApplyModelViewMatrix();

  if (myIsWireframe)
  {
    redraw (theWorkspace);
  }
  else
  {
    redrawZBuffer (theWorkspace);
  }

  // restore aspects
  if (!aPrevTexture.IsNull())
  {
    theWorkspace->EnableTexture (aPrevTexture);
  }

  theWorkspace->SetAspectText (aPrevAspectText);
  theWorkspace->SetAspectLine (aPrevAspectLine);
}
