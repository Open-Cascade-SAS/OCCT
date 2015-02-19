// Created on: 2004-06-22
// Created by: STV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#include <Aspect_ColorScale.ixx>

#include <Aspect_SequenceOfColor.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

#include <Precision.hxx>

#include <stdio.h>

Aspect_ColorScale::Aspect_ColorScale()
: MMgt_TShared(),
myMin( 0.0 ),
myMax( 1.0 ),
myTitle( "" ),
myFormat( "%.4g" ),
myInterval( 10 ),
myColorType( Aspect_TOCSD_AUTO ),
myLabelType( Aspect_TOCSD_AUTO ),
myAtBorder( Standard_True ),
myReversed( Standard_False ),
myLabelPos( Aspect_TOCSP_RIGHT ),
myTitlePos( Aspect_TOCSP_CENTER ),
myXPos( 0 ),
myYPos( 0 ),
myWidth( 0.2 ),
myHeight( 1 ),
myTextHeight(20)
{
}

Standard_Real Aspect_ColorScale::GetMin() const
{
  return myMin;
}

Standard_Real Aspect_ColorScale::GetMax() const
{
  return myMax;
}

void Aspect_ColorScale::GetRange (Standard_Real& theMin, Standard_Real& theMax) const
{
  theMin = myMin;
  theMax = myMax;
}

Aspect_TypeOfColorScaleData Aspect_ColorScale::GetLabelType() const
{
  return myLabelType;
}

Aspect_TypeOfColorScaleData Aspect_ColorScale::GetColorType() const
{
  return myColorType;
}

Standard_Integer Aspect_ColorScale::GetNumberOfIntervals() const
{
  return myInterval;
}

TCollection_ExtendedString Aspect_ColorScale::GetTitle() const
{
  return myTitle;
}

TCollection_AsciiString Aspect_ColorScale::GetFormat() const
{
  return myFormat;
}

TCollection_ExtendedString Aspect_ColorScale::GetLabel (const Standard_Integer theIndex) const
{
  if (GetLabelType() == Aspect_TOCSD_USER)
  {
    if (theIndex < 0
     || theIndex >= myLabels.Length())
    {
      return "";
    }

    return myLabels.Value (theIndex + 1);
  }

  const Standard_Real           aVal    = GetNumber (theIndex);
  const TCollection_AsciiString aFormat = Format();
  Standard_Character aBuf[1024];
  sprintf (aBuf, aFormat.ToCString(), aVal);
  return TCollection_ExtendedString (aBuf);
}

Quantity_Color Aspect_ColorScale::GetColor (const Standard_Integer theIndex) const
{
  if (GetColorType() == Aspect_TOCSD_USER)
  {
    if (theIndex < 0
     || theIndex >= myColors.Length())
    {
      return Quantity_Color();
    }

    return myColors.Value (theIndex + 1);
  }
  return Quantity_Color (HueFromValue (theIndex, 0, GetNumberOfIntervals() - 1), 1.0, 1.0, Quantity_TOC_HLS);
}

void Aspect_ColorScale::GetLabels (TColStd_SequenceOfExtendedString& theLabels) const
{
  theLabels.Clear();
  for (Standard_Integer i = 1; i <= myLabels.Length(); i++)
    theLabels.Append (myLabels.Value (i));
}

void Aspect_ColorScale::GetColors (Aspect_SequenceOfColor& theColors) const
{
  theColors.Clear();
  for (Standard_Integer i = 1; i <= myColors.Length(); i++)
    theColors.Append (myColors.Value (i));
}

Aspect_TypeOfColorScalePosition Aspect_ColorScale::GetLabelPosition() const
{
  return myLabelPos;
}

Aspect_TypeOfColorScalePosition Aspect_ColorScale::GetTitlePosition() const
{
  return myTitlePos;
}

Standard_Boolean Aspect_ColorScale::IsReversed() const
{
  return myReversed;
}

Standard_Boolean Aspect_ColorScale::IsLabelAtBorder() const
{
  return myAtBorder;
}

