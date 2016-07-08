// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_GraduatedTrihedron.hxx>

#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <gp_Ax3.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_View.hxx>
#include <Precision.hxx>

#ifndef _WIN32
  #include <string.h>
#endif

namespace
{
  static const OpenGl_TextParam THE_LABEL_PARAMS =
  {
    16, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM
  };
}

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
OpenGl_GraduatedTrihedron::OpenGl_GraduatedTrihedron()
: myMin (0.0f, 0.0f, 0.0f),
  myMax (100.0f, 100.0f, 100.0f),
  myIsInitialized (Standard_False)
{
  //
}

// =======================================================================
// function : SetValues
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::SetValues (const Graphic3d_GraduatedTrihedron& theData)
{
  myData          = theData;
  myIsInitialized = Standard_False;
}

// =======================================================================
// function : Destructor
// purpose  :
// =======================================================================
OpenGl_GraduatedTrihedron::~OpenGl_GraduatedTrihedron()
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::Release (OpenGl_Context* theCtx)
{
  myAxes[0].Release (theCtx);
  myAxes[1].Release (theCtx);
  myAxes[2].Release (theCtx);
  myLabelValues.Release (theCtx);
}

// =======================================================================
// function : initResources
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::initGlResources (const Handle(OpenGl_Context)& theCtx) const
{
  myAxes[0].Release     (theCtx.operator->());
  myAxes[1].Release     (theCtx.operator->());
  myAxes[2].Release     (theCtx.operator->());
  myLabelValues.Release (theCtx.operator->());

  // Initialize text label parameters for x, y, and z axes
  myAxes[0] = Axis (myData.XAxisAspect(), OpenGl_Vec3 (1.0f, 0.0f, 0.0f));
  myAxes[1] = Axis (myData.YAxisAspect(), OpenGl_Vec3 (0.0f, 1.0f, 0.0f));
  myAxes[2] = Axis (myData.ZAxisAspect(), OpenGl_Vec3 (0.0f, 0.0f, 1.0f));

  // Initialize constant primitives: text, arrows.
  myAxes[0].InitArrow (theCtx, myData.ArrowsLength(), OpenGl_Vec3 (0.0f, 0.0f, 1.0f));
  myAxes[1].InitArrow (theCtx, myData.ArrowsLength(), OpenGl_Vec3 (0.0f, 0.0f, 1.0f));
  myAxes[2].InitArrow (theCtx, myData.ArrowsLength(), OpenGl_Vec3 (1.0f, 0.0f, 0.0f));
  for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].Label.SetFontSize (theCtx, myData.NamesSize());
  }

  myLabelValues.SetFontSize (theCtx, myData.ValuesSize());

  myAspectLabels.Aspect()->SetTextFontAspect (myData.NamesFontAspect());
  myAspectLabels.Aspect()->SetFont (myData.NamesFont());

  myAspectValues.Aspect()->SetTextFontAspect (myData.ValuesFontAspect());
  myAspectValues.Aspect()->SetFont (myData.ValuesFont());

  // Grid aspect
  myGridLineAspect.Aspect()->SetColor (myData.GridColor());
}

