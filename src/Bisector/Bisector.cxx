// File:	Bisector.cxx
// Created:	Tue mar 3 17:54:52 1994
// Author:	Yves FRICAUD
//		<yfr@phylox>


#include <Bisector.ixx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

//======================================================================
// function : IsConvex
// Purpose  :
//======================================================================
Standard_Boolean Bisector::IsConvex(const Handle(Geom2d_Curve)& Cu,
				    const Standard_Real         Sign)
{
  
  Standard_Real U1 = (Cu->LastParameter() + Cu->FirstParameter())/2.;
  gp_Pnt2d      P1;
  gp_Vec2d      V1,V2;
  Cu->D2(U1,P1,V1,V2);
  Standard_Real Tol = 1.e-5;
  if (Sign*(V1^V2) < Tol) return Standard_True; // <= 0.
  else                    return Standard_False;
}      

