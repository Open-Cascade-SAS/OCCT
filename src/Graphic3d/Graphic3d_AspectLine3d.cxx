
// File		Graphic3d_AspectLine3d.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

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
#include <Graphic3d_AspectLine3d.ixx>

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
