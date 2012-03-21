// Created on: 1995-03-09
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



#include <Poly_Polygon2D.ixx>
#include <gp_Pnt2d.hxx>

//=======================================================================
//function : Poly_Polygon2D
//purpose  : 
//=======================================================================

Poly_Polygon2D::Poly_Polygon2D(const TColgp_Array1OfPnt2d& Nodes): 
    myDeflection(0.),
    myNodes(1, Nodes.Length())
{
  Standard_Integer i, j= 1;
  for (i = Nodes.Lower(); i <= Nodes.Upper(); i++)
    myNodes(j++) = Nodes(i);
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real Poly_Polygon2D::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void Poly_Polygon2D::Deflection(const Standard_Real D)
{
  myDeflection = D;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt2d& Poly_Polygon2D::Nodes() const 
{
  return myNodes;
}


