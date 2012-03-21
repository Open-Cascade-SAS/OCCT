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



#include <BRep_Polygon3D.ixx>


//=======================================================================
//function : BRep_Polygon3D
//purpose  : 
//=======================================================================

BRep_Polygon3D::BRep_Polygon3D(const Handle(Poly_Polygon3D)& P, 
			       const TopLoc_Location&        L): 
			       BRep_CurveRepresentation(L),
			       myPolygon3D(P)
{
}

//=======================================================================
//function : IsPolygon3D
//purpose  : 
//=======================================================================

Standard_Boolean BRep_Polygon3D::IsPolygon3D() const 
{
  return Standard_True;
}

//=======================================================================
//function : Polygon3D
//purpose  : 
//=======================================================================

const Handle(Poly_Polygon3D)& BRep_Polygon3D::Polygon3D() const 
{
  return myPolygon3D;
}

//=======================================================================
//function : Polygon3d
//purpose  : 
//=======================================================================

void BRep_Polygon3D::Polygon3D(const Handle(Poly_Polygon3D)& P)
{
  myPolygon3D = P;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_Polygon3D::Copy() const 
{
  Handle(BRep_Polygon3D) P = new BRep_Polygon3D(myPolygon3D, Location());
  return P;
}

