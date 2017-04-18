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


#include <GC_MakeCylindricalSurface.hxx>
#include <GC_MakeTrimmedCylinder.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a cylinder limited by three points <P1>, <P2> and <P3>.         +
//   the height og the resulting cylinder is the distance from <P1> to <P2>.     +
//   The radius is the distance from <P3> to axis <P1P2>.                 +
//=========================================================================
GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Pnt& P1 ,
						 const gp_Pnt& P2 ,
						 const gp_Pnt& P3 ) 
{
  GC_MakeCylindricalSurface Cyl(P1,P2,P3);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCyl = new Geom_RectangularTrimmedSurface(Cyl.Value(),0.,2.*M_PI,0.,
				  P2.Distance(P1),Standard_True,Standard_True);
  }
}

//=========================================================================
//   Creation of a cylinder limited by a circle and height.          +
//=========================================================================

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Circ&      Circ   ,
						 const Standard_Real Height ) {
  GC_MakeCylindricalSurface Cyl(Circ);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCyl = new Geom_RectangularTrimmedSurface(Cyl.Value(),0.,2.*M_PI,0.,
					   Height,Standard_True,Standard_True);
  }
}
     
//=========================================================================
//=========================================================================

GC_MakeTrimmedCylinder::GC_MakeTrimmedCylinder(const gp_Ax1&       A1     ,
						 const Standard_Real Radius ,
						 const Standard_Real Height ) {
  GC_MakeCylindricalSurface Cyl(A1,Radius);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCyl = new Geom_RectangularTrimmedSurface(Cyl.Value(),0.,2.*M_PI,0.,
					Height,Standard_True,Standard_True);
  }
}

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCylinder::
       Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeTrimmedCylinder::Value() - no result");
  return TheCyl;
}
