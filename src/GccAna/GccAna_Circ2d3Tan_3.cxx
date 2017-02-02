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
#include <GccAna_Lin2dBisec.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>

//=========================================================================
//   Creation of a circle tangent to three straight lines.                +
//   Create Bissectrices at Qualified1 and Qualified2 and                 +
//          Bissectrices at Qualified1 and Qualified3.                    +
//   Intersect bissectrices calculated in this way ==> Center points      +
//   Choose the center point that corresponds to qualifiers and           +
//   construct the solution of radius equal to the distance between the   +
//   chosen center point and straight line Qualified1.                               +
//=========================================================================
GccAna_Circ2d3Tan::
   GccAna_Circ2d3Tan (const GccEnt_QualifiedLin& Qualified1,
                      const GccEnt_QualifiedLin& Qualified2,
                      const GccEnt_QualifiedLin& Qualified3,
                      const Standard_Real
                     ):

//=========================================================================
//   Initialization of fields.                                           +
//=========================================================================

   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4) ,
   qualifier3(1,4) ,
   TheSame1(1,4)   ,
   TheSame2(1,4)   ,
   TheSame3(1,4)   ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   pnttg3sol(1,4)  ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   par3sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    ,
   pararg3(1,4)    
{

   TheSame1.Init(0);
   TheSame2.Init(0);
   TheSame3.Init(0);
   gp_Dir2d dirx(1.0,0.0);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() ||
	 Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
       !(Qualified2.IsEnclosed() ||
	 Qualified2.IsOutside() || Qualified2.IsUnqualified()) ||
       !(Qualified3.IsEnclosed() ||
	 Qualified3.IsOutside() || Qualified3.IsUnqualified())) {
     throw GccEnt_BadQualifier();
     return;
   }

