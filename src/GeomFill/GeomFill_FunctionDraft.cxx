// Created on: 1998-04-27
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
#include <Adaptor3d_HSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomFill_FunctionDraft.hxx>
#include <GeomFill_Tensor.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>

//#include <Precision.hxx>
//*******************************************************
//   Calcul de la valeur de la fonction :
//                      G(w(t)) - S(u(t),v(t)) = 0
//  ou     G = generatrice   et   S = surface d'arret
//   et de ses derivees
//*******************************************************
//*******************************************************
// Function : FunctionDraft
// Purpose : Initialisation de la section et de la surface d'arret
//*******************************************************
GeomFill_FunctionDraft::GeomFill_FunctionDraft
  (const Handle(Adaptor3d_HSurface)& S, const Handle(Adaptor3d_HCurve)& C)
{
  TheCurve = C ;
  TheSurface = S;
}

//*******************************************************
// Function : NbVariables (t, u, v)
// Purpose :
//*******************************************************
 Standard_Integer GeomFill_FunctionDraft::NbVariables()const 
{
  return 3;
}

//*******************************************************
// Function : NbEquations
// Purpose :
//*******************************************************
 Standard_Integer GeomFill_FunctionDraft::NbEquations()const
{
  return 3;
}

//*******************************************************
// Function : Value
// Purpose : calcul of the value of the function at <X>
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::Value(const math_Vector& X,
					        math_Vector& F) 
{
  gp_Pnt P,P1;
  TheCurve->D0(X(1), P);
  TheSurface->D0(X(2), X(3), P1);

  F(1) = P.Coord(1) - P1.Coord(1);
  F(2) = P.Coord(2) - P1.Coord(2);
  F(3) = P.Coord(3) - P1.Coord(3);

  return Standard_True;
}

//*******************************************************
// Function : Derivatives
// Purpose :calcul of the derivative of the function
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::Derivatives(const math_Vector& X,
						      math_Matrix& D) 
{
  Standard_Integer i;
  gp_Pnt P,P1;
  gp_Vec DP,DP1U,DP1V;
  TheCurve->D1(X(1),P,DP);
  TheSurface->D1(X(2),X(3),P1,DP1U,DP1V);

  for (i=1;i<=3;i++)
    {
      D(i,1) = DP.Coord(i);  
      D(i,2) = -DP1U.Coord(i);
      D(i,3) = -DP1V.Coord(i);
    }
  return Standard_True;
}

//*******************************************************
// Function : Values
// Purpose : calcul of the value and the derivative of the function
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::Values(const math_Vector& X,
						math_Vector& F,
						math_Matrix& D) 
{ 
  Standard_Integer i;
  gp_Pnt P,P1;
  gp_Vec DP,DP1U,DP1V;
  TheCurve->D1(X(1),P,DP); //derivee de la generatrice
  TheSurface->D1(X(2),X(3),P1,DP1U,DP1V); //derivee de la new surface

  for (i=1;i<=3;i++)
    { 
      F(i) = P.Coord(i) - P1.Coord(i);

      D(i,1) = DP.Coord(i);  
      D(i,2) = -DP1U.Coord(i);
      D(i,3) = -DP1V.Coord(i);
    }

  return Standard_True;
}

//*******************************************************
// Function : DerivT
// Purpose : calcul of the first derivative from t 
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::DerivT(const Handle(Adaptor3d_HCurve)& C,
						const Standard_Real Param,
						const Standard_Real W,
						const gp_Vec & dN,
					        const Standard_Real teta,
					        math_Vector& F) 

{ 
  gp_Pnt P;
  gp_Vec DP;

  C->D1(Param, P, DP); // derivee de la section

  F(1) = DP.Coord(1) + W * dN.Coord(1) * Sin(teta);
  F(2) = DP.Coord(2) + W * dN.Coord(2) * Sin(teta);
  F(3) = DP.Coord(3) + W * dN.Coord(3) * Sin(teta);
  
  return Standard_True;
}

//*******************************************************
// Function : Deriv2T
// Purpose : calcul of the second derivatice from t
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::Deriv2T(const Handle(Adaptor3d_HCurve)& C,
						 const Standard_Real Param,
						 const Standard_Real W,
						 const gp_Vec & d2N,
						 const Standard_Real teta,
						 math_Vector& F) 
{ 
  gp_Pnt P;
  gp_Vec DP,D2P;

  C->D2(Param, P, DP, D2P); // derivee de la section

  F(1) = D2P.Coord(1) + W * d2N.Coord(1) * Sin(teta);
  F(2) = D2P.Coord(2) + W * d2N.Coord(2) * Sin(teta);
  F(3) = D2P.Coord(3) + W * d2N.Coord(3) * Sin(teta);
  
  return Standard_True;
}

//*******************************************************
// Function : DerivTX
// Purpose : calcul of the second derivative from t and x
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::DerivTX(const gp_Vec & dN,
						  const Standard_Real teta,
						  math_Matrix& D) 
{ 
//  gp_Pnt P;
//  gp_Vec DP,D2P;

  Standard_Integer i;
  for (i=1;i<=3;i++)
    {
      D(i,1) = dN.Coord(i)*Sin(teta); //derivee / W
      D(i,2) = 0.;  // derivee / U
      D(i,3) = 0.;  // derivee / V
    }

  return Standard_True;
}

//*******************************************************
// Function : Deriv2X
// Purpose : calcul of the second derivative from x
//*******************************************************
 Standard_Boolean GeomFill_FunctionDraft::Deriv2X(const math_Vector & X,
						  GeomFill_Tensor& T) 
{ 
  gp_Pnt P;
  gp_Vec DPu,DPv;
  gp_Vec D2Pu, D2Pv, D2Puv;
  Standard_Integer i;

  TheSurface->D2(X(2), X(3), P, DPu, DPv, D2Pu, D2Pv, D2Puv);
 
  T.Init(0.); // tenseur

  for (i=1;i<=3;i++)
    {
      T(i,2,2) = -D2Pu.Coord(i);
      T(i,3,2) = T(i,2,3) = -D2Puv.Coord(i);
      T(i,3,3) = -D2Pv.Coord(i);
    }

  return Standard_True;
}



