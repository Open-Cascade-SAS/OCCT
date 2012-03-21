// Created on: 1995-03-15
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRep_PolygonOnClosedTriangulation.ixx>

//=======================================================================
//function : BRep_PolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

BRep_PolygonOnClosedTriangulation::BRep_PolygonOnClosedTriangulation
  (const Handle(Poly_PolygonOnTriangulation)& P1,
   const Handle(Poly_PolygonOnTriangulation)& P2, 
   const Handle(Poly_Triangulation)&          T, 
   const TopLoc_Location&                     L):
   BRep_PolygonOnTriangulation(P1, T, L),
   myPolygon2(P2)
{
}

//=======================================================================
//function : IsPolygonOnclosedTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnClosedTriangulation::IsPolygonOnClosedTriangulation() const 
{
  return Standard_True;
}


//=======================================================================
//function : PolygonOnTriangulation2
//purpose  : 
//=======================================================================

void BRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2
  (const Handle(Poly_PolygonOnTriangulation)& P2)
{
  myPolygon2 = P2;
}
//=======================================================================
//function : PolygonOnTriangulation2
//purpose  : 
//=======================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2() const 
{
  return myPolygon2;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnClosedTriangulation::Copy() const 
{
  Handle(BRep_PolygonOnClosedTriangulation) P;
  P = new BRep_PolygonOnClosedTriangulation(PolygonOnTriangulation(), myPolygon2, 
					    Triangulation(), Location());

  return P;
}

