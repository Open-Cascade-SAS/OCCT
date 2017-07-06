// Created on: 1996-02-20
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Poly_PolygonOnTriangulation.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Poly_PolygonOnTriangulation,Standard_Transient)

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
//function : Copy
//purpose  : 
//=======================================================================

Handle(Poly_PolygonOnTriangulation) Poly_PolygonOnTriangulation::Copy() const
{
  Handle(Poly_PolygonOnTriangulation) aCopy;
  if (myParameters.IsNull())
    aCopy = new Poly_PolygonOnTriangulation(myNodes);
  else
    aCopy = new Poly_PolygonOnTriangulation(myNodes, myParameters->Array1());
  aCopy->Deflection(myDeflection);
  return aCopy;
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

