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
//		of tracing of markers

//-Warning	Context of tracing of markers is defined by :
//		- the color
//		- the type
//		- the scale

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_AspectMarker.ixx>

//-Aliases

//-Global data definitions

//	-- color
//	MyColor	:	Color;

//	-- type
//	MyType	:	TypeOfMarker;

//	-- scale
//	MyScale	:	Standard_Real;

//-Constructors

//-Destructors

//-Methods, in order

Aspect_AspectMarker::Aspect_AspectMarker () {

	MyColor	= Quantity_NOC_YELLOW;
	MyType	= Aspect_TOM_X;
	MyScale	= 1.0;

}

Aspect_AspectMarker::Aspect_AspectMarker (const Quantity_Color& AColor, const Aspect_TypeOfMarker AType, const Standard_Real AScale) {

	if (AScale <= 0.0)
		Aspect_AspectMarkerDefinitionError::Raise
			("Bad value for MarkerScale");

	MyColor	= AColor;
	MyType	= AType;
	MyScale	= AScale;

}

void Aspect_AspectMarker::SetColor (const Quantity_Color& AColor) {

	MyColor	= AColor;

}

void Aspect_AspectMarker::SetType (const Aspect_TypeOfMarker AType) {

	MyType	= AType;

}

void Aspect_AspectMarker::SetScale (const Standard_Real AScale) {

	if (AScale <= 0.0)
		Aspect_AspectMarkerDefinitionError::Raise
			("Bad value for MarkerScale");

	MyScale	= AScale;

}

void Aspect_AspectMarker::Values (Quantity_Color& AColor, Aspect_TypeOfMarker& AType, Standard_Real& AScale) const {

	AColor	= MyColor;
	AType	= MyType;
	AScale	= MyScale;

}