void Aspect_ColorScale::SetMin (const Standard_Real theMin)
{
  SetRange (theMin, GetMax());
}

void Aspect_ColorScale::SetMax (const Standard_Real theMax)
{
  SetRange (GetMin(), theMax);
}

void Aspect_ColorScale::SetRange (const Standard_Real theMin, const Standard_Real theMax)
{
  if (myMin == theMin && myMax == theMax)
    return;
 
  myMin = Min( theMin, theMax );
  myMax = Max( theMin, theMax );

  if (GetColorType() == Aspect_TOCSD_AUTO)
    UpdateColorScale();
}

void Aspect_ColorScale::SetLabelType (const Aspect_TypeOfColorScaleData theType)
{
  if (myLabelType == theType)
    return;

  myLabelType = theType;
  UpdateColorScale();
}

void Aspect_ColorScale::SetColorType (const Aspect_TypeOfColorScaleData theType)
{
  if (myColorType == theType)
    return;

  myColorType = theType;
  UpdateColorScale();
}

void Aspect_ColorScale::SetNumberOfIntervals (const Standard_Integer theNum)
{
  if (myInterval == theNum || theNum < 1)
    return;

  myInterval = theNum;
  UpdateColorScale();
}

void Aspect_ColorScale::SetTitle (const TCollection_ExtendedString& theTitle)
{
  if (myTitle == theTitle)
    return;

  myTitle = theTitle;
  UpdateColorScale();
}

void Aspect_ColorScale::SetFormat (const TCollection_AsciiString& theFormat)
{
  if (myFormat == theFormat)
    return;

  myFormat = theFormat;
  if (GetLabelType() == Aspect_TOCSD_AUTO)
    UpdateColorScale();
}

void Aspect_ColorScale::SetLabel (const TCollection_ExtendedString& theLabel, const Standard_Integer theIndex)
{
  Standard_Boolean changed = Standard_False;
  Standard_Integer i = theIndex < 0 ? myLabels.Length() + 1 : theIndex + 1;
  if (i <= myLabels.Length()) {
    changed = myLabels.Value (i) != theLabel;
    myLabels.SetValue (i, theLabel);
  }
  else {
    changed = Standard_True;
    while (i > myLabels.Length())
      myLabels.Append (TCollection_ExtendedString());
    myLabels.SetValue (i, theLabel);
  }
  if (changed)
    UpdateColorScale();
}

void Aspect_ColorScale::SetColor (const Quantity_Color& theColor, const Standard_Integer theIndex)
{
  Standard_Boolean changed = Standard_False;
  Standard_Integer i = theIndex < 0 ? myColors.Length() + 1 : theIndex + 1;
  if (i <= myColors.Length()) {
    changed = myColors.Value (i) != theColor;
    myColors.SetValue (i, theColor);
  }
  else {
    changed = Standard_True;
    while ( i > myColors.Length() )
      myColors.Append (Quantity_Color());
    myColors.SetValue (i, theColor);
  }
  if (changed)
    UpdateColorScale();
}

void Aspect_ColorScale::SetLabels (const TColStd_SequenceOfExtendedString& theSeq)
{
  myLabels.Clear();
  for (Standard_Integer i = 1; i <= theSeq.Length(); i++)
    myLabels.Append (theSeq.Value (i));
}

void Aspect_ColorScale::SetColors (const Aspect_SequenceOfColor& theSeq)
{
  myColors.Clear();
  for (Standard_Integer i = 1; i <= theSeq.Length(); i++)
    myColors.Append (theSeq.Value (i));
}

void Aspect_ColorScale::SetLabelPosition (const Aspect_TypeOfColorScalePosition thePos)
{
  if (myLabelPos == thePos)
    return;

  myLabelPos = thePos;
  UpdateColorScale();
}

void Aspect_ColorScale::SetTitlePosition (const Aspect_TypeOfColorScalePosition thePos)
{
  if (myTitlePos == thePos)
    return;

  myTitlePos = thePos;
  UpdateColorScale();
}

