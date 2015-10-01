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
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_Set.hxx>
#include <BOPTools_SetMapHasher.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <NCollection_DataMap.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

typedef NCollection_IndexedDataMap
  <BOPTools_Set,
  TopoDS_Shape,
  BOPTools_SetMapHasher> BOPTools_IndexedDataMapOfSetShape;
//
static
  TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim);
//
static
  void CollectContainers(const TopoDS_Shape& theS,
                         BOPCol_ListOfShape& theLSC);


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BOP::BOPAlgo_BOP()
:
  BOPAlgo_Builder(),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
{
  Clear();
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_BOP::BOPAlgo_BOP
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Builder(theAllocator),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
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
  myTools.Clear();
  myMapTools.Clear();
  myDims[0]=-1;
  myDims[1]=-1;
  //
  BOPAlgo_Builder::Clear();
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
//function : AddTool
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::AddTool(const TopoDS_Shape& theShape)
{
  if (myMapTools.Add(theShape)) {
    myTools.Append(theShape);
  }
}
//=======================================================================
//function : SetTools
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::SetTools(const BOPCol_ListOfShape& theShapes)
{
  BOPCol_ListIteratorOfListOfShape aIt;
  //
  myTools.Clear();
  aIt.Initialize(theShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    AddTool(aS);
  }
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::CheckData()
{
  Standard_Integer i, j, iDim, aNbArgs, aNbTools;
  Standard_Boolean bFlag, bFuse;
  BOPCol_ListIteratorOfListOfShape aItLS;
  //
  myErrorStatus=0;
  //
  if (!(myOperation==BOPAlgo_COMMON ||
        myOperation==BOPAlgo_FUSE || 
        myOperation==BOPAlgo_CUT|| 
        myOperation==BOPAlgo_CUT21)) {
    // non-licit operation
    myErrorStatus=14;
    return;
  }
  //
  aNbArgs=myArguments.Extent();
  if (!aNbArgs) {
    // invalid number of Arguments
    myErrorStatus=100; 
    return;
  }
  //
  aNbTools=myTools.Extent();
  if (!aNbTools) { 
    // invalid number of Tools
    myErrorStatus=100;
    return;
  }
  //
  if (!myPaveFiller) {
    myErrorStatus=101; 
    return;
  }
  //
  myErrorStatus=myPaveFiller->ErrorStatus();
  if (myErrorStatus) {
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
  //
  for (i=0; i<2; ++i) {
    const BOPCol_ListOfShape& aLS=(!i)? myArguments : myTools;
    aItLS.Initialize(aLS);
    for (j=0; aItLS.More(); aItLS.Next(), ++j) {
      const TopoDS_Shape& aS=aItLS.Value();
      bFlag=BOPTools_AlgoTools3D::IsEmptyShape(aS);
      if(bFlag) {
        myWarningStatus=2;
      }
      //
      iDim=BOPTools_AlgoTools::Dimension(aS);
      if (iDim<0) {
        // non-homogenious argument
        myErrorStatus=13; 
        return;
      }
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
        // non-homogenious argument
        myErrorStatus=13;
        return;
      }
    }
  }
  //
  if (((myOperation == BOPAlgo_FUSE)  && (iDimMax[0] != iDimMax[1])) ||
      ((myOperation == BOPAlgo_CUT)   && (iDimMax[0] >  iDimMin[1])) ||
      ((myOperation == BOPAlgo_CUT21) && (iDimMin[0] <  iDimMax[1])) ) {
    // non-licit operation for the arguments
    myErrorStatus=14;
    return;
  }
  //
  myDims[0] = iDimMin[0];
  myDims[1] = iDimMin[1];
}
//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::Prepare()
{
  //
  BOPAlgo_Builder::Prepare();
  //
  if(myWarningStatus == 2) {
    Standard_Integer i;
    BRep_Builder aBB;
    BOPCol_ListIteratorOfListOfShape aItLS;
    //
    switch(myOperation) {
      case BOPAlgo_FUSE: {
        for (i=0; i<2; ++i) {
          const BOPCol_ListOfShape& aLS=(!i)? myArguments : myTools;
          aItLS.Initialize(aLS);
          for (; aItLS.More(); aItLS.Next()) {
            const TopoDS_Shape& aS=aItLS.Value();
            aBB.Add(myShape, aS);
          }
        }
      }
        break;
      //  
      case BOPAlgo_CUT: {
        aItLS.Initialize(myArguments);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aS=aItLS.Value();
          if(!BOPTools_AlgoTools3D::IsEmptyShape(aS)) {
            aBB.Add(myShape, aS);
          } 
        }
      }
        break;
      
      case BOPAlgo_CUT21: {
        aItLS.Initialize(myTools);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aS=aItLS.Value();
          if(!BOPTools_AlgoTools3D::IsEmptyShape(aS)) {
            aBB.Add(myShape, aS);
          } 
        }
      }
        break;
        //
      default:
        break;
      }
  }
}
//=======================================================================
//function : BuildResult
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::BuildResult(const TopAbs_ShapeEnum theType)
{
  TopAbs_ShapeEnum aType;
  BRep_Builder aBB;
  BOPCol_MapOfShape aM;
  BOPCol_ListIteratorOfListOfShape aIt, aItIm;
  //
  myErrorStatus=0;
  //
  const BOPCol_ListOfShape& aLA=myDS->Arguments();
  aIt.Initialize(aLA);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aType=aS.ShapeType();
    if (aType==theType) {
      if (myImages.IsBound(aS)){
        const BOPCol_ListOfShape& aLSIm=myImages.Find(aS);
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
  BOPCol_ListIteratorOfListOfShape aItLS;
  //
  myErrorStatus=0;
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
  BOPCol_ListOfShape aLS(aAllocator);
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
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myPaveFiller=(BOPAlgo_PaveFiller*)&theFiller;
  myDS=myPaveFiller->PDS();
  myContext=myPaveFiller->Context();
  //
  // 1. CheckData
  CheckData();
  if (myErrorStatus && !myWarningStatus) {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (myErrorStatus) {
    return;
  }
  //
  if(myWarningStatus == 2) {
    return;
  }
  // 3. Fill Images
  // 3.1 Vertices
  FillImagesVertices();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_VERTEX);
  if (myErrorStatus) {
    return;
  }
  // 3.2 Edges
  FillImagesEdges();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_EDGE);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.3 Wires
  FillImagesContainers(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.4 Faces
  FillImagesFaces();
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_FACE);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.5 Shells
  FillImagesContainers(TopAbs_SHELL);
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_SHELL);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.6 Solids
  FillImagesSolids();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_SOLID);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.7 CompSolids
  FillImagesContainers(TopAbs_COMPSOLID);
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_COMPSOLID);
  if (myErrorStatus) {
    return;
  }
  //
  // 3.8 Compounds
  FillImagesCompounds();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_COMPOUND);
  if (myErrorStatus) {
    return;
  }
  //
  // 4.BuildShape;
  BuildShape();
  if (myErrorStatus) {
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
  myErrorStatus = 0;
  //
  aBB.MakeCompound(aC);
  //
  // A. Fuse
  if (myOperation == BOPAlgo_FUSE) {
    BOPCol_MapOfShape aMFence;
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
  BOPCol_IndexedMapOfShape aMArgs, aMTools;
  BOPCol_IndexedMapOfShape aMArgsIm, aMToolsIm;
  BOPCol_ListIteratorOfListOfShape aItLS;
  //
  for (i = 0; i < 2; ++i) {
    const BOPCol_ListOfShape& aLS = !i ? myArguments : myTools;
    BOPCol_IndexedMapOfShape& aMS = !i ? aMArgs : aMTools;
    aItLS.Initialize(aLS);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      iDim = BOPTools_AlgoTools::Dimension(aS);
      aType = TypeToExplore(iDim);
      BOPTools::MapShapes(aS, aType, aMS);
    }
  }
  //
  bCheckEdges = Standard_False;
  //
  for (i = 0; i < 2; ++i) {
    const BOPCol_IndexedMapOfShape& aMS = !i ? aMArgs : aMTools;
    BOPCol_IndexedMapOfShape& aMSIm = !i ? aMArgsIm : aMToolsIm;
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
        const BOPCol_ListOfShape& aLSIm = myImages.Find(aS);
        aItLS.Initialize(aLSIm);
        for (; aItLS.More(); aItLS.Next()) {
          const TopoDS_Shape& aSIm = aItLS.Value();
          aMSIm.Add(aSIm);
        }
      }
      else {
        aMSIm.Add(aS);
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
  const BOPCol_IndexedMapOfShape& aMIt = bCut21 ? aMToolsIm : aMArgsIm;
  const BOPCol_IndexedMapOfShape& aMCheck = bCut21 ? aMArgsIm : aMToolsIm;
  //
  BOPCol_IndexedMapOfShape aMCheckExp, aMItExp;
  //
  if (bCommon) {
    aNb = aMIt.Extent();
    for (i = 1; i <= aNb; ++i) {
      const TopoDS_Shape& aS = aMIt(i);
      iDimMax = BOPTools_AlgoTools::Dimension(aS);
      for (iDim = iDimMin; iDim < iDimMax; ++iDim) {
        aType = TypeToExplore(iDim);
        BOPTools::MapShapes(aS, aType, aMItExp);
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
      BOPTools::MapShapes(aS, aType, aMCheckExp);
    }
    aMCheckExp.Add(aS);
  }
  //
  aNb = aMItExp.Extent();
  for (i = 1; i <= aNb; ++i) {
    const TopoDS_Shape& aS = aMItExp(i);
    //
    bContains = aMCheckExp.Contains(aS);
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
    BOPCol_MapOfShape aMFence;
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
          BOPTools::MapShapes(aS, aMFence);
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
  BOPCol_IndexedMapOfShape aMVC;
  //
  // 1. Vertices of aC
  BOPTools::MapShapes(aC, TopAbs_VERTEX, aMVC);
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
  BOPCol_ListIteratorOfListOfShape aItLS, aItLSIm, aItLCB;
  TopoDS_Iterator aIt;
  BRep_Builder aBB;
  TopoDS_Shape aRC, aRCB;
  //
  TopoDS_Compound aResult;
  aBB.MakeCompound(aResult);
  //
  BOPCol_MapOfShape aMSRC, aMFence;
  BOPTools::MapShapes(myRC, aMSRC);
  //
  // collect images of containers
  for (i = 0; i < 2; ++i) {
    const BOPCol_ListOfShape& aLS = !i ? myArguments : myTools;
    //
    aItLS.Initialize(aLS);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS = aItLS.Value();
      //
      CollectContainers(aS, aLSC);
    }
  }
  // make containers
  aItLS.Initialize(aLSC);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSC = aItLS.Value();
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
          if (aMSRC.Contains(aSIm) && aMFence.Add(aSIm)) {
            aBB.Add(aRC, aSIm);
          }
        }
      }
      else if (aMSRC.Contains(aS) && aMFence.Add(aS)) {
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
      if (aType == TopAbs_SHELL) {
        BOPTools_AlgoTools::OrientFacesOnShell(aRCB);
      }
      //
      aBB.Add(aResult, aRCB);
    }
  }
  //
  // add the rest of the shapes into result
  BOPCol_MapOfShape aMSResult;
  BOPTools::MapShapes(aResult, aMSResult);
  //
  aIt.Initialize(myRC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    if (!aMSResult.Contains(aS)) {
      aBB.Add(aResult, aS);
    }
  }
  //
  myShape = aResult;
}
//=======================================================================
//function : BuildSolid
//purpose  : 
//=======================================================================
void BOPAlgo_BOP::BuildSolid()
{
  Standard_Boolean bHasInterf, bHasSharedFaces;
  Standard_Integer i, aNbF, aNbSx, iX, iErr, aNbZ;
  TopAbs_Orientation aOr, aOr1;
  TopoDS_Iterator aIt;
  TopoDS_Shape aRC;
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_IndexedMapOfShape aMFI;
  BOPCol_IndexedDataMapOfShapeListOfShape aMFS, aMEF;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPCol_ListOfShape aSFS;
  BOPAlgo_BuilderSolid aSB; 
  BOPCol_MapOfShape aMSA, aMZ;
  BOPTools_IndexedDataMapOfSetShape aDMSTS;
  //
  myErrorStatus=0;
  //
  // Map of of Solids of Arguments
  for (i=0; i<2; ++i) {
    const BOPCol_ListOfShape& aLSA=(i) ? myArguments : myTools;
    aItLS.Initialize(aLSA);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aSA=aItLS.Value();
      aExp.Init(aSA, TopAbs_SOLID);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape& aZA=aExp.Current();
        aMSA.Add(aZA);
        //
        BOPTools::MapShapesAndAncestors(aZA, 
                                        TopAbs_FACE, 
                                        TopAbs_SOLID, 
                                        aMFS);
      }
    }
  }
  //
  aNbF=aMFS.Extent();
  for (i=1; i<aNbF; ++i) {
    //const TopoDS_Shape& aFA=aMFZA.FindKey(i);
    const BOPCol_ListOfShape& aLZA=aMFS(i);
    aNbZ=aLZA.Extent();
    if (aNbZ > 1) {
      aItLS.Initialize(aLZA);
      for(; aItLS.More(); aItLS.Next()) {
        const TopoDS_Shape& aZA=aItLS.Value();
        aMZ.Add(aZA);
      }
    }
  }
  //
  aMFS.Clear();
  //
  aIt.Initialize(myRC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value(); 
    if (aMSA.Contains(aSx)) {
      iX=myDS->Index(aSx);
      bHasInterf=myDS->HasInterf(iX);
      bHasSharedFaces=aMZ.Contains(aSx);
      //
      if (!bHasInterf && !bHasSharedFaces) {
        // It means that the solid aSx will be added
        // to the result as is. 
        // The solid aSx will not participate 
        // in creation of a new solid(s).
        BOPTools_Set aST;
        //
        aST.Add(aSx, TopAbs_FACE);
        //
        if (!aDMSTS.Contains(aST)) {
          aDMSTS.Add(aST, aSx);
        }
        
        continue; 
      }
    } 
    //
    aExp.Init(aSx, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aFx=aExp.Current();
      //
      aOr=aFx.Orientation();
      if (aOr==TopAbs_INTERNAL) {
        aMFI.Add(aFx);
        continue;
      }
      //
      if (!aMFS.Contains(aFx)) {
        BOPCol_ListOfShape aLSx;
        //
        aLSx.Append(aSx);
        aMFS.Add(aFx, aLSx);
      }
      else {
        iX=aMFS.FindIndex(aFx);
        const TopoDS_Shape& aFx1=aMFS.FindKey(iX);
        aOr1=aFx1.Orientation();
        if (aOr1!=aOr) {
          BOPCol_ListOfShape& aLSx=aMFS.ChangeFromKey(aFx);
          aLSx.Append(aSx);
          aMFS.Add(aFx, aLSx);
        }
      }
    }
  } // for (; aIt.More(); aIt.Next()) {
  //faces that will be added in the end;
  BOPCol_ListOfShape aLF, aLFx; 
  // SFS
  aNbF=aMFS.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aFx=aMFS.FindKey(i);
    const BOPCol_ListOfShape& aLSx=aMFS(i);
    aNbSx=aLSx.Extent();
    if (aNbSx==1) {
      BOPTools::MapShapesAndAncestors
        (aFx,TopAbs_EDGE, TopAbs_FACE, aMEF);
      if (IsBoundSplits(aFx, aMEF)){
        aLFx.Append(aFx);
        continue;
      }
      aLF.Append(aFx);
    }
  }

  aItLS.Initialize(aLF);
  for(; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aFx=aItLS.Value();
    aSFS.Append(aFx);
  }
  // add faces from aLFx to aSFS;
  aItLS.Initialize(aLFx);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aFx=aItLS.Value();
    aSFS.Append(aFx);
  }
  //
  aNbF=aMFI.Extent();
  for (i=1; i<=aNbF; ++i) {
    TopoDS_Shape aFx;
    //
    aFx=aMFI.FindKey(i);
    aFx.Orientation(TopAbs_FORWARD);
    aSFS.Append(aFx);
    aFx.Orientation(TopAbs_REVERSED);
    aSFS.Append(aFx);
  }
  //
  // BuilderSolid
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);  
  //
  aSB.SetContext(myContext);
  aSB.SetShapes(aSFS);
  aSB.Perform();
  iErr=aSB.ErrorStatus();
  if (iErr) {
    myErrorStatus=30; // SolidBuilder failed
    return;
  }
  //
  const BOPCol_ListOfShape& aLSR=aSB.Areas();
  //
  aItLS.Initialize(aLSR);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSR=aItLS.Value();
    aBB.Add(aRC, aSR);
  }
  //
  aNbSx = aDMSTS.Extent();
  for (i = 1; i <= aNbSx; ++i) {
    const TopoDS_Shape& aSx = aDMSTS(i);
    aBB.Add(aRC, aSx);
  }
  //
  myShape=aRC;
}
//=======================================================================
//function : IsBoundSplits
//purpose  : 
//=======================================================================
Standard_Boolean BOPAlgo_BOP::IsBoundSplits
  (const TopoDS_Shape& aS,
   BOPCol_IndexedDataMapOfShapeListOfShape& aMEF)
{
  Standard_Boolean bRet = Standard_False;
  if (mySplits.IsBound(aS) || myOrigins.IsBound(aS)) {
    return !bRet;
  }

  BOPCol_ListIteratorOfListOfShape aIt;
  Standard_Integer aNbLS;
  TopAbs_Orientation anOr;
  //
  //check face aF may be connected to face from mySplits
  TopExp_Explorer aExp(aS, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aExp.Current()));
    //
    anOr = aE.Orientation();
    if (anOr==TopAbs_INTERNAL) {
      continue;
    }
    //
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    const BOPCol_ListOfShape& aLS=aMEF.FindFromKey(aE);
    aNbLS = aLS.Extent();
    if (!aNbLS) {
      continue;
    }
    //
    aIt.Initialize(aLS);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSx = aIt.Value();
      if (mySplits.IsBound(aSx) || myOrigins.IsBound(aS)) {
        return !bRet;
      }
    }
  }
  //
  return bRet;
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
                       BOPCol_ListOfShape& theLSC)
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

