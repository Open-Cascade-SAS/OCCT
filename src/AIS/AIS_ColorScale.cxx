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
#include <Aspect_Window.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <StdPrs_Curve.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ColorScale, AIS_InteractiveObject)

namespace
{
  //! Method to add colored quad into array of triangles.
  static void addColoredQuad (const Handle(Graphic3d_ArrayOfTriangles)& theTris,
                              const Standard_Integer theXLeft, const Standard_Integer theYBottom,
                              const Standard_Integer theSizeX, const Standard_Integer theSizeY,
                              const Quantity_Color& theColorBottom,
                              const Quantity_Color& theColorTop)
  {
    const Standard_Integer aVertIndex = theTris->VertexNumber() + 1;
    theTris->AddVertex (gp_Pnt (theXLeft,            theYBottom,            0.0), theColorBottom);
    theTris->AddVertex (gp_Pnt (theXLeft + theSizeX, theYBottom,            0.0), theColorBottom);
    theTris->AddVertex (gp_Pnt (theXLeft,            theYBottom + theSizeY, 0.0), theColorTop);
    theTris->AddVertex (gp_Pnt (theXLeft + theSizeX, theYBottom + theSizeY, 0.0), theColorTop);
    theTris->AddEdge (aVertIndex);
    theTris->AddEdge (aVertIndex + 1);
    theTris->AddEdge (aVertIndex + 2);
    theTris->AddEdge (aVertIndex + 1);
    theTris->AddEdge (aVertIndex + 2);
    theTris->AddEdge (aVertIndex + 3);
  }

  //! Compute hue angle from specified value.
  static Quantity_Color colorFromValueEx (const Standard_Real theValue,
                                          const Standard_Real theMin,
                                          const Standard_Real theMax,
                                          const Graphic3d_Vec3d& theHlsMin,
                                          const Graphic3d_Vec3d& theHlsMax)
  {
    const Standard_Real aValueDelta = theMax - theMin;
    Standard_Real aValue = 0.0;
    if (aValueDelta != 0.0)
    {
      aValue = (theValue - theMin) / aValueDelta;
    }

    Standard_Real aHue        = NCollection_Lerp<Standard_Real>::Interpolate (theHlsMin[0], theHlsMax[0], aValue);
    Standard_Real aLightness  = NCollection_Lerp<Standard_Real>::Interpolate (theHlsMin[1], theHlsMax[1], aValue);
    Standard_Real aSaturation = NCollection_Lerp<Standard_Real>::Interpolate (theHlsMin[2], theHlsMax[2], aValue);
    return Quantity_Color (AIS_ColorScale::hueToValidRange (aHue), aLightness, aSaturation, Quantity_TOC_HLS);
  }
}

//=======================================================================
//function : AIS_ColorScale
//purpose  :
//=======================================================================
AIS_ColorScale::AIS_ColorScale()
: myMin (0.0),
  myMax (1.0),
  myColorHlsMin (230.0, 1.0, 1.0),
  myColorHlsMax (0.0,   1.0, 1.0),
  myFormat ("%.4g"),
  myNbIntervals (10),
  myColorType (Aspect_TOCSD_AUTO),
  myLabelType (Aspect_TOCSD_AUTO),
  myIsLabelAtBorder (Standard_True),
  myIsReversed (Standard_False),
  myIsLogarithmic (Standard_False),
  myIsSmooth (Standard_False),
  myLabelPos (Aspect_TOCSP_RIGHT),
  myTitlePos (Aspect_TOCSP_LEFT),
  myXPos (0),
  myYPos (0),
  myBreadth (0),
  myHeight  (0),
  mySpacing (5),
  myTextHeight (20)
{
  SetDisplayMode (0);
}