void Aspect_ColorScale::SetReversed (const Standard_Boolean theReverse)
{
  if (myReversed == theReverse)
    return;

  myReversed = theReverse;
  UpdateColorScale();
}

void Aspect_ColorScale::SetLabelAtBorder (const Standard_Boolean theOn)
{
  if (myAtBorder == theOn)
    return;

  myAtBorder = theOn;
  UpdateColorScale();
}

void Aspect_ColorScale::GetPosition (Standard_Real& theX, Standard_Real& theY) const
{
  theX = myXPos;
  theY = myYPos;
}

Standard_Real Aspect_ColorScale::GetXPosition() const
{
  return myXPos;
}

Standard_Real Aspect_ColorScale::GetYPosition() const
{
  return myYPos;
}

void Aspect_ColorScale::SetPosition (const Standard_Real theX, const Standard_Real theY)
{
  if (myXPos == theX && myYPos == theY)
    return;

  myXPos = theX;
  myYPos = theY;

  UpdateColorScale();
}

void Aspect_ColorScale::SetXPosition (const Standard_Real theX)
{
  SetPosition (theX, GetYPosition());
}

void Aspect_ColorScale::SetYPosition (const Standard_Real theY)
{
  SetPosition (GetXPosition(), theY);
}

void Aspect_ColorScale::GetSize (Standard_Real& theWidth, Standard_Real& theHeight) const
{
  theWidth = myWidth;
  theHeight = myHeight;
}

Standard_Real Aspect_ColorScale::GetWidth() const
{
  return myWidth;
}

Standard_Real Aspect_ColorScale::GetHeight() const
{
  return myHeight;
}

void Aspect_ColorScale::SetSize (const Standard_Real theWidth, const Standard_Real theHeight)
{
  if (myWidth == theWidth && myHeight == theHeight)
    return;

  myWidth = theWidth;
  myHeight = theHeight;

  UpdateColorScale();
}

void Aspect_ColorScale::SetWidth (const Standard_Real theWidth)
{
  SetSize (theWidth, GetHeight());
}

void Aspect_ColorScale::SetHeight (const Standard_Real theHeight)
{
  SetSize (GetWidth(), theHeight);
}

void Aspect_ColorScale::SizeHint (Standard_Integer& theWidth, Standard_Integer& theHeight) const
{
  Standard_Integer num = GetNumberOfIntervals();

  Standard_Integer spacer = 5;
  Standard_Integer textWidth = 0;
  Standard_Integer textHeight = TextHeight ("");
  Standard_Integer colorWidth = 20;

  if (GetLabelPosition() != Aspect_TOCSP_NONE)
    for (Standard_Integer idx = 0; idx < num; idx++)
      textWidth = Max (textWidth, TextWidth (GetLabel (idx + 1)));

  Standard_Integer scaleWidth = 0;
  Standard_Integer scaleHeight = 0;

  Standard_Integer titleWidth = 0;
  Standard_Integer titleHeight = 0;

  if (IsLabelAtBorder()) {
    num++;
    if (GetTitle().Length())
      titleHeight += 10;
  }

  scaleWidth = colorWidth + textWidth + ( textWidth ? 3 : 2 ) * spacer;
  scaleHeight = (Standard_Integer)( 1.5 * ( num + 1 ) * textHeight );
  
  if (GetTitle().Length()) {
    titleHeight = TextHeight (GetTitle()) + spacer;
    titleWidth =  TextWidth (GetTitle()) + 10;
  }

  theWidth = Max (titleWidth, scaleWidth);
  theHeight = scaleHeight + titleHeight;
}

