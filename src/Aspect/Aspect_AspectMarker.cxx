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
//		of tracing of markers
//-Warning	Context of tracing of markers is defined by :
//		- the color
//		- the type
//		- the scale
//-References	
//-Language	C++ 2.0
//-Declarations
// for the class

#include <Aspect_AspectMarker.hxx>
#include <Aspect_AspectMarkerDefinitionError.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

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
