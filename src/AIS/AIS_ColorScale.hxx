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

class AIS_ColorScale;
DEFINE_STANDARD_HANDLE(AIS_ColorScale, AIS_InteractiveObject)
//! Class for drawing a custom color scale.
//!
//! The color scale consists of rectangular color bar (composed of fixed
//! number of color intervals), optional labels, and title.
//! The labels can be positioned either at the boundaries of the intervals,
//! or at the middle of each interval.
//! Colors and labels can be either defined automatically or set by the user.
//! Automatic labels are calculated from numerical limits of the scale,
//! its type (logarithmic or plain), and formatted by specified format string.
class AIS_ColorScale : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_ColorScale, AIS_InteractiveObject)
public:

  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  Standard_EXPORT static Standard_Boolean FindColor (const Standard_Real theValue,
                                                     const Standard_Real theMin,
                                                     const Standard_Real theMax,
                                                     const Standard_Integer theColorsCount,
                                                     const Graphic3d_Vec3d& theColorHlsMin,
                                                     const Graphic3d_Vec3d& theColorHlsMax,
                                                     Quantity_Color& theColor);

  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  static Standard_Boolean FindColor (const Standard_Real theValue,
                                     const Standard_Real theMin,
                                     const Standard_Real theMax,
                                     const Standard_Integer theColorsCount,
                                     Quantity_Color& theColor)
  {
    return FindColor (theValue, theMin, theMax, theColorsCount,
                      Graphic3d_Vec3d (230.0, 1.0, 1.0),
                      Graphic3d_Vec3d (0.0,   1.0, 1.0),
                      theColor);
  }

  //! Shift hue into valid range.
  //! Lightness and Saturation should be specified in valid range [0.0, 1.0],
  //! however Hue might be given out of Quantity_Color range to specify desired range for interpolation.
  static Standard_Real hueToValidRange (const Standard_Real theHue)
  {
    Standard_Real aHue = theHue;
    while (aHue <   0.0) { aHue += 360.0; }
    while (aHue > 360.0) { aHue -= 360.0; }
    return aHue;
  }

