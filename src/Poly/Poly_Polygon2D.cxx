// File:	Poly_Polygon2D.cxx
// Created:	Thu Mar  9 16:36:49 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


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


