// Created on: 1991-12-20
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
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

//========================================================================
//  CREATION D UNE LIGNE TANGENTE A DEUX COURBES.                        +
//========================================================================

#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanCirCu.hxx>
#include <Geom2dGcc_FunctionTanCuCu.hxx>
#include <Geom2dGcc_FunctionTanCuPnt.hxx>
#include <Geom2dGcc_Lin2d2TanIter.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <math_FunctionRoot.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Lin2d2TanIter::
Geom2dGcc_Lin2d2TanIter (const GccEnt_QualifiedCirc& Qualified1 ,
                         const Geom2dGcc_QCurve&     Qualified2 ,
                         const Standard_Real         Param2     ,
                         const Standard_Real         Tolang     ) {

                           par1sol = 0.;
                           pararg1 = 0.;

                           //Standard_Real Tol = Abs(Tolang);

                           WellDone = Standard_False;
                           if (Qualified1.IsEnclosed()) { throw GccEnt_BadQualifier(); }
                           gp_Circ2d C1 = Qualified1.Qualified();
                           Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                           Standard_Real U1 = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                           Standard_Real U2 = Geom2dGcc_CurveTool::LastParameter(Cu2);
                           Geom2dGcc_FunctionTanCirCu func(C1,Cu2);
                           math_FunctionRoot sol(func,Param2,Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolang)),U1,U2,100);
                           if (sol.IsDone()) {
                             Standard_Real Usol = sol.Root();
                             //     gp_Pnt2d Origine,Pt;
                             //  Modified by Sergey KHROMOV - Thu Apr  5 17:39:47 2001 Begin
                             Standard_Real Norm;
                             func.Value(Usol, Norm);
                             if (Abs(Norm) < Tolang) {
                               //  Modified by Sergey KHROMOV - Thu Apr  5 17:39:48 2001 End
                               gp_Pnt2d Origine;
                               gp_Vec2d Vect1;
                               gp_Vec2d Vect2;
                               Geom2dGcc_CurveTool::D2(Cu2,Usol,Origine,Vect1,Vect2);
                               gp_Vec2d Vdir(C1.Location().XY() - Origine.XY());
                               Standard_Real sign1 = Vect1.Dot(Vdir);
                               if (sign1 <= 0. ) { Vect1.Reverse(); }
                               Standard_Real sign2 = Vect2.Crossed(Vect1);
                               if (Qualified2.IsUnqualified() || 
                                 (Qualified2.IsEnclosing() && sign2<=0.) ||
                                 (Qualified2.IsOutside() && sign1 <= 0. && sign2 >= 0.) ||
                                 (Qualified2.IsEnclosed() && sign1 >= 0. && sign2 >= 0.)) {
                                   if (Qualified1.IsUnqualified() ||
                                     (Qualified1.IsOutside() && Vect1.Angle(Vdir) <= 0.) ||
                                     (Qualified1.IsEnclosing() && Vect1.Angle(Vdir) >= 0.)) {
                                       gp_Dir2d direc(Vect1);
                                       Standard_Real R1 = C1.Radius();
                                       gp_XY normal(-R1*direc.Y(),R1*direc.X());
                                       sign1 = Vect1.Crossed(Vdir);
                                       if (Qualified1.IsEnclosing()) {
                                         pnttg1sol = gp_Pnt2d(C1.Location().XY()-normal);
                                       }
                                       else if (Qualified1.IsOutside()) {
                                         pnttg1sol = gp_Pnt2d(C1.Location().XY()+normal);
                                       }
                                       else {
                                         if (sign1 >= 0.) {
                                           pnttg1sol = gp_Pnt2d(C1.Location().XY()-normal);
                                         }
                                         else {
                                           pnttg1sol = gp_Pnt2d(C1.Location().XY()+normal);
                                         }
                                       }
                                       // 	 if (gp_Vec2d(direc.XY()).Angle(gp_Vec2d(pnttg1sol,Origine)) <= Tol) {
                                       pnttg2sol = Origine;
                                       linsol = gp_Lin2d(pnttg1sol,direc);
                                       WellDone = Standard_True;
                                       qualifier1 = Qualified1.Qualifier();
                                       qualifier2 = Qualified2.Qualifier();
                                       pararg2 = Usol;
                                       par1sol = 0.;
                                       par2sol = pnttg2sol.Distance(pnttg1sol);
                                       pararg1 = 0.;
                                   }
                               }
                             }
                           }
}

