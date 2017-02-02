// Created on: 1991-10-11
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
//   CREATION of the BISSECTICE between a CIRCLE and a POINT.               +
//=========================================================================

#include <GccAna_CircPnt2dBisec.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BElips.hxx>
#include <GccInt_BHyper.hxx>
#include <GccInt_Bisec.hxx>
#include <GccInt_BLine.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
GccAna_CircPnt2dBisec::
   GccAna_CircPnt2dBisec (const gp_Circ2d& Circle ,
		          const gp_Pnt2d&  Point  )
{
  circle = Circle;
  point = Point;
  myTolerance = 1.e-10;
  DefineSolutions();
}

GccAna_CircPnt2dBisec::
   GccAna_CircPnt2dBisec (const gp_Circ2d& Circle ,
		          const gp_Pnt2d&  Point,
                          const Standard_Real Tolerance)
{
  circle = Circle;
  point = Point;
  myTolerance = 1.e-10;
  if (myTolerance < Tolerance)
    myTolerance = Tolerance;
  
  DefineSolutions();
}

void GccAna_CircPnt2dBisec::DefineSolutions()
{
  Standard_Real dist = circle.Radius() - point.Distance(circle.Location());
  
  if (Abs(dist) < myTolerance)
  {
    theposition = 0;
    NbrSol = 1;
  }
  else if (dist > 0.0)
  {
    theposition = -1;
    NbrSol = 1;
  }
  else {
    theposition = 1;
    NbrSol = 2;
  }
  
  WellDone = Standard_True;
}

//=========================================================================
//  Processing.                                                           +
//  Return the coordinates of origins of the straight line (xloc,yloc) and+
//  of the circle (xcencirc, ycencirc).                                       +
//  Also return the coordinates of the direction of the straight line (xdir,   +
//  ydir) and the radius of circle R1.                                       +
//  Check at which side of the straight line is found the center of circle    +
//  to orientate the parabola (sign).                                    +
//  Create axis of each parabola (axeparab1, axeparb2), then    +
//  two parabolas (biscirPnt1, biscirPnt1).                          +
//=========================================================================

Handle(GccInt_Bisec) GccAna_CircPnt2dBisec::
   ThisSolution (const Standard_Integer Index) const 
{
  
  if (!WellDone)
    throw StdFail_NotDone();
  
  if ((Index <=0) || (Index > NbrSol))
    throw Standard_OutOfRange();

  Handle(GccInt_Bisec) bissol;  
  Standard_Real xpoint = point.X();
  Standard_Real ypoint = point.Y();
  Standard_Real xcencir = circle.Location().X();
  Standard_Real ycencir = circle.Location().Y();
  Standard_Real R1 = circle.Radius();
  Standard_Real dist = point.Distance(circle.Location());

  if (dist < myTolerance)
    {
      gp_Circ2d biscirpnt1(gp_Ax2d(point,gp_Dir2d(1.0,0.0)),R1/2.);
      bissol = new GccInt_BCirc(biscirpnt1);
      //       ==========================================================
    }
  else {
    gp_Pnt2d center((xpoint+xcencir)/2.,(ypoint+ycencir)/2.);
    gp_Ax2d majax(center,gp_Dir2d(xpoint-xcencir,ypoint-ycencir));
    
    //=========================================================================
    //   The point is inside the circle.                                +
    //=========================================================================
    
    if (theposition == -1) {
      gp_Elips2d biscirpnt(majax,R1/2.,Sqrt(R1*R1-dist*dist)/2.);
      bissol = new GccInt_BElips(biscirpnt);
      //         ===========================================================
    }
    
    //=========================================================================
    //   The point is on the circle.                                          +
    //   There is only one solution : straight line passing through point and the center +
    //   of the circle.                                                           +
    //=========================================================================
    
    else if (theposition == 0) {
      gp_Dir2d dirsol;
      if (circle.IsDirect()) 
	dirsol=gp_Dir2d(xcencir-xpoint,ycencir-ypoint);
      else dirsol = gp_Dir2d(xpoint-xcencir,ypoint-ycencir);
      gp_Lin2d biscirpnt(point,dirsol);
      bissol = new GccInt_BLine(biscirpnt);
      //         =========================================================
    }
    
    //=========================================================================
    //   The point is outside of the circle.                                +
    //   There are two solutions : two main branches of the hyperbola.+
    //=========================================================================
    
    else {
      //	   Standard_Real d1 = sqrt(dist*R1-R1*R1);
      Standard_Real d1 = sqrt(dist*dist-R1*R1)/2.0;
      Standard_Real d2 = R1/2.;
      if (Index == 1) {
	gp_Hypr2d biscirpnt1(majax,d2,d1);
	bissol = new GccInt_BHyper(biscirpnt1);
	//           =========================================
      }
      else {
	gp_Hypr2d biscirpnt1(majax,d2,d1);
	gp_Hypr2d biscirpnt2 = biscirpnt1.OtherBranch();
	bissol = new GccInt_BHyper(biscirpnt2);
	//           =========================================
      }
    }
  }
  return bissol;
}


//=========================================================================

Standard_Boolean GccAna_CircPnt2dBisec::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_CircPnt2dBisec::
   NbSolutions () const { return NbrSol; }


