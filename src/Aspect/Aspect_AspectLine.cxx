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
//		of line tracing
//-Warning	A context of line tracing is defined by :
//		- the color
//		- the type of line
//		- the thickness
//-References	
//-Language	C++ 2.0
//-Declarations
// for the class

#include <Aspect_AspectLine.hxx>
#include <Aspect_AspectLineDefinitionError.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_AspectLine,MMgt_TShared)

//-Aliases
//-Global data definitions
//	-- color
//	MyColor	:	Color;
//	-- type of line
//	MyType	:	TypeOfLine;
//	-- thickness
//	MyWidth	:	Standard_Real;
//-Constructors
//-Destructors
//-Methods, in order
Aspect_AspectLine::Aspect_AspectLine () {

	MyColor	= Quantity_NOC_YELLOW;
	MyType	= Aspect_TOL_SOLID;
	MyWidth	= 1.0;

}

Aspect_AspectLine::Aspect_AspectLine (const Quantity_Color& AColor, const Aspect_TypeOfLine AType, const Standard_Real AWidth) {

	if (AWidth <= 0.0)
		Aspect_AspectLineDefinitionError::Raise
			("Bad value for LineWidth");

	MyColor	= AColor;
	MyType	= AType;
	MyWidth	= AWidth;

}

void Aspect_AspectLine::SetColor (const Quantity_Color& AColor) {

	MyColor	= AColor;

}

void Aspect_AspectLine::SetType (const Aspect_TypeOfLine AType) {

	MyType	= AType;

}

void Aspect_AspectLine::SetWidth (const Standard_Real AWidth) {

	if (AWidth <= 0.0)
		Aspect_AspectLineDefinitionError::Raise
			("Bad value for LineWidth");

	MyWidth	= AWidth;

}

void Aspect_AspectLine::Values (Quantity_Color& AColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const {

	AColor	= MyColor;
	AType	= MyType;
	AWidth	= MyWidth;

}
