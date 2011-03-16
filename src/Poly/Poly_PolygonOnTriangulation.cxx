// File:	Poly_PolygonOnTriangulation.cxx
// Created:	Tue Feb  20 14:43:28 1996
// Author:	Laurent PAINNOT
//		<lpa@nonox>

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

