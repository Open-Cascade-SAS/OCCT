// Copyright (c) 1998-1999 Matra Datavision
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

#include <PBRep_PolygonOnClosedTriangulation.ixx>


//=======================================================================
//function : PBRep_PolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

PBRep_PolygonOnClosedTriangulation::PBRep_PolygonOnClosedTriangulation
(const Handle(PPoly_PolygonOnTriangulation)& aP1,
 const Handle(PPoly_PolygonOnTriangulation)& aP2,
 const Handle(PPoly_Triangulation)&      aPPol,
 const PTopLoc_Location&                 aPLoc) :
 PBRep_PolygonOnTriangulation(aP1, aPPol, aPLoc),
 myPolygon2(aP2)
{
}


//=======================================================================
//function : PBRep_PolygonOnClosedTriangulation::IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnClosedTriangulation::IsPolygonOnClosedTriangulation() const 
{
  return Standard_True;
}

//=======================================================================
//function : PBRep_CurveRepresentation::PolygonOnTriangulation2
//purpose  : 
//=======================================================================

Handle(PPoly_PolygonOnTriangulation) 
     PBRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2() const 
{
  return myPolygon2;
}
