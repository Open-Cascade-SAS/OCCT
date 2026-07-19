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

#include <GC_MakeEllipse2d.hxx>
#include <gce_MakeElips2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

GC_MakeEllipse2d::GC_MakeEllipse2d(const gp_Elips2d& E)
{
  TheError   = gce_Done;
  TheEllipse = new Geom2d_Ellipse(E);
}

//=================================================================================================

GC_MakeEllipse2d::GC_MakeEllipse2d(const gp_Ax22d& Axis,
                                   const double    MajorRadius,
                                   const double    MinorRadius)
{
  gce_MakeElips2d E = gce_MakeElips2d(Axis, MajorRadius, MinorRadius);
  TheError          = E.Status();
  if (TheError == gce_Done)
  {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

//=================================================================================================

GC_MakeEllipse2d::GC_MakeEllipse2d(const gp_Ax2d& MajorAxis,
                                   const double   MajorRadius,
                                   const double   MinorRadius,
                                   const bool     Sense)
{
  gce_MakeElips2d E = gce_MakeElips2d(MajorAxis, MajorRadius, MinorRadius, Sense);
  TheError          = E.Status();
  if (TheError == gce_Done)
  {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

//=================================================================================================

GC_MakeEllipse2d::GC_MakeEllipse2d(const gp_Pnt2d& S1, const gp_Pnt2d& S2, const gp_Pnt2d& Center)
{
  gce_MakeElips2d E = gce_MakeElips2d(S1, S2, Center);
  TheError          = E.Status();
  if (TheError == gce_Done)
  {
    TheEllipse = new Geom2d_Ellipse(E.Value());
  }
}

//=================================================================================================

const occ::handle<Geom2d_Ellipse>& GC_MakeEllipse2d::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "GC_MakeEllipse2d::Value() - no result");
  return TheEllipse;
}
