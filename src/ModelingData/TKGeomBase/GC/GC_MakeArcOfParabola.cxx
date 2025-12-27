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
#include <GC_MakeArcOfParabola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

GC_MakeArcOfParabola::GC_MakeArcOfParabola(const gp_Parab&        Parab,
                                           const gp_Pnt&          P1,
                                           const gp_Pnt&          P2,
                                           const bool Sense)
{
  double         Alpha1 = ElCLib::Parameter(Parab, P1);
  double         Alpha2 = ElCLib::Parameter(Parab, P2);
  occ::handle<Geom_Parabola> P      = new Geom_Parabola(Parab);
  TheArc                       = new Geom_TrimmedCurve(P, Alpha1, Alpha2, Sense);
  TheError                     = gce_Done;
}

GC_MakeArcOfParabola::GC_MakeArcOfParabola(const gp_Parab&        Parab,
                                           const gp_Pnt&          P,
                                           const double    Alpha,
                                           const bool Sense)
{
  double         Alphafirst = ElCLib::Parameter(Parab, P);
  occ::handle<Geom_Parabola> Parabola   = new Geom_Parabola(Parab);
  TheArc                           = new Geom_TrimmedCurve(Parabola, Alphafirst, Alpha, Sense);
  TheError                         = gce_Done;
}

GC_MakeArcOfParabola::GC_MakeArcOfParabola(const gp_Parab&        Parab,
                                           const double    Alpha1,
                                           const double    Alpha2,
                                           const bool Sense)
{
  occ::handle<Geom_Parabola> P = new Geom_Parabola(Parab);
  TheArc                  = new Geom_TrimmedCurve(P, Alpha1, Alpha2, Sense);
  TheError                = gce_Done;
}

const occ::handle<Geom_TrimmedCurve>& GC_MakeArcOfParabola::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "GC_MakeArcOfParabola::Value() - no result");
  return TheArc;
}
