// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <BOPAlgo_Builder.hxx>

#include <NCollection_IncAllocator.hxx>

#include <TopAbs_State.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
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

#include <BOPCol_DataMapOfIntegerShape.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

#include <NCollection_UBTreeFiller.hxx>
#include <BOPDS_BoxBndTree.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPInt_Context.hxx>


static
  Standard_Boolean IsClosedShell(const TopoDS_Shell& aSh);

static
  void OwnInternalShapes(const TopoDS_Shape& ,
                         BOPCol_IndexedMapOfShape& );

static
  void TreatCompound(const TopoDS_Shape& theS,
                     BOPCol_MapOfShape& aMFence,
                     BOPCol_ListOfShape& theLS);

//=======================================================================
//class     : BOPAlgo_ShapeBox
//purpose   : Auxiliary class
//=======================================================================
class BOPAlgo_ShapeBox {
 public:
  BOPAlgo_ShapeBox() {
  };
  //
  ~BOPAlgo_ShapeBox() {
  };
  //
  void SetShape(const TopoDS_Shape& aS) {
    myShape=aS;
  };
  //
  const TopoDS_Shape& Shape()const {
    return myShape;
  };
  //
  void SetBox(const Bnd_Box& aBox) {
    myBox=aBox;
  };
  //
  const Bnd_Box& Box()const {
    return myBox;
  };
  //
 protected:
  TopoDS_Shape myShape;
  Bnd_Box myBox;
};
//
typedef NCollection_DataMap\
  <Standard_Integer, BOPAlgo_ShapeBox, TColStd_MapIntegerHasher> \
  BOPAlgo_DataMapOfIntegerShapeBox; 
//
typedef BOPAlgo_DataMapOfIntegerShapeBox::Iterator \
  BOPAlgo_DataMapIteratorOfDataMapOfIntegerShapeBox; 
// 

