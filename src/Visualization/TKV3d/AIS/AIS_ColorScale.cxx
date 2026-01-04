// Created on: 2015-02-03
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <AIS_ColorScale.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Text.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <StdPrs_Curve.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ColorScale, AIS_InteractiveObject)

namespace
{
//! Method to add colored quad into array of triangles.
static void addColoredQuad(const occ::handle<Graphic3d_ArrayOfTriangles>& theTris,
                           const int                                      theXLeft,
                           const int                                      theYBottom,
                           const int                                      theSizeX,
                           const int                                      theSizeY,
                           const Quantity_Color&                          theColorBottom,
                           const Quantity_Color&                          theColorTop)
{
  const int aVertIndex = theTris->VertexNumber() + 1;
  theTris->AddVertex(gp_Pnt(theXLeft, theYBottom, 0.0), theColorBottom);
  theTris->AddVertex(gp_Pnt(theXLeft + theSizeX, theYBottom, 0.0), theColorBottom);
  theTris->AddVertex(gp_Pnt(theXLeft, theYBottom + theSizeY, 0.0), theColorTop);
  theTris->AddVertex(gp_Pnt(theXLeft + theSizeX, theYBottom + theSizeY, 0.0), theColorTop);
  theTris->AddEdges(aVertIndex, aVertIndex + 1, aVertIndex + 2);
  theTris->AddEdges(aVertIndex + 1, aVertIndex + 2, aVertIndex + 3);
}

//! Compute hue angle from specified value.
static Quantity_Color colorFromValueEx(const double                    theValue,
                                       const double                    theMin,
                                       const double                    theMax,
                                       const NCollection_Vec3<double>& theHlsMin,
                                       const NCollection_Vec3<double>& theHlsMax)
{
  const double aValueDelta = theMax - theMin;
  double       aValue      = 0.0;
  if (aValueDelta != 0.0)
  {
    aValue = (theValue - theMin) / aValueDelta;
  }

  double aHue        = NCollection_Lerp<double>::Interpolate(theHlsMin[0], theHlsMax[0], aValue);
  double aLightness  = NCollection_Lerp<double>::Interpolate(theHlsMin[1], theHlsMax[1], aValue);
  double aSaturation = NCollection_Lerp<double>::Interpolate(theHlsMin[2], theHlsMax[2], aValue);
  return Quantity_Color(AIS_ColorScale::hueToValidRange(aHue),
                        aLightness,
                        aSaturation,
                        Quantity_TOC_HLS);
}

//! Return the index of discrete interval for specified value.
//! Note that when value lies exactly on the border between two intervals,
//! determining which interval to return is undefined operation;
//! Current implementation returns the following interval in this case.
//! @param theValue [in] value to map
//! @param theMin   [in] values range, lower value
//! @param theMax   [in] values range, upper value
//! @param theNbIntervals [in] number of discrete intervals
//! @return index of interval within [1, theNbIntervals] range
static int colorDiscreteInterval(double theValue, double theMin, double theMax, int theNbIntervals)
{
  if (std::abs(theMax - theMin) <= Precision::Approximation())
  {
    return 1;
  }

  int anInterval =
    1 + (int)std::floor(double(theNbIntervals) * (theValue - theMin) / (theMax - theMin));
  // map the very upper value (theValue==theMax) to the largest color interval
  anInterval = std::min(anInterval, theNbIntervals);
  return anInterval;
}
} // namespace

//=================================================================================================

AIS_ColorScale::AIS_ColorScale()
    : myMin(0.0),
      myMax(1.0),
      myColorHlsMin(230.0, 1.0, 1.0),
      myColorHlsMax(0.0, 1.0, 1.0),
      myFormat("%.4g"),
      myNbIntervals(10),
      myColorType(Aspect_TOCSD_AUTO),
      myLabelType(Aspect_TOCSD_AUTO),
      myIsLabelAtBorder(true),
      myIsReversed(false),
      myIsLogarithmic(false),
      myIsSmooth(false),
      myLabelPos(Aspect_TOCSP_RIGHT),
      myTitlePos(Aspect_TOCSP_LEFT),
      myXPos(0),
      myYPos(0),
      myBreadth(0),
      myHeight(0),
      mySpacing(5),
      myTextHeight(20)
{
  SetDisplayMode(0);
  myDrawer->SetupOwnShadingAspect();
  myDrawer->ShadingAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  myDrawer->ShadingAspect()->Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Opaque);
  myDrawer->ShadingAspect()->Aspect()->SetInteriorColor(Quantity_NOC_WHITE);
}

