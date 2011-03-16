
// File		Graphic3d_Group_7.cxx (Polyline)
// Created	Fevrier 1992
// Author	NW,JPB,CAL
//              11/97 ; CAL : ajout polyline par 2 points

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux groupes
//		de primitives

//-Warning	Un groupe est defini dans une structure
//		Il s'agit de la plus petite entite editable

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

#include <Graphic3d_VertexC.hxx>

//-Methods, in order

void Graphic3d_Group::Polyline (const Graphic3d_Array1OfVertex& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i, j;
		Standard_Integer Lower	= ListVertex.Lower ();
		Standard_Integer Upper	= ListVertex.Upper ();
		// Parcours des sommets
		for (j=0, i=Lower; i<=Upper; i++, j++) {
			ListVertex (i).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->Polyline (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polyline (const Graphic3d_Array1OfVertexC& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i, j;
		Standard_Integer Lower	= ListVertex.Lower ();
		Standard_Integer Upper	= ListVertex.Upper ();
		// Parcours des sommets
		for (j=0, i=Lower; i<=Upper; i++, j++) {
			ListVertex (i).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->Polyline (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polyline (const Graphic3d_Vertex& APT1, const Graphic3d_Vertex& APT2, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	MyIsEmpty	= Standard_False;

Standard_Real X1, Y1, Z1;
Standard_Real X2, Y2, Z2;

	APT1.Coord (X1, Y1, Z1);
	APT2.Coord (X2, Y2, Z2);

	// Min-Max Update
	if (EvalMinMax) {
		if (X1 < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X1);
		if (Y1 < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y1);
		if (Z1 < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z1);
		if (X1 > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X1);
		if (Y1 > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y1);
		if (Z1 > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z1);
		if (X2 < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X2);
		if (Y2 < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y2);
		if (Z2 < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z2);
		if (X2 > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X2);
		if (Y2 > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y2);
		if (Z2 > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z2);
	}

	MyGraphicDriver->Polyline (MyCGroup, X1, Y1, Z1, X2, Y2, Z2, EvalMinMax);

	Update ();

}
