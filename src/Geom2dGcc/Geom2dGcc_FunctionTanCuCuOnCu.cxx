// Created on: 1992-01-20
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <ElCLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanCuCuOnCu.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Standard_ConstructionError.hxx>

void Geom2dGcc_FunctionTanCuCuOnCu::
  InitDerivative(const math_Vector&  X,
		       gp_Pnt2d&     Point1,
		       gp_Pnt2d&     Point2,
		       gp_Pnt2d&     Point3,
		       gp_Vec2d&     Tan1,
		       gp_Vec2d&     Tan2,
		       gp_Vec2d&     Tan3,
		       gp_Vec2d&     D21,
		       gp_Vec2d&     D22,
		       gp_Vec2d&     D23) {
  switch (TheType) {
  case Geom2dGcc_CuCuOnCu:
    {
      Geom2dGcc_CurveTool::D2(Curv1,X(1),Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      Geom2dGcc_CurveTool::D2(Curvon,X(3),Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_CiCuOnCu:
    {
      ElCLib::D2(X(1),Circ1,Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      Geom2dGcc_CurveTool::D2(Curvon,X(3),Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_LiCuOnCu:
    {
      ElCLib::D1(X(1),Lin1,Point1,Tan1);
      D21 = gp_Vec2d(0.,0.);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      Geom2dGcc_CurveTool::D2(Curvon,X(3),Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_CuPtOnCu:
    {
      Geom2dGcc_CurveTool::D2(Curv1,X(1),Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curvon,X(3),Point3,Tan3,D23);
      Point2 = Pnt2;
      Tan2 = gp_Vec2d(0.,0.);
      D22 = gp_Vec2d(0.,0.);
    }
    break;
  case Geom2dGcc_CuCuOnCi:
    {
      Geom2dGcc_CurveTool::D2(Curv1,X(1),Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      ElCLib::D2(X(3),Circon,Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_CiCuOnCi:
    {
      ElCLib::D2(X(1),Circ1,Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      ElCLib::D2(X(3),Circon,Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_LiCuOnCi:
    {
      ElCLib::D1(X(1),Lin1,Point1,Tan1);
      D21 = gp_Vec2d(0.,0.);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      ElCLib::D2(X(3),Circon,Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_CuPtOnCi:
    {
      Geom2dGcc_CurveTool::D2(Curv1,X(1),Point1,Tan1,D21);
      Point2 = Pnt2;
      Tan2 = gp_Vec2d(0.,0.);
      D22 = gp_Vec2d(0.,0.);
      ElCLib::D2(X(3),Circon,Point3,Tan3,D23);
    }
    break;
  case Geom2dGcc_CuCuOnLi:
    {
      Geom2dGcc_CurveTool::D2(Curv1,X(1),Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      ElCLib::D1(X(3),Linon,Point3,Tan3);
      D23 = gp_Vec2d(0.,0.);
    }
    break;
  case Geom2dGcc_CiCuOnLi:
    {
      ElCLib::D2(X(1),Circ1,Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      ElCLib::D1(X(3),Linon,Point3,Tan3);
      D23 = gp_Vec2d(0.,0.);
    }
    break;
  case Geom2dGcc_LiCuOnLi:
    {
      ElCLib::D1(X(1),Lin1,Point1,Tan1);
      Geom2dGcc_CurveTool::D2(Curv2,X(2),Point2,Tan2,D22);
      D21 = gp_Vec2d(0.,0.);
      ElCLib::D1(X(3),Linon,Point3,Tan3);
      D23 = gp_Vec2d(0.,0.);
    }
    break;
  case Geom2dGcc_CuPtOnLi:
    {
      Geom2dGcc_CurveTool::D2(Curv1,X(1),Point1,Tan1,D21);
      Point2 = Pnt2;
      Tan2 = gp_Vec2d(0.,0.);
      D22 = gp_Vec2d(0.,0.);
      ElCLib::D1(X(3),Linon,Point3,Tan3);
      D23 = gp_Vec2d(0.,0.);
    }
    break;
  default:
    {
      throw Standard_ConstructionError();
    }
  }
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const Geom2dAdaptor_Curve&     C1  ,
			     const Geom2dAdaptor_Curve&     C2  ,
			     const Geom2dAdaptor_Curve&     C3  ,
			     const Standard_Real Rad ) {
  Curv1  = C1;
  Curv2  = C2;
  Curvon = C3;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuCuOnCu;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const gp_Circ2d&    C1  ,
			     const Geom2dAdaptor_Curve&     C2  ,
			     const Geom2dAdaptor_Curve&     C3  ,
			     const Standard_Real Rad ) {
  Circ1  = C1;
  Curv2  = C2;
  Curvon = C3;
  FirstRad = Rad;
  TheType = Geom2dGcc_CiCuOnCu;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const gp_Lin2d&     L1  ,
			     const Geom2dAdaptor_Curve&     C2  ,
			     const Geom2dAdaptor_Curve&     C3  ,
			     const Standard_Real Rad ) {
  Lin1  = L1;
  Curv2  = C2;
  Curvon = C3;
  FirstRad = Rad;
  TheType = Geom2dGcc_LiCuOnCu;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const Geom2dAdaptor_Curve& C1  ,
			     const gp_Pnt2d&  P2  ,
			     const Geom2dAdaptor_Curve&  C3  ,
			     const Standard_Real       Rad ) {
  Curv1  = C1;
  Pnt2  = P2;
  Curvon = C3;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuPtOnCu;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const Geom2dAdaptor_Curve& C1   ,
			     const Geom2dAdaptor_Curve&  C2   ,
			     const gp_Lin2d&  OnLi ,
			     const Standard_Real       Rad  ) {
  Curv1  = C1;
  Curv2  = C2;
  Linon = OnLi;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuCuOnLi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const gp_Circ2d&           C1   ,
			       const Geom2dAdaptor_Curve& C2   ,
			       const gp_Lin2d&            OnLi ,
			       const Standard_Real                 Rad  ) {
  Circ1  = C1;
  Curv2  = C2;
  Linon = OnLi;
  FirstRad = Rad;
  TheType = Geom2dGcc_CiCuOnLi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const gp_Lin2d& L1  ,
			       const Geom2dAdaptor_Curve&  C2  ,
			       const gp_Lin2d&  OnLi ,
			       const Standard_Real       Rad ) {
  Lin1  = L1;
  Curv2  = C2;
  Linon = OnLi;
  FirstRad = Rad;
  TheType = Geom2dGcc_LiCuOnLi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const Geom2dAdaptor_Curve& C1  ,
			       const gp_Pnt2d&            P2  ,
			       const gp_Lin2d&            OnLi ,
			       const Standard_Real                 Rad ) {
  Curv1  = C1;
  Pnt2  = P2;
  Linon = OnLi;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuPtOnLi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const Geom2dAdaptor_Curve& C1   ,
			       const Geom2dAdaptor_Curve& C2   ,
			       const gp_Circ2d&           OnCi ,
			       const Standard_Real                 Rad  ) {
  Curv1  = C1;
  Curv2  = C2;
  Circon = OnCi;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuCuOnCi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const gp_Circ2d&           C1  ,
			       const Geom2dAdaptor_Curve& C2  ,
			       const gp_Circ2d&           OnCi ,
			       const Standard_Real                 Rad ) {
  Circ1  = C1;
  Curv2  = C2;
  Circon = OnCi;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuCuOnCi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const gp_Lin2d& L1  ,
			       const Geom2dAdaptor_Curve&  C2  ,
			       const gp_Circ2d& OnCi ,
			       const Standard_Real       Rad ) {
  Lin1  = L1;
  Curv2  = C2;
  Circon = OnCi;
  FirstRad = Rad;
  TheType = Geom2dGcc_LiCuOnCi;
}

Geom2dGcc_FunctionTanCuCuOnCu::
  Geom2dGcc_FunctionTanCuCuOnCu(const Geom2dAdaptor_Curve& C1  ,
			       const gp_Pnt2d&            P2  ,
			       const gp_Circ2d&           OnCi ,
			       const Standard_Real                 Rad ) {
  Curv1  = C1;
  Pnt2  = P2;
  Circon = OnCi;
  FirstRad = Rad;
  TheType = Geom2dGcc_CuPtOnCi;
}

Standard_Integer Geom2dGcc_FunctionTanCuCuOnCu::
  NbVariables () const { return 4; }

Standard_Integer Geom2dGcc_FunctionTanCuCuOnCu::
  NbEquations () const { return 4; }

Standard_Boolean Geom2dGcc_FunctionTanCuCuOnCu::
  Value (const math_Vector& X    ,
	       math_Vector& Fval ) {
  gp_Pnt2d Point1,Point2,Point3;
  gp_Vec2d Tan1,Tan2,Tan3,D21,D22,D23;
  InitDerivative(X,Point1,Point2,Point3,Tan1,Tan2,Tan3,D21,D22,D23);
//pipj (normes) et PiPj (non Normes).
  gp_Vec2d P1P2(Point1,Point2);
  gp_Vec2d P2P3(Point2,Point3);
  gp_Vec2d P3P1(Point3,Point1);
  gp_Vec2d p1p2,p2p3,p3p1;
//  if (FirstRad < 1.) {FirstRad = 1.; }
  p1p2 = P1P2/FirstRad;
  p2p3 = P2P3/FirstRad;
  p3p1 = P3P1/FirstRad;
//norme des Tani.
  Standard_Real nnor1 = Tan1.Magnitude();
  Standard_Real nnor2 = Tan2.Magnitude();
// Fonctions Fui.
// ==============
  Fval(1) = (P3P1.Dot(P3P1)-X(4)*X(4))/(FirstRad*FirstRad);
  Fval(2) = (P2P3.Dot(P2P3)-X(4)*X(4))/(FirstRad*FirstRad);
  Fval(3) = P3P1.Dot(Tan1)/(nnor1*FirstRad);
  Fval(4) = P2P3.Dot(Tan2)/(nnor2*FirstRad);
  return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCuCuOnCu::
  Derivatives (const math_Vector& X     ,
	             math_Matrix& Deriv ) {
  gp_Pnt2d Point1,Point2,Point3;
  gp_Vec2d Tan1,Tan2,Tan3;
  gp_Vec2d D21,D22,D23;
  InitDerivative(X,Point1,Point2,Point3,Tan1,Tan2,Tan3,D21,D22,D23);
//pipj (normes) et PiPj (non Normes).
  gp_Vec2d P1P2(Point1,Point2);
  gp_Vec2d P2P3(Point2,Point3);
  gp_Vec2d P3P1(Point3,Point1);
  gp_Vec2d p1p2,p2p3,p3p1;
//  if (FirstRad < 1.) {FirstRad = 1.; }
  p1p2 = P1P2/FirstRad;
  p2p3 = P2P3/FirstRad;
  p3p1 = P3P1/FirstRad;
//normales au courbes normees Nori et non nromees nori et norme des nori.
  Standard_Real nnor1 = Tan1.Magnitude();
  Standard_Real nnor2 = Tan2.Magnitude();
// Derivees dFui/uj  1 <= ui <= 3 , 1 <= uj <= 3
// =============================================
  Deriv(1,1) = 2.*Tan1.Dot(P3P1)/(FirstRad*FirstRad);
  Deriv(1,2) = 0.;
  Deriv(1,3) = -2.*Tan3.Dot(P3P1)/(FirstRad*FirstRad);
  Deriv(1,4) = -2.*X(4)/(FirstRad*FirstRad);

  Deriv(2,1) = 0.;
  Deriv(2,2) = -2.*Tan2.Dot(P2P3)/(FirstRad*FirstRad);
  Deriv(2,3) = 2.*Tan3.Dot(P2P3)/(FirstRad*FirstRad);
  Deriv(2,4) = -2.*X(4)/(FirstRad*FirstRad);

  Deriv(3,1) = (P3P1.Dot(D21)+Tan1.Dot(Tan1))/(FirstRad*nnor1)-
    (P3P1.Dot(Tan1)*D21.Dot(Tan1))/(FirstRad*nnor1*nnor1*nnor1);
  Deriv(3,2) = 0.;
  Deriv(3,3) = -(Tan3.Dot(Tan1))/(FirstRad*nnor1);
  Deriv(3,4) = 0.;

  Deriv(4,1) = 0.;
  Deriv(4,2) = (P2P3.Dot(D22)-Tan2.Dot(Tan2))/(FirstRad*nnor2)-
    P2P3.Dot(Tan2)*Tan2.Dot(D22)/(FirstRad*nnor2*nnor2*nnor2);
  Deriv(4,3) = Tan3.Dot(Tan2)/(FirstRad*nnor1);
  Deriv(4,4) = 0.;
  return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCuCuOnCu::
  Values (const math_Vector& X     ,
	        math_Vector& Fval  ,
	        math_Matrix& Deriv ) {
  gp_Pnt2d Point1,Point2,Point3;
  gp_Vec2d Tan1,Tan2,Tan3;
  gp_Vec2d D21,D22,D23;
  InitDerivative(X,Point1,Point2,Point3,Tan1,Tan2,Tan3,D21,D22,D23);
//pipj (normes) et PiPj (non Normes).
  gp_Vec2d P1P2(Point1,Point2);
  gp_Vec2d P2P3(Point2,Point3);
  gp_Vec2d P3P1(Point3,Point1);
  gp_Vec2d p1p2,p2p3,p3p1;
//  if (FirstRad < 1.) {FirstRad = 1.; }
  p1p2 = P1P2/FirstRad;
  p2p3 = P2P3/FirstRad;
  p3p1 = P3P1/FirstRad;
//normales au courbes normees Nori et non nromees nori et norme des nori.
  Standard_Real nnor1 = Tan1.Magnitude();
  Standard_Real nnor2 = Tan2.Magnitude();
// Fonctions Fui.
// ==============
  Fval(1) = (P3P1.Dot(P3P1)-X(4)*X(4))/(FirstRad*FirstRad);
  Fval(2) = (P2P3.Dot(P2P3)-X(4)*X(4))/(FirstRad*FirstRad);
  Fval(3) = P3P1.Dot(Tan1)/(nnor1*FirstRad);
  Fval(4) = P2P3.Dot(Tan2)/(nnor2*FirstRad);
// Derivees dFui/uj  1 <= ui <= 3 , 1 <= uj <= 3
// =============================================
  Deriv(1,1) = 2.*Tan1.Dot(P3P1)/(FirstRad*FirstRad);
  Deriv(1,2) = 0.;
  Deriv(1,3) = -2.*Tan3.Dot(P3P1)/(FirstRad*FirstRad);
  Deriv(1,4) = -2.*X(4)/(FirstRad*FirstRad);

  Deriv(2,1) = 0.;
  Deriv(2,2) = -2.*Tan2.Dot(P2P3)/(FirstRad*FirstRad);
  Deriv(2,3) = 2.*Tan3.Dot(P2P3)/(FirstRad*FirstRad);
  Deriv(2,4) = -2.*X(4)/(FirstRad*FirstRad);

  Deriv(3,1) = (P3P1.Dot(D21)+Tan1.Dot(Tan1))/(FirstRad*nnor1)-
    (P3P1.Dot(Tan1)*D21.Dot(Tan1))/(FirstRad*nnor1*nnor1*nnor1);
  Deriv(3,2) = 0.;
  Deriv(3,3) = -(Tan3.Dot(Tan1))/(FirstRad*nnor1);
  Deriv(3,4) = 0.;

  Deriv(4,1) = 0.;
  Deriv(4,2) = (P2P3.Dot(D22)-Tan2.Dot(Tan2))/(FirstRad*nnor2)-
    P2P3.Dot(Tan2)*Tan2.Dot(D22)/(FirstRad*nnor2*nnor2*nnor2);
  Deriv(4,3) = Tan3.Dot(Tan2)/(FirstRad*nnor1);
  Deriv(4,4) = 0.;
  return Standard_True;
}

