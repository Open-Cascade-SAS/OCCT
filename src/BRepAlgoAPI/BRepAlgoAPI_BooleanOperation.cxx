// Created on: 1993-10-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_PDS.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Check.hxx>
#include <BRepLib_FuseEdges.hxx>
#include <BRepTools.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <stdio.h>
///XXXXXXXXXX
//XXXXXXXXXX
//XXXX
//=======================================================================
//class : BRepAlgoAPI_DumpOper
//purpose  : 
//=======================================================================
class BRepAlgoAPI_DumpOper {
 public:
  BRepAlgoAPI_DumpOper() :
    myIsDump(Standard_False),
    myIsDumpArgs(Standard_False),
    myIsDumpRes(Standard_False)  {
      OSD_Environment env("CSF_DEBUG_BOP");
      TCollection_AsciiString pathdump = env.Value();
      myIsDump = (!pathdump.IsEmpty() ? Standard_True: Standard_False);
      myPath=pathdump.ToCString();
  };
  //
  virtual ~BRepAlgoAPI_DumpOper() {
  };
  //
  Standard_Boolean IsDump()const {
    return myIsDump;
  };
  //
  void SetIsDumpArgs(const Standard_Boolean bFlag) {
    myIsDumpArgs=bFlag;
  }
  //
  Standard_Boolean IsDumpArgs()const {
    return myIsDumpArgs;
  };
  //
  void SetIsDumpRes(const Standard_Boolean bFlag) {
    myIsDumpRes=bFlag;
  };
  //
  Standard_Boolean IsDumpRes()const {
    return myIsDumpRes;
  };
  //
  void Dump(
            const TopoDS_Shape& theShape1,
            const TopoDS_Shape& theShape2,
            const TopoDS_Shape& theResult,
            BOPAlgo_Operation theOperation);
  //
 protected:
  Standard_Boolean myIsDump;
  Standard_Boolean myIsDumpArgs;
  Standard_Boolean myIsDumpRes;
  Standard_CString myPath;
};
//XXXX
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation()
:
  BRepAlgoAPI_BuilderAlgo(),  
  myOperation(BOPAlgo_UNKNOWN),
  myBuilderCanWork(Standard_False),
  myFuseEdges(Standard_False)
{ 
  myEntryType=1;
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation
  (const BOPAlgo_PaveFiller& aPF)
:
  BRepAlgoAPI_BuilderAlgo(aPF),  
  myOperation(BOPAlgo_UNKNOWN),
  myBuilderCanWork(Standard_False),
  myFuseEdges(Standard_False)
{ 
  myEntryType=0;
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation
  (const TopoDS_Shape& aS1, 
   const TopoDS_Shape& aS2,
   const BOPAlgo_Operation anOp)
: 
  BRepAlgoAPI_BuilderAlgo(),
  myOperation(anOp),
  myBuilderCanWork(Standard_False),
  myFuseEdges(Standard_False)
{
  myEntryType=1;
  //
  myArguments.Append(aS1);
  myTools.Append(aS2);
}
//=======================================================================
//function : BRepAlgoAPI_BooleanOperation
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::BRepAlgoAPI_BooleanOperation
  (const TopoDS_Shape& aS1, 
   const TopoDS_Shape& aS2,
   const BOPAlgo_PaveFiller& aPF,
   const BOPAlgo_Operation anOp)
: 
  BRepAlgoAPI_BuilderAlgo(aPF),
  myOperation(anOp),
  myBuilderCanWork(Standard_False),
  myFuseEdges(Standard_False)
{ 
  myEntryType=0;
  //
  myArguments.Append(aS1);
  myTools.Append(aS2);
  //
  myDSFiller=(BOPAlgo_PaveFiller*)&aPF;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BRepAlgoAPI_BooleanOperation::~BRepAlgoAPI_BooleanOperation()
{
  Clear();
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BooleanOperation::Clear()
{
  BRepAlgoAPI_BuilderAlgo::Clear();
  //
  myModifFaces.Clear();
  myEdgeMap.Clear();
}
//=======================================================================
//function : SetTools
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BooleanOperation::SetTools
  (const TopTools_ListOfShape& theLS)
{
  myTools=theLS;
}
//=======================================================================
//function : Tools
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::Tools()const
{
  return myTools;
}
//=======================================================================
//function : SetOperation
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BooleanOperation::SetOperation 
  (const BOPAlgo_Operation anOp)
{
  myOperation=anOp;
}
//=======================================================================
//function : Operation
//purpose  : 
//=======================================================================
BOPAlgo_Operation BRepAlgoAPI_BooleanOperation::Operation()const
{
  return myOperation;
}
//=======================================================================
//function : Shape1
//purpose  : 
//=======================================================================
const TopoDS_Shape& BRepAlgoAPI_BooleanOperation::Shape1() const 
{
  return myArguments.First();
}
//=======================================================================
//function : Shape2
//purpose  : 
//=======================================================================
const TopoDS_Shape& BRepAlgoAPI_BooleanOperation::Shape2() const 
{
  return myTools.First();
}
//=======================================================================
//function : BuilderCanWork
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::BuilderCanWork() const
{
  return myBuilderCanWork;
}
//=======================================================================
//function : FuseEdges
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::FuseEdges ()const
{
  return myFuseEdges;
}
//=======================================================================
//function : SetAttributes
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BooleanOperation::SetAttributes()
{
}
//=======================================================================
//function : Build2
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BooleanOperation::Build()
{
  GetReport()->Clear();

  Standard_Integer aNbArgs, aNbTools;
  BRepAlgoAPI_DumpOper aDumpOper;
  //
  myBuilderCanWork=Standard_False;
  NotDone();
  //
  aNbArgs=myArguments.Extent();
  aNbTools=myTools.Extent();
  if (aNbArgs<1 && aNbTools<1) {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  if (myOperation==BOPAlgo_UNKNOWN) {
    AddError (new BOPAlgo_AlertBOPNotSet);
    return;
  }
  //
  //-----------------------------------------------
  TopTools_ListOfShape aLS;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aLS.Append(aS);
  }
  aIt.Initialize(myTools);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aLS.Append(aS);
  }
  //-----------------------------------------------
  //
  if (myEntryType) {
    if (myDSFiller) {
      delete myDSFiller;
    }
    myDSFiller=new BOPAlgo_PaveFiller(myAllocator);
    //
    myDSFiller->SetArguments(aLS);
    //
    myDSFiller->SetRunParallel(myRunParallel);
    myDSFiller->SetProgressIndicator(myProgressIndicator);
    myDSFiller->SetFuzzyValue(myFuzzyValue);
    myDSFiller->SetNonDestructive(myNonDestructive);
    myDSFiller->SetGlue(myGlue);
    //
    SetAttributes();
    //
    myDSFiller->Perform(); 
    //
    GetReport()->Merge (myDSFiller->GetReport());
    if (HasErrors())
    {
      return;
    }
  }// if (myEntryType) {
  //
  //XXXX
  const TopoDS_Shape& aS1 = myArguments.First();
  const TopoDS_Shape& aS2 = myTools.First();
  if (aDumpOper.IsDump()) {
    BRepAlgoAPI_Check aChekArgs(aS1, aS2, myOperation);
    aDumpOper.SetIsDumpArgs(!aChekArgs.IsValid());
  }
  //XXXX
  // 
  if (myBuilder) {
    delete myBuilder;
    myBuilder = NULL;
  }
  //
  BOPAlgo_BOP *pBOP;
  //
  if(myOperation==BOPAlgo_SECTION) {
    myBuilder=new BOPAlgo_Section(myAllocator);
    myBuilder->SetArguments(aLS);
  }
  else{
    pBOP=new BOPAlgo_BOP(myAllocator); 
    pBOP->SetArguments(myArguments);
    pBOP->SetTools(myTools); 
    pBOP->SetOperation(myOperation);
    myBuilder=pBOP;
  }
  //
  myBuilder->SetRunParallel(myRunParallel);
  myBuilder->SetProgressIndicator(myProgressIndicator);
  //
  myBuilder->PerformWithFiller(*myDSFiller);
  //
  GetReport()->Merge (myBuilder->GetReport());
  if (HasErrors())
  {
    return;
  }
  //
  myShape=myBuilder->Shape();
  //
  myBuilderCanWork=Standard_True;
  Done(); 
  //
  //XXXX
  if (aDumpOper.IsDump()) {
    BRepAlgoAPI_Check aCheckRes(myShape);
    aDumpOper.SetIsDumpRes(!aCheckRes.IsValid());
    aDumpOper.Dump(aS1, aS2, myShape,myOperation);
  }
  //XXXX
}
//=======================================================================
//function : RefineEdges
//purpose  : 
//=======================================================================
void BRepAlgoAPI_BooleanOperation::RefineEdges ()
{
  if(myFuseEdges) { 
    return; //Edges have been refined
  }
  //
  TopTools_IndexedMapOfShape mapOldEdges;
  TopTools_ListOfShape aLS;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aLS.Append(aS);
  }
  aIt.Initialize(myTools);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aLS.Append(aS);
  }
  //
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    TopExp::MapShapes (aS, TopAbs_EDGE, mapOldEdges);
  }
  //----------------------------------------------
  BRepLib_FuseEdges FE(myShape);
  FE.SetConcatBSpl(Standard_True);
  FE.AvoidEdges (mapOldEdges);
  //
  // Get List of edges that have been fused
  myFuseEdges = Standard_False;
  myModifFaces.Clear();
  myEdgeMap.Clear();
  TopTools_DataMapOfIntegerListOfShape aFusedEdges;

  FE.Edges(aFusedEdges);
  Standard_Integer nle = aFusedEdges.Extent();
  if (nle != 0) {
    FE.Perform();
    myShape = FE.Shape();

    TopTools_DataMapOfIntegerShape aResultEdges;

    FE.ResultEdges(aResultEdges);
    FE.Faces(myModifFaces);
    myFuseEdges = Standard_True;
    
    Standard_Integer i;
    for(i = 1; i <= nle; ++i) {
      const TopoDS_Shape& aNewE = aResultEdges(i);
      const TopTools_ListOfShape& aListOfOldEdges = aFusedEdges(i);
      TopTools_ListIteratorOfListOfShape anIter(aListOfOldEdges);
      for(; anIter.More(); anIter.Next()) {
        myEdgeMap.Bind(anIter.Value(), aNewE);
      }
    }
  }
}
//=======================================================================
//function : RefinedList
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::RefinedList
  (const TopTools_ListOfShape& theL) 
{
  myGenerated.Clear();
  TopTools_MapOfShape aMap;

  TopTools_ListIteratorOfListOfShape anIter(theL);

  for(; anIter.More(); anIter.Next()) {
    const TopoDS_Shape& anS = anIter.Value();

    if(anS.ShapeType() == TopAbs_EDGE) {
      if(myEdgeMap.IsBound(anS)) {
        const TopoDS_Shape& aNewEdge = myEdgeMap.Find(anS);
        if(aMap.Add(aNewEdge)) {
          myGenerated.Append(aNewEdge);
        }
      }
      else {
        myGenerated.Append(anS);
      }
    }
    else if (anS.ShapeType() == TopAbs_FACE) {
      if(myModifFaces.IsBound(anS)) {
        myGenerated.Append(myModifFaces.Find(anS));
      }
      else {
        myGenerated.Append(anS);
      }
    }
    else {
      myGenerated.Append(anS);
    }
  }

  return myGenerated;

}
//=======================================================================
//function : SectionEdges
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::SectionEdges()
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  //
  Standard_Integer aNb, i, j, aNbCurves, nE;
  BOPDS_ListIteratorOfListOfPaveBlock anIt;
  //
  const BOPDS_PDS& pDS = myDSFiller->PDS();
  BOPDS_VectorOfInterfFF& aFFs=pDS->InterfFF();
  myGenerated.Clear();
  //
  aNb=aFFs.Extent();
  for (i = 0; i < aNb; i++) {
    BOPDS_InterfFF& aFFi=aFFs(i);
    const BOPDS_VectorOfCurve& aSeqOfCurve=aFFi.Curves();
    //
    aNbCurves=aSeqOfCurve.Extent();
    for (j=0; j<aNbCurves; j++) {
      const BOPDS_Curve& aCurve=aSeqOfCurve(j);
      const BOPDS_ListOfPaveBlock& aSectEdges = aCurve.PaveBlocks();
      //
      anIt.Initialize(aSectEdges);
      for(; anIt.More(); anIt.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB = anIt.Value();
        nE = aPB->Edge();
        const TopoDS_Shape& aE = pDS->Shape(nE);
        myGenerated.Append(aE);
      }
    }
  }
  //
  if(myFuseEdges) {
    TopTools_ListOfShape theLS;
    theLS.Assign(myGenerated);
    //
    RefinedList(theLS);
  }
  //
  return myGenerated;
}
//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::Generated
  (const TopoDS_Shape& S) 
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  //
  if(myFuseEdges) {
    const TopTools_ListOfShape& aL = myBuilder->Generated(S);
    return RefinedList(aL);
  }
  
  return myBuilder->Generated(S);
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BRepAlgoAPI_BooleanOperation::Modified
  (const TopoDS_Shape& aS) 
{
  if (myBuilder==NULL) {
    myGenerated.Clear();
    return myGenerated;
  }
  else {
    myGenerated = myBuilder->Modified(aS);

    if(myFuseEdges) {
      TopTools_ListOfShape theLS;
      theLS.Assign(myGenerated);
      //
      RefinedList(theLS);
    }
    return myGenerated;
  }
}
//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::IsDeleted
  (const TopoDS_Shape& aS) 
{
  Standard_Boolean bDeleted = Standard_True; 
  if (myBuilder != NULL) {
    bDeleted=myBuilder->IsDeleted(aS);
  }
  return bDeleted; 
}
//=======================================================================
//function : HasModified
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::HasModified() const
{
  if (myBuilder==NULL) {
    return Standard_False;
  }
  return myBuilder->HasModified();
}
//=======================================================================
//function : HasGenerated
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::HasGenerated() const
{
  if (myBuilder==NULL) {
    return Standard_False;
  }
  return myBuilder->HasGenerated();
}
//=======================================================================
//function : HasDeleted
//purpose  : 
//=======================================================================
Standard_Boolean BRepAlgoAPI_BooleanOperation::HasDeleted() const
{
  if (myBuilder==NULL) {
    return Standard_False;
  }
  return myBuilder->HasDeleted();
}
//XXXX
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void BRepAlgoAPI_DumpOper::Dump (const TopoDS_Shape& theShape1,
                                 const TopoDS_Shape& theShape2,
                                 const TopoDS_Shape& theResult,
                                 BOPAlgo_Operation theOperation)
{
  if (!(myIsDumpArgs && myIsDumpRes)) {
    return;
  }
  //
  TCollection_AsciiString aPath(myPath);
  aPath += "/";
  Standard_Integer aNumOper = 1;
  Standard_Boolean isExist = Standard_True;
  TCollection_AsciiString aFileName;
 
  while(isExist)
  {
    aFileName = aPath + "BO_" + TCollection_AsciiString(aNumOper) +".tcl";
    OSD_File aScript(aFileName);
    isExist = aScript.Exists();
    if(isExist)
      aNumOper++;
  }

  FILE* afile = fopen(aFileName.ToCString(), "w+");
  if(!afile)
    return;
  if(myIsDumpArgs)
    fprintf(afile,"%s\n","# Arguments are invalid");

  TCollection_AsciiString aName1;
  TCollection_AsciiString aName2;
  TCollection_AsciiString aNameRes;
  if(!theShape1.IsNull())
  {
    aName1 = aPath +
      "Arg1_" + TCollection_AsciiString(aNumOper) + ".brep";
    BRepTools::Write(theShape1, aName1.ToCString());
  }
  else
    fprintf(afile,"%s\n","# First argument is Null ");
   
  if(!theShape2.IsNull())
  {
    aName2 =  aPath +
      "Arg2_"+ TCollection_AsciiString(aNumOper) + ".brep";

    BRepTools::Write(theShape2, aName2.ToCString());
  }
  else
    fprintf(afile,"%s\n","# Second argument is Null ");
   
   if(!theResult.IsNull())
  {
    aNameRes =  aPath +
      "Result_"+ TCollection_AsciiString(aNumOper) + ".brep";

    BRepTools::Write(theResult, aNameRes.ToCString());
  }
  else
    fprintf(afile,"%s\n","# Result is Null ");
  
  fprintf(afile, "%s %s %s\n","restore",  aName1.ToCString(), "arg1");
  fprintf(afile, "%s %s %s\n","restore",  aName2.ToCString(), "arg2");;
  TCollection_AsciiString aBopString;
  switch (theOperation)
  {
    case BOPAlgo_COMMON : aBopString += "bcommon Res "; break;
    case BOPAlgo_FUSE   : aBopString += "bfuse Res "; break;
    case BOPAlgo_CUT    : 
    case BOPAlgo_CUT21  : aBopString += "bcut Res "; break;
    case BOPAlgo_SECTION : aBopString += "bsection Res "; break;
    default : break;
  };
  aBopString += ("arg1 arg2");
  if(theOperation == BOPAlgo_CUT21)
    aBopString += " 1";

  fprintf(afile, "%s\n",aBopString.ToCString());
  fclose(afile);
}
