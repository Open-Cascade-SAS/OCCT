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
#include <BOPTest.hxx>
#include <BOPTest_Objects.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Splitter.hxx>
#include <BRepTest_Objects.hxx>
#include <DBRep.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <Draw_ProgressIndicator.hxx>

#include <stdio.h>

static Standard_Integer bapibuild(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bapibop(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bapisplit(Draw_Interpretor&, Standard_Integer, const char**);

//=================================================================================================

void BOPTest::APICommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done)
    return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands
  theCommands.Add(
    "bapibuild",
    "Builds the result of General Fuse operation using top level API.\n"
    "\t\tObjects for the operation are added using commands baddobjects and baddtools.\n"
    "\t\tUsage: bapibuild result",
    __FILE__,
    bapibuild,
    g);

  theCommands.Add(
    "bapibop",
    "Builds the result of Boolean operation using top level API.\n"
    "\t\tObjects for the operation are added using commands baddobjects and baddtools.\n"
    "\t\tUsage: bapibop r operation\n"
    "\t\tWhere:\n"
    "\t\tresult - name of the result shape\n"
    "\t\top - type of Boolean operation. Possible values:\n"
    "\t\t     - 0/common - for Common operation\n"
    "\t\t     - 1/fuse - for Fuse operation\n"
    "\t\t     - 2/cut - for Cut operation\n"
    "\t\t     - 3/tuc/cut21 - for Cut21 operation\n"
    "\t\t     - 4/section - for Section operation",
    __FILE__,
    bapibop,
    g);

  theCommands.Add(
    "bapisplit",
    "Builds the result of Split operation using top level API.\n"
    "\t\tObjects for the operation are added using commands baddobjects and baddtools.\n"
    "\t\tUsage: bapisplit result",
    __FILE__,
    bapisplit,
    g);
}

//=================================================================================================

