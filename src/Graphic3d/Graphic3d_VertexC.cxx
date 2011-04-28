
// File		Graphic3d_VertexC.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration of variables specific to points

//-Warning	A point is defined by its coordinates and color

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexC.ixx>

//-Aliases

//-Global data definitions

//	-- point color
//	MyColor		:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexC::Graphic3d_VertexC () {
}

// :(AX, AY, AZ) as VertexC inherits Vertex and it is necessary to call
// constructor of Vertex with AX, AY and AZ to update fields MyX, MyY, MyZ.

Graphic3d_VertexC::Graphic3d_VertexC (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Quantity_Color& AColor):
Graphic3d_Vertex (AX, AY, AZ),
MyColor (AColor) {
}

// :(APoint) as VertexC inherits Vertex and it is necessary to call
// constructor of Vertex with APoint to update fields MyX, MyY, MyZ.

Graphic3d_VertexC::Graphic3d_VertexC (const Graphic3d_Vertex& APoint, const Quantity_Color& AColor):
Graphic3d_Vertex (APoint),
MyColor (AColor) {
}

Quantity_Color Graphic3d_VertexC::Color () const {

	return (MyColor);

}

void Graphic3d_VertexC::SetColor (const Quantity_Color& ColorNew) {

	MyColor	= ColorNew;

}