public:

  //! Default constructor.
  Standard_EXPORT AIS_ColorScale();

  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  Standard_EXPORT Standard_Boolean FindColor (const Standard_Real theValue, Quantity_Color& theColor) const;

  //! Returns minimal value of color scale, 0.0 by default.
  Standard_Real GetMin() const { return myMin; }

  //! Sets the minimal value of color scale.
  void SetMin (const Standard_Real theMin) { SetRange (theMin, GetMax()); }

  //! Returns maximal value of color scale, 1.0 by default.
  Standard_Real GetMax() const { return myMax; }

  //! Sets the maximal value of color scale.
  void SetMax (const Standard_Real theMax) { SetRange (GetMin(), theMax); }

  //! Returns minimal and maximal values of color scale, 0.0 to 1.0 by default.
  void GetRange (Standard_Real& theMin, Standard_Real& theMax) const
  {
    theMin = myMin;
    theMax = myMax;
  }

  //! Sets the minimal and maximal value of color scale.
  //! Note that values order will be ignored - the minimum and maximum values will be swapped if needed.
  //! ::SetReversed() should be called to swap displaying order.
  Standard_EXPORT void SetRange (const Standard_Real theMin, const Standard_Real theMax);

  //! Returns the hue angle corresponding to minimum value, 230 by default (blue).
  Standard_Real HueMin() const { return myColorHlsMin[0]; }

  //! Returns the hue angle corresponding to maximum value, 0 by default (red).
  Standard_Real HueMax() const { return myColorHlsMax[0]; }

  //! Returns the hue angle range corresponding to minimum and maximum values, 230 to 0 by default (blue to red).
  void HueRange (Standard_Real& theMinAngle,
                 Standard_Real& theMaxAngle) const
  {
    theMinAngle = myColorHlsMin[0];
    theMaxAngle = myColorHlsMax[0];
  }

  //! Sets hue angle range corresponding to minimum and maximum values.
  //! The valid angle range is [0, 360], see Quantity_Color and Quantity_TOC_HLS for more details.
  void SetHueRange (const Standard_Real theMinAngle,
                    const Standard_Real theMaxAngle)
  {
    myColorHlsMin[0] = theMinAngle;
    myColorHlsMax[0] = theMaxAngle;
  }

  //! Returns color range corresponding to minimum and maximum values, blue to red by default.
  void ColorRange (Quantity_Color& theMinColor,
                   Quantity_Color& theMaxColor) const
  {
    theMinColor.SetValues (hueToValidRange (myColorHlsMin[0]), myColorHlsMin[1], myColorHlsMin[2], Quantity_TOC_HLS);
    theMaxColor.SetValues (hueToValidRange (myColorHlsMin[0]), myColorHlsMin[1], myColorHlsMin[2], Quantity_TOC_HLS);
  }

  //! Sets color range corresponding to minimum and maximum values.
  void SetColorRange (const Quantity_Color& theMinColor,
                      const Quantity_Color& theMaxColor)
  {
    theMinColor.Values (myColorHlsMin[0], myColorHlsMin[1], myColorHlsMin[2], Quantity_TOC_HLS);
    theMaxColor.Values (myColorHlsMax[0], myColorHlsMax[1], myColorHlsMax[2], Quantity_TOC_HLS);
  }

  //! Returns the type of labels, Aspect_TOCSD_AUTO by default.
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  Aspect_TypeOfColorScaleData GetLabelType() const { return myLabelType; }

  //! Sets the type of labels.
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  void SetLabelType (const Aspect_TypeOfColorScaleData theType) { myLabelType = theType; }

  //! Returns the type of colors, Aspect_TOCSD_AUTO by default.
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  Aspect_TypeOfColorScaleData GetColorType() const { return myColorType; }

  //! Sets the type of colors.
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  void SetColorType (const Aspect_TypeOfColorScaleData theType) { myColorType = theType; }

  //! Returns the number of color scale intervals, 10 by default.
  Standard_Integer GetNumberOfIntervals() const { return myNbIntervals; }

  //! Sets the number of color scale intervals.
  Standard_EXPORT void SetNumberOfIntervals (const Standard_Integer theNum);

  //! Returns the color scale title string, empty string by default.
  const TCollection_ExtendedString& GetTitle() const { return myTitle; }

  //! Sets the color scale title string.
  void SetTitle (const TCollection_ExtendedString& theTitle) { myTitle = theTitle; }

  //! Returns the format for numbers, "%.4g" by default.
  //! The same like format for function printf().
  //! Used if GetLabelType() is TOCSD_AUTO;
  const TCollection_AsciiString& GetFormat() const { return myFormat; }

  //! Returns the format of text.
  const TCollection_AsciiString& Format() const { return myFormat; }

  //! Sets the color scale auto label format specification.
  void SetFormat (const TCollection_AsciiString& theFormat) { myFormat = theFormat; }

  //! Returns the user specified label with index theIndex.
  //! Index is in range from 1 to GetNumberOfIntervals() or to
  //! GetNumberOfIntervals() + 1 if IsLabelAtBorder() is true.
  //! Returns empty string if label not defined.
  Standard_EXPORT TCollection_ExtendedString GetLabel (const Standard_Integer theIndex) const;

  //! Returns the user specified color from color map with index (starts at 1).
  //! Returns default color if index is out of range in color map.
  Standard_EXPORT Quantity_Color GetIntervalColor (const Standard_Integer theIndex) const;

  //! Sets the color of the specified interval. 
  //! Note that list is automatically resized to include specified index.
  //! @param theColor color value to set
  //! @param theIndex index in range [1, GetNumberOfIntervals()];
  //!                 appended to the end of list if -1 is specified
  Standard_EXPORT void SetIntervalColor (const Quantity_Color& theColor, const Standard_Integer theIndex);

  //! Returns the user specified labels.
  Standard_EXPORT void GetLabels (TColStd_SequenceOfExtendedString& theLabels) const;

  //! Returns the user specified labels.
  const TColStd_SequenceOfExtendedString& Labels() const { return myLabels; }

  //! Sets the color scale labels.
  //! The length of the sequence should be equal to GetNumberOfIntervals() or to GetNumberOfIntervals() + 1 if IsLabelAtBorder() is true.
  //! If length of the sequence does not much the number of intervals,
  //! then these labels will be considered as "free" and will be located
  //! at the virtual intervals corresponding to the number of labels
  //! (with flag IsLabelAtBorder() having the same effect as in normal case).
  Standard_EXPORT void SetLabels (const TColStd_SequenceOfExtendedString& theSeq);

  //! Returns the user specified colors.
  Standard_EXPORT void GetColors (Aspect_SequenceOfColor& theColors) const;

  //! Returns the user specified colors.
  const Aspect_SequenceOfColor& GetColors() const { return myColors; }

  //! Sets the color scale colors.
  //! The length of the sequence should be equal to GetNumberOfIntervals().
  Standard_EXPORT void SetColors (const Aspect_SequenceOfColor& theSeq);

  //! Returns the position of labels concerning color filled rectangles, Aspect_TOCSP_RIGHT by default.
  Aspect_TypeOfColorScalePosition GetLabelPosition() const { return myLabelPos; }

  //! Sets the color scale labels position relative to color bar.
  void SetLabelPosition (const Aspect_TypeOfColorScalePosition thePos) { myLabelPos = thePos; }

  //! Returns the position of color scale title, Aspect_TOCSP_LEFT by default.
  Aspect_TypeOfColorScalePosition GetTitlePosition() const { return myTitlePos; }

  //! Sets the color scale title position.
  Standard_DEPRECATED("AIS_ColorScale::SetTitlePosition() has no effect!")
  void SetTitlePosition (const Aspect_TypeOfColorScalePosition thePos) { myTitlePos = thePos; }

  //! Returns TRUE if the labels and colors used in reversed order, FALSE by default.
  //!  - Normal,   bottom-up order with Minimal value on the Bottom and Maximum value on Top.
  //!  - Reversed, top-down  order with Maximum value on the Bottom and Minimum value on Top.
  Standard_Boolean IsReversed() const { return myIsReversed; }

  //! Sets true if the labels and colors used in reversed order.
  void SetReversed (const Standard_Boolean theReverse) { myIsReversed = theReverse; }

  //! Return TRUE if color transition between neighbor intervals
  //! should be linearly interpolated, FALSE by default.
  Standard_Boolean IsSmoothTransition() const { return myIsSmooth; }

  //! Setup smooth color transition.
  void SetSmoothTransition (const Standard_Boolean theIsSmooth) { myIsSmooth = theIsSmooth; }

  //! Returns TRUE if the labels are placed at border of color intervals, TRUE by default.
  //! The automatically generated label will show value exactly on the current position:
  //!  - value connecting two neighbor intervals (TRUE)
  //!  - value in the middle of interval (FALSE)
  Standard_Boolean IsLabelAtBorder() const { return myIsLabelAtBorder; }

  //! Sets true if the labels are placed at border of color intervals (TRUE by default).
  //! If set to False, labels will be drawn at color intervals rather than at borders.
  void SetLabelAtBorder (const Standard_Boolean theOn) { myIsLabelAtBorder = theOn; }

  //! Returns TRUE if the color scale has logarithmic intervals, FALSE by default.
  Standard_Boolean IsLogarithmic() const { return myIsLogarithmic; }

  //! Sets true if the color scale has logarithmic intervals.
  void SetLogarithmic (const Standard_Boolean isLogarithmic) { myIsLogarithmic = isLogarithmic; }

  //! Sets the color scale label at index.
  //! Note that list is automatically resized to include specified index.
  //! @param theLabel new label text
  //! @param theIndex index in range [1, GetNumberOfIntervals()] or [1, GetNumberOfIntervals() + 1] if IsLabelAtBorder() is true;
  //!                 label is appended to the end of list if negative index is specified
  Standard_EXPORT void SetLabel (const TCollection_ExtendedString& theLabel, const Standard_Integer theIndex);

  //! Returns the size of color bar, 0 and 0 by default
  //! (e.g. should be set by user explicitly before displaying).
  void GetSize (Standard_Integer& theBreadth, Standard_Integer& theHeight) const
  {
    theBreadth = myBreadth;
    theHeight  = myHeight;
  }

  //! Sets the size of color bar.
  void SetSize (const Standard_Integer theBreadth, const Standard_Integer theHeight)
  {
    myBreadth = theBreadth;
    myHeight  = theHeight;
  }

  //! Returns the breadth of color bar, 0 by default
  //! (e.g. should be set by user explicitly before displaying).
  Standard_Integer GetBreadth() const { return myBreadth; }

  //! Sets the width of color bar.
  void SetBreadth (const Standard_Integer theBreadth) { myBreadth = theBreadth; }

  //! Returns the height of color bar, 0 by default
  //! (e.g. should be set by user explicitly before displaying).
  Standard_Integer GetHeight() const { return myHeight; }

  //! Sets the height of color bar.
  void SetHeight (const Standard_Integer theHeight) { myHeight  = theHeight; }

  //! Returns the bottom-left position of color scale, 0x0 by default.
  void GetPosition (Standard_Real& theX, Standard_Real& theY) const
  {
    theX = myXPos;
    theY = myYPos;
  }

  //! Sets the position of color scale.
  void SetPosition (const Standard_Integer theX, const Standard_Integer theY)
  {
    myXPos = theX;
    myYPos = theY;
  }

  //! Returns the left position of color scale, 0 by default.
  Standard_Integer GetXPosition() const { return myXPos; }

  //! Sets the left position of color scale.
  void SetXPosition (const Standard_Integer theX) { myXPos = theX; }

  //! Returns the bottom position of color scale, 0 by default.
  Standard_Integer GetYPosition() const { return myYPos; }

  //! Sets the bottom position of color scale.
  void SetYPosition (const Standard_Integer theY) { myYPos = theY; }

  //! Returns the font height of text labels, 20 by default.
  Standard_Integer GetTextHeight() const { return myTextHeight; }

  //! Sets the height of text of color scale.
  void SetTextHeight (const Standard_Integer theHeight) { myTextHeight = theHeight; }

