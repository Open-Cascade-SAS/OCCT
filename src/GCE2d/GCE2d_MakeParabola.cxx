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


#include <GCE2d_MakeParabola.hxx>
#include <gce_MakeParab2d.hxx>
#include <Geom2d_Parabola.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Parab2d& Prb)
{
  TheError = gce_Done;
  TheParabola = new Geom2d_Parabola(Prb);
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax2d&         MirrorAxis,
				       const Standard_Real    Focal     ,
				       const Standard_Boolean Sense     )
{
  if (Focal <0.0) { TheError = gce_NullFocusLength; }
  else {
    TheError = gce_Done;
    TheParabola = new Geom2d_Parabola(MirrorAxis,Focal,Sense);
  }
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax22d&     Axis ,
				       const Standard_Real Focal)
{
  if (Focal <0.0) { TheError = gce_NullFocusLength; }
  else {
    TheError = gce_Done;
    TheParabola = new Geom2d_Parabola(Axis,Focal);
  }
}

GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Ax2d&         D     ,
				       const gp_Pnt2d&        F     ,
				       const Standard_Boolean Sense )
{
  TheError = gce_Done;
  gp_Parab2d para(D,F,Sense);
  TheParabola = new Geom2d_Parabola(para);
}


GCE2d_MakeParabola::GCE2d_MakeParabola(const gp_Pnt2d& S1 ,
				       const gp_Pnt2d& O  ) {
  gce_MakeParab2d P = gce_MakeParab2d(S1,O);
  TheError = P.Status();
  if (TheError == gce_Done) {
    TheParabola = new Geom2d_Parabola(P.Value());
  }
}

const Handle(Geom2d_Parabola)& GCE2d_MakeParabola::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GCE2d_MakeParabola::Value() - no result");
  return TheParabola;
}
