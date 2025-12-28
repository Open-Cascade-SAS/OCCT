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
#include <Quantity_Color.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <TCollection_ExtendedString.hxx>

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
  Standard_EXPORT static bool FindColor(const double                    theValue,
                                        const double                    theMin,
                                        const double                    theMax,
                                        const int                       theColorsCount,
                                        const NCollection_Vec3<double>& theColorHlsMin,
                                        const NCollection_Vec3<double>& theColorHlsMax,
                                        Quantity_Color&                 theColor);

  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  static bool FindColor(const double    theValue,
                        const double    theMin,
                        const double    theMax,
                        const int       theColorsCount,
                        Quantity_Color& theColor)
  {
    return FindColor(theValue,
                     theMin,
                     theMax,
                     theColorsCount,
                     NCollection_Vec3<double>(230.0, 1.0, 1.0),
                     NCollection_Vec3<double>(0.0, 1.0, 1.0),
                     theColor);
  }

  //! Shift hue into valid range.
  //! Lightness and Saturation should be specified in valid range [0.0, 1.0],
  //! however Hue might be given out of Quantity_Color range to specify desired range for
  //! interpolation.
  static double hueToValidRange(const double theHue)
  {
    double aHue = theHue;
    while (aHue < 0.0)
    {
      aHue += 360.0;
    }
    while (aHue > 360.0)
    {
      aHue -= 360.0;
    }
    return aHue;
  }

