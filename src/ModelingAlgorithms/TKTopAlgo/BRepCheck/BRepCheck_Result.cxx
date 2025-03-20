// Created on: 1995-12-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepCheck_Result.hxx>

#include <BRepCheck.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Result, Standard_Transient)

//=================================================================================================

BRepCheck_Result::BRepCheck_Result()
    : myMin(Standard_False),
      myBlind(Standard_False)
{
  //
}

//=================================================================================================

void BRepCheck_Result::Init(const TopoDS_Shape& S)
{
  myShape = S;
  myMin   = Standard_False;
  myBlind = Standard_False;
  myMap.Clear();
  Minimum();
}

//=================================================================================================

void BRepCheck_Result::SetFailStatus(const TopoDS_Shape& S)
{
  Standard_Mutex::Sentry          aLock(myMutex.get());
  Handle(BRepCheck_HListOfStatus) aList;
  if (!myMap.Find(S, aList))
  {
    aList = new BRepCheck_HListOfStatus();
    myMap.Bind(S, aList);
  }

  BRepCheck::Add(*aList, BRepCheck_CheckFail);
}

//=================================================================================================

void BRepCheck_Result::InitContextIterator()
{
  myIter.Initialize(myMap);
  // At least 1 element : the Shape itself
  if (myIter.Key().IsSame(myShape))
  {
    myIter.Next();
  }
}

//=================================================================================================

void BRepCheck_Result::NextShapeInContext()
{
  myIter.Next();
  if (myIter.More() && myIter.Key().IsSame(myShape))
  {
    myIter.Next();
  }
}

//=================================================================================================

void BRepCheck_Result::SetParallel(Standard_Boolean theIsParallel)
{
  if (theIsParallel && myMutex.IsNull())
  {
    myMutex.reset(new Standard_HMutex());
  }
}
