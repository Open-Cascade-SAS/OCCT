// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Graphic3d_AspectLine3d_HeaderFile
#define _Graphic3d_AspectLine3d_HeaderFile

#include <Graphic3d_Aspects.hxx>

//! Creates and updates a group of attributes for 3d line primitives.
//! This group contains the color, the type of line, and its thickness.
class Graphic3d_AspectLine3d : public Graphic3d_Aspects
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectLine3d, Graphic3d_Aspects)
public:
  //! Creates a context table for line primitives
  //! defined with the following default values:
  //!
  //! Color = Quantity_NOC_YELLOW;
  //! Type  = Aspect_TOL_SOLID;
  //! Width = 1.0;
  Standard_EXPORT Graphic3d_AspectLine3d();

  //! Creates a context table for line primitives defined with the specified values.
  //! Warning: theWidth is the "line width scale factor".
  //! The nominal line width is 1 pixel.
  //! The width of the line is determined by applying the line width scale factor to this nominal
  //! line width. The supported line widths vary by 1-pixel units.
  Standard_EXPORT Graphic3d_AspectLine3d(const Quantity_Color& theColor,
                                         Aspect_TypeOfLine     theType,
                                         Standard_Real         theWidth);

  //! Return line type.
  Aspect_TypeOfLine Type() const { return myLineType; }

  //! Modifies the type of line.
  void SetType(const Aspect_TypeOfLine theType) { SetLineType(theType); }

  //! Return line width.
  Standard_ShortReal Width() const { return myLineWidth; }

  //! Modifies the line thickness.
  //! Warning: Raises Standard_OutOfRange if the width is a negative value.
  void SetWidth(const Standard_Real theWidth) { SetWidth((float)theWidth); }

  //! Modifies the line thickness.
  //! Warning: Raises Standard_OutOfRange if the width is a negative value.
  void SetWidth(Standard_ShortReal theWidth) { SetLineWidth(theWidth); }
};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectLine3d, Graphic3d_Aspects)

#endif // _Graphic3d_AspectLine3d_HeaderFile