//=================================================================================================

TCollection_ExtendedString AIS_ColorScale::GetLabel(const int theIndex) const
{
  if (myLabelType == Aspect_TOCSD_USER)
  {
    if (theIndex >= NCollection_Sequence<TCollection_ExtendedString>::Lower()
        || theIndex <= myLabels.Upper())
    {
      return myLabels.Value(theIndex);
    }
    return TCollection_ExtendedString();
  }

  // value to be shown depends on label position
  const double aVal = myIsLabelAtBorder
                        ? GetIntervalValue(theIndex - 1)
                        : (0.5 * (GetIntervalValue(theIndex - 1) + GetIntervalValue(theIndex)));

  char aBuf[1024];
  Sprintf(aBuf, myFormat.ToCString(), aVal);
  return TCollection_ExtendedString(aBuf);
}

//=================================================================================================

Quantity_Color AIS_ColorScale::GetIntervalColor(const int theIndex) const
{
  if (myColorType == Aspect_TOCSD_USER)
  {
    if (theIndex <= 0 || theIndex > myColors.Length())
    {
      return Quantity_Color();
    }
    return myColors.Value(theIndex);
  }

  return colorFromValue(theIndex - 1, 0, myNbIntervals - 1);
}

//=================================================================================================

void AIS_ColorScale::GetLabels(NCollection_Sequence<TCollection_ExtendedString>& theLabels) const
{
  theLabels.Clear();
  for (NCollection_Sequence<TCollection_ExtendedString>::Iterator aLabIter(myLabels);
       aLabIter.More();
       aLabIter.Next())
  {
    theLabels.Append(aLabIter.Value());
  }
}

//=================================================================================================

void AIS_ColorScale::GetColors(NCollection_Sequence<Quantity_Color>& theColors) const
{
  theColors.Clear();
  for (NCollection_Sequence<Quantity_Color>::Iterator aColorIter(myColors); aColorIter.More();
       aColorIter.Next())
  {
    theColors.Append(aColorIter.Value());
  }
}

//=================================================================================================

void AIS_ColorScale::SetRange(const double theMin, const double theMax)
{
  myMin = std::min(theMin, theMax);
  myMax = std::max(theMin, theMax);
}

//=================================================================================================

void AIS_ColorScale::SetNumberOfIntervals(const int theNum)
{
  if (theNum < 1)
  {
    return;
  }

  myNbIntervals = theNum;
}

//=================================================================================================

void AIS_ColorScale::SetLabel(const TCollection_ExtendedString& theLabel, const int theIndex)
{
  const int aLabIndex = (theIndex <= 0 ? myLabels.Length() + 1 : theIndex);
  while (myLabels.Length() < aLabIndex)
  {
    myLabels.Append(TCollection_ExtendedString());
  }
  myLabels.SetValue(aLabIndex, theLabel);
}

//=================================================================================================

void AIS_ColorScale::SetIntervalColor(const Quantity_Color& theColor, const int theIndex)
{
  const int aColorIndex = (theIndex <= 0 ? myColors.Length() + 1 : theIndex);
  while (myColors.Length() < aColorIndex)
  {
    myColors.Append(Quantity_Color());
  }
  myColors.SetValue(aColorIndex, theColor);
}

//=================================================================================================

void AIS_ColorScale::SetLabels(const NCollection_Sequence<TCollection_ExtendedString>& theSeq)
{
  myLabels.Clear();
  for (NCollection_Sequence<TCollection_ExtendedString>::Iterator aLabIter(theSeq); aLabIter.More();
       aLabIter.Next())
  {
    myLabels.Append(aLabIter.Value());
  }
}

