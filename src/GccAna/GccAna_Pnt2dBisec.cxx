// Created on: 1991-10-04
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
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
//   CREATION of the BISSECTRICE between two POINTS.                         +
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