public:
  //! Default constructor.
  Standard_EXPORT AIS_ColorScale();

  //! Calculate color according passed value; returns true if value is in range or false, if isn't
  Standard_EXPORT bool FindColor(const double theValue, Quantity_Color& theColor) const;

  //! Returns minimal value of color scale, 0.0 by default.
  double GetMin() const { return myMin; }

  //! Sets the minimal value of color scale.
  void SetMin(const double theMin) { SetRange(theMin, GetMax()); }

  //! Returns maximal value of color scale, 1.0 by default.
  double GetMax() const { return myMax; }

  //! Sets the maximal value of color scale.
  void SetMax(const double theMax) { SetRange(GetMin(), theMax); }

  //! Returns minimal and maximal values of color scale, 0.0 to 1.0 by default.
  void GetRange(double& theMin, double& theMax) const
  {
    theMin = myMin;
    theMax = myMax;
  }

  //! Sets the minimal and maximal value of color scale.
  //! Note that values order will be ignored - the minimum and maximum values will be swapped if
  //! needed.
  //! ::SetReversed() should be called to swap displaying order.
  Standard_EXPORT void SetRange(const double theMin, const double theMax);

  //! Returns the hue angle corresponding to minimum value, 230 by default (blue).
  double HueMin() const { return myColorHlsMin[0]; }

  //! Returns the hue angle corresponding to maximum value, 0 by default (red).
  double HueMax() const { return myColorHlsMax[0]; }

  //! Returns the hue angle range corresponding to minimum and maximum values, 230 to 0 by default
  //! (blue to red).
  void HueRange(double& theMinAngle, double& theMaxAngle) const
  {
    theMinAngle = myColorHlsMin[0];
    theMaxAngle = myColorHlsMax[0];
  }

  //! Sets hue angle range corresponding to minimum and maximum values.
  //! The valid angle range is [0, 360], see Quantity_Color and Quantity_TOC_HLS for more details.
  void SetHueRange(const double theMinAngle, const double theMaxAngle)
  {
    myColorHlsMin[0] = theMinAngle;
    myColorHlsMax[0] = theMaxAngle;
  }

  //! Returns color range corresponding to minimum and maximum values, blue to red by default.
  void ColorRange(Quantity_Color& theMinColor, Quantity_Color& theMaxColor) const
  {
    theMinColor.SetValues(hueToValidRange(myColorHlsMin[0]),
                          myColorHlsMin[1],
                          myColorHlsMin[2],
                          Quantity_TOC_HLS);
    theMaxColor.SetValues(hueToValidRange(myColorHlsMax[0]),
                          myColorHlsMax[1],
                          myColorHlsMax[2],
                          Quantity_TOC_HLS);
  }

  //! Sets color range corresponding to minimum and maximum values.
  void SetColorRange(const Quantity_Color& theMinColor, const Quantity_Color& theMaxColor)
  {
    theMinColor.Values(myColorHlsMin[0], myColorHlsMin[1], myColorHlsMin[2], Quantity_TOC_HLS);
    theMaxColor.Values(myColorHlsMax[0], myColorHlsMax[1], myColorHlsMax[2], Quantity_TOC_HLS);
  }

  //! Returns the type of labels, Aspect_TOCSD_AUTO by default.
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  Aspect_TypeOfColorScaleData GetLabelType() const { return myLabelType; }

  //! Sets the type of labels.
  //! Aspect_TOCSD_AUTO - labels as boundary values for intervals
  //! Aspect_TOCSD_USER - user specified label is used
  void SetLabelType(const Aspect_TypeOfColorScaleData theType) { myLabelType = theType; }

  //! Returns the type of colors, Aspect_TOCSD_AUTO by default.
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  Aspect_TypeOfColorScaleData GetColorType() const { return myColorType; }

  //! Sets the type of colors.
  //! Aspect_TOCSD_AUTO - value between Red and Blue
  //! Aspect_TOCSD_USER - user specified color from color map
  void SetColorType(const Aspect_TypeOfColorScaleData theType) { myColorType = theType; }

  //! Returns the number of color scale intervals, 10 by default.
  int GetNumberOfIntervals() const { return myNbIntervals; }

  //! Sets the number of color scale intervals.
  Standard_EXPORT void SetNumberOfIntervals(const int theNum);

  //! Returns the color scale title string, empty string by default.
  const TCollection_ExtendedString& GetTitle() const { return myTitle; }

  //! Sets the color scale title string.
  void SetTitle(const TCollection_ExtendedString& theTitle) { myTitle = theTitle; }

  //! Returns the format for numbers, "%.4g" by default.
  //! The same like format for function printf().
  //! Used if GetLabelType() is TOCSD_AUTO;
  const TCollection_AsciiString& GetFormat() const { return myFormat; }

  //! Returns the format of text.
  const TCollection_AsciiString& Format() const { return myFormat; }

  //! Sets the color scale auto label format specification.
  void SetFormat(const TCollection_AsciiString& theFormat) { myFormat = theFormat; }

  //! Returns the user specified label with index theIndex.
  //! Index is in range from 1 to GetNumberOfIntervals() or to
  //! GetNumberOfIntervals() + 1 if IsLabelAtBorder() is true.
  //! Returns empty string if label not defined.
  Standard_EXPORT TCollection_ExtendedString GetLabel(const int theIndex) const;

  //! Returns the user specified color from color map with index (starts at 1).
  //! Returns default color if index is out of range in color map.
  Standard_EXPORT Quantity_Color GetIntervalColor(const int theIndex) const;

  //! Sets the color of the specified interval.
  //! Note that list is automatically resized to include specified index.
  //! @param theColor color value to set
  //! @param theIndex index in range [1, GetNumberOfIntervals()];
  //!                 appended to the end of list if -1 is specified
  Standard_EXPORT void SetIntervalColor(const Quantity_Color& theColor, const int theIndex);

  //! Returns the user specified labels.
  Standard_EXPORT void GetLabels(NCollection_Sequence<TCollection_ExtendedString>& theLabels) const;

  //! Returns the user specified labels.
  const NCollection_Sequence<TCollection_ExtendedString>& Labels() const { return myLabels; }

  //! Sets the color scale labels.
  //! The length of the sequence should be equal to GetNumberOfIntervals() or to
  //! GetNumberOfIntervals() + 1 if IsLabelAtBorder() is true. If length of the sequence does not
  //! much the number of intervals, then these labels will be considered as "free" and will be
  //! located at the virtual intervals corresponding to the number of labels (with flag
  //! IsLabelAtBorder() having the same effect as in normal case).
  Standard_EXPORT void SetLabels(const NCollection_Sequence<TCollection_ExtendedString>& theSeq);

  //! Returns the user specified colors.
  Standard_EXPORT void GetColors(NCollection_Sequence<Quantity_Color>& theColors) const;

  //! Returns the user specified colors.
  const NCollection_Sequence<Quantity_Color>& GetColors() const { return myColors; }

  //! Sets the color scale colors.
  //! The length of the sequence should be equal to GetNumberOfIntervals().
  Standard_EXPORT void SetColors(const NCollection_Sequence<Quantity_Color>& theSeq);

  //! Populates colors scale by colors of the same lightness value in CIE Lch
  //! color space, distributed by hue, with perceptually uniform differences
  //! between consequent colors.
  //! See MakeUniformColors() for description of parameters.
  void SetUniformColors(double theLightness, double theHueFrom, double theHueTo)
  {
    SetColors(MakeUniformColors(myNbIntervals, theLightness, theHueFrom, theHueTo));
    SetColorType(Aspect_TOCSD_USER);
  }

  //! Generates sequence of colors of the same lightness value in CIE Lch
  //! color space (see #Quantity_TOC_CIELch), with hue values in the specified range.
  //! The colors are distributed across the range such as to have perceptually
  //! same difference between neighbour colors.
  //! For each color, maximal chroma value fitting in sRGB gamut is used.
  //!
  //! @param theNbColors - number of colors to generate
  //! @param theLightness - lightness to be used (0 is black, 100 is white, 32 is
  //!        lightness of pure blue)
  //! @param theHueFrom - hue value at the start of the scale
  //! @param theHueTo - hue value defining the end of the scale
  //!
  //! Hue value can be out of the range [0, 360], interpreted as modulo 360.
  //! The colors of the scale will be in the order of increasing hue if
  //! theHueTo > theHueFrom, and decreasing otherwise.
  Standard_EXPORT static NCollection_Sequence<Quantity_Color> MakeUniformColors(int    theNbColors,
                                                                                double theLightness,
                                                                                double theHueFrom,
                                                                                double theHueTo);

  //! Returns the position of labels concerning color filled rectangles, Aspect_TOCSP_RIGHT by
  //! default.
  Aspect_TypeOfColorScalePosition GetLabelPosition() const { return myLabelPos; }

  //! Sets the color scale labels position relative to color bar.
  void SetLabelPosition(const Aspect_TypeOfColorScalePosition thePos) { myLabelPos = thePos; }

  //! Returns the position of color scale title, Aspect_TOCSP_LEFT by default.
  Aspect_TypeOfColorScalePosition GetTitlePosition() const { return myTitlePos; }

  //! Sets the color scale title position.
  Standard_DEPRECATED("AIS_ColorScale::SetTitlePosition() has no effect!")
  void SetTitlePosition(const Aspect_TypeOfColorScalePosition thePos) { myTitlePos = thePos; }

  //! Returns TRUE if the labels and colors used in reversed order, FALSE by default.
  //!  - Normal,   bottom-up order with Minimal value on the Bottom and Maximum value on Top.
  //!  - Reversed, top-down  order with Maximum value on the Bottom and Minimum value on Top.
  bool IsReversed() const { return myIsReversed; }

  //! Sets true if the labels and colors used in reversed order.
  void SetReversed(const bool theReverse) { myIsReversed = theReverse; }

  //! Return TRUE if color transition between neighbor intervals
  //! should be linearly interpolated, FALSE by default.
  bool IsSmoothTransition() const { return myIsSmooth; }

  //! Setup smooth color transition.
  void SetSmoothTransition(const bool theIsSmooth) { myIsSmooth = theIsSmooth; }

  //! Returns TRUE if the labels are placed at border of color intervals, TRUE by default.
  //! The automatically generated label will show value exactly on the current position:
  //!  - value connecting two neighbor intervals (TRUE)
  //!  - value in the middle of interval (FALSE)
  bool IsLabelAtBorder() const { return myIsLabelAtBorder; }

  //! Sets true if the labels are placed at border of color intervals (TRUE by default).
  //! If set to False, labels will be drawn at color intervals rather than at borders.
  void SetLabelAtBorder(const bool theOn) { myIsLabelAtBorder = theOn; }

  //! Returns TRUE if the color scale has logarithmic intervals, FALSE by default.
  bool IsLogarithmic() const { return myIsLogarithmic; }

  //! Sets true if the color scale has logarithmic intervals.
  void SetLogarithmic(const bool isLogarithmic) { myIsLogarithmic = isLogarithmic; }

  //! Sets the color scale label at index.
  //! Note that list is automatically resized to include specified index.
  //! @param theLabel new label text
  //! @param theIndex index in range [1, GetNumberOfIntervals()] or [1, GetNumberOfIntervals() + 1]
  //! if IsLabelAtBorder() is true;
  //!                 label is appended to the end of list if negative index is specified
  Standard_EXPORT void SetLabel(const TCollection_ExtendedString& theLabel, const int theIndex);

  //! Returns the size of color bar, 0 and 0 by default
  //! (e.g. should be set by user explicitly before displaying).
  void GetSize(int& theBreadth, int& theHeight) const
  {
    theBreadth = myBreadth;
    theHeight  = myHeight;
  }

  //! Sets the size of color bar.
  void SetSize(const int theBreadth, const int theHeight)
  {
    myBreadth = theBreadth;
    myHeight  = theHeight;
  }

  //! Returns the breadth of color bar, 0 by default
  //! (e.g. should be set by user explicitly before displaying).
  int GetBreadth() const { return myBreadth; }

  //! Sets the width of color bar.
  void SetBreadth(const int theBreadth) { myBreadth = theBreadth; }

  //! Returns the height of color bar, 0 by default
  //! (e.g. should be set by user explicitly before displaying).
  int GetHeight() const { return myHeight; }

  //! Sets the height of color bar.
  void SetHeight(const int theHeight) { myHeight = theHeight; }

  //! Returns the bottom-left position of color scale, 0x0 by default.
  void GetPosition(double& theX, double& theY) const
  {
    theX = myXPos;
    theY = myYPos;
  }

  //! Sets the position of color scale.
  void SetPosition(const int theX, const int theY)
  {
    myXPos = theX;
    myYPos = theY;
  }

  //! Returns the left position of color scale, 0 by default.
  int GetXPosition() const { return myXPos; }

  //! Sets the left position of color scale.
  void SetXPosition(const int theX) { myXPos = theX; }

  //! Returns the bottom position of color scale, 0 by default.
  int GetYPosition() const { return myYPos; }

  //! Sets the bottom position of color scale.
  void SetYPosition(const int theY) { myYPos = theY; }

  //! Returns the font height of text labels, 20 by default.
  int GetTextHeight() const { return myTextHeight; }

  //! Sets the height of text of color scale.
  void SetTextHeight(const int theHeight) { myTextHeight = theHeight; }

