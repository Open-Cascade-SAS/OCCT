
// File		Graphic3d_Group_9.cxx (Quadrangle)
// Created	Fevrier 1992
// Author	NW,JPB,CAL
// Modified
//              27/08/97 ; PCT : ajout coordonnee texture

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux groupes
//		de primitives

//-Warning	Un groupe est defini dans une structure
//		Il s'agit de la plus petite entite editable

//-References	

//-Language	C++ 2.0

//-Declarations

#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

void Graphic3d_Group::QuadrangleMesh (const Graphic3d_Array2OfVertex& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Real X, Y, Z;

	// Min-Max Update
	if (EvalMinMax) {
Standard_Integer i, j;
Standard_Integer LowerRow	= ListVertex.LowerRow ();
Standard_Integer UpperRow	= ListVertex.UpperRow ();
Standard_Integer LowerCol	= ListVertex.LowerCol ();
Standard_Integer UpperCol	= ListVertex.UpperCol ();
		// Parcours des sommets
		for (i=LowerRow; i<=UpperRow; i++)
		    for (j=LowerCol; j<=UpperCol; j++) {
			ListVertex (i, j).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->QuadrangleMesh (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::QuadrangleMesh (const Graphic3d_Array2OfVertexN& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Real X, Y, Z;

Standard_Integer i, j;
Standard_Integer LowerRow	= ListVertex.LowerRow ();
Standard_Integer UpperRow	= ListVertex.UpperRow ();
Standard_Integer LowerCol	= ListVertex.LowerCol ();
Standard_Integer UpperCol	= ListVertex.UpperCol ();

	// Min-Max Update
	if (EvalMinMax) {
		// Parcours des sommets
		for (i=LowerRow; i<=UpperRow; i++)
		    for (j=LowerCol; j<=UpperCol; j++) {
			ListVertex (i, j).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->QuadrangleMesh (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}


void Graphic3d_Group::QuadrangleMesh(const Graphic3d_Array2OfVertexNT& ListVertex,const Standard_Boolean EvalMinMax) 
{
	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Real X, Y, Z;

Standard_Integer i, j;
Standard_Integer LowerRow	= ListVertex.LowerRow ();
Standard_Integer UpperRow	= ListVertex.UpperRow ();
Standard_Integer LowerCol	= ListVertex.LowerCol ();
Standard_Integer UpperCol	= ListVertex.UpperCol ();

	// Min-Max Update
	if (EvalMinMax) {
		// Parcours des sommets
		for (i=LowerRow; i<=UpperRow; i++)
		    for (j=LowerCol; j<=UpperCol; j++) {
			ListVertex (i, j).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->QuadrangleMesh (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}
