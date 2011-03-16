
// File		Graphic3d_Group_5.cxx (Polygon)
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
#include <Graphic3d_VertexNT.hxx>

//-Methods, in order

void Graphic3d_Group::Polygon (const Graphic3d_Array1OfVertex& ListVertex, const Graphic3d_TypeOfPolygon AType, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
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

	MyGraphicDriver->Polygon (MyCGroup, ListVertex, AType, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polygon (const Graphic3d_Array1OfVertex& ListVertex, const Graphic3d_Vector& Normal, const Graphic3d_TypeOfPolygon AType, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
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

	MyGraphicDriver->Polygon
		(MyCGroup, ListVertex, Normal, AType, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polygon (const Graphic3d_Array1OfVertexN& ListVertex, const Graphic3d_TypeOfPolygon AType, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i;
		Standard_Integer Lower	= ListVertex.Lower ();
		Standard_Integer Upper	= ListVertex.Upper ();
		// Parcours des sommets
		for (i=Lower; i<=Upper; i++) {
			ListVertex (i).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->Polygon (MyCGroup, ListVertex, AType, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polygon (const Graphic3d_Array1OfVertexN& ListVertex, const Graphic3d_Vector& Normal, const Graphic3d_TypeOfPolygon AType, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i;
		Standard_Integer Lower	= ListVertex.Lower ();
		Standard_Integer Upper	= ListVertex.Upper ();
		// Parcours des sommets
		for (i=Lower; i<=Upper; i++) {
			ListVertex (i).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->Polygon
		(MyCGroup, ListVertex, Normal, AType, EvalMinMax);

	Update ();

}

void Graphic3d_Group::PolygonSet (const TColStd_Array1OfInteger& Bounds, const Graphic3d_Array1OfVertex& ListVertex, const Graphic3d_TypeOfPolygon AType, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

Standard_Real X, Y, Z;

Standard_Integer i, ii;
Standard_Integer j;
Standard_Integer begin_points;
Standard_Integer end_points;
Standard_Integer nbpoints;
Standard_Integer Lower, Upper;

	begin_points	= ListVertex.Lower ();
	end_points	= ListVertex.Upper ();

	Lower	= Bounds.Lower ();
	Upper	= Bounds.Upper ();

	// Parcours des facettes
	for (j=Lower; j<=Upper; j++) {
		nbpoints	= Bounds.Value (j);

		Graphic3d_Array1OfVertex ListOfVertex (0, nbpoints-1);

		// Parcours des sommets
		for (i=0, ii=begin_points;
			((ii<=end_points) && (i<=nbpoints-1)); i++, ii++) {
			ListVertex (ii).Coord (X, Y, Z);
			ListOfVertex (i).SetCoord (X, Y, Z);
		}

		Polygon (ListOfVertex, AType, EvalMinMax);
		begin_points += nbpoints;
	}

}

void Graphic3d_Group::Polygon (const TColStd_Array1OfInteger& Bounds, const Graphic3d_Array1OfVertex& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i, ii;
		Standard_Integer k, kk;
		Standard_Integer begin_points;
		Standard_Integer end_points;
		Standard_Integer Lower, Upper;

		begin_points	= ListVertex.Lower ();
		end_points	= ListVertex.Upper ();

		Lower	= Bounds.Lower ();
		Upper	= Bounds.Upper ();
		// Parcours des facettes
		for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
			// Parcours des sommets
			for (i=0, ii=begin_points;
			((ii<=end_points) || (i==int (Bounds.Value (kk))-1));
								i++, ii++) {
				ListVertex (ii).Coord (X, Y, Z);

				if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
				if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
				if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
				if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
				if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
				if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
			}
			begin_points += int (Bounds.Value (kk));
		}
	}

	MyGraphicDriver->PolygonHoles
		(MyCGroup, Bounds, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polygon (const TColStd_Array1OfInteger& Bounds, const Graphic3d_Array1OfVertex& ListVertex, const Graphic3d_Vector& Normal, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i, ii;
		Standard_Integer k, kk;
		Standard_Integer begin_points;
		Standard_Integer end_points;
		Standard_Integer Lower, Upper;

		begin_points	= ListVertex.Lower ();
		end_points	= ListVertex.Upper ();

		Lower	= Bounds.Lower ();
		Upper	= Bounds.Upper ();
		// Parcours des facettes
		for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
			// Parcours des sommets
			for (i=0, ii=begin_points;
			((ii<=end_points) || (i==int (Bounds.Value (kk))-1));
								i++, ii++) {
				ListVertex (ii).Coord (X, Y, Z);

				if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
				if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
				if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
				if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
				if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
				if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
			}
			begin_points += int (Bounds.Value (kk));
		}
	}

	MyGraphicDriver->PolygonHoles
		(MyCGroup, Bounds, ListVertex, Normal, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polygon (const TColStd_Array1OfInteger& Bounds, const Graphic3d_Array1OfVertexN& ListVertex, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i, ii;
		Standard_Integer k, kk;
		Standard_Integer begin_points;
		Standard_Integer end_points;
		Standard_Integer Lower, Upper;

		begin_points	= ListVertex.Lower ();
		end_points	= ListVertex.Upper ();

		Lower	= Bounds.Lower ();
		Upper	= Bounds.Upper ();
		// Parcours des facettes
		for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
			// Parcours des sommets
			for (i=0, ii=begin_points;
			((ii<=end_points) || (i==int (Bounds.Value (kk))-1));
								i++, ii++) {
				ListVertex (ii).Coord (X, Y, Z);
				if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
				if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
				if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
				if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
				if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
				if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
			}
			begin_points += int (Bounds.Value (kk));
		}
	}

	MyGraphicDriver->PolygonHoles
		(MyCGroup, Bounds, ListVertex, EvalMinMax);

	Update ();

}

void Graphic3d_Group::Polygon (const TColStd_Array1OfInteger& Bounds, const Graphic3d_Array1OfVertexN& ListVertex, const Graphic3d_Vector& Normal, const Standard_Boolean EvalMinMax) {

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i, ii;
		Standard_Integer k, kk;
		Standard_Integer begin_points;
		Standard_Integer end_points;
		Standard_Integer Lower, Upper;

		begin_points	= ListVertex.Lower ();
		end_points	= ListVertex.Upper ();

		Lower	= Bounds.Lower ();
		Upper	= Bounds.Upper ();
		// Parcours des facettes
		for (k=0, kk=Lower; kk<=Upper; k++, kk++) {
			// Parcours des sommets
			for (i=0, ii=begin_points;
			((ii<=end_points) || (i==int (Bounds.Value (kk))-1));
								i++, ii++) {
				ListVertex (ii).Coord (X, Y, Z);
				if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
				if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
				if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
				if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
				if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
				if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
			}
			begin_points += int (Bounds.Value (kk));
		}
	}

	MyGraphicDriver->PolygonHoles
		(MyCGroup, Bounds, ListVertex, Normal, EvalMinMax);

	Update ();

}



void Graphic3d_Group::Polygon(const Graphic3d_Array1OfVertexNT& ListVertex,const Graphic3d_TypeOfPolygon AType,const Standard_Boolean EvalMinMax) 
{

	if (IsDeleted ()) return;

	if (! MyContainsFacet) MyStructure->GroupsWithFacet (+1);
	MyContainsFacet	= Standard_True;
	MyIsEmpty	= Standard_False;

	// Min-Max Update
	if (EvalMinMax) {
		Standard_Real X, Y, Z;
		Standard_Integer i;
		Standard_Integer Lower	= ListVertex.Lower ();
		Standard_Integer Upper	= ListVertex.Upper ();
		// Parcours des sommets
		for (i=Lower; i<=Upper; i++) {
			ListVertex (i).Coord (X, Y, Z);
			if (X < MyBounds.XMin) MyBounds.XMin	= Standard_ShortReal (X);
			if (Y < MyBounds.YMin) MyBounds.YMin	= Standard_ShortReal (Y);
			if (Z < MyBounds.ZMin) MyBounds.ZMin	= Standard_ShortReal (Z);
			if (X > MyBounds.XMax) MyBounds.XMax	= Standard_ShortReal (X);
			if (Y > MyBounds.YMax) MyBounds.YMax	= Standard_ShortReal (Y);
			if (Z > MyBounds.ZMax) MyBounds.ZMax	= Standard_ShortReal (Z);
		}
	}

	MyGraphicDriver->Polygon (MyCGroup, ListVertex, AType, EvalMinMax);

	Update ();

}