Standard_Integer bapibop(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 3)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  BOPAlgo_Operation anOp = BOPTest::GetOperationType(a[2]);
  if (anOp == BOPAlgo_UNKNOWN)
  {
    di << "Invalid operation type\n";
    return 0;
  }
  //
  Standard_Boolean              bRunParallel, bNonDestructive;
  Standard_Real                 aFuzzyValue;
  BRepAlgoAPI_Common            aCommon;
  BRepAlgoAPI_Fuse              aFuse;
  BRepAlgoAPI_Cut               aCut;
  BRepAlgoAPI_Section           aSection;
  BRepAlgoAPI_BooleanOperation* pBuilder;
  //
  pBuilder = NULL;
  //
  switch (anOp)
  {
    case BOPAlgo_COMMON:
      pBuilder = &aCommon;
      break;
      //
    case BOPAlgo_FUSE:
      pBuilder = &aFuse;
      break;
      //
    case BOPAlgo_CUT:
    case BOPAlgo_CUT21:
      pBuilder = &aCut;
      break;
      //
    case BOPAlgo_SECTION:
      pBuilder = &aSection;
      break;
      //
    default:
      break;
  }
  //
  TopTools_ListOfShape& aLS = BOPTest_Objects::Shapes();
  TopTools_ListOfShape& aLT = BOPTest_Objects::Tools();
  //
  bRunParallel           = BOPTest_Objects::RunParallel();
  aFuzzyValue            = BOPTest_Objects::FuzzyValue();
  bNonDestructive        = BOPTest_Objects::NonDestructive();
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  if (anOp != BOPAlgo_CUT21)
  {
    pBuilder->SetArguments(aLS);
    pBuilder->SetTools(aLT);
  }
  else
  {
    pBuilder->SetArguments(aLT);
    pBuilder->SetTools(aLS);
  }
  //
  pBuilder->SetRunParallel(bRunParallel);
  pBuilder->SetFuzzyValue(aFuzzyValue);
  pBuilder->SetNonDestructive(bNonDestructive);
  pBuilder->SetGlue(aGlue);
  pBuilder->SetCheckInverted(BOPTest_Objects::CheckInverted());
  pBuilder->SetUseOBB(BOPTest_Objects::UseOBB());
  pBuilder->SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());
  //
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
  pBuilder->Build(aProgress->Start());
  pBuilder->SimplifyResult(BOPTest_Objects::UnifyEdges(),
                           BOPTest_Objects::UnifyFaces(),
                           BOPTest_Objects::Angular());

  // Store the history of operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(pBuilder->History());

  if (pBuilder->HasWarnings())
  {
    Standard_SStream aSStream;
    pBuilder->DumpWarnings(aSStream);
    di << aSStream;
  }
  //
  if (pBuilder->HasErrors())
  {
    Standard_SStream aSStream;
    pBuilder->DumpErrors(aSStream);
    di << aSStream;
    return 0;
  }
  //
  const TopoDS_Shape& aR = pBuilder->Shape();
  if (aR.IsNull())
  {
    di << "Result is a null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=================================================================================================

Standard_Integer bapibuild(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  Standard_Boolean        bRunParallel, bNonDestructive;
  Standard_Integer        iErr;
  Standard_Real           aFuzzyValue;
  BRepAlgoAPI_BuilderAlgo aBuilder;
  //
  TopTools_ListOfShape aLS = BOPTest_Objects::Shapes();
  TopTools_ListOfShape aLT = BOPTest_Objects::Tools();
  //
  aLS.Append(aLT);
  bRunParallel           = BOPTest_Objects::RunParallel();
  aFuzzyValue            = BOPTest_Objects::FuzzyValue();
  bNonDestructive        = BOPTest_Objects::NonDestructive();
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  aBuilder.SetArguments(aLS);
  aBuilder.SetRunParallel(bRunParallel);
  aBuilder.SetFuzzyValue(aFuzzyValue);
  aBuilder.SetNonDestructive(bNonDestructive);
  aBuilder.SetGlue(aGlue);
  aBuilder.SetCheckInverted(BOPTest_Objects::CheckInverted());
  aBuilder.SetUseOBB(BOPTest_Objects::UseOBB());
  aBuilder.SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());
  //
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
  aBuilder.Build(aProgress->Start());
  aBuilder.SimplifyResult(BOPTest_Objects::UnifyEdges(),
                          BOPTest_Objects::UnifyFaces(),
                          BOPTest_Objects::Angular());

  // Store the history of operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(aBuilder.History());

  if (aBuilder.HasWarnings())
  {
    Standard_SStream aSStream;
    aBuilder.DumpWarnings(aSStream);
    di << aSStream;
  }
  //
  iErr = aBuilder.HasErrors();
  if (iErr)
  {
    Standard_SStream aSStream;
    aBuilder.DumpErrors(aSStream);
    di << aSStream;
    return 0;
  }
  //
  const TopoDS_Shape& aR = aBuilder.Shape();
  if (aR.IsNull())
  {
    di << "Result is a null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=================================================================================================

Standard_Integer bapisplit(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2)
  {
    di.PrintHelp(a[0]);
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
  aSplitter.SetCheckInverted(BOPTest_Objects::CheckInverted());
  aSplitter.SetUseOBB(BOPTest_Objects::UseOBB());
  aSplitter.SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());
  //
  // performing operation
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(di, 1);
  aSplitter.Build(aProgress->Start());
  aSplitter.SimplifyResult(BOPTest_Objects::UnifyEdges(),
                           BOPTest_Objects::UnifyFaces(),
                           BOPTest_Objects::Angular());

  // Store the history of operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(aSplitter.History());

  // check warning status
  if (aSplitter.HasWarnings())
  {
    Standard_SStream aSStream;
    aSplitter.DumpWarnings(aSStream);
    di << aSStream;
  }
  // checking error status
  Standard_Integer iErr = aSplitter.HasErrors();
  if (iErr)
  {
    Standard_SStream aSStream;
    aSplitter.DumpErrors(aSStream);
    di << aSStream;
    return 0;
  }
  //
  // getting the result of the operation
  const TopoDS_Shape& aR = aSplitter.Shape();
  if (aR.IsNull())
  {
    di << "Result is a null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}
