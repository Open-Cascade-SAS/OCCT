// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//
#include <BOPAlgo_Builder.hxx>
//
#include <Precision.hxx>
//
#include <Bnd_Box.hxx>
#include <TopAbs_State.hxx>
//
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
//
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
//
#include <BRep_Builder.hxx>
//
#include <BOPAlgo_Tools.hxx>
#include <BOPAlgo_BuilderSolid.hxx>
//
#include <IntTools_Context.hxx>
//
#include <BOPDS_DS.hxx>
#include <BOPDS_ShapeInfo.hxx>
//
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_MapOfSet.hxx>
#include <BOPTools_Set.hxx>
#include <BOPTools_Parallel.hxx>
//
#include <BOPAlgo_Tools.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Vector.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <algorithm>

static
  void OwnInternalShapes(const TopoDS_Shape& ,
                         TopTools_IndexedMapOfShape& );


//=======================================================================
//function : FillImagesSolids
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::FillImagesSolids()
{
  Standard_Boolean bHasSolids;
  Standard_Integer i, aNbS;
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
  Handle(NCollection_BaseAllocator) aAlr;
  //
  aAlr=NCollection_BaseAllocator::CommonBaseAllocator();
  //
  TopTools_DataMapOfShapeListOfShape theInParts(100, aAlr);
  TopTools_DataMapOfShapeShape theDraftSolids(100, aAlr);
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
void BOPAlgo_Builder::FillIn3DParts
  (TopTools_DataMapOfShapeListOfShape& theInParts,
   TopTools_DataMapOfShapeShape& theDraftSolids,
   const Handle(NCollection_BaseAllocator)& )
{
  Handle(NCollection_BaseAllocator) anAlloc = new NCollection_IncAllocator;

  // Find all faces that are IN solids

  // Store boxes of the shapes into a map
  TopTools_DataMapOfShapeBox aShapeBoxMap(1, anAlloc);

  // Fence map
  TopTools_MapOfShape aMFence(1, anAlloc);

  // Get all faces
  TopTools_ListOfShape aLFaces(anAlloc);

  Standard_Integer i, aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_FACE)
      continue;

    const TopoDS_Shape& aS = aSI.Shape();
    const TopTools_ListOfShape* pLSIm = myImages.Seek(aS);

    if (pLSIm)
    {
      TopTools_ListIteratorOfListOfShape aItLSIm(*pLSIm);
      for (; aItLSIm.More(); aItLSIm.Next())
      {
        const TopoDS_Shape& aSIm = aItLSIm.Value();
        if (aMFence.Add(aSIm))
          aLFaces.Append(aSIm);
      }
    }
    else
    {
      aLFaces.Append(aS);
      aShapeBoxMap.Bind(aS, aSI.Box());
    }
  }

  BRep_Builder aBB;

  // Get all solids
  TopTools_ListOfShape aLSolids(anAlloc);
  // Keep INTERNAL faces of the solids
  TopTools_DataMapOfShapeListOfShape aSolidsIF(1, anAlloc);
  // Draft solids
  TopTools_IndexedDataMapOfShapeShape aDraftSolid(1, anAlloc);

  for (i = 0; i < aNbS; ++i)
  {
    BOPDS_ShapeInfo& aSI = myDS->ChangeShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_SOLID)
      continue;

    const TopoDS_Shape& aS = aSI.Shape();
    const TopoDS_Solid& aSolid = (*(TopoDS_Solid*)(&aS));
    //
    // Bounding box for the solid aS
    Bnd_Box& aBoxS = aSI.ChangeBox();
    if (aBoxS.IsVoid())
      myDS->BuildBndBoxSolid(i, aBoxS, myCheckInverted);

    // Build Draft Solid
    TopTools_ListOfShape aLIF;
    TopoDS_Solid aSD;
    aBB.MakeSolid(aSD);
    BuildDraftSolid(aSolid, aSD, aLIF);

    aLSolids.Append(aSD);
    aSolidsIF.Bind(aSD, aLIF);
    aShapeBoxMap.Bind(aSD, aBoxS);
    aDraftSolid.Add(aS, aSD);
  }

  // Perform classification of the faces
  TopTools_IndexedDataMapOfShapeListOfShape anInParts;

  BOPAlgo_Tools::ClassifyFaces(aLFaces, aLSolids, myRunParallel,
                               myContext, anInParts, aShapeBoxMap, aSolidsIF);

  // Analyze the results of classification
  Standard_Integer aNbSol = aDraftSolid.Extent();
  for (i = 1; i <= aNbSol; ++i)
  {
    const TopoDS_Solid& aSolid = TopoDS::Solid(aDraftSolid.FindKey(i));
    const TopoDS_Solid& aSDraft = TopoDS::Solid(aDraftSolid(i));
    const TopTools_ListOfShape& aLInFaces = anInParts.FindFromKey(aSDraft);
    const TopTools_ListOfShape& aLInternal = aSolidsIF.Find(aSDraft);

    Standard_Integer aNbIN = aLInFaces.Extent();

    if (!aNbIN)
    {
      Standard_Boolean bHasImage = Standard_False;
      // Check if the shells of the solid have image
      for (TopoDS_Iterator it(aSolid); it.More() && !bHasImage; it.Next())
        bHasImage = myImages.IsBound(it.Value());

      if (!bHasImage)
        // no need to split the solid
        continue;
    }

    theDraftSolids.Bind(aSolid, aSDraft);

    Standard_Integer aNbInt = aLInternal.Extent();
    if (aNbInt || aNbIN)
    {
      // Combine the lists
      TopTools_ListOfShape *pLIN  = theInParts.Bound(aSolid, TopTools_ListOfShape());

      TopTools_ListIteratorOfListOfShape aItLS(aLInFaces);
      for (; aItLS.More(); aItLS.Next())
        pLIN->Append(aItLS.Value());

      aItLS.Initialize(aLInternal);
      for (; aItLS.More(); aItLS.Next())
        pLIN->Append(aItLS.Value());
    }
  }
}
//=======================================================================
//function : BuildDraftSolid
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::BuildDraftSolid(const TopoDS_Shape& theSolid,
                                      TopoDS_Shape& theDraftSolid,
                                      TopTools_ListOfShape& theLIF)
{
  Standard_Boolean bToReverse;
  Standard_Integer iFlag;
  TopAbs_Orientation aOrF, aOrSh, aOrSd;
  TopoDS_Iterator aIt1, aIt2;
  TopoDS_Shell aShD;
  TopoDS_Shape aFx;
  BRep_Builder aBB;
  TopTools_ListIteratorOfListOfShape aItS; 
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
        const TopTools_ListOfShape& aLSp=myImages.Find(aF);
        aItS.Initialize(aLSp);
        for (; aItS.More(); aItS.Next()) {
          aFx=aItS.Value();
          //
          if (myShapesSD.IsBound(aFx)) {
            //
            if (aOrF==TopAbs_INTERNAL) {
              aFx.Orientation(aOrF);
              theLIF.Append(aFx);
            }
            else {
              bToReverse=BOPTools_AlgoTools::IsSplitToReverseWithWarn
                (aFx, aF, myContext, myReport);
              if (bToReverse) {
                aFx.Reverse();
              }
              //
              iFlag=1;
              aBB.Add(aShD, aFx);
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
      aShD.Closed (BRep_Tool::IsClosed (aShD));
      aBB.Add(theDraftSolid, aShD);
    }
  } //for (; aIt1.More(); aIt1.Next()) {
}

//=======================================================================

//=======================================================================
//class : BOPAlgo_SplitSolid
//purpose  : Auxiliary class to extend the BOPAlgo_BuilderSolid with the solid to split
//=======================================================================
class BOPAlgo_SplitSolid : public BOPAlgo_BuilderSolid
{
public:
  //! Sets the solid
  void SetSolid(const TopoDS_Solid& theSolid) { mySolid = theSolid; }

  //! Returns the solid
  const TopoDS_Solid& Solid() const { return mySolid; }

private:
  TopoDS_Solid mySolid; //!< Solid to split
};

// Vector of Solid Builders
typedef NCollection_Vector<BOPAlgo_SplitSolid> BOPAlgo_VectorOfBuilderSolid;
// Functors to split solids
typedef BOPTools_Functor<BOPAlgo_SplitSolid,
                       BOPAlgo_VectorOfBuilderSolid> BOPAlgo_BuilderSolidFunctor;
//
typedef BOPTools_Cnt<BOPAlgo_BuilderSolidFunctor,
                   BOPAlgo_VectorOfBuilderSolid> BOPAlgo_BuilderSolidCnt;
//=======================================================================

//=======================================================================
//function : BuildSplitSolids
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::BuildSplitSolids
  (TopTools_DataMapOfShapeListOfShape& theInParts,
   TopTools_DataMapOfShapeShape& theDraftSolids,
   const Handle(NCollection_BaseAllocator)&  )
{
  Standard_Boolean bFlagSD;
  Standard_Integer i, aNbS;
  TopExp_Explorer aExp;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  Handle(NCollection_BaseAllocator) aAlr0;
  aAlr0=NCollection_BaseAllocator::CommonBaseAllocator();
  //
  TopTools_ListOfShape aSFS(aAlr0), aLSEmpty(aAlr0);
  TopTools_MapOfShape aMFence(100, aAlr0);
  BOPTools_MapOfSet aMST(100, aAlr0);
  BOPAlgo_VectorOfBuilderSolid aVBS;
  //
  // 0. Find same domain solids for non-interfered solids
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
  // Build temporary map of solids images to avoid rebuilding
  // of the solids without internal faces
  TopTools_IndexedDataMapOfShapeListOfShape aSolidsIm;
  // 1. Build solids for interfered source solids
  for (i = 0; i < aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_SOLID)
      continue;

    const TopoDS_Shape& aS = aSI.Shape();
    const TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aS));
    if (!theDraftSolids.IsBound(aS))
      continue;

    const TopoDS_Shape& aSD = theDraftSolids.Find(aS);
    const TopTools_ListOfShape* pLFIN = theInParts.Seek(aS);
    if (!pLFIN || pLFIN->IsEmpty())
    {
      aSolidsIm(aSolidsIm.Add(aS, TopTools_ListOfShape())).Append(aSD);
      continue;
    }

    aSFS.Clear();
    //
    // 1.1 Fill Shell Faces Set
    aExp.Init(aSD, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aF = aExp.Current();
      aSFS.Append(aF);
    }
    //
    // 1.2 Fill internal faces
    aIt.Initialize(*pLFIN);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape aF = aIt.Value();
      //
      aF.Orientation(TopAbs_FORWARD);
      aSFS.Append(aF);
      aF.Orientation(TopAbs_REVERSED);
      aSFS.Append(aF);
    }
    //
    // 1.3 Build new solids
    BOPAlgo_SplitSolid& aBS=aVBS.Appended();
    aBS.SetSolid(aSolid);
    aBS.SetShapes(aSFS);
    aBS.SetRunParallel(myRunParallel);
    aBS.SetProgressIndicator(myProgressIndicator);
  }//for (i=0; i<aNbS; ++i) {
  //
  Standard_Integer k, aNbBS;
  //
  aNbBS=aVBS.Length();
  //
  //===================================================
  BOPAlgo_BuilderSolidCnt::Perform(myRunParallel, aVBS);
  //===================================================
  //
  for (k = 0; k < aNbBS; ++k)
  {
    BOPAlgo_SplitSolid& aBS = aVBS(k);
    aSolidsIm.Add(aBS.Solid(), aBS.Areas());
    myReport->Merge(aBS.GetReport());
  }
  //
  // Add new solids to images map
  aNbBS = aSolidsIm.Extent();
  for (k = 1; k <= aNbBS; ++k)
  {
    const TopoDS_Shape& aS = aSolidsIm.FindKey(k);
    const TopTools_ListOfShape& aLSR = aSolidsIm(k);
    //
    if (!myImages.IsBound(aS)) {
      TopTools_ListOfShape* pLSx = myImages.Bound(aS, TopTools_ListOfShape());
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
        pLSx->Append(aSx);
        //
        TopTools_ListOfShape* pLOr = myOrigins.ChangeSeek(aSx);
        if (!pLOr) {
          pLOr = myOrigins.Bound(aSx, TopTools_ListOfShape());
        }
        pLOr->Append(aS);
        //
        if (bFlagSD) {
          myShapesSD.Bind(aSR, aSx);
        }
      }
    }
  }
}
//=======================================================================
//function :FillInternalShapes 
//purpose  : 
//=======================================================================
void BOPAlgo_Builder::FillInternalShapes()
{
  Standard_Integer i, j,  aNbS, aNbSI, aNbSx;
  TopAbs_ShapeEnum aType;
  TopAbs_State aState; 
  TopoDS_Iterator aItS;
  BRep_Builder aBB;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  //
  Handle(NCollection_BaseAllocator) aAllocator;
  //-----------------------------------------------------scope f
  aAllocator=NCollection_BaseAllocator::CommonBaseAllocator();
  //
  TopTools_IndexedDataMapOfShapeListOfShape aMSx(100, aAllocator);
  TopTools_IndexedMapOfShape aMx(100, aAllocator);
  TopTools_IndexedMapOfShape aMSI(100, aAllocator);
  TopTools_MapOfShape aMFence(100, aAllocator);
  TopTools_MapOfShape aMSOr(100, aAllocator);
  TopTools_ListOfShape aLSd(aAllocator);
  TopTools_ListOfShape aLArgs(aAllocator);
  TopTools_ListOfShape aLSC(aAllocator);
  TopTools_ListOfShape aLSI(aAllocator);
  //
  // 1. Shapes to process
  //
  // 1.1 Shapes from pure arguments aMSI 
  // 1.1.1 vertex, edge, wire
  //
  const TopTools_ListOfShape& aArguments=myDS->Arguments();
  aIt.Initialize(aArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    BOPAlgo_Tools::TreatCompound(aS, aMFence, aLSC);
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
    if (aType==TopAbs_VERTEX || 
        aType==TopAbs_EDGE ||
        aType==TopAbs_WIRE) {
      if (aMFence.Add(aS)) {
        if (myImages.IsBound(aS)) {
          const TopTools_ListOfShape &aLSp=myImages.Find(aS);
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
    UserBreak();
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
        const TopTools_ListOfShape &aLSp=myImages.Find(aSi);
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
      const TopTools_ListOfShape &aLSp=myImages.Find(aS);
      aIt.Initialize(aLSp);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aSp=aIt.Value();
        if (aMFence.Add(aSp)) { 
          TopExp::MapShapesAndAncestors(aSp, TopAbs_VERTEX, TopAbs_EDGE, aMSx);
          TopExp::MapShapesAndAncestors(aSp, TopAbs_VERTEX, TopAbs_FACE, aMSx);
          TopExp::MapShapesAndAncestors(aSp, TopAbs_EDGE  , TopAbs_FACE, aMSx);
          aLSd.Append(aSp);
        }
      }
    }
    else {
      if (aMFence.Add(aS)) {
        TopExp::MapShapesAndAncestors(aS, TopAbs_VERTEX, TopAbs_EDGE, aMSx);
        TopExp::MapShapesAndAncestors(aS, TopAbs_VERTEX, TopAbs_FACE, aMSx);
        TopExp::MapShapesAndAncestors(aS, TopAbs_EDGE  , TopAbs_FACE, aMSx);
        aLSd.Append(aS);
        aMSOr.Add(aS); 
      }
    }
  }// for (i=0; i<aNbS; ++i) {
  //
  // 3. Some shapes of aMSI can be already tied with faces of 
  //    split solids
  aNbSI = aMSI.Extent();
  for (i = 1; i <= aNbSI; ++i) {
    const TopoDS_Shape& aSI = aMSI(i);
    if (aMSx.Contains(aSI)) {
      const TopTools_ListOfShape &aLSx=aMSx.FindFromKey(aSI);
      aNbSx = aLSx.Extent();
      if (!aNbSx) {
        aLSI.Append(aSI);
      }
    }
    else {
      aLSI.Append(aSI);
    }
  }
  //
  // 4. Just check it
  aNbSI = aLSI.Extent();
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
    aIt1.Initialize(aLSI);
    for (; aIt1.More();) {
      TopoDS_Shape aSI = aIt1.Value();
      aSI.Orientation(TopAbs_INTERNAL);
      //
      aState=BOPTools_AlgoTools::ComputeStateByOnePoint
        (aSI, aSd, 1.e-11, myContext);
      //
      if (aState != TopAbs_IN) {
        aIt1.Next();
        continue;
      }
      //
      if (aMSOr.Contains(aSd)) {
        // make new solid
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
        // no need to check for images of aSd as aMSOr contains only original solids
        TopTools_ListOfShape* pLS = myImages.Bound(aSd, TopTools_ListOfShape());
        pLS->Append(aSdx);
        //
        TopTools_ListOfShape* pLOr = myOrigins.Bound(aSdx, TopTools_ListOfShape());
        pLOr->Append(aSd);
        //
        aMSOr.Remove(aSd);
        aSd=aSdx;
      }
      else {
        aBB.Add(aSd, aSI);
      }
      //
      aLSI.Remove(aIt1);
    }//for (; aIt1.More();) {
  }//for (; aIt.More(); aIt.Next()) {
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
                       TopTools_IndexedMapOfShape& theMx)
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