void Aspect_ColorScale::DrawScale ( const Quantity_Color& theBgColor,
				      const Standard_Integer theX, const Standard_Integer theY,
				      const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  if (!BeginPaint())
    return;

  Standard_Integer num = GetNumberOfIntervals();
  Aspect_TypeOfColorScalePosition labPos = GetLabelPosition();

  Standard_Integer spacer = 5;
  Standard_Integer textWidth = 0;
  Standard_Integer textHeight = TextHeight ("");

  Standard_Boolean drawLabel = GetLabelPosition() != Aspect_TOCSP_NONE;

  TCollection_ExtendedString aTitle = GetTitle();

  Standard_Integer titleHeight = 0;

  Standard_Integer aGray = (Standard_Integer)(255 * ( theBgColor.Red() * 11 + theBgColor.Green() * 16 + theBgColor.Blue() * 5 ) / 32);
  Quantity_Color aFgColor (aGray < 128 ? Quantity_NOC_WHITE : Quantity_NOC_BLACK);

  // Draw title
  if (aTitle.Length()) {
    titleHeight = TextHeight (aTitle) + 2 * spacer;
    PaintText (aTitle, theX + spacer, theY + spacer, aFgColor);
  }

  Standard_Boolean reverse = IsReversed();

  Aspect_SequenceOfColor colors;
  TColStd_SequenceOfExtendedString labels;
  for (int idx = 0; idx < num; idx++) {
    if (reverse) {
      colors.Append (GetColor (idx));
      labels.Append (GetLabel (idx));
    }
    else {
      colors.Prepend (GetColor (idx));
      labels.Prepend (GetLabel (idx));
    }
  }

  if (IsLabelAtBorder()) {
    if (reverse)
      labels.Append (GetLabel (num));
    else
      labels.Prepend (GetLabel (num));
  }

  if (drawLabel)
    for (Standard_Integer i = 1; i <= labels.Length(); i++)
      textWidth = Max (textWidth, TextWidth (labels.Value (i)));

  Standard_Integer lab = labels.Length();

  Standard_Real spc = ( theHeight - ( ( Min (lab, 2) + Abs (lab - num - 1) ) * textHeight ) - titleHeight );
  Standard_Real val = spc != 0 ? 1.0 * ( lab - Min (lab, 1) ) * textHeight / spc : 0;
  Standard_Real iPart;
  Standard_Real fPart = modf (val, &iPart);
  Standard_Integer filter = (Standard_Integer)iPart + ( fPart != 0 ? 1 : 0 );

  Standard_Real step = 1.0 * ( theHeight - ( lab - num + Abs (lab - num - 1) ) * textHeight - titleHeight ) / num;

  Standard_Integer ascent = 0;
  Standard_Integer colorWidth = Max (5, Min (20, theWidth - textWidth - 3 * spacer));
  if (labPos == Aspect_TOCSP_CENTER || !drawLabel)
    colorWidth = theWidth - 2 * spacer;

  // Draw colors
  Standard_Integer x = theX + spacer;
  if (labPos == Aspect_TOCSP_LEFT)
    x += textWidth + ( textWidth ? 1 : 0 ) * spacer;

  Standard_Real offset = 1.0 * textHeight / 2 * ( lab - num + Abs (lab - num - 1) ) + titleHeight;
  for (Standard_Integer ci = 1; ci <= colors.Length() && step > 0; ci++ ) {
    Standard_Integer y = (Standard_Integer)( theY + ( ci - 1 )* step + offset);
    Standard_Integer h = (Standard_Integer)( theY + ( ci ) * step + offset ) - y;
    PaintRect (x, y, colorWidth, h, colors.Value (ci), Standard_True);
  }

  if (step > 0)
    PaintRect (x - 1, (Standard_Integer)(theY + offset - 1), colorWidth + 2, (Standard_Integer)(colors.Length() * step + 2), aFgColor);

  // Draw labels
  offset = 1.0 * Abs (lab - num - 1) * ( step - textHeight ) / 2 + 1.0 * Abs (lab - num - 1) * textHeight / 2;
  offset += titleHeight;
  if (drawLabel && labels.Length() && filter > 0) {
    Standard_Integer i1 = 0;
    Standard_Integer i2 = lab - 1;
    Standard_Integer last1 (i1), last2 (i2);
    x = theX + spacer;
    switch ( labPos ) {
    case Aspect_TOCSP_NONE:
    case Aspect_TOCSP_LEFT:
      break;
    case Aspect_TOCSP_CENTER:
      x += ( colorWidth - textWidth ) / 2;
      break;
    case Aspect_TOCSP_RIGHT:
      x += colorWidth + spacer;
      break;
    }
    while (i2 - i1 >= filter || ( i2 == 0 && i1 == 0 )) {
      Standard_Integer pos1 = i1;
      Standard_Integer pos2 = lab - 1 - i2;
      if (filter && !( pos1 % filter )) {
        PaintText (labels.Value (i1 + 1), x, (Standard_Integer)( theY + i1 * step + ascent + offset ), aFgColor);
        last1 = i1;
      }
      if (filter && !( pos2 % filter )) {
        PaintText (labels.Value (i2 + 1), x, (Standard_Integer)( theY + i2 * step + ascent + offset ), aFgColor);
        last2 = i2;
      }
      i1++;
      i2--;
    }
    Standard_Integer pos = i1;
    Standard_Integer i0 = -1;
    while (pos <= i2 && i0 == -1) {
      if (filter && !( pos % filter ) && Abs (pos - last1) >= filter && Abs (pos - last2) >= filter)
        i0 = pos;
      pos++;
    }

    if (i0 != -1)
      PaintText (labels.Value (i0 + 1), x, (Standard_Integer)( theY + i0 * step + ascent + offset ), aFgColor);
  }

  EndPaint();
}

