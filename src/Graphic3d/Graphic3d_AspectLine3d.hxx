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

#include <Aspect_AspectLineDefinitionError.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Standard.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <Quantity_ColorRGBA.hxx>

//! Creates and updates a group of attributes for 3d line primitives.
//! This group contains the color, the type of line, and its thickness.
class Graphic3d_AspectLine3d : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_AspectLine3d, Standard_Transient)
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
  //! The width of the line is determined by applying the line width scale factor to this nominal line width.
  //! The supported line widths vary by 1-pixel units.
  Standard_EXPORT Graphic3d_AspectLine3d (const Quantity_Color&   theColor,
                                          const Aspect_TypeOfLine theType,
                                          const Standard_Real     theWidth);

  //! Return color.
  const Quantity_ColorRGBA& ColorRGBA() const { return myColor; }

  //! Return color.
  const Quantity_Color& Color() const { return myColor.GetRGB(); }

  //! Modifies the color.
  void SetColor (const Quantity_Color& theColor) { myColor.SetRGB (theColor); }

  //! Return line type.
  Aspect_TypeOfLine Type() const { return myType; }

  //! Modifies the type of line.
  void SetType (const Aspect_TypeOfLine theType) { myType = theType; }

  //! Return line width.
  Standard_ShortReal Width() const { return myWidth; }

  //! Modifies the line thickness.
  //! Warning: Raises AspectLineDefinitionError if the width is a negative value.
  void SetWidth (const Standard_Real theWidth) { SetWidth ((float )theWidth); }

  //! Modifies the line thickness.
  //! Warning: Raises AspectLineDefinitionError if the width is a negative value.
  void SetWidth (const Standard_ShortReal theWidth)
  {
    if (theWidth <= 0.0f)
    {
      throw Aspect_AspectLineDefinitionError("Graphic3d_AspectLine3d, Bad value for LineWidth");
    }
    myWidth = theWidth;
  }

  //! Return shader program.
  const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const { return myProgram; }

  //! Sets up OpenGL/GLSL shader program.
  void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram) { myProgram = theProgram; }

  //! Check for equality with another line aspect.
  bool IsEqual (const Graphic3d_AspectLine3d& theOther)
  {
    if (this == &theOther)
    {
      return true;
    }

    return myProgram == theOther.myProgram
        && myType    == theOther.myType
        && myColor   == theOther.myColor
        && myWidth   == theOther.myWidth;
  }

public:

  //! Returns the current values of the group.
  Standard_DEPRECATED("Deprecated method Values() should be replaced by individual property getters")
  void Values (Quantity_Color&    theColor,
               Aspect_TypeOfLine& theType,
               Standard_Real&     theWidth) const
  {
    theColor = myColor.GetRGB();
    theType  = myType;
    theWidth = myWidth;
  }

protected:

  Handle(Graphic3d_ShaderProgram) myProgram;
  Quantity_ColorRGBA myColor;
  Aspect_TypeOfLine  myType;
  Standard_ShortReal myWidth;

};

DEFINE_STANDARD_HANDLE(Graphic3d_AspectLine3d, Standard_Transient)

#endif // _Graphic3d_AspectLine3d_HeaderFile
