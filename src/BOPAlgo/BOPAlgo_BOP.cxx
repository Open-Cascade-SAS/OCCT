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

#include <BOPAlgo_BOP.ixx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>

#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>

#include <BOPDS_DS.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <BOPAlgo_BuilderSolid.hxx>

#include <BRep_Tool.hxx>
#include <NCollection_IncAllocator.hxx>
//
#include <BOPTools_Set.hxx>
#include <BOPTools_SetMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

typedef NCollection_DataMap  
  <BOPTools_Set, 
  TopoDS_Shape, 
  BOPTools_SetMapHasher> BOPTools_DataMapOfSetShape; 
//
typedef BOPTools_DataMapOfSetShape::Iterator 
  BOPTools_DataMapIteratorOfDataMapOfSetShape; 

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
void BOPAlgo_BOP::SetTools(const TopTools_ListOfShape& theShapes)
{
  TopTools_ListIteratorOfListOfShape aIt;
  //
  myTools.Clear();
  aIt.Initialize(theShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    AddTool(aS);
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
  Standard_Boolean bFlag;
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
  // myDims
  myDims[0]=-1;
  myDims[1]=-1;
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
        myDims[i]=iDim;
        continue;
      }
      //
      if (iDim!=myDims[i]) { 
        // non-homogenious argument
        myErrorStatus=13;
        return;
      }
    }
  }
  //
  if (myDims[0]<myDims[1]) {
    if (myOperation==BOPAlgo_FUSE ||
        myOperation==BOPAlgo_CUT21) { 
      // non-licit operation for the arguments
      myErrorStatus=14;
      return;
    }
  }
  else if (myDims[0]>myDims[1]) {
    if (myOperation==BOPAlgo_FUSE ||
        myOperation==BOPAlgo_CUT) {
      // non-licit operation for the arguments
      myErrorStatus=14; 
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
  aAllocator=new NCollection_IncAllocator;
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
  Standard_Boolean bFlag1, bFlag2, bIsBound;
  Standard_Integer aDmin;
  TopAbs_ShapeEnum aTmin;
  TopoDS_Compound aC;
  TopoDS_Shape aSAIm, aSTIm;
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_DataMapOfShapeShape aDMSSA;
  BOPCol_ListIteratorOfListOfShape aItLS, aItIm; 
  Standard_Boolean bHasInterf;
  Standard_Integer iX;
  BOPTools_DataMapOfSetShape aDMSTS;
  //
  myErrorStatus=0;
  //
  aBB.MakeCompound(aC);
  //
  // A. Fuse
  if (myOperation==BOPAlgo_FUSE) {
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
  aDmin=myDims[1];
  if (myDims[0] < myDims[1]) {
    aDmin=myDims[0];
  }
  aTmin=TypeToExplore(aDmin);
  //
  // B. Common, Cut, Cut21
  //
  bFlag1=(myOperation==BOPAlgo_COMMON || myOperation==BOPAlgo_CUT21);
  bFlag2=(myOperation==BOPAlgo_CUT || myOperation==BOPAlgo_CUT21);
  //
  const BOPCol_ListOfShape& aLA=( bFlag1) ? myArguments : myTools;
  aItLS.Initialize(aLA);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSA=aItLS.Value();
    //
    if (myImages.IsBound(aSA)){
      const BOPCol_ListOfShape& aLSAIm=myImages.Find(aSA);
      aItIm.Initialize(aLSAIm);
      for (; aItIm.More(); aItIm.Next()) {
        const TopoDS_Shape& aSAIm=aItIm.Value();
        aExp.Init(aSAIm, aTmin);
        for (; aExp.More(); aExp.Next()) {
          const TopoDS_Shape aSIm=aExp.Current();
          aDMSSA.Bind(aSIm, aSIm);
        }
      }
    }
    //
    else {
      aExp.Init(aSA, aTmin);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape aSIm=aExp.Current();
        aDMSSA.Bind(aSIm, aSIm);
        if (aTmin==TopAbs_SOLID) {
          iX=myDS->Index(aSIm);
          bHasInterf=myDS->HasInterf(iX);
          if (!bHasInterf) {
            BOPTools_Set aST;
            //
            aST.Add(aSIm, TopAbs_FACE);
            //
            aDMSTS.Bind(aST, aSIm);
          }
        }
      }
    }
  } //for (; aItLS.More(); aItLS.Next())
  //
  const BOPCol_ListOfShape& aLT=(!bFlag1) ? myArguments : myTools;
  aItLS.Initialize(aLT);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aST=aItLS.Value();
    if (myImages.IsBound(aST)){
      const BOPCol_ListOfShape& aLSTIm=myImages.Find(aST);
      aItIm.Initialize(aLSTIm);
      for (; aItIm.More(); aItIm.Next()) {
        const TopoDS_Shape& aSTIm=aItIm.Value();
        aExp.Init(aSTIm, aTmin);
        for (; aExp.More(); aExp.Next()) {
          const TopoDS_Shape aSIm=aExp.Current();
          // skip degenerated edges
          if (aTmin==TopAbs_EDGE) {
            const TopoDS_Edge& aEIm=*((TopoDS_Edge*)&aSIm);
            if (BRep_Tool::Degenerated(aEIm)) {
              continue;
            }
          }
          //
          bIsBound=aDMSSA.IsBound(aSIm);
          if (!bFlag2) { // ie common
            if (bIsBound) {
              const TopoDS_Shape& aSImA=aDMSSA.Find(aSIm);
              aBB.Add(aC, aSImA);
            }
          }
          else {// ie cut or cut21
            if (!bIsBound) {
              aBB.Add(aC, aSIm);
            }
          }
        }
      }
    }// if (myImages.IsBound(aST)){
    else {
      aExp.Init(aST, aTmin);
      for (; aExp.More(); aExp.Next()) {
        const TopoDS_Shape aSIm=aExp.Current();
        // skip degenerated edges
        if (aTmin==TopAbs_EDGE) {
          const TopoDS_Edge& aEIm=*((TopoDS_Edge*)&aSIm);
          if (BRep_Tool::Degenerated(aEIm)) {
            continue;
          }
        }
        bIsBound=aDMSSA.IsBound(aSIm);
        if (!bFlag2) { // ie common
          if (bIsBound) {
            const TopoDS_Shape& aSImA=aDMSSA.Find(aSIm);
            aBB.Add(aC, aSImA);
          }
          else {
             if (aTmin==TopAbs_SOLID) {
               BOPTools_Set aST;
               //
               aST.Add(aSIm, TopAbs_FACE);
               //
               if (aDMSTS.IsBound(aST)) {
                 const TopoDS_Shape& aSImA=aDMSTS.Find(aST);
                 aBB.Add(aC, aSImA);
               }
             }
          }
        }
        else {// ie cut or cut21
          if (!bIsBound) {
            if (aTmin==TopAbs_SOLID) {
              BOPTools_Set aST;
              //
              aST.Add(aSIm, TopAbs_FACE);
              //
              bIsBound=aDMSTS.IsBound(aST); 
            }
            //
            if (!bIsBound) {
              aBB.Add(aC, aSIm);
            }
          }
        }
      }
    }
  } //for (; aItLS.More(); aItLS.Next())
  //
  // the squats around degeneracy
  if (aTmin!=TopAbs_EDGE) {
    myRC=aC;
    return;
  }
  //---------------------------------------------------------
  //
  // The squats around degenerated edges
  Standard_Integer i, aNbS, nVD;
  TopAbs_ShapeEnum aType;
  BOPCol_IndexedMapOfShape aMVC;
  //
  // 1. Vertices of aC
  BOPTools::MapShapes(aC, TopAbs_VERTEX, aMVC);
  //
  // 2. DE candidates
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    aType=aSI.ShapeType();
    if (aType!=aTmin) {
      continue;
    }
    //
    const TopoDS_Edge& aE=*((TopoDS_Edge*)&aSI.Shape());
    if (!BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    nVD=aSI.SubShapes().First();
    const TopoDS_Shape& aVD=myDS->Shape(nVD);
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
    BOPTools_AlgoTools::MakeConnexityBlocks
      (myRC, aT1, aT2, aLCB);
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
  
  else {//aDmin=3
    Standard_Integer aNbObjs, aNbTools;
    //
    aNbObjs=myArguments.Extent();
    aNbTools=myTools.Extent();
    //
    if (aNbObjs==1 && aNbTools==1) {
      if (myOperation==BOPAlgo_FUSE) {
        BuildSolid();
      }
      else {
        myShape=myRC;
      }
    }
    else {
      BuildSolid();
    }
  }
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
  BOPTools_DataMapOfSetShape aDMSTS;
  BOPTools_DataMapIteratorOfDataMapOfSetShape aItDMSTS;
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
        if (!aDMSTS.IsBound(aST)) {
          aDMSTS.Bind(aST, aSx);
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
  aItDMSTS.Initialize(aDMSTS);
  for (; aItDMSTS.More(); aItDMSTS.Next()) {
    const TopoDS_Shape& aSx=aItDMSTS.Value();
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
