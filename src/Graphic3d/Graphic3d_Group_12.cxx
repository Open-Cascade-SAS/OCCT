
// File		Graphic3d_Group_12.cxx (Curve)
// Created	Aout 1994
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1994

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

//-Methods, in order

void Graphic3d_Group::Bezier (const Graphic3d_Array1OfVertex& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();

	if (i <= 2)
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
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

	MyGraphicDriver->Bezier (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Bezier (const Graphic3d_Array1OfVertex& ListVertex, const TColStd_Array1OfReal& ListWeight, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();
	j	= ListWeight.Length ();

	if ((i <= 2) || (i != j))
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
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

	MyGraphicDriver->Bezier
		(MyCGroup, ListVertex, ListWeight, EvalMinMax);

	Update ();

}