// =======================================================================
// method  : getNormal
// purpose : Normal of the view (not normalized!)
// =======================================================================
Standard_ShortReal OpenGl_GraduatedTrihedron::getNormal (const Handle(OpenGl_Context)& theContext,
                                                         OpenGl_Vec3& theNormal) const
{
  const Standard_Integer* aViewport = theContext->Viewport();

  OpenGl_Mat4 aModelMatrix;
  OpenGl_Mat4 aProjMatrix;
  aModelMatrix.Convert (theContext->ModelWorldState.Current() * theContext->WorldViewState.Current());
  aProjMatrix .Convert (theContext->ProjectionState.Current());

  OpenGl_Vec3 aPoint1, aPoint2, aPoint3;
  Graphic3d_TransformUtils::UnProject<Standard_ShortReal> ((Standard_ShortReal) aViewport[0],
                                                           (Standard_ShortReal) aViewport[1],
                                                           0.0f,
                                                           aModelMatrix, aProjMatrix, aViewport,
                                                           aPoint1.x(), aPoint1.y(), aPoint1.z());

  Graphic3d_TransformUtils::UnProject<Standard_ShortReal> ((Standard_ShortReal) (aViewport[0] + aViewport[2]),
                                                           (Standard_ShortReal) aViewport[1],
                                                           0.0f,
                                                           aModelMatrix, aProjMatrix, aViewport,
                                                           aPoint2.x(), aPoint2.y(), aPoint2.z());

  Graphic3d_TransformUtils::UnProject<Standard_ShortReal> ((Standard_ShortReal) aViewport[0],
                                                           (Standard_ShortReal) (aViewport[1] + aViewport[3]),
                                                           0.0f,
                                                           aModelMatrix, aProjMatrix, aViewport,
                                                           aPoint3.x(), aPoint3.y(), aPoint3.z());

  const OpenGl_Vec3 aD1 = aPoint3 - aPoint1;
  const OpenGl_Vec3 aD2 = aPoint2 - aPoint1;
  theNormal =  OpenGl_Vec3::Cross (aD1, aD2);

  // Distance corresponding to 1 pixel
  return aD2.Modulus() / (float) aViewport[2];
}

// =======================================================================
// method  : getDistancetoCorner
// purpose : 
// =======================================================================
Standard_ShortReal OpenGl_GraduatedTrihedron::getDistanceToCorner (const OpenGl_Vec3& theNormal,
                                                                   const OpenGl_Vec3& theCenter,
                                                                   const Standard_ShortReal theX,
                                                                   const Standard_ShortReal theY,
                                                                   const Standard_ShortReal theZ) const
{
  return theNormal.x() * (theX - theCenter.x())
       + theNormal.y() * (theY - theCenter.y())
       + theNormal.z() * (theZ - theCenter.z());
}

// =======================================================================
// method  : getGridAxes
// purpose : 
// =======================================================================
Standard_ExtCharacter OpenGl_GraduatedTrihedron::getGridAxes (const Standard_ShortReal theCorners[8],
                                                              GridAxes& theGridAxes) const
{
  // Find the farest corner
  Standard_Byte aMaxIndex = 0;
  Standard_ShortReal aMax = theCorners[aMaxIndex] > 0.0f ? theCorners[aMaxIndex] : 0.0f;

  for (Standard_Byte anIt = 0; anIt < 8; ++anIt)
  {
    if (theCorners[anIt] > aMax)
    {
      aMax = theCorners[anIt];
      aMaxIndex = anIt;
    }
  }

  switch (aMaxIndex)
  {
    case 0: // (0,0,0)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0.0f, 1.0f, 0.0f);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0.0f, 0.0f, 1.0f);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMax.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());

      return OOZ_XOZ | OYO_XYO |
             XOO_XYO | OOZ_OYZ |
             XOO_XOZ | OYO_OYZ;
    }
    case 1: // (0,0,1)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMin.x(), myMin.y(), myMax.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0.0f, 1.0f, 0.0f);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0.0f, 0.0f, -1.0f);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMax.z());

      return OOZ_XOZ | OYZ_XYZ |
             OOZ_OYZ | XOZ_XYZ |
             XOO_XOZ | OYO_OYZ;
    }
    case 2: // (0,1,0)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMin.x(), myMax.y(), myMin.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0.0f, -1.0f, 0.0f);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0.0f, 0.0f, 1.0f);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMax.x(), myMax.y(), myMin.z());

      return OYO_XYO | OYZ_XYZ |
             XOO_XYO | OOZ_OYZ |
             XYO_XYZ | OYO_OYZ;
    }
    case 3: // (0,1,1)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMin.x(), myMax.y(), myMax.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0.0f, -1.0f, 0.0f);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0.0f, 0.0f, -1.0f);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMax.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMin.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMax.z());

      return OOZ_XOZ | OYZ_XYZ | OYO_XYO |
             OOZ_OYZ | XOZ_XYZ |
             OYO_OYZ | XYO_XYZ;
    }
    case 4: // (1,0,0)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (-1, 0, 0);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0, 1, 0);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0, 0, 1);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMax.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMin.z());

      return OOZ_XOZ | OYO_XYO |
             XOO_XYO | XOZ_XYZ |
             XOO_XOZ | XYO_XYZ;
    }
    case 5: // (1,0,1)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMax.x(), myMin.y(), myMax.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (-1, 0, 0);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0, 1, 0);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0, 0, -1);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMin.x(), myMin.y(), myMax.z());

      return OOZ_XOZ | OYZ_XYZ |
             XOO_XYO | XOZ_XYZ | OOZ_OYZ |
             XOO_XOZ | XYO_XYZ;
    }
    case 6: // (1,1,0)
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (-1, 0, 0);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0, -1, 0);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0, 0, 1);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMin.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMin.z());

      return OYO_XYO | OYZ_XYZ |
             XOO_XYO | XOZ_XYZ |
             XOO_XOZ | XYO_XYZ | OYO_OYZ;
    }
    case 7: // (1,1,1)
    default:
    {
      theGridAxes.Origin = OpenGl_Vec3 (myMax.x(), myMax.y(), myMax.z());
      theGridAxes.Axes[0] = OpenGl_Vec3 (-1, 0, 0);
      theGridAxes.Axes[1] = OpenGl_Vec3 (0, -1, 0);
      theGridAxes.Axes[2] = OpenGl_Vec3 (0, 0, -1);

      theGridAxes.Ticks[0] = OpenGl_Vec3 (myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[1] = OpenGl_Vec3 (myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = OpenGl_Vec3 (myMax.x(), myMin.y(), myMax.z());

      return OYO_XYO | OYZ_XYZ | OOZ_XOZ |
             XOO_XYO | XOZ_XYZ | OOZ_OYZ |
             XOO_XOZ | XYO_XYZ | OYO_OYZ;
    }
  }
}

