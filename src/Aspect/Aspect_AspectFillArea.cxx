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

// Modified:    15/01/98 ; FMN : Ajout Hidden Line
//-Version	
//-Design	Declaration of variables specific to the context
//		of tracing facets
//-Warning	Context of tracing facets id defined by:
//		- the style of the interior of the facet
//		- the style of the border of the facet
//		- the color
//-References	
//-Language	C++ 2.0
//-Declarations
// for the class

#include <Aspect_AspectFillArea.hxx>
#include <Aspect_AspectFillAreaDefinitionError.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

//-Aliases
//-Global data definitions
//	-- interior
//	MyInteriorStyle	:	InteriorStyle;
//	MyInteriorColor	:	Color;
//	-- border
//	MyEdgeColor	:	Color;
//	MyEdgeType	:	TypeOfLine;
//	MyEdgeWidth	:	Standard_Real;
//	-- shading
//	MyHatchStyle	:	HatchStyle;
//-Constructors
//-Destructors
//-Methods, in order
Aspect_AspectFillArea::Aspect_AspectFillArea () {

	MyInteriorStyle	= Aspect_IS_EMPTY;
	MyInteriorColor	= Quantity_NOC_CYAN1;
	MyBackInteriorColor = Quantity_NOC_CYAN1;
	MyEdgeColor	= Quantity_NOC_WHITE;
	MyEdgeType	= Aspect_TOL_SOLID;
	MyEdgeWidth	= 1.0;
	MyHatchStyle	= Aspect_HS_VERTICAL;

}

Aspect_AspectFillArea::Aspect_AspectFillArea (const Aspect_InteriorStyle InteriorStyle, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeLineWidth) {

	if (EdgeLineWidth <= 0.0)
		Aspect_AspectFillAreaDefinitionError::Raise
			("Bad value for EdgeLineWidth");

	MyInteriorStyle	= InteriorStyle;
	MyInteriorColor	= InteriorColor;
	MyEdgeColor	= EdgeColor;
	MyEdgeType	= EdgeLineType;
	MyEdgeWidth	= EdgeLineWidth;
	MyHatchStyle	= Aspect_HS_VERTICAL;

}

void Aspect_AspectFillArea::SetInteriorStyle (const Aspect_InteriorStyle AStyle) {

	MyInteriorStyle	= AStyle;

}

void Aspect_AspectFillArea::SetInteriorColor (const Quantity_Color& AColor) {

	MyInteriorColor	= AColor;

}

void Aspect_AspectFillArea::SetBackInteriorColor( const Quantity_Color& color )
{
  MyBackInteriorColor = color;
}

void Aspect_AspectFillArea::SetEdgeColor (const Quantity_Color& AColor) {

	MyEdgeColor	= AColor;

}

void Aspect_AspectFillArea::SetEdgeLineType (const Aspect_TypeOfLine AType) {

	MyEdgeType	= AType;

}

void Aspect_AspectFillArea::SetEdgeWidth (const Standard_Real AWidth) {

	if (AWidth <= 0.0)
		Aspect_AspectFillAreaDefinitionError::Raise
			("Bad value for EdgeLineWidth");

	MyEdgeWidth	= AWidth;

}

void Aspect_AspectFillArea::SetHatchStyle (const Aspect_HatchStyle AStyle) {

	MyHatchStyle	= AStyle;

}

Aspect_HatchStyle Aspect_AspectFillArea::HatchStyle () const {

	return (MyHatchStyle);

}

void Aspect_AspectFillArea::Values (Aspect_InteriorStyle& AStyle, Quantity_Color& AIntColor, Quantity_Color& AEdgeColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const {

	AStyle		= MyInteriorStyle;
	AIntColor	= MyInteriorColor;
	AEdgeColor	= MyEdgeColor;
	AType		= MyEdgeType;
	AWidth		= MyEdgeWidth;

}

void Aspect_AspectFillArea::Values (Aspect_InteriorStyle& AStyle, Quantity_Color& AIntColor, Quantity_Color& BackIntColor, Quantity_Color& AEdgeColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const {

	AStyle		= MyInteriorStyle;
	AIntColor	= MyInteriorColor;
	BackIntColor    = MyBackInteriorColor;
	AEdgeColor	= MyEdgeColor;
	AType		= MyEdgeType;
	AWidth		= MyEdgeWidth;

}
