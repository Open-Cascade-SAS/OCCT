// File:	ChFi3d_SearchSing.cxx
// Created:	Fri Mar 28 15:37:38 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <ChFi3d_SearchSing.ixx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

ChFi3d_SearchSing::ChFi3d_SearchSing(const Handle(Geom_Curve)& C1,
				     const Handle(Geom_Curve)& C2)
{
  myC1 = C1;
  myC2 = C2;
}



Standard_Boolean ChFi3d_SearchSing::Value(const Standard_Real X,
					  Standard_Real& F) 
{
  gp_Pnt P1, P2;
  gp_Vec V1, V2;
  myC1->D1(X, P1, V1);
  myC2->D1(X, P2, V2);
  gp_Vec V(P1,P2);
  F = V * (V2-V1);
  return Standard_True;
}

Standard_Boolean ChFi3d_SearchSing::Derivative(const Standard_Real X,
					       Standard_Real& D ) 
{
  gp_Pnt P1, P2;
  gp_Vec V1, V2, W1, W2;
  myC1->D2(X, P1, V1, W1);
  myC2->D2(X, P2, V2, W2);
  gp_Vec V(P1,P2), VPrim;
  VPrim = V2 -V1;
  D = VPrim.SquareMagnitude() + (V * (W2-W1));
  return Standard_True;
}

Standard_Boolean ChFi3d_SearchSing::Values(const Standard_Real X,
					   Standard_Real& F,
					   Standard_Real& D ) 
{
  gp_Pnt P1, P2;
  gp_Vec V1, V2, W1, W2;
  myC1->D2(X, P1, V1, W1);
  myC2->D2(X, P2, V2, W2);
  gp_Vec V(P1,P2), VPrim;
  VPrim = V2 -V1;
  F = V * VPrim;
  D = VPrim.SquareMagnitude() + (V * (W2-W1));
  return Standard_True;
}
