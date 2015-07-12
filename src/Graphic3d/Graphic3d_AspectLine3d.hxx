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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Graphic3d_ShaderProgram.hxx>
#include <Aspect_AspectLine.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Standard_Real.hxx>
class Quantity_Color;


class Graphic3d_AspectLine3d;
DEFINE_STANDARD_HANDLE(Graphic3d_AspectLine3d, Aspect_AspectLine)

//! Creates and updates a group of attributes
//! for 3d line primitives. This group contains the
//! colour, the type of line, and its thickness.
class Graphic3d_AspectLine3d : public Aspect_AspectLine
{

public:

  
  //! Creates a context table for line primitives
  //! defined with the following default values:
  //!
  //! Colour	: NOC_YELLOW
  //! Line type	: TOL_SOLID
  //! Width	: 1.0
  Standard_EXPORT Graphic3d_AspectLine3d();
  
  //! Creates a context table for line primitives
  //! defined with the specified values.
  //! Warning: <AWidth> is the "linewidth scale factor".
  //! The nominal line width is 1 pixel. The width of
  //! the line is determined by applying the linewidth scale
  //! factor to this nominal line width.
  //! The supported linewidths vary by 1-pixel units.
  Standard_EXPORT Graphic3d_AspectLine3d(const Quantity_Color& AColor, const Aspect_TypeOfLine AType, const Standard_Real AWidth);
  
  //! Sets up OpenGL/GLSL shader program.
  Standard_EXPORT void SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram);
  
  Standard_EXPORT const Handle(Graphic3d_ShaderProgram)& ShaderProgram() const;




  DEFINE_STANDARD_RTTI(Graphic3d_AspectLine3d,Aspect_AspectLine)

protected:




private:


  Handle(Graphic3d_ShaderProgram) MyShaderProgram;


};







#endif // _Graphic3d_AspectLine3d_HeaderFile
