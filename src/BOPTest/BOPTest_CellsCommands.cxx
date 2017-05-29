// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <BOPTest.hxx>
#include <BOPTest_Objects.hxx>

#include <Draw.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>

#include <BOPDS_DS.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_CellsBuilder.hxx>


static Standard_Integer bcbuild (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcaddall (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcremoveall (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcadd (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcremove (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcremoveint (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcmakecontainers (Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : CellsCommands
//purpose  : 
//=======================================================================
void BOPTest::CellsCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands
  
  theCommands.Add("bcbuild", "Cells builder. Use: bcbuild r",
                  __FILE__, bcbuild, g);
  theCommands.Add("bcaddall", "Add all parts to result. Use: bcaddall r [-m material [-u]]",
                  __FILE__, bcaddall, g);
  theCommands.Add("bcremoveall", "Remove all parts from result. Use: bcremoveall",
                  __FILE__, bcremoveall, g);
  theCommands.Add("bcadd", "Add parts to result. Use: bcadd r s1 (0,1) s2 (0,1) ... [-m material [-u]]",
                  __FILE__, bcadd, g);
  theCommands.Add("bcremove", "Remove parts from result. Use: bcremove r s1 (0,1) s2 (0,1) ...", 
                  __FILE__, bcremove, g);
  theCommands.Add("bcremoveint", "Remove internal boundaries. Use: bcremoveint r",
                  __FILE__, bcremoveint, g);
  theCommands.Add("bcmakecontainers", "Make containers from the parts added to result. Use: bcmakecontainers r",
                  __FILE__, bcmakecontainers, g);
}

//=======================================================================
//function : bcbuild
//purpose  : 
//=======================================================================
Standard_Integer bcbuild(Draw_Interpretor& di,
                         Standard_Integer n, 
                         const char** a)
{
  if (n != 2) {
    di << "Cells builder. Use: bcbuild r\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 1;
  }
  //
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  BOPAlgo_PaveFiller& aPF = BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  aCBuilder.Clear();
  //
  BOPCol_ListOfShape& aLSObj = BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aCBuilder.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTool = BOPTest_Objects::Tools();
  aIt.Initialize(aLSTool);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aCBuilder.AddArgument(aS);
  }
  //
  // set the options to the algorithm
  Standard_Boolean bRunParallel = BOPTest_Objects::RunParallel();
  Standard_Real aTol = BOPTest_Objects::FuzzyValue();
  Standard_Boolean bNonDestructive = BOPTest_Objects::NonDestructive();
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  aCBuilder.SetRunParallel(bRunParallel);
  aCBuilder.SetFuzzyValue(aTol);
  aCBuilder.SetNonDestructive(bNonDestructive);
  aCBuilder.SetGlue(aGlue);
  //
  aCBuilder.PerformWithFiller(aPF); 
  BOPTest::ReportAlerts(aCBuilder);
  if (aCBuilder.HasErrors()) {
    return 0;
  }
  //
  BOPTest_Objects::SetBuilder(&aCBuilder);
  //
  const TopoDS_Shape& aR = aCBuilder.GetAllParts();
  if (aR.IsNull()) {
    di << "no parts were built\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bcaddall
//purpose  : 
//=======================================================================
Standard_Integer bcaddall(Draw_Interpretor& di,
                          Standard_Integer n,
                          const char** a)
{
  if (n < 2 || n > 5) {
    di << "Add all parts to result. Use: bcaddall r [-m material [-u]]\n";
    return 1;
  }
  //
  Standard_Integer iMaterial = 0;
  Standard_Boolean bUpdate = Standard_False;
  //
  if (n > 3) {
    if (!strcmp(a[2], "-m")) {
      iMaterial = Draw::Atoi(a[3]);
    }
    //
    if (n == 5) {
      bUpdate = !strcmp(a[4], "-u");
    }
  }
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  //
  aCBuilder.ClearWarnings();
  aCBuilder.AddAllToResult(iMaterial, bUpdate);
  BOPTest::ReportAlerts(aCBuilder);
  //
  const TopoDS_Shape& aR = aCBuilder.Shape();
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bcremoveall
//purpose  : 
//=======================================================================
Standard_Integer bcremoveall(Draw_Interpretor& di,
                             Standard_Integer n, 
                             const char**)
{
  if (n != 1) {
    di << "Remove all parts from result. Use: bcremoveall\n";
    return 1;
  }
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  //
  aCBuilder.RemoveAllFromResult();
  //
  return 0;
}

//=======================================================================
//function : bcadd
//purpose  : 
//=======================================================================
Standard_Integer bcadd(Draw_Interpretor& di,
                       Standard_Integer n, 
                       const char** a)
{
  if (n < 4) {
    di << "Add parts to result. Use: bcadd r s1 (0,1) s2 (0,1) ... [-m material [-u]]\n";
    return 1;
  }
  //
  BOPCol_ListOfShape aLSToTake, aLSToAvoid;
  Standard_Integer i, iMaterial, iTake, n1;
  Standard_Boolean bUpdate;
  //
  iMaterial = 0;
  bUpdate = Standard_False;
  n1 = n;
  //
  if (!strcmp(a[n-3], "-m")) {
    iMaterial = Draw::Atoi(a[n-2]);
    bUpdate = !strcmp(a[n-1], "-u");
    n1 = n - 3;
  }
  else if (!strcmp(a[n-2], "-m")) {
    iMaterial = Draw::Atoi(a[n-1]);
    n1 = n - 2;
  }
  //
  for (i = 2; i < n1; i += 2) {
    const TopoDS_Shape& aS = DBRep::Get(a[i]);
    if (aS.IsNull()) {
      di << a[i] << " is a null shape\n";
      continue;
    }
    iTake = Draw::Atoi(a[i+1]);
    //
    if (iTake) {
      aLSToTake.Append(aS);
    }
    else {
      aLSToAvoid.Append(aS);
    }
  }
  //
  if (aLSToTake.IsEmpty()) {
    di << "No shapes from which to add the parts\n";
    return 1;
  }
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  //
  aCBuilder.ClearWarnings();
  aCBuilder.AddToResult(aLSToTake, aLSToAvoid, iMaterial, bUpdate);
  BOPTest::ReportAlerts(aCBuilder);
  //
  const TopoDS_Shape& aR = aCBuilder.Shape();
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bcremove
//purpose  : 
//=======================================================================
Standard_Integer bcremove(Draw_Interpretor& di,
                          Standard_Integer n, 
                          const char** a)
{
  if (n < 4 || ((n % 2) != 0)) {
    di << "Remove parts from result. Use: bcremove r s1 (0,1) s2 (0,1) ...\n";
    return 1;
  }
  //
  BOPCol_ListOfShape aLSToTake, aLSToAvoid;
  Standard_Integer i, iTake;
  //
  for (i = 2; i < n; i += 2) {
    const TopoDS_Shape& aS = DBRep::Get(a[i]);
    if (aS.IsNull()) {
      di << a[i] << " is a null shape\n";
      return 1;
    }
    iTake = Draw::Atoi(a[i+1]);
    //
    if (iTake) {
      aLSToTake.Append(aS);
    }
    else {
      aLSToAvoid.Append(aS);
    }
  }
  //
  if (aLSToTake.IsEmpty()) {
    di << "No shapes from which to remove the parts\n";
    return 1;
  }
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  aCBuilder.RemoveFromResult(aLSToTake, aLSToAvoid);
  //
  const TopoDS_Shape& aR = aCBuilder.Shape();
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bcremoveint
//purpose  : 
//=======================================================================
Standard_Integer bcremoveint(Draw_Interpretor& di,
                             Standard_Integer n, 
                             const char** a)
{
  if (n != 2) {
    di << "Remove internal boundaries. Use: bcremoveint r\n";
    return 1;
  }
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  //
  aCBuilder.ClearWarnings();
  aCBuilder.RemoveInternalBoundaries();
  BOPTest::ReportAlerts(aCBuilder);
  //
  const TopoDS_Shape& aR = aCBuilder.Shape();
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bcmakecontainers
//purpose  : 
//=======================================================================
Standard_Integer bcmakecontainers(Draw_Interpretor& di,
                                  Standard_Integer n, 
                                  const char** a)
{
  if (n != 2) {
    di << "Make containers from the parts added to result. Use: bcmakecontainers r\n";
    return 1;
  }
  //
  BOPAlgo_CellsBuilder& aCBuilder = BOPTest_Objects::CellsBuilder();
  aCBuilder.MakeContainers();
  //
  const TopoDS_Shape& aR = aCBuilder.Shape();
  //
  DBRep::Set(a[1], aR);
  return 0;
}