// =======================================================================
// function : renderLine
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::renderLine (const OpenGl_PrimitiveArray&    theLine,
                                            const Handle(OpenGl_Workspace)& theWorkspace,
                                            const OpenGl_Mat4& theMat,
                                            const Standard_ShortReal theXt,
                                            const Standard_ShortReal theYt,
                                            const Standard_ShortReal theZt) const
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  OpenGl_Mat4 aMat (theMat);
  Graphic3d_TransformUtils::Translate (aMat, theXt, theYt, theZt);
  aContext->WorldViewState.SetCurrent (aMat);
  aContext->ApplyWorldViewMatrix();
  theLine.Render (theWorkspace);
}

// =======================================================================
// function : renderGridPlane
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::renderGridPlane (const Handle(OpenGl_Workspace)& theWorkspace,
                                                 const Standard_Integer& theIndex,
                                                 const GridAxes& theGridAxes,
                                                 OpenGl_Mat4& theMat) const
{
  const Graphic3d_AxisAspect& aCurAspect = myData.AxisAspect (theIndex);
  if (aCurAspect.TickmarksNumber() <= 0)
  {
    return;
  }

  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  Standard_ShortReal aStep = theGridAxes.Axes[theIndex].GetData()[theIndex]
                            * (myMax.GetData()[theIndex] - myMin.GetData()[theIndex]) / aCurAspect.TickmarksNumber();

  // NOTE:
  // Get two other axes directions and draw lines Axis.TickmarksNumber times.
  // Combining together from three axes, these lines will make a grid.
  for (Standard_Integer anIter = 1; anIter <= 2; ++anIter)
  {
    OpenGl_Mat4 aMat (theMat);
    const Standard_Integer anIndex = (theIndex + anIter) % 3;
    const Axis& anAxis = myAxes[anIndex];
    OpenGl_Vec3 aStart (theGridAxes.Origin);
    if (theGridAxes.Axes[anIndex].GetData()[anIndex] < 0.0)
    {
      aStart.ChangeData()[anIndex] = myMin.GetData()[anIndex];
    }

    Graphic3d_TransformUtils::Translate (aMat, aStart.x(), aStart.y(), aStart.z());
    aContext->WorldViewState.SetCurrent (aMat);
    aContext->ApplyWorldViewMatrix();

    const OpenGl_Vec3 aStepVec (myAxes[theIndex].Direction * aStep);
    for (Standard_Integer anIt = myData.ToDrawAxes() ? 1 : 0; anIt < aCurAspect.TickmarksNumber(); ++anIt)
    {
      Graphic3d_TransformUtils::Translate (aMat, aStepVec.x(), aStepVec.y(), aStepVec.z());
      aContext->WorldViewState.SetCurrent (aMat);
      aContext->ApplyWorldViewMatrix();
      anAxis.Line.Render (theWorkspace);
    }
  }
}

