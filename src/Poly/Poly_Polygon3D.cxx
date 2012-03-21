// Created on: 1995-03-07
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



#include <Poly_Polygon3D.ixx>
#include <gp_Pnt.hxx>

//=======================================================================
//function : Poly_Polygon3D
//purpose  : 
//=======================================================================

Poly_Polygon3D::Poly_Polygon3D(const TColgp_Array1OfPnt& Nodes): 
    myDeflection(0.),
    myNodes(1, Nodes.Length())
{
  Standard_Integer i, j= 1;
  for (i = Nodes.Lower(); i <= Nodes.Upper(); i++)
    myNodes(j++) = Nodes(i);
}

//=======================================================================
//function : Poly_Polygon3D
//purpose  : 
//=======================================================================

Poly_Polygon3D::Poly_Polygon3D(const TColgp_Array1OfPnt&   Nodes,
			       const TColStd_Array1OfReal& P): 
    myDeflection(0.),
    myNodes(1, Nodes.Length())
    
{
  myParameters = new TColStd_HArray1OfReal(1, P.Length());
  Standard_Integer i, j= 1;
  for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) {
    myNodes(j) = Nodes(i);
    myParameters->SetValue(j, P(i));
    j++;
  }
}


//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real Poly_Polygon3D::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void Poly_Polygon3D::Deflection(const Standard_Real D)
{
  myDeflection = D;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt& Poly_Polygon3D::Nodes() const 
{
  return myNodes;
}

//=======================================================================
//function : HasParameters
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Polygon3D::HasParameters() const 
{
  return !myParameters.IsNull();
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

const TColStd_Array1OfReal& Poly_Polygon3D::Parameters() const 
{
  return myParameters->Array1();
}

//=======================================================================
//function : ChangeParameters
//purpose  : 
//=======================================================================

TColStd_Array1OfReal& Poly_Polygon3D::ChangeParameters() const 
{
  return myParameters->ChangeArray1();
}


