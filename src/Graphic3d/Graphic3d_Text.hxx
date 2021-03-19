// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Text_HeaderFile
#define _Graphic3d_Text_HeaderFile

#include <gp_Ax2.hxx>

#include <Font_TextFormatter.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <NCollection_String.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

//! This class allows the definition of a text object for display.
//! The text might be defined in one of ways, using:
//! - text value and position,
//! - text value, orientation and the state whether the text uses position as point of attach.
//! - text formatter. Formatter contains text, height and alignment parameter.
//!
//! This class also has parameters of the text height and H/V alignments.
//! Custom formatting is available using Font_TextFormatter.
class Graphic3d_Text : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_Text, Standard_Transient)

public:

  //! Creates default text parameters.
  Standard_EXPORT Graphic3d_Text (const Standard_ShortReal theHeight);

  //! Destructor.
  virtual ~Graphic3d_Text() {}

  //! Returns text value.
  const NCollection_String& Text() const { return myText; }

  //! Sets text value.
  void SetText (const NCollection_String& theText) { myText = theText; }

  //! Sets text value.
  void SetText (const TCollection_AsciiString& theText) { myText = theText.ToCString(); }

  //! Sets text value.
  void SetText (Standard_CString theText) { myText = theText; }

  //! @return text formatter; NULL by default, which means standard text formatter will be used.
  const Handle(Font_TextFormatter)& TextFormatter() const { return myFormatter; }

  //! Setup text default formatter for text within this context.
  void SetTextFormatter (const Handle(Font_TextFormatter)& theFormatter) { myFormatter = theFormatter; }

  //! The 3D point of attachment is projected.
  //! If the orientation is defined, the text is written in the plane of projection.
  const gp_Pnt& Position() const { return myOrientation.Location(); }

  //! Sets text point.
  void SetPosition (const gp_Pnt& thePoint) { myOrientation.SetLocation (thePoint); }

  //! Returns text orientation in 3D space.
  const gp_Ax2& Orientation() const { return myOrientation; }

  //! Returns true if the text is filled by a point
  Standard_Boolean HasPlane() const { return myHasPlane; }

  //! Sets text orientation in 3D space.
  Standard_EXPORT void SetOrientation (const gp_Ax2& theOrientation);

  //! Reset text orientation in 3D space.
  Standard_EXPORT void ResetOrientation();

  //! Returns true if the text has an anchor point
  Standard_Boolean HasOwnAnchorPoint() const { return myHasOwnAnchor; }

  //! Returns true if the text has an anchor point
  void SetOwnAnchorPoint (const Standard_Boolean theHasOwnAnchor) { myHasOwnAnchor = theHasOwnAnchor; }

  //! Sets height of text. (Relative to the Normalized Projection Coordinates (NPC) Space).
  Standard_ShortReal Height() const { return myHeight; }

  //! Returns height of text
  void SetHeight (const Standard_ShortReal theHeight) { myHeight = theHeight; }

  //! Returns horizontal alignment of text.
  Graphic3d_HorizontalTextAlignment HorizontalAlignment() const { return myHAlign; }

  //! Sets horizontal alignment of text.
  void SetHorizontalAlignment (const Graphic3d_HorizontalTextAlignment theJustification) { myHAlign = theJustification; }

  //! Returns vertical alignment of text.
  Graphic3d_VerticalTextAlignment VerticalAlignment() const { return myVAlign; }

  //! Sets vertical alignment of text.
  void SetVerticalAlignment (const Graphic3d_VerticalTextAlignment theJustification) { myVAlign = theJustification; }

protected:
  Handle(Font_TextFormatter) myFormatter; //!< text formatter

  NCollection_String myText; //!< text value
  gp_Ax2 myOrientation; //!< Text orientation in 3D space.

  Standard_ShortReal myHeight; //!< height of text
  Graphic3d_HorizontalTextAlignment myHAlign; //!< horizontal alignment
  Graphic3d_VerticalTextAlignment myVAlign; //!< vertical alignment

  Standard_Boolean myHasPlane; //!< Check if text have orientation in 3D space.
  Standard_Boolean myHasOwnAnchor; //!< flag if text uses position as point of attach
};

DEFINE_STANDARD_HANDLE(Graphic3d_Text, Standard_Transient)

#endif // _Graphic3d_Text_HeaderFile