//=================================================================================================

void AIS_ColorScale::SetColors(const NCollection_Sequence<Quantity_Color>& theSeq)
{
  myColors.Clear();
  for (NCollection_Sequence<Quantity_Color>::Iterator aColorIter(theSeq); aColorIter.More();
       aColorIter.Next())
  {
    myColors.Append(aColorIter.Value());
  }
}

//=================================================================================================

NCollection_Sequence<Quantity_Color> AIS_ColorScale::MakeUniformColors(int    theNbColors,
                                                                       double theLightness,
                                                                       double theHueFrom,
                                                                       double theHueTo)
{
  NCollection_Sequence<Quantity_Color> aResult;

  // adjust range to be within (0, 360], with sign according to theHueFrom and theHueTo
  double           aHueRange = std::fmod(theHueTo - theHueFrom, 360.);
  constexpr double aHueEps   = Precision::Angular() * 180. / M_PI;
  if (std::abs(aHueRange) <= aHueEps)
  {
    aHueRange = (aHueRange < 0 ? -360. : 360.);
  }

  // treat limit cases
  if (theNbColors < 1)
  {
    return aResult;
  }
  if (theNbColors == 1)
  {
    double aHue = std::fmod(theHueFrom, 360.);
    if (aHue < 0.)
    {
      aHue += 360.;
    }
    Quantity_Color aColor(theLightness, 130., aHue, Quantity_TOC_CIELch);
    aResult.Append(aColor);
    return aResult;
  }

  // discretize the range with 1 degree step
  const int                          NBCOLORS = 2 + (int)std::abs(aHueRange / 1.);
  double                             aHueStep = aHueRange / (NBCOLORS - 1);
  NCollection_Array1<Quantity_Color> aGrid(0, NBCOLORS - 1);
  for (int i = 0; i < NBCOLORS; i++)
  {
    double aHue = std::fmod(theHueFrom + i * aHueStep, 360.);
    if (aHue < 0.)
    {
      aHue += 360.;
    }
    aGrid(i).SetValues(theLightness, 130., aHue, Quantity_TOC_CIELch);
  }

  // and compute distances between each two colors in a grid
  NCollection_Array1<double> aMetric(0, NBCOLORS - 1);
  double                     aLength = 0.;
  for (int i = 0, j = NBCOLORS - 1; i < NBCOLORS; j = i++)
  {
    aLength += (aMetric(i) = aGrid(i).DeltaE2000(aGrid(j)));
  }

  // determine desired step by distance;
  // normally we aim to distribute colors from start to end
  // of the range, but if distance between first and last points of the range
  // is less than that step (e.g. range is full 360 deg),
  // then distribute by the whole 360 deg scope to ensure that first
  // and last colors are sufficiently distanced
  double aDStep = (aLength - aMetric.First()) / (theNbColors - 1);
  if (aMetric.First() < aDStep)
  {
    aDStep = aLength / theNbColors;
  }

  // generate sequence
  aResult.Append(aGrid(0));
  double aParam = 0., aPrev = 0., aTarget = aDStep;
  for (int i = 1; i < NBCOLORS; i++)
  {
    aParam = aPrev + aMetric(i);
    while (aTarget <= aParam)
    {
      float          aCoefPrev = float((aParam - aTarget) / (aParam - aPrev));
      float          aCoefCurr = float((aTarget - aPrev) / (aParam - aPrev));
      Quantity_Color aColor(aGrid(i).Rgb() * aCoefCurr + aGrid(i - 1).Rgb() * aCoefPrev);
      aResult.Append(aColor);
      aTarget += aDStep;
    }
    aPrev = aParam;
  }
  if (aResult.Length() < theNbColors)
  {
    aResult.Append(aGrid.Last());
  }
  Standard_ASSERT_VOID(aResult.Length() == theNbColors,
                       "Failed to generate requested nb of colors");
  return aResult;
}

//=================================================================================================

