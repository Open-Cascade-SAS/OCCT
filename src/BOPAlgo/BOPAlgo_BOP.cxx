// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPAlgo_BOP.ixx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPAlgo_BuilderSolid.hxx>
#include <TopoDS_Edge.hxx>

static
  TopAbs_ShapeEnum TypeToExplore(const Standard_Integer theDim);

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
  myNbArgs=2;
  Clear();
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPAlgo_BOP::BOPAlgo_BOP(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Builder(theAllocator),
  myTools(myAllocator),
  myMapTools(100, myAllocator)
{
  myNbArgs=2;
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
//function : AddArgument
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::AddArgument(const TopoDS_Shape& theShape)
{
  if (myMapFence.Add(theShape)) {
    myArguments.Append(theShape);
    myArgs[0]=theShape;
  }
}
//=======================================================================
//function : AddTool
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::AddTool(const TopoDS_Shape& theShape)
{
  if (myMapTools.Add(theShape)) {
    myTools.Append(theShape);
    myArgs[1]=theShape;
    //
    if (myMapFence.Add(theShape)) {
      myArguments.Append(theShape);
    }
  }
}
//=======================================================================
//function : Object
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOPAlgo_BOP::Object()const
{
  return myArgs[0];
}
//=======================================================================
//function : Tool
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOPAlgo_BOP::Tool()const
{
  return myArgs[1];
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
  Standard_Integer i, aNb;
  Standard_Boolean bFlag;
  //
  myErrorStatus=0;
  //
  aNb=myArguments.Extent();
  if (aNb!=myNbArgs) {
    if (aNb!=1 || !(myArgs[0].IsSame(myArgs[1]))) {
      myErrorStatus=10; // invalid number of arguments
      return;
    }
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
  for (i=0; i<myNbArgs; ++i) {
    if (myArgs[i].IsNull()) {
      myErrorStatus=11; // argument is null shape
      return;
    }
  }
  //
  for (i=0; i<myNbArgs; ++i) {
    bFlag = BOPTools_AlgoTools3D::IsEmptyShape(myArgs[i]);
    if(bFlag) {
      myWarningStatus = 2;
    }
  }
  //
  for (i=0; i<myNbArgs; ++i) {
    myDims[i]=BOPTools_AlgoTools::Dimension(myArgs[i]);
    if (myDims[i]<0) {
      myErrorStatus=13; // non-homogenious argument
      return;
    }
  }
  //
  if (myOperation==BOPAlgo_UNKNOWN) {
    myErrorStatus=14; // non-licit operation
    return;
  }
  else if (myDims[0]<myDims[1]) {
    if (myOperation==BOPAlgo_FUSE ||
        myOperation==BOPAlgo_CUT21) {
      myErrorStatus=14; // non-licit operation for the arguments
      return;
    }
  }
  else if (myDims[0]>myDims[1]) {
    if (myOperation==BOPAlgo_FUSE ||
        myOperation==BOPAlgo_CUT) {
      myErrorStatus=14; // non-licit operation for the arguments
      return;
    }
  }
}
//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::Prepare()
{
  Standard_Integer i;
  BRep_Builder aBB;
  //
  BOPAlgo_Builder::Prepare();
  //
  if(myWarningStatus == 2) {
    switch(myOperation) {
      case BOPAlgo_FUSE:
        for ( i = 0; i < myNbArgs; i++ ) {
          aBB.Add(myShape, myArgs[i]);
        }
        break;
      case BOPAlgo_COMMON:
      case BOPAlgo_SECTION:
        break;
      case BOPAlgo_CUT:
        if(BOPTools_AlgoTools3D::IsEmptyShape(myArgs[0])) {
          break;
        } else {
          aBB.Add(myShape, myArgs[0]);
        }
        break;
      case BOPAlgo_CUT21:
        if(BOPTools_AlgoTools3D::IsEmptyShape(myArgs[1])) {
          break;
        } else {
          aBB.Add(myShape, myArgs[1]);
        }
        break;
      default:
        break;
    }
  }
}
//=======================================================================
//function : PerformInternal
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::PerformInternal(const BOPAlgo_PaveFiller& theFiller)
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
  if(myWarningStatus == 2) {
    return;
  }
  //
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
  
  BuildResult(TopAbs_EDGE);
  if (myErrorStatus) {
    return;
  }
  //-------------------------------- SECTION f
  if (myOperation==BOPAlgo_SECTION) {
    BuildSection();
    PrepareHistory();
    PostTreat();
    return;
  }
  //-------------------------------- SECTION t
  //
  // 3.3 Wires
  FillImagesContainers(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  
  // 3.4 Faces
  FillImagesFaces();
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_FACE);
  if (myErrorStatus) {
    return;
  }
  // 3.5 Shells
  
  FillImagesContainers(TopAbs_SHELL);
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_SHELL);
  if (myErrorStatus) {
    return;
  }
  // 3.6 Solids
  FillImagesSolids();
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_SOLID);
  if (myErrorStatus) {
    return;
  }
  // 3.7 CompSolids
  FillImagesContainers(TopAbs_COMPSOLID);
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_COMPSOLID);
  if (myErrorStatus) {
    return;
  }
  // 3.8 Compounds
  FillImagesCompounds();
  if (myErrorStatus) {
    return;
  }
  
  BuildResult(TopAbs_COMPOUND);
  if (myErrorStatus) {
    return;
  }
  //
  // 6.BuildShape;
  BuildShape();
  // 
  // 4.History
  PrepareHistory();

  //
  // 5 Post-treatment 
  PostTreat();
}
//=======================================================================
//function : BuildShape
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::BuildShape()
{
  Standard_Integer aDmin, aNbLCB;
  TopAbs_ShapeEnum aT1, aT2, aTR;
  TopoDS_Shape aR, aRC;
  TopoDS_Iterator aIt;
  BRep_Builder aBB;
  BOPCol_ListOfShape aLCB;
  BOPCol_ListIteratorOfListOfShape aItLCB;
  //
  myErrorStatus=0;
  //
  BuildRC();
  //myShape=myRC;
  //
  aDmin=myDims[1];
  if (myDims[0]<myDims[1]) {
    aDmin=myDims[0];
  }
  //
  if (!aDmin) {
    myShape=myRC;
    return;
  }
  //
  else if (aDmin==1 || aDmin==2) { //edges, faces 
    aT1=TopAbs_VERTEX;
    aT2=TopAbs_EDGE;
    aTR=TopAbs_WIRE;
    if (aDmin==2) {
      aT1=TopAbs_EDGE;
      aT2=TopAbs_FACE;
      aTR=TopAbs_SHELL;
    }
    //
    BOPTools_AlgoTools::MakeConnexityBlocks(myRC, aT1, aT2, aLCB);
    aNbLCB=aLCB.Extent();
    if (!aNbLCB) {
      myShape=myRC;
      return;
    }
    //
    BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);  
    //
    aItLCB.Initialize(aLCB);
    for (; aItLCB.More(); aItLCB.Next()) {
      BOPTools_AlgoTools::MakeContainer(aTR, aR);  
      //
      const TopoDS_Shape& aCB=aItLCB.Value();
      aIt.Initialize(aCB);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aS=aIt.Value();
        aBB.Add(aR, aS);
      }
      //
      if (aTR==TopAbs_SHELL) {
        BOPTools_AlgoTools::OrientFacesOnShell(aR);
      }
      //
      aBB.Add(aRC, aR);
    }
    myShape=aRC;
  }// elase if (aDmin==1 || aDmin==2) {
  
  else { //aDmin=3
    if (myOperation==BOPAlgo_FUSE) {
      BuildSolid();
    }
    else {
      myShape=myRC;
    }
  }
}
//=======================================================================
//function : BuildRC
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::BuildRC()
{
  Standard_Boolean bFlag;
  Standard_Integer i, aDmin, aNb[2], iX = 0, iY = 0;
  TopAbs_ShapeEnum aTmin;
  TopoDS_Compound aC, aCS[2];
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aItIm;
  BOPCol_IndexedMapOfShape aMS[2];
  BOPCol_IndexedMapOfShape aMSV[2];
  //
  myErrorStatus=0;
  //
  // A. Fuse
  if (myOperation==BOPAlgo_FUSE) {
    aBB.MakeCompound(aC);
    aTmin=TypeToExplore(myDims[0]);
    aExp.Init(myShape, aTmin);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aS=aExp.Current();
      aBB.Add(aC, aS);
    }
    myRC=aC;
    return;
  }
  //
  // B. Non-Fuse
  //
  // 1. Compounds CS that consist of an Arg or Images of the Arg
  for (i=0; i<myNbArgs; ++i) {
    aBB.MakeCompound(aCS[i]);
    const TopoDS_Shape& aS=myArgs[i];
    if (myImages.IsBound(aS)){
      const BOPCol_ListOfShape& aLSIm=myImages.Find(aS);
      aItIm.Initialize(aLSIm);
      for (; aItIm.More(); aItIm.Next()) {
        const TopoDS_Shape& aSIm=aItIm.Value();
        aBB.Add(aCS[i], aSIm);
      }
    }
    else {
      aBB.Add(aCS[i], aS);
    }
  }
  //
  aDmin=myDims[1];
  if (myDims[0]<myDims[1]) {
    aDmin=myDims[0];
  }
  aTmin=TypeToExplore(aDmin);
  for (i=0; i<myNbArgs; ++i) {
    aExp.Init(aCS[i], aTmin);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape aS=aExp.Current();
      if (aTmin == TopAbs_EDGE) {
        const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aS));
        if (BRep_Tool::Degenerated(aE)) {
          TopExp_Explorer aExpE(aE, TopAbs_VERTEX);
          TopoDS_Shape aS1 = aExpE.Current();
          if (myImages.IsBound(aS1)){
            const BOPCol_ListOfShape& aLSIm=myImages.Find(aS1);
            const TopoDS_Shape& aSIm=aLSIm.First();
            aMSV[i].Add(aSIm);
          } else {
            aMSV[i].Add(aS1);
          }
        }
      }
      //
      if (myImages.IsBound(aS)){
        const BOPCol_ListOfShape& aLSIm=myImages.Find(aS);
        aItIm.Initialize(aLSIm);
        for (; aItIm.More(); aItIm.Next()) {
          const TopoDS_Shape& aSIm=aItIm.Value();
          aMS[i].Add(aSIm);
        }
      }
      else {
        aMS[i].Add(aS);
      }
    }
    aNb[i]=aMS[i].Extent();
  }
  //
  aBB.MakeCompound(aC);
  //
  // 3. Find common parts
  if (myOperation==BOPAlgo_COMMON) {
    if (myDims[0]==myDims[1]) {
      iX=(aNb[0]>aNb[1])? 1 : 0;
    } else {
      iX=(myDims[0]<myDims[1]) ? 0 : 1;
    }
    iY=(iX+1)%2;
  }
  else if (myOperation==BOPAlgo_CUT) {
    iX=0;
    iY=1;
  }
  else if (myOperation==BOPAlgo_CUT21) {
    iX=1;
    iY=0;
  }
  for (i=1; i<=aNb[iX]; ++i) {
    const TopoDS_Shape& aSx=aMS[iX].FindKey(i);
    bFlag=aMS[iY].Contains(aSx);
    if (aTmin == TopAbs_EDGE) {
      const TopoDS_Edge& aE = (*(TopoDS_Edge*)(&aSx));
      if (BRep_Tool::Degenerated(aE)) {
        TopExp_Explorer aExpE(aE, TopAbs_VERTEX);
        TopoDS_Shape aSx1 = aExpE.Current();
        TopoDS_Shape aSIm;
        if (myImages.IsBound(aSx1)) {
          const BOPCol_ListOfShape& aLSIm=myImages.Find(aSx1);
          aSIm=aLSIm.First();
        } else {
          aSIm = aSx1;
        }
        bFlag=aMSV[iY].Contains(aSIm);
      }
    }
    //
    if (myOperation!=BOPAlgo_COMMON) {
      bFlag=!bFlag;
    }
    //
    if (bFlag) {
      aBB.Add(aC, aSx);
    }
  }
  //
  myRC=aC;
}
//
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
//function : BuildSolid
//purpose  : 
//=======================================================================
  void BOPAlgo_BOP::BuildSolid()
{
  Standard_Integer i, aNbF, aNbSx, iX, iErr;
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
  //
  myErrorStatus=0;
  //
  aIt.Initialize(myRC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value(); 
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
  }
  //
  BOPCol_ListOfShape aLF, aLFx; //faces that will be added in the end;
  // SFS
  aNbF=aMFS.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aFx=aMFS.FindKey(i);
    const BOPCol_ListOfShape& aLSx=aMFS(i);
    aNbSx=aLSx.Extent();
    if (aNbSx==1) {
      BOPTools::MapShapesAndAncestors(aFx, TopAbs_EDGE, TopAbs_FACE, aMEF);
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
  myShape=aRC;
}

//=======================================================================
//function : IsBoundImages
//purpose  : 
//=======================================================================
  Standard_Boolean BOPAlgo_BOP::IsBoundSplits(const TopoDS_Shape& aS,
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
      if (mySplits.IsBound(aSx)  || myOrigins.IsBound(aS)) {
        return !bRet;
      }
    }
  }

  return bRet;
}
