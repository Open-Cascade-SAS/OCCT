// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <BOPAlgo_Builder.hxx>

#include <NCollection_IncAllocator.hxx>

#include <TopAbs_State.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
//
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
//
#include <BOPDS_DS.hxx>
#include <BOPDS_ShapeInfo.hxx>
//
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
//
#include <BOPTools_MapOfSet.hxx>
#include <BOPTools_Set.hxx>
//
#include <BOPAlgo_BuilderSolid.hxx>


static
  void OwnInternalShapes(const TopoDS_Shape& ,
                         BOPCol_IndexedMapOfShape& );

//=======================================================================
//function : FillImagesSolids
//purpose  : 
//=======================================================================
  void BOPAlgo_Builder::FillImagesSolids()
{
  myErrorStatus=0;
  //
  Handle(NCollection_IncAllocator) aAllocator;
  //-----------------------------------------------------scope_1 f
  aAllocator=new NCollection_IncAllocator();
  //
  BOPCol_DataMapOfShapeListOfShape theInParts(100, aAllocator);
  BOPCol_DataMapOfShapeShape theDraftSolids(100, aAllocator);
  //
  FillIn3DParts(theInParts, theDraftSolids, aAllocator);
  BuildSplitSolids(theInParts, theDraftSolids, aAllocator);
  FillInternalShapes();
  //
  theInParts.Clear();
  theDraftSolids.Clear();
  //-----------------------------------------------------scope_1 t
}
//=======================================================================
//function : FillIn3DParts
//purpose  : 
//=======================================================================
  void BOPAlgo_Builder::FillIn3DParts(BOPCol_DataMapOfShapeListOfShape& theInParts,
                                      BOPCol_DataMapOfShapeShape& theDraftSolids,
                                      const Handle(NCollection_BaseAllocator)& theAllocator)
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsIN, bHasImage;
  Standard_Integer aNbS, aNbSolids, i, j, aNbFaces, aNbFP, aNbFPx, aNbFIN, aNbLIF, aNbEFP;
  TopAbs_ShapeEnum aType;  
  TopAbs_State aState;
  TopoDS_Iterator aIt, aItF;
  BRep_Builder aBB;
  TopoDS_Solid aSolidSp; 
  TopoDS_Face aFP;
  BOPCol_ListIteratorOfListOfShape aItS, aItFP, aItEx;	
  //
  BOPCol_ListOfShape aLIF(theAllocator);
  BOPCol_MapOfShape aMFDone(100, theAllocator);
  BOPCol_IndexedMapOfShape aMSolids(100, theAllocator);
  BOPCol_IndexedMapOfShape aMFaces(100, theAllocator);
  BOPCol_IndexedMapOfShape aMFIN(100, theAllocator);
  BOPCol_IndexedMapOfShape aMS(100, theAllocator);
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF(100, theAllocator);
  //
  theDraftSolids.Clear();
  //
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    const TopoDS_Shape& aS=aSI.Shape();
    //
    aType=aSI.ShapeType();
    switch(aType) {
      case TopAbs_SOLID: {
        aMSolids.Add(aS);
        break;
      }
      //
      case TopAbs_FACE: {
        // all faces (originals or images)
        if (myImages.IsBound(aS)) {
          const BOPCol_ListOfShape& aLS=myImages.Find(aS);
          aItS.Initialize(aLS);
          for (; aItS.More(); aItS.Next()) {
            const TopoDS_Shape& aFx=aItS.Value();
            aMFaces.Add(aFx);
          }
        }
        else {
          aMFaces.Add(aS);
        }
        break;
      }
      //
      default:
        break;
    }
  }
  //
  aNbFaces=aMFaces.Extent();
  aNbSolids=aMSolids.Extent();
  //
  for (i=1; i<=aNbSolids; ++i) {
    const TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aMSolids(i)));
    //
    aMFDone.Clear();
    aMFIN.Clear();
    aMEF.Clear();
    //
    aBB.MakeSolid(aSolidSp);
    // 
    // Draft solid and its pure internal faces => aSolidSp, aLIF
    aLIF.Clear();
    BuildDraftSolid(aSolid, aSolidSp, aLIF);
    aNbLIF=aLIF.Extent();
    //
    // 1 all faces/edges from aSolid [ aMS ]
    bHasImage=Standard_False;
    aMS.Clear();
    aIt.Initialize(aSolid);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aShell=aIt.Value();
      //
      if (myImages.IsBound(aShell)) {
        bHasImage=Standard_True;
        //
        const BOPCol_ListOfShape& aLS=myImages.Find(aShell);
        aItS.Initialize(aLS);
        for (; aItS.More(); aItS.Next()) {
          const TopoDS_Shape& aSx=aItS.Value();
          aMS.Add(aSx);
          BOPTools::MapShapes(aSx, TopAbs_FACE, aMS);
          BOPTools::MapShapes(aSx, TopAbs_EDGE, aMS);
          BOPTools::MapShapesAndAncestors(aSx, TopAbs_EDGE, TopAbs_FACE, aMEF);
        }
      }
      else {
        //aMS.Add(aShell);
        BOPTools::MapShapes(aShell, TopAbs_FACE, aMS);
        BOPTools::MapShapesAndAncestors(aShell, TopAbs_EDGE, TopAbs_FACE, aMEF);
      }
    }
    //
    // 2 all faces that are not from aSolid [ aLFP1 ]
    BOPCol_IndexedDataMapOfShapeListOfShape aMEFP(100, theAllocator);
    BOPCol_ListOfShape aLFP1(theAllocator);
    BOPCol_ListOfShape aLFP(theAllocator);
    BOPCol_ListOfShape aLCBF(theAllocator);
    BOPCol_ListOfShape aLFIN(theAllocator);
    BOPCol_ListOfShape aLEx(theAllocator);
    //
    // for all non-solid faces build EF map [ aMEFP ]
    for (j=1; j<=aNbFaces; ++j) {
      const TopoDS_Shape& aFace=aMFaces(j);
      if (!aMS.Contains(aFace)) {
        BOPTools::MapShapesAndAncestors(aFace, TopAbs_EDGE, TopAbs_FACE, aMEFP);
      }
    }
    //
    // among all faces from aMEFP select these that have same edges
    // with the solid (i.e aMEF). These faces will be treated first 
    // to prevent the usage of 3D classifier.
    // The full list of faces to process is aLFP1. 
    aNbEFP=aMEFP.Extent();
    for (j=1; j<=aNbEFP; ++j) {
      const TopoDS_Shape& aE=aMEFP.FindKey(j);
      //
      if (aMEF.Contains(aE)) { // !!
        const BOPCol_ListOfShape& aLF=aMEFP(j);
        aItFP.Initialize(aLF);
        for (; aItFP.More(); aItFP.Next()) {
          const TopoDS_Shape& aF=aItFP.Value();
          if (aMFDone.Add(aF)) {
            aLFP1.Append(aF);
          }
        }
      }
      else {
        aLEx.Append(aE);
      }
    }
    //
    aItEx.Initialize(aLEx);
    for (; aItEx.More(); aItEx.Next()) {
      const TopoDS_Shape& aE=aItEx.Value();
      const BOPCol_ListOfShape& aLF=aMEFP.FindFromKey(aE);
      aItFP.Initialize(aLF);
      for (; aItFP.More(); aItFP.Next()) {
        const TopoDS_Shape& aF=aItFP.Value();
        if (aMFDone.Add(aF)) {
          //aLFP2.Append(aF);
          aLFP1.Append(aF);
        }
      }
    }
    //
    //==========
    //
    // 3 Process faces aLFP1
    aMFDone.Clear();
    aNbFP=aLFP1.Extent();
    aItFP.Initialize(aLFP1);
    for (; aItFP.More(); aItFP.Next()) {
      const TopoDS_Shape& aSP=aItFP.Value();
      if (!aMFDone.Add(aSP)) {
        continue;
      }
      
      //
      // first face to process
      aFP=(*(TopoDS_Face*)(&aSP));
      bIsIN=BOPTools_AlgoTools::IsInternalFace(aFP, aSolidSp, aMEF, 1.e-14, myContext);
      aState=(bIsIN) ? TopAbs_IN : TopAbs_OUT;
      //
      // collect faces to process [ aFP is the first ]
      aLFP.Clear();
      aLFP.Append(aFP);
      aItS.Initialize(aLFP1);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aSk=aItS.Value();
        if (!aMFDone.Contains(aSk)) {
          aLFP.Append(aSk);
        }
      }
      //
      // Connexity Block that spreads from aFP the Bound 
      // or till the end of the block itself
      aLCBF.Clear();
      BOPTools_AlgoTools::MakeConnexityBlock(aLFP, aMS, aLCBF, theAllocator);
      //
      // fill states for the Connexity Block 
      aItS.Initialize(aLCBF);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aSx=aItS.Value();
        aMFDone.Add(aSx);
        if (aState==TopAbs_IN) {
          aMFIN.Add(aSx);
        }
      }
      //
      aNbFPx=aMFDone.Extent();
      if (aNbFPx==aNbFP) {
        break;
      }
    }//for (; aItFP.More(); aItFP.Next())
    //
    // faces Inside aSolid
    aLFIN.Clear();
    aNbFIN=aMFIN.Extent();
    if (aNbFIN || aNbLIF) {
      for (j=1; j<=aNbFIN; ++j) {
        const TopoDS_Shape& aFIn=aMFIN(j);
        aLFIN.Append(aFIn);
      }
      //
      aItS.Initialize(aLIF);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aFIN=aItS.Value();
        aLFIN.Append(aFIN);
      }
      //
      theInParts.Bind(aSolid, aLFIN);
    }
    if (aNbFIN || bHasImage) {
      theDraftSolids.Bind(aSolid, aSolidSp);
    }
  }// for (; aItMS.More(); aItMS.Next()) {
}
//=======================================================================
//function : BuildDraftSolid
//purpose  : 
//=======================================================================
  void BOPAlgo_Builder::BuildDraftSolid(const TopoDS_Shape& theSolid,
                                        TopoDS_Shape& theDraftSolid,
                                        BOPCol_ListOfShape& theLIF)
{
  myErrorStatus=0;
  //
  Standard_Boolean bToReverse;
  Standard_Integer iFlag;
  TopAbs_Orientation aOrF, aOrSh, aOrSd;
  TopoDS_Iterator aIt1, aIt2;
  TopoDS_Shell aShD;
  TopoDS_Shape aFSDx, aFx;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aItS;	
  //
  aOrSd=theSolid.Orientation();
  theDraftSolid.Orientation(aOrSd);
  //
  aIt1.Initialize(theSolid);
  for (; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aSh=aIt1.Value();
    if(aSh.ShapeType()!=TopAbs_SHELL) {
      continue; // mb internal edges,vertices
    }
    //
    aOrSh=aSh.Orientation();
    aBB.MakeShell(aShD);
    aShD.Orientation(aOrSh);
    iFlag=0;
    //
    aIt2.Initialize(aSh);
    for (; aIt2.More(); aIt2.Next()) {
      const TopoDS_Shape& aF=aIt2.Value();
      aOrF=aF.Orientation();
      //
      if (myImages.IsBound(aF)) {
        const BOPCol_ListOfShape& aLSp=myImages.Find(aF);
        aItS.Initialize(aLSp);
        for (; aItS.More(); aItS.Next()) {
          aFx=aItS.Value();
          //
          if (myShapesSD.IsBound(aFx)) {
            aFSDx=myShapesSD.Find(aFx);
            //
            if (aOrF==TopAbs_INTERNAL) {
              aFSDx.Orientation(aOrF);
              theLIF.Append(aFSDx);
            }
            else {
              bToReverse=BOPTools_AlgoTools::IsSplitToReverse(aFSDx, aF, myContext); 
              if (bToReverse) {
                aFSDx.Reverse();
              }
              //
              iFlag=1;
              aBB.Add(aShD, aFSDx);
            }
          }//if (myShapesSD.IsBound(aFx)) {
          else {
            aFx.Orientation(aOrF);
            if (aOrF==TopAbs_INTERNAL) {
              theLIF.Append(aFx);
            }
            else{
              iFlag=1;
              aBB.Add(aShD, aFx);
            }
          }
        }
      } // if (myImages.IsBound(aF)) { 
      //
      else {
        if (aOrF==TopAbs_INTERNAL) {
          theLIF.Append(aF);
        }
        else{
          iFlag=1;
          aBB.Add(aShD, aF);
        }
      }
    } //for (; aIt2.More(); aIt2.Next()) {
    //
    if (iFlag) {
      aBB.Add(theDraftSolid, aShD);
    }
  } //for (; aIt1.More(); aIt1.Next()) {
}
//=======================================================================
//function : BuildSplitSolids
//purpose  : 
//=======================================================================
  void BOPAlgo_Builder::BuildSplitSolids(BOPCol_DataMapOfShapeListOfShape& theInParts,
                                         BOPCol_DataMapOfShapeShape& theDraftSolids,
                                         const Handle(NCollection_BaseAllocator)& theAllocator)
{
  myErrorStatus=0;
  //
  Standard_Boolean bFlagSD;
  Standard_Integer i, aNbS, iErr, aNbSFS;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_DataMapIteratorOfDataMapOfShapeShape aIt1;
  //
  BOPCol_ListOfShape aSFS(theAllocator), aLSEmpty(theAllocator);
  BOPCol_MapOfShape aMFence(100, theAllocator);
  BOPTools_MapOfSet aMST(100, theAllocator);
  //
  // 0. Find same domain solids for non-interferred solids
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    //
    if (aSI.ShapeType()!=TopAbs_SOLID) {
      continue;
    }
    //
    const TopoDS_Shape& aS=aSI.Shape();
    if (!aMFence.Add(aS)) {
      continue;
    }
    if(theDraftSolids.IsBound(aS)) {
      continue;
    }
    //
    BOPTools_Set aST;
    //
    aST.Add(aS, TopAbs_FACE);
    aMST.Add(aST);
    //
  } //for (i=1; i<=aNbS; ++i) 
  //
  // 1. Build solids for interferred source solids
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    //
    if (aSI.ShapeType()!=TopAbs_SOLID) {
      continue;
    }
    //
    const TopoDS_Shape& aS=aSI.Shape();
    if(!theDraftSolids.IsBound(aS)) {
      continue;
    }
    const TopoDS_Shape& aSD=theDraftSolids.Find(aS);
    const BOPCol_ListOfShape& aLFIN=
      (theInParts.IsBound(aS)) ? theInParts.Find(aS) : aLSEmpty;
    //
    // 1.1 Fill Shell Faces Set
    aSFS.Clear();
    aExp.Init(aSD, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aF=aExp.Current();
      aSFS.Append(aF);
    }
    //
    aIt.Initialize(aLFIN);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape aF=aIt.Value();
      //
      aF.Orientation(TopAbs_FORWARD);
      aSFS.Append(aF);
      aF.Orientation(TopAbs_REVERSED);
      aSFS.Append(aF);
    }
    //
    aNbSFS=aSFS.Extent();
    //DEB f
    // <-A
    //DEB t
    //
    // 1.3 Build new solids
    BOPAlgo_BuilderSolid aSB(theAllocator);
    //
    //aSB.SetContext(myContext);
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
    // 1.4 Collect resulting solids and theirs set of faces. 
    //     Update Images.
    if (!myImages.IsBound(aS)) {
      BOPCol_ListOfShape aLSx;
      //
      myImages.Bind(aS, aLSx);
      BOPCol_ListOfShape& aLSIm=myImages.ChangeFind(aS);
      //
      aIt.Initialize(aLSR);
      for (; aIt.More(); aIt.Next()) {
        BOPTools_Set aST;
        //
        const TopoDS_Shape& aSR=aIt.Value();
        aST.Add(aSR, TopAbs_FACE);
        //
        bFlagSD=aMST.Contains(aST);
        //
        const BOPTools_Set& aSTx=aMST.Added(aST);
        const TopoDS_Shape& aSx=aSTx.Shape();
        aLSIm.Append(aSx);
        //
        if (bFlagSD) {
          myShapesSD.Bind(aSR, aSx);
        }
      }
    }
  } // for (; aIt1.More(); aIt1.Next()) {
}

