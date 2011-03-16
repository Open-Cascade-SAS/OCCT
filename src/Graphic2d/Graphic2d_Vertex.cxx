
// File		Graphic2d_Vertex.cxx
// Created	Aout 1993
// Author	CAL

//-Copyright	MatraDatavision 1993

//-Version	

//-Design	Declaration des variables specifiques aux points

//-Warning	Un point est defini par ses coordonnees

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic2d_Vertex.ixx>

//-Aliases

//-Global data definitions
static Standard_ShortReal epsilon = ShortRealEpsilon();

//	-- les coordonnees du point
//	myX		:	Standard_ShortReal;
//	myY		:	Standard_ShortReal;

//-Constructors

//-Destructors

//-Methods, in order

Graphic2d_Vertex::Graphic2d_Vertex () {

	myX	= 0.0;
	myY	= 0.0;

}

Graphic2d_Vertex::Graphic2d_Vertex (const Quantity_Length X, const Quantity_Length Y) {

	myX	= Standard_ShortReal (X);
	myY	= Standard_ShortReal (Y);

}

Graphic2d_Vertex::Graphic2d_Vertex (const Standard_ShortReal X, const Standard_ShortReal Y) {

	myX	= X;
	myY	= Y;

}

void Graphic2d_Vertex::Coord (Quantity_Length& X, Quantity_Length& Y) const {

	X	= Quantity_Length (myX);
	Y	= Quantity_Length (myY);

}

Quantity_Length Graphic2d_Vertex::X () const {

	return (Quantity_Length (myX));

}

Quantity_Length Graphic2d_Vertex::Y () const {

	return (Quantity_Length (myY));

}

Standard_Boolean Graphic2d_Vertex::IsEqual(const Graphic2d_Vertex &other) const {

 	if( (Abs(myX - other.myX) > epsilon) ||
		(Abs(myY - other.myY) > epsilon) ) return Standard_False;
	else return Standard_True;
}

void Graphic2d_Vertex::SetCoord (const Quantity_Length Xnew, const Quantity_Length Ynew) {

	myX	= Standard_ShortReal (Xnew);
	myY	= Standard_ShortReal (Ynew);

}

void Graphic2d_Vertex::SetXCoord (const Quantity_Length Xnew) {

	myX	= Standard_ShortReal (Xnew);

}

void Graphic2d_Vertex::SetYCoord (const Quantity_Length Ynew) {

	myY	= Standard_ShortReal (Ynew);

}

Quantity_Length Graphic2d_Vertex::Distance (const Graphic2d_Vertex& AV1, const Graphic2d_Vertex& AV2) {

	return (sqrt ( (AV1.X () - AV2.X ()) * (AV1.X () - AV2.X ())
		     + (AV1.Y () - AV2.Y ()) * (AV1.Y () - AV2.Y ())));

}