Geom2dGcc_Lin2d2TanIter::
Geom2dGcc_Lin2d2TanIter (const Geom2dGcc_QCurve& Qualified1 ,
                         const Geom2dGcc_QCurve& Qualified2 ,
                         const Standard_Real      Param1     ,
                         const Standard_Real      Param2     ,
                         const Standard_Real      Tolang     ) {
                           par1sol = 0.;
                           pararg1 = 0.;
                           WellDone = Standard_False;
                           if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                             Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
                             !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
                             Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
                               throw GccEnt_BadQualifier();
                               return;
                           }
                           Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                           Geom2dAdaptor_Curve Cu2 = Qualified2.Qualified();
                           Geom2dGcc_FunctionTanCuCu Func(Cu1,Cu2);
                           math_Vector Umin(1,2);
                           math_Vector Umax(1,2);
                           math_Vector Ufirst(1,2);
                           math_Vector tol(1,2);
                           Umin(1) = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                           Umin(2) = Geom2dGcc_CurveTool::FirstParameter(Cu2);
                           Umax(1) = Geom2dGcc_CurveTool::LastParameter(Cu1);
                           Umax(2) = Geom2dGcc_CurveTool::LastParameter(Cu2);
                           Ufirst(1) = Param1;
                           Ufirst(2) = Param2;
                           tol(1) = Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolang));
                           tol(2) = Geom2dGcc_CurveTool::EpsX(Cu2,Abs(Tolang));
                           math_FunctionSetRoot Root(Func, tol);
                           Root.Perform(Func, Ufirst, Umin, Umax);
                           if (Root.IsDone()) {
                             Root.Root(Ufirst);
                             //  Modified by Sergey KHROMOV - Thu Apr  5 17:45:00 2001 Begin
                             math_Vector Norm(1,2);
                             Func.Value(Ufirst, Norm);
                             if (Abs(Norm(1)) < Tolang && Abs(Norm(2)) < Tolang) {
                               //  Modified by Sergey KHROMOV - Thu Apr  5 17:45:01 2001 End
                               gp_Pnt2d point1,point2;
                               gp_Vec2d Vect11,Vect12,Vect21,Vect22;
                               Geom2dGcc_CurveTool::D2(Cu1,Ufirst(1),point1,Vect11,Vect12);
                               Geom2dGcc_CurveTool::D2(Cu2,Ufirst(2),point2,Vect21,Vect22);
                               gp_Vec2d Vec(point1.XY(),point2.XY());
                               Standard_Real Angle1 = Vec.Angle(Vect12);
                               Standard_Real sign1 = Vect11.Dot(Vec);
                               if (Qualified1.IsUnqualified() || 
                                 (Qualified1.IsEnclosing() && Angle1 >= 0.) ||
                                 (Qualified1.IsOutside() && Angle1 <= 0. && sign1 <= 0.) ||
                                 (Qualified1.IsEnclosed() && Angle1 <= 0. && sign1 >= 0.)) {
                                   Angle1 = Vec.Angle(Vect22);
                                   sign1 = Vect21.Dot(Vec);
                                   if (Qualified2.IsUnqualified() || 
                                     (Qualified2.IsEnclosing() && Angle1 >= 0.) ||
                                     (Qualified2.IsOutside() && Angle1 <= 0. && sign1 <= 0.) ||
                                     (Qualified2.IsEnclosed() && Angle1 <= 0. && sign1 >= 0.)) {
                                       qualifier1 = Qualified1.Qualifier();
                                       qualifier2 = Qualified2.Qualifier();
                                       pararg1 = Ufirst(1);
                                       par1sol = 0.;
                                       pnttg1sol = point1;
                                       pararg2 = Ufirst(2);
                                       pnttg2sol = point2;
                                       par2sol = pnttg2sol.Distance(pnttg1sol);
                                       gp_Dir2d dir(pnttg2sol.X()-pnttg1sol.X(),pnttg2sol.Y()-pnttg1sol.Y());
                                       linsol = gp_Lin2d(pnttg1sol,dir);
                                       WellDone = Standard_True;
                                   }
                               }
                             }
                           }
}