void AIS_ColorScale::SizeHint(int& theWidth, int& theHeight) const
{
  const int aTextHeight = TextHeight("");
  const int aColorWidth = 20;
  int       aTextWidth  = 0;
  if (myLabelPos != Aspect_TOCSP_NONE)
  {
    for (int aLabIter = (myIsLabelAtBorder ? 0 : 1); aLabIter <= myNbIntervals; ++aLabIter)
    {
      aTextWidth = std::max(aTextWidth, TextWidth(GetLabel(aLabIter)));
    }
  }

  const int aScaleWidth  = aColorWidth + aTextWidth + (aTextWidth ? 3 : 2) * mySpacing;
  const int aScaleHeight = (int)(1.5 * (myNbIntervals + (myIsLabelAtBorder ? 2 : 1)) * aTextHeight);

  int aTitleWidth  = 0;
  int aTitleHeight = 0;
  if (!myTitle.IsEmpty())
  {
    aTitleHeight = TextHeight(myTitle) + mySpacing;
    aTitleWidth  = TextWidth(myTitle) + mySpacing * 2;
  }

  theWidth  = std::max(aTitleWidth, aScaleWidth);
  theHeight = aScaleHeight + aTitleHeight;
}

//=================================================================================================

double AIS_ColorScale::GetIntervalValue(const int theIndex) const
{
  if (myNbIntervals <= 0)
  {
    return 0.0;
  }

  if (IsLogarithmic())
  {
    double aMin     = myMin > 0 ? myMin : 1.0;
    double aDivisor = std::pow(myMax / aMin, 1.0 / myNbIntervals);
    return aMin * std::pow(aDivisor, theIndex);
  }

  double aNum = 0;
  if (myNbIntervals > 0)
  {
    aNum = GetMin() + theIndex * (std::abs(GetMax() - GetMin()) / myNbIntervals);
  }
  return aNum;
}

//=================================================================================================

Quantity_Color AIS_ColorScale::colorFromValue(const double theValue,
                                              const double theMin,
                                              const double theMax) const
{
  return colorFromValueEx(theValue, theMin, theMax, myColorHlsMin, myColorHlsMax);
}

//=================================================================================================

bool AIS_ColorScale::FindColor(const double theValue, Quantity_Color& theColor) const
{
  if (theValue < myMin || theValue > myMax || myMax < myMin)
  {
    theColor = Quantity_Color();
    return false;
  }

  if (myColorType == Aspect_TOCSD_USER)
  {
    const int anInterval = colorDiscreteInterval(theValue, myMin, myMax, myNbIntervals);
    if (anInterval < NCollection_Sequence<Quantity_Color>::Lower() || anInterval > myColors.Upper())
    {
      theColor = Quantity_Color();
      return false;
    }

    theColor = myColors.Value(anInterval);
    return true;
  }

  return FindColor(theValue, myMin, myMax, myNbIntervals, theColor);
}

//=================================================================================================

bool AIS_ColorScale::FindColor(const double                    theValue,
                               const double                    theMin,
                               const double                    theMax,
                               const int                       theColorsCount,
                               const NCollection_Vec3<double>& theColorHlsMin,
                               const NCollection_Vec3<double>& theColorHlsMax,
                               Quantity_Color&                 theColor)
{
  if (theValue < theMin || theValue > theMax || theMax < theMin)
  {
    return false;
  }

  const int anInterval = colorDiscreteInterval(theValue, theMin, theMax, theColorsCount);
  theColor =
    colorFromValueEx(anInterval - 1, 0, theColorsCount - 1, theColorHlsMin, theColorHlsMax);
  return true;
}

//=================================================================================================

int AIS_ColorScale::computeMaxLabelWidth(
  const NCollection_Sequence<TCollection_ExtendedString>& theLabels) const
{
  int aWidthMax = 0;
  for (NCollection_Sequence<TCollection_ExtendedString>::Iterator aLabIter(theLabels);
       aLabIter.More();
       aLabIter.Next())
  {
    if (!aLabIter.Value().IsEmpty())
    {
      aWidthMax = std::max(aWidthMax, TextWidth(aLabIter.Value()));
    }
  }
  return aWidthMax;
}

//=================================================================================================

