
// File		Graphic3d_VertexC.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees et sa couleur

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexC.ixx>

//-Aliases

//-Global data definitions

//	-- la couleur du point
//	MyColor		:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexC::Graphic3d_VertexC () {
}

// le :(AX, AY, AZ) car VertexC herite de Vertex et il faut appeler le
// constructeur de Vertex avec AX, AY et AZ pour mettre a jour
// les fields MyX, MyY, MyZ.

Graphic3d_VertexC::Graphic3d_VertexC (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Quantity_Color& AColor):
Graphic3d_Vertex (AX, AY, AZ),
MyColor (AColor) {
}

// le :(APoint) car VertexC herite de Vertex et il faut appeler le
// constructeur de Vertex avec APoint pour mettre a jour
// les fields MyX, MyY, MyZ.

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
