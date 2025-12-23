// Created on: 1992-10-14
// Created by: Christophe MARION
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

#include <HLRBRep_TheQuadCurvExactInterCSurf.hxx>

#include <gp_Lin.hxx>
#include <HLRBRep_LineTool.hxx>
#include <HLRBRep_Surface.hxx>
#include <HLRBRep_SurfaceTool.hxx>
#include <HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf.hxx>

#include "../../TKGeomAlgo/IntCurveSurface/IntCurveSurface_QuadricCurveExactInterUtils.pxx"

//==================================================================================================

HLRBRep_TheQuadCurvExactInterCSurf::HLRBRep_TheQuadCurvExactInterCSurf(HLRBRep_Surface* S,
                                                                       const gp_Lin&    C)
    : nbpnts(-1),
      nbintv(-1)
{
  IntCurveSurface_QuadricCurveExactInterUtils::PerformIntersection<
    HLRBRep_Surface*,
    HLRBRep_SurfaceTool,
    gp_Lin,
    HLRBRep_LineTool,
    HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf>(S, C, pnts, intv, nbpnts, nbintv);
}

//==================================================================================================

Standard_Boolean HLRBRep_TheQuadCurvExactInterCSurf::IsDone() const
{
  return (nbpnts != -1);
}

//==================================================================================================

Standard_Integer HLRBRep_TheQuadCurvExactInterCSurf::NbRoots() const
{
  return nbpnts;
}

//==================================================================================================

Standard_Integer HLRBRep_TheQuadCurvExactInterCSurf::NbIntervals() const
{
  return nbintv;
}

//==================================================================================================

Standard_Real HLRBRep_TheQuadCurvExactInterCSurf::Root(const Standard_Integer Index) const
{
  return pnts(Index);
}

//==================================================================================================

void HLRBRep_TheQuadCurvExactInterCSurf::Intervals(const Standard_Integer Index,
                                                   Standard_Real&         a,
                                                   Standard_Real&         b) const
{
  Standard_Integer Index2 = Index + Index - 1;
  a                       = intv(Index2);
  b                       = intv(Index2 + 1);
}
