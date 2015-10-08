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

#ifndef _AIS_ColorScale_HeaderFile
#define _AIS_ColorScale_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Aspect_TypeOfColorScaleData.hxx>
#include <Aspect_TypeOfColorScalePosition.hxx>
#include <Aspect_SequenceOfColor.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>


//! Class for drawing a custom color scale
class AIS_ColorScale : public AIS_InteractiveObject {

public:

  //! Default constructor.
  Standard_EXPORT AIS_ColorScale();

  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  Standard_EXPORT Standard_Boolean FindColor (const Standard_Real theValue, Quantity_Color& theColor) const;

  Standard_EXPORT static Standard_Boolean FindColor (const Standard_Real theValue, const Standard_Real theMin, const Standard_Real theMax, const Standard_Integer theColorsCount, Quantity_Color& theColor);

  //! Returns minimal value of color scale;
  Standard_EXPORT Standard_Real GetMin() const { return myMin; }

  //! Returns maximal value of color scale;
  Standard_EXPORT Standard_Real GetMax() const { return myMax; }

  //! Returns minimal and maximal values of color scale;
  Standard_EXPORT void GetRange (Standard_Real& theMin, Standard_Real& theMax) const;

  //! Returns the type of labels;
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  Standard_EXPORT Aspect_TypeOfColorScaleData GetLabelType() const { return myLabelType; }

  //! Returns the type of colors;
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  Standard_EXPORT Aspect_TypeOfColorScaleData GetColorType() const { return myColorType; }

  //! Returns the number of color scale intervals;
  Standard_EXPORT Standard_Integer GetNumberOfIntervals() const { return myInterval; }

  //! Returns the color scale title string;
  Standard_EXPORT TCollection_ExtendedString GetTitle() const { return myTitle; }

  //! Returns the format for numbers.
  //! The same like format for function printf().
  //! Used if GetLabelType() is TOCSD_AUTO;
  Standard_EXPORT TCollection_AsciiString GetFormat() const { return myFormat; }

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
  Standard_EXPORT Aspect_TypeOfColorScalePosition GetLabelPosition() const { return myLabelPos; }

  //! Returns the position of color scale title.
  Standard_EXPORT Aspect_TypeOfColorScalePosition GetTitlePosition() const { return myTitlePos; }

  //! Returns true if the labels and colors used in reversed order.
  Standard_EXPORT Standard_Boolean IsReversed() const { return myReversed; }

  //! Returns true if the labels placed at border of color filled rectangles.
  Standard_EXPORT Standard_Boolean IsLabelAtBorder() const { return myAtBorder; }

  //! Returns true if the color scale has logarithmic intervals
  Standard_Boolean IsLogarithmic() const { return myIsLogarithmic; }

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

  //! Sets true if the color scale has logarithmic intervals.
  void SetLogarithmic (const Standard_Boolean isLogarithmic) { myIsLogarithmic = isLogarithmic; };

  //! Returns the size of color scale.
  Standard_EXPORT void GetSize (Standard_Integer& theWidth, Standard_Integer& theHeight) const;

  //! Returns the width of color scale.
  Standard_EXPORT Standard_Integer GetWidth() const { return myWidth; }

  //! Returns the height of color scale.
  Standard_EXPORT Standard_Integer GetHeight() const { return myHeight; }

  //! Returns the background color of color scale.
  const Quantity_Color& GetBgColor() const { return myBgColor; }

  //! Sets the size of color scale.
  Standard_EXPORT void SetSize (const Standard_Integer theWidth, const Standard_Integer theHeight);

  //! Sets the width of color scale.
  Standard_EXPORT void SetWidth (const Standard_Integer theWidth);

  //! Sets the height of color scale.
  Standard_EXPORT void SetHeight (const Standard_Integer theHeight);

  //! Sets the background color of color scale.
  void SetBGColor (const Quantity_Color& theBgColor) { myBgColor = theBgColor; };

  //! Returns the position of color scale.
  Standard_EXPORT void GetPosition (Standard_Real& theX, Standard_Real& theY) const;

  //! Returns the X position of color scale.
  Standard_EXPORT Standard_Integer GetXPosition() const { return myXPos; }

  //! Returns the height of color scale.
  Standard_EXPORT Standard_Integer GetYPosition() const { return myYPos; }

