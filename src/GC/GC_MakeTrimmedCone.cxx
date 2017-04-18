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


#include <Extrema_ExtPElC.hxx>
#include <GC_MakeConicalSurface.hxx>
#include <GC_MakeTrimmedCone.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation of a cone by four points.                                +
//   First two give the axis.                                     +
//   The third gives the base radius.                              +
//   the third and the fourth demi-angle.                          +
//=========================================================================
GC_MakeTrimmedCone::GC_MakeTrimmedCone(const gp_Pnt& P1 ,
					 const gp_Pnt& P2 ,
					 const gp_Pnt& P3 ,
					 const gp_Pnt& P4 ) 
{
  GC_MakeConicalSurface Cone(P1,P2,P3,P4);
  TheError = Cone.Status();
  if (TheError == gce_Done) {
    gp_Dir D1(P2.XYZ()-P1.XYZ());
    gp_Lin L1(P1,D1);
    Extrema_ExtPElC ext1(P3,L1,1.0e-7,-2.0e+100,+2.0e+100);
    Extrema_ExtPElC ext2(P4,L1,1.0e-7,-2.0e+100,+2.0e+100);
    gp_Pnt P5 = ext1.Point(1).Value();
    gp_Pnt P6 = ext2.Point(1).Value();
    Standard_Real D = P6.Distance(P5)/cos((Cone.Value())->SemiAngle());
    TheCone=new Geom_RectangularTrimmedSurface(Cone.Value(),0.,2.*M_PI,0.,D,Standard_True,Standard_True);
  }
}

//=========================================================================
//=========================================================================

GC_MakeTrimmedCone::GC_MakeTrimmedCone(const gp_Pnt&       P1 ,
					 const gp_Pnt&       P2 ,
					 const Standard_Real R1 ,
					 const Standard_Real R2 ) 
{
  GC_MakeConicalSurface Cone(P1,P2,R1,R2);
  TheError = Cone.Status();
  if (TheError == gce_Done) {
    Standard_Real D = (P2.Distance(P1))/cos((Cone.Value())->SemiAngle());
    TheCone=new Geom_RectangularTrimmedSurface(Cone.Value(),0.,2.*M_PI,0.,D,Standard_True,Standard_True);
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

const Handle(Geom_RectangularTrimmedSurface)& GC_MakeTrimmedCone::
       Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeTrimmedCone::Value() - no result");
  return TheCone;
}
