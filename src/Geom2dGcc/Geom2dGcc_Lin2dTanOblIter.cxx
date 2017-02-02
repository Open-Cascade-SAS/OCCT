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
// CREATION D UNE LIGNE TANGENTE A UNE COURBE ET PARALLELE A UNE DROITE. +
//========================================================================

#include <GccEnt_BadQualifier.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_FunctionTanObl.hxx>
#include <Geom2dGcc_IsParallel.hxx>
#include <Geom2dGcc_Lin2dTanOblIter.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <math_FunctionRoot.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Lin2dTanOblIter::
Geom2dGcc_Lin2dTanOblIter (const Geom2dGcc_QCurve&  Qualified1 ,
                           const gp_Lin2d&          TheLin     ,
                           const Standard_Real      Param1     ,
                           const Standard_Real      TolAng     ,
                           const Standard_Real      Angle      )
{

  par1sol = 0.;
  pararg1 = 0.;
  WellDone = Standard_False;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  Paral2 = Standard_False;
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();
  Standard_Real U1 = Geom2dGcc_CurveTool::FirstParameter(Cu1);
  Standard_Real U2 = Geom2dGcc_CurveTool::LastParameter(Cu1);
  gp_Dir2d Dir(TheLin.Direction());
  Standard_Real A = Dir.X();
  Standard_Real B = Dir.Y();
  gp_Dir2d TheDirection(Dir);
  if (Abs(Angle) > Abs(TolAng)) {
    if (Abs(Abs(Angle)-M_PI) <= Abs(TolAng)) {
      Paral2 = Standard_True;
      TheDirection = Dir.Reversed();
    }
    else if (Abs(Angle-M_PI/2) <= Abs(TolAng)) { TheDirection=gp_Dir2d(-B,A); }
    else if (Abs(Angle+M_PI/2) <= Abs(TolAng)) { TheDirection=gp_Dir2d(B,-A); }
    else {
      TheDirection=gp_Dir2d(A*Cos(Angle)-B*Sin(Angle),
        A*Sin(Angle)+B*Cos(Angle));
    }
  }
  else { Paral2 = Standard_True; }
  Geom2dGcc_FunctionTanObl func(Cu1,TheDirection);
  math_FunctionRoot sol(func,Param1,
    Geom2dGcc_CurveTool::EpsX(Cu1,Abs(TolAng)),U1,U2,100);
  if (sol.IsDone()) {
    Standard_Real Usol = sol.Root();
    gp_Pnt2d Origine;
    gp_Vec2d Vect1,Vect2;
    Geom2dGcc_CurveTool::D2(Cu1,Usol,Origine,Vect1,Vect2);
    Standard_Real sign1 = Vect1.XY().Dot(TheDirection.XY());
    Standard_Real sign2 = Vect2.XY().Crossed(TheDirection.XY());
    if (Qualified1.IsUnqualified() || 
      (Qualified1.IsEnclosing() && sign2<=0.) ||
      (Qualified1.IsOutside() && sign1 <= 0. && sign2 >= 0.) ||
      (Qualified1.IsEnclosed() && sign1 >= 0. && sign2 >= 0.)) {
        WellDone = Standard_True;
        linsol = gp_Lin2d(Origine,TheDirection);
        pnttg1sol = Origine;
        qualifier1 = Qualified1.Qualifier();
        pararg1 = Usol;
        par1sol = 0.;
        if (!Paral2) {
          IntAna2d_AnaIntersection Intp(linsol,TheLin);
          if (Intp.IsDone() && !Intp.IsEmpty()) {
            if (Intp.NbPoints()==1) {
              pntint2sol = Intp.Point(1).Value();
              par2sol = gp_Vec2d(linsol.Direction()).
                Dot(gp_Vec2d(linsol.Location(),pntint2sol));
              pararg2 = gp_Vec2d(TheLin.Direction()).
                Dot(gp_Vec2d(TheLin.Location(),pntint2sol));
            }
          }
        }
    }
  }
}

Standard_Boolean Geom2dGcc_Lin2dTanOblIter::
IsDone () const { return WellDone; }

gp_Lin2d Geom2dGcc_Lin2dTanOblIter::ThisSolution () const 
{	
  if (!WellDone) throw StdFail_NotDone();

  return linsol;
}

void Geom2dGcc_Lin2dTanOblIter:: 
WhichQualifier (GccEnt_Position& Qualif1) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else {
    Qualif1 = qualifier1;
  }
}

Standard_Boolean Geom2dGcc_Lin2dTanOblIter::
IsParallel2 () const { return Paral2; }

void Geom2dGcc_Lin2dTanOblIter::
Tangency1 (Standard_Real& ParSol    ,
           Standard_Real& ParArg    ,
           gp_Pnt2d& PntSol) const {
             if (!WellDone) { throw StdFail_NotDone(); }
             else {
               ParSol = par1sol;
               ParArg = pararg1;
               PntSol = gp_Pnt2d(pnttg1sol);
             }
}

void Geom2dGcc_Lin2dTanOblIter::
Intersection2 (Standard_Real&     ParSol ,
               Standard_Real&     ParArg ,
               gp_Pnt2d& PntSol ) const {
                 if (!WellDone) { throw StdFail_NotDone(); }
                 else if (Paral2) { throw Geom2dGcc_IsParallel(); }
                 else {
                   PntSol = pntint2sol;
                   ParSol = par2sol;
                   ParArg = pararg2;
                 }
}

