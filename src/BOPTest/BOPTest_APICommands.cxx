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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPTest.hxx>
#include <BOPTest_Objects.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Splitter.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <stdio.h>
#include <string.h>
static 
  void ConvertList(const BOPCol_ListOfShape& aLSB,
                   TopTools_ListOfShape& aLS);

static Standard_Integer bapibuild(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bapibop  (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bapisplit(Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : APICommands
//purpose  : 
//=======================================================================
void BOPTest::APICommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands  
  theCommands.Add("bapibuild", "use bapibuild r" , __FILE__, bapibuild, g);
  theCommands.Add("bapibop", "use bapibop r type" , __FILE__, bapibop, g);
  theCommands.Add("bapisplit", "use bapisplit r" , __FILE__, bapisplit, g);
}
//=======================================================================
//function : bapibop
//purpose  : 
//=======================================================================
Standard_Integer bapibop(Draw_Interpretor& di,
                         Standard_Integer n, 
                         const char** a) 
{ 
  if (n<3) {
    di << " use bapibop r type\n";
    return 0;
  }
  //
  Standard_Boolean bRunParallel, bNonDestructive;
  Standard_Integer iOp;
  Standard_Real aFuzzyValue;
  BRepAlgoAPI_Common aCommon;
  BRepAlgoAPI_Fuse aFuse;
  BRepAlgoAPI_Cut aCut;
  BRepAlgoAPI_Section aSection;
  BRepAlgoAPI_BooleanOperation *pBuilder;
  BOPAlgo_Operation aOp;
  //
  pBuilder=NULL;
  iOp=atoi(a[2]);
  if (iOp<0 || iOp>4) {
    di << "invalid operation type\n";
    return 0;
  }
  aOp=(BOPAlgo_Operation)iOp;
  //
  switch (aOp) {
   case BOPAlgo_COMMON:
     pBuilder=&aCommon;
     break;
     //
   case BOPAlgo_FUSE:
     pBuilder=&aFuse;
     break;
     //
   case BOPAlgo_CUT:
   case BOPAlgo_CUT21:
     pBuilder=&aCut;
     break;
     //
   case BOPAlgo_SECTION:
     pBuilder=&aSection;
     break;
     //
   default:
     break;
  }
  //
  BOPCol_ListOfShape& aLSB=BOPTest_Objects::Shapes();
  BOPCol_ListOfShape& aLTB=BOPTest_Objects::Tools();
  //
  TopTools_ListOfShape aLS, aLT;
  ConvertList(aLSB, aLS);
  ConvertList(aLTB, aLT);
  //
  bRunParallel=BOPTest_Objects::RunParallel();
  aFuzzyValue=BOPTest_Objects::FuzzyValue();
  bNonDestructive = BOPTest_Objects::NonDestructive();
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  if (aOp!=BOPAlgo_CUT21) {
    pBuilder->SetArguments(aLS);
    pBuilder->SetTools(aLT);
  }
  else {
    pBuilder->SetArguments(aLT);
    pBuilder->SetTools(aLS);
  }
  //
  pBuilder->SetRunParallel(bRunParallel);
  pBuilder->SetFuzzyValue(aFuzzyValue);
  pBuilder->SetNonDestructive(bNonDestructive);
  pBuilder->SetGlue(aGlue);
  //
  pBuilder->Build(); 
  //
  if (pBuilder->HasWarnings()) {
    Standard_SStream aSStream;
    pBuilder->DumpWarnings(aSStream);
    di << aSStream;
  }
  //
  if (pBuilder->HasErrors()) {
    Standard_SStream aSStream;
    pBuilder->DumpErrors(aSStream);
    di << aSStream;
    return 0;
  }
  //
  const TopoDS_Shape& aR=pBuilder->Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}
//=======================================================================
//function : bapibuild
//purpose  : 
//=======================================================================
Standard_Integer bapibuild(Draw_Interpretor& di,
                        Standard_Integer n, 
                        const char** a) 
{ 
  if (n<2) {
    di << " use bapibuild r\n";
    return 0;
  }
  //
  Standard_Boolean bRunParallel, bNonDestructive;
  Standard_Integer iErr;
  Standard_Real aFuzzyValue;
  BRepAlgoAPI_BuilderAlgo aBuilder;
  //
  BOPCol_ListOfShape& aLSB=BOPTest_Objects::Shapes();
  BOPCol_ListOfShape& aLTB=BOPTest_Objects::Tools();
  //
  TopTools_ListOfShape aLS;
  ConvertList(aLSB, aLS);
  ConvertList(aLTB, aLS);
  //
  bRunParallel=BOPTest_Objects::RunParallel();
  aFuzzyValue=BOPTest_Objects::FuzzyValue();
  bNonDestructive = BOPTest_Objects::NonDestructive();
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  aBuilder.SetArguments(aLS);
  aBuilder.SetRunParallel(bRunParallel);
  aBuilder.SetFuzzyValue(aFuzzyValue);
  aBuilder.SetNonDestructive(bNonDestructive);
  aBuilder.SetGlue(aGlue);
  //
  aBuilder.Build(); 
  //
  if (aBuilder.HasWarnings()) {
    Standard_SStream aSStream;
    aBuilder.DumpWarnings(aSStream);
    di << aSStream;
  }
  //
  iErr=aBuilder.HasErrors();
  if (iErr) {
    Standard_SStream aSStream;
    aBuilder.DumpErrors(aSStream);
    di << aSStream;
    return 0;
  }
  //
  const TopoDS_Shape& aR=aBuilder.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}
//=======================================================================
//function : ConvertLists
//purpose  : 
//=======================================================================
void ConvertList(const BOPCol_ListOfShape& aLSB,
                 TopTools_ListOfShape& aLS)
{
  BOPCol_ListIteratorOfListOfShape aItB;
  //
  aItB.Initialize(aLSB);
  for (; aItB.More(); aItB.Next()) {
    const TopoDS_Shape& aS=aItB.Value();
    aLS.Append(aS);
  }
}

//=======================================================================
//function : bapisplit
//purpose  : 
//=======================================================================
Standard_Integer bapisplit(Draw_Interpretor& di,
  Standard_Integer n,
  const char** a)
{
  if (n < 2) {
    di << " use bapisplit r\n";
    return 1;
  }
  //
  BRepAlgoAPI_Splitter aSplitter;
  // setting arguments
  aSplitter.SetArguments(BOPTest_Objects::Shapes());
  aSplitter.SetTools(BOPTest_Objects::Tools());
  // setting options
  aSplitter.SetRunParallel(BOPTest_Objects::RunParallel());
  aSplitter.SetFuzzyValue(BOPTest_Objects::FuzzyValue());
  aSplitter.SetNonDestructive(BOPTest_Objects::NonDestructive());
  aSplitter.SetGlue(BOPTest_Objects::Glue());
  //
  // performing operation
  aSplitter.Build();
  // check warning status
  if (aSplitter.HasWarnings()) {
    Standard_SStream aSStream;
    aSplitter.DumpWarnings(aSStream);
    di << aSStream;
  }
  // checking error status
  Standard_Integer iErr = aSplitter.HasErrors();
  if (iErr) {
    Standard_SStream aSStream;
    aSplitter.DumpErrors(aSStream);
    di << aSStream;
    return 0;
  }
  //
  // getting the result of the operation
  const TopoDS_Shape& aR = aSplitter.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}