// =======================================================================
// function : renderAxis
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::renderAxis (const Handle(OpenGl_Workspace)& theWorkspace,
                                            const Standard_Integer& theIndex,
                                            const OpenGl_Mat4& theMat) const
{
  const Axis& anAxis = myAxes[theIndex];

  theWorkspace->SetAspectLine (&anAxis.LineAspect);
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  // Reset transformations
  aContext->WorldViewState.SetCurrent (theMat);
  aContext->ApplyWorldViewMatrix();

  // Render arrow
  OpenGl_Vec3 anArrowVec = myMin + anAxis.Direction * (myMax - myMin);

  Graphic3d_TransformPers aTransMode (Graphic3d_TMF_ZoomPers, gp_Pnt (Standard_Real(anArrowVec.x()),
                                                                      Standard_Real(anArrowVec.y()),
                                                                      Standard_Real(anArrowVec.z())));
  const OpenGl_Mat4& aProjection = aContext->ProjectionState.Current();
  const OpenGl_Mat4& aWorldView  = aContext->WorldViewState.Current();
  const Standard_Integer aWidth  = theWorkspace->Width();
  const Standard_Integer aHeight = theWorkspace->Height();

  // Take into account Transform Persistence
  aContext->ModelWorldState.SetCurrent (aTransMode.Compute (theWorkspace->View()->Camera(), aProjection, aWorldView, aWidth, aHeight));
  aContext->ApplyModelViewMatrix();

  anAxis.Arrow.Render (theWorkspace);

  // Get current Model-View and Projection states
  OpenGl_Mat4 aModelMat;
  OpenGl_Mat4 aProjMat;
  aModelMat.Convert (aContext->WorldViewState.Current() * aContext->ModelWorldState.Current());
  aProjMat .Convert (aContext->ProjectionState.Current());

  // Get the window's (fixed) coordinates for before matrices modifications
  OpenGl_Vec3 aEndPoint = -anAxis.Direction * myData.ArrowsLength();
  OpenGl_Vec3 aWinPoint;
  Graphic3d_TransformUtils::Project<Standard_ShortReal> (aEndPoint.x(), aEndPoint.y(), aEndPoint.z(),
                                                         aModelMat, aProjMat, aContext->Viewport(),
                                                         aWinPoint.x(), aWinPoint.y(), aWinPoint.z());

  aContext->ModelWorldState.SetIdentity();
  aModelMat.Convert (aContext->WorldViewState.Current());
  aProjMat .Convert (aContext->ProjectionState.Current());

  // Get start point of zoom persistent arrow
  OpenGl_Vec3 anArrowStart;
  Graphic3d_TransformUtils::UnProject<Standard_ShortReal> (aWinPoint.x(), aWinPoint.y(), aWinPoint.z(),
                                                           aModelMat, aProjMat, aContext->Viewport(),
                                                           anArrowStart.x(), anArrowStart.y(), anArrowStart.z());
  // Render axis line
  aModelMat = theMat;
  Graphic3d_TransformUtils::Translate (aModelMat, myMin.x(), myMin.y(), myMin.z());

  Standard_ShortReal aScaleFactor = ( (anArrowStart - myMin)*anAxis.Direction ).Modulus()
                                     / (anAxis.Direction * (myMax - myMin) ).Modulus();
  OpenGl_Vec3 aScaleAxes = anAxis.Direction * aScaleFactor;
  Graphic3d_TransformUtils::Scale (aModelMat, aScaleAxes.x(), aScaleAxes.y(), aScaleAxes.z());

  aContext->WorldViewState.SetCurrent (aModelMat);
  aContext->ApplyWorldViewMatrix();
  anAxis.Line.Render (theWorkspace);
}

