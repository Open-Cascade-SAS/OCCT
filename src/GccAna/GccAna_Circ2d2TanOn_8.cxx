// Created on: 1992-01-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


//=========================================================================
//   Creation of a circle tangent to two elements : Straight line.        +
//                                                  Circle.               +
//                                                  Point.                +
//                              center on a third : Straight line.        +
//                                                  Circle.               +
//=========================================================================

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_ConstructionError.hxx>
#include <GccInt_IType.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_BCirc.hxx>
#include <IntAna2d_Conic.hxx>
#include <GccAna_CircPnt2dBisec.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <Precision.hxx>
GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1 ,
                        const gp_Pnt2d&             Point2     ,
                        const gp_Circ2d&            OnCirc     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4),
   TheSame1(1,4)   ,
   TheSame2(1,4)   ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   pntcen(1,4)     ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    ,
   parcen3(1,4)    
{
  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
    GccEnt_BadQualifier::Raise();
    return;
  }
  Standard_Real Tol = Abs(Tolerance);
  TColStd_Array1OfReal Radius(1,2);
  gp_Dir2d dirx(1.,0.);
  gp_Circ2d C1 = Qualified1.Qualified();
  Standard_Real R1 = C1.Radius();
  gp_Pnt2d center1(C1.Location());

//=========================================================================
//   Processing of boundary cases.                                          +
//=========================================================================

  Standard_Integer nbsol1 = 1;
//  Standard_Integer nbsol2 = 0;
  Standard_Real Ron = OnCirc.Radius();
  Standard_Real distcco = OnCirc.Location().Distance(center1);
  gp_Pnt2d pinterm;
  gp_Dir2d dircc;
  Standard_Boolean SameCenter(Standard_False);
  if (!OnCirc.Location().IsEqual(center1,Precision::Confusion())) {
    dircc = gp_Dir2d(OnCirc.Location().XY()-center1.XY());
    pinterm = gp_Pnt2d(center1.XY()+(distcco-Ron)*dircc.XY());
    Standard_Real distcc2 =pinterm.Distance(Point2);
    Standard_Real distcc1 =pinterm.Distance(center1);
    Standard_Real d1 = Abs(distcc2-Abs(distcc1-R1));
    Standard_Real d2 = Abs(distcc2-(distcc1+R1));
    if ( d1 > Tol || d2 > Tol) {
      if (!SameCenter) pinterm = gp_Pnt2d(center1.XY()+(distcco+Ron)*dircc.XY());
      distcc2 =pinterm.Distance(Point2);
      distcc1 =pinterm.Distance(center1);
      d1 = Abs(distcc2-Abs(distcc1-R1));
      d2 = Abs(distcc2-(distcc1+R1));
      if ( d1 > Tol || d2 > Tol ) { nbsol1 = 0; }
    }
    if (nbsol1 > 0) {
      if (Qualified1.IsEnclosed() || Qualified1.IsOutside()) {
	nbsol1 = 1;
	Radius(1) = Abs(distcc1-R1);
      }
      else if (Qualified1.IsEnclosing()) {
	nbsol1 = 1;
	Radius(1) = R1+distcc1;
      }
      else if (Qualified1.IsUnqualified()) {
	nbsol1 = 2;
	Radius(1) = Abs(distcc1-R1);
	Radius(2) = R1+distcc1;
      }
      for (Standard_Integer i = 1 ; i <= nbsol1 ; i++) {
	WellDone = Standard_True;
	if (!(i == 1 && SameCenter)) {
	  NbrSol++;
	  cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),Radius(i));
//    ===========================================================
	  gp_Dir2d dc1;
	  if (!SameCenter) dc1 = gp_Dir2d(center1.XY()-pinterm.XY());
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
	  qualifier2(NbrSol) = GccEnt_noqualifier;
	  if (!SameCenter) 
	    pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+Radius(i)*dc1.XY());
	  else
	    pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY());
	  pnttg2sol(NbrSol) = Point2;
	  pntcen(NbrSol) = cirsol(NbrSol).Location();
	  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	  pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	  par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
	  pararg2(NbrSol) = 0.;
	  parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
	}
      }
      if (WellDone) { return; }
    }
  }
  
