// Created on: 1991-09-24
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


#include <ElCLib.hxx>
#include <GccAna_Circ2d2TanRad.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Precision.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

// circular tangent to two lines of given radius
//===============================================
//========================================================================
// Initialize WellDone to false.                                       +
// Return two lines L1 and L2.                                 +
// Leave with error if the construction is impossible.     +
// Create parallel lines to L1 and L2 in the proper direction.                   +
// Intersect parallels ==> The center point of the solution.   +
// Create the solution to be added to the already found solutions.         +
// Fill the fields.                                                +
//========================================================================
GccAna_Circ2d2TanRad::
   GccAna_Circ2d2TanRad (const GccEnt_QualifiedLin&  Qualified1 ,
                         const GccEnt_QualifiedLin&  Qualified2 ,
                         const Standard_Real         Radius     ,
                         const Standard_Real                    ):
   qualifier1(1,4) ,
   qualifier2(1,4),
   TheSame1(1,4)   ,
   TheSame2(1,4)   ,
   cirsol(1,4)     ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    
{

  gp_Dir2d dirx(1.0,0.0);
  TColStd_Array1OfReal cote1(1,2);
  TColStd_Array1OfReal cote2(1,2);
  Standard_Integer nbrcote1=0;
  Standard_Integer nbrcote2=0;
  NbrSol = 0;
  WellDone = Standard_False;
  if (!(Qualified1.IsEnclosed() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() ||
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
    throw GccEnt_BadQualifier();
    return;
  }
  gp_Lin2d L1 = Qualified1.Qualified();
  gp_Lin2d L2 = Qualified2.Qualified();
  Standard_Real x1dir = (L1.Direction()).X();
  Standard_Real y1dir = (L1.Direction()).Y();
  Standard_Real lx1loc = (L1.Location()).X();
  Standard_Real ly1loc = (L1.Location()).Y();
  Standard_Real x2dir = (L2.Direction()).X();
  Standard_Real y2dir = (L2.Direction()).Y();
  Standard_Real lx2loc = (L2.Location()).X();
  Standard_Real ly2loc = (L2.Location()).Y();
  gp_Pnt2d origin1(lx1loc,ly1loc);
  gp_Pnt2d origin2(lx2loc,ly2loc);
  gp_Dir2d normL1(x1dir,y1dir);
  gp_Dir2d normL2(x2dir,y2dir);
  if (Radius < 0.0) { throw Standard_NegativeValue(); }
  else {
    if (L1.Direction().IsParallel(L2.Direction(),Precision::Angular())) { 
      WellDone = Standard_True;
    }
    else {
      if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed()) {
//    =======================================================
	nbrcote1 = 1;
	nbrcote2 = 1;
	cote1(1) = 1.0;
	cote2(1) = 1.0;
      }
      else if(Qualified1.IsEnclosed() && Qualified2.IsOutside()) {
//    ==========================================================
	nbrcote1 = 1;
	nbrcote2 = 1;
	cote1(1) = 1.0;
	cote2(1) = -1.0;
      }
      else if (Qualified1.IsOutside() && Qualified2.IsEnclosed()) {
//    ===========================================================
	nbrcote1 = 1;
	nbrcote2 = 1;
	cote1(1) = -1.0;
	cote2(1) = 1.0;
      }
      else if(Qualified1.IsOutside() && Qualified2.IsOutside()) {
//    =========================================================
	nbrcote1 = 1;
	nbrcote2 = 1;
	cote1(1) = -1.0;
	cote2(1) = -1.0;
      }
      if(Qualified1.IsEnclosed() && Qualified2.IsUnqualified()) {
//    =========================================================
	nbrcote1 = 1;
	nbrcote2 = 2;
	cote1(1) = 1.0;
	cote2(1) = 1.0;
	cote2(2) = -1.0;
      }
      if(Qualified1.IsUnqualified() && Qualified2.IsEnclosed()) {
//    =========================================================
	nbrcote1 = 2;
	nbrcote2 = 1;
	cote1(1) = 1.0;
	cote1(2) = -1.0;
	cote2(1) = 1.0;
      }
      else if(Qualified1.IsOutside() && Qualified2.IsUnqualified()) {
//    =============================================================
	nbrcote1 = 1;
	nbrcote2 = 2;
	cote1(1) = -1.0;
	cote2(1) = 1.0;
	cote2(2) = -1.0;
      }
      if(Qualified1.IsUnqualified() && Qualified2.IsOutside()) {
//    ========================================================
	nbrcote1 = 2;
	nbrcote2 = 1;
	cote1(1) = 1.0;
	cote1(2) = -1.0;
	cote2(1) = -1.0;
      }
      else if(Qualified1.IsUnqualified() && Qualified2.IsUnqualified()) {
//    =================================================================
	nbrcote1 = 2;
	nbrcote2 = 2;
	cote1(1) = 1.0;
	cote1(2) = -1.0;
	cote2(1) = 1.0;
	cote2(2) = -1.0;
      }
      for (Standard_Integer jcote1 = 1 ; jcote1 <= nbrcote1 ; jcote1++) {
	for (Standard_Integer jcote2 = 1 ; jcote2 <= nbrcote2 ; jcote2++) {
	  gp_Lin2d linint1(gp_Pnt2d(lx1loc-cote1(jcote1)*y1dir*Radius,
				    ly1loc+cote1(jcote1)*x1dir*Radius),
			   L1.Direction());
	  gp_Lin2d linint2(gp_Pnt2d(lx2loc-cote2(jcote2)*y2dir*Radius,
				    ly2loc+cote2(jcote2)*x2dir*Radius),
			   L2.Direction());
	  IntAna2d_AnaIntersection Intp(linint1,linint2);
	  if (Intp.IsDone()) {
	    if (!Intp.IsEmpty()) {
	      for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
		NbrSol++;
		gp_Pnt2d Center(Intp.Point(i).Value());
		cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//              =======================================================
		gp_Dir2d dc1(origin1.XY()-Center.XY());
		gp_Dir2d dc2(origin2.XY()-Center.XY());
		if (!Qualified1.IsUnqualified()) { 
		  qualifier1(NbrSol) = Qualified1.Qualifier();
		}
		else if (dc1.Dot(normL1) > 0.0) {
		  qualifier1(NbrSol) = GccEnt_outside;
		}
		else { qualifier1(NbrSol) = GccEnt_enclosed; }
		if (!Qualified2.IsUnqualified()) { 
		  qualifier2(NbrSol) = Qualified2.Qualifier();
		}
		else if (dc2.Dot(normL2) > 0.0) {
		  qualifier2(NbrSol) = GccEnt_outside;
		}
		else { qualifier2(NbrSol) = GccEnt_enclosed; }
		TheSame1(NbrSol) = 0;
		TheSame2(NbrSol) = 0;
		pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+
				     cote1(jcote1)*Radius*gp_XY(y1dir,-x1dir));
		pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+
				     cote2(jcote2)*Radius*gp_XY(y2dir,-x2dir));
	      }
	    }
	    WellDone = Standard_True;
	  }
	}
      }
    }
  }
  for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
    par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
    pararg1(i)=ElCLib::Parameter(L1,pnttg1sol(i));
    par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
    pararg2(i)=ElCLib::Parameter(L2,pnttg2sol(i));
  }
}



