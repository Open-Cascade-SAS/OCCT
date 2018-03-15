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
#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_IndexedDataMapOfSetShape.hxx>
#include <BOPTools_Set.hxx>
#include <BOPTools_SetMapHasher.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

static
  TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim);
//
static
  void CollectContainers(const TopoDS_Shape& theS,
                         TopTools_ListOfShape& theLSC);
//
static
  void RemoveDuplicates(TopTools_ListOfShape& theContainers);
//
static
  void RemoveDuplicates(TopTools_ListOfShape& theContainers,
                        const TopAbs_ShapeEnum theType);
//
static
  Standard_Integer NbCommonItemsInMap(const TopTools_MapOfShape& theM1,
                                      const TopTools_MapOfShape& theM2);
//
static
  void MapFacesToBuildSolids(const TopoDS_Shape& theSol,
                             TopTools_IndexedDataMapOfShapeListOfShape& theMFS,
                             TopTools_IndexedMapOfShape& theMFI);

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BOP::BOPAlgo_BOP()
: BOPAlgo_ToolsProvider()
{
  Clear();
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BOP::BOPAlgo_BOP(const Handle(NCollection_BaseAllocator)& theAllocator)
: BOPAlgo_ToolsProvider(theAllocator)
{
  Clear();
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_BOP::~BOPAlgo_BOP()
{
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::Clear()
{
  myOperation=BOPAlgo_UNKNOWN;
  myDims[0]=-1;
  myDims[1]=-1;

  BOPAlgo_ToolsProvider::Clear();
}
//=======================================================================
//function : SetOperation
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::SetOperation(const BOPAlgo_Operation theOperation)
{
  myOperation=theOperation;
}
//=======================================================================
//function : Operation
//purpose  : 
//=======================================================================
BOPAlgo_Operation BOPAlgo_BOP::Operation()const
{
  return myOperation;
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::CheckData()
{
  Standard_Integer i, j, iDim, aNbArgs, aNbTools;
  Standard_Boolean bFuse;
  TopTools_ListIteratorOfListOfShape aItLS;
  //
  if (!(myOperation==BOPAlgo_COMMON ||
        myOperation==BOPAlgo_FUSE || 
        myOperation==BOPAlgo_CUT|| 
        myOperation==BOPAlgo_CUT21)) {
    // non-licit operation
    AddError (new BOPAlgo_AlertBOPNotSet);
    return;
  }
  //
  aNbArgs=myArguments.Extent();
  if (!aNbArgs) {
    // invalid number of Arguments
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  aNbTools=myTools.Extent();
  if (!aNbTools) { 
    // invalid number of Tools
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  CheckFiller();
  if (HasErrors()) {
    return;
  }
  //
  bFuse = (myOperation == BOPAlgo_FUSE);
  //
  // The rules for different types of operations are the following:
  // 1. FUSE:   All arguments and tools should have the same dimension;
  // 2. CUT:    The MAXIMAL dimension of the ARGUMENTS should be less
  //            or equal to the MINIMAL dimension of the TOOLS;
  // 3. CUT21:  The MINIMAL dimension of ARGUMENTS should be grater
  //            or equal to the MAXIMAL dimension of the TOOLS;
  // 4. COMMON: The arguments and tools could have any dimensions.
  //
  Standard_Integer iDimMin[2], iDimMax[2];
  Standard_Boolean bHasValid[2] = {Standard_False, Standard_False};
  //
  for (i=0; i<2; ++i) {
    const TopTools_ListOfShape& aLS=(!i)? myArguments : myTools;
    aItLS.Initialize(aLS);
    for (j=0; aItLS.More(); aItLS.Next(), ++j) {
      const TopoDS_Shape& aS=aItLS.Value();
      Standard_Boolean bIsEmpty = BOPTools_AlgoTools3D::IsEmptyShape(aS);
      if (bIsEmpty) {
        AddWarning(new BOPAlgo_AlertEmptyShape (aS));
        continue;
      }
      //
      iDim = BOPTools_AlgoTools::Dimension(aS);
      if (iDim < 0) {
        // non-homogeneous argument
        AddError (new BOPAlgo_AlertBOPNotAllowed);
        return;
      }
      //
      bHasValid[i] = Standard_True;
      //
      if (!j) {
        iDimMin[i] = iDim;
        iDimMax[i] = iDim;
        continue;
      }
      //
      if (iDim < iDimMin[i]) {
        iDimMin[i] = iDim;
      }
      else if (iDim > iDimMax[i]) {
        iDimMax[i] = iDim;
      }
      //
      if (bFuse && (iDimMin[i] != iDimMax[i])) {
        // non-homogeneous argument
        AddError (new BOPAlgo_AlertBOPNotAllowed);
        return;
      }
    }
  }
  //
  if (bHasValid[0] && bHasValid[1]) {
    if (((myOperation == BOPAlgo_FUSE)  && (iDimMax[0] != iDimMax[1])) ||
        ((myOperation == BOPAlgo_CUT)   && (iDimMax[0] >  iDimMin[1])) ||
        ((myOperation == BOPAlgo_CUT21) && (iDimMin[0] <  iDimMax[1])) )
    {
      // non-licit operation for the arguments
      AddError (new BOPAlgo_AlertBOPNotAllowed);
      return;
    }
    myDims[0] = iDimMin[0];
    myDims[1] = iDimMin[1];
  }
}
//=======================================================================
//function : TreatEmtpyShape
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_BOP::TreatEmptyShape()
{
  if (! GetReport()->HasAlert (STANDARD_TYPE(BOPAlgo_AlertEmptyShape)))
  {
    return Standard_False;
  }
  //
  // Find non-empty objects
  TopTools_ListOfShape aLValidObjs;
  TopTools_ListIteratorOfListOfShape aItLS(myArguments);
  for (; aItLS.More(); aItLS.Next()) {
    if (!BOPTools_AlgoTools3D::IsEmptyShape(aItLS.Value())) {
      aLValidObjs.Append(aItLS.Value());
    }
  }
  //
  // Find non-empty tools
  TopTools_ListOfShape aLValidTools;
  aItLS.Initialize(myTools);
  for (; aItLS.More() ; aItLS.Next()) {
    if (!BOPTools_AlgoTools3D::IsEmptyShape(aItLS.Value())) {
      aLValidTools.Append(aItLS.Value());
    }
  }
  //
  Standard_Boolean bHasValidObj  = (aLValidObjs .Extent() > 0);
  Standard_Boolean bHasValidTool = (aLValidTools.Extent() > 0);
  //
  if (bHasValidObj && bHasValidTool) {
    // We need to continue the operation to obtain the result
    return Standard_False;
  }
  //
  if (!bHasValidObj && !bHasValidTool) {
    // All shapes are empty shapes, the result will always be empty shape
    return Standard_True;
  }
  //
  // One of the groups of arguments consists of empty shapes only,
  // so we can build the result of operation right away just by
  // choosing the list of shapes to add to result, depending on
  // the type of the operation
  TopTools_ListOfShape *pLResult = NULL;
  //
  switch (myOperation) {
    case BOPAlgo_FUSE:
      // Add not empty shapes into result
      pLResult = bHasValidObj ? &aLValidObjs : &aLValidTools;
      break;
    case BOPAlgo_CUT:
      // Add objects into result
      pLResult = &aLValidObjs;
      break;
    case BOPAlgo_CUT21:
      // Add tools into result
      pLResult = &aLValidTools;
      break;
    case BOPAlgo_COMMON:
      // Common will be empty
      break;
    default:
      break;
  }
  //
  if (pLResult) {
    aItLS.Initialize(*pLResult);
    for (; aItLS.More(); aItLS.Next()) {
      BRep_Builder().Add(myShape, aItLS.Value());
    }
  }
  return Standard_True;
}
//=======================================================================
//function : BuildResult
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::BuildResult(const TopAbs_ShapeEnum theType)
{
  TopAbs_ShapeEnum aType;
  BRep_Builder aBB;
  TopTools_MapOfShape aM;
  TopTools_ListIteratorOfListOfShape aIt, aItIm;
  //
  const TopTools_ListOfShape& aLA=myDS->Arguments();
  aIt.Initialize(aLA);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aType=aS.ShapeType();
    if (aType==theType) {
      if (myImages.IsBound(aS)){
        const TopTools_ListOfShape& aLSIm=myImages.Find(aS);
        aItIm.Initialize(aLSIm);
        for (; aItIm.More(); aItIm.Next()) {
          const TopoDS_Shape& aSIm=aItIm.Value();
          if (aM.Add(aSIm)) {
            aBB.Add(myShape, aSIm);
          }
        }
      }
      else {
        if (aM.Add(aS)) {
          aBB.Add(myShape, aS);
        }
      }
    }
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::Perform()
{
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPAlgo_PaveFiller* pPF;
  TopTools_ListIteratorOfListOfShape aItLS;
  //
  GetReport()->Clear();
  //
  if (myEntryPoint==1) {
    if (myPaveFiller) {
      delete myPaveFiller;
      myPaveFiller=NULL;
    }
  }
  //
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  TopTools_ListOfShape aLS(aAllocator);
  //
  aItLS.Initialize(myArguments);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aS=aItLS.Value();
    aLS.Append(aS);
  }
  //
  aItLS.Initialize(myTools);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aS=aItLS.Value();
    aLS.Append(aS);
  }
  //
  pPF=new BOPAlgo_PaveFiller(aAllocator);
  pPF->SetArguments(aLS);
  pPF->SetRunParallel(myRunParallel);
  pPF->SetProgressIndicator(myProgressIndicator);
  pPF->SetFuzzyValue(myFuzzyValue);
  pPF->SetNonDestructive(myNonDestructive);
  pPF->SetGlue(myGlue);
  pPF->SetUseOBB(myUseOBB);
  //
  pPF->Perform();
  //
  myEntryPoint=1;
  PerformInternal(*pPF);
}
//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::PerformInternal1(const BOPAlgo_PaveFiller& theFiller)
{
  myPaveFiller=(BOPAlgo_PaveFiller*)&theFiller;
  myDS=myPaveFiller->PDS();
  myContext=myPaveFiller->Context();
  myFuzzyValue = myPaveFiller->FuzzyValue();
  myNonDestructive = myPaveFiller->NonDestructive();
  //
  // 1. CheckData
  CheckData();
  if (HasErrors()) {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (HasErrors()) {
    return;
  }
  //
  if (GetReport()->HasAlert (STANDARD_TYPE(BOPAlgo_AlertEmptyShape)))
  {
    Standard_Boolean bDone = TreatEmptyShape();
    if (bDone) {
      return;
    }
  }
  //
  // 3. Fill Images
  // 3.1 Vertices
  FillImagesVertices();
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_VERTEX);
  if (HasErrors()) {
    return;
  }
  // 3.2 Edges
  FillImagesEdges();
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_EDGE);
  if (HasErrors()) {
    return;
  }
  //
  // 3.3 Wires
  FillImagesContainers(TopAbs_WIRE);
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_WIRE);
  if (HasErrors()) {
    return;
  }
  //
  // 3.4 Faces
  FillImagesFaces();
  if (HasErrors()) {
    return;
  }
  
  BuildResult(TopAbs_FACE);
  if (HasErrors()) {
    return;
  }
  //
  // 3.5 Shells
  FillImagesContainers(TopAbs_SHELL);
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_SHELL);
  if (HasErrors()) {
    return;
  }
  //
  // 3.6 Solids
  FillImagesSolids();
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_SOLID);
  if (HasErrors()) {
    return;
  }
  //
  // 3.7 CompSolids
  FillImagesContainers(TopAbs_COMPSOLID);
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_COMPSOLID);
  if (HasErrors()) {
    return;
  }
  //
  // 3.8 Compounds
  FillImagesCompounds();
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_COMPOUND);
  if (HasErrors()) {
    return;
  }
  //
  // 4.BuildShape;
  BuildShape();
  if (HasErrors()) {
    return;
  }
  // 
  // 5.History
  PrepareHistory();
  //
  // 6 Post-treatment 
  PostTreat();
}
//=======================================================================
//function : BuildRC
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::BuildRC()
{
  TopAbs_ShapeEnum aType;
  TopoDS_Compound aC;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aC);
  //
  // A. Fuse
  if (myOperation == BOPAlgo_FUSE) {
    TopTools_MapOfShape aMFence;
    aType = TypeToExplore(myDims[0]);
    TopExp_Explorer aExp(myShape, aType);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aS = aExp.Current();
      if (aMFence.Add(aS)) {
        aBB.Add(aC, aS);
      }
    }
    myRC = aC;
    return;
  }
  //
  // B. Common, Cut, Cut21
  //
  Standard_Integer i, j, aNb, iDim;
  Standard_Boolean bCheckEdges, bContains, bCut21, bCommon;
  TopTools_ListIteratorOfListOfShape aItLS;
  //
  // prepare the building elements of arguments to get its splits
  TopTools_IndexedMapOfShape aMArgs, aMTools;
  for (i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLS = !i ? myArguments : myTools;
    TopTools_IndexedMapOfShape& aMS = !i ? aMArgs : aMTools;
    aItLS.Initialize(aLS);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      iDim = BOPTools_AlgoTools::Dimension(aS);
      if (iDim < 0) {
        continue;
      }
      aType = TypeToExplore(iDim);
      TopExp::MapShapes(aS, aType, aMS);
    }
  }
  //
  bCheckEdges = Standard_False;
  //
  // get splits of building elements
  TopTools_IndexedMapOfShape aMArgsIm, aMToolsIm;
  BOPTools_IndexedDataMapOfSetShape aMSetArgs, aMSetTools;

  for (i = 0; i < 2; ++i) {
    const TopTools_IndexedMapOfShape& aMS = !i ? aMArgs : aMTools;
    TopTools_IndexedMapOfShape& aMSIm = !i ? aMArgsIm : aMToolsIm;
    BOPTools_IndexedDataMapOfSetShape& aMSet = !i ? aMSetArgs : aMSetTools;
    //
    aNb = aMS.Extent();
    for (j = 1; j <= aNb; ++j) {
      const TopoDS_Shape& aS = aMS(j);
      aType = aS.ShapeType();
      if (aType == TopAbs_EDGE) {
        const TopoDS_Edge& aE = *(TopoDS_Edge*)&aS;
        bCheckEdges = Standard_True;
        if (BRep_Tool::Degenerated(aE)) {
          continue;
        }
      }
      //
      if (myImages.IsBound(aS)) {
        const TopTools_ListOfShape& aLSIm = myImages.Find(aS);
        aItLS.Initialize(aLSIm);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aSIm = aItLS.Value();
          aMSIm.Add(aSIm);
        }
      }
      else {
        aMSIm.Add(aS);
        if (aS.ShapeType() == TopAbs_SOLID) {
          BOPTools_Set aST;
          aST.Add(aS, TopAbs_FACE);
          if (!aMSet.Contains(aST)) {
            aMSet.Add(aST, aS);
          }
        }
      }
    }
  }
  //
  // compare the maps and make the result
  //
  Standard_Integer iDimMin, iDimMax;
  //
  iDimMin = Min(myDims[0], myDims[1]);
  bCommon = (myOperation == BOPAlgo_COMMON);
  bCut21  = (myOperation == BOPAlgo_CUT21);
  //
  const TopTools_IndexedMapOfShape& aMIt = bCut21 ? aMToolsIm : aMArgsIm;
  const TopTools_IndexedMapOfShape& aMCheck = bCut21 ? aMArgsIm : aMToolsIm;
  const BOPTools_IndexedDataMapOfSetShape& aMSetCheck = bCut21 ? aMSetArgs : aMSetTools;
  //
  TopTools_IndexedMapOfShape aMCheckExp, aMItExp;
  //
  if (bCommon) {
    aNb = aMIt.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aS = aMIt(i);
      iDimMax = BOPTools_AlgoTools::Dimension(aS);
      for (iDim = iDimMin; iDim < iDimMax; ++iDim) {
        aType = TypeToExplore(iDim);
        TopExp::MapShapes(aS, aType, aMItExp);
      }
      aMItExp.Add(aS);
    }
  }
  else {
    aMItExp = aMIt;
  }
  //
  aNb = aMCheck.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aS = aMCheck(i);
    iDimMax = BOPTools_AlgoTools::Dimension(aS);
    for (iDim = iDimMin; iDim < iDimMax; ++iDim) {
      aType = TypeToExplore(iDim);
      TopExp::MapShapes(aS, aType, aMCheckExp);
    }
    aMCheckExp.Add(aS);
  }
  //
  aNb = aMItExp.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aS = aMItExp(i);
    //
    bContains = aMCheckExp.Contains(aS);
    if (!bContains && aS.ShapeType() == TopAbs_SOLID) {
      BOPTools_Set aST;
      aST.Add(aS, TopAbs_FACE);
      bContains = aMSetCheck.Contains(aST);
    }
    //
    if (bCommon) {
      if (bContains) {
        aBB.Add(aC, aS);
      }
    }
    else {
      if (!bContains) {
        aBB.Add(aC, aS);
      }
    }
  }
  //
  // filter result for COMMON operation
  if (bCommon) {
    TopTools_MapOfShape aMFence;
    TopExp_Explorer aExp;
    TopoDS_Compound aCx;
    aBB.MakeCompound(aCx);
    //
    for (iDim = 3; iDim >= iDimMin; --iDim) {
      aType = TypeToExplore(iDim);
      aExp.Init(aC, aType);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aS = aExp.Current();
        if (aMFence.Add(aS)) {
          aBB.Add(aCx, aS);
          TopExp::MapShapes(aS, aMFence);
        }
      }
    }
    aC = aCx;
  }
  //
  if (!bCheckEdges) {
    myRC = aC;
    return;
  }
  //
  // The squats around degenerated edges
  Standard_Integer nVD;
  TopTools_IndexedMapOfShape aMVC;
  //
  // 1. Vertices of aC
  TopExp::MapShapes(aC, TopAbs_VERTEX, aMVC);
  //
  // 2. DE candidates
  aNb = myDS->NbSourceShapes();
  for (i = 0; i < aNb; ++i) {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    aType = aSI.ShapeType();
    if (aType != TopAbs_EDGE) {
      continue;
    }
    //
    const TopoDS_Edge& aE = *((TopoDS_Edge*)&aSI.Shape());
    if (!BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    nVD = aSI.SubShapes().First();
    const TopoDS_Shape& aVD = myDS->Shape(nVD);
    //
    if (!aMVC.Contains(aVD)) {
      continue;
    }
    //
    if (myDS->IsNewShape(nVD)) {
      continue;
    }
    //
    if (myDS->HasInterf(nVD)) {
      continue;
    }
    //
    aBB.Add(aC, aE);
  }
  //
  myRC=aC;
}
//=======================================================================
//function : BuildShape
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::BuildShape()
{
  BuildRC();
  //
  if ((myOperation == BOPAlgo_FUSE) && (myDims[0] == 3)) {
    BuildSolid();
    return;
  }
  //
  Standard_Integer i;
  TopAbs_ShapeEnum aType, aT1, aT2;
  TopTools_ListOfShape aLSC, aLCB;
  TopTools_ListIteratorOfListOfShape aItLS, aItLSIm, aItLCB;
  TopoDS_Iterator aIt;
  BRep_Builder aBB;
  TopoDS_Shape aRC, aRCB;
  //
  TopTools_MapOfShape aMSRC;
  TopExp::MapShapes(myRC, aMSRC);
  //
  // collect images of containers
  for (i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLS = !i ? myArguments : myTools;
    //
    aItLS.Initialize(aLS);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      //
      CollectContainers(aS, aLSC);
    }
  }
  // make containers
  TopTools_ListOfShape aLCRes;
  TopTools_MapOfShape aMInpFence;
  aItLS.Initialize(aLSC);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSC = aItLS.Value();
    aMInpFence.Add(aSC);
    //
    BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);
    //
    aIt.Initialize(aSC);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS = aIt.Value();
      if (myImages.IsBound(aS)) {
        const TopTools_ListOfShape& aLSIm = myImages.Find(aS);
        //
        aItLSIm.Initialize(aLSIm);
        for (; aItLSIm.More(); aItLSIm.Next()) {
          const TopoDS_Shape& aSIm = aItLSIm.Value();
          if (aMSRC.Contains(aSIm)) {
            aBB.Add(aRC, aSIm);
          }
        }
      }
      else if (aMSRC.Contains(aS)) {
        aBB.Add(aRC, aS);
      }
    }
    //
    aType = aSC.ShapeType();
    switch (aType) {
      case TopAbs_WIRE: {
        aT1 = TopAbs_VERTEX;
        aT2 = TopAbs_EDGE;
        break;
      }
      case TopAbs_SHELL: {
        aT1 = TopAbs_EDGE;
        aT2 = TopAbs_FACE;
        break;
      }
      default: {
        aT1 = TopAbs_FACE;
        aT2 = TopAbs_SOLID;
      }
    }
    //
    aLCB.Clear();
    BOPTools_AlgoTools::MakeConnexityBlocks(aRC, aT1, aT2, aLCB);
    if (aLCB.IsEmpty()) {
      continue;
    }
    //
    aItLCB.Initialize(aLCB);
    for (; aItLCB.More(); aItLCB.Next()) {
      BOPTools_AlgoTools::MakeContainer(aType, aRCB);
      //
      const TopoDS_Shape& aCB = aItLCB.Value();
      aIt.Initialize(aCB);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aCBS = aIt.Value();
        aBB.Add(aRCB, aCBS);
      }
      //
      if (aType == TopAbs_WIRE) {
        // reorient wire
        BOPTools_AlgoTools::OrientEdgesOnWire(aRCB);
      }
      else if (aType == TopAbs_SHELL) {
        BOPTools_AlgoTools::OrientFacesOnShell(aRCB);
      }
      //
      aRCB.Orientation(aSC.Orientation());
      //
      aLCRes.Append(aRCB);
    }
  }
  //
  RemoveDuplicates(aLCRes);
  //
  // add containers to result
  TopoDS_Compound aResult;
  aBB.MakeCompound(aResult);
  //
  aItLS.Initialize(aLCRes);
  for (; aItLS.More(); aItLS.Next()) {
    aBB.Add(aResult, aItLS.Value());
  }

  // create map of containers
  TopTools_MapOfShape aMSResult;
  TopExp::MapShapes(aResult, aMSResult);

  // get input non-container shapes
  TopTools_ListOfShape aLSNonCont;
  for (i = 0; i < 2; ++i)
  {
    const TopTools_ListOfShape& aLS = !i ? myArguments : myTools;
    aItLS.Initialize(aLS);
    for (; aItLS.More(); aItLS.Next())
    {
      const TopoDS_Shape& aS = aItLS.Value();
      BOPAlgo_Tools::TreatCompound(aS, aMInpFence, aLSNonCont);
    }
  }

  // put non-container shapes in the result
  aItLS.Initialize(aLSNonCont);
  for (; aItLS.More(); aItLS.Next())
  {
    const TopoDS_Shape& aS = aItLS.Value();
    if (myImages.IsBound(aS))
    {
      const TopTools_ListOfShape& aLSIm = myImages.Find(aS);
      aItLSIm.Initialize(aLSIm);
      for (; aItLSIm.More(); aItLSIm.Next())
      {
        const TopoDS_Shape& aSIm = aItLSIm.Value();
        if (aMSRC.Contains(aSIm) && aMSResult.Add(aSIm))
          aBB.Add(aResult, aSIm);
      }
    }
    else if (aMSRC.Contains(aS) && aMSResult.Add(aS))
      aBB.Add(aResult, aS);
  }

  myShape = aResult;
}
//=======================================================================
//function : BuildSolid
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::BuildSolid()
{
  // Containers
  TopTools_ListOfShape aLSC;
  //
  TopTools_ListIteratorOfListOfShape aItLS;
  TopExp_Explorer aExp;
  BRep_Builder aBB;
  //
  // Get solids from input arguments
  TopTools_MapOfShape aMSA;
  // Map the arguments to find shared faces
  TopTools_IndexedDataMapOfShapeListOfShape aMFS;
  for (Standard_Integer i = 0; i < 2; ++i) {
    const TopTools_ListOfShape& aLSA = (i) ? myArguments : myTools;
    aItLS.Initialize(aLSA);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSA = aItLS.Value();
      aExp.Init(aSA, TopAbs_SOLID);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aSol = aExp.Current();
        aMSA.Add(aSol);
        TopExp::MapShapesAndAncestors(aSol, TopAbs_FACE, TopAbs_SOLID, aMFS);
      }
      //
      // get Compsolids from input arguments
      CollectContainers(aSA, aLSC);
    }
  }
  //
  // Find solids in input arguments sharing faces with other solids
  TopTools_MapOfShape aMTSols;
  Standard_Integer i, aNb = aMFS.Extent();
  for (i = 1; i < aNb; ++i) {
    const TopTools_ListOfShape& aLSols = aMFS(i);
    if (aLSols.Extent() > 1) {
      aItLS.Initialize(aLSols);
      for(; aItLS.More(); aItLS.Next()) {
        aMTSols.Add(aItLS.Value());
      }
    }
  }
  //
  // Possibly untouched solids - to be added to results as is
  TopTools_IndexedMapOfShape aMUSols;
  // Use map to chose the most outer faces to build result solids
  aMFS.Clear();
  // Internal faces
  TopTools_IndexedMapOfShape aMFI;
  //
  TopoDS_Iterator aIt(myRC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx = aIt.Value();
    if (aMSA.Contains(aSx)) {
      if (!aMTSols.Contains(aSx)) {
        aMUSols.Add(aSx);
        continue;
      }
    }
    //
    MapFacesToBuildSolids(aSx, aMFS, aMFI);
  } // for (; aIt.More(); aIt.Next()) {
  //
  // Process possibly untouched solids.
  // Really untouched solids will be added into result as is.
  // Others will be processed by BuilderSolid.
  BOPTools_IndexedDataMapOfSetShape aDMSTS;
  //
  aNb = aMUSols.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aSx = aMUSols(i);
    //
    aExp.Init(aSx, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      if (aMFS.Contains(aExp.Current())) {
        break;
      }
    }
    //
    if (aExp.More()) {
      MapFacesToBuildSolids(aSx, aMFS, aMFI);
    }
    else {
      BOPTools_Set aST;
      aST.Add(aSx, TopAbs_FACE);
      if (!aDMSTS.Contains(aST)) {
        aDMSTS.Add(aST, aSx);
      }
    }
  }
  //
  TopTools_IndexedDataMapOfShapeListOfShape aMEF;
  // Fill the list of faces to build the result solids
  TopTools_ListOfShape aSFS;
  aNb = aMFS.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopTools_ListOfShape& aLSx = aMFS(i);
    if (aLSx.Extent() == 1) {
      const TopoDS_Shape& aFx = aMFS.FindKey(i);
      TopExp::MapShapesAndAncestors(aFx, TopAbs_EDGE, TopAbs_FACE, aMEF);
      aSFS.Append(aFx);
    }
  }
  // Internal faces
  aNb = aMFI.Extent();
  for (i = 1; i <= aNb; ++i) {
    TopoDS_Shape aFx = aMFI.FindKey(i);
    aSFS.Append(aFx.Oriented(TopAbs_FORWARD));
    aSFS.Append(aFx.Oriented(TopAbs_REVERSED));
  }
  //
  TopoDS_Shape aRC;
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);
  if (aSFS.Extent()) {
    // Build solids from set of faces
    BOPAlgo_BuilderSolid aBS;
    aBS.SetContext(myContext);
    aBS.SetShapes(aSFS);
    aBS.Perform();
    if (aBS.HasErrors()) {
      AddError (new BOPAlgo_AlertSolidBuilderFailed); // SolidBuilder failed
      return;
    }

    myReport->Merge(aBS.GetReport());

    // new solids
    const TopTools_ListOfShape& aLSR = aBS.Areas();
    //
    // add new solids to result
    aItLS.Initialize(aLSR);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSR = aItLS.Value();
      aBB.Add(aRC, aSR);
    }
  }
  //
  // add untouched solids to result
  aNb = aDMSTS.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aSx = aDMSTS(i);
    aBB.Add(aRC, aSx);
  }
  //
  if (aLSC.IsEmpty()) {
    // no Compsolids in arguments
    myShape = aRC;
    return;
  }
  //
  // build new Compsolids from new solids containing splits
  // of faces from arguments of type Compsolid
  //
  TopoDS_Shape aResult;
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aResult);
  //
  aIt.Initialize(aRC);
  if (!aIt.More()) {
    // no solids in the result
    myShape = aRC;
    return;
  }
  //
  const TopoDS_Shape& aSol1 = aIt.Value();
  aIt.Next();
  //
  // optimization for one solid in the result
  if (!aIt.More()) {
    TopoDS_Shape aCS;
    BOPTools_AlgoTools::MakeContainer(TopAbs_COMPSOLID, aCS);
    aBB.Add(aCS, aSol1);
    //
    aBB.Add(aResult, aCS);
    myShape = aResult;
    return;
  }
  //
  // get splits of faces of the Compsolid arguments
  TopTools_MapOfShape aMFCs;
  aItLS.Initialize(aLSC);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aCs = aItLS.Value();
    aExp.Init(aCs, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aF = aExp.Current();
      const TopTools_ListOfShape* pLFIm = myImages.Seek(aF);
      if (!pLFIm) {
        aMFCs.Add(aF);
      }
      else {
        TopTools_ListIteratorOfListOfShape aItLFIm(*pLFIm);
        for (; aItLFIm.More(); aItLFIm.Next()) {
          aMFCs.Add(aItLFIm.Value());
        }
      }
    }
  }
  //
  // build connexity blocks from new solids
  TopTools_ListOfShape aLCBS;
  BOPTools_AlgoTools::MakeConnexityBlocks(aRC, TopAbs_FACE, TopAbs_SOLID, aLCBS);
  //
  aItLS.Initialize(aLCBS);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aCB = aItLS.Value();
    //
    // check if the Compsolid should be created
    aExp.Init(aCB, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      if (aMFCs.Contains(aExp.Current())) {
        break;
      }
    }
    //
    if (!aExp.More()) {
      // add solids directly into result as their origins are not Compsolids
      for (aIt.Initialize(aCB); aIt.More(); aIt.Next()) {
        aBB.Add(aResult, aIt.Value());
      }
      continue;
    }
    //
    // make Compsolid
    TopoDS_Shape aCS;
    BOPTools_AlgoTools::MakeContainer(TopAbs_COMPSOLID, aCS);
    //
    aIt.Initialize(aCB);
    for (; aIt.More(); aIt.Next()) {
      aBB.Add(aCS, aIt.Value());
    }
    //
    aBB.Add(aResult, aCS);
  }
  //
  myShape = aResult;
}
//=======================================================================
//function : TypeToExplore
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim)
{
  TopAbs_ShapeEnum aRet;
  //
  switch(theDim) {
  case 0:
    aRet=TopAbs_VERTEX;
    break;
  case 1:
    aRet=TopAbs_EDGE;
    break;
  case 2:
    aRet=TopAbs_FACE;
    break;
  case 3:
    aRet=TopAbs_SOLID;
    break;
  default:
    aRet=TopAbs_SHAPE;
    break;
  }
  return aRet;
}
//=======================================================================
//function : CollectContainers
//purpose  : 
//=======================================================================
void CollectContainers(const TopoDS_Shape& theS,
                       TopTools_ListOfShape& theLSC)
{
  TopAbs_ShapeEnum aType = theS.ShapeType();
  if (aType == TopAbs_WIRE ||
      aType == TopAbs_SHELL ||
      aType == TopAbs_COMPSOLID) {
    theLSC.Append(theS);
    return;
  }
  //
  if (aType != TopAbs_COMPOUND) {
    return;
  }
  //
  TopoDS_Iterator aIt(theS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    CollectContainers(aS, theLSC);
  }
}