void AIS_ColorScale::updateTextAspect()
{
  // update text aspect
  const Quantity_Color aFgColor(hasOwnColor ? myDrawer->Color() : Quantity_NOC_WHITE);
  if (!myDrawer->HasOwnTextAspect())
  {
    myDrawer->SetTextAspect(new Prs3d_TextAspect());
    *myDrawer->TextAspect()->Aspect() = *myDrawer->Link()->TextAspect()->Aspect();
  }

  const occ::handle<Prs3d_TextAspect>& anAspect = myDrawer->TextAspect();
  anAspect->SetColor(aFgColor);
  anAspect->SetHeight(myTextHeight);
  anAspect->SetHorizontalJustification(Graphic3d_HTA_LEFT);
  anAspect->SetVerticalJustification(Graphic3d_VTA_BOTTOM);
  anAspect->Aspect()->SetTextZoomable(true);
}

//=================================================================================================

void AIS_ColorScale::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                             const occ::handle<Prs3d_Presentation>& thePrs,
                             const int                              theMode)
{
  if (theMode != 0)
  {
    return;
  }

  // update text aspect
  updateTextAspect();

  const int aTitleOffset = !myTitle.IsEmpty() ? (myTextHeight + mySpacing) : 0;

  const int aBarYOffset = myTextHeight / 2 + 2 * mySpacing; // a half-label offset
  const int aBarBottom  = myYPos + aBarYOffset;
  const int aBarTop     = myYPos + myHeight - aTitleOffset - aBarYOffset;
  const int aBarHeight  = aBarTop - aBarBottom;

  NCollection_Sequence<TCollection_ExtendedString> aLabels;
  if (myLabelType == Aspect_TOCSD_USER)
  {
    aLabels = myLabels;
  }
  else
  {
    const int aNbLabels = myIsLabelAtBorder ? myNbIntervals + 1 : myNbIntervals;
    for (int aLabIter = 1; aLabIter <= aNbLabels; ++aLabIter)
    {
      if (myIsReversed)
      {
        aLabels.Prepend(GetLabel(aLabIter));
      }
      else
      {
        aLabels.Append(GetLabel(aLabIter));
      }
    }
  }

  const int aTextWidth    = myLabelPos != Aspect_TOCSP_NONE ? computeMaxLabelWidth(aLabels) : 0;
  int       aColorBreadth = std::max(5, std::min(20, myBreadth - aTextWidth - 3 * mySpacing));
  if (myLabelPos == Aspect_TOCSP_CENTER || myLabelPos == Aspect_TOCSP_NONE)
  {
    aColorBreadth += aTextWidth;
  }

  // draw title
  occ::handle<Graphic3d_Group> aLabelsGroup;
  if (!myTitle.IsEmpty() || !aLabels.IsEmpty())
  {
    aLabelsGroup = thePrs->NewGroup();
    aLabelsGroup->SetGroupPrimitivesAspect(myDrawer->TextAspect()->Aspect());
  }
  if (!myTitle.IsEmpty())
  {
    drawText(aLabelsGroup,
             myTitle,
             myXPos + mySpacing,
             aBarTop + aBarYOffset,
             Graphic3d_VTA_BOTTOM);
  }

  // draw colors
  drawColorBar(thePrs, aBarBottom, aBarHeight, aTextWidth, aColorBreadth);

  // draw Labels
  drawLabels(aLabelsGroup, aLabels, aBarBottom, aBarHeight, aTextWidth, aColorBreadth);
}

//=================================================================================================

