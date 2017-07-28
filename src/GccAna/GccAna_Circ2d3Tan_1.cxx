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


#include <ElCLib.hxx>
#include <GccAna_Circ2d3Tan.hxx>
#include <GccAna_CircLin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_BParab.hxx>
#include <GccInt_IType.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

//=========================================================================
//   Creation of a circle tangent to two circles and a straight line.     +
//=========================================================================
GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const GccEnt_QualifiedCirc& Qualified1,
                      const GccEnt_QualifiedCirc& Qualified2,
                      const GccEnt_QualifiedLin&  Qualified3,
		      const Standard_Real         Tolerance ):

//=========================================================================
//   Initialization of fields.                                           +
//=========================================================================

   cirsol(1,16)     , 
   qualifier1(1,16) ,
   qualifier2(1,16) ,
   qualifier3(1,16),
   TheSame1(1,16)   ,
   TheSame2(1,16)   ,
   TheSame3(1,16)   ,
   pnttg1sol(1,16)  ,
   pnttg2sol(1,16)  ,
   pnttg3sol(1,16)  ,
   par1sol(1,16)    ,
   par2sol(1,16)    ,
   par3sol(1,16)    ,
   pararg1(1,16)    ,
   pararg2(1,16)    ,
   pararg3(1,16)    
{

  gp_Dir2d dirx(1.0,0.0);
  Standard_Real Tol = Abs(Tolerance);
  WellDone = Standard_False;
  NbrSol = 0;
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
	Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
      !(Qualified3.IsEnclosed() || 
	Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Circ2d C2 = Qualified2.Qualified();
  gp_Lin2d L3 = Qualified3.Qualified();
  Standard_Real R1 = C1.Radius();
  Standard_Real R2 = C2.Radius();
  gp_Pnt2d center1(C1.Location());
  gp_Pnt2d center2(C2.Location());
  (void )center2;

  gp_Pnt2d origin3(L3.Location());
  gp_Dir2d dir3(L3.Direction());
  gp_Dir2d normL3(-dir3.Y(),dir3.X());
  
  TColStd_Array1OfReal Radius(1,2);
  GccAna_CircLin2dBisec Bis1(C1,L3);
  GccAna_CircLin2dBisec Bis2(C2,L3);
  if (Bis1.IsDone() && Bis2.IsDone()) {
    Standard_Integer nbsolution1 = Bis1.NbSolutions();
    Standard_Integer nbsolution2 = Bis2.NbSolutions();
    for (Standard_Integer i = 1 ; i <=  nbsolution1; i++) {
      Handle(GccInt_Bisec) Sol1 = Bis1.ThisSolution(i);
      GccInt_IType typ1 = Sol1->ArcType();
      IntAna2d_AnaIntersection Intp;
      for (Standard_Integer k = 1 ; k <=  nbsolution2; k++) {
	Handle(GccInt_Bisec) Sol2 = Bis2.ThisSolution(k);
	GccInt_IType typ2 = Sol2->ArcType();
	if (typ1 == GccInt_Lin) {
	  if (typ2 == GccInt_Lin) {
	    Intp.Perform(Sol1->Line(),Sol2->Line());
	  }
	  else if (typ2 == GccInt_Par) {
	    Intp.Perform(Sol1->Line(),IntAna2d_Conic(Sol2->Parabola()));
	  }
	}
	else if (typ1 == GccInt_Par) {
	  if (typ2 == GccInt_Lin) {
	    Intp.Perform(Sol2->Line(),IntAna2d_Conic(Sol1->Parabola()));
	  }
	  else if (typ2 == GccInt_Par) {
	    Intp.Perform(Sol1->Parabola(),IntAna2d_Conic(Sol2->Parabola()));
	  }
	}
	if (Intp.IsDone()) {
	  if (!Intp.IsEmpty()) {
	    for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	      Standard_Real Rradius=0;
	      gp_Pnt2d Center(Intp.Point(j).Value());

// pop : if the coordinates are too great, no creation		 
	      if (Center.X() > 1e10 || 
		  Center.Y() > 1e10  ) break;	      

	      Standard_Real dist1 = Center.Distance(C1.Location());
	       Standard_Real dist2 = Center.Distance(C2.Location());
	      Standard_Real dist3 = L3.Distance(Center);

// pop : if the coordinates are too great, no creation			 
	      if (dist3 > 1e10  ) break;	

	      Standard_Integer nbsol1 = 0;
	      Standard_Integer nbsol2 = 0;
	       Standard_Integer nbsol3 = 0;
	      Standard_Boolean ok = Standard_False;
	      if (Qualified1.IsEnclosed()) {
		if (dist1-R1 < Tolerance) {
		  Radius(1) = Abs(R1-dist1);
		   nbsol1 = 1;
		  ok = Standard_True;
		}
	      }
	      else if (Qualified1.IsOutside()) {
		if (R1-dist1 < Tolerance) {
		  Radius(1) = Abs(R1-dist1);
		  nbsol1 = 1;
		  ok = Standard_True;
		}
	      }
	      else if (Qualified1.IsEnclosing()) {
		ok = Standard_True;
		nbsol1 = 1;
		Radius(1) = Abs(R1-dist1);
	       }
	      else if (Qualified1.IsUnqualified()) {
		ok = Standard_True;
		nbsol1 = 2;
		Radius(1) = Abs(R1-dist1);
		Radius(2) = R1+dist1;
	      }
	      if (Qualified2.IsEnclosed() && ok) {
		if (dist2-R2 < Tolerance) {
		  for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		    if (Abs(Radius(ii)-Abs(R2-dist2)) < Tol) {
		      Radius(1) = Abs(R2-dist2);
		      ok = Standard_True;
		      nbsol2 = 1;
		    }
		  }
		}
	      }
	      else if (Qualified2.IsOutside() && ok) {
		if (R2-dist2 < Tolerance) {
		  for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		    if (Abs(Radius(ii)-Abs(R2-dist2)) < Tol) {
		      Radius(1) = Abs(R2-dist2);
		      ok = Standard_True;
		      nbsol2 = 1;
		    }
		  }
		}
	      }
	      else if (Qualified2.IsEnclosing() && ok) {
		for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		  if (Abs(Radius(ii)-R2-dist2) < Tol) {
		    Radius(1) = R2+dist2;
		    ok = Standard_True;
		    nbsol2 = 1;
		  }
		}
	      }
	      else if (Qualified2.IsUnqualified() && ok) {
		for (Standard_Integer ii = 1 ; ii <= nbsol1 ; ii++) {
		  if (Abs(Radius(ii)-Abs(R2-dist2)) < Tol) {
		     Rradius = Abs(R2-dist2);
		     ok = Standard_True;
		     nbsol2++;
		   }
		  else if (Abs(Radius(ii)-R2-dist2) < Tol) {
		    Rradius = R2+dist2;
		    ok = Standard_True;
		    nbsol2++;
		  }
		}
		if (nbsol2 == 1) {
		  Radius(1) = Rradius;
		}
		else if (nbsol2 == 2) {
		  Radius(1) = Abs(R2-dist2);
		  Radius(2) = R2+dist2;
		}
	      }
	      if (Qualified3.IsEnclosed() && ok) {
		if ((((L3.Location().X()-Center.X())*(-L3.Direction().Y()))+
		    ((L3.Location().Y()-Center.Y())*(L3.Direction().X())))<=0){
		  ok = Standard_True;
		  nbsol3 = 1;
		}
	      }
	      else if (Qualified2.IsOutside() && ok) {
		if ((((L3.Location().X()-Center.X())*(-L3.Direction().Y()))+
	            ((L3.Location().Y()-Center.Y())*(L3.Direction().X())))>=0){
		  ok = Standard_True;
		  nbsol3 = 1;
		}
	      }
	      else if (Qualified2.IsUnqualified() && ok) {
		ok = Standard_True;
		nbsol3 = 1;
	      }
	      if (ok) {
		for (Standard_Integer ind3 = 1 ; ind3 <= nbsol3 ; ind3++) {
		  NbrSol++;
		  cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(ind3));
//                ==========================================================
		  Standard_Real distcc1 = Center.Distance(center1);
		  if (!Qualified1.IsUnqualified()) { 
		    qualifier1(NbrSol) = Qualified1.Qualifier();
		  }
		  else if (Abs(distcc1+Radius(ind3)-R1) < Tol) {
		    qualifier1(NbrSol) = GccEnt_enclosed;
		  }
		  else if (Abs(distcc1-R1-Radius(ind3)) < Tol) {
		    qualifier1(NbrSol) = GccEnt_outside;
		  }
		  else { qualifier1(NbrSol) = GccEnt_enclosing; }
		  Standard_Real distcc2 = Center.Distance(center1);
		  if (!Qualified2.IsUnqualified()) { 
		    qualifier2(NbrSol) = Qualified2.Qualifier();
		  }
		  else if (Abs(distcc2+Radius(ind3)-R2) < Tol) {
		    qualifier2(NbrSol) = GccEnt_enclosed;
		  }
		  else if (Abs(distcc2-R2-Radius(ind3)) < Tol) {
		    qualifier2(NbrSol) = GccEnt_outside;
		  }
		  else { qualifier2(NbrSol) = GccEnt_enclosing; }
		  gp_Dir2d dc3(origin3.XY()-Center.XY());
		  if (!Qualified3.IsUnqualified()) { 
		    qualifier3(NbrSol) = Qualified3.Qualifier();
		  }
		  else if (dc3.Dot(normL3) > 0.0) {
		    qualifier3(NbrSol) = GccEnt_outside;
		  }
		  else { qualifier3(NbrSol) = GccEnt_enclosed; }
		  if (Center.Distance(C1.Location()) <= Tolerance &&
		      Abs(Radius(ind3)-R1) <= Tolerance) {
		    TheSame1(NbrSol) = 1;
		  }
		  else {
		    TheSame1(NbrSol) = 0;
		    gp_Dir2d dc(C1.Location().XY()-Center.XY());
		    pnttg1sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(ind3)*dc.XY());
		    // POP for protection if cirsol(NbrSol).Location == pnttg1sol(NbrSol)
		    if (cirsol(NbrSol).Location().IsEqual(pnttg1sol(NbrSol),Precision::Confusion()))
		      par1sol(NbrSol)=1;
		    else
		      par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
							pnttg1sol(NbrSol));
		    // POP for protection if C1.Location == pnttg1sol(NbrSol)
		    if (C1.Location().IsEqual(pnttg1sol(NbrSol),Precision::Confusion()))
		      pararg1(NbrSol)=1;
		    else
		      pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
		  }
		  if (Center.Distance(C2.Location()) <= Tolerance &&
		      Abs(Radius(ind3)-R2) <= Tolerance) {
		    TheSame2(NbrSol) = 1;
		  }
		  else {
		    TheSame2(NbrSol) = 0;
		    gp_Dir2d dc(C2.Location().XY()-Center.XY());
		    pnttg2sol(NbrSol)=gp_Pnt2d(Center.XY()+Radius(ind3)*dc.XY());
		    // POP for protection if cirsol(NbrSol).Location == pnttg1sol(NbrSol)
		    if (cirsol(NbrSol).Location().IsEqual(pnttg1sol(NbrSol),Precision::Confusion()))
		      par1sol(NbrSol)=1;
		    else
		      par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
							pnttg2sol(NbrSol));
		    // POP for protection if C2.Location == pnttg2sol(NbrSol)
		    if (C2.Location().IsEqual(pnttg2sol(NbrSol),Precision::Confusion()))
		      pararg2(NbrSol)=1;
		    else
		      pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
		  }
		  TheSame3(NbrSol) = 0;
		  gp_Dir2d dc(L3.Location().XY()-Center.XY());
		  Standard_Real sign = dc.Dot(gp_Dir2d(-L3.Direction().Y(),
						       L3.Direction().X()));
		  dc = gp_Dir2d(sign*gp_XY(-L3.Direction().Y(),
					   L3.Direction().X()));
		   pnttg3sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius(ind3)*dc.XY());
		  par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						    pnttg3sol(NbrSol));
		  pararg3(NbrSol)=ElCLib::Parameter(L3,pnttg3sol(NbrSol));
		}
	      }
	    }
	   }
	   WellDone = Standard_True;
	}
       }
    }
  }
}
