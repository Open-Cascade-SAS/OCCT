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


#include <GC_MakeHyperbola.hxx>
#include <gce_MakeHypr.hxx>
#include <Geom_Hyperbola.hxx>
#include <gp_Ax2.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Hypr& H)
{
  TheError = gce_Done;
  TheHyperbola = new Geom_Hyperbola(H);
}

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Ax2&       A2         ,
				     const Standard_Real MajorRadius,
				     const Standard_Real MinorRadius)
{
  if (MajorRadius < 0. || MinorRadius < 0.0) { TheError = gce_NegativeRadius; }
  else {
    TheError = gce_Done;
    TheHyperbola = new Geom_Hyperbola(gp_Hypr(A2,MajorRadius,MinorRadius));
  }
}

GC_MakeHyperbola::GC_MakeHyperbola(const gp_Pnt& S1     ,
				     const gp_Pnt& S2     ,
				     const gp_Pnt& Center ) {
  gce_MakeHypr H = gce_MakeHypr(S1,S2,Center);
  TheError = H.Status();
  if (TheError == gce_Done) {
    TheHyperbola = new Geom_Hyperbola(H.Value());
  }
}

const Handle(Geom_Hyperbola)& GC_MakeHyperbola::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GC_MakeHyperbola::Value() - no result");
  return TheHyperbola;
}
