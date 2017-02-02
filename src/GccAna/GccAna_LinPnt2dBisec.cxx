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
//   CREATION of the BISSECTRICE between a DROITE and POINTS.             +
//=========================================================================

#include <GccAna_LinPnt2dBisec.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_BParab.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
GccAna_LinPnt2dBisec::
   GccAna_LinPnt2dBisec (const gp_Lin2d& Line1 ,
		         const gp_Pnt2d& Point2) {

   WellDone = Standard_False;

   Standard_Real xdir = Line1.Direction().X();
   Standard_Real ydir = Line1.Direction().Y();
   Standard_Real xloc = Line1.Location().X();
   Standard_Real yloc = Line1.Location().Y();
   Standard_Real dist = Line1.Distance(Point2);
//   if ( dist > gp::Resolution()) {
   if ( dist > 1.e-10)
     {
       Standard_Real xpoint2 = Point2.X();
       Standard_Real ypoint2 = Point2.Y();
       if ((-ydir*(xpoint2-xloc)+xdir*(ypoint2-yloc)) > 0.0)
	 {
	   gp_Ax2d axeparab(gp_Pnt2d(Point2.XY()-dist/2.*gp_XY(-ydir,xdir)),
			    gp_Dir2d(-ydir,xdir));
	   gp_Parab2d bislinpnt(axeparab,dist/2.0);
	   bissol = new GccInt_BParab(bislinpnt);
//         =====================================
	 }
       else
	 {
	   gp_Ax2d axeparab(gp_Pnt2d(Point2.XY()+dist/2.*gp_XY(-ydir,xdir)),
			    gp_Dir2d(ydir,-xdir));
	   gp_Parab2d bislinpnt(axeparab,dist/2.0);
	   bissol = new GccInt_BParab(bislinpnt);
//         =====================================
	 }
       WellDone = Standard_True;
     }
   else
     {
       gp_Lin2d bislinpnt(Point2,gp_Dir2d(-ydir,xdir));
       bissol = new GccInt_BLine(bislinpnt);
//     ====================================
       WellDone = Standard_True;
     }
 }

//=========================================================================

Standard_Boolean GccAna_LinPnt2dBisec::
   IsDone () const { return WellDone; }

Handle(GccInt_Bisec) GccAna_LinPnt2dBisec::
   ThisSolution () const 
{
  if (!WellDone)
    throw StdFail_NotDone();

  return bissol;
}
