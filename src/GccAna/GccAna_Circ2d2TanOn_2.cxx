// Created on: 1992-01-02
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
#include <GccAna_Circ2d2TanOn.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedLin&  Qualified1 , 
                        const GccEnt_QualifiedLin&  Qualified2 , 
                        const gp_Lin2d&             OnLine     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,2)     ,
   qualifier1(1,2) ,
   qualifier2(1,2) ,
   TheSame1(1,2)   ,
   TheSame2(1,2)   ,
   pnttg1sol(1,2)  ,
   pnttg2sol(1,2)  ,
   pntcen(1,2)     ,
   par1sol(1,2)    ,
   par2sol(1,2)    ,
   pararg1(1,2)    ,
   pararg2(1,2)    ,
   parcen3(1,2)
{
  // initialisations
  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;

  if (!(Qualified1.IsEnclosed() ||
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() ||
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
  throw GccEnt_BadQualifier();
    return;
  }

  gp_Dir2d dirx(1.,0.);
  Standard_Real Tol = Abs(Tolerance);

  // calculation of bisectrices of L1 and L2
  gp_Lin2d L1(Qualified1.Qualified());
  gp_Lin2d L2(Qualified2.Qualified());
  gp_Pnt2d originL1(L1.Location());
  gp_Pnt2d originL2(L2.Location());
  GccAna_Lin2dBisec Bis(L1,L2);
  
  if (Bis.IsDone()) {
    
    if (Bis.NbSolutions() == 1 || Bis.NbSolutions() == 2) {
      // if 1 bisectrice, L1 and L2 are parallel
      // if 2 bisectrices, L1 and L2 are intersected

      for (Standard_Integer k = 1 ; k <= Bis.NbSolutions() ; k++) {
	IntAna2d_AnaIntersection Intp(Bis.ThisSolution(k),OnLine);
	if (Intp.IsDone()) {
	  WellDone = Standard_True;
	  // for degenerated cases, no acceptable solution
	  // (OnLine and bisectrice strictly parallel or not)
	  if (!Intp.IdenticalElements() 
	       && !Intp.ParallelElements() 
	       && !Intp.IsEmpty()) {
	    // at maximum 1 point of intersection !
	    for (Standard_Integer l = 1 ; l <= Intp.NbPoints() ; l++) {
	      gp_Pnt2d pt(Intp.Point(l).Value());
	      gp_Ax2d axe(pt,dirx);
	      Standard_Real Radius = L1.Distance(pt);
	      if (!L1.Contains(pt,Tol) && Radius<1.0/Tol && NbrSol<2) {
		// acceptable solution : the radius is correct
		NbrSol++;
		cirsol(NbrSol) = gp_Circ2d(axe,Radius);
	      }
	    }
	  }
	}
      }

    }    

  }
  
  // parce following the qualifiers NbrSol acceptable solutions
  
  for (Standard_Integer i=1 ; i <= NbrSol ; i++) {

    gp_Pnt2d pbid(cirsol(i).Location());
    Standard_Real Radius = cirsol(i).Radius();
    Standard_Boolean ok = Standard_False;
    
    // solution Outside or Enclosed / L1
    gp_Dir2d dc1(originL1.XY()-pbid.XY());
    Standard_Real sign1 = dc1.Dot(gp_Dir2d(-L1.Direction().Y(),
					    L1.Direction().X()));
    if (sign1>0.0) ok = (Qualified1.IsUnqualified() 
			  || Qualified1.IsOutside());
    else ok = (Qualified1.IsUnqualified() 
	        || Qualified1.IsEnclosed());

    // solution Outside or Enclosed / L2
    gp_Dir2d dc2(originL2.XY()-pbid.XY());
    Standard_Real sign2 = dc2.Dot(gp_Dir2d(-L2.Direction().Y(),
                                            L2.Direction().X()));
    if (sign2>0.0) ok = ok && (Qualified2.IsUnqualified() 
			        || Qualified2.IsOutside());
    else ok = ok &&(Qualified2.IsUnqualified() 
		     || Qualified2.IsEnclosed());

    if (ok) {
      // solution to be preserved
      dc1 = gp_Dir2d(sign1*gp_XY(-L1.Direction().Y(),
				  L1.Direction().X()));
      pnttg1sol(i) = gp_Pnt2d(pbid.XY()+Radius*dc1.XY());
      if (sign1>0.0) qualifier1(i) = GccEnt_outside;
      else qualifier1(i) = GccEnt_enclosed;
      dc2 = gp_Dir2d(sign2*gp_XY(-L2.Direction().Y(),
				  L2.Direction().X()));
      pnttg2sol(i) = gp_Pnt2d(pbid.XY()+Radius*dc2.XY());
      if (sign2>0.0) qualifier2(i) = GccEnt_outside;
      else qualifier2(i) = GccEnt_enclosed;
      pntcen(i) = pbid;
      par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
      pararg1(i)=ElCLib::Parameter(L1,pnttg1sol(i));
      par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
      pararg2(i)=ElCLib::Parameter(L2,pnttg2sol(i));
      parcen3(i)=ElCLib::Parameter(OnLine,pntcen(i));
    }
    else {
      // solution to be rejected
      if (i==NbrSol) NbrSol--;
      else {
	for (Standard_Integer k = i+1 ; k <= NbrSol ; k++) {
	  cirsol(k-1) = cirsol(k);
	}
	NbrSol--;
	i--;
      }
    }

  }

}