// =======================================================================
// function : renderTickmarkTextLabels
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::renderTickmarkLabels (const Handle(OpenGl_Workspace)& theWorkspace,
                                                      const OpenGl_Mat4& theMat,
                                                      const Standard_Integer theIndex,
                                                      const GridAxes& theGridAxes,
                                                      const Standard_ShortReal theDpix) const
{
  const Graphic3d_AxisAspect& aCurAspect = myData.AxisAspect (theIndex);
  if (!aCurAspect.ToDrawName() && !aCurAspect.ToDrawValues())
  {
    return;
  }

  Standard_Character aTextValue[128];
  const Axis& anAxis = myAxes[theIndex];
  const OpenGl_Vec3 aSizeVec (myMax - myMin);
  Standard_ShortReal aStep = theGridAxes.Axes[theIndex].GetData()[theIndex]
                       * (myMax.GetData()[theIndex] - myMin.GetData()[theIndex]) / aCurAspect.TickmarksNumber();

  OpenGl_Vec3 aDir = (theGridAxes.Ticks[theIndex] - theGridAxes.Origin).Normalized();
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();

  if (aCurAspect.ToDrawTickmarks() && aCurAspect.TickmarksNumber() > 0)
  {
    theWorkspace->SetAspectLine (&myGridLineAspect);

    OpenGl_Mat4 aModelMat (theMat);

    anAxis.InitTickmark (aContext, aDir * (Standard_ShortReal) aCurAspect.TickmarksLength() * theDpix);
    Graphic3d_TransformUtils::Translate (aModelMat, theGridAxes.Ticks[theIndex].x(),
                                                    theGridAxes.Ticks[theIndex].y(),
                                                    theGridAxes.Ticks[theIndex].z());
    aContext->WorldViewState.SetCurrent (aModelMat);
    aContext->ApplyWorldViewMatrix();
    OpenGl_Vec3 aStepVec = anAxis.Direction * aStep;
    for (Standard_Integer anIter = 0; anIter <= aCurAspect.TickmarksNumber(); ++anIter)
    {
      anAxis.Tickmark.Render (theWorkspace);
      Graphic3d_TransformUtils::Translate (aModelMat, aStepVec.x(), aStepVec.y(), aStepVec.z());
      aContext->WorldViewState.SetCurrent (aModelMat);
      aContext->ApplyWorldViewMatrix();
    }
  }

  // Restore matrix
  aContext->WorldViewState.SetCurrent (theMat);
  aContext->ApplyWorldViewMatrix();

  if (aCurAspect.ToDrawName())
  {
    Standard_Real anOffset = aCurAspect.NameOffset() + aCurAspect.TickmarksLength();

    OpenGl_Vec3 aMiddle (theGridAxes.Ticks[theIndex] + aSizeVec * theGridAxes.Axes[theIndex] * 0.5f + aDir * (Standard_ShortReal)(theDpix * anOffset));

    myAspectLabels.Aspect()->SetColor (anAxis.NameColor);
    theWorkspace->SetAspectText (&myAspectLabels);
    anAxis.Label.SetPosition (aMiddle);
    anAxis.Label.Render (theWorkspace);
  }

  if (aCurAspect.ToDrawValues() && aCurAspect.TickmarksNumber() > 0)
  {
    myAspectValues.Aspect()->SetColor (anAxis.LineAspect.Aspect()->Color());
    theWorkspace->SetAspectText (&myAspectValues);
    Standard_Real anOffset = aCurAspect.ValuesOffset() + aCurAspect.TickmarksLength();

    for (Standard_Integer anIt = 0; anIt <= aCurAspect.TickmarksNumber(); ++anIt)
    {
      sprintf (aTextValue, "%g", theGridAxes.Ticks[theIndex].GetData()[theIndex] + anIt * aStep);
      OpenGl_Vec3 aPos (theGridAxes.Ticks[theIndex] + anAxis.Direction* (Standard_ShortReal) (anIt * aStep) + aDir * (Standard_ShortReal) (theDpix * anOffset));
      myLabelValues.Init (theWorkspace->GetGlContext(), aTextValue, aPos);
      myLabelValues.Render (theWorkspace);
    }
  }
}

