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

#include <GC_MakeLine2d.hxx>
#include <gce_MakeLin2d.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

GC_MakeLine2d::GC_MakeLine2d(const gp_Ax2d& A)
{
  TheError = gce_Done;
  TheLine  = new Geom2d_Line(A);
}

//=================================================================================================

GC_MakeLine2d::GC_MakeLine2d(const gp_Lin2d& L)
{
  TheError = gce_Done;
  TheLine  = new Geom2d_Line(L);
}

//=================================================================================================

GC_MakeLine2d::GC_MakeLine2d(const gp_Pnt2d& P, const gp_Dir2d& V)
{
  TheError = gce_Done;
  TheLine  = new Geom2d_Line(P, V);
}

//=================================================================================================

GC_MakeLine2d::GC_MakeLine2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2)
{
  gce_MakeLin2d L(P1, P2);
  TheError = L.Status();
  if (TheError == gce_Done)
  {
    TheLine = new Geom2d_Line(L.Value());
  }
}

//=================================================================================================

GC_MakeLine2d::GC_MakeLine2d(const gp_Lin2d& Lin, const gp_Pnt2d& Point)
{
  gce_MakeLin2d L(Lin, Point);
  TheError = L.Status();
  if (TheError == gce_Done)
  {
    TheLine = new Geom2d_Line(L.Value());
  }
}

//=================================================================================================

GC_MakeLine2d::GC_MakeLine2d(const gp_Lin2d& Lin, const double Dist)
{
  gce_MakeLin2d L(Lin, Dist);
  TheError = L.Status();
  if (TheError == gce_Done)
  {
    TheLine = new Geom2d_Line(L.Value());
  }
}

//=================================================================================================

const occ::handle<Geom2d_Line>& GC_MakeLine2d::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "GC_MakeLine2d::Value() - no result");
  return TheLine;
}
