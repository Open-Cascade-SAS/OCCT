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

#include <ElCLib.hxx>
#include <GC_MakeLine2d.hxx>
#include <GC_MakeSegment2d.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

#include <cmath>

//=================================================================================================

GC_MakeSegment2d::GC_MakeSegment2d(const gp_Pnt2d& P1, const gp_Dir2d& V, const gp_Pnt2d& P2)
{
  gp_Lin2d Line(P1, V);
  double   Ulast = ElCLib::Parameter(Line, P2);
  if (std::abs(Ulast) > gp::Resolution())
  {
    occ::handle<Geom2d_Line> L = new Geom2d_Line(Line);
    TheSegment                 = new Geom2d_TrimmedCurve(L, 0.0, Ulast, true);
    TheError                   = gce_Done;
  }
  else
  {
    TheError = gce_ConfusedPoints;
  }
}

//=================================================================================================

GC_MakeSegment2d::GC_MakeSegment2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2)
{
  double dist = P1.Distance(P2);
  if (dist > gp::Resolution())
  {
    occ::handle<Geom2d_Line> L = GC_MakeLine2d(P1, P2);
    TheSegment                 = new Geom2d_TrimmedCurve(L, 0., dist, true);
    TheError                   = gce_Done;
  }
  else
  {
    TheError = gce_ConfusedPoints;
  }
}

//=================================================================================================

GC_MakeSegment2d::GC_MakeSegment2d(const gp_Lin2d& Line, const gp_Pnt2d& Point, const double U)
{
  double                   Ufirst = ElCLib::Parameter(Line, Point);
  occ::handle<Geom2d_Line> L      = new Geom2d_Line(Line);
  TheSegment                      = new Geom2d_TrimmedCurve(L, Ufirst, U, true);
  TheError                        = gce_Done;
}

//=================================================================================================

GC_MakeSegment2d::GC_MakeSegment2d(const gp_Lin2d& Line, const gp_Pnt2d& P1, const gp_Pnt2d& P2)
{
  double                   Ufirst = ElCLib::Parameter(Line, P1);
  double                   Ulast  = ElCLib::Parameter(Line, P2);
  occ::handle<Geom2d_Line> L      = new Geom2d_Line(Line);
  TheSegment                      = new Geom2d_TrimmedCurve(L, Ufirst, Ulast, true);
  TheError                        = gce_Done;
}

//=================================================================================================

GC_MakeSegment2d::GC_MakeSegment2d(const gp_Lin2d& Line, const double U1, const double U2)
{
  occ::handle<Geom2d_Line> L = new Geom2d_Line(Line);
  TheSegment                 = new Geom2d_TrimmedCurve(L, U1, U2, true);
  TheError                   = gce_Done;
}

//=================================================================================================

const occ::handle<Geom2d_TrimmedCurve>& GC_MakeSegment2d::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "GC_MakeSegment2d::Value() - no result");
  return TheSegment;
}