void AIS_ColorScale::drawColorBar(const occ::handle<Prs3d_Presentation>& thePrs,
                                  const int                              theBarBottom,
                                  const int                              theBarHeight,
                                  const int                              theMaxLabelWidth,
                                  const int                              theColorBreadth)
{
  const double aStepY = double(theBarHeight) / double(myNbIntervals);
  if (aStepY <= 0.0)
  {
    return;
  }

  // Draw colors
  const int anXLeft =
    myLabelPos == Aspect_TOCSP_LEFT
      ? myXPos + mySpacing + theMaxLabelWidth + (theMaxLabelWidth != 0 ? 1 : 0) * mySpacing
      : myXPos + mySpacing;

  NCollection_Sequence<Quantity_Color> aColors;
  for (int anIntervalIter = 1; anIntervalIter <= myNbIntervals; ++anIntervalIter)
  {
    if (myIsReversed)
    {
      aColors.Prepend(GetIntervalColor(anIntervalIter));
    }
    else
    {
      aColors.Append(GetIntervalColor(anIntervalIter));
    }
  }

  occ::handle<Graphic3d_ArrayOfTriangles> aTriangles;
  if (myIsSmooth && myColorType == Aspect_TOCSD_USER)
  {
    // Smooth custom intervals, so that the color in the center of interval is equal to specified
    // one (thus the halves of first and last intervals have solid color)
    aTriangles =
      new Graphic3d_ArrayOfTriangles((aColors.Length() + 1) * 4,     // quads
                                     (aColors.Length() + 1) * 2 * 3, // quads as triangles
                                                                     // clang-format off
                                                 false, true);                   // per-vertex colors
                                                                     // clang-format on
    Quantity_Color aColor1(aColors.Value(1)), aColor2;
    int            aSizeY        = int(aStepY / 2);
    const int      anYBottom     = theBarBottom + aSizeY;
    int            anYBottomIter = anYBottom;
    addColoredQuad(aTriangles, anXLeft, theBarBottom, theColorBreadth, aSizeY, aColor1, aColor1);
    for (int aColorIter = 0; aColorIter < myNbIntervals - 1; ++aColorIter)
    {
      aColor1 = aColors.Value(aColorIter + 1);
      aColor2 = aColors.Value(aColorIter + 2);
      aSizeY  = anYBottom + int((aColorIter + 1) * aStepY) - anYBottomIter;
      addColoredQuad(aTriangles, anXLeft, anYBottomIter, theColorBreadth, aSizeY, aColor1, aColor2);
      anYBottomIter += aSizeY;
    }
    aColor2 = aColors.Value(myNbIntervals);
    aSizeY  = theBarBottom + theBarHeight - anYBottomIter;
    addColoredQuad(aTriangles, anXLeft, anYBottomIter, theColorBreadth, aSizeY, aColor2, aColor2);
  }
  else if (myIsSmooth)
  {
    // smooth transition between standard colors - without solid color regions at the beginning and
    // end of full color range
    const Quantity_Color aColorsFixed[5] = {colorFromValue(0, 0, 4),
                                            colorFromValue(1, 0, 4),
                                            colorFromValue(2, 0, 4),
                                            colorFromValue(3, 0, 4),
                                            colorFromValue(4, 0, 4)};
    aTriangles                           = new Graphic3d_ArrayOfTriangles(4 * 4, // quads
                                                4 * 2 * 3, // quads as triangles
                                                false,
                                                true); // per-vertex colors
    int anYBottomIter                    = theBarBottom;
    addColoredQuad(aTriangles,
                   anXLeft,
                   theBarBottom,
                   theColorBreadth,
                   theBarHeight / 4,
                   aColorsFixed[0],
                   aColorsFixed[1]);
    anYBottomIter += theBarHeight / 4;
    addColoredQuad(aTriangles,
                   anXLeft,
                   anYBottomIter,
                   theColorBreadth,
                   theBarHeight / 4,
                   aColorsFixed[1],
                   aColorsFixed[2]);
    anYBottomIter += theBarHeight / 4;
    addColoredQuad(aTriangles,
                   anXLeft,
                   anYBottomIter,
                   theColorBreadth,
                   theBarHeight / 4,
                   aColorsFixed[2],
                   aColorsFixed[3]);
    anYBottomIter += theBarHeight / 4;
    const int aLastSizeY = theBarBottom + theBarHeight - anYBottomIter;
    addColoredQuad(aTriangles,
                   anXLeft,
                   anYBottomIter,
                   theColorBreadth,
                   aLastSizeY,
                   aColorsFixed[3],
                   aColorsFixed[4]);
  }
  else
  {
    // no color smoothing
    aTriangles        = new Graphic3d_ArrayOfTriangles(aColors.Length() * 4,     // quads
                                                aColors.Length() * 2 * 3, // quads as triangles
                                                false,
                                                true); // per-vertex colors
    int anYBottomIter = theBarBottom;
    for (int aColorIter = 0; aColorIter < myNbIntervals; ++aColorIter)
    {
      const Quantity_Color& aColor = aColors.Value(aColorIter + 1);
      const int             aSizeY = theBarBottom + int((aColorIter + 1) * aStepY) - anYBottomIter;
      addColoredQuad(aTriangles, anXLeft, anYBottomIter, theColorBreadth, aSizeY, aColor, aColor);
      anYBottomIter += aSizeY;
    }
  }

  occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
  aGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
  aGroup->AddPrimitiveArray(aTriangles);

  const Quantity_Color aFgColor(hasOwnColor ? myDrawer->Color() : Quantity_NOC_WHITE);
  drawFrame(thePrs, anXLeft - 1, theBarBottom - 1, theColorBreadth + 2, theBarHeight + 2, aFgColor);
}