  //! Sets the position of color scale.
  Standard_EXPORT void SetPosition (const Standard_Integer theX, const Standard_Integer theY);

  //! Sets the X position of color scale.
  Standard_EXPORT void SetXPosition (const Standard_Integer theX);

  //! Sets the Y position of color scale.
  Standard_EXPORT void SetYPosition (const Standard_Integer theY);

  //! Returns the height of text of color scale.
  Standard_EXPORT Standard_Integer GetTextHeight() const { return myTextHeight; }

  //! Sets the height of text of color scale.
  Standard_EXPORT void SetTextHeight (const Standard_Integer theHeight) { myTextHeight = theHeight; }

  //! Returns the width of text.
  //! @param theText [in] the text of which to calculate width.
  Standard_EXPORT Standard_Integer TextWidth (const TCollection_ExtendedString& theText) const;

  //! Returns the height of text.
  //! @param theText [in] the text of which to calculate height.
  Standard_EXPORT Standard_Integer TextHeight (const TCollection_ExtendedString& theText) const;

  Standard_EXPORT void TextSize (const TCollection_ExtendedString& theText, const Standard_Integer theHeight, Standard_Integer& theWidth, Standard_Integer& theAscent, Standard_Integer& theDescent) const;


  DEFINE_STANDARD_RTTI(AIS_ColorScale,AIS_InteractiveObject)

protected:

  //! Draws a frame.
  //! @param theX [in] the X coordinate of frame position.
  //! @param theY [in] the Y coordinate of frame position.
  //! @param theWidth [in] the width of frame.
  //! @param theHeight [in] the height of frame.
  //! @param theColor [in] the color of frame.
  Standard_EXPORT void DrawFrame (const Handle(Prs3d_Presentation)& thePresentation,
                       const Standard_Integer theX, const Standard_Integer theY,
                       const Standard_Integer theWidth, const Standard_Integer theHeight,
                       const Quantity_Color& theColor);

  //! Draws a text.
  //! @param theText [in] the text to draw.
  //! @param theX [in] the X coordinate of text position.
  //! @param theY [in] the Y coordinate of text position.
  //! @param theColor [in] the color of text.
  Standard_EXPORT void DrawText (const Handle(Prs3d_Presentation)& thePresentation,
                  const TCollection_ExtendedString& theText,
                  const Standard_Integer theX, const Standard_Integer theY,
                  const Quantity_Color& theColor);

private:

  //! Returns the size of color scale.
  //! @param theWidth [out] the width of color scale.
  //! @param theHeight [out] the height of color scale.
  void SizeHint (Standard_Integer& theWidth, Standard_Integer& theHeight) const;

  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode);

  void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                           const Standard_Integer /*aMode*/){};

  //! Returns the format of text.
  TCollection_AsciiString Format() const;

  //! Returns the value of given interval.
  Standard_Real GetNumber (const Standard_Integer theIndex) const;

  //! Returns the value of given logarithmic interval.
  Standard_Real GetLogNumber (const Standard_Integer theIndex) const;

  //! Returns the color's hue for the given value in the given interval.
  //! @param theValue [in] the current value of interval.
  //! @param theMin [in] the min value of interval.
  //! @param theMax [in] the max value of interval.
  static Standard_Integer HueFromValue (const Standard_Integer theValue, const Standard_Integer theMin, const Standard_Integer theMax);

private:

  Standard_Real myMin;
  Standard_Real myMax;
  TCollection_ExtendedString myTitle;
  TCollection_AsciiString myFormat;
  Standard_Integer myInterval;
  Aspect_TypeOfColorScaleData myColorType;
  Aspect_TypeOfColorScaleData myLabelType;
  Standard_Boolean myAtBorder;
  Standard_Boolean myReversed;
  Standard_Boolean myIsLogarithmic;
  Aspect_SequenceOfColor myColors;
  TColStd_SequenceOfExtendedString myLabels;
  Aspect_TypeOfColorScalePosition myLabelPos;
  Aspect_TypeOfColorScalePosition myTitlePos;
  Standard_Integer myXPos;
  Standard_Integer myYPos;
  Standard_Integer myWidth;
  Standard_Integer myHeight;
  Standard_Integer myTextHeight;
  Quantity_Color myBgColor;
};
#endif
