// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BOPAlgo_Algo.ixx>

#include <NCollection_BaseAllocator.hxx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
  BOPAlgo_Algo::BOPAlgo_Algo()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myErrorStatus(1),
  myWarningStatus(0)
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
  BOPAlgo_Algo::BOPAlgo_Algo(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myErrorStatus(1),
  myWarningStatus(0)
{}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
  BOPAlgo_Algo::~BOPAlgo_Algo()
{
}
//=======================================================================
//function : Allocator
//purpose  : 
//=======================================================================
  const Handle(NCollection_BaseAllocator)& BOPAlgo_Algo::Allocator()const
{
  return myAllocator;
}
//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
  void BOPAlgo_Algo::CheckData()
{
  myErrorStatus=0;
}
//=======================================================================
// function: CheckResult
// purpose: 
//=======================================================================
  void BOPAlgo_Algo::CheckResult()
{
  myErrorStatus=0;
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
  Standard_Integer BOPAlgo_Algo::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
// function: WarningStatus
// purpose: 
//=======================================================================
  Standard_Integer BOPAlgo_Algo::WarningStatus()const
{
  return myWarningStatus;
}
//  myErrorStatus
//
// 1 - object is just initialized
