// File:	Poly_Polygon3D.cxx
// Created:	Tue Mar  7 14:43:28 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


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


