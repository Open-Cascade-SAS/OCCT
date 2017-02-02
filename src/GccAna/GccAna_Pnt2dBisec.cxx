// Created on: 1991-10-04
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
//   CREATION of the BISSECTRICE between two POINTS.                         +
//=========================================================================

#include <GccAna_NoSolution.hxx>
#include <GccAna_Pnt2dBisec.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

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
   if (!WellDone) { throw StdFail_NotDone(); }
   else if (!HasSol) { throw GccAna_NoSolution(); }
   return linsol;
 }