//=======================================================================
//function : GetLabel
//purpose  :
//=======================================================================
TCollection_ExtendedString AIS_ColorScale::GetLabel (const Standard_Integer theIndex) const
{
  if (myLabelType == Aspect_TOCSD_USER)
  {
    if (theIndex >= myLabels.Lower()
     || theIndex <= myLabels.Upper())
    {
      return myLabels.Value(theIndex);
    }
    return TCollection_ExtendedString();
  }

  // value to be shown depends on label position
  const Standard_Real aVal = myIsLabelAtBorder
                           ? GetIntervalValue (theIndex - 1)
                           : (0.5 * (GetIntervalValue (theIndex - 1) + GetIntervalValue (theIndex)));

  char aBuf[1024];
  sprintf (aBuf, myFormat.ToCString(), aVal);
  return TCollection_ExtendedString (aBuf);
}

//=======================================================================
//function : GetIntervalColor
//purpose  :
//=======================================================================
Quantity_Color AIS_ColorScale::GetIntervalColor (const Standard_Integer theIndex) const
{
  if (myColorType== Aspect_TOCSD_USER)
  {
    if (theIndex <= 0 || theIndex > myColors.Length())
    {
      return Quantity_Color();
    }
    return myColors.Value (theIndex);
  }

  return colorFromValue (theIndex - 1, 0, myNbIntervals - 1);
}

//=======================================================================
//function : GetLabels
//purpose  :
//=======================================================================
void AIS_ColorScale::GetLabels (TColStd_SequenceOfExtendedString& theLabels) const
{
  theLabels.Clear();
  for (TColStd_SequenceOfExtendedString::Iterator aLabIter (myLabels); aLabIter.More(); aLabIter.Next())
  {
    theLabels.Append (aLabIter.Value());
  }
}

//=======================================================================
//function : GetColors
//purpose  :
//=======================================================================
void AIS_ColorScale::GetColors (Aspect_SequenceOfColor& theColors) const
{
  theColors.Clear();
  for (Aspect_SequenceOfColor::Iterator aColorIter (myColors); aColorIter.More(); aColorIter.Next())
  {
    theColors.Append (aColorIter.Value());
  }
}

//=======================================================================
//function : SetRange
//purpose  :
//=======================================================================
void AIS_ColorScale::SetRange (const Standard_Real theMin, const Standard_Real theMax)
{
  myMin = Min (theMin, theMax);
  myMax = Max (theMin, theMax);
}

//=======================================================================
//function : SetNumberOfIntervals
//purpose  :
//=======================================================================
void AIS_ColorScale::SetNumberOfIntervals (const Standard_Integer theNum)
{
  if (theNum < 1)
  {
    return;
  }

  myNbIntervals = theNum;
}

//=======================================================================
//function : SetLabel
//purpose  :
//=======================================================================
void AIS_ColorScale::SetLabel (const TCollection_ExtendedString& theLabel,
                               const Standard_Integer theIndex)
{
  const Standard_Integer aLabIndex = (theIndex <= 0 ? myLabels.Length() + 1 : theIndex);
  while (myLabels.Length() < aLabIndex)
  {
    myLabels.Append (TCollection_ExtendedString());
  }
  myLabels.SetValue (aLabIndex, theLabel);
}

//=======================================================================
//function : SetIntervalColor
//purpose  :
//=======================================================================
void AIS_ColorScale::SetIntervalColor (const Quantity_Color&  theColor,
                                       const Standard_Integer theIndex)
{
  const Standard_Integer aColorIndex = (theIndex <= 0 ? myColors.Length() + 1 : theIndex);
  while (myColors.Length() < aColorIndex)
  {
    myColors.Append (Quantity_Color());
  }
  myColors.SetValue (aColorIndex, theColor);
}

//=======================================================================
//function : SetLabels
//purpose  :
//=======================================================================
void AIS_ColorScale::SetLabels (const TColStd_SequenceOfExtendedString& theSeq)
{
  myLabels.Clear();
  for (TColStd_SequenceOfExtendedString::Iterator aLabIter (theSeq); aLabIter.More(); aLabIter.Next())
  {
    myLabels.Append (aLabIter.Value());
  }
}

