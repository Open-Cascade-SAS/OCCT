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

//-Version	
//-Design	Declaration of variables specific to the context
//		of tracing of lines 3d
//-Warning	Context of tracing of lines 3d inherits the context
//		defined by :
//		- the color
//		- the type of trait
//		- the thickness
//-References	
//-Language	C++ 2.0
//-Declarations
// for the class

#include <Graphic3d_AspectLine3d.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_AspectLine3d,Aspect_AspectLine)

//-Aliases
//-Global data definitions
//-Constructors
//-Destructors
//-Methods, in order
Graphic3d_AspectLine3d::Graphic3d_AspectLine3d () {
}

// (AColor, AType, AWidth)
// because  AspectLine3d inherits AspectLine and it is necessary to call
// initialisation of AspectLine with AColor, AType, AWidth.

Graphic3d_AspectLine3d::Graphic3d_AspectLine3d (const Quantity_Color& AColor, const Aspect_TypeOfLine AType, const Standard_Real AWidth):Aspect_AspectLine (AColor, AType, AWidth) {}

void Graphic3d_AspectLine3d::SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram)
{
  MyShaderProgram = theProgram;
}

const Handle(Graphic3d_ShaderProgram)& Graphic3d_AspectLine3d::ShaderProgram() const
{
  return MyShaderProgram;
}
