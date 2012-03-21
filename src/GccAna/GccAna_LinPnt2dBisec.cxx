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
//   CREATION of the BISSECTRICE between a DROITE and POINTS.             +
//=========================================================================

#include <GccAna_LinPnt2dBisec.ixx>

#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <GccInt_BParab.hxx>
#include <GccInt_BLine.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

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
    StdFail_NotDone::Raise();

  return bissol;
}
