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
#include <Geom2dGcc_FunctionTanCuCu.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_Matrix.hxx>
#include <Standard_ConstructionError.hxx>

void Geom2dGcc_FunctionTanCuCu::
InitDerivative(const math_Vector& X,
               gp_Pnt2d&    Point1,
               gp_Pnt2d&    Point2,
               gp_Vec2d&    Tan1  ,
               gp_Vec2d&    Tan2  ,
               gp_Vec2d&    D21   ,
               gp_Vec2d&    D22   )
{
  switch (TheType)
  {
  case Geom2dGcc_CuCu:
    {
      Geom2dGcc_CurveTool::D2(TheCurve1,X(1),Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(TheCurve2,X(2),Point2,Tan2,D22);
    }
    break;
  case Geom2dGcc_CiCu:
    {
      ElCLib::D2(X(1),TheCirc1,Point1,Tan1,D21);
      Geom2dGcc_CurveTool::D2(TheCurve2,X(2),Point2,Tan2,D22);
    }
    break;
  default:
    {
    }
  }
}

Geom2dGcc_FunctionTanCuCu::
Geom2dGcc_FunctionTanCuCu(const Geom2dAdaptor_Curve& C1  ,
                          const Geom2dAdaptor_Curve& C2  ) {
                            TheCurve1 = C1;
                            TheCurve2 = C2;
                            TheType = Geom2dGcc_CuCu;
}

Geom2dGcc_FunctionTanCuCu::
Geom2dGcc_FunctionTanCuCu(const gp_Circ2d& C1  ,
                          const Geom2dAdaptor_Curve&  C2  ) {
                            TheCirc1 = C1;
                            TheCurve2 = C2;
                            TheType = Geom2dGcc_CiCu;
}


//=========================================================================
//  soit P1 le point sur la courbe TheCurve1 d abscisse u1.               +
//  soit P2 le point sur la courbe TheCurve2 d abscisse u2.               +
//  soit T1 la tangente a la courbe TheCurve1 en P1.                      +
//  soit T2 la tangente a la courbe TheCurve2 en P2.                      +
//  Nous voulons P1 et P2 tels que :                                      +
//           --->    -->                                                  +
//        *  P1P2 /\ T1 = 0                                               +
//                                                                        +
//           -->   -->                                                    +
//        *  T1 /\ T2 = 0                                                 +
//                                                                        +
//  Nous cherchons donc les zeros des fonctions suivantes:                +
//             --->    -->                                                +
//        *    P1P2 /\ T1                                                 +
//          --------------- = F1(u)                                       +
//            --->     -->                                                +
//          ||P1P2||*||T1||                                               +
//                                                                        +
//              -->   -->                                                 +
//        *     T1 /\ T2                                                  +
//          --------------- = F2(u)                                       +
//             -->    -->                                                 +
//           ||T2||*||T1||                                                +
//                                                                        +
//  Les derivees de ces fonctions sont :                                  +
//                                           2              2             +
//   dF1        P1P2/\N1       (P1P2/\T1)*[T1*(-T1).P1P2+P1P2*(T1.N1)]    +
//  ----- = --------------- - -----------------------------------------   +
//   du1                                     3        3                   +
//           ||P1P2||*||T1||         ||P1P2|| * ||T1||                    +
//                                                                        +
//                                                    2                   +
//   dF1        T2/\T1                  (P1P2/\T1)*[T1*(T2.P1P2)          +
//  ----- = --------------- - -----------------------------------------   +
//   du2                                     3        3                   +
//           ||P1P2||*||T1||         ||P1P2|| * ||T1||                    +
//                                                                        +
//                                                          2             +
//   dF2             N1/\T2                 T1/\T2*(N1.T1)T2              +
//  ----- =     ----------------  -  -----------------------------        +
//   du1                                          3        3              +
//                ||T1||*||T2||             ||T1|| * ||T2||               +
//                                                                        +
//                                                          2             +
//   dF2             T1/\N2                 T1/\T2*(N2.T2)T1              +
//  ----- =     ----------------  -  -----------------------------        +
//   du2                                          3        3              +
//                ||T1||*||T2||             ||T1|| * ||T2||               +
//                                                                        +
//=========================================================================

Standard_Integer Geom2dGcc_FunctionTanCuCu::
NbVariables() const { return 2; }

Standard_Integer Geom2dGcc_FunctionTanCuCu::
NbEquations() const { return 2; }

Standard_Boolean Geom2dGcc_FunctionTanCuCu::
Value (const math_Vector& X    ,
       math_Vector& Fval ) {
         gp_Pnt2d Point1;
         gp_Pnt2d Point2;
         gp_Vec2d Vect11;
         gp_Vec2d Vect21;
         gp_Vec2d Vect12;
         gp_Vec2d Vect22;
         InitDerivative(X,Point1,Point2,Vect11,Vect21,Vect12,Vect22);
         Standard_Real NormeD11 = Vect11.Magnitude();
         Standard_Real NormeD21 = Vect21.Magnitude();
         gp_Vec2d TheDirection(Point1,Point2);
         Standard_Real squaredir = TheDirection.Dot(TheDirection);
         Fval(1) = TheDirection.Crossed(Vect11)/(NormeD11*squaredir);
         Fval(2) = Vect11.Crossed(Vect21)/(NormeD11*NormeD21);
         return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCuCu::
Derivatives (const math_Vector& X     ,
             math_Matrix& Deriv ) {
               gp_Pnt2d Point1;
               gp_Pnt2d Point2;
               gp_Vec2d Vect11;
               gp_Vec2d Vect21;
               gp_Vec2d Vect12;
               gp_Vec2d Vect22;
               InitDerivative(X,Point1,Point2,Vect11,Vect21,Vect12,Vect22);
               Standard_Real NormeD11 = Vect11.Magnitude();
               Standard_Real NormeD21 = Vect21.Magnitude();
#ifdef OCCT_DEBUG
               gp_Vec2d V2V1(Vect11.XY(),Vect21.XY());
#else
               Vect11.XY();
               Vect21.XY();
#endif
               gp_Vec2d TheDirection(Point1,Point2);
               Standard_Real squaredir = TheDirection.Dot(TheDirection);
               Deriv(1,1) = TheDirection.Crossed(Vect12)/(NormeD11*squaredir)+
                 (TheDirection.Crossed(Vect11)*NormeD11*NormeD11*Vect11.Dot(TheDirection))/
                 (NormeD11*NormeD11*NormeD11*squaredir*squaredir*squaredir);
               Deriv(1,2) = Vect21.Crossed(Vect11)/(NormeD11*squaredir)-
                 (TheDirection.Crossed(Vect11)*NormeD11*NormeD11*Vect21.Dot(TheDirection))/
                 (NormeD11*NormeD11*NormeD11*squaredir*squaredir*squaredir);
               Deriv(2,1)=(Vect12.Crossed(Vect21))/(NormeD11*NormeD21)-
                 (Vect11.Crossed(Vect21))*(Vect12.Dot(Vect11))*NormeD21*NormeD21/
                 (NormeD11*NormeD11*NormeD11*NormeD21*NormeD21*NormeD21);
               Deriv(2,2)=(Vect11.Crossed(Vect22))/(NormeD11*NormeD21)-
                 (Vect11.Crossed(Vect21))*(Vect22.Dot(Vect21))*NormeD11*NormeD11/
                 (NormeD11*NormeD11*NormeD11*NormeD21*NormeD21*NormeD21);
               return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCuCu::
Values (const math_Vector& X     ,
        math_Vector& Fval  ,
        math_Matrix& Deriv ) {
          gp_Pnt2d Point1;
          gp_Pnt2d Point2;
          gp_Vec2d Vect11;
          gp_Vec2d Vect21;
          gp_Vec2d Vect12;
          gp_Vec2d Vect22;
          InitDerivative(X,Point1,Point2,Vect11,Vect21,Vect12,Vect22);
          Standard_Real NormeD11 = Vect11.Magnitude();
          Standard_Real NormeD21 = Vect21.Magnitude();
#ifdef OCCT_DEBUG
          gp_Vec2d V2V1(Vect11.XY(),Vect21.XY());
#else
          Vect11.XY();
          Vect21.XY();
#endif
          gp_Vec2d TheDirection(Point1,Point2);
          Standard_Real squaredir = TheDirection.Dot(TheDirection);
          Fval(1) = TheDirection.Crossed(Vect11)/(NormeD11*squaredir);
          Fval(2) = Vect11.Crossed(Vect21)/(NormeD11*NormeD21);
          Deriv(1,1) = TheDirection.Crossed(Vect12)/(NormeD11*squaredir)+
            (TheDirection.Crossed(Vect11)*NormeD11*NormeD11*Vect11.Dot(TheDirection))/
            (NormeD11*NormeD11*NormeD11*squaredir*squaredir*squaredir);
          Deriv(1,2) = Vect21.Crossed(Vect11)/(NormeD11*squaredir)-
            (TheDirection.Crossed(Vect11)*NormeD11*NormeD11*Vect21.Dot(TheDirection))/
            (NormeD11*NormeD11*NormeD11*squaredir*squaredir*squaredir);
          Deriv(2,1)=(Vect12.Crossed(Vect21))/(NormeD11*NormeD21)-
            (Vect11.Crossed(Vect21))*(Vect12.Dot(Vect11))*NormeD21*NormeD21/
            (NormeD11*NormeD11*NormeD11*NormeD21*NormeD21*NormeD21);
          Deriv(2,2)=(Vect11.Crossed(Vect22))/(NormeD11*NormeD21)-
            (Vect11.Crossed(Vect21))*(Vect22.Dot(Vect21))*NormeD11*NormeD11/
            (NormeD11*NormeD11*NormeD11*NormeD21*NormeD21*NormeD21);
          return Standard_True;
}
