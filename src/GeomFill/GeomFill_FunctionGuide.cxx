// Created on: 1998-07-09
// Created by: Stephanie HUMEAU
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor3d_HCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomFill_FunctionGuide.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomTools.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//#include <Standard_NotImplemented.hxx>
//==============================================
//   Calcul de la valeur de la fonction :
//                      G(w) - S(teta,v) = 0
// ou G : guide   et   S : surface de revolution 
//==============================================
//==============================================
// Function : FunctionGuide
// Purpose : Initialisation de la section et de la surface d'arret
//==============================================
GeomFill_FunctionGuide::GeomFill_FunctionGuide
     (const Handle(GeomFill_SectionLaw)& S, 
      const Handle(Adaptor3d_HCurve)& C,
      const Standard_Real Param)
: TheGuide(C), TheLaw(S), TheUonS(Param)
{
  Standard_Real Tol = Precision::Confusion();
  if (TheLaw->IsConstant(Tol)) {
    isconst = Standard_True;
    TheConst = TheLaw->ConstantSection();
    First = TheConst->FirstParameter();
    Last =  TheConst->LastParameter();
  }
  else {
   isconst = Standard_False;
   TheConst.Nullify(); 
  }
  TheCurve.Nullify();
}

//==============================================
// Function : SetParam
// Purpose : Initialisation de la surface de revolution
//==============================================
// void GeomFill_FunctionGuide::SetParam(const Standard_Real Param,
 void GeomFill_FunctionGuide::SetParam(const Standard_Real ,
				       const gp_Pnt& C,
				       const gp_XYZ& D,
				       const gp_XYZ& DX)
{
  Centre = C.XYZ();
  Dir = D;

  //repere fixe
  gp_Ax3 Rep (gp::Origin(), gp::DZ(), gp::DX());
 

  // calculer transfo entre triedre et Oxyz
  gp_Dir B2 = DX;
  gp_Ax3 RepTriedre(C, D, B2);
  gp_Trsf Transfo;
  Transfo.SetTransformation(RepTriedre, Rep);
   

  if (isconst) {
    TheCurve = new (Geom_TrimmedCurve) 
      (Handle(Geom_Curve)::DownCast(TheConst->Copy()),
	First, Last);
  }
  else {
    Standard_Integer NbPoles, NbKnots, Deg;
    TheLaw->SectionShape(NbPoles, NbKnots, Deg);
    TColStd_Array1OfInteger Mult(1,NbKnots);
    TheLaw->Mults( Mult);
    TColStd_Array1OfReal Knots(1,NbKnots);
    TheLaw->Knots(Knots);
    TColgp_Array1OfPnt Poles(1, NbPoles);
    TColStd_Array1OfReal Weights(1,  NbPoles);
    TheLaw->D0(TheUonS, Poles, Weights);
    if (TheLaw->IsRational()) 
      TheCurve = new (Geom_BSplineCurve)
	(Poles, Weights, Knots, Mult ,
	 Deg, TheLaw->IsUPeriodic());
   else 
     TheCurve = new (Geom_BSplineCurve)
	(Poles, Knots, Mult,
	 Deg, TheLaw->IsUPeriodic()); 
  }

  gp_Ax1 Axe(C, Dir);
  TheCurve->Transform(Transfo);
  TheSurface = new(Geom_SurfaceOfRevolution) (TheCurve, Axe);
}

//==============================================
// Function : NbVariables (w, u, v)
// Purpose :
//==============================================
 Standard_Integer GeomFill_FunctionGuide::NbVariables()const 
{ 
  return 3;
}

//==============================================
// Function : NbEquations
// Purpose :
//==============================================
 Standard_Integer GeomFill_FunctionGuide::NbEquations()const
{
  return 3;
}

