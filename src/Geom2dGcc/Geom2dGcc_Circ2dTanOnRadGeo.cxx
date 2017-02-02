// Copyright (c) 1995-1999 Matra Datavision
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
//       circulaire tangent a un element de type :  - Cercle.            +
//                                                  - Ligne.             +
//                                                  - Point.             +
//                  centre sur un deuxieme element de type :  - Cercle.  +
//                                                            - Ligne.   +
//                  de rayon donne : Radius.                             +
//========================================================================

#include <Adaptor2d_OffsetCurve.hxx>
#include <ElCLib.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dGcc_Circ2dTanOnRadGeo.hxx>
#include <Geom2dGcc_CurveTool.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <math_DirectPolynomialRoots.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

//=========================================================================
//  Cercle tangent  :  a un cercle Qualified1 (C1).                       +
//         centre   :  sur une droite OnLine.                             +
//         de rayon :  Radius.                                            +
//                                                                        + 
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du cercle C1 et           +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================
Geom2dGcc_Circ2dTanOnRadGeo::
Geom2dGcc_Circ2dTanOnRadGeo (const Geom2dGcc_QCurve& Qualified1, 
                             const gp_Lin2d&     OnLine    ,
                             const Standard_Real Radius    ,
                             const Standard_Real Tolerance ):

//=========================================================================
// Initialisation des champs.                                             +
//=========================================================================

