
// File		Graphic3d_Group_11.cxx (Triangle)
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

// for the class
#include <Graphic3d_Group.jxx>
#include <Graphic3d_Group.pxx>

#include <Graphic3d_VertexN.hxx>
#include <Graphic3d_VertexC.hxx>
#include <Graphic3d_VertexNT.hxx>

//-Methods, in order

void Graphic3d_Group::TriangleMesh (const Graphic3d_Array1OfVertex& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
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

	MyGraphicDriver->TriangleMesh (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::TriangleMesh (const Graphic3d_Array1OfVertexN& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
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

	MyGraphicDriver->TriangleMesh (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}


void Graphic3d_Group::TriangleMesh(const Graphic3d_Array1OfVertexNT& ListVertex,const Standard_Boolean EvalMinMax) 
{

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
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

	MyGraphicDriver->TriangleMesh (MyCGroup, ListVertex, EvalMinMax);

	Update ();

}


void Graphic3d_Group::TriangleSet (const Graphic3d_Array1OfVertex& ListVertex, const Aspect_Array1OfEdge& ListEdge, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();
	j	= ListEdge.Length ();

	if ((i <= 2) || (j <= 2))
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
Standard_Integer Lower = ListVertex.Lower ();
Standard_Integer Upper = ListVertex.Upper ();
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

	MyGraphicDriver->TriangleSet
		(MyCGroup, ListVertex, ListEdge, EvalMinMax);

	Update ();

}

void Graphic3d_Group::TriangleSet (const Graphic3d_Array1OfVertexN& ListVertex, const Aspect_Array1OfEdge& ListEdge, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();
	j	= ListEdge.Length ();

	if ((i <= 2) || (j <= 2))
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
Standard_Integer Lower = ListVertex.Lower ();
Standard_Integer Upper = ListVertex.Upper ();
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

	MyGraphicDriver->TriangleSet
		(MyCGroup, ListVertex, ListEdge, EvalMinMax);

	Update ();

}

void Graphic3d_Group::TriangleSet (const Graphic3d_Array1OfVertexC& ListVertex, const Aspect_Array1OfEdge& ListEdge, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();
	j	= ListEdge.Length ();

	if ((i <= 2) || (j <= 2))
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
Standard_Integer Lower = ListVertex.Lower ();
Standard_Integer Upper = ListVertex.Upper ();
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

	MyGraphicDriver->TriangleSet
		(MyCGroup, ListVertex, ListEdge, EvalMinMax);

	Update ();

}

void Graphic3d_Group::TriangleSet (const Graphic3d_Array1OfVertexNC& ListVertex, const Aspect_Array1OfEdge& ListEdge, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();
	j	= ListEdge.Length ();

	if ((i <= 2) || (j <= 2))
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
Standard_Integer Lower = ListVertex.Lower ();
Standard_Integer Upper = ListVertex.Upper ();
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

	MyGraphicDriver->TriangleSet
		(MyCGroup, ListVertex, ListEdge, EvalMinMax);

	Update ();

}



void Graphic3d_Group::TriangleSet(const Graphic3d_Array1OfVertexNT& ListVertex,const Aspect_Array1OfEdge& ListEdge,const Standard_Boolean EvalMinMax) 
{

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Integer i, j;

	i	= ListVertex.Length ();
	j	= ListEdge.Length ();

	if ((i <= 2) || (j <= 2))
	Graphic3d_GroupDefinitionError::Raise ("Bad number of vertices");

	// Min-Max Update
	if (EvalMinMax) {
Standard_Real X, Y, Z;
Standard_Integer Lower = ListVertex.Lower ();
Standard_Integer Upper = ListVertex.Upper ();
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

	MyGraphicDriver->TriangleSet
		(MyCGroup, ListVertex, ListEdge, EvalMinMax);

	Update ();

}