public:
  //! Returns the width of text.
  //! @param[in] theText  the text of which to calculate width.
  Standard_EXPORT int TextWidth(const TCollection_ExtendedString& theText) const;

  //! Returns the height of text.
  //! @param[in] theText  the text of which to calculate height.
  Standard_EXPORT int TextHeight(const TCollection_ExtendedString& theText) const;

  Standard_EXPORT void TextSize(const TCollection_ExtendedString& theText,
                                const int                         theHeight,
                                int&                              theWidth,
                                int&                              theAscent,
                                int&                              theDescent) const;

public:
  //! Return true if specified display mode is supported.
  virtual bool AcceptDisplayMode(const int theMode) const override { return theMode == 0; }

  //! Compute presentation.
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>& thePresentation,
                                       const int                              theMode) override;

  //! Compute selection - not implemented for color scale.
  virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& /*aSelection*/,
                                const int /*aMode*/) override
  {
  }

private:
  //! Returns the size of color scale.
  //! @param[out] theWidth  the width of color scale.
  //! @param[out] theHeight  the height of color scale.
  void SizeHint(int& theWidth, int& theHeight) const;

  //! Returns the upper value of given interval, or minimum for theIndex = 0.
  double GetIntervalValue(const int theIndex) const;

  //! Returns the color for the given value in the given interval.
  //! @param[in] theValue  the current value of interval
  //! @param[in] theMin    the min value of interval
  //! @param[in] theMax    the max value of interval
  Quantity_Color colorFromValue(const double theValue,
                                const double theMin,
                                const double theMax) const;

  //! Initialize text aspect for drawing the labels.
  void updateTextAspect();

  //! Simple alias for Prs3d_Text::Draw().
  //! @param[in] theGroup  presentation group
  //! @param[in] theText   text to draw
  //! @param[in] theX      X coordinate of text position
  //! @param[in] theY      Y coordinate of text position
  //! @param[in] theVertAlignment  text vertical alignment
  void drawText(const occ::handle<Graphic3d_Group>&   theGroup,
                const TCollection_ExtendedString&     theText,
                const int                             theX,
                const int                             theY,
                const Graphic3d_VerticalTextAlignment theVertAlignment);

  //! Determine the maximum text label width in pixels.
  int computeMaxLabelWidth(const NCollection_Sequence<TCollection_ExtendedString>& theLabels) const;

  //! Draw labels.
  void drawLabels(const occ::handle<Graphic3d_Group>&                     theGroup,
                  const NCollection_Sequence<TCollection_ExtendedString>& theLabels,
                  const int                                               theBarBottom,
                  const int                                               theBarHeight,
                  const int                                               theMaxLabelWidth,
                  const int                                               theColorBreadth);

  //! Draw a color bar.
  void drawColorBar(const occ::handle<Prs3d_Presentation>& thePrs,
                    const int                              theBarBottom,
                    const int                              theBarHeight,
                    const int                              theMaxLabelWidth,
                    const int                              theColorBreadth);

  //! Draw a frame.
  //! @param[in] theX  the X coordinate of frame position.
  //! @param[in] theY  the Y coordinate of frame position.
  //! @param[in] theWidth  the width of frame.
  //! @param[in] theHeight  the height of frame.
  //! @param[in] theColor  the color of frame.
  void drawFrame(const occ::handle<Prs3d_Presentation>& thePrs,
                 const int                              theX,
                 const int                              theY,
                 const int                              theWidth,
                 const int                              theHeight,
                 const Quantity_Color&                  theColor);

