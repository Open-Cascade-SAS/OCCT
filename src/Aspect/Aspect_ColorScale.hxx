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

#ifndef _Aspect_ColorScale_HeaderFile
#define _Aspect_ColorScale_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_SequenceOfColor.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>
#include <MMgt_TShared.hxx>
class Quantity_Color;
class TCollection_ExtendedString;
class TCollection_AsciiString;


class Aspect_ColorScale;
DEFINE_STANDARD_HANDLE(Aspect_ColorScale, MMgt_TShared)

//! Defines a color scale for viewer.
class Aspect_ColorScale : public MMgt_TShared
{

public:

  
  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  Standard_EXPORT Standard_Boolean FindColor (const Standard_Real theValue, Quantity_Color& theColor) const;
  
  Standard_EXPORT static Standard_Boolean FindColor (const Standard_Real theValue, const Standard_Real theMin, const Standard_Real theMax, const Standard_Integer theColorsCount, Quantity_Color& theColor);
  
  //! Returns minimal value of color scale;
  Standard_EXPORT Standard_Real GetMin() const;
  
  //! Returns maximal value of color scale;
  Standard_EXPORT Standard_Real GetMax() const;
  
  //! Returns minimal and maximal values of color scale;
  Standard_EXPORT void GetRange (Standard_Real& theMin, Standard_Real& theMax) const;
  
  //! Returns the type of labels;
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  Standard_EXPORT Aspect_TypeOfColorScaleData GetLabelType() const;
  
  //! Returns the type of colors;
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  Standard_EXPORT Aspect_TypeOfColorScaleData GetColorType() const;
  
  //! Returns the number of color scale intervals;
  Standard_EXPORT Standard_Integer GetNumberOfIntervals() const;
  
  //! Returns the color scale title string;
  Standard_EXPORT TCollection_ExtendedString GetTitle() const;
  
  //! Returns the format for numbers.
  //! The same like format for function printf().
  //! Used if GetLabelType() is TOCSD_AUTO;
  Standard_EXPORT TCollection_AsciiString GetFormat() const;
  
  //! Returns the user specified label with index <anIndex>.
  //! Returns empty string if label not defined.
  Standard_EXPORT TCollection_ExtendedString GetLabel (const Standard_Integer theIndex) const;
  
  //! Returns the user specified color from color map with index <anIndex>.
  //! Returns default color if index out of range in color map.
  Standard_EXPORT Quantity_Color GetColor (const Standard_Integer theIndex) const;
  
  //! Returns the user specified labels.
  Standard_EXPORT void GetLabels (TColStd_SequenceOfExtendedString& theLabels) const;
  
  //! Returns the user specified colors.
  Standard_EXPORT void GetColors (Aspect_SequenceOfColor& theColors) const;
  
  //! Returns the position of labels concerning color filled rectangles.
  Standard_EXPORT Aspect_TypeOfColorScalePosition GetLabelPosition() const;
  
  //! Returns the position of color scale title.
  Standard_EXPORT Aspect_TypeOfColorScalePosition GetTitlePosition() const;
  
  //! Returns true if the labels and colors used in reversed order.
  Standard_EXPORT Standard_Boolean IsReversed() const;
  
  //! Returns true if the labels placed at border of color filled rectangles.
  Standard_EXPORT Standard_Boolean IsLabelAtBorder() const;
  
  //! Sets the minimal value of color scale.
  Standard_EXPORT void SetMin (const Standard_Real theMin);
  
  //! Sets the maximal value of color scale.
  Standard_EXPORT void SetMax (const Standard_Real theMax);
  
  //! Sets the minimal and maximal value of color scale.
  Standard_EXPORT void SetRange (const Standard_Real theMin, const Standard_Real theMax);
  
  //! Sets the type of labels.
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  Standard_EXPORT void SetLabelType (const Aspect_TypeOfColorScaleData theType);
  
  //! Sets the type of colors.
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  Standard_EXPORT void SetColorType (const Aspect_TypeOfColorScaleData theType);
  
  //! Sets the number of color scale intervals.
  Standard_EXPORT void SetNumberOfIntervals (const Standard_Integer theNum);
  
  //! Sets the color scale title string.
  Standard_EXPORT void SetTitle (const TCollection_ExtendedString& theTitle);
  
  //! Sets the color scale auto label format specification.
  Standard_EXPORT void SetFormat (const TCollection_AsciiString& theFormat);
  
  //! Sets the color scale label at index. Index started from 1.
  Standard_EXPORT void SetLabel (const TCollection_ExtendedString& theLabel, const Standard_Integer anIndex = -1);
  
  //! Sets the color scale color at index. Index started from 1.
  Standard_EXPORT void SetColor (const Quantity_Color& theColor, const Standard_Integer theIndex = -1);
  
  //! Sets the color scale labels.
  Standard_EXPORT void SetLabels (const TColStd_SequenceOfExtendedString& theSeq);
  
  //! Sets the color scale colors.
  Standard_EXPORT void SetColors (const Aspect_SequenceOfColor& theSeq);
  
  //! Sets the color scale labels position concerning color filled rectangles.
  Standard_EXPORT void SetLabelPosition (const Aspect_TypeOfColorScalePosition thePos);
  
  //! Sets the color scale title position.
  Standard_EXPORT void SetTitlePosition (const Aspect_TypeOfColorScalePosition thePos);
  
  //! Sets true if the labels and colors used in reversed order.
  Standard_EXPORT void SetReversed (const Standard_Boolean theReverse);
  
  //! Sets true if the labels placed at border of color filled rectangles.
  Standard_EXPORT void SetLabelAtBorder (const Standard_Boolean theOn);
  
  //! Returns the size of color scale.
  Standard_EXPORT void GetSize (Standard_Real& theWidth, Standard_Real& theHeight) const;
  