//==============================================
// Function : Value
// Purpose : calcul of the value of the function at <X>
//==============================================
 Standard_Boolean GeomFill_FunctionGuide::Value(const math_Vector& X,
					        math_Vector& F) 
{
  gp_Pnt P,P1;
 

  TheGuide->D0(X(1), P);
  TheSurface->D0(X(2), X(3), P1);
  
  F(1) = P.Coord(1) - P1.Coord(1);
  F(2) = P.Coord(2) - P1.Coord(2); 
  F(3) = P.Coord(3) - P1.Coord(3);

  return Standard_True;
}

//==============================================
// Function : Derivatives
// Purpose :calcul of the derivative of the function
//==============================================
 Standard_Boolean GeomFill_FunctionGuide::Derivatives(const math_Vector& X,
						      math_Matrix& D) 
{
  gp_Pnt P,P1;
  gp_Vec DP,DP1U,DP1V;

  TheGuide->D1(X(1),P,DP);
  TheSurface->D1(X(2),X(3),P1,DP1U,DP1V);

  Standard_Integer i;
  for (i=1;i<=3;i++)
    {
      D(i,1) = DP.Coord(i);  
      D(i,2) = -DP1U.Coord(i);
      D(i,3) = -DP1V.Coord(i);
    }// for

  return Standard_True;
}

//==============================================
// Function : Values
// Purpose : calcul of the value and the derivative of the function
//==============================================
 Standard_Boolean GeomFill_FunctionGuide::Values(const math_Vector& X,
						 math_Vector& F,
						 math_Matrix& D)
{ 
  gp_Pnt P,P1;
  gp_Vec DP,DP1U,DP1V;

  TheGuide->D1(X(1),P,DP); //derivee de la generatrice
  TheSurface->D1(X(2),X(3),P1,DP1U,DP1V); //derivee de la new surface

  Standard_Integer i;
  for (i=1;i<=3;i++)
    { 
      F(i) = P.Coord(i) - P1.Coord(i);

      D(i,1) = DP.Coord(i);  
      D(i,2) = -DP1U.Coord(i);
      D(i,3) = -DP1V.Coord(i);
    }// for

  return Standard_True;
}

//==============================================
// Function : DerivT
// Purpose : calcul of the first derivative from t 
//==============================================
 Standard_Boolean GeomFill_FunctionGuide::DerivT(const math_Vector& X,
						 const gp_XYZ& DCentre,
						 const gp_XYZ& DDir,
						 math_Vector& F) 
{ 
  gp_Pnt P;
  gp_Vec DS;
  DSDT(X(2),X(3), DCentre,DDir, DS);

  TheCurve->D0(X(1), P);

  F(1) = P.Coord(1) - DS.Coord(1);
  F(2) = P.Coord(2) - DS.Coord(2); 
  F(3) = P.Coord(3) - DS.Coord(3);  

  return Standard_True;
}

//=========================================================
// Function : DSDT
// Purpose : calcul de la derive de la surface /t en U, V
//=========================================================
 void GeomFill_FunctionGuide::DSDT(const Standard_Real U,
				   const Standard_Real V,
				   const gp_XYZ& DC,
				   const gp_XYZ& DDir,
				   gp_Vec& DS) const
{ 
   // C origine sur l'axe de revolution
   // Vdir vecteur unitaire definissant la direction de l'axe de revolution
   // Q(v) point de parametre V sur la courbe de revolution
   // OM (u,v) = OC + CQ * Cos(U) + (CQ.Vdir)(1-Cos(U)) * Vdir +
   //            (Vdir^CQ)* Sin(U)


   gp_Pnt Pc;
   TheCurve->D0(V, Pc);                  //Q(v)
//   if (!isconst) 

   gp_XYZ& Q  = Pc.ChangeCoord(), DQ(0, 0, 0); //Q
   if (!isconst) {
     cout << "Not implemented" << endl;
   }


   Q.Subtract(Centre);  //CQ
   DQ -= DC;

   gp_XYZ DVcrossCQ;
   DVcrossCQ.SetLinearForm(DDir.Crossed (Q), 
			   Dir.Crossed(DQ));   //Vdir^CQ
   DVcrossCQ.Multiply (Sin(U)); //(Vdir^CQ)*Sin(U)

   Standard_Real CosU =  Cos(U);
   gp_XYZ DVdotCQ;
   DVdotCQ.SetLinearForm(DDir.Dot(Q) + Dir.Dot(DQ), Dir,
			Dir.Dot(Q), DDir);//(CQ.Vdir)(1-Cos(U))Vdir
   DVdotCQ.Add (DVcrossCQ);    //addition des composantes

   DQ.Multiply (CosU);
   DQ.Add (DVdotCQ);
   DQ.Add (DC);
   DS.SetXYZ(DQ);
}