Geom2dGcc_Lin2d2TanIter::
Geom2dGcc_Lin2d2TanIter (const Geom2dGcc_QCurve& Qualified1 ,
                         const gp_Pnt2d&          ThePoint   ,
                         const Standard_Real      Param1     ,
                         const Standard_Real      Tolang     ) {

                           par1sol = 0.;
                           pararg1 = 0.;
                           WellDone = Standard_False;
                           if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
                             Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
                               throw GccEnt_BadQualifier();
                               return;
                           }
                           Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
                           Standard_Real U1 = Geom2dGcc_CurveTool::FirstParameter(Cu1);
                           Standard_Real U2 = Geom2dGcc_CurveTool::LastParameter(Cu1);
                           Geom2dGcc_FunctionTanCuPnt func(Cu1,ThePoint);
                           math_FunctionRoot sol(func,Param1,Geom2dGcc_CurveTool::EpsX(Cu1,Abs(Tolang)),U1,U2,100);
                           if (sol.IsDone()) {
                             Standard_Real Usol = sol.Root();
                             //  Modified by Sergey KHROMOV - Thu Apr  5 17:45:17 2001 Begin
                             Standard_Real Norm;
                             func.Value(Usol, Norm);
                             if (Abs(Norm) < Tolang) {
                               //  Modified by Sergey KHROMOV - Thu Apr  5 17:45:19 2001 End
                               gp_Pnt2d Origine;
                               gp_Vec2d Vect1;
                               gp_Vec2d Vect2;
                               Geom2dGcc_CurveTool::D2(Cu1,Usol,Origine,Vect1,Vect2);
                               gp_Vec2d Vdir(ThePoint.XY()-Origine.XY());
                               Standard_Real sign1 = Vect1.Dot(Vdir);
                               Standard_Real sign2 = Vect2.Crossed(Vdir);
                               if (Qualified1.IsUnqualified() || 
                                 (Qualified1.IsEnclosing() && 
                                 ((sign1 >= 0. && sign2 <= 0.) || (sign1 <= 0. && sign2 <= 0.))) ||
                                 (Qualified1.IsOutside() && sign1 <= 0. && sign2 >= 0.) ||
                                 (Qualified1.IsEnclosed() && sign1 >= 0. && sign2 >= 0.)) {
                                   WellDone = Standard_True;
                                   linsol = gp_Lin2d(Origine,gp_Dir2d(Vdir));
                                   qualifier1 = Qualified1.Qualifier();
                                   qualifier2 = GccEnt_noqualifier;
                                   pnttg1sol = Origine;
                                   pnttg2sol = ThePoint;
                                   pararg1 = Usol;
                                   par1sol = 0.;
                                   pararg2 = ThePoint.Distance(Origine);
                                   par2sol = 0.;
                               }
                             }
                           }
}

Standard_Boolean Geom2dGcc_Lin2d2TanIter::
IsDone () const { return WellDone; }

gp_Lin2d Geom2dGcc_Lin2d2TanIter::
ThisSolution () const 
{
  if (!WellDone) throw StdFail_NotDone();
  return linsol;
}

void Geom2dGcc_Lin2d2TanIter:: 
WhichQualifier (GccEnt_Position& Qualif1  ,
                GccEnt_Position& Qualif2  ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    Qualif1 = qualifier1;
    Qualif2 = qualifier2;
  }
}

void Geom2dGcc_Lin2d2TanIter::
Tangency1 (Standard_Real& ParSol ,
           Standard_Real& ParArg ,
           gp_Pnt2d& Pnt) const {
             if (!WellDone) { throw StdFail_NotDone(); }
             else {
               ParSol = par1sol;
               ParArg = pararg1;
               Pnt    = pnttg1sol;
             }
}

void Geom2dGcc_Lin2d2TanIter::
Tangency2 (Standard_Real& ParSol ,
           Standard_Real& ParArg ,
           gp_Pnt2d& Pnt) const {
             if (!WellDone) { throw StdFail_NotDone(); }
             else {
               ParSol = par2sol;
               ParArg = pararg2;
               Pnt    = pnttg2sol;
             }
}