//=======================================================================
//function : SetColors
//purpose  :
//=======================================================================
void AIS_ColorScale::SetColors (const Aspect_SequenceOfColor& theSeq)
{
  myColors.Clear();
  for (Aspect_SequenceOfColor::Iterator aColorIter (theSeq); aColorIter.More(); aColorIter.Next())
  {
    myColors.Append (aColorIter.Value());
  }
}

//=======================================================================
//function : SizeHint
//purpose  :
//=======================================================================
void AIS_ColorScale::SizeHint (Standard_Integer& theWidth, Standard_Integer& theHeight) const
{
  const Standard_Integer aTextHeight = TextHeight ("");
  const Standard_Integer aColorWidth = 20;
  Standard_Integer aTextWidth = 0;
  if (myLabelPos != Aspect_TOCSP_NONE)
  {
    for (Standard_Integer aLabIter = (myIsLabelAtBorder ? 0 : 1); aLabIter <= myNbIntervals; ++aLabIter)
    {
      aTextWidth = Max (aTextWidth, TextWidth (GetLabel (aLabIter)));
    }
  }

  const Standard_Integer aScaleWidth  = aColorWidth + aTextWidth + (aTextWidth ? 3 : 2) * mySpacing;
  const Standard_Integer aScaleHeight = (Standard_Integer)(1.5 * (myNbIntervals + (myIsLabelAtBorder ? 2 : 1)) * aTextHeight);

  Standard_Integer aTitleWidth  = 0;
  Standard_Integer aTitleHeight = 0;
  if (!myTitle.IsEmpty())
  {
    aTitleHeight = TextHeight (myTitle) + mySpacing;
    aTitleWidth =  TextWidth  (myTitle) + mySpacing * 2;
  }

  theWidth  = Max (aTitleWidth, aScaleWidth);
  theHeight = aScaleHeight + aTitleHeight;
}

//=======================================================================
//function : GetIntervalValue
//purpose  :
//=======================================================================
Standard_Real AIS_ColorScale::GetIntervalValue (const Standard_Integer theIndex) const
{
  if (myNbIntervals <= 0)
  {
    return 0.0;
  }

  if (IsLogarithmic())
  {
    Standard_Real aMin     = myMin > 0 ? myMin : 1.0;
    Standard_Real aDivisor = std::pow (myMax / aMin, 1.0 / myNbIntervals);
    return aMin * std::pow (aDivisor,theIndex);
  }

  Standard_Real aNum = 0;
  if (myNbIntervals > 0)
  {
    aNum = GetMin() + theIndex * (Abs (GetMax() - GetMin()) / myNbIntervals);
  }
  return aNum;
}

//=======================================================================
//function : colorFromValue
//purpose  :
//=======================================================================
Quantity_Color AIS_ColorScale::colorFromValue (const Standard_Real theValue,
                                               const Standard_Real theMin,
                                               const Standard_Real theMax) const
{
  return colorFromValueEx (theValue, theMin, theMax, myColorHlsMin, myColorHlsMax);
}

//=======================================================================
//function : FindColor
//purpose  :
//=======================================================================
Standard_Boolean AIS_ColorScale::FindColor (const Standard_Real theValue,
                                            Quantity_Color& theColor) const
{
  if (theValue < myMin || theValue > myMax || myMax < myMin)
  {
    theColor = Quantity_Color();
    return Standard_False;
  }

  if (myColorType == Aspect_TOCSD_USER)
  {
    Standard_Integer anIndex = 0;
    if (Abs (myMax - myMin) > Precision::Approximation())
    {
      anIndex = (theValue - myMin < Precision::Confusion()) 
        ? 1
        : Standard_Integer (Ceiling (( theValue - myMin ) / ( (myMax - myMin) / myNbIntervals)));
    }

    if (anIndex <= 0 || anIndex > myColors.Length())
    {
      theColor = Quantity_Color();
      return Standard_False;
    }

    theColor = myColors.Value (anIndex);
    return Standard_True;
  }

  return FindColor (theValue, myMin, myMax, myNbIntervals, theColor);
}

