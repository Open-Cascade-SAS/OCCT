
// File		Graphic3d_VertexNC.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL
// Modified:	03-02-98 : FMN ; Add Flag Normal

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees, sa normale
//		et sa couleur

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_VertexNC.ixx>

//-Aliases

//-Global data definitions

//	-- la couleur
//	MyColor		:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_VertexNC::Graphic3d_VertexNC ():
Graphic3d_VertexN (0.0, 0.0, 0.0, 0.0, 0.0, 1.0) {

Quantity_Color AColor (Quantity_NOC_WHITE);

	MyColor	= AColor;

}

// le :(AX, AY, AZ) car VertexNC herite de VertexN et il faut appeler le
// constructeur de VertexN avec AX, AY, AZ, ANX, ANY et ANZ pour
// mettre a jour les fields.

Graphic3d_VertexNC::Graphic3d_VertexNC (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ, const Standard_Real ANX, const Standard_Real ANY, const Standard_Real ANZ, const Quantity_Color& AColor,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (AX, AY, AZ, ANX, ANY, ANZ,FlagNormalise),
MyColor (AColor) {
}

// le :(APoint, AVector) car VertexNC herite de VertexN et il faut appeler le
// constructeur de VertexN avec APoint et AVector pour mettre a jour
// les fields.

Graphic3d_VertexNC::Graphic3d_VertexNC (const Graphic3d_Vertex& APoint, const Graphic3d_Vector& AVector, const Quantity_Color& AColor,const Standard_Boolean FlagNormalise):
Graphic3d_VertexN (APoint, AVector,FlagNormalise),
MyColor (AColor) {
}

Quantity_Color Graphic3d_VertexNC::Color () const {

	return (MyColor);

}

void Graphic3d_VertexNC::SetColor (const Quantity_Color& ColorNew) {

	MyColor	= ColorNew;

}
