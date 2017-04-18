// Created on: 1992-10-02
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


#include <GC_MakePlane.hxx>
#include <gce_MakePln.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>

GC_MakePlane::GC_MakePlane(const gp_Pln& Pl)
{
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pl);
}

GC_MakePlane::GC_MakePlane(const gp_Pnt& P,
			     const gp_Dir& V)
{
  TheError = gce_Done;
  ThePlane = new Geom_Plane(P,V);
}

GC_MakePlane::GC_MakePlane(const Standard_Real A,
			     const Standard_Real B,
			     const Standard_Real C,
			     const Standard_Real D)
{
  if (Sqrt(A*A + B*B +C*C) <= gp::Resolution()) {
    TheError = gce_BadEquation; 
  }
  else {
    TheError = gce_Done;
    ThePlane = new Geom_Plane(gp_Pln(A,B,C,D));
  }
}

//=========================================================================
//   Creation d un Geom_Plane passant par trois points.                   +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Pnt& P1    ,
			     const gp_Pnt& P2    ,
			     const gp_Pnt& P3    ) {
  gce_MakePln Pl(P1,P2,P3);
  TheError = Pl.Status();
  if (TheError == gce_Done) {
    ThePlane = new Geom_Plane(Pl.Value());
  }
}

//=========================================================================
//   Creation d un Geom_Plane parallele a un pln a une distance donnee.   +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Pln& Pl   ,
			     const Standard_Real    Dist ) {
  gp_Pln Pln = gce_MakePln(Pl,Dist);
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pln);
}

//=========================================================================
//   Creation d un Geom_Plane parallele a un pln passant par un point     +
//   <Point1>.                                                            +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Pln& Pl    ,
			     const gp_Pnt& Point ) {
  gp_Pln Pln= gce_MakePln(Pl,Point);
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pln);
}

//=========================================================================
//  Creation d un Geom_Plane a partir d un Ax1 (Point + Normale).         +
//=========================================================================

GC_MakePlane::GC_MakePlane(const gp_Ax1& Axis ) {
  gp_Pln Pln = gce_MakePln(Axis);
  TheError = gce_Done;
  ThePlane = new Geom_Plane(Pln);
}
     
//=========================================================================
//  Creation d un Geom_Plane par un tableau de points.                    +
//=========================================================================

/*GC_MakePlane::GC_MakePlane(const TColgp_Array1OfPnt&    Pts     ,
	 		           Standard_Real            ErrMax  ,
			           Standard_Real            ErrMean ) {
  GC_MakePln Pln(Pts,ErrMax,ErrMean);
  TheError = Pln.Status();
  if (TheError == GC_Done) {
    ThePlane = new Geom_Plane(Pln.Value());
  }
}
*/

const Handle(Geom_Plane)& GC_MakePlane::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakePlane::Value() - no result");
  return ThePlane;
}
