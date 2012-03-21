// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.




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
#include <Aspect_AspectLine.ixx>

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