//=========================================================
// Function : Deriv2T
// Purpose : calcul of the second derivatice from t
//=========================================================

/* Standard_Boolean GeomFill_FunctionGuide::Deriv2T(const Standard_Real Param1,
						  const Standard_Real Param,
						  const Standard_Real Param0,
						  const math_Vector & R1,
						  const math_Vector & R,
						  const math_Vector & R0,
						  math_Vector& F) 
{
  math_Vector F1(1,3,0);
  math_Vector F2(1,3,0);
 
  DerivT(Param1, Param, R1, R, F1);
  DerivT(Param, Param0, R, R0, F2);
  
  Standard_Real h1 = Param - Param1;
  Standard_Real h2 = Param0 - Param;
  
  Standard_Integer i;  
  for (i=1;i<=3;i++)  
    F(i) = (F2(i) - F1(i))  / ((h2 + h1)/2);
 
  return Standard_True; 
}

//=========================================================
// Function : DerivTX
// Purpose : calcul of the second derivative from t and x
//=========================================================
 Standard_Boolean GeomFill_FunctionGuide::DerivTX(const Standard_Real Param,
						  const Standard_Real Param0,
						  const math_Vector & R,
						  const math_Vector & X0,
						  math_Matrix& D) 
{ 
  gp_Pnt P1,P2;
  gp_Vec DP1,DP2,DP2U,DP2V,DP1U,DP1V;

  TheCurve->D1(R(1), P1, DP1); // guide
  TheCurve->D1(X0(1), P2, DP2); 
  TheSurface->D1(R(2), R(3), P1, DP1U, DP1V); // surface
  TheSurface->D1(X0(2), X0(3), P2, DP2U, DP2V); //derivee de la new surface
 
  Standard_Real h = Param0 - Param;

  Standard_Integer i;
  for (i=1;i<=3;i++)
    {
      D(i,1) = (DP2.Coord(i) - DP1.Coord(i)) / h;
      //D(i,2) = - (DP2U.Coord(i) - DP1U.Coord(i)) / h;  
      D(i,2) = - DP1U.Coord(i) * (X0(2)-R(2)) / h;  
      //D(i,3) = - (DP2V.Coord(i) - DP1V.Coord(i)) / h;   
      D(i,3) = - DP1V.Coord(i) * (X0(3)-R(3)) / h;  
    }// for

  return Standard_True;
}

//=========================================================
// Function : Deriv2X
// Purpose : calcul of the second derivative from x
//=========================================================
 Standard_Boolean GeomFill_FunctionGuide::Deriv2X(const math_Vector & X,
						  GeomFill_Tensor& T) 
{ 
  gp_Pnt P,P1;
  gp_Vec DP,D2P,DPU,DPV;
  gp_Vec D2PU, D2PV, D2PUV;

  TheCurve->D2(X(1), P1, DP, D2P);
  TheSurface->D2(X(2), X(3), P, DPU, DPV, D2PU, D2PV, D2PUV);
 
  T.Init(0.); // tenseur

  Standard_Integer i;
  for (i=1;i<=3;i++)
    {
      T(i,1,1) = D2P.Coord(i);  
      T(i,2,2) = -D2PU.Coord(i);
      T(i,3,2) = T(i,2,3) = -D2PUV.Coord(i);
      T(i,3,3) = -D2PV.Coord(i);
    }// for

  return Standard_True;
}*/










