//=======================================================================
//function : RemoveDuplicates
//purpose  : Filters the containers with identical contents
//=======================================================================
void RemoveDuplicates(TopTools_ListOfShape& theContainers)
{
  RemoveDuplicates(theContainers, TopAbs_WIRE);
  RemoveDuplicates(theContainers, TopAbs_SHELL);
  RemoveDuplicates(theContainers, TopAbs_COMPSOLID);
}

//=======================================================================
//function : RemoveDuplicates
//purpose  : Filters the containers of given type with identical contents
//=======================================================================
void RemoveDuplicates(TopTools_ListOfShape& theContainers,
                      const TopAbs_ShapeEnum theType)
{
  // get containers of given type
  TopTools_ListOfShape aLC;
  TopTools_ListIteratorOfListOfShape aItLC(theContainers);
  for (; aItLC.More(); aItLC.Next()) {
    const TopoDS_Shape& aC = aItLC.Value();
    if (aC.ShapeType() == theType) {
      aLC.Append(aC);
    }
  }
  //
  if (aLC.IsEmpty()) {
    return;
  }
  //
  // map containers to compare its contents
  NCollection_IndexedDataMap<TopoDS_Shape, TopTools_MapOfShape> aContents;
  //
  aItLC.Initialize(aLC);
  for (; aItLC.More(); aItLC.Next()) {
    const TopoDS_Shape& aC = aItLC.Value();
    //
    TopTools_MapOfShape& aMC = aContents(aContents.Add(aC, TopTools_MapOfShape()));
    //
    TopoDS_Iterator aIt(aC);
    for (; aIt.More(); aIt.Next()) {
      aMC.Add(aIt.Value());
    }
  }
  //
  // compare the contents of the containers and find duplicates
  TopTools_MapOfShape aDuplicates;
  //
  Standard_Integer i, j, aNb = aContents.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aCi = aContents.FindKey(i);
    if (aDuplicates.Contains(aCi)) {
      continue;
    }
    const TopTools_MapOfShape& aMi = aContents(i);
    Standard_Integer aNbi = aMi.Extent();
    //
    for (j = i + 1; j <= aNb; ++j) {
      const TopoDS_Shape& aCj = aContents.FindKey(j);
      if (aDuplicates.Contains(aCj)) {
        continue;
      }
      const TopTools_MapOfShape& aMj = aContents(j);
      Standard_Integer aNbj = aMj.Extent();
      //
      Standard_Integer aNbCommon = NbCommonItemsInMap(aMi, aMj);
      //
      if (aNbj == aNbCommon) {
        aDuplicates.Add(aCj);
        continue;
      }
      //
      if (aNbi == aNbCommon) {
        aDuplicates.Add(aCi);
        break;
      }
    }
  }
  //
  if (aDuplicates.IsEmpty()) {
    return;
  }
  //
  // remove duplicating containers
  aItLC.Initialize(theContainers);
  for (; aItLC.More(); ) {
    const TopoDS_Shape& aC = aItLC.Value();
    if (aDuplicates.Contains(aC)) {
      theContainers.Remove(aItLC);
      continue;
    }
    aItLC.Next();
  }
}

