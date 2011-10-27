
// File		Aspect_AspectMarker.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

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
