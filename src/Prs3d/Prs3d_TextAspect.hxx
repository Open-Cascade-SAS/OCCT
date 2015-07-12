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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_PlaneAngle.hxx>
#include <Standard_Real.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_TextPath.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_CString.hxx>
#include <Quantity_Length.hxx>
class Graphic3d_AspectText3d;
class Quantity_Color;


class Prs3d_TextAspect;
DEFINE_STANDARD_HANDLE(Prs3d_TextAspect, Prs3d_BasicAspect)

//! Defines the attributes when displaying a text.
class Prs3d_TextAspect : public Prs3d_BasicAspect
{

public:

  
  //! Constructs an empty framework for defining display attributes of text.
  Standard_EXPORT Prs3d_TextAspect();
  
  Standard_EXPORT Prs3d_TextAspect(const Handle(Graphic3d_AspectText3d)& theAspect);
  
  Standard_EXPORT void SetColor (const Quantity_Color& aColor);
  
  //! Sets the color of the type used in text display.
  Standard_EXPORT void SetColor (const Quantity_NameOfColor aColor);
  
  //! Sets the font used in text display.
  Standard_EXPORT void SetFont (const Standard_CString aFont);
  
  //! Returns the height-width ratio, also known as the expansion factor.
  Standard_EXPORT void SetHeightWidthRatio (const Standard_Real aRatio);
  
  //! Sets the length of the box which text will occupy.
  Standard_EXPORT void SetSpace (const Quantity_Length aSpace);
  
  //! Sets the height of the text.
  Standard_EXPORT void SetHeight (const Standard_Real aHeight);
  
  //! Sets the angle
  Standard_EXPORT void SetAngle (const Quantity_PlaneAngle anAngle);
  
  //! Returns the height of the text box.
  Standard_EXPORT Standard_Real Height() const;
  
  //! Returns the angle
  Standard_EXPORT Quantity_PlaneAngle Angle() const;
  
  //! Sets horizontal alignment of text.
  Standard_EXPORT void SetHorizontalJustification (const Graphic3d_HorizontalTextAlignment aJustification);
  
  //! Sets the vertical alignment of text.
  Standard_EXPORT void SetVerticalJustification (const Graphic3d_VerticalTextAlignment aJustification);
  
  //! Sets the orientation of text.
  Standard_EXPORT void SetOrientation (const Graphic3d_TextPath anOrientation);
  
  //! Returns the horizontal alignment of the text.
  //! The range of values includes:
  //! -   left
  //! -   center
  //! -   right, and
  //! -   normal (justified).
  Standard_EXPORT Graphic3d_HorizontalTextAlignment HorizontalJustification() const;
  
  //! Returns the vertical alignment of the text.
  //! The range of values includes:
  //! -   normal
  //! -   top
  //! -   cap
  //! -   half
  //! -   base
  //! -   bottom
  Standard_EXPORT Graphic3d_VerticalTextAlignment VerticalJustification() const;
  
  //! Returns the orientation of the text.
  //! Text can be displayed in the following directions:
  //! -   up
  //! -   down
  //! -   left, or
  //! -   right
  Standard_EXPORT Graphic3d_TextPath Orientation() const;
  
  //! Returns the purely textual attributes used in the display of text.
  //! These include:
  //! -   color
  //! -   font
  //! -   height/width ratio, that is, the expansion factor, and
  //! -   space between characters.
  Standard_EXPORT Handle(Graphic3d_AspectText3d) Aspect() const;
  
  Standard_EXPORT void SetAspect (const Handle(Graphic3d_AspectText3d)& theAspect);




  DEFINE_STANDARD_RTTI(Prs3d_TextAspect,Prs3d_BasicAspect)

protected:




private:


  Handle(Graphic3d_AspectText3d) myTextAspect;
  Quantity_PlaneAngle myAngle;
  Standard_Real myHeight;
  Graphic3d_HorizontalTextAlignment myHorizontalJustification;
  Graphic3d_VerticalTextAlignment myVerticalJustification;
  Graphic3d_TextPath myOrientation;


};







#endif // _Prs3d_TextAspect_HeaderFile
