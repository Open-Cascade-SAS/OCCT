// Created on: 1996-02-20
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Poly_PolygonOnTriangulation.ixx>

//=======================================================================
//function : Poly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

Poly_PolygonOnTriangulation::Poly_PolygonOnTriangulation
(const TColStd_Array1OfInteger&    Nodes) :
    myDeflection(0.0),
    myNodes(1, Nodes.Length())
{
  myNodes = Nodes;
}

//=======================================================================
//function : Poly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

Poly_PolygonOnTriangulation::Poly_PolygonOnTriangulation
   (const TColStd_Array1OfInteger&    Nodes, 
    const TColStd_Array1OfReal&       Parameters):
    myDeflection(0.0),
    myNodes(1, Nodes.Length())
{
  myNodes = Nodes;
  myParameters = new TColStd_HArray1OfReal(1, Parameters.Length());
  myParameters->ChangeArray1() = Parameters;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real Poly_PolygonOnTriangulation::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void Poly_PolygonOnTriangulation::Deflection(const Standard_Real D)
{
  myDeflection  = D;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

const TColStd_Array1OfInteger& Poly_PolygonOnTriangulation::Nodes() const 
{
  return myNodes;
}


//=======================================================================
//function : HasParameters
//purpose  : 
//=======================================================================

Standard_Boolean Poly_PolygonOnTriangulation::HasParameters() const 
{
  return (!myParameters.IsNull());
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) Poly_PolygonOnTriangulation::Parameters() const 
{
  return myParameters;
}

