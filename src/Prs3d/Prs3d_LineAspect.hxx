// Created on: 1993-04-26
// Created by: Jean-Louis Frenkel
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

#ifndef _Prs3d_LineAspect_HeaderFile
#define _Prs3d_LineAspect_HeaderFile

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Quantity_Color.hxx>

//! A framework for defining how a line will be displayed
//! in a presentation. Aspects of line display include
//! width, color and type of line.
//! The definition set by this class is then passed to the
//! attribute manager Prs3d_Drawer.
//! Any object which requires a value for line aspect as
//! an argument may then be given the attribute manager
//! as a substitute argument in the form of a field such as myDrawer for example.
class Prs3d_LineAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_LineAspect, Prs3d_BasicAspect)
public:
  
  //! Constructs a framework for line aspect defined by
  //! -   the color aColor
  //! -   the type of line aType and
  //! -   the line thickness aWidth.
  //! Type of line refers to whether the line is solid or dotted, for example.
  Standard_EXPORT Prs3d_LineAspect (const Quantity_Color& theColor, const Aspect_TypeOfLine theType, const Standard_Real theWidth);
  
  Prs3d_LineAspect(const Handle(Graphic3d_AspectLine3d)& theAspect) : myAspect (theAspect) {}

  //! Sets the line color defined at the time of construction.
  //! Default value: Quantity_NOC_YELLOW
  void SetColor (const Quantity_Color& theColor) { myAspect->SetColor (theColor); }

  //! Sets the type of line defined at the time of construction.
  //! This could, for example, be solid, dotted or made up of dashes.
  //! Default value: Aspect_TOL_SOLID
  void SetTypeOfLine (const Aspect_TypeOfLine theType) { myAspect->SetType (theType); }
  
  //! Sets the line width defined at the time of construction.
  //! Default value: 1.
  void SetWidth (const Standard_Real theWidth) { myAspect->SetWidth (theWidth); }

  //! Returns the line aspect. This is defined as the set of
  //! color, type and thickness attributes.
  const Handle(Graphic3d_AspectLine3d)& Aspect() const { return myAspect; }
  
  void SetAspect (const Handle(Graphic3d_AspectLine3d)& theAspect) { myAspect = theAspect; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  Handle(Graphic3d_AspectLine3d) myAspect;

};

DEFINE_STANDARD_HANDLE(Prs3d_LineAspect, Prs3d_BasicAspect)

#endif // _Prs3d_LineAspect_HeaderFile