//=======================================================================
//function : FindColor
//purpose  :
//=======================================================================
Standard_Boolean AIS_ColorScale::FindColor (const Standard_Real theValue,
                                            const Standard_Real theMin,
                                            const Standard_Real theMax,
                                            const Standard_Integer theColorsCount,
                                            const Graphic3d_Vec3d& theColorHlsMin,
                                            const Graphic3d_Vec3d& theColorHlsMax,
                                            Quantity_Color& theColor)
{
  if (theValue < theMin || theValue > theMax || theMax < theMin)
  {
    return Standard_False;
  }

  Standard_Real anInterval = 0.0;
  if (Abs (theMax - theMin) > Precision::Approximation())
  {
    anInterval = Floor (Standard_Real (theColorsCount) * (theValue - theMin) / (theMax - theMin));
  }

  theColor = colorFromValueEx (anInterval, 0, theColorsCount - 1, theColorHlsMin, theColorHlsMax);
  return Standard_True;
}

//=======================================================================
//function : computeMaxLabelWidth
//purpose  :
//=======================================================================
Standard_Integer AIS_ColorScale::computeMaxLabelWidth (const TColStd_SequenceOfExtendedString& theLabels) const
{
  {
    Handle(V3d_Viewer) aViewer = GetContext()->CurrentViewer();
    aViewer->InitActiveViews(); // for AIS_ColorScale::TextSize()
  }

  Standard_Integer aWidthMax = 0;
  for (TColStd_SequenceOfExtendedString::Iterator aLabIter (theLabels); aLabIter.More(); aLabIter.Next())
  {
    if (!aLabIter.Value().IsEmpty())
    {
      aWidthMax = Max (aWidthMax, TextWidth (aLabIter.Value()));
    }
  }
  return aWidthMax;
}

