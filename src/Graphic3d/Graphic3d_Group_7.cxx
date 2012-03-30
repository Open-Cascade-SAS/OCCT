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


//              11/97 ; CAL : ajout polyline par 2 points


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

#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_VertexC.hxx>
#include <gp_Pnt.hxx>

void Graphic3d_Group::Polyline (const Graphic3d_Array1OfVertex& theListVertex,
                                const Standard_Boolean          theToEvalMinMax)
{
	if (IsDeleted())
  {
    return;
  }

  Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines (theListVertex.Length());

  Standard_Real aX, aY, aZ;
  Standard_Integer aVertLower = theListVertex.Lower();
  Standard_Integer aVertUpper = theListVertex.Upper();
  for (Standard_Integer aVertIter = aVertLower; aVertIter <= aVertUpper; ++aVertIter)
  {
    const Graphic3d_Vertex& aVert = theListVertex (aVertIter);
    aVert.Coord (aX, aY, aZ);
    aPrims->AddVertex (aX, aY, aZ);
  }

  AddPrimitiveArray (aPrims, theToEvalMinMax);
}

void Graphic3d_Group::Polyline (const Graphic3d_Array1OfVertexC& theListVertex,
                                const Standard_Boolean           theToEvalMinMax)
{
	if (IsDeleted())
  {
    return;
  }

  Handle(Graphic3d_ArrayOfPrimitives) aPrims
    = new Graphic3d_ArrayOfPolylines (theListVertex.Length(), 0, 0, Standard_True); // color per vertex

  Standard_Real aX, aY, aZ;
  Standard_Integer aVertLower = theListVertex.Lower();
  Standard_Integer aVertUpper = theListVertex.Upper();
  for (Standard_Integer aVertIter = aVertLower; aVertIter <= aVertUpper; ++aVertIter)
  {
    const Graphic3d_VertexC& aVert = theListVertex (aVertIter);
    aVert.Coord (aX, aY, aZ);
    aPrims->AddVertex (gp_Pnt (aX, aY, aZ), aVert.Color());
  }

  AddPrimitiveArray (aPrims, theToEvalMinMax);
}

void Graphic3d_Group::Polyline (const Graphic3d_Vertex& thePnt1,
                                const Graphic3d_Vertex& thePnt2,
                                const Standard_Boolean  theToEvalMinMax)
{
	if (IsDeleted())
  {
    return;
  }

  Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfPolylines (2);

  Standard_Real aX, aY, aZ;
	thePnt1.Coord (aX, aY, aZ);
  aPrims->AddVertex (aX, aY, aZ);

  thePnt2.Coord (aX, aY, aZ);
  aPrims->AddVertex (aX, aY, aZ);

  AddPrimitiveArray (aPrims, theToEvalMinMax);
}
