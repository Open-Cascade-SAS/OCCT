// Created on: 1994-09-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <LProp_CurAndInf.hxx>
#include <Standard_OutOfRange.hxx>

//=================================================================================================

LProp_CurAndInf::LProp_CurAndInf() {}

//=================================================================================================

void LProp_CurAndInf::AddInflection(const Standard_Real Param)
{
  if (theParams.IsEmpty())
  {
    theParams.Append(Param);
    theTypes.Append(LProp_Inflection);
    return;
  }
  if (Param > theParams.Last())
  {
    theParams.Append(Param);
    theTypes.Append(LProp_Inflection);
    return;
  }
  for (Standard_Integer i = 1; i <= theParams.Length(); i++)
  {
    if (theParams.Value(i) > Param)
    {
      theParams.InsertBefore(i, Param);
      theTypes.InsertBefore(i, LProp_Inflection);
      break;
    }
  }
}

//=================================================================================================

void LProp_CurAndInf::AddExtCur(const Standard_Real Param, const Standard_Boolean IsMin)
{
  LProp_CIType TypePoint;
  if (IsMin)
    TypePoint = LProp_MinCur;
  else
    TypePoint = LProp_MaxCur;

  if (theParams.IsEmpty())
  {
    theParams.Append(Param);
    theTypes.Append(TypePoint);
    return;
  }
  if (Param > theParams.Last())
  {
    theParams.Append(Param);
    theTypes.Append(TypePoint);
    return;
  }
  for (Standard_Integer i = 1; i <= theParams.Length(); i++)
  {
    if (theParams.Value(i) > Param)
    {
      theParams.InsertBefore(i, Param);
      theTypes.InsertBefore(i, TypePoint);
      break;
    }
  }
}

//=================================================================================================

void LProp_CurAndInf::Clear()
{
  theParams.Clear();
  theTypes.Clear();
}

//=================================================================================================

Standard_Boolean LProp_CurAndInf::IsEmpty() const
{
  return theParams.IsEmpty();
}

//=================================================================================================

Standard_Integer LProp_CurAndInf::NbPoints() const
{
  return theParams.Length();
}

//=================================================================================================

Standard_Real LProp_CurAndInf::Parameter(const Standard_Integer N) const
{
  if (N < 1 || N > NbPoints())
  {
    throw Standard_OutOfRange();
  }
  return theParams.Value(N);
}

//=================================================================================================

LProp_CIType LProp_CurAndInf::Type(const Standard_Integer N) const
{
  if (N < 1 || N > NbPoints())
  {
    throw Standard_OutOfRange();
  }
  return theTypes.Value(N);
}
