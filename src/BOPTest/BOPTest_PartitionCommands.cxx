// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>
#include <BOPAlgo_Splitter.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTest.hxx>
#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>
#include <BRepTest_Objects.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf.hxx>
#include <OSD_Timer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>

#include <stdio.h>
#include <string.h>
//
//
static Standard_Integer bfillds  (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bbuild   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bbop     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bsplit   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer buildbop (Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : PartitionCommands
//purpose  : 
//=======================================================================
void BOPTest::PartitionCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands  
  theCommands.Add("bfillds", "use bfillds [-t]"  , __FILE__, bfillds, g);
  theCommands.Add("bbuild" , "use bbuild r [-t]" , __FILE__, bbuild, g);
  theCommands.Add("bbop"   , "use bbop r op [-t]", __FILE__, bbop, g);
  theCommands.Add("bsplit" , "use bsplit r [-t]" , __FILE__, bsplit, g);

  theCommands.Add("buildbop", "Builds the result of BOP basing on the GF.\n"
                  " The command uses classification approach for building the result of BOP\n"
                  " (thus it operates on solids only and can be used on open solids):\n"
                  "  - FUSE is built from the faces OUT of all arguments\n"
                  "  - COMMON is built from the faces IN any of the object/tools\n"
                  "  - CUT is built from the objects faces OUT of the tools and tools faces IN the objects.\n"
                  " Please note that history for solids will not be available.\n\n"
                  " Usage: buildbop result -o s1 [s2 ...] -t s3 [s4 ...] -op operation (common/fuse/cut/tuc)\n"
                  " Where:\n"
                  " result      - result shape of the operation\n"
                  " s1 s2 s3 s4 - arguments (solids) of the GF operation\n"
                  " operation   - type of boolean operation",
                  __FILE__, buildbop, g);
}
//=======================================================================
//function : bfillds
//purpose  : 
//=======================================================================
Standard_Integer bfillds(Draw_Interpretor& di, 
                         Standard_Integer n, 
                         const char** a) 
{ 
  if (n > 2) {
    di << " use bfillds [-t]\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Boolean bRunParallel, bNonDestructive, bShowTime;
  Standard_Integer i, aNbS;
  Standard_Real aTol;
  TopTools_ListIteratorOfListOfShape aIt;
  TopTools_ListOfShape aLC;
  TopTools_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aNbS=aLS.Extent();
  if (!aNbS) {
    di << " no objects to process\n";
    return 0;
  }
  //
  bShowTime = Standard_False;
  //
  bRunParallel=BOPTest_Objects::RunParallel();
  bNonDestructive = BOPTest_Objects::NonDestructive();
  aTol = BOPTest_Objects::FuzzyValue();
  BOPAlgo_GlueEnum aGlue = BOPTest_Objects::Glue();
  //
  for (i=1; i<n; ++i) {
    if (!strcmp(a[i], "-t")) {
      bShowTime=Standard_True;
    }
  }
  //
  TopTools_ListOfShape& aLT=BOPTest_Objects::Tools();
  //
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aLC.Append(aS);
  }
  //
  aIt.Initialize(aLT);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
     aLC.Append(aS);
  }
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  aPF.SetArguments(aLC);
  aPF.SetRunParallel(bRunParallel);
  aPF.SetNonDestructive(bNonDestructive);
  aPF.SetFuzzyValue(aTol);
  aPF.SetGlue(aGlue);
  aPF.SetUseOBB(BOPTest_Objects::UseOBB());
  //
  OSD_Timer aTimer;
  aTimer.Start();
  //
  aPF.Perform();
  BOPTest::ReportAlerts(aPF.GetReport());
  if (aPF.HasErrors()) {
    return 0;
  }
  //
  aTimer.Stop();
  //
  if (bShowTime)
  {
    Sprintf(buf, "  Tps: %7.2lf\n", aTimer.ElapsedTime());
    di << buf;
  }
  //
  return 0;
}
//=======================================================================
//function : bbuild
//purpose  : 
//=======================================================================
Standard_Integer bbuild(Draw_Interpretor& di,
                        Standard_Integer n, 
                        const char** a) 
{ 
  if (n<2) {
    di << " use bbuild r [-t]\n";
    return 0;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[128];
  Standard_Boolean bRunParallel, bShowTime;
  Standard_Integer i;

  TopTools_ListIteratorOfListOfShape aIt;
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPTest_Objects::SetBuilderDefault();
  BOPAlgo_Builder& aBuilder=BOPTest_Objects::Builder();
  aBuilder.Clear();
  //
  TopTools_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  TopTools_ListOfShape& aLSTool=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTool);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  bShowTime=Standard_False;
  bRunParallel=BOPTest_Objects::RunParallel();
  for (i=2; i<n; ++i) {
    if (!strcmp(a[i], "-t")) {
      bShowTime=Standard_True;
    }
  }
  aBuilder.SetRunParallel(bRunParallel);
  aBuilder.SetCheckInverted(BOPTest_Objects::CheckInverted());
  aBuilder.SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());
  //
  //
  OSD_Timer aTimer;
  aTimer.Start();
  //
  aBuilder.PerformWithFiller(aPF); 
  BOPTest::ReportAlerts(aBuilder.GetReport());

  // Set history of GF operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(aPF.Arguments(), aBuilder);

  if (aBuilder.HasErrors()) {
    return 0;
  }
  //
  aTimer.Stop();
  //
  if (bShowTime)
  {
    Sprintf(buf, "  Tps: %7.2lf\n", aTimer.ElapsedTime());
    di << buf;
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
//function : bbop
//purpose  : 
//=======================================================================
Standard_Integer bbop(Draw_Interpretor& di, 
                      Standard_Integer n, 
                      const char** a) 
{ 
  if (n<3) {
    di << " use bbop r op [-t]\n";
    return 0;
  }
  //
  BOPDS_PDS pDS=BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Boolean bRunParallel, bShowTime;
  Standard_Integer iOp, i;
  BOPAlgo_Operation aOp;
  TopTools_ListIteratorOfListOfShape aIt; 
  //
  iOp=Draw::Atoi(a[2]);
  if (iOp<0 || iOp>4) {
    di << " invalid operation type\n";
    return 0;
  }
  aOp=(BOPAlgo_Operation)iOp;
  //
  bShowTime=Standard_False;
  bRunParallel=BOPTest_Objects::RunParallel();
  for (i=3; i<n; ++i) {
    if (!strcmp(a[i], "-t")) {
      bShowTime=Standard_True;
    }
  }
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_Builder *pBuilder=NULL;
  
  if (aOp!=BOPAlgo_SECTION) { 
    pBuilder=&BOPTest_Objects::BOP();
  } 
  else {
    pBuilder=&BOPTest_Objects::Section();
  }
  //
  pBuilder->Clear();
  //
  TopTools_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    pBuilder->AddArgument(aS);
  }
  //
  if (aOp!=BOPAlgo_SECTION) {
    BOPAlgo_BOP *pBOP=(BOPAlgo_BOP *)pBuilder;
    //
    TopTools_ListOfShape& aLSTools=BOPTest_Objects::Tools();
    aIt.Initialize(aLSTools);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS=aIt.Value();
      pBOP->AddTool(aS);
    }
    //
    pBOP->SetOperation(aOp);
  }
  else {
    TopTools_ListOfShape& aLSTools=BOPTest_Objects::Tools();
    aIt.Initialize(aLSTools);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS=aIt.Value();
      pBuilder->AddArgument(aS);
    }
  }
  //
  pBuilder->SetRunParallel(bRunParallel);
  pBuilder->SetCheckInverted(BOPTest_Objects::CheckInverted());
  pBuilder->SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());
  //
  OSD_Timer aTimer;
  aTimer.Start();
  //
  pBuilder->PerformWithFiller(aPF);
  BOPTest::ReportAlerts(pBuilder->GetReport());

  // Set history of Boolean operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(aPF.Arguments(), *pBuilder);

  if (pBuilder->HasErrors()) {
    return 0;
  }
  //
  aTimer.Stop();
  //
  if (bShowTime) {
    Sprintf(buf, "  Tps: %7.2lf\n", aTimer.ElapsedTime());
    di << buf;
  }
  //
  const TopoDS_Shape& aR=pBuilder->Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  BOPTest_Objects::SetBuilder(pBuilder);
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bsplit
//purpose  : 
//=======================================================================
Standard_Integer bsplit(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a)
{ 
  if (n < 2) {
    di << " use bsplit r [-t (show time)]\n";
    return 1;
  }
  //
  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  BOPAlgo_PaveFiller& aPF = BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_Splitter* pSplitter = &BOPTest_Objects::Splitter();
  pSplitter->Clear();
  //
  // set objects
  const TopTools_ListOfShape& aLSObjects = BOPTest_Objects::Shapes();
  pSplitter->SetArguments(aLSObjects);
  //
  // set tools
  TopTools_ListOfShape& aLSTools = BOPTest_Objects::Tools();
  pSplitter->SetTools(aLSTools);
  //
  // set options
  pSplitter->SetRunParallel(BOPTest_Objects::RunParallel());
  pSplitter->SetNonDestructive(BOPTest_Objects::NonDestructive());
  pSplitter->SetFuzzyValue(BOPTest_Objects::FuzzyValue());
  pSplitter->SetCheckInverted(BOPTest_Objects::CheckInverted());
  pSplitter->SetToFillHistory(BRepTest_Objects::IsHistoryNeeded());
  //
  // measure the time of the operation
  OSD_Timer aTimer;
  aTimer.Start();
  //
  // perform the operation
  pSplitter->PerformWithFiller(aPF);
  //
  aTimer.Stop();
  BOPTest::ReportAlerts(pSplitter->GetReport());

  // Set history of Split operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(aPF.Arguments(), *pSplitter);

  if (pSplitter->HasErrors()) {
    return 0;
  }
  //
  // show time if necessary
  if (n == 3 && !strcmp(a[2], "-t")) {
    char buf[20];
    Sprintf(buf, "  Tps: %7.2lf\n", aTimer.ElapsedTime());
    di << buf;
  }
  //
  // Debug commands support
  BOPTest_Objects::SetBuilder(pSplitter);
  //
  const TopoDS_Shape& aR = pSplitter->Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : buildbop
//purpose  : 
//=======================================================================
Standard_Integer buildbop(Draw_Interpretor& di,
                          Standard_Integer n,
                          const char** a)
{
  if (n < 3)
  {
    di.PrintHelp(a[0]);
    return 1;
  }

  BOPDS_PDS pDS = BOPTest_Objects::PDS();
  if (!pDS)
  {
    di << "Error: perform intersection of arguments first";
    return 1;
  }

  BOPAlgo_Builder *pBuilder = &BOPTest_Objects::Builder();
  if (pBuilder->HasErrors())
  {
    di << "Error: there were problems during GF";
    return 0;
  }

  if (pBuilder->Arguments().IsEmpty() ||
      pBuilder->Shape().IsNull())
  {
    di << "Error: it seems the GF has not been yet performed";
    return 1;
  }

  // Get arguments and operation
  TopTools_ListOfShape aLObjects, aLTools;
  BOPAlgo_Operation anOp = BOPAlgo_UNKNOWN;

  for (Standard_Integer i = 2; i < n; ++i)
  {
    if (!strcmp(a[i], "-o") || !strcmp(a[i], "-t"))
    {
      if (i == (n - 1))
      {
        di << "Error: shapes are expected after the key " << a[i];
        return 1;
      }

      TopTools_ListOfShape& aList = !strcmp(a[i], "-o") ? aLObjects : aLTools;
      Standard_Integer j = i + 1;
      for (; j < n; ++j)
      {
        if (a[j][0] == '-')
        {
          // reached the following key
          i = j - 1;
          break;
        }
        else
        {
          // Get the shape
          TopoDS_Shape aS = DBRep::Get(a[j]);
          if (aS.IsNull())
          {
            di << "Error: " << a[j] << " is a null shape";
            return 1;
          }
          if (aS.ShapeType() != TopAbs_SOLID)
          {
            di << "Error: " << a[j] << " is not a solid";
            return 1;
          }
          if (pDS->Index(aS) < 0)
          {
            di << "Error: " << a[j] << " is not an argument of GF";
            return 1;
          }
          aList.Append(aS);
        }
      }
      // End of arguments is reached
      if (j == n) break;
    }
    else if (!strcmp(a[i], "-op"))
    {
      if (i == (n - 1))
      {
        di << "Error: operation type is expected after the key " << a[i];
        return 1;
      }

      ++i;
      if (!strcasecmp(a[i], "common"))
        anOp = BOPAlgo_COMMON;
      else if (!strcasecmp(a[i], "fuse"))
        anOp = BOPAlgo_FUSE;
      else if (!strcasecmp(a[i], "cut"))
        anOp = BOPAlgo_CUT;
      else if (!strcasecmp(a[i], "tuc"))
        anOp = BOPAlgo_CUT21;
      else
      {
        di << "Error: unknown operation type";
        return 1;
      }
    }
    else
    {
      di << "Error: " << a[i] << " invalid key";
      return 1;
    }
  }

  if (anOp == BOPAlgo_UNKNOWN)
  {
    di << "Error: operation has not been specified";
    return 1;
  }

  Standard_Boolean hasObjects = !aLObjects.IsEmpty();
  Standard_Boolean hasTools   = !aLTools.IsEmpty();
  if (!hasObjects && !hasTools)
  {
    di << "Error: no shapes are given";
    return 1;
  }

  // Create new report for the operation
  Handle(Message_Report) aReport = new Message_Report;

  // Build specific operation
  pBuilder->BuildBOP(aLObjects, aLTools, anOp, aReport);

  // Report alerts of the operation
  BOPTest::ReportAlerts(aReport);

  if (!aReport->GetAlerts(Message_Fail).IsEmpty())
  {
    return 0;
  }

  // Set history of Split operation into the session
  if (BRepTest_Objects::IsHistoryNeeded())
    BRepTest_Objects::SetHistory(pDS->Arguments(), *pBuilder);

  // Result shape
  const TopoDS_Shape& aR = pBuilder->Shape();
  // Draw result shape
  DBRep::Set(a[1], aR);

  return 0;
}