//=======================================================================
//function : updateTextAspect
//purpose  :
//=======================================================================
void AIS_ColorScale::updateTextAspect()
{
  // update text aspect
  const Quantity_Color aFgColor (hasOwnColor ? myDrawer->Color() : Quantity_NOC_WHITE);
  if (!myDrawer->HasOwnTextAspect())
  {
    myDrawer->SetTextAspect (new Prs3d_TextAspect());
    *myDrawer->TextAspect()->Aspect() = *myDrawer->Link()->TextAspect()->Aspect();
  }

  const Handle(Prs3d_TextAspect)& anAspect = myDrawer->TextAspect();
  anAspect->SetColor  (aFgColor);
  anAspect->SetHeight (myTextHeight);
  anAspect->SetHorizontalJustification (Graphic3d_HTA_LEFT);
  anAspect->SetVerticalJustification (Graphic3d_VTA_BOTTOM);
  anAspect->Aspect()->SetTextZoomable (Standard_True);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================
void AIS_ColorScale::Compute (const Handle(PrsMgr_PresentationManager3d)& ,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer theMode)
{
  if (theMode != 0)
  {
    return;
  }

  // update text aspect
  updateTextAspect();

  const Standard_Integer aTitleOffset = !myTitle.IsEmpty() ? (myTextHeight + mySpacing) : 0;

  const Standard_Integer aBarYOffset = myTextHeight / 2 + 2 * mySpacing; // a half-label offset
  const Standard_Integer aBarBottom  = myYPos + aBarYOffset;
  const Standard_Integer aBarTop     = myYPos + myHeight - aTitleOffset - aBarYOffset;
  const Standard_Integer aBarHeight  = aBarTop - aBarBottom;

  // draw title
  if (!myTitle.IsEmpty())
  {
    drawText (Prs3d_Root::CurrentGroup (thePrs), myTitle,
              myXPos + mySpacing,
              aBarTop + aBarYOffset,
              Graphic3d_VTA_BOTTOM);
  }

  TColStd_SequenceOfExtendedString aLabels;
  if (myLabelType == Aspect_TOCSD_USER)
  {
    aLabels = myLabels;
  }
  else
  {
    const Standard_Integer aNbLabels = myIsLabelAtBorder ? myNbIntervals + 1 : myNbIntervals;
    for (Standard_Integer aLabIter = 1; aLabIter <= aNbLabels; ++aLabIter)
    {
      if (myIsReversed)
      {
        aLabels.Prepend (GetLabel (aLabIter));
      }
      else
      {
        aLabels.Append (GetLabel (aLabIter));
      }
    }
  }

  const Standard_Integer aTextWidth = myLabelPos != Aspect_TOCSP_NONE ? computeMaxLabelWidth (aLabels) : 0;
  Standard_Integer aColorBreadth = Max (5, Min (20, myBreadth - aTextWidth - 3 * mySpacing));
  if (myLabelPos == Aspect_TOCSP_CENTER
   || myLabelPos == Aspect_TOCSP_NONE)
  {
    aColorBreadth += aTextWidth;
  }

  // draw colors
  drawColorBar (thePrs, aBarBottom, aBarHeight, aTextWidth, aColorBreadth);

  // draw Labels
  drawLabels (thePrs, aLabels, aBarBottom, aBarHeight, aTextWidth, aColorBreadth);
}

//=======================================================================
//function : drawColorBar
//purpose  :
//=======================================================================
void AIS_ColorScale::drawColorBar (const Handle(Prs3d_Presentation)& thePrs,
                                   const Standard_Integer theBarBottom,
                                   const Standard_Integer theBarHeight,
                                   const Standard_Integer theMaxLabelWidth,
                                   const Standard_Integer theColorBreadth)
{
  const Standard_Real aStepY = Standard_Real(theBarHeight) / Standard_Real(myNbIntervals);
  if (aStepY <= 0.0)
  {
    return;
  }

  // Draw colors
  const Standard_Integer anXLeft = myLabelPos == Aspect_TOCSP_LEFT
                                 ? myXPos + mySpacing + theMaxLabelWidth + (theMaxLabelWidth != 0 ? 1 : 0) * mySpacing
                                 : myXPos + mySpacing;

  Aspect_SequenceOfColor aColors;
  for (Standard_Integer anIntervalIter = 1; anIntervalIter <= myNbIntervals; ++anIntervalIter)
  {
    if (myIsReversed)
    {
      aColors.Prepend (GetIntervalColor (anIntervalIter));
    }
    else
    {
      aColors.Append (GetIntervalColor (anIntervalIter));
    }
  }

  Handle(Graphic3d_ArrayOfTriangles) aTriangles;
  if (myIsSmooth
   && myColorType == Aspect_TOCSD_USER)
  {
    // Smooth custom intervals, so that the color in the center of interval is equal to specified one
    // (thus the halves of first and last intervals have solid color)
    aTriangles = new Graphic3d_ArrayOfTriangles ((aColors.Length() + 1) * 4,     // quads
                                                 (aColors.Length() + 1) * 2 * 3, // quads as triangles
                                                 false, true);                   // per-vertex colors
    Quantity_Color aColor1 (aColors.Value (1)), aColor2;
    Standard_Integer       aSizeY        = Standard_Integer(aStepY / 2);
    const Standard_Integer anYBottom     = theBarBottom + aSizeY;
    Standard_Integer       anYBottomIter = anYBottom;
    addColoredQuad (aTriangles,
                    anXLeft, theBarBottom,
                    theColorBreadth, aSizeY,
                    aColor1, aColor1);
    for (Standard_Integer aColorIter = 0; aColorIter < myNbIntervals - 1; ++aColorIter)
    {
      aColor1 = aColors.Value (aColorIter + 1);
      aColor2 = aColors.Value (aColorIter + 2);
      aSizeY  = anYBottom + Standard_Integer((aColorIter + 1) * aStepY) - anYBottomIter;
      addColoredQuad (aTriangles,
                      anXLeft, anYBottomIter,
                      theColorBreadth, aSizeY,
                      aColor1, aColor2);
      anYBottomIter += aSizeY;
    }
    aColor2 = aColors.Value (myNbIntervals);
    aSizeY  = theBarBottom + theBarHeight - anYBottomIter;
    addColoredQuad (aTriangles,
                    anXLeft, anYBottomIter,
                    theColorBreadth, aSizeY,
                    aColor2, aColor2);
  }
  else if (myIsSmooth)
  {
    // smooth transition between standard colors - without solid color regions at the beginning and end of full color range
    const Quantity_Color aColorsFixed[5] =
    {
      colorFromValue (0, 0, 4),
      colorFromValue (1, 0, 4),
      colorFromValue (2, 0, 4),
      colorFromValue (3, 0, 4),
      colorFromValue (4, 0, 4)
    };
    aTriangles = new Graphic3d_ArrayOfTriangles (4 * 4,        // quads
                                                 4 * 2 * 3,    // quads as triangles
                                                 false, true); // per-vertex colors
    Standard_Integer anYBottomIter = theBarBottom;
    addColoredQuad (aTriangles,
                    anXLeft, theBarBottom,
                    theColorBreadth, theBarHeight / 4,
                    aColorsFixed[0], aColorsFixed[1]);
    anYBottomIter += theBarHeight / 4;
    addColoredQuad (aTriangles,
                    anXLeft, anYBottomIter,
                    theColorBreadth, theBarHeight / 4,
                    aColorsFixed[1], aColorsFixed[2]);
    anYBottomIter += theBarHeight / 4;
    addColoredQuad (aTriangles,
                    anXLeft, anYBottomIter,
                    theColorBreadth, theBarHeight / 4,
                    aColorsFixed[2], aColorsFixed[3]);
    anYBottomIter += theBarHeight / 4;
    const Standard_Integer aLastSizeY  = theBarBottom + theBarHeight - anYBottomIter;
    addColoredQuad (aTriangles,
                    anXLeft, anYBottomIter,
                    theColorBreadth, aLastSizeY,
                    aColorsFixed[3], aColorsFixed[4]);
  }
  else
  {
    // no color smoothing
    aTriangles = new Graphic3d_ArrayOfTriangles (aColors.Length() * 4,     // quads
                                                 aColors.Length() * 2 * 3, // quads as triangles
                                                 false, true);             // per-vertex colors
    Standard_Integer anYBottomIter = theBarBottom;
    for (Standard_Integer aColorIter = 0; aColorIter < myNbIntervals; ++aColorIter)
    {
      const Quantity_Color&  aColor = aColors.Value (aColorIter + 1);
      const Standard_Integer aSizeY = theBarBottom + Standard_Integer((aColorIter + 1) * aStepY) - anYBottomIter;
      addColoredQuad (aTriangles,
                      anXLeft, anYBottomIter,
                      theColorBreadth, aSizeY,
                      aColor, aColor);
      anYBottomIter += aSizeY;
    }
  }

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePrs);
  aGroup->AddPrimitiveArray (aTriangles);

  const Quantity_Color aFgColor (hasOwnColor ? myDrawer->Color() : Quantity_NOC_WHITE);
  drawFrame (thePrs,
             anXLeft - 1, theBarBottom - 1,
             theColorBreadth + 2,
             theBarHeight + 2,
             aFgColor);
}

