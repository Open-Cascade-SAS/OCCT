// Created on: 1991-12-13
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

//=========================================================================
//   Creation d un cercle tangent a deux elements : Droite.               +
//                                                  Cercle.               +
//                                                  Point.                +
//                                                  Courbes.              +
//                        centre sur un troisieme : Droite.               +
//                                                  Cercle.               +
//                                                  Courbes.              +
//=========================================================================

#include <Adaptor2d_OffsetCurve.hxx>
#include <ElCLib.hxx>
#include <GccAna_Circ2dBisec.hxx>
#include <GccAna_CircLin2dBisec.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccAna_LinPnt2dBisec.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BHyper.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dGcc_Circ2d2TanOnGeo.hxx>
#include <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

Geom2dGcc_Circ2d2TanOnGeo::
Geom2dGcc_Circ2d2TanOnGeo (const GccEnt_QualifiedCirc&     Qualified1 ,
                           const GccEnt_QualifiedCirc&     Qualified2 ,
                           const Geom2dAdaptor_Curve&      OnCurv     ,
                           const Standard_Real             Tolerance  ):
  cirsol(1,8)    ,
  qualifier1(1,8),
  qualifier2(1,8),
  TheSame1(1,8)  ,
  TheSame2(1,8)  ,
  pnttg1sol(1,8) ,
  pnttg2sol(1,8) ,
  pntcen(1,8)    ,
  par1sol(1,8)   ,
  par2sol(1,8)   ,
  pararg1(1,8)   ,
  pararg2(1,8)   ,
  parcen3(1,8)   
{
  WellDone = Standard_False;
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  NbrSol = 0;
  TColStd_Array1OfReal Rbid(1,2);
  TColStd_Array1OfReal RBid(1,2);
  TColStd_Array1OfReal Radius(1,2);
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
    !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
    Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Circ2d C2 = Qualified2.Qualified();
  Standard_Real R1 = C1.Radius();
  Standard_Real R2 = C2.Radius();
  gp_Dir2d dirx(1.,0.);
  gp_Pnt2d center1(C1.Location());
  gp_Pnt2d center2(C2.Location());
  GccAna_Circ2dBisec Bis(C1,C2);
  if (Bis.IsDone()) {
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    Standard_Integer nbsolution = Bis.NbSolutions();
    Handle(Geom2dAdaptor_HCurve) HCu2 = new Geom2dAdaptor_HCurve(OnCurv); 
    Adaptor2d_OffsetCurve Cu2(HCu2,0.);
    firstparam = Max(Cu2.FirstParameter(),thefirst);
    lastparam  = Min(Cu2.LastParameter(),thelast);
    IntRes2d_Domain D2(Cu2.Value(firstparam), firstparam, Tol,
                       Cu2.Value(lastparam), lastparam, Tol);
    Standard_Real Tol1 = Abs(Tolerance);
    Standard_Real Tol2 = Tol1;
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      switch (type) {
      case GccInt_Cir:
        {
          gp_Circ2d Circ(Sol->Circle());
          IntRes2d_Domain D1(ElCLib::Value(0.,Circ),   0.,Tol1,
            ElCLib::Value(2.*M_PI,Circ),2.*M_PI,Tol2);
          D1.SetEquivalentParameters(0.,2.*M_PI);
          Intp.Perform(Circ,D1,Cu2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Ell:
        {
          gp_Elips2d Elips(Sol->Ellipse());
          IntRes2d_Domain D1(ElCLib::Value(0.,Elips),   0.,Tol1,
            ElCLib::Value(2.*M_PI,Elips),2.*M_PI,Tol2);
          D1.SetEquivalentParameters(0.,2.*M_PI);
          Intp.Perform(Elips,D1,Cu2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Hpr:
        {
          gp_Hypr2d Hypr(Sol->Hyperbola());
          IntRes2d_Domain D1(ElCLib::Value(-4.,Hypr),-4.,Tol1,
            ElCLib::Value(4.,Hypr),4.,Tol2);
          Intp.Perform(Hypr,D1,Cu2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Lin:
        {
          gp_Lin2d Line(Sol->Line());
          IntRes2d_Domain D1;
          Intp.Perform(Line,D1,Cu2,D2,Tol1,Tol2);
        }
        break;
      default:
        {
          throw Standard_ConstructionError();
        }
      }
      if (Intp.IsDone()) {
        if ((!Intp.IsEmpty())) {
          for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
            gp_Pnt2d Center(Intp.Point(j).Value());
            Standard_Real dist1 = Center.Distance(C1.Location());
            Standard_Real dist2 = Center.Distance(C2.Location());
            Standard_Integer nbsol = 0;
            Standard_Integer nnsol = 0;
            R1 = C1.Radius();
            R2 = C2.Radius();
            if (Qualified1.IsEnclosed()) {
              if (dist1-R1 < Tol) { 
                nbsol = 1;
                Rbid(1) = Abs(R1-dist1);
              }
            }
            else if (Qualified1.IsOutside()) {
              if (R1-dist1 < Tol) { 
                nbsol = 1;
                Rbid(1) = Abs(dist1-R1);
              }
            }
            else if (Qualified1.IsEnclosing()) {
              nbsol = 1;
              Rbid(1) = dist1+R1;
            }
            else if (Qualified1.IsUnqualified()) {
              nbsol = 2;
              Rbid(1) = dist1+R1;
              Rbid(1) = Abs(dist1-R1);
            }
            if (Qualified2.IsEnclosed() && nbsol != 0) {
              if (dist2-R2 < Tol) {
                RBid(1) = Abs(R2-dist2);
              }
            }
            else if (Qualified2.IsOutside() && nbsol != 0) {
              if (R2-dist2 < Tol) {
                RBid(1) = Abs(R2-dist2);
              }
            }
            else if (Qualified2.IsEnclosing() && nbsol != 0) {
              RBid(1) = dist2+R2;
            }
            else if (Qualified2.IsUnqualified() && nbsol != 0) {
              RBid(1) = dist2+R2;
              RBid(2) = Abs(R2-dist2);
            }
            for (Standard_Integer isol = 1; isol <= nbsol ; isol++) {
              for (Standard_Integer jsol = 1; jsol <= nbsol ; jsol++) {
                if (Abs(Rbid(isol)-RBid(jsol)) <= Tol) {
                  nnsol++;
                  Radius(nnsol) = (RBid(jsol)+Rbid(isol))/2.;
                }
              }
            }
            if (nnsol > 0) {
              for (Standard_Integer k = 1 ; k <= nnsol ; k++) {
                NbrSol++;
                cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
                //              ==========================================================
                Standard_Real distcc1 = Center.Distance(center1);
                Standard_Real distcc2 = Center.Distance(center2);
                if (!Qualified1.IsUnqualified()) { 
                  qualifier1(NbrSol) = Qualified1.Qualifier();
                }
                else if (Abs(distcc1+Radius(i)-R1) < Tol) {
                  qualifier1(NbrSol) = GccEnt_enclosed;
                }
                else if (Abs(distcc1-R1-Radius(i)) < Tol) {
                  qualifier1(NbrSol) = GccEnt_outside;
                }
                else { qualifier1(NbrSol) = GccEnt_enclosing; }
                if (!Qualified2.IsUnqualified()) { 
                  qualifier2(NbrSol) = Qualified2.Qualifier();
                }
                else if (Abs(distcc2+Radius(i)-R2) < Tol) {
                  qualifier2(NbrSol) = GccEnt_enclosed;
                }
                else if (Abs(distcc2-R2-Radius(i)) < Tol) {
                  qualifier2(NbrSol) = GccEnt_outside;
                }
                else { qualifier2(NbrSol) = GccEnt_enclosing; }
                if (dist1 <= Tol && Abs(Radius(k)-C1.Radius()) <= Tol) {
                  TheSame1(NbrSol) = 1;
                }
                else {
                  TheSame1(NbrSol) = 0;
                  gp_Dir2d dc1(C1.Location().XY()-Center.XY());
                  pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k)*dc1.XY());
                  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                    pnttg1sol(NbrSol));
                  pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
                }
                if (dist2 <= Tol && Abs(Radius(k)-C2.Radius()) <= Tol) {
                  TheSame2(NbrSol) = 1;
                }
                else {
                  TheSame2(NbrSol) = 0;
                  gp_Dir2d dc2(C2.Location().XY()-Center.XY());
                  pnttg2sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(k)*dc2.XY());
                  par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                    pnttg2sol(NbrSol));
                  pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
                }
                pntcen(NbrSol) = Center;
                parcen3(NbrSol) = Intp.Point(j).ParamOnSecond();
              }
              WellDone = Standard_True;
            }
          }
        }
      }
    }
  }
}

