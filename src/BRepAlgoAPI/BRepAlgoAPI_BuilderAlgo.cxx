// Created by: Peter KURNEV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <BRepAlgoAPI_BuilderAlgo.ixx>

#include <NCollection_BaseAllocator.hxx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo()
:
  BRepAlgoAPI_Algo(),
  myDSFiller(NULL),
  myBuilder(NULL),
  myFuzzyValue(0.)
{}
//=======================================================================
// function: 
// purpose: 
//=======================================================================
BRepAlgoAPI_BuilderAlgo::BRepAlgoAPI_BuilderAlgo
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BRepAlgoAPI_Algo(theAllocator),
  myDSFiller(NULL),
  myBuilder(NULL),
  myFuzzyValue(0.)
{}

//=======================================================================
// function: ~
// purpose: 
//=======================================================================
BRepAlgoAPI_BuilderAlgo::~BRepAlgoAPI_BuilderAlgo()
{
}
//=======================================================================
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BuilderAlgo::SetFuzzyValue(const Standard_Real theFuzz)
{
  if (theFuzz > 0.) {
    myFuzzyValue = theFuzz;
  }
}
//=======================================================================
//function : FuzzyValue
//purpose  : 
//=======================================================================
Standard_Real BRepAlgoAPI_BuilderAlgo::FuzzyValue() const
{
  return myFuzzyValue;
}