//=======================================================================
//function : drawLabels
//purpose  :
//=======================================================================
void AIS_ColorScale::drawLabels (const Handle(Prs3d_Presentation)& thePrs,
                                 const TColStd_SequenceOfExtendedString& theLabels,
                                 const Standard_Integer theBarBottom,
                                 const Standard_Integer theBarHeight,
                                 const Standard_Integer theMaxLabelWidth,
                                 const Standard_Integer theColorBreadth)
{
  if (myLabelPos == Aspect_TOCSP_NONE
   || theLabels.IsEmpty())
  {
    return;
  }

  const Standard_Integer aNbLabels    = theLabels.Size();
  const Standard_Integer aNbIntervals = myIsLabelAtBorder ? aNbLabels - 1 : aNbLabels;
  const Standard_Real    aStepY       = Standard_Real(theBarHeight) / Standard_Real(aNbIntervals);
  if (aStepY <= 0.0)
  {
    return;
  }

  Standard_Integer aFilter = 0;
  {
    const Standard_Integer aTitleHeight = !myTitle.IsEmpty() ? (myTextHeight + 2 * mySpacing) : mySpacing;
    const Standard_Integer aSpc         = myHeight - aTitleHeight - ((Min (aNbLabels, 2) + Abs (aNbLabels - aNbIntervals - 1)) * myTextHeight);
    if (aSpc <= 0)
    {
      return;
    }

    const Standard_Real aVal = Standard_Real(aNbLabels) * myTextHeight / aSpc;
    Standard_Real anIPart = 0.0;
    Standard_Real anFPart = std::modf (aVal, &anIPart);
    aFilter = (Standard_Integer )anIPart + (anFPart != 0 ? 1 : 0);
  }
  if (aFilter <= 0)
  {
    return;
  }

  Standard_Integer anXLeft = myXPos + mySpacing;
  const Standard_Integer anAscent = 0;
  switch (myLabelPos)
  {
    case Aspect_TOCSP_NONE:
    case Aspect_TOCSP_LEFT:
    {
      break;
    }
    case Aspect_TOCSP_CENTER:
    {
      anXLeft += (theColorBreadth - theMaxLabelWidth) / 2;
      break;
    }
    case Aspect_TOCSP_RIGHT:
    {
      anXLeft += theColorBreadth + mySpacing;
      break;
    }
  }

  Standard_Integer i1 = 0;
  Standard_Integer i2 = aNbLabels - 1;
  Standard_Integer aLast1 = i1;
  Standard_Integer aLast2 = i2;
  const Standard_Integer anYBottom = myIsLabelAtBorder
                                   ? theBarBottom
                                   : theBarBottom + Standard_Integer(aStepY / 2);
  while (i2 - i1 >= aFilter || ( i2 == 0 && i1 == 0 ))
  {
    Standard_Integer aPos1 = i1;
    Standard_Integer aPos2 = aNbLabels - 1 - i2;
    if (aFilter && !(aPos1 % aFilter))
    {
      drawText (Prs3d_Root::CurrentGroup (thePrs), theLabels.Value (i1 + 1),
                anXLeft, anYBottom + Standard_Integer(i1 * aStepY + anAscent),
                Graphic3d_VTA_CENTER);
      aLast1 = i1;
    }
    if (aFilter && !(aPos2 % aFilter))
    {
      drawText (Prs3d_Root::CurrentGroup (thePrs), theLabels.Value (i2 + 1),
                anXLeft, anYBottom + Standard_Integer(i2 * aStepY + anAscent),
                Graphic3d_VTA_CENTER);
      aLast2 = i2;
    }
    i1++;
    i2--;
  }
  Standard_Integer aPos = i1;
  Standard_Integer i0   = -1;
  while (aPos <= i2 && i0 == -1)
  {
    if (aFilter && !(aPos % aFilter)
      && Abs (aPos - aLast1) >= aFilter
      && Abs (aPos - aLast2) >= aFilter)
    {
      i0 = aPos;
    }
    aPos++;
  }

  if (i0 != -1)
  {
    drawText (Prs3d_Root::CurrentGroup (thePrs), theLabels.Value (i0 + 1),
              anXLeft, anYBottom + Standard_Integer(i0 * aStepY + anAscent),
              Graphic3d_VTA_CENTER);
  }
}

