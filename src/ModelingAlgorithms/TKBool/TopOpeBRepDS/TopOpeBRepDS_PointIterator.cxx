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

#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>

//=================================================================================================

TopOpeBRepDS_PointIterator::TopOpeBRepDS_PointIterator(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L)
    : TopOpeBRepDS_InterferenceIterator(L)
{
  Match();
}

//=================================================================================================

bool TopOpeBRepDS_PointIterator::MatchInterference(
  const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  TopOpeBRepDS_Kind GT = I->GeometryType();
  bool              r  = (GT == TopOpeBRepDS_POINT) || (GT == TopOpeBRepDS_VERTEX);
  return r;
}

//=================================================================================================

int TopOpeBRepDS_PointIterator::Current() const
{
  return Value()->Geometry();
}

//=================================================================================================

TopAbs_Orientation TopOpeBRepDS_PointIterator::Orientation(const TopAbs_State S) const
{
  occ::handle<TopOpeBRepDS_Interference> I = Value();
  const TopOpeBRepDS_Transition&         T = I->Transition();
  TopAbs_Orientation                     o = T.Orientation(S);
  return o;
}

//=================================================================================================

double TopOpeBRepDS_PointIterator::Parameter() const
{
  const occ::handle<TopOpeBRepDS_Interference>& I = Value();
  occ::handle<Standard_Type>                    T = I->DynamicType();
  if (T == STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference))
  {
    return occ::down_cast<TopOpeBRepDS_CurvePointInterference>(I)->Parameter();
  }
  else if (T == STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference))
  {
    return occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->Parameter();
  }
  else
  {
    throw Standard_ProgramError("TopOpeBRepDS_PointIterator::Parameter()");
  }
}

//=================================================================================================

bool TopOpeBRepDS_PointIterator::IsVertex() const
{
  return (Value()->GeometryType() == TopOpeBRepDS_VERTEX);
}

//=================================================================================================

bool TopOpeBRepDS_PointIterator::IsPoint() const
{
  return (Value()->GeometryType() == TopOpeBRepDS_POINT);
}

//=================================================================================================

bool TopOpeBRepDS_PointIterator::DiffOriented() const
{
  const occ::handle<TopOpeBRepDS_Interference>& I = Value();
  if (I->DynamicType() == STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference))
  {
    return occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->Config()
           == TopOpeBRepDS_DIFFORIENTED;
  }
  else
  {
    throw Standard_ProgramError("TopOpeBRepDS_PointIterator::DiffOriented()");
  }
}

//=================================================================================================

bool TopOpeBRepDS_PointIterator::SameOriented() const
{
  const occ::handle<TopOpeBRepDS_Interference>& I = Value();
  if (I->DynamicType() == STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference))
  {
    return occ::down_cast<TopOpeBRepDS_EdgeVertexInterference>(I)->Config()
           == TopOpeBRepDS_SAMEORIENTED;
  }
  else
  {
    throw Standard_ProgramError("TopOpeBRepDS_PointIterator::SameOriented()");
  }
}

//=================================================================================================

int TopOpeBRepDS_PointIterator::Support() const
{
  return (Value()->Support());
}
