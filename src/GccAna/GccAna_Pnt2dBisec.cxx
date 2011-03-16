// File:	GccAna_Pnt2dBisec.cxx
// Created:	Fri Oct  4 09:53:31 1991
// Author:	Remi GILET
//		<reg@phobox>


//=========================================================================
//   CREATION DE LA BISSECTRICE ENTRE DEUX POINTS.                         +
//=========================================================================

#include <GccAna_Pnt2dBisec.ixx>

#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <GccAna_NoSolution.hxx>
#include <gp.hxx>

//=========================================================================

GccAna_Pnt2dBisec::
   GccAna_Pnt2dBisec (const gp_Pnt2d& Point1,
		      const gp_Pnt2d& Point2) {

   WellDone = Standard_False;
//   if (Point1.Distance(Point2) > gp::Resolution()) {
   if (Point1.Distance(Point2) > 1.e-10) {
     gp_Dir2d dir1(Point2.XY()-Point1.XY());
     linsol = gp_Lin2d(gp_Pnt2d((Point2.X()+Point1.X())/2.,
//   ======================================================
				   (Point2.Y()+Point1.Y())/2.),
//                                 ============================
			  gp_Dir2d(-dir1.Y(),dir1.X()));
//                        =============================
     HasSol = Standard_True;
     WellDone = Standard_True;
   }
   else { 
     HasSol = Standard_False;
     WellDone = Standard_True;
   }
 }


//=========================================================================

Standard_Boolean GccAna_Pnt2dBisec::
   IsDone () const { return WellDone; }

Standard_Boolean GccAna_Pnt2dBisec::
   HasSolution () const { return HasSol; }

gp_Lin2d GccAna_Pnt2dBisec::
   ThisSolution () const {
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (!HasSol) { GccAna_NoSolution::Raise(); }
   return linsol;
 }
