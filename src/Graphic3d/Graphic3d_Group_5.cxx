// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// Modified    
//              27/08/97 ; PCT : ajout coordonnee texture


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