//=================================================================================================

void AIS_ColorScale::drawLabels(const occ::handle<Graphic3d_Group>&                     theGroup,
                                const NCollection_Sequence<TCollection_ExtendedString>& theLabels,
                                const int theBarBottom,
                                const int theBarHeight,
                                const int theMaxLabelWidth,
                                const int theColorBreadth)
{
  if (myLabelPos == Aspect_TOCSP_NONE || theLabels.IsEmpty())
  {
    return;
  }

  const int    aNbLabels    = theLabels.Size();
  const int    aNbIntervals = myIsLabelAtBorder ? aNbLabels - 1 : aNbLabels;
  const double aStepY       = double(theBarHeight) / double(aNbIntervals);
  if (aStepY <= 0.0)
  {
    return;
  }

  int aFilter = 0;
  {
    const int aTitleHeight = !myTitle.IsEmpty() ? (myTextHeight + 2 * mySpacing) : mySpacing;
    const int aSpc =
      myHeight - aTitleHeight
      - ((std::min(aNbLabels, 2) + std::abs(aNbLabels - aNbIntervals - 1)) * myTextHeight);
    if (aSpc <= 0)
    {
      return;
    }

    const double aVal    = double(aNbLabels) * myTextHeight / aSpc;
    double       anIPart = 0.0;
    double       anFPart = std::modf(aVal, &anIPart);
    aFilter              = (int)anIPart + (anFPart != 0 ? 1 : 0);
  }
  if (aFilter <= 0)
  {
    return;
  }

  int       anXLeft  = myXPos + mySpacing;
  const int anAscent = 0;
  switch (myLabelPos)
  {
    case Aspect_TOCSP_NONE:
    case Aspect_TOCSP_LEFT: {
      break;
    }
    case Aspect_TOCSP_CENTER: {
      anXLeft += (theColorBreadth - theMaxLabelWidth) / 2;
      break;
    }
    case Aspect_TOCSP_RIGHT: {
      anXLeft += theColorBreadth + mySpacing;
      break;
    }
  }

  int       i1        = 0;
  int       i2        = aNbLabels - 1;
  int       aLast1    = i1;
  int       aLast2    = i2;
  const int anYBottom = myIsLabelAtBorder ? theBarBottom : theBarBottom + int(aStepY / 2);
  while (i2 - i1 >= aFilter || (i2 == 0 && i1 == 0))
  {
    int aPos1 = i1;
    int aPos2 = aNbLabels - 1 - i2;
    if (aFilter && !(aPos1 % aFilter))
    {
      drawText(theGroup,
               theLabels.Value(i1 + 1),
               anXLeft,
               anYBottom + int(i1 * aStepY + anAscent),
               Graphic3d_VTA_CENTER);
      aLast1 = i1;
    }
    if (aFilter && !(aPos2 % aFilter))
    {
      drawText(theGroup,
               theLabels.Value(i2 + 1),
               anXLeft,
               anYBottom + int(i2 * aStepY + anAscent),
               Graphic3d_VTA_CENTER);
      aLast2 = i2;
    }
    i1++;
    i2--;
  }
  int aPos = i1;
  int i0   = -1;
  while (aPos <= i2 && i0 == -1)
  {
    if (aFilter && !(aPos % aFilter) && std::abs(aPos - aLast1) >= aFilter
        && std::abs(aPos - aLast2) >= aFilter)
    {
      i0 = aPos;
    }
    aPos++;
  }

  if (i0 != -1)
  {
    drawText(theGroup,
             theLabels.Value(i0 + 1),
             anXLeft,
             anYBottom + int(i0 * aStepY + anAscent),
             Graphic3d_VTA_CENTER);
  }
}