//=======================================================================
//function : FillImagesSolids
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::FillImagesSolids()
{
  Standard_Boolean bHasSolids;
  Standard_Integer i, aNbS;
  //
  myErrorStatus=0;
  //
  bHasSolids=Standard_False;
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.ShapeType()==TopAbs_SOLID) {
      bHasSolids=!bHasSolids;
      break;
    }
  }
  //
  if (!bHasSolids) {
    return;
  }
  //
  Handle(NCollection_IncAllocator) aAlr;
  //
  aAlr=new NCollection_IncAllocator();
  //
  BOPCol_DataMapOfShapeListOfShape theInParts(100, aAlr);
  BOPCol_DataMapOfShapeShape theDraftSolids(100, aAlr);
  //
  FillIn3DParts(theInParts, theDraftSolids, aAlr);
  BuildSplitSolids(theInParts, theDraftSolids, aAlr);
  FillInternalShapes();
  //
  theInParts.Clear();
  theDraftSolids.Clear();
}
//=======================================================================
//function : FillIn3DParts
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::FillIn3DParts(BOPCol_DataMapOfShapeListOfShape& theInParts,
				    BOPCol_DataMapOfShapeShape& theDraftSolids,
				    const BOPCol_BaseAllocator& )
{
  Standard_Boolean bHasImage;
  Standard_Integer i, k, aNbS, aNbLIF, nFP, aNbFP, aNbFIN, iIsIN;
  TopoDS_Solid aSD;
  TopoDS_Iterator aIt;
  BRep_Builder aBB; 
  BOPCol_ListIteratorOfListOfInteger aItLI, aItLI1;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPAlgo_ShapeBox aSB;
  Handle(NCollection_IncAllocator) aAlr0;
  //
  aAlr0=new NCollection_IncAllocator();
  BOPAlgo_DataMapOfIntegerShapeBox aDMISB(100, aAlr0);
  BOPAlgo_DataMapIteratorOfDataMapOfIntegerShapeBox aItDMISB;
  //
  myErrorStatus=0;
  theDraftSolids.Clear();
  //
  // 1. aDMISB map Index/FaceBox
  k=0;
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    //
    const TopoDS_Shape& aS=aSI.Shape();
    //
    if (myImages.IsBound(aS)) {
      const BOPCol_ListOfShape& aLS=myImages.Find(aS);
      aItLS.Initialize(aLS);
      for (; aItLS.More(); aItLS.Next()) {
	const TopoDS_Shape& aSx=aItLS.Value();
	//
	Bnd_Box aBox;
	BRepBndLib::Add(aSx, aBox);
	//
	aSB.SetShape(aSx);
	aSB.SetBox(aBox);
	//
	aDMISB.Bind(k, aSB);
	++k;
      }
    }
    else {
      const Bnd_Box& aBox=aSI.Box();
      //
      aSB.SetShape(aS);
      aSB.SetBox(aBox);
      //
      aDMISB.Bind(k, aSB);
      ++k;
    }
  }//for (i=0; i<aNbS; ++i) {
  //
  // 2. Solids
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.ShapeType()!=TopAbs_SOLID) {
      continue;
    }
    // 
    //---------------------------------------------
    Handle(NCollection_IncAllocator) aAlr1;
    //
    aAlr1=new NCollection_IncAllocator();
    //
    BOPCol_ListOfShape aLFIN(aAlr1);
    BOPCol_ListOfShape aLIF(aAlr1);
    BOPCol_IndexedMapOfShape aMF(100, aAlr1);
    BOPCol_IndexedDataMapOfShapeListOfShape aMEF(100, aAlr1);
    //
    BOPDS_BoxBndTreeSelector aSelector;
    BOPDS_BoxBndTree aBBTree;
    NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
    Bnd_Box aBoxS;
    //
    const TopoDS_Shape& aS=aSI.Shape();
    const TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aS));
    //
    // 2.0 Flag bHasImage
    bHasImage=Standard_False;
    aIt.Initialize(aS);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aShell=aIt.Value();
      bHasImage=myImages.IsBound(aShell);
      if (bHasImage){
	break;
      }
    }
    //
    // 2.1 Compute Bnd_Box for the solid aS  [ aBoxS ]
    BuildBndBox(i, aBoxS);
    //-----
    //
    // 2.2 Build Draft Solid [aSD]
    aBB.MakeSolid(aSD);
    //
    BuildDraftSolid(aSolid, aSD, aLIF);
    aNbLIF=aLIF.Extent();
    //
    BOPTools::MapShapesAndAncestors(aSD, TopAbs_EDGE, TopAbs_FACE, aMEF);
    //
    // 2.3 Faces from aSD and own internal faces => aMF 
    BOPTools::MapShapes(aSD, TopAbs_FACE, aMF);
    //
    aItLS.Initialize(aLIF);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aFI=aItLS.Value();
      aMF.Add(aFI);
    }
    //
    // 2.4. Prepare TreeFiller
    aItDMISB.Initialize(aDMISB);
    for (; aItDMISB.More(); aItDMISB.Next()) {
      k=aItDMISB.Key();
      const BOPAlgo_ShapeBox& aSBk=aItDMISB.Value();
      const TopoDS_Shape& aFk=aSBk.Shape();
      if (aMF.Contains(aFk)) {
	continue;
      }
      //
      const Bnd_Box& aBk=aSBk.Box();
      //
      aTreeFiller.Add(k, aBk);
    }
    //
    // 2.5. Shake TreeFiller
    aTreeFiller.Fill();
    //
    // 2.6. Select boxes of faces that are not out of aBoxS
    aSelector.Clear();
    aSelector.SetBox(aBoxS);
    //
    aNbFP=aBBTree.Select(aSelector);
    //
    const BOPCol_ListOfInteger& aLIFP=aSelector.Indices();
    //
    // 2.7. Collect faces that are IN aSolid [ aLFIN ]
    BOPCol_ListOfShape aLFP(aAlr1);
    BOPCol_ListOfShape aLCBF(aAlr1);
    BOPCol_MapOfShape aMFDone(100, aAlr1);
    BOPCol_IndexedMapOfShape aME(100, aAlr1);
    //
    BOPTools::MapShapes(aSD, TopAbs_EDGE, aME);
    //
    aItLI.Initialize(aLIFP);
    for (; aItLI.More(); aItLI.Next()) {
      nFP=aItLI.Value();
      const BOPAlgo_ShapeBox& aSBF=aDMISB.Find(nFP);
      const TopoDS_Face& aFP=(*(TopoDS_Face*)&aSBF.Shape());
      if (aMFDone.Contains(aFP)) {
	continue;
      }
      //
      aMFDone.Add(aFP);
      //
      iIsIN=BOPTools_AlgoTools::IsInternalFace(aFP, aSD, aMEF, 1.e-14, myContext);
      //
      aLFP.Clear();
      aLFP.Append(aFP);
      //
      aItLI1.Initialize(aLIFP);
      for (; aItLI1.More(); aItLI1.Next()) {
	const TopoDS_Shape& aFx=aDMISB.Find(aItLI1.Value()).Shape();
	if (!aMFDone.Contains(aFx)) {
	  aLFP.Append(aFx);
	}
      }
      //
      aLCBF.Clear();
      //---------------------------------------- 
      {
	Handle(NCollection_IncAllocator) aAlr2;
	aAlr2=new NCollection_IncAllocator();
	//
	BOPTools_AlgoTools::MakeConnexityBlock(aLFP, aME, aLCBF, aAlr2);
      }
      //----------------------------------------
      aItLS.Initialize(aLCBF);
      for (; aItLS.More(); aItLS.Next()) {
        const TopoDS_Shape& aFx=aItLS.Value();
        aMFDone.Add(aFx);
        if (iIsIN) {
          aLFIN.Append(aFx);
        }
      }
    }// for (; aItLI.More(); aItLI.Next()) {
    //
    // 2.8. Store the results in theInParts, theDraftSolids
    aNbFIN=aLFIN.Extent();
    if (aNbFIN || aNbLIF) {
      aItLS.Initialize(aLIF);
      for (; aItLS.More(); aItLS.Next()) {
	const TopoDS_Shape& aFI=aItLS.Value();
	aLFIN.Append(aFI);
      }
      theInParts.Bind(aSolid, aLFIN);
    }
    //
    if (aNbFIN || bHasImage) {
      theDraftSolids.Bind(aSolid, aSD);
    }
    //---------------------------------------------
  }// for (i=0; i<aNbS; ++i) {
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
				       const BOPCol_BaseAllocator&  )
{
  myErrorStatus=0;
  //
  Standard_Boolean bFlagSD;
  Standard_Integer i, aNbS, iErr, aNbSFS;
  TopExp_Explorer aExp;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_DataMapIteratorOfDataMapOfShapeShape aIt1;
  //
  Handle(NCollection_IncAllocator) aAlr0;
  aAlr0=new NCollection_IncAllocator();
  //
  BOPCol_ListOfShape aSFS(aAlr0), aLSEmpty(aAlr0);
  BOPCol_MapOfShape aMFence(100, aAlr0);
  BOPTools_MapOfSet aMST(100, aAlr0);
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
    //
    // 1.3 Build new solids   
    Handle(NCollection_IncAllocator) aAlr1;
    aAlr1=new NCollection_IncAllocator();  
    //
    BOPAlgo_BuilderSolid aSB(aAlr1);
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
  }// for (i=0; i<aNbS; ++i) {
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
  BOPCol_ListOfShape aLSC(aAllocator);
  //
  // 1. Shapes to process
  //
  // 1.1 Shapes from pure arguments aMSI 
  // 1.1.1 vertex, edge, wire
  //
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    TreatCompound(aS, aMFence, aLSC);
  }
  aIt.Initialize(aLSC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
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
//function : BuildBndBox
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::BuildBndBox(const Standard_Integer theIndex,
				  Bnd_Box& aBoxS)
{
  Standard_Boolean bIsOpenBox;
  Standard_Integer nSh, nFc;
  Standard_Real aTolS, aTolFc;
  TopAbs_State aState; 
  BOPCol_ListIteratorOfListOfInteger aItLI, aItLI1;
  //
  const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(theIndex);
  const TopoDS_Shape& aS=aSI.Shape();
  const TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aS));
  //
  bIsOpenBox=Standard_False;
  //
  aTolS=0.;
  const BOPCol_ListOfInteger& aLISh=aSI.SubShapes();
  aItLI.Initialize(aLISh);
  for (; aItLI.More(); aItLI.Next()) {
    nSh=aItLI.Value();
    const BOPDS_ShapeInfo& aSISh=myDS->ShapeInfo(nSh);
    if (aSISh.ShapeType()!=TopAbs_SHELL) {
      continue;
    }
    //
    const BOPCol_ListOfInteger& aLIFc=aSISh.SubShapes();
    aItLI1.Initialize(aLIFc);
    for (; aItLI1.More(); aItLI1.Next()) {
      nFc=aItLI1.Value();
      const BOPDS_ShapeInfo& aSIFc=myDS->ShapeInfo(nFc);
      if (aSIFc.ShapeType()!=TopAbs_FACE) {
	continue;
      }
      //
      const Bnd_Box& aBFc=aSIFc.Box();
      aBoxS.Add(aBFc);
      //
      if (!bIsOpenBox) {
	bIsOpenBox=(aBFc.IsOpenXmin() || aBFc.IsOpenXmax() ||
		    aBFc.IsOpenYmin() || aBFc.IsOpenYmax() ||
		    aBFc.IsOpenZmin() || aBFc.IsOpenZmax()); 
	if (bIsOpenBox) {
	  break;
	}
      }
      //
      const TopoDS_Face& aFc=*((TopoDS_Face*)&aSIFc.Shape());
      aTolFc=BRep_Tool::Tolerance(aFc);
      if (aTolFc>aTolS) {
	aTolS=aTolFc;
      }
    }//for (; aItLI1.More(); aItLI1.Next()) {
    if (bIsOpenBox) {
      break;
    }
    //
    const TopoDS_Shell& aSh=*((TopoDS_Shell*)&aSISh.Shape());
    bIsOpenBox=IsClosedShell(aSh);
    if (bIsOpenBox) {
      break;
    }
  }//for (; aItLI.More(); aItLI.Next()) {
  //
  if (bIsOpenBox) {
    aBoxS.SetWhole();
  }
  else {
    BRepClass3d_SolidClassifier& aSC=myContext->SolidClassifier(aSolid);
    aSC.PerformInfinitePoint(aTolS);
    aState=aSC.State();
    if (aState==TopAbs_IN) {
      aBoxS.SetWhole();
    }
  }
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
//=======================================================================
//function : IsClosedShell
//purpose  : 
//=======================================================================
Standard_Boolean IsClosedShell(const TopoDS_Shell& aSh)
{
  Standard_Boolean bRet;
  Standard_Integer i, aNbE, aNbF;
  TopAbs_Orientation aOrF;
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF; 
  BOPCol_ListIteratorOfListOfShape aItLS;
  //
  bRet=Standard_False;
  //
  BOPTools::MapShapesAndAncestors(aSh, TopAbs_EDGE, TopAbs_FACE, aMEF);
  // 
  aNbE=aMEF.Extent();
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Edge& aE=*((TopoDS_Edge*)&aMEF.FindKey(i));
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    aNbF=0;
    const BOPCol_ListOfShape& aLF=aMEF(i);
    aItLS.Initialize(aLF);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aF=aItLS.Value();
      aOrF=aF.Orientation();
      if (aOrF==TopAbs_INTERNAL || aOrF==TopAbs_EXTERNAL) {
	continue;
      }
      ++aNbF;
    }
    //
    if (aNbF==1) {
      bRet=!bRet; // True
      break;
    }
  }
  //
  return bRet;
}
//=======================================================================
//function : TreatCompound
//purpose  : 
//=======================================================================
void TreatCompound(const TopoDS_Shape& theS,
                   BOPCol_MapOfShape& aMFence,
                   BOPCol_ListOfShape& theLS)
{
  TopAbs_ShapeEnum aType;
  //
  aType = theS.ShapeType();
  if (aType != TopAbs_COMPOUND) {
    if (aMFence.Add(theS)) {
      theLS.Append(theS);
    }
    return;
  }
  //
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(theS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    TreatCompound(aS, aMFence, theLS);
  }
}

//
// ErrorStatus
// 30 - SolidBuilder failed
// A