public:

  //! Returns the width of text.
  //! @param theText [in] the text of which to calculate width.
  Standard_EXPORT Standard_Integer TextWidth (const TCollection_ExtendedString& theText) const;

  //! Returns the height of text.
  //! @param theText [in] the text of which to calculate height.
  Standard_EXPORT Standard_Integer TextHeight (const TCollection_ExtendedString& theText) const;

  Standard_EXPORT void TextSize (const TCollection_ExtendedString& theText,
                                 const Standard_Integer theHeight,
                                 Standard_Integer& theWidth,
                                 Standard_Integer& theAscent,
                                 Standard_Integer& theDescent) const;

public:

  //! Return true if specified display mode is supported.
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Compute presentation.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                        const Handle(Prs3d_Presentation)& thePresentation,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection - not implemented for color scale.
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                                 const Standard_Integer /*aMode*/) Standard_OVERRIDE {}

private:

  //! Returns the size of color scale.
  //! @param theWidth [out] the width of color scale.
  //! @param theHeight [out] the height of color scale.
  void SizeHint (Standard_Integer& theWidth, Standard_Integer& theHeight) const;

  //! Returns the upper value of given interval, or minimum for theIndex = 0.
  Standard_Real GetIntervalValue (const Standard_Integer theIndex) const;

  //! Returns the color for the given value in the given interval.
  //! @param theValue [in] the current value of interval
  //! @param theMin   [in] the min value of interval
  //! @param theMax   [in] the max value of interval
  Quantity_Color colorFromValue (const Standard_Real theValue,
                                 const Standard_Real theMin,
                                 const Standard_Real theMax) const;

  //! Initialize text aspect for drawing the labels.
  void updateTextAspect();

  //! Simple alias for Prs3d_Text::Draw().
  //! @param theGroup [in] presentation group
  //! @param theText  [in] text to draw
  //! @param theX     [in] X coordinate of text position
  //! @param theY     [in] Y coordinate of text position
  //! @param theVertAlignment [in] text vertical alignment
  void drawText (const Handle(Graphic3d_Group)& theGroup,
                 const TCollection_ExtendedString& theText,
                 const Standard_Integer theX, const Standard_Integer theY,
                 const Graphic3d_VerticalTextAlignment theVertAlignment);

  //! Determine the maximum text label width in pixels.
  Standard_Integer computeMaxLabelWidth (const TColStd_SequenceOfExtendedString& theLabels) const;

  //! Draw labels.
  void drawLabels (const Handle(Prs3d_Presentation)& thePrs,
                   const TColStd_SequenceOfExtendedString& theLabels,
                   const Standard_Integer theBarBottom,
                   const Standard_Integer theBarHeight,
                   const Standard_Integer theMaxLabelWidth,
                   const Standard_Integer theColorBreadth);

  //! Draw a color bar.
  void drawColorBar (const Handle(Prs3d_Presentation)& thePrs,
                     const Standard_Integer theBarBottom,
                     const Standard_Integer theBarHeight,
                     const Standard_Integer theMaxLabelWidth,
                     const Standard_Integer theColorBreadth);

  //! Draw a frame.
  //! @param theX [in] the X coordinate of frame position.
  //! @param theY [in] the Y coordinate of frame position.
  //! @param theWidth [in] the width of frame.
  //! @param theHeight [in] the height of frame.
  //! @param theColor [in] the color of frame.
  void drawFrame (const Handle(Prs3d_Presentation)& thePrs,
                  const Standard_Integer theX, const Standard_Integer theY,
                  const Standard_Integer theWidth, const Standard_Integer theHeight,
                  const Quantity_Color& theColor);

