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


#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::BRepAlgoAPI_Check()
: 
  BRepAlgoAPI_Algo(),
  myAnalyzer(NULL),
  myFuzzyValue(0.)
{
}

//=======================================================================
//function : BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::BRepAlgoAPI_Check(const TopoDS_Shape& theS,
                                     const Standard_Boolean bTestSE,
                                     const Standard_Boolean bTestSI)
: BRepAlgoAPI_Algo(),
  myFuzzyValue(0.)
{
  Init(theS, TopoDS_Shape(), BOPAlgo_UNKNOWN, bTestSE, bTestSI);
  //
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
: BRepAlgoAPI_Algo(),
  myFuzzyValue(0.)
{
  Init(theS1, theS2, theOp, bTestSE, bTestSI);
  //
  Perform();
}

//=======================================================================
//function : ~BRepAlgoAPI_Check
//purpose  : 
//=======================================================================
BRepAlgoAPI_Check::~BRepAlgoAPI_Check()
{
  if(myAnalyzer){
    delete myAnalyzer;
    myAnalyzer=NULL;
  }
  myResult.Clear();
}

//=======================================================================
//function : SetFuzzyValue
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Check::SetFuzzyValue(const Standard_Real theFuzz)
{
  myFuzzyValue = (theFuzz < 0.) ? 0. : theFuzz;
}
//=======================================================================
//function : FuzzyValue
//purpose  : 
//=======================================================================
Standard_Real BRepAlgoAPI_Check::FuzzyValue() const
{
  return myFuzzyValue;
}

//=======================================================================
//function : SetData
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Check::SetData(const TopoDS_Shape& theS,
                                const Standard_Boolean bTestSE,
                                const Standard_Boolean bTestSI)
{
  Init(theS, TopoDS_Shape(), BOPAlgo_UNKNOWN, bTestSE, bTestSI);
}

//=======================================================================
//function : SetData
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Check::SetData(const TopoDS_Shape& theS1,
                                const TopoDS_Shape& theS2,
                                const BOPAlgo_Operation theOp,
                                const Standard_Boolean bTestSE,
                                const Standard_Boolean bTestSI)
{
  Init(theS1, theS2, theOp, bTestSE, bTestSI);
}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Check::Init(const TopoDS_Shape& theS1,
                             const TopoDS_Shape& theS2,
                             const BOPAlgo_Operation theOp,
                             const Standard_Boolean bTestSE,
                             const Standard_Boolean bTestSI)
{
  myResult.Clear();
  myS1 = theS1;
  myS2 = theS2;
  //
  myAnalyzer = new BOPAlgo_ArgumentAnalyzer();
  //
  myAnalyzer->SetShape1(myS1);
  myAnalyzer->SetShape2(myS2);
  myAnalyzer->OperationType() = theOp;
  myAnalyzer->ArgumentTypeMode() = Standard_True;
  myAnalyzer->SmallEdgeMode() = bTestSE;
  myAnalyzer->SelfInterMode() = bTestSI;
  //
  myAnalyzer->SetRunParallel(myRunParallel);
  myAnalyzer->SetProgressIndicator(myProgressIndicator);
  myAnalyzer->SetFuzzyValue(myFuzzyValue);
}

//=======================================================================
//function : Result
//purpose  : 
//=======================================================================
const BOPAlgo_ListOfCheckResult& BRepAlgoAPI_Check::Result()
{
  return myResult;
}

//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_Check::IsValid()
{
  return myResult.IsEmpty();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepAlgoAPI_Check::Perform()
{
  Standard_Boolean isS1, isS2;
  //incompatibility of shape types, small edges and self-interference
  myAnalyzer->Perform();
  if (myAnalyzer->HasFaulty()) {
    myResult = myAnalyzer->GetCheckResult();
  }

  //topological validity
  isS1 = !myS1.IsNull();
  isS2 = !myS2.IsNull();
  if (isS1) {
    BRepCheck_Analyzer anS1(myS1);
    isS1 = anS1.IsValid();
  } else {
    isS1 = Standard_True;
  }
  //
  if (isS2) {
    BRepCheck_Analyzer anS2(myS2);
    isS2 = anS2.IsValid();
  } else {
    isS2 = Standard_True;
  }
  if (!isS1 || !isS2) {
    BOPAlgo_CheckResult aRes;
    aRes.SetCheckStatus(BOPAlgo_NotValid);
    if (!isS1) {
      aRes.SetShape1(myS1);
    }
    if (!isS1) {
      aRes.SetShape2(myS2);
    }
    myResult.Append(aRes);
  }
}
