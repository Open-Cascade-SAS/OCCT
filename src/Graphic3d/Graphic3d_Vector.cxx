
// File		Graphic3d_Vector.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL
// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux vecteurs

//-Warning	Un vecteur est defini par ses composantes ou par
//		deux points
//		Il peut etre normalise

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Vector.ixx>

#include <Standard_Boolean.hxx>

//-Aliases

//-Global data definitions

#define Graphic3d_Vector_MyEpsilon 0.000001

//	-- les coordonnees du vecteur
//	MyX		:	Standard_ShortReal;
//	MyY		:	Standard_ShortReal;
//	MyZ		:	Standard_ShortReal;

//	-- la norme du vecteur
//	MyNorme		:	Standard_ShortReal;

//-Constructors

Graphic3d_Vector::Graphic3d_Vector ():
MyX (Standard_ShortReal (1.0)),
MyY (Standard_ShortReal (1.0)),
MyZ (Standard_ShortReal (1.0)),
MyNorme (Standard_ShortReal (1.0)) {
}

Graphic3d_Vector::Graphic3d_Vector (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ):
MyX (Standard_ShortReal (AX)),
MyY (Standard_ShortReal (AY)),
MyZ (Standard_ShortReal (AZ)),
MyNorme (Standard_ShortReal (Graphic3d_Vector::NormeOf (AX, AY, AZ))) {
}

Graphic3d_Vector::Graphic3d_Vector (const Graphic3d_Vertex& APoint1, const Graphic3d_Vertex& APoint2) {

Standard_Real X1, Y1, Z1;
Standard_Real X2, Y2, Z2;

	APoint1.Coord (X1, Y1, Z1);
	APoint2.Coord (X2, Y2, Z2);

	MyX	= Standard_ShortReal (X2 - X1);
	MyY	= Standard_ShortReal (Y2 - Y1);
	MyZ	= Standard_ShortReal (Z2 - Z1);

	MyNorme	= Standard_ShortReal (Graphic3d_Vector::NormeOf (X2 - X1, Y2 - Y1, Z2 - Z1));

}

//-Destructors

//-Methods, in order

void Graphic3d_Vector::Coord (Standard_Real& AX, Standard_Real& AY, Standard_Real& AZ) const {

	AX	= Standard_Real (MyX);
	AY	= Standard_Real (MyY);
	AZ	= Standard_Real (MyZ);

}

Standard_Real Graphic3d_Vector::X () const {

	return Standard_Real (MyX);

}

Standard_Real Graphic3d_Vector::Y () const {

	return Standard_Real (MyY);

}

Standard_Real Graphic3d_Vector::Z () const {

	return Standard_Real (MyZ);

}

void Graphic3d_Vector::Normalize () {

	if (Abs (MyNorme) <= RealEpsilon ())
		Graphic3d_VectorError::Raise ("The norm is null");

	if (!IsNormalized()) // CQO CTS40181
	    {	
	       MyX	= MyX / MyNorme;
	       MyY	= MyY / MyNorme;
	       MyZ	= MyZ / MyNorme;
	    }

	MyNorme	= Standard_ShortReal (1.0);

}

void Graphic3d_Vector::SetCoord (const Standard_Real Xnew, const Standard_Real Ynew, const Standard_Real Znew) {

	MyX	= Standard_ShortReal (Xnew);
	MyY	= Standard_ShortReal (Ynew);
	MyZ	= Standard_ShortReal (Znew);

	MyNorme	= Standard_ShortReal (Graphic3d_Vector::NormeOf (Standard_Real (MyX), Standard_Real (MyY), Standard_Real (MyZ)));

}

void Graphic3d_Vector::SetXCoord (const Standard_Real Xnew) {

	MyX	= Standard_ShortReal (Xnew);

	MyNorme	= Standard_ShortReal (Graphic3d_Vector::NormeOf (Standard_Real (MyX), Standard_Real (MyY), Standard_Real (MyZ)));

}

void Graphic3d_Vector::SetYCoord (const Standard_Real Ynew) {

	MyY	= Standard_ShortReal (Ynew);

	MyNorme	= Standard_ShortReal (Graphic3d_Vector::NormeOf (Standard_Real (MyX), Standard_Real (MyY), Standard_Real (MyZ)));

}

void Graphic3d_Vector::SetZCoord (const Standard_Real Znew) {

	MyZ	= Standard_ShortReal (Znew);

	MyNorme	= Standard_ShortReal (Graphic3d_Vector::NormeOf (Standard_Real (MyX), Standard_Real (MyY), Standard_Real (MyZ)));

}

Standard_Boolean Graphic3d_Vector::LengthZero () const {

	return (Abs (Standard_Real (MyNorme)) <= RealEpsilon ());

}

Standard_Boolean Graphic3d_Vector::IsNormalized () const {

	return (Abs (Standard_Real (MyNorme) - 1.0) <=
				Graphic3d_Vector_MyEpsilon);

}

Standard_Boolean Graphic3d_Vector::IsParallel (const Graphic3d_Vector& AV1, const Graphic3d_Vector& AV2) {

Standard_Real Result;

	Result	= (AV1.Y () * AV2.Z () - AV1.Z () * AV2.Y ())
		- (AV1.X () * AV2.Z () - AV1.Z () * AV2.X ())
		- (AV1.X () * AV2.Y () - AV1.Y () * AV2.X ());

	return (Abs (Result) <= Graphic3d_Vector_MyEpsilon);

}

Standard_Real Graphic3d_Vector::NormeOf (const Standard_Real AX, const Standard_Real AY, const Standard_Real AZ) {

	return (Sqrt (AX*AX+AY*AY+AZ*AZ));

}

Standard_Real Graphic3d_Vector::NormeOf (const Graphic3d_Vector& AVector) {

Standard_Real X, Y, Z;

	AVector.Coord(X, Y, Z);
	return (Graphic3d_Vector::NormeOf (X, Y, Z));

}
