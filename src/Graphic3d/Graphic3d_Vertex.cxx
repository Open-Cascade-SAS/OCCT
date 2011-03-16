
// File		Graphic3d_Vertex.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Vertex.ixx>

//-Aliases

//-Global data definitions

//	-- les coordonnees du point
//	MyX		:	Standard_ShortReal;
//	MyY		:	Standard_ShortReal;
//	MyZ		:	Standard_ShortReal;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_Vertex::Graphic3d_Vertex ():
MyX (Standard_ShortReal (0.0)),
MyY (Standard_ShortReal (0.0)),
MyZ (Standard_ShortReal (0.0)) {
}

Graphic3d_Vertex::Graphic3d_Vertex (const Graphic3d_Vertex& P) {

	MyX	= Standard_ShortReal (P.X ());
	MyY	= Standard_ShortReal (P.Y ());
	MyZ	= Standard_ShortReal (P.Z ());

}

Graphic3d_Vertex::Graphic3d_Vertex (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ):
MyX (Standard_ShortReal (AX)),
MyY (Standard_ShortReal (AY)),
MyZ (Standard_ShortReal (AZ)) {
}

void Graphic3d_Vertex::Coord (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const {

	X	= Standard_Real (MyX);
	Y	= Standard_Real (MyY);
	Z	= Standard_Real (MyZ);

}

Standard_Real Graphic3d_Vertex::X () const {

	return Standard_Real (MyX);

}

Standard_Real Graphic3d_Vertex::Y () const {

	return Standard_Real (MyY);

}

Standard_Real Graphic3d_Vertex::Z () const {

	return Standard_Real (MyZ);

}

void Graphic3d_Vertex::SetCoord (const Standard_Real Xnew, const Standard_Real Ynew, const Standard_Real Znew) {

	MyX	= Standard_ShortReal (Xnew);
	MyY	= Standard_ShortReal (Ynew);
	MyZ	= Standard_ShortReal (Znew);

}

void Graphic3d_Vertex::SetXCoord (const Standard_Real Xnew) {

	MyX	= Standard_ShortReal (Xnew);

}

void Graphic3d_Vertex::SetYCoord (const Standard_Real Ynew) {

	MyY	= Standard_ShortReal (Ynew);

}

void Graphic3d_Vertex::SetZCoord (const Standard_Real Znew) {

	MyZ	= Standard_ShortReal (Znew);

}

Standard_Real Graphic3d_Vertex::Distance (const Graphic3d_Vertex& AV1, const Graphic3d_Vertex& AV2) {

	return (Sqrt ( (AV1.X () - AV2.X ()) * (AV1.X () - AV2.X ())
		     + (AV1.Y () - AV2.Y ()) * (AV1.Y () - AV2.Y ())
		     + (AV1.Z () - AV2.Z ()) * (AV1.Z () - AV2.Z ())));
}