//=========================================================================
//   General case.                                                         +
//=========================================================================

  GccAna_CircPnt2dBisec Bis(C1,Point2);
  if (Bis.IsDone()) {
    Standard_Integer nbsolution = Bis.NbSolutions();
    for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {

      Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
      GccInt_IType type = Sol->ArcType();
      IntAna2d_AnaIntersection Intp;
      if (type == GccInt_Lin) {
	Intp.Perform(Sol->Line(),OnCirc);
      }
      else if (type == GccInt_Cir) {
	Intp.Perform(OnCirc,Sol->Circle());
      }
      else if (type == GccInt_Hpr) {
	Intp.Perform(OnCirc,IntAna2d_Conic(Sol->Hyperbola()));
      }
      else if (type == GccInt_Ell) {
	Intp.Perform(OnCirc,IntAna2d_Conic(Sol->Ellipse()));
      }
      if (Intp.IsDone()) {
	if ((!Intp.IsEmpty())&&(!Intp.ParallelElements())&&
	    (!Intp.IdenticalElements())) {
	  for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	    gp_Pnt2d Center(Intp.Point(j).Value());
	    Standard_Real dist1 = center1.Distance(Center);
	    Standard_Integer nbsol = 1;
	    Standard_Boolean ok = Standard_False;
	    if (Qualified1.IsEnclosed()) {
	      if (dist1-C1.Radius() <= Tol) {
		ok = Standard_True;
		Radius(1) = Abs(C1.Radius()-dist1);
	      }
	    }
	    else if (Qualified1.IsOutside()) {
	      if (C1.Radius()-dist1 <= Tol) {
		ok = Standard_True;
		Radius(1) = Abs(C1.Radius()-dist1);
	      }
	    }
	    else if (Qualified1.IsEnclosing()) {
	      ok = Standard_True;
	      Radius(1) = C1.Radius()+dist1;
	    }
	    else if (Qualified1.IsUnqualified()) {
	      ok = Standard_True;
	      nbsol = 2;
	      Radius(1) = Abs(C1.Radius()-dist1);
	      Radius(2) = C1.Radius()+dist1;
	    }
	    if (ok) {
	      for (Standard_Integer k = 1 ; k <= nbsol ; k++) {
		
		// pop : protection against case center1 == Center
		if (center1.IsEqual(Center,Precision::Confusion())) {
		  continue;
		}		  
		if (OnCirc.Distance(Center)>Tol) {
		  continue;
		}
		if (NbrSol == 4 ) break;
		NbrSol++;
		cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
//              ==========================================================
		Standard_Real distcc1 = Center.Distance(center1);
		if (!Qualified1.IsUnqualified()) { 
		  qualifier1(NbrSol) = Qualified1.Qualifier();
		}
		else if (Abs(distcc1+Radius(k)-R1) < Tol) {
		  qualifier1(NbrSol) = GccEnt_enclosed;
		}
		else if (Abs(distcc1-R1-Radius(k)) < Tol) {
		  qualifier1(NbrSol) = GccEnt_outside;
		}
		else { qualifier1(NbrSol) = GccEnt_enclosing; }
		qualifier2(NbrSol) = GccEnt_noqualifier;
		if (distcc1 <= Tol && Abs(Radius(k)-C1.Radius()) <= Tol) {
		  TheSame1(NbrSol) = 1;
		}
		else {
		  TheSame1(NbrSol) = 0;
		  gp_Dir2d dc1(center1.XY()-Center.XY());
		  pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+
					       Radius(k)*dc1.XY());
		  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg1sol(NbrSol));
		  pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		}
		TheSame2(NbrSol) = 0;
		pntcen(NbrSol) = Center;
		pnttg2sol(NbrSol) = Point2;
		pararg2(NbrSol) = 0.;
		par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						  pnttg2sol(NbrSol));
		parcen3(NbrSol)=ElCLib::Parameter(OnCirc,pntcen(NbrSol));
	      }
	    }
	  }
	}
	WellDone = Standard_True;
      }
    }
  }
}