Standard_Boolean Aspect_ColorScale::BeginPaint()
{
  return Standard_True;
}

Standard_Boolean Aspect_ColorScale::EndPaint()
{
  return Standard_True;
}

void Aspect_ColorScale::UpdateColorScale()
{
}

TCollection_AsciiString Aspect_ColorScale::Format() const
{
  return GetFormat();
}

Standard_Real Aspect_ColorScale::GetNumber (const Standard_Integer theIndex) const
{
  Standard_Real aNum = 0;
  if (GetNumberOfIntervals() > 0)
    aNum = GetMin() + theIndex * ( Abs (GetMax() - GetMin()) / GetNumberOfIntervals() );
  return aNum;
}

Standard_Integer Aspect_ColorScale::HueFromValue (const Standard_Integer theValue,
                                                  const Standard_Integer theMin, const Standard_Integer theMax)
{
  Standard_Integer minLimit (0), maxLimit (230);

  Standard_Integer aHue = maxLimit;
  if (theMin != theMax)
    aHue = (Standard_Integer)( maxLimit - ( maxLimit - minLimit ) * ( theValue - theMin ) / ( theMax - theMin ) );

  aHue = Min (Max (minLimit, aHue), maxLimit);

  return aHue;
}

Standard_Integer  Aspect_ColorScale::GetTextHeight() const {
  return myTextHeight;
}

void Aspect_ColorScale::SetTextHeight (const Standard_Integer theHeight) {
  myTextHeight = theHeight;
  UpdateColorScale ();
}


Standard_Boolean Aspect_ColorScale::FindColor (const Standard_Real theValue,
                                               Quantity_Color& theColor) const
{
  return FindColor (theValue, myMin, myMax, myInterval, theColor);
}


Standard_Boolean Aspect_ColorScale::FindColor (const Standard_Real theValue,
                                               const Standard_Real theMin,
                                               const Standard_Real theMax,
                                               const Standard_Integer theColorsCount,
                                               Quantity_Color& theColor)
{
  if(theValue<theMin || theValue>theMax || theMax<theMin)
    return Standard_False;

  else
  {
    Standard_Real IntervNumber = 0;
    if(Abs (theMax-theMin) > Precision::Approximation())
      IntervNumber = Floor (Standard_Real( theColorsCount ) * ( theValue - theMin ) / ( theMax - theMin ));

    Standard_Integer Interv = Standard_Integer (IntervNumber);

    theColor = Quantity_Color (HueFromValue (Interv, 0, theColorsCount - 1), 1.0, 1.0, Quantity_TOC_HLS);

    return Standard_True;
  } 
}