// =======================================================================
// function : Render
// purpose  : call_graduatedtrihedron_redraw
// =======================================================================
void OpenGl_GraduatedTrihedron::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  if (!myIsInitialized)
  {
    initGlResources (theWorkspace->GetGlContext());
    myIsInitialized = Standard_True;
  }

  // Update boundary box
  OpenGl_Vec3 anOldMin = myMin;
  OpenGl_Vec3 anOldMax = myMax;

  if (myData.CubicAxesCallback)
  {
    myData.CubicAxesCallback (myData.PtrView);
    if (!myAxes[0].Line.IsInitialized()
     || !myAxes[1].Line.IsInitialized()
     || !myAxes[2].Line.IsInitialized()
     ||  OpenGl_Vec3 (anOldMin - myMin).Modulus() > Precision::Confusion()
     ||  OpenGl_Vec3 (anOldMax - myMax).Modulus() > Precision::Confusion())
    {
      myAxes[0].InitLine (aContext, OpenGl_Vec3 (myMax.x() - myMin.x(), 0.0f, 0.0f));
      myAxes[1].InitLine (aContext, OpenGl_Vec3 (0.0f, myMax.y() - myMin.y(), 0.0f));
      myAxes[2].InitLine (aContext, OpenGl_Vec3 (0.0f, 0.0f, myMax.z() - myMin.z()));
    }
  }

  // Find the farest point of bounding box

  // Get normal of the view out of user and distance corresponding to 1 pixel
  OpenGl_Vec3 aNormal;
  Standard_ShortReal aDpix = getNormal (aContext, aNormal);
  aNormal.Normalize();

  // Get central point of bounding box
  OpenGl_Vec3 aCenter;
  aCenter = (myMin + myMax) * 0.5f;

  // Check distance to corners of bounding box along the normal
  Standard_ShortReal aCorners[8];
  aCorners[0] = getDistanceToCorner (aNormal, aCenter, myMin.x(), myMin.y(), myMin.z());
  aCorners[1] = getDistanceToCorner (aNormal, aCenter, myMin.x(), myMin.y(), myMax.z());
  aCorners[2] = getDistanceToCorner (aNormal, aCenter, myMin.x(), myMax.y(), myMin.z());
  aCorners[3] = getDistanceToCorner (aNormal, aCenter, myMin.x(), myMax.y(), myMax.z());
  aCorners[4] = getDistanceToCorner (aNormal, aCenter, myMax.x(), myMin.y(), myMin.z());
  aCorners[5] = getDistanceToCorner (aNormal, aCenter, myMax.x(), myMin.y(), myMax.z());
  aCorners[6] = getDistanceToCorner (aNormal, aCenter, myMax.x(), myMax.y(), myMin.z());
  aCorners[7] = getDistanceToCorner (aNormal, aCenter, myMax.x(), myMax.y(), myMax.z());

  // NOTE:
  // (0, 0, 1), (0, 1, 0) and (0, 0, 1) directions from (myMin.x(), Ymin, Zmin) point
  // are reserved for trihedron axes.
  // So for the grid here are 9 edges of cube,
  // and, depending on the farest point, 2 or 3 of them may not be drawn
  // if they overlap displayed model.
  
  // Write an axes state what axes of bounding box are to be drawn
  GridAxes aGridAxes;
  Standard_ExtCharacter anAxesState = getGridAxes (aCorners, aGridAxes);

  // Remember current aspects
  const OpenGl_AspectLine* anOldAspectLine = theWorkspace->AspectLine();
  const OpenGl_AspectText* anOldAspectText = theWorkspace->AspectText();

  OpenGl_Mat4 aModelMatrix;
  aModelMatrix.Convert (aContext->WorldViewState.Current());

  // Remember model-view matrix
  aContext->WorldViewState.Push();
  aContext->WorldViewState.SetCurrent (aModelMatrix);
  aContext->ApplyWorldViewMatrix();

  if (myData.ToDrawGrid())
  {
    theWorkspace->SetAspectLine (&myGridLineAspect);

    // render grid edges
    if (anAxesState & XOO_XYO)
    {
      renderLine (myAxes[1].Line, theWorkspace, aModelMatrix, myMax.x(), myMin.y(), myMin.z());
    }

    if (anAxesState & XOO_XOZ)
    {
      renderLine (myAxes[2].Line,theWorkspace, aModelMatrix, myMax.x(), myMin.y(), myMin.z());
    }

    if (anAxesState & OYO_OYZ)
    {
      renderLine (myAxes[2].Line, theWorkspace, aModelMatrix, myMin.x(), myMax.y(), myMin.z());
    }

    if (anAxesState & OYO_XYO)
    {
      renderLine (myAxes[0].Line, theWorkspace, aModelMatrix, myMin.x(), myMax.y(), myMin.z());
    }

    if (anAxesState & OOZ_XOZ)
    {
      renderLine (myAxes[0].Line, theWorkspace, aModelMatrix, myMin.z(), myMin.y(), myMax.z());
    }

    if (anAxesState & OOZ_OYZ)
    {
      renderLine (myAxes[1].Line, theWorkspace, aModelMatrix, myMin.x(), myMin.y(), myMax.z());
    }

    if (anAxesState & OYZ_XYZ)
    {
      renderLine (myAxes[0].Line, theWorkspace, aModelMatrix, myMin.x(), myMax.y(), myMax.z());
    }

    if (anAxesState & XOZ_XYZ)
    {
      renderLine (myAxes[1].Line, theWorkspace, aModelMatrix, myMax.x(), myMin.y(), myMax.z());
    }

    if (anAxesState & XYO_XYZ)
    {
      renderLine (myAxes[2].Line, theWorkspace, aModelMatrix, myMax.x(), myMax.y(), myMin.z());
    }

    for (Standard_Integer anIter = 0 ; anIter < 3; ++anIter)
    {
      renderGridPlane (theWorkspace, anIter, aGridAxes, aModelMatrix);
    }
  }

  // Axes (arrows)
  if (myData.ToDrawAxes())
  {
    for (Standard_Integer anIter = 0; anIter < 3; ++anIter)
    {
      renderAxis (theWorkspace, anIter, aModelMatrix);
    }
  }

  // Names of axes & values
  for (Standard_Integer anIter = 0; anIter < 3; ++anIter)
  {
    // Restore current matrix
    aContext->WorldViewState.SetCurrent (aModelMatrix);
    aContext->ApplyWorldViewMatrix();
    renderTickmarkLabels (theWorkspace, aModelMatrix, anIter, aGridAxes, aDpix);
  }

  theWorkspace->SetAspectLine (anOldAspectLine);
  theWorkspace->SetAspectText (anOldAspectText);

  aContext->WorldViewState.Pop();
  aContext->ApplyWorldViewMatrix();
}

