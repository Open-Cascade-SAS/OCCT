// Created on: 1993-09-14
// Created by: Jean-Louis FRENKEL
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

#ifndef _Prs3d_TextAspect_HeaderFile
#define _Prs3d_TextAspect_HeaderFile

#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_TextPath.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Prs3d_BasicAspect.hxx>

//! Defines the attributes when displaying a text.
class Prs3d_TextAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_TextAspect, Prs3d_BasicAspect)
public:

  //! Constructs an empty framework for defining display attributes of text.
  Standard_EXPORT Prs3d_TextAspect();
  
  Standard_EXPORT Prs3d_TextAspect(const Handle(Graphic3d_AspectText3d)& theAspect);

  //! Sets the color of the type used in text display.
  void SetColor (const Quantity_Color& theColor) { myTextAspect->SetColor (theColor); }

  //! Sets the font used in text display.
  void SetFont (const Standard_CString theFont) { myTextAspect->SetFont (theFont); }
  
  //! Returns the height-width ratio, also known as the expansion factor.
  void SetHeightWidthRatio (const Standard_Real theRatio) { myTextAspect->SetExpansionFactor (theRatio); }

  //! Sets the length of the box which text will occupy.
  void SetSpace (const Standard_Real theSpace) { myTextAspect->SetSpace (theSpace); }
  
  //! Sets the height of the text.
  void SetHeight (const Standard_Real theHeight) { myHeight = theHeight; }
  
  //! Sets the angle
  void SetAngle (const Standard_Real theAngle) { myAngle = theAngle; }
  
  //! Returns the height of the text box.
  Standard_Real Height() const { return myHeight; }
  
  //! Returns the angle
  Standard_Real Angle() const { return myAngle; }
  
  //! Sets horizontal alignment of text.
  void SetHorizontalJustification (const Graphic3d_HorizontalTextAlignment theJustification) { myHorizontalJustification = theJustification; }
  
  //! Sets the vertical alignment of text.
  void SetVerticalJustification (const Graphic3d_VerticalTextAlignment theJustification) { myVerticalJustification = theJustification; }
  
  //! Sets the orientation of text.
  void SetOrientation (const Graphic3d_TextPath theOrientation) { myOrientation = theOrientation; }
  
  //! Returns the horizontal alignment of the text.
  //! The range of values includes:
  //! -   left
  //! -   center
  //! -   right, and
  //! -   normal (justified).
  Graphic3d_HorizontalTextAlignment HorizontalJustification() const { return myHorizontalJustification; }
  
  //! Returns the vertical alignment of the text.
  //! The range of values includes:
  //! -   normal
  //! -   top
  //! -   cap
  //! -   half
  //! -   base
  //! -   bottom
  Graphic3d_VerticalTextAlignment VerticalJustification() const { return myVerticalJustification; }
  
  //! Returns the orientation of the text.
  //! Text can be displayed in the following directions:
  //! -   up
  //! -   down
  //! -   left, or
  //! -   right
  Graphic3d_TextPath Orientation() const { return myOrientation; }

  //! Returns the purely textual attributes used in the display of text.
  //! These include:
  //! -   color
  //! -   font
  //! -   height/width ratio, that is, the expansion factor, and
  //! -   space between characters.
  const Handle(Graphic3d_AspectText3d)& Aspect() const { return myTextAspect; }

  void SetAspect (const Handle(Graphic3d_AspectText3d)& theAspect) { myTextAspect = theAspect; }

protected:

  Handle(Graphic3d_AspectText3d) myTextAspect;
  Standard_Real myAngle;
  Standard_Real myHeight;
  Graphic3d_HorizontalTextAlignment myHorizontalJustification;
  Graphic3d_VerticalTextAlignment myVerticalJustification;
  Graphic3d_TextPath myOrientation;

};

DEFINE_STANDARD_HANDLE(Prs3d_TextAspect, Prs3d_BasicAspect)

#endif // _Prs3d_TextAspect_HeaderFile