//=================================================================================================

void AIS_ColorScale::drawFrame(const occ::handle<Prs3d_Presentation>& thePrs,
                               const int                              theX,
                               const int                              theY,
                               const int                              theWidth,
                               const int                              theHeight,
                               const Quantity_Color&                  theColor)
{
  occ::handle<Graphic3d_ArrayOfPolylines> aPrim = new Graphic3d_ArrayOfPolylines(5);
  aPrim->AddVertex(theX, theY, 0.0);
  aPrim->AddVertex(theX + theWidth, theY, 0.0);
  aPrim->AddVertex(theX + theWidth, theY + theHeight, 0.0);
  aPrim->AddVertex(theX, theY + theHeight, 0.0);
  aPrim->AddVertex(theX, theY, 0.0);

  occ::handle<Graphic3d_AspectLine3d> anAspect =
    new Graphic3d_AspectLine3d(theColor, Aspect_TOL_SOLID, 1.0);
  occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
  aGroup->SetGroupPrimitivesAspect(anAspect);
  aGroup->AddPrimitiveArray(aPrim);
}

//=================================================================================================

void AIS_ColorScale::drawText(const occ::handle<Graphic3d_Group>&   theGroup,
                              const TCollection_ExtendedString&     theText,
                              const int                             theX,
                              const int                             theY,
                              const Graphic3d_VerticalTextAlignment theVertAlignment)
{
  const occ::handle<Prs3d_TextAspect>& anAspect = myDrawer->TextAspect();

  occ::handle<Graphic3d_Text> aText = new Graphic3d_Text((float)anAspect->Height());
  aText->SetText(theText.ToExtString());
  aText->SetOrientation(gp_Ax2(gp_Pnt(theX, theY, 0.0), gp::DZ()));
  aText->SetOwnAnchorPoint(false);
  aText->SetVerticalAlignment(theVertAlignment);

  theGroup->AddText(aText);
}

//=================================================================================================

int AIS_ColorScale::TextWidth(const TCollection_ExtendedString& theText) const
{
  int aWidth = 0, anAscent = 0, aDescent = 0;
  TextSize(theText, myTextHeight, aWidth, anAscent, aDescent);
  return aWidth;
}

//=================================================================================================

int AIS_ColorScale::TextHeight(const TCollection_ExtendedString& theText) const
{
  int aWidth = 0, anAscent = 0, aDescent = 0;
  TextSize(theText, myTextHeight, aWidth, anAscent, aDescent);
  return anAscent + aDescent;
}

//=================================================================================================

void AIS_ColorScale::TextSize(const TCollection_ExtendedString& theText,
                              const int                         theHeight,
                              int&                              theWidth,
                              int&                              theAscent,
                              int&                              theDescent) const
{
  float aWidth = 10.0f, anAscent = 1.0f, aDescent = 1.0f;
  if (HasInteractiveContext())
  {
    const TCollection_AsciiString       aText(theText);
    const occ::handle<V3d_Viewer>&      aViewer = GetContext()->CurrentViewer();
    const occ::handle<Graphic3d_CView>& aView   = aViewer->ActiveViewIterator().Value()->View();
    aViewer->Driver()
      ->TextSize(aView, aText.ToCString(), (float)theHeight, aWidth, anAscent, aDescent);
  }
  theWidth   = (int)aWidth;
  theAscent  = (int)anAscent;
  theDescent = (int)aDescent;
}