// =======================================================================
// method  : SetMinMax
// purpose :
// =======================================================================
void OpenGl_GraduatedTrihedron::SetMinMax (const OpenGl_Vec3& theMin, const OpenGl_Vec3& theMax)
{
  myMin = theMin;
  myMax = theMax;
}

// =======================================================================
// method  : OpenGl_GraduatedTrihedron::Axis constructor
// purpose :
// =======================================================================
OpenGl_GraduatedTrihedron::Axis::Axis (const Graphic3d_AxisAspect& theAspect,
                                       const OpenGl_Vec3&          theDirection)
: Direction (theDirection),
  Label     (NCollection_String ((Standard_Utf16Char* )theAspect.Name().ToExtString()).ToCString(), theDirection, THE_LABEL_PARAMS),
  Tickmark  (NULL),
  Line      (NULL),
  Arrow     (NULL)
{
  NameColor = theAspect.NameColor();
  LineAspect.Aspect()->SetColor (theAspect.Color());
}

// =======================================================================
// method  : OpenGl_GraduatedTrihedron::Axis::~Axis
// purpose :
// =======================================================================
OpenGl_GraduatedTrihedron::Axis::~Axis()
{
  //
}

// =======================================================================
// method  : OpenGl_GraduatedTrihedron::Axis operator=
// purpose :
// =======================================================================
OpenGl_GraduatedTrihedron::Axis& OpenGl_GraduatedTrihedron::Axis::operator= (const Axis& theOther)
{
  Direction  = theOther.Direction;
  NameColor  = theOther.NameColor;
  LineAspect = theOther.LineAspect;
  Label      = theOther.Label;

  Line    .InitBuffers (NULL, Graphic3d_TOPA_SEGMENTS,  theOther.Line.Indices(),     theOther.Line.Attributes(),     theOther.Line.Bounds());
  Tickmark.InitBuffers (NULL, Graphic3d_TOPA_SEGMENTS,  theOther.Tickmark.Indices(), theOther.Tickmark.Attributes(), theOther.Tickmark.Bounds());
  Arrow   .InitBuffers (NULL, Graphic3d_TOPA_POLYLINES, theOther.Arrow.Indices(),    theOther.Arrow.Attributes(),    theOther.Arrow.Bounds());
  return *this;
}

