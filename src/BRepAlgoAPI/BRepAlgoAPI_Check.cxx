// Created on: 2012-12-17
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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


#include <BRepAlgoAPI_Check.hxx>

#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BRepCheck_Analyzer.hxx>

//=======================================================================
//function : BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::BRepAlgoAPI_Check()
:
  BOPAlgo_Options(),
  myTestSE(Standard_True),
  myTestSI(Standard_True),
  myOperation(BOPAlgo_UNKNOWN)
{
}

//=======================================================================
//function : BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::BRepAlgoAPI_Check(const TopoDS_Shape& theS,
                                     const Standard_Boolean bTestSE,
                                     const Standard_Boolean bTestSI)
:
  BOPAlgo_Options(),
  myS1(theS),
  myTestSE(bTestSE),
  myTestSI(bTestSI),
  myOperation(BOPAlgo_UNKNOWN)
{
  Perform();
}

//=======================================================================
//function : BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::BRepAlgoAPI_Check(const TopoDS_Shape& theS1,
                                     const TopoDS_Shape& theS2,
                                     const BOPAlgo_Operation theOp,
                                     const Standard_Boolean bTestSE,
                                     const Standard_Boolean bTestSI)
:
  BOPAlgo_Options(),
  myS1(theS1),
  myS2(theS2),
  myTestSE(bTestSE),
  myTestSI(bTestSI),
  myOperation(theOp)
{
  Perform();
}

//=======================================================================
//function : ~BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::~BRepAlgoAPI_Check()
{
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Check::Perform()
{
  // Check the incompatibility of shapes types, small edges and self-interference
  BOPAlgo_ArgumentAnalyzer anAnalyzer;
  // Set the shapes and options for the check
  anAnalyzer.SetShape1(myS1);
  anAnalyzer.SetShape2(myS2);
  anAnalyzer.OperationType() = myOperation;
  anAnalyzer.ArgumentTypeMode() = Standard_True;
  anAnalyzer.SmallEdgeMode() = myTestSE;
  anAnalyzer.SelfInterMode() = myTestSI;
  // Set options from BOPAlgo_Options
  anAnalyzer.SetRunParallel(myRunParallel);
  anAnalyzer.SetProgressIndicator(myProgressIndicator);
  anAnalyzer.SetFuzzyValue(myFuzzyValue);
  // Perform the check
  anAnalyzer.Perform();
  // Get the results
  myFaultyShapes = anAnalyzer.GetCheckResult();

  // Check the topological validity of the shapes
  Standard_Boolean isValidS1 = !myS1.IsNull() ?
    BRepCheck_Analyzer(myS1).IsValid() : Standard_True;

  Standard_Boolean isValidS2 = !myS2.IsNull() ?
    BRepCheck_Analyzer(myS2).IsValid() : Standard_True;

  if (!isValidS1 || !isValidS2) {
    BOPAlgo_CheckResult aRes;
    aRes.SetCheckStatus(BOPAlgo_NotValid);
    if (!isValidS1) {
      aRes.SetShape1(myS1);
    }
    if (!isValidS2) {
      aRes.SetShape2(myS2);
    }
    myFaultyShapes.Append(aRes);
  }
}