//=========================================================================
//   Creation d un cercle tangent a un Cercle C1 et a une Droite L2.      +
//                        centre sur une courbe OnCurv.                   +
//  Nous calculons les bissectrices a C1 et L2 qui nous donnent           +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tangents a C1 et L2.                                                  +
//  Nous intersectons ces bissectrices avec la courbe OnCurv ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant C1 et L2.  +
//=========================================================================

Geom2dGcc_Circ2d2TanOnGeo::
Geom2dGcc_Circ2d2TanOnGeo (const GccEnt_QualifiedCirc&     Qualified1 , 
                           const GccEnt_QualifiedLin&      Qualified2 , 
                           const Geom2dAdaptor_Curve&                 OnCurv     ,
                           const Standard_Real             Tolerance  ):
cirsol(1,8)    ,
qualifier1(1,8),
qualifier2(1,8),
TheSame1(1,8)  ,
TheSame2(1,8)  ,
pnttg1sol(1,8) ,
pnttg2sol(1,8) ,
pntcen(1,8)    ,
par1sol(1,8)   ,
par2sol(1,8)   ,
pararg1(1,8)   ,
pararg2(1,8)   ,
parcen3(1,8)   
{

  WellDone = Standard_False;
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  NbrSol = 0;
  Standard_Real Tol = Abs(Tolerance);
  Standard_Real Radius;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
    !(Qualified2.IsEnclosed() ||
    Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  gp_Dir2d dirx(1.,0.);
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Lin2d L2 = Qualified2.Qualified();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(C1.Location());
  gp_Pnt2d origin2(L2.Location());
  gp_Dir2d dir2(L2.Direction());
  gp_Dir2d normL2(-dir2.Y(),dir2.X());

  GccAna_CircLin2dBisec Bis(C1,L2);
  if (Bis.IsDone()) {
    Standard_Real Tol1 = Abs(Tolerance);
    Standard_Real Tol2 = Tol1;
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    Standard_Integer nbsolution = Bis.NbSolutions();
    Handle(Geom2dAdaptor_HCurve) HCu2 = new Geom2dAdaptor_HCurve(OnCurv); 
    Adaptor2d_OffsetCurve C2(HCu2,0.);
    firstparam = Max(C2.FirstParameter(),thefirst);
    lastparam  = Min(C2.LastParameter(),thelast);
    IntRes2d_Domain D2(C2.Value(firstparam), firstparam, Tol,
                       C2.Value(lastparam), lastparam, Tol);
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      switch (type) {
      case GccInt_Lin:
        {
          gp_Lin2d Line(Sol->Line());
          IntRes2d_Domain D1;
          Intp.Perform(Line,D1,C2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Par:
        {
          gp_Parab2d Parab(Sol->Parabola());
          IntRes2d_Domain D1(ElCLib::Value(-40,Parab),-40,Tol1,
            ElCLib::Value(40,Parab),40,Tol1);
          Intp.Perform(Parab,D1,C2,D2,Tol1,Tol2);
        }
        break;
      default:
        {
          throw Standard_ConstructionError();
        }
      }
      if (Intp.IsDone()) {
        if (!Intp.IsEmpty()) {
          for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
            gp_Pnt2d Center(Intp.Point(j).Value());
            Standard_Real dist1 = Center.Distance(center1);
            //	    Standard_Integer nbsol = 1;
            Standard_Boolean ok = Standard_False;
            if (Qualified1.IsEnclosed()) {
              if (dist1-R1 < Tol) { ok = Standard_True; }
            }
            else if (Qualified1.IsOutside()) {
              if (R1-dist1 < Tol) { ok = Standard_True; }
            }
            else if (Qualified1.IsEnclosing() || Qualified1.IsUnqualified()) {
              ok = Standard_True;
            }
            Radius = L2.Distance(Center);
            if (Qualified2.IsEnclosed() && ok) {
              ok = Standard_False;
              if ((((origin2.X()-Center.X())*(-dir2.Y()))+
                ((origin2.Y()-Center.Y())*(dir2.X())))<=0){
                  ok = Standard_True;
              }
            }
            else if (Qualified2.IsOutside() && ok) {
              ok = Standard_False;
              if ((((origin2.X()-Center.X())*(-dir2.Y()))+
                ((origin2.Y()-Center.Y())*(dir2.X())))>=0){
                  ok = Standard_True;
              }
            }
            if (Qualified1.IsEnclosing()&&dist1>Radius) { ok=Standard_False; }
            if (ok) {
              NbrSol++;
              cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
              //            =======================================================
#ifdef OCCT_DEBUG
              gp_Dir2d aDC1(center1.XY()-Center.XY());
#endif
              gp_Dir2d dc2(origin2.XY()-Center.XY());
              Standard_Real distcc1 = Center.Distance(center1);
              if (!Qualified1.IsUnqualified()) { 
                qualifier1(NbrSol) = Qualified1.Qualifier();
              }
              else if (Abs(distcc1+Radius-R1) < Tol) {
                qualifier1(NbrSol) = GccEnt_enclosed;
              }
              else if (Abs(distcc1-R1-Radius) < Tol) {
                qualifier1(NbrSol) = GccEnt_outside;
              }
              else { qualifier1(NbrSol) = GccEnt_enclosing; }
              if (!Qualified2.IsUnqualified()) { 
                qualifier2(NbrSol) = Qualified2.Qualifier();
              }
              else if (dc2.Dot(normL2) > 0.0) {
                qualifier2(NbrSol) = GccEnt_outside;
              }
              else { qualifier2(NbrSol) = GccEnt_enclosed; }
              if (dist1 <= Tol && Abs(Radius-C1.Radius()) <= Tol) {
                TheSame1(NbrSol) = 1;
              }
              else {
                TheSame1(NbrSol) = 0;
                gp_Dir2d dc1(center1.XY()-Center.XY());
                pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius*dc1.XY());
                par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                  pnttg1sol(NbrSol));
                pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
              }
              TheSame2(NbrSol) = 0;
              Standard_Real sign = dc2.Dot(gp_Dir2d(-dir2.Y(),dir2.X()));
              dc2 = gp_Dir2d(sign*gp_XY(-dir2.Y(),dir2.X()));
              pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc2.XY());
              par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                pnttg2sol(NbrSol));
              pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
              pntcen(NbrSol) = Center;
              parcen3(NbrSol) = Intp.Point(j).ParamOnSecond();
            }
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//   Creation d un cercle tant a deux Droites L1 et L2.                +
//                        centre sur une courbe OnCurv.                   +
//  Nous calculons les bissectrices a L1 et L2 qui nous donnent           +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tants a L1 et L2.                                                  +
//  Nous intersectons ces bissectrices avec la courbe OnCurv ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant L1 et L2.  +
//=========================================================================

Geom2dGcc_Circ2d2TanOnGeo::
Geom2dGcc_Circ2d2TanOnGeo (const GccEnt_QualifiedLin&      Qualified1 , 
                           const GccEnt_QualifiedLin&      Qualified2 , 
                           const Geom2dAdaptor_Curve&                 OnCurv     ,
                           const Standard_Real             Tolerance  ):
cirsol(1,8)    ,
qualifier1(1,8),
qualifier2(1,8),
TheSame1(1,8)  ,
TheSame2(1,8)  ,
pnttg1sol(1,8) ,
pnttg2sol(1,8) ,
pntcen(1,8)    ,
par1sol(1,8)   ,
par2sol(1,8)   ,
pararg1(1,8)   ,
pararg2(1,8)   ,
parcen3(1,8)   
{

  WellDone = Standard_False;
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
    !(Qualified2.IsEnclosed() ||
    Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  Standard_Real Tol = Abs(Tolerance);
  Standard_Real Radius=0;
  gp_Dir2d dirx(1.,0.);
  gp_Lin2d L1 = Qualified1.Qualified();
  gp_Lin2d L2 = Qualified2.Qualified();
  gp_Dir2d dir1(L1.Direction());
  gp_Dir2d dir2(L2.Direction());
  gp_Dir2d Dnor1(-dir1.Y(),dir1.X());
  gp_Dir2d Dnor2(-dir2.Y(),dir2.X());
  gp_Pnt2d origin1(L1.Location());
  gp_Pnt2d origin2(L2.Location());
  GccAna_Lin2dBisec Bis(L1,L2);
  if (Bis.IsDone()) {
    Standard_Real Tol1 = Abs(Tolerance);
    Standard_Real Tol2 = Tol1;
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    Standard_Integer nbsolution = Bis.NbSolutions();
    Handle(Geom2dAdaptor_HCurve) HCu2 = new Geom2dAdaptor_HCurve(OnCurv); 
    Adaptor2d_OffsetCurve C2(HCu2,0.);
    firstparam = Max(C2.FirstParameter(),thefirst);
    lastparam  = Min(C2.LastParameter(),thelast);
    IntRes2d_Domain D2(C2.Value(firstparam), firstparam, Tol,
                       C2.Value(lastparam), lastparam, Tol);
    IntRes2d_Domain D1;
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Intp.Perform(Bis.ThisSolution(i),D1,C2,D2,Tol1,Tol2);
      if (Intp.IsDone()) {
        if ((!Intp.IsEmpty())) {
          for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
            gp_Pnt2d Center(Intp.Point(j).Value());
            Standard_Real dist1 = L1.Distance(Center);
            Standard_Real dist2 = L2.Distance(Center);
            //	    Standard_Integer nbsol = 1;
            Standard_Boolean ok = Standard_False;
            if (Qualified1.IsEnclosed()) {
              if ((((origin1.X()-Center.X())*(-dir1.Y()))+
                ((origin1.Y()-Center.Y())*(dir1.X())))<=0){
                  ok = Standard_True;
              }
            }
            else if (Qualified1.IsOutside()) {
              if ((((origin1.X()-Center.X())*(-dir1.Y()))+
                ((origin1.Y()-Center.Y())*(dir1.X())))>=0){
                  ok = Standard_True;
              }
            }
            else if (Qualified1.IsUnqualified()) { ok = Standard_True; }
            if (Qualified2.IsEnclosed() && ok) {
              ok = Standard_False;
              if ((((origin2.X()-Center.X())*(-dir2.Y()))+
                ((origin2.Y()-Center.Y())*(dir2.X())))<=0){
                  ok = Standard_True;
                  Radius = (dist1+dist2)/2.;
              }
            }
            else if (Qualified2.IsOutside() && ok) {
              ok = Standard_False;
              if ((((origin2.X()-Center.X())*(-dir2.Y()))+
                ((origin2.Y()-Center.Y())*(dir2.X())))>=0){
                  ok = Standard_True;
                  Radius = (dist1+dist2)/2.;
              }
            }
            else if (Qualified2.IsUnqualified() && ok) {
              Radius = (dist1+dist2)/2.;
            }
            if (ok) {
              NbrSol++;
              cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
              //            =======================================================
              gp_Dir2d dc1(origin1.XY()-Center.XY());
              gp_Dir2d dc2(origin2.XY()-Center.XY());
              if (!Qualified1.IsUnqualified()) { 
                qualifier1(NbrSol) = Qualified1.Qualifier();
              }
              else if (dc1.Dot(Dnor1) > 0.0) {
                qualifier1(NbrSol) = GccEnt_outside;
              }
              else { qualifier1(NbrSol) = GccEnt_enclosed; }
              if (!Qualified2.IsUnqualified()) { 
                qualifier2(NbrSol) = Qualified2.Qualifier();
              }
              else if (dc2.Dot(Dnor2) > 0.0) {
                qualifier2(NbrSol) = GccEnt_outside;
              }
              else { qualifier2(NbrSol) = GccEnt_enclosed; }
              TheSame1(NbrSol) = 0;
              TheSame2(NbrSol) = 0;
              Standard_Real sign = dc1.Dot(Dnor1);
              dc1 = gp_Dir2d(sign*gp_XY(-dir1.Y(),dir1.X()));
              pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc1.XY());
              par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                pnttg1sol(NbrSol));
              pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
              sign = dc2.Dot(gp_Dir2d(-dir2.Y(),dir2.X()));
              dc2 = gp_Dir2d(sign*gp_XY(-dir2.Y(),dir2.X()));
              pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc2.XY());
              par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                pnttg2sol(NbrSol));
              pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
              pntcen(NbrSol) = Center;
              parcen3(NbrSol) = Intp.Point(j).ParamOnSecond();
            }
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//   Creation d un cercle tant a un Cercle C1, passant par un point P2 +
//                        centre sur une courbe OnCurv.                   +
//  Nous calculons les bissectrices a C1 et Point2 qui nous donnent       +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tants a C1 et Point2.                                              +
//  Nous intersectons ces bissectrices avec la courbe OnCurv ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant C1.        +
//=========================================================================

Geom2dGcc_Circ2d2TanOnGeo::
Geom2dGcc_Circ2d2TanOnGeo (const GccEnt_QualifiedCirc&     Qualified1 , 
                           const gp_Pnt2d&                 Point2     , 
                           const Geom2dAdaptor_Curve&                 OnCurv     ,
                           const Standard_Real             Tolerance  ):
cirsol(1,8)    ,
qualifier1(1,8),
qualifier2(1,8),
TheSame1(1,8)  ,
TheSame2(1,8)  ,
pnttg1sol(1,8) ,
pnttg2sol(1,8) ,
pntcen(1,8)    ,
par1sol(1,8)   ,
par2sol(1,8)   ,
pararg1(1,8)   ,
pararg2(1,8)   ,
parcen3(1,8)   
{

  WellDone = Standard_False;
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  Standard_Real Tol = Abs(Tolerance);
  Standard_Real Radius;
  gp_Dir2d dirx(1.,0.);
  gp_Circ2d C1 = Qualified1.Qualified();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(C1.Location());
  GccAna_CircPnt2dBisec Bis(C1,Point2);
  if (Bis.IsDone()) {
    Standard_Real Tol1 = Abs(Tolerance);
    Standard_Real Tol2 = Tol1;
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    Standard_Integer nbsolution = Bis.NbSolutions();
    Handle(Geom2dAdaptor_HCurve) HCu2 = new Geom2dAdaptor_HCurve(OnCurv); 
    Adaptor2d_OffsetCurve C2(HCu2,0.);
    firstparam = Max(C2.FirstParameter(),thefirst);
    lastparam  = Min(C2.LastParameter(),thelast);
    IntRes2d_Domain D2(C2.Value(firstparam), firstparam, Tol,
                       C2.Value(lastparam), lastparam, Tol);
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      switch (type) {
      case GccInt_Cir:
        {
          gp_Circ2d Circ(Sol->Circle());
          IntRes2d_Domain D1(ElCLib::Value(0.,Circ),   0.,Tol1,
            ElCLib::Value(2.*M_PI,Circ),2.*M_PI,Tol2);
          D1.SetEquivalentParameters(0.,2.*M_PI);
          Intp.Perform(Circ,D1,C2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Lin:
        {
          gp_Lin2d Line(Sol->Line());
          IntRes2d_Domain D1;
          Intp.Perform(Line,D1,C2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Ell:
        {
          gp_Elips2d Elips(Sol->Ellipse());
          IntRes2d_Domain D1(ElCLib::Value(0.,Elips),   0.,Tol1,
            ElCLib::Value(2.*M_PI,Elips),2.*M_PI,Tol2);
          D1.SetEquivalentParameters(0.,2.*M_PI);
          Intp.Perform(Elips,D1,C2,D2,Tol1,Tol2);
        }
        break;
      case GccInt_Hpr:
        {
          gp_Hypr2d Hypr(Sol->Hyperbola());
          IntRes2d_Domain D1(ElCLib::Value(-4.,Hypr),-4.,Tol1,
            ElCLib::Value(4.,Hypr),4.,Tol2);
          Intp.Perform(Hypr,D1,C2,D2,Tol1,Tol2);
        }
        break;
      default:
        {
          throw Standard_ConstructionError();
        }
      }
      if (Intp.IsDone()) {
        if ((!Intp.IsEmpty())) {
          for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
            gp_Pnt2d Center(Intp.Point(j).Value());
            Radius = Center.Distance(Point2);
            Standard_Real dist1 = center1.Distance(Center);
            //	    Standard_Integer nbsol = 1;
            Standard_Boolean ok = Standard_False;
            if (Qualified1.IsEnclosed()) {
              if (dist1-R1 <= Tol) { ok = Standard_True; }
            }
            else if (Qualified1.IsOutside()) {
              if (R1-dist1 <= Tol) { ok = Standard_True; }
            }
            else if (Qualified1.IsEnclosing()) { ok = Standard_True; }
            else if (Qualified1.IsUnqualified()) { ok = Standard_True; }
            if (ok) {
              NbrSol++;
              cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
              //            =======================================================
              Standard_Real distcc1 = Center.Distance(center1);
              if (!Qualified1.IsUnqualified()) { 
                qualifier1(NbrSol) = Qualified1.Qualifier();
              }
              else if (Abs(distcc1+Radius-R1) < Tol) {
                qualifier1(NbrSol) = GccEnt_enclosed;
              }
              else if (Abs(distcc1-R1-Radius) < Tol) {
                qualifier1(NbrSol) = GccEnt_outside;
              }
              else { qualifier1(NbrSol) = GccEnt_enclosing; }
              qualifier2(NbrSol) = GccEnt_noqualifier;
              if (dist1 <= Tol && Abs(Radius-R1) <= Tol) {
                TheSame1(NbrSol) = 1;
              }
              else {
                TheSame1(NbrSol) = 0;
                gp_Dir2d dc1(center1.XY()-Center.XY());
                pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius*dc1.XY());
                par1sol(NbrSol) = 0.;
                par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                  pnttg1sol(NbrSol));
                pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
              }
              TheSame2(NbrSol) = 0;
              pnttg2sol(NbrSol) = Point2;
              pntcen(NbrSol) = Center;
              parcen3(NbrSol) = Intp.Point(j).ParamOnSecond();
              pararg2(NbrSol) = 0.;
              par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
                pnttg2sol(NbrSol));
            }
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//   Creation d un cercle tant a une ligne L1, passant par un point P2 +
//                        centre sur une courbe OnCurv.                   +
//  Nous calculons les bissectrices a L1 et Point2 qui nous donnent       +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tants a L1 et passant par Point2.                                  +
//  Nous intersectons ces bissectrices avec la courbe OnCurv ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant L1.        +
//=========================================================================

Geom2dGcc_Circ2d2TanOnGeo::
Geom2dGcc_Circ2d2TanOnGeo (const GccEnt_QualifiedLin&      Qualified1 , 
                           const gp_Pnt2d&                 Point2     , 
                           const Geom2dAdaptor_Curve&                 OnCurv     ,
                           const Standard_Real             Tolerance  ):
cirsol(1,8)    ,
qualifier1(1,8),
qualifier2(1,8),
TheSame1(1,8)  ,
TheSame2(1,8)  ,
pnttg1sol(1,8) ,
pnttg2sol(1,8) ,
pntcen(1,8)    ,
par1sol(1,8)   ,
par2sol(1,8)   ,
pararg1(1,8)   ,
pararg2(1,8)   ,
parcen3(1,8)   
{

  WellDone = Standard_False;
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() ||
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  gp_Dir2d dirx(1.,0.);
  gp_Lin2d L1 = Qualified1.Qualified();
  gp_Pnt2d origin1(L1.Location());
  gp_Dir2d dir1(L1.Direction());
  gp_Dir2d normal(-dir1.Y(),dir1.X());
  GccAna_LinPnt2dBisec Bis(L1,Point2);
  if (Bis.IsDone()) {
    Standard_Real Tol1 = Abs(Tolerance);
    Standard_Real Tol2 = Tol1;
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    Handle(Geom2dAdaptor_HCurve) HCu2 = new Geom2dAdaptor_HCurve(OnCurv); 
    Adaptor2d_OffsetCurve C2(HCu2,0.);
    firstparam = Max(C2.FirstParameter(),thefirst);
    lastparam  = Min(C2.LastParameter(),thelast);
    IntRes2d_Domain D2(C2.Value(firstparam), firstparam, Tol,
                       C2.Value(lastparam), lastparam, Tol);
    Handle(GccInt_Bisec) Sol = Bis.ThisSolution();
    GccInt_IType type = Sol->ArcType();
    switch (type) {
    case GccInt_Lin:
      {
        gp_Lin2d Line(Sol->Line());
        IntRes2d_Domain D1;
        Intp.Perform(Line,D1,C2,D2,Tol1,Tol2);
      }
      break;
    case GccInt_Par:
      {
        gp_Parab2d Parab(Sol->Parabola());
        IntRes2d_Domain D1(ElCLib::Value(-40,Parab),-40,Tol1,
          ElCLib::Value(40,Parab),40,Tol1);
        Intp.Perform(Parab,D1,C2,D2,Tol1,Tol2);
      }
      break;
    default:
      {
        throw Standard_ConstructionError();
      }
    }
    if (Intp.IsDone()) {
      if ((!Intp.IsEmpty())) {
        for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
          gp_Pnt2d Center(Intp.Point(j).Value());
          Standard_Real Radius = L1.Distance(Center);
          //	  Standard_Integer nbsol = 1;
          Standard_Boolean ok = Standard_False;
          if (Qualified1.IsEnclosed()) {
            if ((((origin1.X()-Center.X())*(-dir1.Y()))+
              ((origin1.Y()-Center.Y())*(dir1.X())))<=0){
                ok = Standard_True;
            }
          }
          else if (Qualified1.IsOutside()) {
            if ((((origin1.X()-Center.X())*(-dir1.Y()))+
              ((origin1.Y()-Center.Y())*(dir1.X())))>=0){
                ok = Standard_True;
            }
          }
          else if (Qualified1.IsUnqualified()) { ok = Standard_True; }
          if (ok) {
            NbrSol++;
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //          =======================================================
            qualifier2(NbrSol) = GccEnt_noqualifier;
            gp_Dir2d dc2(origin1.XY()-Center.XY());
            if (!Qualified1.IsUnqualified()) { 
              qualifier1(NbrSol) = Qualified1.Qualifier();
            }
            else if (dc2.Dot(normal) > 0.0) {
              qualifier1(NbrSol) = GccEnt_outside;
            }
            else { qualifier1(NbrSol) = GccEnt_enclosed; }
            TheSame1(NbrSol) = 0;
            TheSame2(NbrSol) = 0;
            gp_Dir2d dc1(origin1.XY()-Center.XY());
            Standard_Real sign = dc1.Dot(gp_Dir2d(-dir1.Y(),dir1.X()));
            dc1=gp_Dir2d(sign*gp_XY(-dir1.Y(),dir1.X()));
            pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc1.XY());
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
            pnttg2sol(NbrSol) = Point2;
            par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg2sol(NbrSol));
            pararg2(NbrSol) = 0.;
            pntcen(NbrSol) = Center;
            parcen3(NbrSol) = Intp.Point(j).ParamOnSecond();
          }
        }
      }
      WellDone = Standard_True;
    }
  }
}

//=========================================================================
//   Creation d un cercle passant par deux point Point1 et Point2         +
//                        centre sur une courbe OnCurv.                   +
//  Nous calculons les bissectrices a Point1 et Point2 qui nous donnent   +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  passant par Point1 et Point2.                                         +
//  Nous intersectons ces bissectrices avec la courbe OnCurv ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//=========================================================================

Geom2dGcc_Circ2d2TanOnGeo::
Geom2dGcc_Circ2d2TanOnGeo (const gp_Pnt2d&               Point1    ,
                           const gp_Pnt2d&               Point2    ,
                           const Geom2dAdaptor_Curve&               OnCurv    ,
                           const Standard_Real           Tolerance ):
cirsol(1,8)    ,
qualifier1(1,8),
qualifier2(1,8),
TheSame1(1,8)  ,
TheSame2(1,8)  ,
pnttg1sol(1,8) ,
pnttg2sol(1,8) ,
pntcen(1,8)    ,
par1sol(1,8)   ,
par2sol(1,8)   ,
pararg1(1,8)   ,
pararg2(1,8)   ,
parcen3(1,8)   
{

  WellDone = Standard_False;
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  NbrSol = 0;
  gp_Dir2d dirx(1.,0.);
  GccAna_Pnt2dBisec Bis(Point1,Point2);
  if (Bis.IsDone()) {
    Standard_Real Tol1 = Abs(Tolerance);
    Standard_Real Tol2 = Tol1;
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    Handle(Geom2dAdaptor_HCurve) HCu2 = new Geom2dAdaptor_HCurve(OnCurv); 
    Adaptor2d_OffsetCurve Cu2(HCu2,0.);
    firstparam = Max(Cu2.FirstParameter(),thefirst);
    lastparam  = Min(Cu2.LastParameter(),thelast);
    IntRes2d_Domain D2(Cu2.Value(firstparam), firstparam, Tol,
                       Cu2.Value(lastparam), lastparam, Tol);
    IntRes2d_Domain D1;
    if (Bis.HasSolution()) {
      Intp.Perform(Bis.ThisSolution(),D1,Cu2,D2,Tol1,Tol2);
      if (Intp.IsDone()) {
        if ((!Intp.IsEmpty())) {
          for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
            gp_Pnt2d Center(Intp.Point(j).Value());
            Standard_Real Radius = Point2.Distance(Center);
            NbrSol++;
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //           =======================================================
            qualifier1(NbrSol) = GccEnt_noqualifier;
            qualifier2(NbrSol) = GccEnt_noqualifier;
            TheSame1(NbrSol) = 0;
            TheSame2(NbrSol) = 0;
            pntcen(NbrSol) = Center;
            pnttg1sol(NbrSol) = Point1;
            pnttg2sol(NbrSol) = Point2;
            pararg1(NbrSol) = 0.;
            pararg2(NbrSol) = 0.;
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg2sol(NbrSol));
            parcen3(NbrSol) = Intp.Point(j).ParamOnSecond();
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

Standard_Boolean Geom2dGcc_Circ2d2TanOnGeo::
IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2d2TanOnGeo::
NbSolutions () const{ return NbrSol; }

gp_Circ2d Geom2dGcc_Circ2d2TanOnGeo::
ThisSolution (const Standard_Integer Index) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }

  return cirsol(Index);
}

void Geom2dGcc_Circ2d2TanOnGeo::
WhichQualifier(const Standard_Integer Index   ,
               GccEnt_Position& Qualif1 ,
               GccEnt_Position& Qualif2 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    Qualif1 = qualifier1(Index);
    Qualif2 = qualifier2(Index);
  }
}

void Geom2dGcc_Circ2d2TanOnGeo:: 
Tangency1 (const Standard_Integer    Index          , 
           Standard_Real&      ParSol         ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&           PntSol         ) const{
             if (!WellDone) { throw StdFail_NotDone(); }
             else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
             else {
               if (TheSame1(Index) == 0) {
                 ParSol = par1sol(Index);
                 ParArg = pararg1(Index);
                 PntSol = gp_Pnt2d(pnttg1sol(Index));
               }
               else { throw StdFail_NotDone(); }
             }
}

void Geom2dGcc_Circ2d2TanOnGeo:: 
Tangency2 (const Standard_Integer    Index          , 
           Standard_Real&      ParSol         ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&           PntSol         ) const{
             if (!WellDone) { throw StdFail_NotDone(); }
             else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
             else {
               if (TheSame2(Index) == 0) {
                 ParSol = par2sol(Index);
                 ParArg = pararg2(Index);
                 PntSol = gp_Pnt2d(pnttg2sol(Index));
               }
               else { throw StdFail_NotDone(); }
             }
}

void Geom2dGcc_Circ2d2TanOnGeo::
CenterOn3 (const Standard_Integer    Index          ,
           Standard_Real&      ParArg         ,
           gp_Pnt2d&           PntSol         ) const{
             if (!WellDone) { throw StdFail_NotDone(); }
             else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
             else {
               ParArg = parcen3(Index);
               PntSol = gp_Pnt2d(pntcen(Index));
             }
}

Standard_Boolean Geom2dGcc_Circ2d2TanOnGeo::
IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) throw StdFail_NotDone();
  if (Index <= 0 ||Index > NbrSol) throw Standard_OutOfRange();

  if (TheSame1(Index) == 0) 
    return Standard_False;

  return Standard_True;
}


Standard_Boolean Geom2dGcc_Circ2d2TanOnGeo::
IsTheSame2 (const Standard_Integer Index) const
{
  if (!WellDone) throw StdFail_NotDone();
  if (Index <= 0 ||Index > NbrSol) throw Standard_OutOfRange();

  if (TheSame2(Index) == 0)
    return Standard_False;

  return Standard_True;
}