  //! Returns the width of color scale.
  Standard_EXPORT Standard_Real GetWidth() const;
  
  //! Returns the height of color scale.
  Standard_EXPORT Standard_Real GetHeight() const;
  
  //! Sets the size of color scale.
  Standard_EXPORT void SetSize (const Standard_Real theWidth, const Standard_Real theHeight);
  
  //! Sets the width of color scale.
  Standard_EXPORT void SetWidth (const Standard_Real theWidth);
  
  //! Sets the height of color scale.
  Standard_EXPORT void SetHeight (const Standard_Real theHeight);
  
  //! Returns the position of color scale.
  Standard_EXPORT void GetPosition (Standard_Real& theX, Standard_Real& theY) const;
  
  //! Returns the X position of color scale.
  Standard_EXPORT Standard_Real GetXPosition() const;
  
  //! Returns the height of color scale.
  Standard_EXPORT Standard_Real GetYPosition() const;
  
  //! Sets the position of color scale.
  Standard_EXPORT void SetPosition (const Standard_Real theX, const Standard_Real theY);
  
  //! Sets the X position of color scale.
  Standard_EXPORT void SetXPosition (const Standard_Real theX);
  
  //! Sets the Y position of color scale.
  Standard_EXPORT void SetYPosition (const Standard_Real theY);
  
  //! Returns the height of text of color scale.
  Standard_EXPORT Standard_Integer GetTextHeight() const;
  
  //! Sets the height of text of color scale.
  Standard_EXPORT void SetTextHeight (const Standard_Integer theHeight);
  
  //! Draws a rectangle.
  //! @param theX [in] the X coordinate of rectangle position.
  //! @param theY [in] the Y coordinate of rectangle position.
  //! @param theWidth [in] the width of rectangle.
  //! @param theHeight [in] the height of rectangle.
  //! @param theColor [in] the color of rectangle.
  //! @param theFilled [in] defines if rectangle must be filled.
  Standard_EXPORT virtual void PaintRect (const Standard_Integer theX, const Standard_Integer theY, const Standard_Integer theWidth, const Standard_Integer theHeight, const Quantity_Color& theColor, const Standard_Boolean theFilled = Standard_False) = 0;
  
  //! Draws a text.
  //! @param theText [in] the text to draw.
  //! @param theX [in] the X coordinate of text position.
  //! @param theY [in] the Y coordinate of text position.
  //! @param theColor [in] the color of text.
  Standard_EXPORT virtual void PaintText (const TCollection_ExtendedString& theText, const Standard_Integer theX, const Standard_Integer theY, const Quantity_Color& theColor) = 0;
  
  //! Returns the width of text.
  //! @param theText [in] the text of which to calculate width.
  Standard_EXPORT virtual Standard_Integer TextWidth (const TCollection_ExtendedString& theText) const = 0;
  
  //! Returns the height of text.
  //! @param theText [in] the text of which to calculate height.
  Standard_EXPORT virtual Standard_Integer TextHeight (const TCollection_ExtendedString& theText) const = 0;




  DEFINE_STANDARD_RTTI(Aspect_ColorScale,MMgt_TShared)

protected:

  
  //! Default constructor.
  Standard_EXPORT Aspect_ColorScale();
  
  //! Returns the size of color scale.
  //! @param theWidth [out] the width of color scale.
  //! @param theHeight [out] the height of color scale.
  Standard_EXPORT void SizeHint (Standard_Integer& theWidth, Standard_Integer& theHeight) const;
  
  //! updates color scale parameters.
  Standard_EXPORT virtual void UpdateColorScale();
  
  //! Draws color scale.
  //! @param theBgColor [in] background color
  //! @param theX [in] the X coordinate of color scale position.
  //! @param theY [in] the Y coordinate of color scale position.
  //! @param theWidth [in] the width of color scale.
  //! @param theHeight [in] the height of color scale.
  Standard_EXPORT void DrawScale (const Quantity_Color& theBgColor, const Standard_Integer theX, const Standard_Integer theY, const Standard_Integer theWidth, const Standard_Integer theHeight);
  
  Standard_EXPORT virtual Standard_Boolean BeginPaint();
  
  Standard_EXPORT virtual Standard_Boolean EndPaint();



private:

  
  //! Returns the format of text.
  Standard_EXPORT TCollection_AsciiString Format() const;
  
  //! Returns the value of given interval.
  Standard_EXPORT Standard_Real GetNumber (const Standard_Integer theIndex) const;
  
  //! Returns the color's hue for the given value in the given interval.
  //! @param theValue [in] the current value of interval.
  //! @param theMin [in] the min value of interval.
  //! @param theMax [in] the max value of interval.
  Standard_EXPORT static Standard_Integer HueFromValue (const Standard_Integer theValue, const Standard_Integer theMin, const Standard_Integer theMax);

  Standard_Real myMin;
  Standard_Real myMax;
  TCollection_ExtendedString myTitle;
  TCollection_AsciiString myFormat;
  Standard_Integer myInterval;
  Aspect_TypeOfColorScaleData myColorType;
  Aspect_TypeOfColorScaleData myLabelType;
  Standard_Boolean myAtBorder;
  Standard_Boolean myReversed;
  Aspect_SequenceOfColor myColors;
  TColStd_SequenceOfExtendedString myLabels;
  Aspect_TypeOfColorScalePosition myLabelPos;
  Aspect_TypeOfColorScalePosition myTitlePos;
  Standard_Real myXPos;
  Standard_Real myYPos;
  Standard_Real myWidth;
  Standard_Real myHeight;
  Standard_Integer myTextHeight;


};







#endif // _Aspect_ColorScale_HeaderFile
