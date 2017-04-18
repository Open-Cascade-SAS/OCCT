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
#include <gce_MakeCylinder.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Cylinder& C)
{
  TheError = gce_Done;
  TheCylinder = new Geom_CylindricalSurface(C);
}

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Ax2& A2    ,
						const Standard_Real  Radius)
{
  if (Radius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheCylinder = new Geom_CylindricalSurface(A2,Radius);
  }
}

//=========================================================================
//   Construction of a cylinder by axis <A1> et radius <Radius>.           +
//=========================================================================

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Ax1& A1     ,
						 const Standard_Real Radius ) 
{
  gce_MakeCylinder Cyl = gce_MakeCylinder(A1,Radius);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCylinder=new Geom_CylindricalSurface(Cyl.Value());
  }
}

//=========================================================================
//   Construction of a cylinder by a circle <Cir>.                      +
//=========================================================================

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Circ& Circ ) {
  gp_Cylinder Cyl = gce_MakeCylinder(Circ);
  TheCylinder=new Geom_CylindricalSurface(Cyl);
  TheError = gce_Done;
}

//=========================================================================
//   Construction of a cylinder by tree points <P1>, <P2>, <P3>.         +
//   Two first points define the axis.                                   +
//   The third gives the radius.                                         +
//=========================================================================

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Pnt& P1 ,
						       const gp_Pnt& P2 ,
						       const gp_Pnt& P3 ) {
  gce_MakeCylinder Cyl = gce_MakeCylinder(P1,P2,P3);
  TheError = Cyl.Status();
  if (TheError == gce_Done) {
    TheCylinder=new Geom_CylindricalSurface(Cyl.Value());
  }
}

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Cylinder& Cyl ,
						     const Standard_Real  Dist)
{
  TheError = gce_Done;
  Standard_Real R = Abs(Cyl.Radius()-Dist);
  TheCylinder = new Geom_CylindricalSurface(Cyl);
  TheCylinder->SetRadius(R);
}

GC_MakeCylindricalSurface::GC_MakeCylindricalSurface(const gp_Cylinder& Cyl ,
						       const gp_Pnt&     Point)
{
  TheError = gce_Done;
  gp_Cylinder C(Cyl);
  gp_Lin L(C.Axis());
  Standard_Real R = L.Distance(Point);
  C.SetRadius(R);
  TheCylinder = new Geom_CylindricalSurface(C);
}

const Handle(Geom_CylindricalSurface)& 
       GC_MakeCylindricalSurface::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeCylindricalSurface::Value() - no result");
  return TheCylinder;
}
