// File:      BOP_CheckResult.cxx
// Created:   02.09.04 16:26:31
// Author:    Oleg FEDYAEV
// Copyright: Open Cascade 2003

#include <BOP_CheckResult.ixx>

//=======================================================================
// function:  BOP_CheckResult()
// purpose: 
//=======================================================================
BOP_CheckResult::BOP_CheckResult() : myStatus(BOP_CheckUnknown)
{
}

void BOP_CheckResult::SetShape1(const TopoDS_Shape& TheShape)
{
  myShape1 = TheShape;
}

void BOP_CheckResult::AddFaultyShape1(const TopoDS_Shape& TheShape)
{
  myFaulty1.Append(TheShape);
}

void BOP_CheckResult::SetShape2(const TopoDS_Shape& TheShape)
{
  myShape2 = TheShape;
}

void BOP_CheckResult::AddFaultyShape2(const TopoDS_Shape& TheShape)
{
  myFaulty2.Append(TheShape);
}

const TopoDS_Shape& BOP_CheckResult::GetShape1() const
{
  return myShape1;
}

const TopoDS_Shape & BOP_CheckResult::GetShape2() const
{
  return myShape2;
}

const TopTools_ListOfShape& BOP_CheckResult::GetFaultyShapes1() const
{
  return myFaulty1;
}

const TopTools_ListOfShape& BOP_CheckResult::GetFaultyShapes2() const
{
  return myFaulty2;
}

void BOP_CheckResult::SetCheckStatus(const BOP_CheckStatus TheStatus)
{
  myStatus = TheStatus;
}

BOP_CheckStatus BOP_CheckResult::GetCheckStatus() const
{
  return myStatus;
}
