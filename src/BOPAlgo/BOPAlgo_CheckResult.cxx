// Created on: 2004-09-02
// Created by: Oleg FEDYAEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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


#include <BOPAlgo_CheckResult.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
// function:  BOPAlgo_CheckResult()
// purpose: 
//=======================================================================
BOPAlgo_CheckResult::BOPAlgo_CheckResult() 
: 
  myStatus(BOPAlgo_CheckUnknown),
  myMaxDist1(0.),
  myMaxDist2(0.),
  myMaxPar1(0.),
  myMaxPar2(0.)
{
}

void BOPAlgo_CheckResult::SetShape1(const TopoDS_Shape& TheShape)
{
  myShape1 = TheShape;
}

void BOPAlgo_CheckResult::AddFaultyShape1(const TopoDS_Shape& TheShape)
{
  myFaulty1.Append(TheShape);
}

void BOPAlgo_CheckResult::SetShape2(const TopoDS_Shape& TheShape)
{
  myShape2 = TheShape;
}

void BOPAlgo_CheckResult::AddFaultyShape2(const TopoDS_Shape& TheShape)
{
  myFaulty2.Append(TheShape);
}

const TopoDS_Shape& BOPAlgo_CheckResult::GetShape1() const
{
  return myShape1;
}

const TopoDS_Shape & BOPAlgo_CheckResult::GetShape2() const
{
  return myShape2;
}

const TopTools_ListOfShape& BOPAlgo_CheckResult::GetFaultyShapes1() const
{
  return myFaulty1;
}

const TopTools_ListOfShape& BOPAlgo_CheckResult::GetFaultyShapes2() const
{
  return myFaulty2;
}

void BOPAlgo_CheckResult::SetCheckStatus(const BOPAlgo_CheckStatus TheStatus)
{
  myStatus = TheStatus;
}

BOPAlgo_CheckStatus BOPAlgo_CheckResult::GetCheckStatus() const
{
  return myStatus;
}

void BOPAlgo_CheckResult::SetMaxDistance1(const Standard_Real theDist)
{
  myMaxDist1 = theDist;
}

void BOPAlgo_CheckResult::SetMaxDistance2(const Standard_Real theDist)
{
  myMaxDist2 = theDist;
}

void BOPAlgo_CheckResult::SetMaxParameter1(const Standard_Real thePar)
{
  myMaxPar1 = thePar;
}

void BOPAlgo_CheckResult::SetMaxParameter2(const Standard_Real thePar)
{
  myMaxPar2 = thePar;
}

Standard_Real BOPAlgo_CheckResult::GetMaxDistance1() const
{
  return myMaxDist1;
}

Standard_Real BOPAlgo_CheckResult::GetMaxDistance2() const
{
  return myMaxDist2;
}

Standard_Real BOPAlgo_CheckResult::GetMaxParameter1() const
{
  return myMaxPar1;
}

Standard_Real BOPAlgo_CheckResult::GetMaxParameter2() const
{
  return myMaxPar2;
}
