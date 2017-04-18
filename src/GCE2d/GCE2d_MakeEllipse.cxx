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


#include <GCE2d_MakeEllipse.hxx>
#include <gce_MakeElips2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Elips2d& E)
{
  TheError = gce_Done;
  TheEllipse = new Geom2d_Ellipse(E);
}

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Ax22d&     Axis        ,
				     const Standard_Real MajorRadius ,
				     const Standard_Real MinorRadius )
{
  gce_MakeElips2d E = gce_MakeElips2d(Axis,MajorRadius,MinorRadius);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Ax2d&         MajorAxis   ,
				     const Standard_Real    MajorRadius ,
				     const Standard_Real    MinorRadius ,
				     const Standard_Boolean Sense       )
{
  gce_MakeElips2d E = gce_MakeElips2d(MajorAxis,MajorRadius,MinorRadius,Sense);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

GCE2d_MakeEllipse::GCE2d_MakeEllipse(const gp_Pnt2d& S1     ,
				     const gp_Pnt2d& S2     ,
				     const gp_Pnt2d& Center ) {
  gce_MakeElips2d E = gce_MakeElips2d(S1,S2,Center);
  TheError = E.Status();
  if (TheError == gce_Done) {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

const Handle(Geom2d_Ellipse)& GCE2d_MakeEllipse::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "GCE2d_MakeEllipse::Value() - no result");
  return TheEllipse;
}
