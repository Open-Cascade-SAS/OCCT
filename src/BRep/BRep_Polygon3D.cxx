// File:	BRep_Polygon3D.cxx
// Created:	Wed Mar 15 13:41:41 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


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

