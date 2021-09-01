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

#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_Splitter.hxx>

//=======================================================================
// function: Empty constructor
// purpose: 
//=======================================================================
BRepAlgoAPI_Splitter::BRepAlgoAPI_Splitter()
  : BRepAlgoAPI_BuilderAlgo() {}

//=======================================================================
// function: Constructor with already prepared PaveFiller
// purpose: 
//=======================================================================
BRepAlgoAPI_Splitter::BRepAlgoAPI_Splitter(const BOPAlgo_PaveFiller& thePF)
  : BRepAlgoAPI_BuilderAlgo(thePF) {}

//=======================================================================
// function: Build
// purpose: 
//=======================================================================
void BRepAlgoAPI_Splitter::Build(const Message_ProgressRange& theRange)
{
  // Set Not Done status by default
  NotDone();
  // Clear the contents
  Clear();
  // Check for availability of arguments and tools
  if (myArguments.IsEmpty() ||
     (myArguments.Extent() + myTools.Extent()) < 2)
  {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }

  // If necessary perform intersection of the argument shapes
  Message_ProgressScope aPS(theRange, "Performing Split operation", myIsIntersectionNeeded ? 100 : 30);
  if (myIsIntersectionNeeded)
  {
    // Combine Arguments and Tools for intersection into a single list
    TopTools_ListOfShape aLArgs = myArguments;
    for (TopTools_ListOfShape::Iterator it(myTools); it.More(); it.Next())
      aLArgs.Append(it.Value());

    // Perform intersection
    IntersectShapes(aLArgs, aPS.Next(70));
    if (HasErrors())
      return;
  }

  // Initialization of the building tool
  myBuilder = new BOPAlgo_Splitter(myAllocator);
  myBuilder->SetArguments(myArguments);
  ((BOPAlgo_Splitter*)myBuilder)->SetTools(myTools);

  // Build result shape basing on the intersection results
  BuildResult(aPS.Next(30));
}
