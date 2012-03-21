// Created on: 2002-02-01
// Created by: Peter KURNEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#include <BOP_WireWire.ixx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <BOPTColStd_Dump.hxx>

#include <BOPTools_DSFiller.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_WireStateFiller.hxx>

#include <BOP_CorrectTolerances.hxx>


//=======================================================================
// function: BOP_WireWire::BOP_WireWire
// purpose: 
//=======================================================================
BOP_WireWire::BOP_WireWire()
{
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void BOP_WireWire::Destroy() 
{
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void BOP_WireWire::Do() 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  // Filling the DS
  BOPTools_DSFiller aDSFiller;
  aDSFiller.SetShapes (myShape1, myShape2);
  //
  aDSFiller.Perform ();
  //
  DoWithFiller(aDSFiller);
}

//=======================================================================
// function: DoWithFiller
// purpose: 
//=======================================================================
  void BOP_WireWire::DoWithFiller(const BOPTools_DSFiller& aDSFiller) 
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  myResultMap.Clear();
  myModifiedMap.Clear();
  //
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
  //
  try {
    OCC_CATCH_SIGNALS
    // modified by NIZHNY-MKK  Fri Sep  3 15:14:17 2004.BEGIN
    if(!myDSFiller->IsDone()) {
      myErrorStatus = 1;
      BOPTColStd_Dump::PrintMessage("DSFiller is invalid: Can not build result\n");
      return;
    }
    // modified by NIZHNY-MKK  Fri Sep  3 15:14:20 2004.END

    Standard_Boolean bIsNewFiller;
    bIsNewFiller=aDSFiller.IsNewFiller();
    
    if (bIsNewFiller) {
      //
      // Preparing;
      // 1 States
      const BOPTools_PaveFiller& aPaveFiller=myDSFiller->PaveFiller();
      BOPTools_WireStateFiller aStateFiller(aPaveFiller);
      aStateFiller.Do();
      
      aDSFiller.SetNewFiller(!bIsNewFiller);
    }
    //
    //DoNewFaces();
    //
    BuildResult();
    //
    BOP_CorrectTolerances::CorrectTolerances(myResult, 0.01);
    //
    FillModified();
    myIsDone=Standard_True;
  }
  catch ( Standard_Failure ) {
    myErrorStatus = 1;
    BOPTColStd_Dump::PrintMessage("Can not build result\n"); 
  }
}

//=======================================================================
// function: BuildResult
// purpose: 
//=======================================================================
  void BOP_WireWire::BuildResult()
{
  AddSplitPartsINOUT();
  AddSplitPartsON();
  //
  MakeResult();
}

