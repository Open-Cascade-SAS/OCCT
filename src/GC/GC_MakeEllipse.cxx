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


#include <GC_MakeEllipse.hxx>
#include <gce_MakeElips.hxx>
#include <Geom_Ellipse.hxx>
#include <gp_Ax2.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeEllipse::GC_MakeEllipse(const gp_Elips& E)
{
  TheError = gce_Done;
  TheEllipse = new Geom_Ellipse(E);
}

GC_MakeEllipse::GC_MakeEllipse(const gp_Ax2&       A2         ,
				 const Standard_Real MajorRadius,
				 const Standard_Real MinorRadius)
{
  if ( MinorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else if ( MajorRadius < MinorRadius) { TheError = gce_InvertAxis; }
  else {
    TheError = gce_Done;
    TheEllipse = new Geom_Ellipse(gp_Elips(A2,MajorRadius,MinorRadius));
  }
}

GC_MakeEllipse::GC_MakeEllipse(const gp_Pnt& S1     ,
				 const gp_Pnt& S2     ,
				 const gp_Pnt& Center ) {
  gce_MakeElips E = gce_MakeElips(S1,S2,Center);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom_Ellipse(E.Value());
  }
}

const Handle(Geom_Ellipse)& GC_MakeEllipse::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeEllipse::Value() - no result");
  return TheEllipse;
}
