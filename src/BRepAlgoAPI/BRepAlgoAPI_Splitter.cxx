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

#include <BRepAlgoAPI_Splitter.hxx>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Splitter.hxx>
#include <BOPAlgo_Alerts.hxx>

//=======================================================================
// function: Empty constructor
// purpose: 
//=======================================================================
BRepAlgoAPI_Splitter::BRepAlgoAPI_Splitter()
  : BRepAlgoAPI_BuilderAlgo() {}

//=======================================================================
// function: Constructor with already filled PaveFiller
// purpose: 
//=======================================================================
BRepAlgoAPI_Splitter::BRepAlgoAPI_Splitter(const BOPAlgo_PaveFiller& thePF)
  : BRepAlgoAPI_BuilderAlgo(thePF) {}

//=======================================================================
// function: Destructor
// purpose: 
//=======================================================================
BRepAlgoAPI_Splitter::~BRepAlgoAPI_Splitter()
{
}

//=======================================================================
// function: SetTools
// purpose: 
//=======================================================================
void BRepAlgoAPI_Splitter::SetTools(const TopTools_ListOfShape& theLS)
{
  myTools = theLS;
}

//=======================================================================
// function: Tools
// purpose: 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_Splitter::Tools() const
{
  return myTools;
}

//=======================================================================
// function: Build
// purpose: 
//=======================================================================
void BRepAlgoAPI_Splitter::Build()
{
  NotDone();
  //
  Clear();
  //
  if (myArguments.IsEmpty() ||
    (myArguments.Extent() + myTools.Extent()) < 2) {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  if (myEntryType) {
    if (myDSFiller) {
      delete myDSFiller;
    }
    myDSFiller = new BOPAlgo_PaveFiller(myAllocator);
    //
    TopTools_ListOfShape aLArgs;
    TopTools_ListIteratorOfListOfShape aItLA(myArguments);
    for (; aItLA.More(); aItLA.Next()) {
      aLArgs.Append(aItLA.Value());
    }
    //
    aItLA.Initialize(myTools);
    for (; aItLA.More(); aItLA.Next()) {
      aLArgs.Append(aItLA.Value());
    }
    //
    myDSFiller->SetArguments(aLArgs);
    //
    myDSFiller->SetRunParallel(myRunParallel);
    myDSFiller->SetProgressIndicator(myProgressIndicator);
    myDSFiller->SetFuzzyValue(myFuzzyValue);
    myDSFiller->SetNonDestructive(myNonDestructive);
    myDSFiller->SetGlue(myGlue);
    myDSFiller->SetUseOBB(myUseOBB);
    //
    myDSFiller->Perform();
    //
    GetReport()->Merge (myDSFiller->GetReport());
    if (HasErrors()) 
    {
      return;
    }
  }
  //
  if (myBuilder) {
    delete myBuilder;
  }
  //
  {
    BOPAlgo_Splitter *pSplitter = new BOPAlgo_Splitter(myAllocator);
    pSplitter->SetArguments(myArguments);
    pSplitter->SetTools(myTools);
    myBuilder = pSplitter;
  }
  //
  myBuilder->SetRunParallel(myRunParallel);
  myBuilder->SetProgressIndicator(myProgressIndicator);
  myBuilder->SetCheckInverted(myCheckInverted);
  //
  myBuilder->PerformWithFiller(*myDSFiller);
  //
  GetReport()->Merge (myBuilder->GetReport());
  //
  Done();
  myShape = myBuilder->Shape();
}
