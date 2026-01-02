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

#include <OpenGl_GraduatedTrihedron.hxx>

#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Text.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <OpenGl_View.hxx>
#include <Precision.hxx>

#ifndef _WIN32
  #include <cstring>
#endif

namespace
{
static float                             THE_LABEL_HEIGHT = 16;
static Graphic3d_HorizontalTextAlignment THE_LABEL_HALIGH = Graphic3d_HTA_LEFT;
static Graphic3d_VerticalTextAlignment   THE_LABEL_VALIGH = Graphic3d_VTA_BOTTOM;
} // namespace

//=================================================================================================

OpenGl_GraduatedTrihedron::OpenGl_GraduatedTrihedron()
    : myMin(0.0f, 0.0f, 0.0f),
      myMax(100.0f, 100.0f, 100.0f),
      myIsInitialized(false)
{
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::SetValues(const Graphic3d_GraduatedTrihedron& theData)
{
  myData          = theData;
  myIsInitialized = false;
}

//=================================================================================================

OpenGl_GraduatedTrihedron::~OpenGl_GraduatedTrihedron() = default;

//=================================================================================================

void OpenGl_GraduatedTrihedron::Release(OpenGl_Context* theCtx)
{
  myAxes[0].Release(theCtx);
  myAxes[1].Release(theCtx);
  myAxes[2].Release(theCtx);
  myLabelValues.Release(theCtx);
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::initGlResources(const occ::handle<OpenGl_Context>& theCtx) const
{
  myAxes[0].Release(theCtx.operator->());
  myAxes[1].Release(theCtx.operator->());
  myAxes[2].Release(theCtx.operator->());
  myLabelValues.Release(theCtx.operator->());

  // Initialize text label parameters for x, y, and z axes
  myAxes[0] = Axis(myData.XAxisAspect(), NCollection_Vec3<float>(1.0f, 0.0f, 0.0f));
  myAxes[1] = Axis(myData.YAxisAspect(), NCollection_Vec3<float>(0.0f, 1.0f, 0.0f));
  myAxes[2] = Axis(myData.ZAxisAspect(), NCollection_Vec3<float>(0.0f, 0.0f, 1.0f));

  // Initialize constant primitives: text, arrows.
  myAxes[0].InitArrow(theCtx, myData.ArrowsLength(), NCollection_Vec3<float>(0.0f, 0.0f, 1.0f));
  myAxes[1].InitArrow(theCtx, myData.ArrowsLength(), NCollection_Vec3<float>(0.0f, 0.0f, 1.0f));
  myAxes[2].InitArrow(theCtx, myData.ArrowsLength(), NCollection_Vec3<float>(1.0f, 0.0f, 0.0f));
  for (int anIt = 0; anIt < 3; ++anIt)
  {
    myAxes[anIt].Label.SetFontSize(theCtx, myData.NamesSize());
  }

  myLabelValues.SetFontSize(theCtx, myData.ValuesSize());

  myAspectLabels.Aspect()->SetAlphaMode(Graphic3d_AlphaMode_MaskBlend, 0.285f);
  myAspectLabels.Aspect()->SetTextFontAspect(myData.NamesFontAspect());
  myAspectLabels.Aspect()->SetTextFont(!myData.NamesFont().IsEmpty()
                                         ? new TCollection_HAsciiString(myData.NamesFont())
                                         : occ::handle<TCollection_HAsciiString>());

  myAspectValues.Aspect()->SetAlphaMode(Graphic3d_AlphaMode_MaskBlend, 0.285f);
  myAspectValues.Aspect()->SetTextFontAspect(myData.ValuesFontAspect());
  myAspectValues.Aspect()->SetTextFont(!myData.ValuesFont().IsEmpty()
                                         ? new TCollection_HAsciiString(myData.ValuesFont())
                                         : occ::handle<TCollection_HAsciiString>());

  // Grid aspect
  myGridLineAspect.Aspect()->SetColor(myData.GridColor());
}

// =======================================================================
// method  : getNormal
// purpose : Normal of the view (not normalized!)
// =======================================================================
float OpenGl_GraduatedTrihedron::getNormal(const occ::handle<OpenGl_Context>& theContext,
                                           NCollection_Vec3<float>&           theNormal) const
{
  const int* aViewport = theContext->Viewport();

  NCollection_Mat4<float> aModelMatrix;
  NCollection_Mat4<float> aProjMatrix;
  aModelMatrix.Convert(theContext->ModelWorldState.Current()
                       * theContext->WorldViewState.Current());
  aProjMatrix.Convert(theContext->ProjectionState.Current());

  NCollection_Vec3<float> aPoint1, aPoint2, aPoint3;
  Graphic3d_TransformUtils::UnProject<float>((float)aViewport[0],
                                             (float)aViewport[1],
                                             0.0f,
                                             aModelMatrix,
                                             aProjMatrix,
                                             aViewport,
                                             aPoint1.x(),
                                             aPoint1.y(),
                                             aPoint1.z());

  Graphic3d_TransformUtils::UnProject<float>((float)(aViewport[0] + aViewport[2]),
                                             (float)aViewport[1],
                                             0.0f,
                                             aModelMatrix,
                                             aProjMatrix,
                                             aViewport,
                                             aPoint2.x(),
                                             aPoint2.y(),
                                             aPoint2.z());

  Graphic3d_TransformUtils::UnProject<float>((float)aViewport[0],
                                             (float)(aViewport[1] + aViewport[3]),
                                             0.0f,
                                             aModelMatrix,
                                             aProjMatrix,
                                             aViewport,
                                             aPoint3.x(),
                                             aPoint3.y(),
                                             aPoint3.z());

  const NCollection_Vec3<float> aD1 = aPoint3 - aPoint1;
  const NCollection_Vec3<float> aD2 = aPoint2 - aPoint1;
  theNormal                         = NCollection_Vec3<float>::Cross(aD1, aD2);

  // Distance corresponding to 1 pixel
  return aD2.Modulus() / (float)aViewport[2];
}

//=================================================================================================

float OpenGl_GraduatedTrihedron::getDistanceToCorner(const NCollection_Vec3<float>& theNormal,
                                                     const NCollection_Vec3<float>& theCenter,
                                                     const float                    theX,
                                                     const float                    theY,
                                                     const float                    theZ) const
{
  return theNormal.x() * (theX - theCenter.x()) + theNormal.y() * (theY - theCenter.y())
         + theNormal.z() * (theZ - theCenter.z());
}

//=================================================================================================

char16_t OpenGl_GraduatedTrihedron::getGridAxes(const float theCorners[8],
                                                GridAxes&   theGridAxes) const
{
  // Find the farthest corner
  uint8_t aMaxIndex = 0;
  float   aMax      = theCorners[aMaxIndex] > 0.0f ? theCorners[aMaxIndex] : 0.0f;

  for (uint8_t anIt = 0; anIt < 8; ++anIt)
  {
    if (theCorners[anIt] > aMax)
    {
      aMax      = theCorners[anIt];
      aMaxIndex = anIt;
    }
  }

  switch (aMaxIndex)
  {
    case 0: // (0,0,0)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0.0f, 1.0f, 0.0f);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0.0f, 0.0f, 1.0f);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMax.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());

      return OOZ_XOZ | OYO_XYO | XOO_XYO | OOZ_OYZ | XOO_XOZ | OYO_OYZ;
    }
    case 1: // (0,0,1)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMax.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0.0f, 1.0f, 0.0f);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0.0f, 0.0f, -1.0f);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMax.z());

      return OOZ_XOZ | OYZ_XYZ | OOZ_OYZ | XOZ_XYZ | XOO_XOZ | OYO_OYZ;
    }
    case 2: // (0,1,0)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMin.x(), myMax.y(), myMin.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0.0f, -1.0f, 0.0f);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0.0f, 0.0f, 1.0f);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMax.x(), myMax.y(), myMin.z());

      return OYO_XYO | OYZ_XYZ | XOO_XYO | OOZ_OYZ | XYO_XYZ | OYO_OYZ;
    }
    case 3: // (0,1,1)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMin.x(), myMax.y(), myMax.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(1.0f, 0.0f, 0.0f);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0.0f, -1.0f, 0.0f);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0.0f, 0.0f, -1.0f);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMax.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMin.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMax.z());

      return OOZ_XOZ | OYZ_XYZ | OYO_XYO | OOZ_OYZ | XOZ_XYZ | OYO_OYZ | XYO_XYZ;
    }
    case 4: // (1,0,0)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(-1, 0, 0);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0, 1, 0);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0, 0, 1);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMax.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMin.z());

      return OOZ_XOZ | OYO_XYO | XOO_XYO | XOZ_XYZ | XOO_XOZ | XYO_XYZ;
    }
    case 5: // (1,0,1)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMax.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(-1, 0, 0);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0, 1, 0);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0, 0, -1);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMin.x(), myMin.y(), myMax.z());

      return OOZ_XOZ | OYZ_XYZ | XOO_XYO | XOZ_XYZ | OOZ_OYZ | XOO_XOZ | XYO_XYZ;
    }
    case 6: // (1,1,0)
    {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(-1, 0, 0);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0, -1, 0);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0, 0, 1);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMin.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMin.z());

      return OYO_XYO | OYZ_XYZ | XOO_XYO | XOZ_XYZ | XOO_XOZ | XYO_XYZ | OYO_OYZ;
    }
    case 7: // (1,1,1)
    default: {
      theGridAxes.Origin  = NCollection_Vec3<float>(myMax.x(), myMax.y(), myMax.z());
      theGridAxes.Axes[0] = NCollection_Vec3<float>(-1, 0, 0);
      theGridAxes.Axes[1] = NCollection_Vec3<float>(0, -1, 0);
      theGridAxes.Axes[2] = NCollection_Vec3<float>(0, 0, -1);

      theGridAxes.Ticks[0] = NCollection_Vec3<float>(myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[1] = NCollection_Vec3<float>(myMax.x(), myMax.y(), myMin.z());
      theGridAxes.Ticks[2] = NCollection_Vec3<float>(myMax.x(), myMin.y(), myMax.z());

      return OYO_XYO | OYZ_XYZ | OOZ_XOZ | XOO_XYO | XOZ_XYZ | OOZ_OYZ | XOO_XOZ | XYO_XYZ
             | OYO_OYZ;
    }
  }
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::renderLine(const OpenGl_PrimitiveArray&         theLine,
                                           const occ::handle<OpenGl_Workspace>& theWorkspace,
                                           const NCollection_Mat4<float>&       theMat,
                                           const float                          theXt,
                                           const float                          theYt,
                                           const float                          theZt) const
{
  const occ::handle<OpenGl_Context>& aContext = theWorkspace->GetGlContext();
  NCollection_Mat4<float>            aMat(theMat);
  Graphic3d_TransformUtils::Translate(aMat, theXt, theYt, theZt);
  aContext->WorldViewState.SetCurrent(aMat);
  aContext->ApplyWorldViewMatrix();
  theLine.Render(theWorkspace);
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::renderGridPlane(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                                const int&                           theIndex,
                                                const GridAxes&                      theGridAxes,
                                                NCollection_Mat4<float>&             theMat) const
{
  const Graphic3d_GraduatedTrihedron::AxisAspect& aCurAspect = myData.AxisAspectAt(theIndex);
  if (aCurAspect.TickmarksNumber() <= 0)
  {
    return;
  }

  const occ::handle<OpenGl_Context>& aContext = theWorkspace->GetGlContext();

  float aStep = theGridAxes.Axes[theIndex].GetData()[theIndex]
                * (myMax.GetData()[theIndex] - myMin.GetData()[theIndex])
                / aCurAspect.TickmarksNumber();

  // NOTE:
  // Get two other axes directions and draw lines Axis.TickmarksNumber times.
  // Combining together from three axes, these lines will make a grid.
  for (int anIter = 1; anIter <= 2; ++anIter)
  {
    NCollection_Mat4<float> aMat(theMat);
    const int               anIndex = (theIndex + anIter) % 3;
    const Axis&             anAxis  = myAxes[anIndex];
    NCollection_Vec3<float> aStart(theGridAxes.Origin);
    if (theGridAxes.Axes[anIndex].GetData()[anIndex] < 0.0)
    {
      aStart.ChangeData()[anIndex] = myMin.GetData()[anIndex];
    }

    Graphic3d_TransformUtils::Translate(aMat, aStart.x(), aStart.y(), aStart.z());
    aContext->WorldViewState.SetCurrent(aMat);
    aContext->ApplyWorldViewMatrix();

    const NCollection_Vec3<float> aStepVec(myAxes[theIndex].Direction * aStep);
    for (int anIt = myData.ToDrawAxes() ? 1 : 0; anIt < aCurAspect.TickmarksNumber(); ++anIt)
    {
      Graphic3d_TransformUtils::Translate(aMat, aStepVec.x(), aStepVec.y(), aStepVec.z());
      aContext->WorldViewState.SetCurrent(aMat);
      aContext->ApplyWorldViewMatrix();
      anAxis.Line.Render(theWorkspace);
    }
  }
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::renderAxis(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                           const int&                           theIndex,
                                           const NCollection_Mat4<float>&       theMat) const
{
  const Axis& anAxis = myAxes[theIndex];

  theWorkspace->SetAspects(&anAxis.LineAspect);
  const occ::handle<OpenGl_Context>& aContext = theWorkspace->GetGlContext();

  // Reset transformations
  aContext->WorldViewState.SetCurrent(theMat);
  aContext->ApplyWorldViewMatrix();

  // Render arrow
  NCollection_Vec3<float> anArrowVec = myMin + anAxis.Direction * (myMax - myMin);

  Graphic3d_TransformPers aTransMode(
    Graphic3d_TMF_ZoomPers,
    gp_Pnt(double(anArrowVec.x()), double(anArrowVec.y()), double(anArrowVec.z())));
  const NCollection_Mat4<float>& aProjection = aContext->ProjectionState.Current();
  const NCollection_Mat4<float>& aWorldView  = aContext->WorldViewState.Current();
  const int                      aWidth      = theWorkspace->Width();
  const int                      aHeight     = theWorkspace->Height();

  // Take into account Transform Persistence
  aContext->ModelWorldState.SetCurrent(
    aTransMode.Compute(aContext->Camera(), aProjection, aWorldView, aWidth, aHeight));
  aContext->ApplyModelViewMatrix();

  anAxis.Arrow.Render(theWorkspace);

  // Get current Model-View and Projection states
  NCollection_Mat4<float> aModelMat;
  NCollection_Mat4<float> aProjMat;
  aModelMat.Convert(aContext->WorldViewState.Current() * aContext->ModelWorldState.Current());
  aProjMat.Convert(aContext->ProjectionState.Current());

  // Get the window's (fixed) coordinates for before matrices modifications
  NCollection_Vec3<float> aEndPoint = -anAxis.Direction * myData.ArrowsLength();
  NCollection_Vec3<float> aWinPoint;
  Graphic3d_TransformUtils::Project<float>(aEndPoint.x(),
                                           aEndPoint.y(),
                                           aEndPoint.z(),
                                           aModelMat,
                                           aProjMat,
                                           aContext->Viewport(),
                                           aWinPoint.x(),
                                           aWinPoint.y(),
                                           aWinPoint.z());

  aContext->ModelWorldState.SetIdentity();
  aModelMat.Convert(aContext->WorldViewState.Current());
  aProjMat.Convert(aContext->ProjectionState.Current());

  // Get start point of zoom persistent arrow
  NCollection_Vec3<float> anArrowStart;
  Graphic3d_TransformUtils::UnProject<float>(aWinPoint.x(),
                                             aWinPoint.y(),
                                             aWinPoint.z(),
                                             aModelMat,
                                             aProjMat,
                                             aContext->Viewport(),
                                             anArrowStart.x(),
                                             anArrowStart.y(),
                                             anArrowStart.z());
  // Render axis line
  aModelMat = theMat;
  Graphic3d_TransformUtils::Translate(aModelMat, myMin.x(), myMin.y(), myMin.z());

  float aScaleFactor = ((anArrowStart - myMin) * anAxis.Direction).Modulus()
                       / (anAxis.Direction * (myMax - myMin)).Modulus();
  NCollection_Vec3<float> aScaleAxes = anAxis.Direction * aScaleFactor;
  Graphic3d_TransformUtils::Scale(aModelMat, aScaleAxes.x(), aScaleAxes.y(), aScaleAxes.z());

  aContext->WorldViewState.SetCurrent(aModelMat);
  aContext->ApplyWorldViewMatrix();
  anAxis.Line.Render(theWorkspace);
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::renderTickmarkLabels(
  const occ::handle<OpenGl_Workspace>& theWorkspace,
  const NCollection_Mat4<float>&       theMat,
  const int                            theIndex,
  const GridAxes&                      theGridAxes,
  const float                          theDpix) const
{
  const Graphic3d_GraduatedTrihedron::AxisAspect& aCurAspect = myData.AxisAspectAt(theIndex);
  if (!aCurAspect.ToDrawName() && !aCurAspect.ToDrawValues())
  {
    return;
  }

  char                          aTextValue[128];
  const Axis&                   anAxis = myAxes[theIndex];
  const NCollection_Vec3<float> aSizeVec(myMax - myMin);
  float                         aStep = theGridAxes.Axes[theIndex].GetData()[theIndex]
                * (myMax.GetData()[theIndex] - myMin.GetData()[theIndex])
                / aCurAspect.TickmarksNumber();

  NCollection_Vec3<float> aDir = (theGridAxes.Ticks[theIndex] - theGridAxes.Origin).Normalized();
  const occ::handle<OpenGl_Context>& aContext = theWorkspace->GetGlContext();

  if (aCurAspect.ToDrawTickmarks() && aCurAspect.TickmarksNumber() > 0)
  {
    theWorkspace->SetAspects(&myGridLineAspect);

    NCollection_Mat4<float> aModelMat(theMat);

    anAxis.InitTickmark(aContext, aDir * (float)aCurAspect.TickmarksLength() * theDpix);
    Graphic3d_TransformUtils::Translate(aModelMat,
                                        theGridAxes.Ticks[theIndex].x(),
                                        theGridAxes.Ticks[theIndex].y(),
                                        theGridAxes.Ticks[theIndex].z());
    aContext->WorldViewState.SetCurrent(aModelMat);
    aContext->ApplyWorldViewMatrix();
    NCollection_Vec3<float> aStepVec = anAxis.Direction * aStep;
    for (int anIter = 0; anIter <= aCurAspect.TickmarksNumber(); ++anIter)
    {
      anAxis.Tickmark.Render(theWorkspace);
      Graphic3d_TransformUtils::Translate(aModelMat, aStepVec.x(), aStepVec.y(), aStepVec.z());
      aContext->WorldViewState.SetCurrent(aModelMat);
      aContext->ApplyWorldViewMatrix();
    }
  }

  // Restore matrix
  aContext->WorldViewState.SetCurrent(theMat);
  aContext->ApplyWorldViewMatrix();

  if (aCurAspect.ToDrawName())
  {
    double anOffset = aCurAspect.NameOffset() + aCurAspect.TickmarksLength();

    NCollection_Vec3<float> aMiddle(theGridAxes.Ticks[theIndex]
                                    + aSizeVec * theGridAxes.Axes[theIndex] * 0.5f
                                    + aDir * (float)(theDpix * anOffset));

    myAspectLabels.Aspect()->SetColor(anAxis.NameColor);
    theWorkspace->SetAspects(&myAspectLabels);
    anAxis.Label.Text()->SetPosition(gp_Pnt(aMiddle.x(), aMiddle.y(), aMiddle.z()));
    anAxis.Label.Render(theWorkspace);
  }

  if (aCurAspect.ToDrawValues() && aCurAspect.TickmarksNumber() > 0)
  {
    myAspectValues.Aspect()->SetColor(anAxis.LineAspect.Aspect()->Color());
    theWorkspace->SetAspects(&myAspectValues);
    double anOffset = aCurAspect.ValuesOffset() + aCurAspect.TickmarksLength();

    for (int anIt = 0; anIt <= aCurAspect.TickmarksNumber(); ++anIt)
    {
      Sprintf(aTextValue, "%g", theGridAxes.Ticks[theIndex].GetData()[theIndex] + anIt * aStep);
      NCollection_Vec3<float> aPos(theGridAxes.Ticks[theIndex]
                                   + anAxis.Direction * (float)(anIt * aStep)
                                   + aDir * (float)(theDpix * anOffset));

      occ::handle<Graphic3d_Text> aText = myLabelValues.Text();
      aText->SetText(aTextValue);
      aText->SetPosition(gp_Pnt(aPos.x(), aPos.y(), aPos.z()));

      myLabelValues.Reset(theWorkspace->GetGlContext());
      myLabelValues.Render(theWorkspace);
    }
  }
}

// =======================================================================
// function : Render
// purpose  : call_graduatedtrihedron_redraw
// =======================================================================
void OpenGl_GraduatedTrihedron::Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
{
  const occ::handle<OpenGl_Context>& aContext = theWorkspace->GetGlContext();
  if (!myIsInitialized)
  {
    initGlResources(theWorkspace->GetGlContext());
    myIsInitialized = true;
  }

  // Update boundary box
  NCollection_Vec3<float> anOldMin = myMin;
  NCollection_Vec3<float> anOldMax = myMax;

  if (myData.CubicAxesCallback(theWorkspace->View()))
  {
    if (!myAxes[0].Line.IsInitialized() || !myAxes[1].Line.IsInitialized()
        || !myAxes[2].Line.IsInitialized()
        || NCollection_Vec3<float>(anOldMin - myMin).Modulus() > Precision::Confusion()
        || NCollection_Vec3<float>(anOldMax - myMax).Modulus() > Precision::Confusion())
    {
      myAxes[0].InitLine(aContext, NCollection_Vec3<float>(myMax.x() - myMin.x(), 0.0f, 0.0f));
      myAxes[1].InitLine(aContext, NCollection_Vec3<float>(0.0f, myMax.y() - myMin.y(), 0.0f));
      myAxes[2].InitLine(aContext, NCollection_Vec3<float>(0.0f, 0.0f, myMax.z() - myMin.z()));
    }
  }

  // Find the farthest point of bounding box

  // Get normal of the view out of user and distance corresponding to 1 pixel
  NCollection_Vec3<float> aNormal;
  float                   aDpix = getNormal(aContext, aNormal);
  aNormal.Normalize();

  // Get central point of bounding box
  NCollection_Vec3<float> aCenter;
  aCenter = (myMin + myMax) * 0.5f;

  // Check distance to corners of bounding box along the normal
  float aCorners[8];
  aCorners[0] = getDistanceToCorner(aNormal, aCenter, myMin.x(), myMin.y(), myMin.z());
  aCorners[1] = getDistanceToCorner(aNormal, aCenter, myMin.x(), myMin.y(), myMax.z());
  aCorners[2] = getDistanceToCorner(aNormal, aCenter, myMin.x(), myMax.y(), myMin.z());
  aCorners[3] = getDistanceToCorner(aNormal, aCenter, myMin.x(), myMax.y(), myMax.z());
  aCorners[4] = getDistanceToCorner(aNormal, aCenter, myMax.x(), myMin.y(), myMin.z());
  aCorners[5] = getDistanceToCorner(aNormal, aCenter, myMax.x(), myMin.y(), myMax.z());
  aCorners[6] = getDistanceToCorner(aNormal, aCenter, myMax.x(), myMax.y(), myMin.z());
  aCorners[7] = getDistanceToCorner(aNormal, aCenter, myMax.x(), myMax.y(), myMax.z());

  // NOTE:
  // (0, 0, 1), (0, 1, 0) and (0, 0, 1) directions from (myMin.x(), Ymin, Zmin) point
  // are reserved for trihedron axes.
  // So for the grid here are 9 edges of cube,
  // and, depending on the farthest point, 2 or 3 of them may not be drawn
  // if they overlap displayed model.

  // Write an axes state what axes of bounding box are to be drawn
  GridAxes aGridAxes;
  char16_t anAxesState = getGridAxes(aCorners, aGridAxes);

  // Remember current aspects
  const OpenGl_Aspects* anOldAspectLine = theWorkspace->Aspects();

  NCollection_Mat4<float> aModelMatrix;
  aModelMatrix.Convert(aContext->WorldViewState.Current());

  // Remember model-view matrix
  aContext->WorldViewState.Push();
  aContext->WorldViewState.SetCurrent(aModelMatrix);
  aContext->ApplyWorldViewMatrix();

  if (myData.ToDrawGrid())
  {
    theWorkspace->SetAspects(&myGridLineAspect);

    // render grid edges
    if (anAxesState & XOO_XYO)
    {
      renderLine(myAxes[1].Line, theWorkspace, aModelMatrix, myMax.x(), myMin.y(), myMin.z());
    }

    if (anAxesState & XOO_XOZ)
    {
      renderLine(myAxes[2].Line, theWorkspace, aModelMatrix, myMax.x(), myMin.y(), myMin.z());
    }

    if (anAxesState & OYO_OYZ)
    {
      renderLine(myAxes[2].Line, theWorkspace, aModelMatrix, myMin.x(), myMax.y(), myMin.z());
    }

    if (anAxesState & OYO_XYO)
    {
      renderLine(myAxes[0].Line, theWorkspace, aModelMatrix, myMin.x(), myMax.y(), myMin.z());
    }

    if (anAxesState & OOZ_XOZ)
    {
      renderLine(myAxes[0].Line, theWorkspace, aModelMatrix, myMin.x(), myMin.y(), myMax.z());
    }

    if (anAxesState & OOZ_OYZ)
    {
      renderLine(myAxes[1].Line, theWorkspace, aModelMatrix, myMin.x(), myMin.y(), myMax.z());
    }

    if (anAxesState & OYZ_XYZ)
    {
      renderLine(myAxes[0].Line, theWorkspace, aModelMatrix, myMin.x(), myMax.y(), myMax.z());
    }

    if (anAxesState & XOZ_XYZ)
    {
      renderLine(myAxes[1].Line, theWorkspace, aModelMatrix, myMax.x(), myMin.y(), myMax.z());
    }

    if (anAxesState & XYO_XYZ)
    {
      renderLine(myAxes[2].Line, theWorkspace, aModelMatrix, myMax.x(), myMax.y(), myMin.z());
    }

    for (int anIter = 0; anIter < 3; ++anIter)
    {
      renderGridPlane(theWorkspace, anIter, aGridAxes, aModelMatrix);
    }
  }

  // Axes (arrows)
  if (myData.ToDrawAxes())
  {
    for (int anIter = 0; anIter < 3; ++anIter)
    {
      renderAxis(theWorkspace, anIter, aModelMatrix);
    }
  }

  // Names of axes & values
  for (int anIter = 0; anIter < 3; ++anIter)
  {
    // Restore current matrix
    aContext->WorldViewState.SetCurrent(aModelMatrix);
    aContext->ApplyWorldViewMatrix();
    renderTickmarkLabels(theWorkspace, aModelMatrix, anIter, aGridAxes, aDpix);
  }

  theWorkspace->SetAspects(anOldAspectLine);

  aContext->WorldViewState.Pop();
  aContext->ApplyWorldViewMatrix();
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::SetMinMax(const NCollection_Vec3<float>& theMin,
                                          const NCollection_Vec3<float>& theMax)
{
  myMin = theMin;
  myMax = theMax;
}

//=================================================================================================

OpenGl_GraduatedTrihedron::Axis::Axis(const Graphic3d_GraduatedTrihedron::AxisAspect& theAspect,
                                      const NCollection_Vec3<float>&                  theDirection)
    : Direction(theDirection),
      Tickmark(nullptr),
      Line(nullptr),
      Arrow(nullptr)
{
  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text(THE_LABEL_HEIGHT);
  aText->SetText((char16_t*)theAspect.Name().ToExtString());
  aText->SetPosition(gp_Pnt(theDirection.x(), theDirection.y(), theDirection.z()));
  aText->SetHorizontalAlignment(THE_LABEL_HALIGH);
  aText->SetVerticalAlignment(THE_LABEL_VALIGH);
  Label     = OpenGl_Text(aText);
  NameColor = theAspect.NameColor();
  LineAspect.Aspect()->SetColor(theAspect.Color());
}

// =======================================================================
// method  : OpenGl_GraduatedTrihedron::Axis::~Axis
// purpose :
// =======================================================================
OpenGl_GraduatedTrihedron::Axis::~Axis() = default;

// =======================================================================
// method  : OpenGl_GraduatedTrihedron::Axis operator=
// purpose :
// =======================================================================
OpenGl_GraduatedTrihedron::Axis& OpenGl_GraduatedTrihedron::Axis::operator=(const Axis& theOther)
{
  Direction  = theOther.Direction;
  NameColor  = theOther.NameColor;
  LineAspect = theOther.LineAspect;
  Label      = theOther.Label;

  Line.InitBuffers(nullptr,
                   Graphic3d_TOPA_SEGMENTS,
                   theOther.Line.Indices(),
                   theOther.Line.Attributes(),
                   theOther.Line.Bounds());
  Tickmark.InitBuffers(nullptr,
                       Graphic3d_TOPA_SEGMENTS,
                       theOther.Tickmark.Indices(),
                       theOther.Tickmark.Attributes(),
                       theOther.Tickmark.Bounds());
  Arrow.InitBuffers(nullptr,
                    Graphic3d_TOPA_POLYLINES,
                    theOther.Arrow.Indices(),
                    theOther.Arrow.Attributes(),
                    theOther.Arrow.Bounds());
  return *this;
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::Axis::InitArrow(const occ::handle<OpenGl_Context>& theContext,
                                                const float                        theLength,
                                                const NCollection_Vec3<float>&     theNormal) const
{
  // Draw from the end point of the aris
  NCollection_Vec3<float> aLengthVec = -Direction * theLength;

  // Radial direction to the arrow
  NCollection_Vec3<float> aRadial = NCollection_Vec3<float>::Cross(this->Direction, theNormal);
  if (aRadial.Modulus() < (float)Precision::Confusion())
  {
    return;
  }
  aRadial = aRadial.Normalized() * theLength * 0.2f;

  // Initialize arrow primitive array
  // Make loop from polyline
  const NCollection_Vec3<float> aPoint1 = aRadial + aLengthVec;
  const NCollection_Vec3<float> aPoint2(0.0f, 0.0f, 0.0f);
  const NCollection_Vec3<float> aPoint3 = -aRadial + aLengthVec;

  occ::handle<Graphic3d_ArrayOfPolylines> anArray = new Graphic3d_ArrayOfPolylines(4);
  anArray->AddVertex(aPoint1);
  anArray->AddVertex(aPoint2);
  anArray->AddVertex(aPoint3);
  anArray->AddVertex(aPoint1);

  Arrow.InitBuffers(theContext,
                    Graphic3d_TOPA_POLYLINES,
                    anArray->Indices(),
                    anArray->Attributes(),
                    anArray->Bounds());
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::Axis::InitTickmark(const occ::handle<OpenGl_Context>& theContext,
                                                   const NCollection_Vec3<float>&     theDir) const
{

  occ::handle<Graphic3d_ArrayOfSegments> anArray = new Graphic3d_ArrayOfSegments(2);
  anArray->AddVertex(0.0f, 0.0f, 0.0f);
  anArray->AddVertex(theDir);
  Tickmark.InitBuffers(theContext,
                       Graphic3d_TOPA_SEGMENTS,
                       anArray->Indices(),
                       anArray->Attributes(),
                       anArray->Bounds());
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::Axis::InitLine(const occ::handle<OpenGl_Context>& theContext,
                                               const NCollection_Vec3<float>&     theDir) const
{

  occ::handle<Graphic3d_ArrayOfSegments> anArray = new Graphic3d_ArrayOfSegments(2);
  anArray->AddVertex(0.0f, 0.0f, 0.0f);
  anArray->AddVertex(theDir);

  Line.InitBuffers(theContext,
                   Graphic3d_TOPA_SEGMENTS,
                   anArray->Indices(),
                   anArray->Attributes(),
                   anArray->Bounds());
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::Axis::Release(OpenGl_Context* theCtx)
{
  Label.Release(theCtx);
  Tickmark.Release(theCtx);
  Line.Release(theCtx);
  Arrow.Release(theCtx);
}

//=================================================================================================

void OpenGl_GraduatedTrihedron::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, OpenGl_GraduatedTrihedron)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, OpenGl_Element)
}
