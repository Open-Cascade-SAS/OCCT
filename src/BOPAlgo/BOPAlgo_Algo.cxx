// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPAlgo_Algo.hxx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BOPAlgo_Algo::BOPAlgo_Algo()
:
  BOPAlgo_Options(NCollection_BaseAllocator::CommonBaseAllocator())
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
BOPAlgo_Algo::BOPAlgo_Algo
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Options(theAllocator)
{}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BOPAlgo_Algo::~BOPAlgo_Algo()
{
}

//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_Algo::CheckData()
{
  GetReport()->Clear(Message_Fail);
}
//=======================================================================
// function: CheckResult
// purpose: 
//=======================================================================
void BOPAlgo_Algo::CheckResult()
{
  GetReport()->Clear(Message_Fail);
}
