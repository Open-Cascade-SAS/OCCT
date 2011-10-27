
// File		Aspect_AspectLine.cxx
// Created	Mars 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

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
