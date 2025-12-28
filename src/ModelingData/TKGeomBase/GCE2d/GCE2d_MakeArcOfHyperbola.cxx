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
#include <GCE2d_MakeArcOfHyperbola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

GCE2d_MakeArcOfHyperbola::GCE2d_MakeArcOfHyperbola(const gp_Hypr2d&       Hypr,
                                                   const gp_Pnt2d&        P1,
                                                   const gp_Pnt2d&        P2,
                                                   const bool Sense)
{
  double            Alpha1 = ElCLib::Parameter(Hypr, P1);
  double            Alpha2 = ElCLib::Parameter(Hypr, P2);
  occ::handle<Geom2d_Hyperbola> H      = new Geom2d_Hyperbola(Hypr);
  TheArc                          = new Geom2d_TrimmedCurve(H, Alpha1, Alpha2, Sense);
  TheError                        = gce_Done;
}

GCE2d_MakeArcOfHyperbola::GCE2d_MakeArcOfHyperbola(const gp_Hypr2d&       Hypr,
                                                   const gp_Pnt2d&        P,
                                                   const double    Alpha,
                                                   const bool Sense)
{
  double            Alphafirst = ElCLib::Parameter(Hypr, P);
  occ::handle<Geom2d_Hyperbola> H          = new Geom2d_Hyperbola(Hypr);
  TheArc                              = new Geom2d_TrimmedCurve(H, Alphafirst, Alpha, Sense);
  TheError                            = gce_Done;
}

GCE2d_MakeArcOfHyperbola::GCE2d_MakeArcOfHyperbola(const gp_Hypr2d&       Hypr,
                                                   const double    Alpha1,
                                                   const double    Alpha2,
                                                   const bool Sense)
{
  occ::handle<Geom2d_Hyperbola> H = new Geom2d_Hyperbola(Hypr);
  TheArc                     = new Geom2d_TrimmedCurve(H, Alpha1, Alpha2, Sense);
  TheError                   = gce_Done;
}

const occ::handle<Geom2d_TrimmedCurve>& GCE2d_MakeArcOfHyperbola::Value() const
{
  StdFail_NotDone_Raise_if(TheError != gce_Done, "GCE2d_MakeArcOfHyperbola::Value() - no result");
  return TheArc;
}