//=======================================================================
//function :FillInternalShapes 
//purpose  : 
//=======================================================================
  void BOPAlgo_Builder::FillInternalShapes()
{
  myErrorStatus=0;
  //
  Standard_Integer i, j,  aNbS, aNbSI, aNbSx, aNbSd;
  TopAbs_ShapeEnum aType;
  TopAbs_State aState; 
  TopoDS_Iterator aItS;
  BRep_Builder aBB;
  BOPCol_MapIteratorOfMapOfShape aItM;
  BOPCol_ListIteratorOfListOfShape aIt, aIt1;
  //
  Handle(NCollection_IncAllocator) aAllocator;
  //-----------------------------------------------------scope f
  aAllocator=new NCollection_IncAllocator();
  //
  BOPCol_IndexedDataMapOfShapeListOfShape aMSx(100, aAllocator);
  BOPCol_IndexedMapOfShape aMx(100, aAllocator);
  BOPCol_MapOfShape aMSI(100, aAllocator);
  BOPCol_MapOfShape aMFence(100, aAllocator);
  BOPCol_MapOfShape aMSOr(100, aAllocator);
  BOPCol_ListOfShape aLSd(aAllocator);
  BOPCol_ListOfShape aLArgs(aAllocator);
  //
  // 1. Shapes to process
  //
  // 1.1 Shapes from pure arguments aMSI 
  // 1.1.1 vertex, edge, wire
  //
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    if (!aMFence.Add(aS)) {
      continue;
    }
    //
    aType=aS.ShapeType();
    if (aType==TopAbs_WIRE) {
      aItS.Initialize(aS);
      for(; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aE=aItS.Value();
        if (aMFence.Add(aE)) {
          aLArgs.Append(aE);
        }
      }
    }
    else if (aType==TopAbs_VERTEX || aType==TopAbs_EDGE){
      aLArgs.Append(aS);
    } 
  }
  aMFence.Clear();
  //
  aIt.Initialize(aLArgs);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aType=aS.ShapeType();
    if (aType==TopAbs_VERTEX || aType==TopAbs_EDGE ||aType==TopAbs_WIRE) {
      if (aMFence.Add(aS)) {
        if (myImages.IsBound(aS)) {
          const BOPCol_ListOfShape &aLSp=myImages.Find(aS);
          aIt1.Initialize(aLSp);
          for (; aIt1.More(); aIt1.Next()) {
            const TopoDS_Shape& aSp=aIt1.Value();
            aMSI.Add(aSp);
          }
        }
        else {
          aMSI.Add(aS);
        }
      }
    }
  }
  
  aNbSI=aMSI.Extent();
  //
  // 2. Internal vertices, edges from source solids
  aMFence.Clear();
  aLSd.Clear();
  //
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    //
    if (aSI.ShapeType()!=TopAbs_SOLID) {
      continue;
    }
    //
    const TopoDS_Shape& aS=aSI.Shape();
    //
    aMx.Clear();
    OwnInternalShapes(aS, aMx);
    //
    aNbSx=aMx.Extent();
    for (j=1; j<=aNbSx; ++j) {
      const TopoDS_Shape& aSi=aMx(j);
      if (myImages.IsBound(aSi)) {
        const BOPCol_ListOfShape &aLSp=myImages.Find(aSi);
        aIt1.Initialize(aLSp);
        for (; aIt1.More(); aIt1.Next()) {
          const TopoDS_Shape& aSp=aIt1.Value();
          aMSI.Add(aSp);
        }
      }
      else {
        aMSI.Add(aSi);
      }
    }
    //
    // build aux map from splits of solids
    if (myImages.IsBound(aS)) {
      const BOPCol_ListOfShape &aLSp=myImages.Find(aS);
      aIt.Initialize(aLSp);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aSp=aIt.Value();
        if (aMFence.Add(aSp)) { 
          BOPTools::MapShapesAndAncestors(aSp, TopAbs_VERTEX, TopAbs_EDGE, aMSx);
          BOPTools::MapShapesAndAncestors(aSp, TopAbs_VERTEX, TopAbs_FACE, aMSx);
          BOPTools::MapShapesAndAncestors(aSp, TopAbs_EDGE  , TopAbs_FACE, aMSx);
          aLSd.Append(aSp);
        }
      }
    }
    else {
      if (aMFence.Add(aS)) {
        BOPTools::MapShapesAndAncestors(aS, TopAbs_VERTEX, TopAbs_EDGE, aMSx);
        BOPTools::MapShapesAndAncestors(aS, TopAbs_VERTEX, TopAbs_FACE, aMSx);
        BOPTools::MapShapesAndAncestors(aS, TopAbs_EDGE  , TopAbs_FACE, aMSx);
        aLSd.Append(aS);
        aMSOr.Add(aS); 
      }
    }
  }// for (i=0; i<aNbS; ++i) {
  //
  aNbSd=aLSd.Extent();
  //
  // 3. Some shapes of aMSI can be already tied with faces of 
  //    split solids
  aItM.Initialize(aMSI); 
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aSI=aItM.Key();
    if (aMSx.Contains(aSI)) {
      const BOPCol_ListOfShape &aLSx=aMSx.FindFromKey(aSI);
      aNbSx=aLSx.Extent();
      if (aNbSx) {
        aMSI.Remove(aSI);
      }
    }
  }
  //
  // 4. Just check it
  aNbSI=aMSI.Extent();
  if (!aNbSI) {
    return;
  }
  //
  // 5 Settle internal vertices and edges into solids
  aMx.Clear();
  aIt.Initialize(aLSd);
  for (; aIt.More(); aIt.Next()) {
    TopoDS_Solid aSd=TopoDS::Solid(aIt.Value());
    //
    aItM.Initialize(aMSI); 
    for (; aItM.More(); aItM.Next()) {
      TopoDS_Shape aSI=aItM.Key();
      aSI.Orientation(TopAbs_INTERNAL);
      //
      aState=BOPTools_AlgoTools::ComputeStateByOnePoint(aSI, aSd, 1.e-11, myContext);
      if (aState==TopAbs_IN) {
        //
        if(aMSOr.Contains(aSd)) {
          //
          TopoDS_Solid aSdx;
          //
          aBB.MakeSolid(aSdx);
          aItS.Initialize(aSd);
          for (; aItS.More(); aItS.Next()) {
            const TopoDS_Shape& aSh=aItS.Value();
            aBB.Add(aSdx, aSh);
          }
          //
          aBB.Add(aSdx, aSI);
          //
          if (myImages.IsBound(aSdx)) {
            BOPCol_ListOfShape& aLS=myImages.ChangeFind(aSdx);
            aLS.Append(aSdx);
          } 
          else {
            BOPCol_ListOfShape aLS;
            aLS.Append(aSdx);
            myImages.Bind(aSd, aLS);
          }
          //
          aMSOr.Remove(aSd);
          aSd=aSdx;
        }
        else {
          aBB.Add(aSd, aSI);
        }
        //
        aMSI.Remove(aSI);
      } //if (aState==TopAbs_IN) {
    }// for (; aItM.More(); aItM.Next()) {
  }//for (; aIt1.More(); aIt1.Next()) {
  //
  //-----------------------------------------------------scope t
  aLArgs.Clear();
  aLSd.Clear();
  aMSOr.Clear();
  aMFence.Clear();
  aMSI.Clear();
  aMx.Clear();
  aMSx.Clear();
}
//=======================================================================
//function : OwnInternalShapes
//purpose  : 
//=======================================================================
  void OwnInternalShapes(const TopoDS_Shape& theS,
                         BOPCol_IndexedMapOfShape& theMx)
{
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(theS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    if (aSx.ShapeType()!=TopAbs_SHELL) {
      theMx.Add(aSx);
    }
  }
}
//
// ErrorStatus
// 30 - SolidBuilder failed
// A
/*
    {
      TopoDS_Compound aCx;
      BRep_Builder aBBx;
      BOPCol_ListIteratorOfListOfShape aItx;
      //
      aBBx.MakeCompound(aCx);
      aItx.Initialize(aSFS1);
      for (; aItx.More(); aItx.Next()) {
      const TopoDS_Shape& aFx=aItx.Value();
      aBBx.Add(aCx, aFx);
      }
      BRepTools::Write(aCx, "cxso");
      int a=0;
    }
    */