private:
  double myMin;                                 //!< values range - minimal value
  double myMax;                                 //!< values range - maximal value
                                                // clang-format off
  NCollection_Vec3<double>                  myColorHlsMin;     //!< HLS color corresponding to minimum value
  NCollection_Vec3<double>                  myColorHlsMax;     //!< HLS color corresponding to maximum value
  TCollection_ExtendedString       myTitle;           //!< optional title string     
  TCollection_AsciiString          myFormat;          //!< Sprintf() format for generating label from value
  int                 myNbIntervals;     //!< number of intervals
  Aspect_TypeOfColorScaleData      myColorType;       //!< color type
  Aspect_TypeOfColorScaleData      myLabelType;       //!< label type
  bool                 myIsLabelAtBorder; //!< at border
  bool                 myIsReversed;      //!< flag indicating reversed order
  bool                 myIsLogarithmic;   //!< flag indicating logarithmic scale
  bool                 myIsSmooth;        //!< flag indicating smooth transition between the colors
  NCollection_Sequence<Quantity_Color>           myColors;          //!< sequence of custom colors
  NCollection_Sequence<TCollection_ExtendedString> myLabels;          //!< sequence of custom text labels
  Aspect_TypeOfColorScalePosition  myLabelPos;        //!< label position relative to the color scale
                                                // clang-format on
  Aspect_TypeOfColorScalePosition myTitlePos;   //!< title position
  int                             myXPos;       //!< left   position
  int                             myYPos;       //!< bottom position
  int                             myBreadth;    //!< color scale breadth
  int                             myHeight;     //!< height of the color scale
  int                             mySpacing;    //!< extra spacing between element
  int                             myTextHeight; //!< label font height
};

#endif
