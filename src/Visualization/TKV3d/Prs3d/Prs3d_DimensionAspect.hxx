// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Prs3d_DimensionAspect_HeaderFile
#define _Prs3d_DimensionAspect_HeaderFile

#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionArrowOrientation.hxx>
#include <Prs3d_DimensionTextHorizontalPosition.hxx>
#include <Prs3d_DimensionTextVerticalPosition.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <TCollection_AsciiString.hxx>

//! defines the attributes when drawing a Length Presentation.
class Prs3d_DimensionAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_DimensionAspect, Prs3d_BasicAspect)
public:
  //! Constructs an empty framework to define the display of dimensions.
  Standard_EXPORT Prs3d_DimensionAspect();

  //! Returns the settings for the display of lines used in presentation of dimensions.
  const Handle(Prs3d_LineAspect)& LineAspect() const { return myLineAspect; }

  //! Sets the display attributes of lines used in presentation of dimensions.
  void SetLineAspect(const Handle(Prs3d_LineAspect)& theAspect) { myLineAspect = theAspect; }

  //! Returns the settings for the display of text used in presentation of dimensions.
  const Handle(Prs3d_TextAspect)& TextAspect() const { return myTextAspect; }

  //! Sets the display attributes of text used in presentation of dimensions.
  void SetTextAspect(const Handle(Prs3d_TextAspect)& theAspect) { myTextAspect = theAspect; }

  //! Check if text for dimension label is 3d.
  Standard_Boolean IsText3d() const { return myIsText3d; }

  //! Sets type of text.
  void MakeText3d(const Standard_Boolean isText3d) { myIsText3d = isText3d; }

  //! Check if 3d text for dimension label is shaded.
  Standard_Boolean IsTextShaded() const { return myIsTextShaded; }

  //! Turns on/off text shading for 3d text.
  void MakeTextShaded(const Standard_Boolean theIsTextShaded) { myIsTextShaded = theIsTextShaded; }

  //! Gets type of arrows.
  Standard_Boolean IsArrows3d() const { return myIsArrows3d; }

  //! Sets type of arrows.
  void MakeArrows3d(const Standard_Boolean theIsArrows3d) { myIsArrows3d = theIsArrows3d; }

  //! Shows if Units are to be displayed along with dimension value.
  Standard_Boolean IsUnitsDisplayed() const { return myToDisplayUnits; }

  //! Specifies whether the units string should be displayed
  //! along with value label or not.
  void MakeUnitsDisplayed(const Standard_Boolean theIsDisplayed)
  {
    myToDisplayUnits = theIsDisplayed;
  }

  //! Sets orientation of arrows (external or internal).
  //! By default orientation is chosen automatically according to situation and text label size.
  void SetArrowOrientation(const Prs3d_DimensionArrowOrientation theArrowOrient)
  {
    myArrowOrientation = theArrowOrient;
  }

  //! Gets orientation of arrows (external or internal).
  Prs3d_DimensionArrowOrientation ArrowOrientation() const { return myArrowOrientation; }

  //! Sets vertical text alignment for text label.
  void SetTextVerticalPosition(const Prs3d_DimensionTextVerticalPosition thePosition)
  {
    myTextVPosition = thePosition;
  }

  //! Gets vertical text alignment for text label.
  Prs3d_DimensionTextVerticalPosition TextVerticalPosition() const { return myTextVPosition; }

  //! Sets horizontal text alignment for text label.
  void SetTextHorizontalPosition(const Prs3d_DimensionTextHorizontalPosition thePosition)
  {
    myTextHPosition = thePosition;
  }

  //! Gets horizontal text alignment for text label.
  Prs3d_DimensionTextHorizontalPosition TextHorizontalPosition() const { return myTextHPosition; }

  //! Returns the settings for displaying arrows.
  const Handle(Prs3d_ArrowAspect)& ArrowAspect() const { return myArrowAspect; }

  //! Sets the display attributes of arrows used in presentation of dimensions.
  void SetArrowAspect(const Handle(Prs3d_ArrowAspect)& theAspect) { myArrowAspect = theAspect; }

  //! Sets the same color for all parts of dimension: lines, arrows and text.
  Standard_EXPORT void SetCommonColor(const Quantity_Color& theColor);

  //! Sets extension size.
  void SetExtensionSize(const Standard_Real theSize) { myExtensionSize = theSize; }

  //! Returns extension size.
  Standard_Real ExtensionSize() const { return myExtensionSize; }

  //! Set size for arrow tail (extension without text).
  void SetArrowTailSize(const Standard_Real theSize) { myArrowTailSize = theSize; }

  //! Returns arrow tail size.
  Standard_Real ArrowTailSize() const { return myArrowTailSize; }

  //! Sets "sprintf"-syntax format for formatting dimension value labels.
  void SetValueStringFormat(const TCollection_AsciiString& theFormat)
  {
    myValueStringFormat = theFormat;
  }

  //! Returns format.
  const TCollection_AsciiString& ValueStringFormat() const { return myValueStringFormat; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  Handle(Prs3d_LineAspect)              myLineAspect;
  Handle(Prs3d_TextAspect)              myTextAspect;
  Handle(Prs3d_ArrowAspect)             myArrowAspect;
  TCollection_AsciiString               myValueStringFormat;
  Standard_Real                         myExtensionSize;
  Standard_Real                         myArrowTailSize;
  Prs3d_DimensionArrowOrientation       myArrowOrientation;
  Prs3d_DimensionTextHorizontalPosition myTextHPosition;
  Prs3d_DimensionTextVerticalPosition   myTextVPosition;
  Standard_Boolean                      myToDisplayUnits;
  Standard_Boolean                      myIsText3d;
  Standard_Boolean                      myIsTextShaded;
  Standard_Boolean                      myIsArrows3d;
};

DEFINE_STANDARD_HANDLE(Prs3d_DimensionAspect, Prs3d_BasicAspect)

#endif // _Prs3d_DimensionAspect_HeaderFile
