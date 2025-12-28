// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <IntCurveSurface_TheQuadCurvExactHInter.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Adaptor3d_Surface.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter.hxx>

#include "IntCurveSurface_QuadricCurveExactInterUtils.pxx"

//==================================================================================================

IntCurveSurface_TheQuadCurvExactHInter::IntCurveSurface_TheQuadCurvExactHInter(
  const occ::handle<Adaptor3d_Surface>& S,
  const occ::handle<Adaptor3d_Curve>&   C)
    : nbpnts(-1),
      nbintv(-1)
{
  IntCurveSurface_QuadricCurveExactInterUtils::PerformIntersection<
    occ::handle<Adaptor3d_Surface>,
    Adaptor3d_HSurfaceTool,
    occ::handle<Adaptor3d_Curve>,
    IntCurveSurface_TheHCurveTool,
    IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter>(S, C, pnts, intv, nbpnts, nbintv);
}

//==================================================================================================

bool IntCurveSurface_TheQuadCurvExactHInter::IsDone() const
{
  return (nbpnts != -1);
}

//==================================================================================================

int IntCurveSurface_TheQuadCurvExactHInter::NbRoots() const
{
  return nbpnts;
}

//==================================================================================================

int IntCurveSurface_TheQuadCurvExactHInter::NbIntervals() const
{
  return nbintv;
}

//==================================================================================================

double IntCurveSurface_TheQuadCurvExactHInter::Root(const int Index) const
{
  return pnts(Index);
}

//==================================================================================================

void IntCurveSurface_TheQuadCurvExactHInter::Intervals(const int Index, double& a, double& b) const
{
  int Index2 = Index + Index - 1;
  a          = intv(Index2);
  b          = intv(Index2 + 1);
}