//=========================================================================
//   Processing.                                                          +
//=========================================================================

   gp_Lin2d L1(Qualified1.Qualified());
   gp_Lin2d L2(Qualified2.Qualified());
   gp_Lin2d L3(Qualified3.Qualified());
   gp_Pnt2d origin1(L1.Location());
   gp_Dir2d dir1(L1.Direction());
   gp_Dir2d normL1(-dir1.Y(),dir1.X());
   gp_Pnt2d origin2(L2.Location());
   gp_Dir2d dir2(L2.Direction());
   gp_Dir2d normL2(-dir2.Y(),dir2.X());
   gp_Pnt2d origin3(L3.Location());
   gp_Dir2d dir3(L3.Direction());
   gp_Dir2d normL3(-dir3.Y(),dir3.X());
   Standard_Real xloc1 = origin1.X();
   Standard_Real xloc2 = origin2.X();
   Standard_Real xloc3 = origin3.X();
   Standard_Real yloc1 = origin1.Y();
   Standard_Real yloc2 = origin2.Y();
   Standard_Real yloc3 = origin3.Y();
   Standard_Real xdir1 = dir1.X();
   Standard_Real xdir2 = dir2.X();
   Standard_Real xdir3 = dir3.X();
   Standard_Real ydir1 = dir1.Y();
   Standard_Real ydir2 = dir2.Y();
   Standard_Real ydir3 = dir3.Y();
   GccAna_Lin2dBisec Bisec1(L1,L2);
   GccAna_Lin2dBisec Bisec2(L1,L3);
   Standard_Integer ncote1=0;
   Standard_Integer ncote2=0;
   Standard_Integer ncote3=0;
   TColStd_Array1OfReal cote1(1,2);
   TColStd_Array1OfReal cote2(1,2);
   TColStd_Array1OfReal cote3(1,2);
   Standard_Integer nbsol = 0;
   if (Bisec1.IsDone() && Bisec2.IsDone()) {
     for (Standard_Integer i = 1 ; i <= Bisec1.NbSolutions() ; i++) {
       for (Standard_Integer j = 1 ; j <= Bisec2.NbSolutions() ; j++) {
	 IntAna2d_AnaIntersection Intp(Bisec1.ThisSolution(i),
				       Bisec2.ThisSolution(j));
	 if (Intp.IsDone()) {
	   if (!Intp.IsEmpty()) {
	     for (Standard_Integer k = 1 ; k <= Intp.NbPoints() ; k++) {
	       nbsol++;
	       Standard_Real Radius = (L1.Distance(Intp.Point(k).Value())+
			      L2.Distance(Intp.Point(k).Value())+
			      L3.Distance(Intp.Point(k).Value()))/3.0;
	       gp_Pnt2d Center(Intp.Point(k).Value());
	       Standard_Real cx = Center.X();
	       Standard_Real cy = Center.Y();
	       cirsol(nbsol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//             ======================================================
	       gp_Dir2d dc1(origin1.XY()-Center.XY());
	       if (!Qualified1.IsUnqualified()) { 
		 qualifier1(nbsol) = Qualified1.Qualifier();
	       }
	       else if (dc1.Dot(normL1) > 0.0) {
		 qualifier1(nbsol) = GccEnt_outside;
	       }
	       else { qualifier1(nbsol) = GccEnt_enclosed; }
	       gp_Dir2d dc2(origin2.XY()-Center.XY());
	       if (!Qualified2.IsUnqualified()) { 
		 qualifier2(nbsol) = Qualified2.Qualifier();
	       }
	       else if (dc2.Dot(normL2) > 0.0) {
		 qualifier2(nbsol) = GccEnt_outside;
	       }
	       else { qualifier2(nbsol) = GccEnt_enclosed; }
	       gp_Dir2d dc3(origin3.XY()-Center.XY());
	       if (!Qualified3.IsUnqualified()) { 
		 qualifier3(nbsol) = Qualified3.Qualifier();
	       }
	       else if (dc3.Dot(normL3) > 0.0) {
		 qualifier3(nbsol) = GccEnt_outside;
	       }
	       else { qualifier3(nbsol) = GccEnt_enclosed; }
	       
	       Standard_Real cross1=gp_Dir2d(-ydir1,xdir1)
		                   .Dot(gp_Dir2d(xloc1-cx,yloc1-cy));
	       Standard_Real cross2=gp_Dir2d(-ydir2,xdir2)
				   .Dot(gp_Dir2d(xloc2-cx,yloc2-cy));
	       Standard_Real cross3=gp_Dir2d(-ydir3,xdir3)
				   .Dot(gp_Dir2d(xloc3-cx,yloc3-cy));
	       if (cross1 != 0.0) {
		 cross1 = cross1/Abs(cross1);
	       }
	       pnttg1sol(nbsol) = gp_Pnt2d(gp_XY(cx,cy)+
					   cross1*Radius*gp_XY(-ydir1,xdir1));
	       if (cross2 != 0.0) {
		 cross2 = cross2/Abs(cross2);
	       }
	       pnttg2sol(nbsol) = gp_Pnt2d(gp_XY(cx,cy)+
					   cross2*Radius*gp_XY(-ydir2,xdir2));
	       if (cross3 != 0.0) {
		 cross3 = cross3/Abs(cross3);
	       }
	       pnttg3sol(nbsol) = gp_Pnt2d(gp_XY(cx,cy)+
					   cross3*Radius*gp_XY(-ydir3,xdir3));
	       par1sol(nbsol)=ElCLib::Parameter(cirsol(nbsol),
						pnttg1sol(nbsol));
	       pararg1(nbsol)=ElCLib::Parameter(L1,pnttg1sol(nbsol));
	       par2sol(nbsol)=ElCLib::Parameter(cirsol(nbsol),
						pnttg2sol(nbsol));
	       pararg2(nbsol)=ElCLib::Parameter(L2,pnttg2sol(nbsol));
	       par3sol(nbsol)=ElCLib::Parameter(cirsol(nbsol),
						pnttg3sol(nbsol));
	       pararg3(nbsol)=ElCLib::Parameter(L3,pnttg3sol(nbsol));
	     }
	   }
	   WellDone = Standard_True;
	 }
       }
     }
   }
   if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed() && 
//  =========================================================
       Qualified3.IsEnclosed()) {
//     ========================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = 1.0;
     cote2(1) = 1.0;
     cote3(1) = 1.0;
   }
   else if (Qualified1.IsEnclosed() && Qualified2.IsEnclosed() && 
// ==============================================================
	    Qualified3.IsOutside()) {
//          =======================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = 1.0;
     cote2(1) = 1.0;
     cote3(1) = -1.0;
   }
   else if (Qualified1.IsEnclosed() && Qualified2.IsOutside() && 
// =============================================================
	    Qualified3.IsEnclosed()) {
//          ========================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = 1.0;
     cote2(1) = -1.0;
     cote3(1) = 1.0;
   }
   else if (Qualified1.IsEnclosed() && Qualified2.IsOutside() && 
// =============================================================
	    Qualified3.IsOutside()) {
//          =======================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = 1.0;
     cote2(1) = -1.0;
     cote3(1) = -1.0;
   }
   else if (Qualified1.IsOutside() && Qualified2.IsEnclosed() && 
// =============================================================
	    Qualified3.IsEnclosed()) {
//          ========================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = -1.0;
     cote2(1) = 1.0;
     cote3(1) = 1.0;
   }
   else if (Qualified1.IsOutside() && Qualified2.IsEnclosed() && 
// =============================================================
	    Qualified3.IsOutside()) {
//          =======================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = -1.0;
     cote2(1) = 1.0;
     cote3(1) = -1.0;
   }
   else if (Qualified1.IsOutside() && Qualified2.IsOutside() && 
// ============================================================
	    Qualified3.IsEnclosed()) {
//          ========================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = -1.0;
     cote2(1) = -1.0;
     cote3(1) = 1.0;
   }
   else if (Qualified1.IsOutside() && Qualified2.IsOutside() && 
// ============================================================
	    Qualified3.IsOutside()) {
//          =======================
     ncote1 = 1;
     ncote2 = 1;
     ncote3 = 1;
     cote1(1) = -1.0;
     cote2(1) = -1.0;
     cote3(1) = -1.0;
   }
   else {
     if (Qualified1.IsUnqualified()) {
//   ====================================
       ncote1 = 2;
       cote1(1) = 1.0;
       cote1(2) = -1.0;
       if (Qualified2.IsUnqualified()) {
//     ===============================
	 ncote2 = 2;
	 cote2(1) = 1.0;
	 cote2(2) = -1.0;
	 if (Qualified3.IsUnqualified()) {
//       ===============================
	   ncote3 = 2;
	   cote2(1) = 1.0;
	   cote2(2) = -1.0;
           NbrSol = nbsol;
           WellDone = Standard_True;
	 }
	 else if (Qualified3.IsEnclosed()) {
//       ===============================
	   ncote3 = 1;
	   cote3(1) = 1.0;
	 }
	 else if (Qualified3.IsOutside()) {
//       ================================
	   ncote3 = 1;
	   cote3(1) = -1.0;
	 }
       }
       else if (Qualified2.IsEnclosed()) {
//     =================================
	 ncote2 = 1;
	 cote2(1) = 1.0;
	 if (Qualified3.IsUnqualified()) {
//       ===============================
	   ncote3 = 2;
	   cote3(1) = 1.0;
	   cote3(1) = -1.0;
	 }
	 else if (Qualified3.IsEnclosed()) {
//       =================================
	   ncote3 = 1;
	   cote3(1) = 1.0;
	 }
	 else if (Qualified3.IsOutside()) {
//       ================================
	   ncote3 = 1;
	   cote3(1) = -1.0;
	 }
       }
       else if (Qualified2.IsOutside()) {
//     ================================
	 ncote2 = 1;
	 cote2(1) = -1.0;
	 if (Qualified3.IsUnqualified()) {
//       ===============================
	   ncote3 = 2;
	   cote3(1) = 1.0;
	   cote3(2) = -1.0;
	 }
	 else if (Qualified3.IsEnclosed()) {
//       =================================
	   ncote3 = 1;
	   cote3(1) = 1.0;
	 }
	 else if (Qualified3.IsOutside()) {
//       ================================
	   ncote3 = 1;
	   cote3(1) = -1.0;
	 }
       }
     }
     else if (Qualified2.IsUnqualified()) {
//   ===================================
       ncote2 = 2;
       cote2(1) = 1.0;
       cote2(2) = -1.0;
       if (Qualified1.IsEnclosed()) {
//     ============================
	 ncote1 = 1;
	 cote1(1) = 1.0;
	 if (Qualified3.IsUnqualified()) {
//       ===============================
	   ncote3 = 2;
	   cote3(1) = -1.0;
	   cote3(1) = -1.0;
	 }
	 else if (Qualified3.IsEnclosed()) {
//       =================================
	   ncote3 = 1;
	   cote3(1) = 1.0;
	 }
	 else if (Qualified3.IsOutside()) {
//       ================================
	   ncote3 = 1;
	   cote3(1) = -1.0;
	 }
       }
       else if (Qualified1.IsOutside()) {
//     ================================
	 ncote1 = 1;
	 cote1(1) = 1.0;
	 if (Qualified3.IsUnqualified()) {
//       ===============================
	   ncote3 = 2;
	   cote3(1) = 1.0;
	   cote3(2) = -1.0;
	 }
	 else if (Qualified3.IsEnclosed()) {
//       =================================
	   ncote3 = 1;
	   cote3(1) = 1.0;
	 }
	 else if (Qualified3.IsOutside()) {
//       ================================
	   ncote3 = 1;
	   cote3(1) = -1.0;
	 }
       }
     }
     else if (Qualified3.IsUnqualified()) {
//   ===================================
       ncote3 = 2;
       cote3(1) = 1.0;
       cote3(2) = -1.0;
       if (Qualified1.IsEnclosed()) {
//     ============================
	 ncote1 = 1;
	 cote1(1) = 1.0;
	 if (Qualified2.IsEnclosed()) {
//       ============================
	   ncote2 = 1;
	   cote2(1) = 1.0;
	 }
	 else if (Qualified2.IsOutside()) {
//       ===============================
	   ncote2 = 1;
	   cote2(1) = -1.0;
	 }
       }
       else if (Qualified1.IsOutside()) {
//     ================================
	 ncote1 = 1;
	 cote1(1) = -1.0;
	 if (Qualified2.IsEnclosed()) {
//       ============================
	   ncote2 = 1;
	   cote2(1) = 1.0;
	 }
	 else if (Qualified2.IsOutside()) {
//       ===============================
	   ncote2 = 1;
	   cote2(1) = -1.0;
	 }
       }
     }
   }
   if (NbrSol > 0) { return; }
   for (Standard_Integer i = 1 ; i <= nbsol ; i++) {
     for (Standard_Integer j1 = 1 ; j1 <= ncote1 ; j1++) {
       for (Standard_Integer j2 = 1 ; j2 <= ncote2 ; j2++) {
	 for (Standard_Integer j3 = 1 ; j3 <= ncote3 ; j3++) {
	   if ((cote2(j2)*((cirsol(i).Location().X()-origin2.X())*
	       (-dir2.Y())+(cirsol(i).Location().Y()-
	       origin2.Y())*(dir2.X())) > 0.0) &&
	       (cote3(j3)*((cirsol(i).Location().X()-origin3.X())*
	       (-dir3.Y())+(cirsol(i).Location().Y()-
	       origin3.Y())*(dir3.X())) > 0.0) &&
               (cote1(j1)*((cirsol(i).Location().X()-origin1.X())*
               (-dir1.Y())+(cirsol(i).Location().Y()-
               origin1.Y())*(dir1.X())) > 0.0)) {
	     NbrSol++;
	     cirsol(NbrSol) = gp_Circ2d(cirsol(i));
//           =====================================
	     Standard_Real Radius = cirsol(NbrSol).Radius();
	     gp_Pnt2d Center(cirsol(NbrSol).Location());
	     gp_Dir2d dc(origin1.XY()-Center.XY());
	     Standard_Real sign = dc.Dot(gp_Dir2d(-dir1.Y(),dir1.X()));
	     dc = gp_Dir2d(sign*gp_XY(-dir1.Y(),dir1.X()));
	     pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc.XY());
	     dc = gp_Dir2d(origin2.XY()-Center.XY());
	     sign = dc.Dot(gp_Dir2d(-dir2.Y(),dir2.X()));
	     dc = gp_Dir2d(sign*gp_XY(-dir2.Y(),dir2.X()));
	     pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc.XY());
	     dc = gp_Dir2d(origin3.XY()-Center.XY());
	     sign = dc.Dot(gp_Dir2d(-dir3.Y(),dir3.X()));
	     dc = gp_Dir2d(sign*gp_XY(-dir3.Y(),dir3.X()));
	     pnttg3sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc.XY());
	     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg1sol(NbrSol));
	     pararg1(NbrSol)=ElCLib::Parameter(L1,pnttg1sol(NbrSol));
	     par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg2sol(NbrSol));
	     pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
	     par3sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					      pnttg3sol(NbrSol));
	     pararg3(NbrSol)=ElCLib::Parameter(L3,pnttg3sol(NbrSol));
	   }
	 }
       }
     }
   }
 }