//=======================================================================
//function : drawFrame
//purpose  :
//=======================================================================
void AIS_ColorScale::drawFrame (const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer theX, const Standard_Integer theY,
                                const Standard_Integer theWidth, const Standard_Integer theHeight,
                                const Quantity_Color& theColor)
{
  Handle(Graphic3d_ArrayOfPolylines) aPrim = new Graphic3d_ArrayOfPolylines(5);
  aPrim->AddVertex (theX,            theY, 0.0);
  aPrim->AddVertex (theX + theWidth, theY, 0.0);
  aPrim->AddVertex (theX + theWidth, theY + theHeight, 0.0);
  aPrim->AddVertex (theX,            theY + theHeight, 0.0);
  aPrim->AddVertex (theX,            theY, 0.0);

  Handle(Graphic3d_AspectLine3d) anAspect = new Graphic3d_AspectLine3d (theColor, Aspect_TOL_SOLID, 1.0);
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePrs);
  aGroup->SetPrimitivesAspect (anAspect);
  aGroup->AddPrimitiveArray (aPrim);
}

//=======================================================================
//function : drawText
//purpose  :
//=======================================================================
void AIS_ColorScale::drawText (const Handle(Graphic3d_Group)& theGroup,
                               const TCollection_ExtendedString& theText,
                               const Standard_Integer theX, const Standard_Integer theY,
                               const Graphic3d_VerticalTextAlignment theVertAlignment)
{
  const Handle(Prs3d_TextAspect)& anAspect = myDrawer->TextAspect();
  theGroup->SetPrimitivesAspect (anAspect->Aspect());
  theGroup->Text (theText,
                  gp_Ax2 (gp_Pnt (theX, theY, 0.0), gp::DZ()),
                  anAspect->Height(),
                  anAspect->Angle(),
                  anAspect->Orientation(),
                  Graphic3d_HTA_LEFT,
                  theVertAlignment,
                  Standard_True,
                  Standard_False); // has own anchor

}

