// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <BOPTest.ixx>

#include <stdio.h>

#include <NCollection_IncAllocator.hxx>

#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf.hxx>

#include <TopoDS_Shape.hxx>
//
#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_BOP.hxx>
//
#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

//
static Standard_Integer bfillds  (Draw_Interpretor&, Standard_Integer, const char**); 
static Standard_Integer bbuild   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bbop     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bclear   (Draw_Interpretor&, Standard_Integer, const char**);
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
  const char* g = "Partition commands";
  // Commands  
  theCommands.Add("bfillds"  , "use bfillds"           , __FILE__, bfillds  , g);
  theCommands.Add("bbuild"   , "use bbuild r"          , __FILE__, bbuild, g);
  theCommands.Add("bbop"     , "use bbop r op"         , __FILE__, bbop, g);
  theCommands.Add("bclear"   , "use bclear"            , __FILE__, bclear, g);
}

//=======================================================================
//function : bclear
//purpose  : 
//=======================================================================
Standard_Integer bclear(Draw_Interpretor& di, Standard_Integer n, const char** ) 
{
  if (n!=1) {
    di << " use bclear\n";
    return 0;
  }
  //
  BOPTest_Objects::Clear(); 
  return 0;
}
//=======================================================================
//function : bfillds
//purpose  : 
//=======================================================================
Standard_Integer bfillds(Draw_Interpretor& di, Standard_Integer n, const char** ) 
{ 
  if (n!=1) {
    di << " Use bfillds\n";
    return 0;
  }
  //
  char buf[32];
  Standard_Integer aNbS, aNbT, iErr;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_ListOfShape aLC;
  
  BOPCol_ListOfShape& aLS=BOPTest_Objects::Shapes();
  aNbS=aLS.Extent();
  if (!aNbS) {
    di << " no objects to process\n";
    return 0;
  }
  //
  BOPCol_ListOfShape& aLT=BOPTest_Objects::Tools();
  aNbT=aLT.Extent();
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
  //
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  return 0;
}

//=======================================================================
//function : bbuild
//purpose  : 
//=======================================================================
Standard_Integer bbuild(Draw_Interpretor& di, Standard_Integer n, const char** a) 
{ 
  if (n!=2) {
    di << " Use bbuild r\n";
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
  Standard_Integer iErr;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_Builder& aBuilder=BOPTest_Objects::Builder();
  aBuilder.Clear();
  //
  BOPCol_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTool=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTool);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBuilder.AddArgument(aS);
  }
  //
  aBuilder.PerformWithFiller(aPF);
  iErr=aBuilder.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
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
//function : bbop
//purpose  : 
//=======================================================================
Standard_Integer bbop(Draw_Interpretor& di, Standard_Integer n, const char** a) 
{ 
  if (n!=3) {
    di << " use bbop r op\n";
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
  Standard_Integer iErr, iOp;
  BOPAlgo_Operation aOp;
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  iOp=Draw::Atoi(a[2]);
  if (iOp<0 || iOp>4) {
    di << " invalid operation type\n";
  }
  aOp=(BOPAlgo_Operation)iOp;
  //
  BOPAlgo_PaveFiller& aPF=BOPTest_Objects::PaveFiller();
  //
  BOPAlgo_BOP& aBOP=BOPTest_Objects::BOP();
  aBOP.Clear();
  //
  BOPCol_ListOfShape& aLSObj=BOPTest_Objects::Shapes();
  aIt.Initialize(aLSObj);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBOP.AddArgument(aS);
  }
  //
  BOPCol_ListOfShape& aLSTools=BOPTest_Objects::Tools();
  aIt.Initialize(aLSTools);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBOP.AddTool(aS);
  }
  //
  aBOP.SetOperation(aOp);
  //
  aBOP.PerformWithFiller(aPF);
  iErr=aBOP.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  const TopoDS_Shape& aR=aBOP.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}

