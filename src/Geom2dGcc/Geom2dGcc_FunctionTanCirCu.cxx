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


#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanCirCu.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

//=========================================================================
//  soit P1 le point sur la courbe Geom2dAdaptor_Curve d abscisse u.      +
//  soit C  le centre du cercle TheCirc.                                  +
//  Nous recherchons un point P2 appartenant au cercle tel que :          +
//           --->   -->                                                   +
//        *  P1P2 . CP2 = 0                                               +
//                                                                        +
//        *    -->  2    2                                                +
//           ||CP2||  = R                                                 +
//  Nous cherchons donc les zeros de la fonction suivante:                +
//                         -->  --> 2                                     +
//             -->  2    ( CP1 . T )      2                               +
//           ||CP1||  -  -----------  -  R   =  F(u)                      +
//                          --> 2                                         +
//                         ||T||                                          +
//                                                                        +
//  La derivee de cette fonction est :                                    +
//                                                                        +
//             2*(CP1.T)(CP1.N)     2*(CP1.T)*(CP1.T)*T.N                 +
//  f(u) =  -  ----------------  +  ---------------------                 +
//                  T.T                  (T.T)*(T.T)                      +
//=========================================================================
//                                                                        +
// skv: Small addition: The function and the derivative are normalized    +
//                      by an average square distance between the circle  +
//                      and the curve.                                    +
//=========================================================================
Geom2dGcc_FunctionTanCirCu::
Geom2dGcc_FunctionTanCirCu(const gp_Circ2d& Circ   ,
                           const Geom2dAdaptor_Curve&  Curv   ) {
                             Curve = Curv;
                             TheCirc = Circ;

                             //  Modified by Sergey KHROMOV - Thu Apr  5 09:51:21 2001 Begin
                             Standard_Integer aNbSamp = Geom2dGcc_CurveTool::NbSamples(Curve);
                             Standard_Real    aFirst  = Geom2dGcc_CurveTool::FirstParameter(Curve);
                             Standard_Real    aLast   = Geom2dGcc_CurveTool::LastParameter(Curve);
                             Standard_Real    aStep   = (aLast - aFirst)/aNbSamp;
                             Standard_Real    anX     = aFirst + aStep/2.;
                             Standard_Integer aNbP    = 0;
                             gp_XY            aLoc(0., 0.);

                             while (anX <= aLast) {
                               aLoc += (Geom2dGcc_CurveTool::Value(Curve, anX)).XY();
                               anX  += aStep;
                               aNbP++;
                             }
                             myWeight = Max((aLoc - TheCirc.Location().XY()).SquareModulus(), TheCirc.Radius());
                             //  Modified by Sergey KHROMOV - Thu Apr  5 09:51:25 2001 End
}


Standard_Boolean Geom2dGcc_FunctionTanCirCu::
Value (const Standard_Real  X    ,
       Standard_Real& Fval ) {
         gp_Pnt2d Point;
         gp_Vec2d Vect1;
         Geom2dGcc_CurveTool::D1(Curve,X,Point,Vect1);
         Standard_Real NormeD1 = Vect1.Magnitude();
         gp_Vec2d TheDirection(TheCirc.Location(),Point);
         Standard_Real squaredir = TheDirection.Dot(TheDirection);
         Standard_Real R = TheCirc.Radius();
         Fval = squaredir-R*R-
           (TheDirection.Dot(Vect1))*(TheDirection.Dot(Vect1))/(NormeD1*NormeD1);
         //  Modified by Sergey KHROMOV - Thu Apr  5 17:38:05 2001 Begin
         Fval /= myWeight;
         //  Modified by Sergey KHROMOV - Thu Apr  5 17:38:06 2001 End
         return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCirCu::
Derivative (const Standard_Real  X     ,
            Standard_Real& Deriv ) {
              gp_Pnt2d Point;
              gp_Vec2d Vect1,Vect2;
              Geom2dGcc_CurveTool::D2(Curve,X,Point,Vect1,Vect2);
              Standard_Real NormeD1 = Vect1.SquareMagnitude();
              gp_Vec2d TheDirection(TheCirc.Location(),Point);
              Standard_Real cp1dott = TheDirection.Dot(Vect1);
              Deriv = -2.*(cp1dott/NormeD1)*
                ((TheDirection.Dot(Vect2))-cp1dott*Vect1.Dot(Vect2)/NormeD1);
              //  Modified by Sergey KHROMOV - Thu Apr  5 17:38:15 2001 Begin
              Deriv /= myWeight;
              //  Modified by Sergey KHROMOV - Thu Apr  5 17:38:15 2001 End
              return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCirCu::
Values (const Standard_Real  X     ,
        Standard_Real& Fval  ,
        Standard_Real& Deriv ) {
          gp_Pnt2d Point;
          gp_Vec2d Vect1,Vect2;
          Geom2dGcc_CurveTool::D2(Curve,X,Point,Vect1,Vect2);
          Standard_Real NormeD1 = Vect1.SquareMagnitude();
          gp_Vec2d TheDirection(TheCirc.Location(),Point);
          Standard_Real squaredir = TheDirection.SquareMagnitude();
          Standard_Real cp1dott = TheDirection.Dot(Vect1);
          Standard_Real R = TheCirc.Radius();

          Fval = squaredir-R*R-cp1dott*cp1dott/NormeD1;
          //  Modified by Sergey KHROMOV - Thu Apr  5 17:38:28 2001 Begin
          Fval /= myWeight;
          //  Modified by Sergey KHROMOV - Thu Apr  5 17:38:28 2001 End

          Deriv = -2.*(cp1dott/NormeD1)*
            ((TheDirection.Dot(Vect2))-cp1dott*Vect1.Dot(Vect2)/NormeD1);
          //  Modified by Sergey KHROMOV - Thu Apr  5 17:37:36 2001 Begin
          Deriv /= myWeight;
          //  Modified by Sergey KHROMOV - Thu Apr  5 17:37:37 2001 End
          return Standard_True;
}
