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
  char buf[128];
  Standard_Boolean bRunParallel;
  Standard_Integer iErr, iOp;
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
    printf(" invalid operation type\n");
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
  //
  pBuilder->Build(); 
  iErr=pBuilder->ErrorStatus();
  if (iErr) {
    Sprintf(buf, " error: %d\n",  iErr);
    di << buf;
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
  char buf[128];
  Standard_Boolean bRunParallel;
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
  //
  aBuilder.SetArguments(aLS);
  aBuilder.SetRunParallel(bRunParallel);
  aBuilder.SetFuzzyValue(aFuzzyValue);
  //
  aBuilder.Build(); 
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
  
