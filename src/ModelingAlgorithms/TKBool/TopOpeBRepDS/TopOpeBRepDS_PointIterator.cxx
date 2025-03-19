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

TopOpeBRepDS_PointIterator::TopOpeBRepDS_PointIterator(const TopOpeBRepDS_ListOfInterference& L)
    : TopOpeBRepDS_InterferenceIterator(L)
{
  Match();
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_PointIterator::MatchInterference(
  const Handle(TopOpeBRepDS_Interference)& I) const
{
  TopOpeBRepDS_Kind GT = I->GeometryType();
  Standard_Boolean  r  = (GT == TopOpeBRepDS_POINT) || (GT == TopOpeBRepDS_VERTEX);
  return r;
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_PointIterator::Current() const
{
  return Value()->Geometry();
}

//=================================================================================================

TopAbs_Orientation TopOpeBRepDS_PointIterator::Orientation(const TopAbs_State S) const
{
  Handle(TopOpeBRepDS_Interference) I = Value();
  const TopOpeBRepDS_Transition&    T = I->Transition();
  TopAbs_Orientation                o = T.Orientation(S);
  return o;
}

//=================================================================================================

Standard_Real TopOpeBRepDS_PointIterator::Parameter() const
{
  const Handle(TopOpeBRepDS_Interference)& I = Value();
  Handle(Standard_Type)                    T = I->DynamicType();
  if (T == STANDARD_TYPE(TopOpeBRepDS_CurvePointInterference))
  {
    return Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I)->Parameter();
  }
  else if (T == STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference))
  {
    return Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I)->Parameter();
  }
  else
  {
    throw Standard_ProgramError("TopOpeBRepDS_PointIterator::Parameter()");
  }
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_PointIterator::IsVertex() const
{
  return (Value()->GeometryType() == TopOpeBRepDS_VERTEX);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_PointIterator::IsPoint() const
{
  return (Value()->GeometryType() == TopOpeBRepDS_POINT);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_PointIterator::DiffOriented() const
{
  const Handle(TopOpeBRepDS_Interference)& I = Value();
  if (I->DynamicType() == STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference))
  {
    return Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I)->Config()
           == TopOpeBRepDS_DIFFORIENTED;
  }
  else
  {
    throw Standard_ProgramError("TopOpeBRepDS_PointIterator::DiffOriented()");
  }
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_PointIterator::SameOriented() const
{
  const Handle(TopOpeBRepDS_Interference)& I = Value();
  if (I->DynamicType() == STANDARD_TYPE(TopOpeBRepDS_EdgeVertexInterference))
  {
    return Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I)->Config()
           == TopOpeBRepDS_SAMEORIENTED;
  }
  else
  {
    throw Standard_ProgramError("TopOpeBRepDS_PointIterator::SameOriented()");
  }
}

//=================================================================================================

Standard_Integer TopOpeBRepDS_PointIterator::Support() const
{
  return (Value()->Support());
}
