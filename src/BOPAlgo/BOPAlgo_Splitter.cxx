// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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


#include <BOPAlgo_Splitter.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Splitter::BOPAlgo_Splitter()
:
  BOPAlgo_Builder(),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Splitter::BOPAlgo_Splitter
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Builder(theAllocator),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_Splitter::~BOPAlgo_Splitter()
{
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_Splitter::Clear()
{
  BOPAlgo_Builder::Clear();
  myTools.Clear();
  myMapTools.Clear();
}
//=======================================================================
//function : AddTool
//purpose  : 
//=======================================================================
void BOPAlgo_Splitter::AddTool(const TopoDS_Shape& theShape)
{
  if (myMapTools.Add(theShape)) {
    myTools.Append(theShape);
  }
}
//=======================================================================
//function : SetTools
//purpose  : 
//=======================================================================
void BOPAlgo_Splitter::SetTools(const BOPCol_ListOfShape& theShapes)
{
  myTools.Clear();
  BOPCol_ListIteratorOfListOfShape aIt(theShapes);
  for (; aIt.More(); aIt.Next()) {
    AddTool(aIt.Value());
  }
}

//=======================================================================
// function: CheckData
// purpose: 
//=======================================================================
void BOPAlgo_Splitter::CheckData()
{
  if (myArguments.IsEmpty() ||
      (myArguments.Extent() + myTools.Extent()) < 2) {
    // too few arguments to process
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  CheckFiller();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_Splitter::Perform()
{
  GetReport()->Clear();
  //
  if (myEntryPoint == 1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller = NULL;
    }
  }
  //
  // prepare shapes for intersection
  BOPCol_ListOfShape aLS;
  //
  BOPCol_ListIteratorOfListOfShape aItLS(myArguments);
  for (; aItLS.More(); aItLS.Next()) {
    aLS.Append(aItLS.Value());
  }
  //
  aItLS.Initialize(myTools);
  for (; aItLS.More(); aItLS.Next()) {
    aLS.Append(aItLS.Value());
  }
  //
  BOPAlgo_PaveFiller *pPF = new BOPAlgo_PaveFiller();
  pPF->SetArguments(aLS);
  pPF->SetRunParallel(myRunParallel);
  pPF->SetProgressIndicator(myProgressIndicator);
  pPF->SetFuzzyValue(myFuzzyValue);
  pPF->SetNonDestructive(myNonDestructive);
  pPF->SetGlue(myGlue);
  //
  pPF->Perform();
  //
  myEntryPoint = 1;
  PerformInternal(*pPF);
}