cirsol(1,8)     ,
qualifier1(1,8) ,
TheSame1(1,8)   ,
pnttg1sol(1,8)  ,
pntcen3(1,8)    ,
par1sol(1,8)    ,
pararg1(1,8)    ,
parcen3(1,8)    
{

  //=========================================================================
  // Traitement.                                                            +
  //=========================================================================

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real Tol = Abs(Tolerance);
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  Standard_Integer nbrcote1 = 0;
  TColStd_Array1OfReal Coef(1,2);
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();

  if (Radius < 0.0) { throw Standard_NegativeValue(); }
  else {
    if (Qualified1.IsEnclosed()) {
      //    ===========================
      nbrcote1 = 1;
      Coef(1) = Radius;
    }
    else if(Qualified1.IsOutside()) {
      //   ===============================
      nbrcote1 = 1;
      Coef(1) = -Radius;
    }
    else if(Qualified1.IsUnqualified()) {
      //   ===================================
      nbrcote1 = 2;
      Coef(1) = Radius;
      Coef(2) = -Radius;
    }
    IntRes2d_Domain D1;
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    for (Standard_Integer jcote1 = 1 ; jcote1 <= nbrcote1 ; jcote1++) {
      Handle(Geom2dAdaptor_HCurve) HCu1 = new Geom2dAdaptor_HCurve(Cu1);
      Adaptor2d_OffsetCurve C2(HCu1,Coef(jcote1));
      firstparam = Max(C2.FirstParameter(),thefirst);
      lastparam  = Min(C2.LastParameter(),thelast);
      IntRes2d_Domain D2(C2.Value(firstparam), firstparam, Tol,
                         C2.Value(lastparam), lastparam, Tol);
      Intp.Perform(OnLine,D1,C2,D2,Tol,Tol);
      if (Intp.IsDone()) {
        if (!Intp.IsEmpty()) {
          for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
            NbrSol++;
            gp_Pnt2d Center(Intp.Point(i).Value());
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //           =======================================================
            qualifier1(NbrSol) = Qualified1.Qualifier();
            TheSame1(NbrSol) = 0;
            pararg1(NbrSol) = Intp.Point(i).ParamOnSecond();
            parcen3(NbrSol) = Intp.Point(i).ParamOnFirst();
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            pnttg1sol(NbrSol) = gp_Pnt2d(Geom2dGcc_CurveTool::Value(Cu1,pararg1(NbrSol)));
            pntcen3(NbrSol) = Center;
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//  Cercle tangent  :  a un cercle Qualified1 (C1).                       +
//         centre   :  sur une droite OnLine.                             +
//         de rayon :  Radius.                                            +
//                                                                        + 
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du cercle C1 et           +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================

Geom2dGcc_Circ2dTanOnRadGeo::
Geom2dGcc_Circ2dTanOnRadGeo (const Geom2dGcc_QCurve& Qualified1,
                             const gp_Circ2d&    OnCirc    , 
                             const Standard_Real Radius    ,
                             const Standard_Real Tolerance ):

//=========================================================================
// Initialisation des champs.                                             +
//=========================================================================

cirsol(1,8)     ,
qualifier1(1,8) ,
TheSame1(1,8)   ,
pnttg1sol(1,8)  ,
pntcen3(1,8)    ,
par1sol(1,8)    ,
pararg1(1,8)    ,
parcen3(1,8)    
{

  //=========================================================================
  // Traitement.                                                            +
  //=========================================================================

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  Standard_Integer nbrcote1=0;
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  TColStd_Array1OfReal cote1(1,2);
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();

  if (Radius < 0.0) {
    throw Standard_NegativeValue();
  }
  else {
    if (Qualified1.IsEnclosed()) {
      //    ===========================
      nbrcote1 = 1;
      cote1(1) = Radius;
    }
    else if(Qualified1.IsOutside()) {
      //   ===============================
      nbrcote1 = 1;
      cote1(1) = -Radius;
    }
    else if(Qualified1.IsUnqualified()) {
      //   ===================================
      nbrcote1 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
    }
    IntRes2d_Domain D1(ElCLib::Value(0.,OnCirc),   0.,Tol,
      ElCLib::Value(2.*M_PI,OnCirc),2.*M_PI,Tol);
    D1.SetEquivalentParameters(0.,2.*M_PI);
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    for (Standard_Integer jcote1 = 1 ; jcote1 <= nbrcote1 ; jcote1++) {
      Handle(Geom2dAdaptor_HCurve) HCu1 = new Geom2dAdaptor_HCurve(Cu1);
      Adaptor2d_OffsetCurve C2(HCu1,cote1(jcote1));
      firstparam = Max(C2.FirstParameter(),thefirst);
      lastparam  = Min(C2.LastParameter(),thelast);
      IntRes2d_Domain D2(C2.Value(firstparam),firstparam,Tol,
                         C2.Value(lastparam),lastparam,Tol);
      Intp.Perform(OnCirc,D1,C2,D2,Tol,Tol);
      if (Intp.IsDone()) {
        if (!Intp.IsEmpty()) {
          for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
            NbrSol++;
            gp_Pnt2d Center(Intp.Point(i).Value());
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //           =======================================================
            qualifier1(NbrSol) = Qualified1.Qualifier();
            TheSame1(NbrSol) = 0;
            pararg1(NbrSol) = Intp.Point(i).ParamOnSecond();
            parcen3(NbrSol) = Intp.Point(i).ParamOnFirst();
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            pnttg1sol(NbrSol) = gp_Pnt2d(Geom2dGcc_CurveTool::Value(Cu1,pararg1(NbrSol)));
            pntcen3(NbrSol) = Center;
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//  Cercle tangent  :  a un cercle Qualified1 (C1).                       +
//         centre   :  sur une droite OnLine.                             +
//         de rayon :  Radius.                                            +
//                                                                        + 
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du cercle C1 et           +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================

Geom2dGcc_Circ2dTanOnRadGeo::
Geom2dGcc_Circ2dTanOnRadGeo (const GccEnt_QualifiedCirc& Qualified1,
                             const Geom2dAdaptor_Curve&             OnCurv    ,
                             const Standard_Real         Radius    ,
                             const Standard_Real         Tolerance ):

//=========================================================================
// Initialisation des champs.                                             +
//=========================================================================

cirsol(1,8)     ,
qualifier1(1,8) ,
TheSame1(1,8)   ,
pnttg1sol(1,8)  ,
pntcen3(1,8)    ,
par1sol(1,8)    ,
pararg1(1,8)    ,
parcen3(1,8)    
{

  //=========================================================================
  // Traitement.                                                            +
  //=========================================================================

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  Standard_Integer nbrcote1=0;
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  TColStd_Array1OfReal cote1(1,2);
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Pnt2d center1(C1.Location());
  Standard_Real R1 = C1.Radius();

  if (Radius < 0.0) {
    throw Standard_NegativeValue();
  }
  else {
    if (Qualified1.IsEnclosed()) {
      //    ===========================
      nbrcote1 = 1;
      cote1(1) = Radius;
    }
    else if(Qualified1.IsOutside()) {
      //   ===============================
      nbrcote1 = 1;
      cote1(1) = -Radius;
    }
    else if(Qualified1.IsUnqualified()) {
      //   ===================================
      nbrcote1 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
    }
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    for (Standard_Integer jcote1 = 1 ; jcote1 <= nbrcote1 ; jcote1++) {
      gp_Circ2d Circ(C1.XAxis(),R1 + cote1(jcote1));
      IntRes2d_Domain D1(ElCLib::Value(0.,Circ),   0.,Tol,
        ElCLib::Value(2.*M_PI,Circ),2.*M_PI,Tol);
      D1.SetEquivalentParameters(0.,2.*M_PI);
      firstparam = Max(Geom2dGcc_CurveTool::FirstParameter(OnCurv),thefirst);
      lastparam  = Min(Geom2dGcc_CurveTool::LastParameter(OnCurv),thelast);
      IntRes2d_Domain D2(Geom2dGcc_CurveTool::Value(OnCurv,firstparam),firstparam,Tol,
        Geom2dGcc_CurveTool::Value(OnCurv,lastparam),lastparam,Tol);
      Intp.Perform(Circ,D1,OnCurv,D2,Tol,Tol);
      if (Intp.IsDone()) {
        if (!Intp.IsEmpty()) {
          for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
            NbrSol++;
            gp_Pnt2d Center(Intp.Point(i).Value());
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //           =======================================================
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
            TheSame1(NbrSol) = 0;
            pararg1(NbrSol) = Intp.Point(i).ParamOnFirst();
            parcen3(NbrSol) = Intp.Point(i).ParamOnSecond();
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            pnttg1sol(NbrSol) = ElCLib::Value(pararg1(NbrSol),C1);
            pntcen3(NbrSol) = Center;
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//  Cercle tangent  :  a un cercle Qualified1 (C1).                       +
//         centre   :  sur une droite OnLine.                             +
//         de rayon :  Radius.                                            +
//                                                                        + 
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du cercle C1 et           +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================

Geom2dGcc_Circ2dTanOnRadGeo::
Geom2dGcc_Circ2dTanOnRadGeo (const GccEnt_QualifiedLin& Qualified1,
                             const Geom2dAdaptor_Curve&            OnCurv    ,
                             const Standard_Real        Radius    ,
                             const Standard_Real        Tolerance ):

//=========================================================================
// Initialisation des champs.                                             +
//=========================================================================

cirsol(1,8)     ,
qualifier1(1,8) ,
TheSame1(1,8)   ,
pnttg1sol(1,8)  ,
pntcen3(1,8)    ,
par1sol(1,8)    ,
pararg1(1,8)    ,
parcen3(1,8)    
{

  //=========================================================================
  // Traitement.                                                            +
  //=========================================================================

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() ||
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  Standard_Integer nbrcote1=0;
  TColStd_Array1OfReal cote1(1,2);
  gp_Lin2d L1 = Qualified1.Qualified();
  gp_Pnt2d origin1(L1.Location());
  gp_Dir2d dir1(L1.Direction());
  gp_Dir2d norm1(-dir1.Y(),dir1.X());

  if (Radius < 0.0) {
    throw Standard_NegativeValue();
  }
  else {
    if (Qualified1.IsEnclosed()) {
      //    ===========================
      nbrcote1 = 1;
      cote1(1) = Radius;
    }
    else if(Qualified1.IsOutside()) {
      //   ===============================
      nbrcote1 = 1;
      cote1(1) = -Radius;
    }
    else if(Qualified1.IsUnqualified()) {
      //   ===================================
      nbrcote1 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
    }
    Geom2dInt_TheIntConicCurveOfGInter Intp;
    for (Standard_Integer jcote1 = 1 ; jcote1 <= nbrcote1 ; jcote1++) {
      gp_Pnt2d Point(dir1.XY()+cote1(jcote1)*norm1.XY());
      gp_Lin2d Line(Point,dir1); // ligne avec deport.
      IntRes2d_Domain D1;
      firstparam = Max(Geom2dGcc_CurveTool::FirstParameter(OnCurv),thefirst);
      lastparam  = Min(Geom2dGcc_CurveTool::LastParameter(OnCurv),thelast);
      IntRes2d_Domain D2(Geom2dGcc_CurveTool::Value(OnCurv,firstparam),firstparam,Tol,
        Geom2dGcc_CurveTool::Value(OnCurv,lastparam),lastparam,Tol);
      Intp.Perform(Line,D1,OnCurv,D2,Tol,Tol);
      if (Intp.IsDone()) {
        if (!Intp.IsEmpty()) {
          for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
            NbrSol++;
            gp_Pnt2d Center(Intp.Point(i).Value());
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //           =======================================================
            gp_Dir2d dc1(origin1.XY()-Center.XY());
            if (!Qualified1.IsUnqualified()) { 
              qualifier1(NbrSol) = Qualified1.Qualifier();
            }
            else if (dc1.Dot(norm1) > 0.0) {	
              qualifier1(NbrSol) = GccEnt_outside; 
            }
            else { qualifier1(NbrSol) = GccEnt_enclosed; }
            TheSame1(NbrSol) = 0;
            pararg1(NbrSol) = Intp.Point(i).ParamOnFirst();
            parcen3(NbrSol) = Intp.Point(i).ParamOnSecond();
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            pnttg1sol(NbrSol) = ElCLib::Value(pararg1(NbrSol),L1);
            pntcen3(NbrSol) = Center;
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//  Cercle tangent  :  a un cercle Qualified1 (C1).                       +
//         centre   :  sur une droite OnLine.                             +
//         de rayon :  Radius.                                            +
//                                                                        + 
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du cercle C1 et           +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================

Geom2dGcc_Circ2dTanOnRadGeo::
Geom2dGcc_Circ2dTanOnRadGeo (const Geom2dGcc_QCurve& Qualified1,
                             const Geom2dAdaptor_Curve&     OnCurv    ,
                             const Standard_Real Radius    ,
                             const Standard_Real Tolerance ):

//=========================================================================
// Initialisation des champs.                                             +
//=========================================================================

cirsol(1,8)     ,
qualifier1(1,8) ,
TheSame1(1,8)   ,
pnttg1sol(1,8)  ,
pntcen3(1,8)    ,
par1sol(1,8)    ,
pararg1(1,8)    ,
parcen3(1,8)    
{

  //=========================================================================
  // Traitement.                                                            +
  //=========================================================================

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  Standard_Integer nbrcote1=0;
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
    Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
      throw GccEnt_BadQualifier();
      return;
  }
  TColStd_Array1OfReal cote1(1,2);
  Geom2dAdaptor_Curve Cu1 = Qualified1.Qualified();

  if (Radius < 0.0) {
    throw Standard_NegativeValue();
  }
  else {
    if (Qualified1.IsEnclosed()) {
      //    ===========================
      nbrcote1 = 1;
      cote1(1) = Radius;
    }
    else if(Qualified1.IsOutside()) {
      //   ===============================
      nbrcote1 = 1;
      cote1(1) = -Radius;
    }
    else if(Qualified1.IsUnqualified()) {
      //   ===================================
      nbrcote1 = 2;
      cote1(1) = Radius;
      cote1(2) = -Radius;
    }
    Geom2dInt_GInter Intp;
    for (Standard_Integer jcote1 = 1 ; jcote1 <= nbrcote1 ; jcote1++) {
      Handle(Geom2dAdaptor_HCurve) HCu1 = new Geom2dAdaptor_HCurve(Cu1);
      Adaptor2d_OffsetCurve C1(HCu1,cote1(jcote1));
      firstparam = Max(C1.FirstParameter(),thefirst);
      lastparam  = Min(C1.LastParameter(),thelast);
      IntRes2d_Domain D1(C1.Value(firstparam), firstparam, Tol,
                         C1.Value(lastparam), lastparam, Tol);
      Handle(Geom2dAdaptor_HCurve) HOnCurv = new Geom2dAdaptor_HCurve(OnCurv);
      Adaptor2d_OffsetCurve C2(HOnCurv);
      firstparam = Max(C2.FirstParameter(),thefirst);
      lastparam  = Min(C2.LastParameter(),thelast);
      IntRes2d_Domain D2(C2.Value(firstparam), firstparam, Tol,
                         C2.Value(lastparam), lastparam, Tol);
      Intp.Perform(C1,D1,C2,D2,Tol,Tol);
      if (Intp.IsDone()) {
        if (!Intp.IsEmpty()) {
          for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
            NbrSol++;
            gp_Pnt2d Center(Intp.Point(i).Value());
            cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
            //           =======================================================
            qualifier1(NbrSol) = Qualified1.Qualifier();
            TheSame1(NbrSol) = 0;
            pararg1(NbrSol) = Intp.Point(i).ParamOnFirst();
            parcen3(NbrSol) = Intp.Point(i).ParamOnSecond();
            par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
              pnttg1sol(NbrSol));
            pnttg1sol(NbrSol) = gp_Pnt2d(Geom2dGcc_CurveTool::Value(Cu1,pararg1(NbrSol)));
            pntcen3(NbrSol) = Center;
          }
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================
//  Cercle tangent  :  a un cercle Qualified1 (C1).                       +
//         centre   :  sur une droite OnLine.                             +
//         de rayon :  Radius.                                            +
//                                                                        + 
//  On initialise le tableau de solutions cirsol ainsi que tous les       +
//  champs.                                                               +
//  On elimine en fonction du qualifieur les cas ne presentant pas de     +
//  solutions.                                                            +
//  On resoud l equation du second degre indiquant que le point de centre +
//  recherche (xc,yc) est a une distance Radius du cercle C1 et           +
//                        sur la droite OnLine.                           +
//  Les solutions sont representees par les cercles :                     +
//                   - de centre Pntcen(xc,yc)                            +
//                   - de rayon Radius.                                   +
//=========================================================================

Geom2dGcc_Circ2dTanOnRadGeo::
Geom2dGcc_Circ2dTanOnRadGeo (const gp_Pnt2d&     Point1    ,
                             const Geom2dAdaptor_Curve&     OnCurv    ,
                             const Standard_Real Radius    ,
                             const Standard_Real Tolerance ):

//=========================================================================
// Initialisation des champs.                                             +
//=========================================================================

cirsol(1,8)     ,
qualifier1(1,8) ,
TheSame1(1,8)   ,
pnttg1sol(1,8)  ,
pntcen3(1,8)    ,
par1sol(1,8)    ,
pararg1(1,8)    ,
parcen3(1,8)    
{

  //=========================================================================
  // Traitement.                                                            +
  //=========================================================================

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real thefirst = -100000.;
  Standard_Real thelast  =  100000.;
  Standard_Real firstparam;
  Standard_Real lastparam;
  Standard_Real Tol = Abs(Tolerance);
  WellDone = Standard_False;
  NbrSol = 0;

  if (Radius < 0.0) {
    throw Standard_NegativeValue();
  }
  else {
    //     gp_Dir2d Dir(-y1dir,x1dir);
    gp_Circ2d Circ(gp_Ax2d(Point1,gp_Dir2d(1.,0.)),Radius);
    IntRes2d_Domain D1(ElCLib::Value(0.,Circ),   0.,Tol,
      ElCLib::Value(2.*M_PI,Circ),2*M_PI,Tol);
    D1.SetEquivalentParameters(0.,2.*M_PI);
    firstparam = Max(Geom2dGcc_CurveTool::FirstParameter(OnCurv),thefirst);
    lastparam  = Min(Geom2dGcc_CurveTool::LastParameter(OnCurv),thelast);
    IntRes2d_Domain D2(Geom2dGcc_CurveTool::Value(OnCurv,firstparam),firstparam,Tol,
      Geom2dGcc_CurveTool::Value(OnCurv,lastparam),lastparam,Tol);
    Geom2dInt_TheIntConicCurveOfGInter Intp(Circ,D1,OnCurv,D2,Tol,Tol);
    if (Intp.IsDone()) {
      if (!Intp.IsEmpty()) {
        for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
          NbrSol++;
          gp_Pnt2d Center(Intp.Point(i).Value());
          cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
          //         =======================================================
          qualifier1(NbrSol) = GccEnt_noqualifier;
          TheSame1(NbrSol) = 0;
          pararg1(NbrSol) = Intp.Point(i).ParamOnFirst();
          parcen3(NbrSol) = Intp.Point(i).ParamOnSecond();
          par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
            pnttg1sol(NbrSol));
          pnttg1sol(NbrSol) = Point1;
          pntcen3(NbrSol) = Center;
        }
        WellDone = Standard_True;
      }
    }
  }
}

//=========================================================================

Standard_Boolean Geom2dGcc_Circ2dTanOnRadGeo::
IsDone () const { return WellDone; }

Standard_Integer Geom2dGcc_Circ2dTanOnRadGeo::
NbSolutions () const { return NbrSol; }

gp_Circ2d Geom2dGcc_Circ2dTanOnRadGeo::
ThisSolution (const Standard_Integer Index) const 
{

  if (Index > NbrSol || Index <= 0)
    throw Standard_OutOfRange();

  return cirsol(Index);
}

void Geom2dGcc_Circ2dTanOnRadGeo::
WhichQualifier(const Standard_Integer Index   ,
               GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { throw StdFail_NotDone(); }
  else if (Index <= 0 ||Index > NbrSol) { throw Standard_OutOfRange(); }
  else {
    Qualif1 = qualifier1(Index);
  }
}

void Geom2dGcc_Circ2dTanOnRadGeo::
Tangency1 (const Standard_Integer Index,
           Standard_Real&   ParSol,
           Standard_Real&   ParArg,
           gp_Pnt2d& PntSol) const{
             if (!WellDone) {
               throw StdFail_NotDone();
             }
             else if (Index <= 0 ||Index > NbrSol) {
               throw Standard_OutOfRange();
             }
             else {
               ParSol = par1sol(Index);
               ParArg = pararg1(Index);
               PntSol = gp_Pnt2d(pnttg1sol(Index));
             }
}

void Geom2dGcc_Circ2dTanOnRadGeo::
CenterOn3 (const Standard_Integer Index,
           Standard_Real&   ParArg, 
           gp_Pnt2d&        PntSol) const {
             if (!WellDone) {
               throw StdFail_NotDone();
             }
             else if (Index <= 0 ||Index > NbrSol) {
               throw Standard_OutOfRange();
             }
             else {
               ParArg = parcen3(Index);
               PntSol = pnttg1sol(Index);
             }
}

Standard_Boolean Geom2dGcc_Circ2dTanOnRadGeo::
IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) throw StdFail_NotDone();
  if (Index <= 0 ||Index > NbrSol) throw Standard_OutOfRange();

  if (TheSame1(Index) == 0) 
    return Standard_False;

  return Standard_True;
}