//=======================================================================
//function : NbCommonItemsInMap
//purpose  : Counts the items contained in both maps
//=======================================================================
Standard_Integer NbCommonItemsInMap(const TopTools_MapOfShape& theM1,
                                    const TopTools_MapOfShape& theM2)
{
  const TopTools_MapOfShape* aMap1 = &theM1;
  const TopTools_MapOfShape* aMap2 = &theM2;
  //
  if (theM2.Extent() < theM1.Extent()) {
    aMap1 = &theM2;
    aMap2 = &theM1;
  }
  //
  Standard_Integer iCommon = 0;
  for (TopTools_MapIteratorOfMapOfShape aIt(*aMap1); aIt.More(); aIt.Next()) {
    if (aMap2->Contains(aIt.Value())) {
      ++iCommon;
    }
  }
  return iCommon;
}
//=======================================================================
//function : MapFacesToBuildSolids
//purpose  : Stores the faces of the given solid into outgoing maps:
//           <theMFS> - not internal faces with reference to solid;
//           <theMFI> - internal faces.
//=======================================================================
void MapFacesToBuildSolids(const TopoDS_Shape& theSol,
                           TopTools_IndexedDataMapOfShapeListOfShape& theMFS,
                           TopTools_IndexedMapOfShape& theMFI)
{
  TopExp_Explorer aExp(theSol, TopAbs_FACE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aF = aExp.Current();
    //
    if (aF.Orientation() == TopAbs_INTERNAL) {
      theMFI.Add(aF);
      continue;
    }
    //
    TopTools_ListOfShape* pLSol = theMFS.ChangeSeek(aF);
    if (!pLSol) {
      pLSol = &theMFS(theMFS.Add(aF, TopTools_ListOfShape()));
      pLSol->Append(theSol);
    }
    else {
      const TopoDS_Shape& aF1 = theMFS.FindKey(theMFS.FindIndex(aF));
      if (aF1.Orientation() != aF.Orientation()) {
        pLSol->Append(theSol);
      }
    }
  }
}