// =======================================================================
// method  : InitArrow
// purpose :
// =======================================================================
void OpenGl_GraduatedTrihedron::Axis::InitArrow (const Handle(OpenGl_Context)& theContext,
                                                 const Standard_ShortReal theLength,
                                                 const OpenGl_Vec3& theNormal) const
{
  // Draw from the end point of the aris
  OpenGl_Vec3 aLengthVec = -Direction * theLength;

  // Radial direction to the arrow
  OpenGl_Vec3 aRadial = OpenGl_Vec3::Cross (this->Direction, theNormal);
  if (aRadial.Modulus() < (Standard_ShortReal) Precision::Confusion())
  {
    return;
  }
  aRadial = aRadial.Normalized() * theLength * 0.2f;

  // Initialize arrow primitive array
  // Make loop from polyline
  const OpenGl_Vec3 aPoint1 = aRadial + aLengthVec;
  const OpenGl_Vec3 aPoint2 (0.0f, 0.0f, 0.0f);
  const OpenGl_Vec3 aPoint3 = -aRadial + aLengthVec;

  Handle(Graphic3d_ArrayOfPolylines) anArray = new Graphic3d_ArrayOfPolylines (4);
  anArray->AddVertex (aPoint1);
  anArray->AddVertex (aPoint2);
  anArray->AddVertex (aPoint3);
  anArray->AddVertex (aPoint1);

  Arrow.InitBuffers (theContext, Graphic3d_TOPA_POLYLINES,
                     anArray->Indices(), anArray->Attributes(), anArray->Bounds());
}

// =======================================================================
// function : InitTickmark
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::Axis::InitTickmark (const Handle(OpenGl_Context)& theContext,
                                                    const OpenGl_Vec3& theDir) const
{

  Handle(Graphic3d_ArrayOfSegments) anArray = new Graphic3d_ArrayOfSegments (2);
  anArray->AddVertex (0.0f, 0.0f, 0.0f);
  anArray->AddVertex (theDir);
  Tickmark.InitBuffers (theContext, Graphic3d_TOPA_SEGMENTS,
                        anArray->Indices(), anArray->Attributes(), anArray->Bounds());

}

// =======================================================================
// function : InitLine
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::Axis::InitLine (const Handle(OpenGl_Context)& theContext,
                                                const OpenGl_Vec3& theDir) const
{

  Handle(Graphic3d_ArrayOfSegments) anArray = new Graphic3d_ArrayOfSegments (2);
  anArray->AddVertex (0.0f, 0.0f, 0.0f);
  anArray->AddVertex (theDir);

  Line.InitBuffers (theContext, Graphic3d_TOPA_SEGMENTS,
                    anArray->Indices(), anArray->Attributes(), anArray->Bounds());
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_GraduatedTrihedron::Axis::Release (OpenGl_Context* theCtx)
{
  Label   .Release (theCtx);
  Tickmark.Release (theCtx);
  Line    .Release (theCtx);
  Arrow   .Release (theCtx);
}
