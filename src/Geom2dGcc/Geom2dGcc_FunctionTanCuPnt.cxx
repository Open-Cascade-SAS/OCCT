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
#include <Geom2dGcc_FunctionTanCuPnt.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

//=========================================================================
//  soit P1 le point sur la courbe Geom2dAdaptor_Curve d abscisse u.      +
//  soit C  le point ThePoint.                                            +
//  Nous cherchons donc les zeros de la fonction suivante:                +
//                                                                        +
//                 -->   -->                                              +
//                 CP1 /\ T                                               +
//             ---------------  =  F(u)                                   +
//             ||CP1|| * ||T||                                            +
//                                                                        +
//  La derivee de cette fonction est :                                    +
//            CP1 /\ N        (T.N)*((CP1/\T).((CP1/\T))                  +
//     f(u) = --------  -  --------------------------------               +
//               N.N            N*N*N*CP1*CP1*CP1                         +
//=========================================================================
Geom2dGcc_FunctionTanCuPnt::
Geom2dGcc_FunctionTanCuPnt(const Geom2dAdaptor_Curve& C      ,
                           const gp_Pnt2d& Point  ) {
                             TheCurv = C;
                             ThePoint = Point;
}


Standard_Boolean Geom2dGcc_FunctionTanCuPnt::
Value (const Standard_Real  X    ,
       Standard_Real& Fval ) {
         gp_Pnt2d Point;
         gp_Vec2d Vect;
         Geom2dGcc_CurveTool::D1(TheCurv,X,Point,Vect);
         Standard_Real NormeD1 = Vect.Magnitude();
         gp_Vec2d TheDirection(ThePoint,Point);
         Standard_Real NormeDir = TheDirection.Magnitude();
         Fval = TheDirection.Crossed(Vect)/(NormeD1*NormeDir);
         return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCuPnt::
Derivative (const Standard_Real  X     ,
            Standard_Real& Deriv ) {
              gp_Pnt2d Point;
              gp_Vec2d Vec1;
              gp_Vec2d Vec2;
              Geom2dGcc_CurveTool::D2(TheCurv,X,Point,Vec1,Vec2);
              gp_Vec2d TheDirection(ThePoint.XY(),gp_XY(Point.XY()));
              Standard_Real NormeD1 = Vec1.Magnitude();
              Standard_Real NormeDir = TheDirection.Magnitude();
              Deriv = TheDirection.Crossed(Vec2)/(NormeD1*NormeDir)-
                (TheDirection.Crossed(Vec1)/(NormeD1*NormeDir))*
                (Vec1.Dot(Vec2)/(NormeD1*NormeD1)+
                Vec1.Dot(TheDirection)/(NormeDir*NormeDir));
              return Standard_True;
}

Standard_Boolean Geom2dGcc_FunctionTanCuPnt::
Values (const Standard_Real  X     ,
        Standard_Real& Fval  ,
        Standard_Real& Deriv ) {
          gp_Pnt2d Point;
          gp_Vec2d Vec1;
          gp_Vec2d Vec2;
          Geom2dGcc_CurveTool::D2(TheCurv,X,Point,Vec1,Vec2);
          gp_Vec2d TheDirection(ThePoint.XY(),gp_XY(Point.XY()));
          Standard_Real NormeD1 = Vec1.Magnitude();
          Standard_Real NormeDir = TheDirection.Magnitude();
          Fval = TheDirection.Crossed(Vec1)/(NormeD1*NormeDir);
          Deriv = TheDirection.Crossed(Vec2)/(NormeD1*NormeDir)-
            (TheDirection.Crossed(Vec1)/(NormeD1*NormeDir))*
            (Vec1.Dot(Vec2)/(NormeD1*NormeD1)+
            Vec1.Dot(TheDirection)/(NormeDir*NormeDir));

          //  cout  << "U = "<< X << " F ="<< Fval <<" DF ="<< Deriv<<endl;

          return Standard_True;
}