private:

  Standard_Real                    myMin;             //!< values range - minimal value
  Standard_Real                    myMax;             //!< values range - maximal value
  Graphic3d_Vec3d                  myColorHlsMin;     //!< HLS color corresponding to minimum value
  Graphic3d_Vec3d                  myColorHlsMax;     //!< HLS color corresponding to maximum value
  TCollection_ExtendedString       myTitle;           //!< optional title string     
  TCollection_AsciiString          myFormat;          //!< sprintf() format for generating label from value
  Standard_Integer                 myNbIntervals;     //!< number of intervals
  Aspect_TypeOfColorScaleData      myColorType;       //!< color type
  Aspect_TypeOfColorScaleData      myLabelType;       //!< label type
  Standard_Boolean                 myIsLabelAtBorder; //!< at border
  Standard_Boolean                 myIsReversed;      //!< flag indicating reversed order
  Standard_Boolean                 myIsLogarithmic;   //!< flag indicating logarithmic scale
  Standard_Boolean                 myIsSmooth;        //!< flag indicating smooth transition between the colors
  Aspect_SequenceOfColor           myColors;          //!< sequence of custom colors
  TColStd_SequenceOfExtendedString myLabels;          //!< sequence of custom text labels
  Aspect_TypeOfColorScalePosition  myLabelPos;        //!< label position relative to the color scale
  Aspect_TypeOfColorScalePosition  myTitlePos;        //!< title position
  Standard_Integer                 myXPos;            //!< left   position
  Standard_Integer                 myYPos;            //!< bottom position
  Standard_Integer                 myBreadth;         //!< color scale breadth
  Standard_Integer                 myHeight;          //!< height of the color scale
  Standard_Integer                 mySpacing;         //!< extra spacing between element
  Standard_Integer                 myTextHeight;      //!< label font height

};

#endif
