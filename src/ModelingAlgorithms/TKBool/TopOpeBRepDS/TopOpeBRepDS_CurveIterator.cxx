// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#include <Geom2d_Curve.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_SurfaceCurveInterference.hxx>

//=================================================================================================

TopOpeBRepDS_CurveIterator::TopOpeBRepDS_CurveIterator(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L)
    : TopOpeBRepDS_InterferenceIterator(L)
{
  Match();
}

//=================================================================================================

bool TopOpeBRepDS_CurveIterator::MatchInterference(
  const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  TopOpeBRepDS_Kind GT = I->GeometryType();
  bool              r  = (GT == TopOpeBRepDS_CURVE);
  return r;
}

//=================================================================================================

int TopOpeBRepDS_CurveIterator::Current() const
{
  occ::handle<TopOpeBRepDS_Interference> I = Value();
  int                                    G = I->Geometry();
  return G;
}

//=================================================================================================

TopAbs_Orientation TopOpeBRepDS_CurveIterator::Orientation(const TopAbs_State S) const
{
  occ::handle<TopOpeBRepDS_Interference> I = Value();
  const TopOpeBRepDS_Transition&         T = I->Transition();
  TopAbs_Orientation                     o = T.Orientation(S);
  return o;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& TopOpeBRepDS_CurveIterator::PCurve() const
{
  return (*((occ::handle<TopOpeBRepDS_SurfaceCurveInterference>*)&Value()))->PCurve();
}