//=======================================================================
//function : TextWidth
//purpose  :
//=======================================================================
Standard_Integer AIS_ColorScale::TextWidth (const TCollection_ExtendedString& theText) const
{
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize (theText, myTextHeight, aWidth, anAscent, aDescent);
  return aWidth;
}

//=======================================================================
//function : TextHeight
//purpose  :
//=======================================================================
Standard_Integer AIS_ColorScale::TextHeight (const TCollection_ExtendedString& theText) const
{
  Standard_Integer aWidth, anAscent, aDescent;
  TextSize (theText, myTextHeight, aWidth, anAscent, aDescent);
  return anAscent + aDescent;
}

//=======================================================================
//function : TextSize
//purpose  :
//=======================================================================
void AIS_ColorScale::TextSize (const TCollection_ExtendedString& theText,
                               const Standard_Integer theHeight,
                               Standard_Integer& theWidth,
                               Standard_Integer& theAscent,
                               Standard_Integer& theDescent) const
{
  if (!HasInteractiveContext())
  {
    return;
  }

  Standard_ShortReal aWidth   = 10.0f;
  Standard_ShortReal anAscent = 1.0f;
  Standard_ShortReal aDescent = 1.0f;
  const TCollection_AsciiString aText (theText);

  const Handle(V3d_Viewer)&      aViewer = GetContext()->CurrentViewer();
  const Handle(Graphic3d_CView)& aView   = aViewer->ActiveViewIterator().Value()->View();
  aViewer->Driver()->TextSize (aView, aText.ToCString(), (Standard_ShortReal)theHeight, aWidth, anAscent, aDescent);
  theWidth   = (Standard_Integer)aWidth;
  theAscent  = (Standard_Integer)anAscent;
  theDescent = (Standard_Integer)aDescent;
}
