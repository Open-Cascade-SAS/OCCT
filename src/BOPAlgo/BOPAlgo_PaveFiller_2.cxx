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


#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//class    : BOPAlgo_VertexEdge
//purpose  : 
//=======================================================================
class BOPAlgo_VertexEdge : public BOPAlgo_Algo {

 public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_VertexEdge() : 
    BOPAlgo_Algo(),
    myIV(-1), myIE(-1), myFlag(-1), myT(-1.), myTolVNew(-1.) {
  };
  //
  virtual ~BOPAlgo_VertexEdge(){
  };
  //
  void SetIndices(const Standard_Integer nV,
                  const Standard_Integer nE) {
    myIV=nV;
    myIE=nE;
  }
  //
  void Indices(Standard_Integer& nV,
               Standard_Integer& nE) const {
    nV=myIV;
    nE=myIE;
  }
  //
  void SetVertex(const TopoDS_Vertex& aV) {
    myV=aV;
  }
  //
  void SetEdge(const TopoDS_Edge& aE) {
    myE=aE;
  }
  //
  const TopoDS_Vertex& Vertex() const {
    return myV;
  }
  //
  const TopoDS_Edge& Edge() const {
    return myE;
  }
  //
  Standard_Integer Flag()const {
    return myFlag;
  }
  //
  Standard_Real Parameter()const {
    return myT;
  }
  //
  Standard_Real VertexNewTolerance()const {
    return myTolVNew;
  }
  //
  void SetContext(const Handle(IntTools_Context)& aContext) {
    myContext=aContext;
  }
  //
  const Handle(IntTools_Context)& Context()const {
    return myContext;
  }
  //
  void SetPaveBlock(const Handle(BOPDS_PaveBlock)& thePB) {
    myPB = thePB;
  }
  //
  const Handle(BOPDS_PaveBlock)& PaveBlock() const {
    return myPB;
  }
  //
  virtual void Perform() {
    BOPAlgo_Algo::UserBreak();
    try
    {
      OCC_CATCH_SIGNALS

      myFlag=myContext->ComputeVE (myV, myE, myT, myTolVNew, myFuzzyValue);
    }
    catch (Standard_Failure)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }
  };
  //
 protected:
  Standard_Integer myIV;
  Standard_Integer myIE;
  Standard_Integer myFlag;
  Standard_Real myT;
  Standard_Real myTolVNew;
  TopoDS_Vertex myV;
  TopoDS_Edge myE;
  Handle(IntTools_Context) myContext;
  Handle(BOPDS_PaveBlock) myPB;
};
//=======================================================================
typedef NCollection_Vector
  <BOPAlgo_VertexEdge> BOPAlgo_VectorOfVertexEdge; 
//
typedef BOPTools_ContextFunctor 
  <BOPAlgo_VertexEdge,
  BOPAlgo_VectorOfVertexEdge,
  Handle(IntTools_Context), 
  IntTools_Context> BOPAlgo_VertexEdgeFunctor;
//
typedef BOPTools_ContextCnt 
  <BOPAlgo_VertexEdgeFunctor,
  BOPAlgo_VectorOfVertexEdge,
  Handle(IntTools_Context)> BOPAlgo_VertexEdgeCnt;
//
//=======================================================================
// function: PerformVE
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::PerformVE()
{
  FillShrunkData(TopAbs_VERTEX, TopAbs_EDGE);
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_EDGE);
  Standard_Integer iSize = myIterator->ExpectedLength();
  if (!iSize) {
    return; 
  }
  //
  // Prepare pairs for intersection
  BOPDS_IndexedDataMapOfPaveBlockListOfInteger aMVEPairs;
  for (; myIterator->More(); myIterator->Next()) {
    Standard_Integer nV, nE;
    myIterator->Value(nV, nE);
    //
    const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
    if (aSIE.HasSubShape(nV)) {
      continue;
    }
    //
    if (aSIE.HasFlag()){
      continue;
    }
    //
    if (myDS->HasInterfShapeSubShapes(nV, nE)) {
      continue;
    }
    //
    const BOPDS_ListOfPaveBlock& aLPB = myDS->PaveBlocks(nE);
    if (aLPB.IsEmpty()) {
      continue;
    }
    //
    const Handle(BOPDS_PaveBlock)& aPB = aLPB.First();
    if (!aPB->IsSplittable()) {
      // this is a micro edge, ignore it
      continue;
    }
    //
    TColStd_ListOfInteger* pLV = aMVEPairs.ChangeSeek(aPB);
    if (!pLV)
      pLV = &aMVEPairs(aMVEPairs.Add(aPB, TColStd_ListOfInteger()));
    pLV->Append(nV);
  }
  //
  IntersectVE(aMVEPairs);
}

//=======================================================================
// function: IntersectVE
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::IntersectVE
  (const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theVEPairs,
   const Standard_Boolean theAddInterfs)
{
  Standard_Integer i, aNbVE = theVEPairs.Extent();
  if (!aNbVE) {
    return;
  }
  //
  BOPDS_VectorOfInterfVE& aVEs = myDS->InterfVE();
  if (theAddInterfs) {
    aVEs.SetIncrement(aNbVE);
  }
  //
  // Prepare for intersection.
  BOPAlgo_VectorOfVertexEdge aVVE;
  // Map to collect all SD connections to add interferences
  // for all vertices having the same SD vertex.
  // It will also be used as a Fence map to avoid repeated
  // intersection of the same SD vertex with edge
  NCollection_DataMap<BOPDS_Pair, TColStd_ListOfInteger, BOPDS_PairMapHasher> aDMVSD;
  //
  for (i = 1; i <= aNbVE; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = theVEPairs.FindKey(i);
    Standard_Integer nE = aPB->OriginalEdge();
    //
    const TColStd_ListOfInteger& aLV = theVEPairs(i);
    TColStd_ListIteratorOfListOfInteger aItLV(aLV);
    for (; aItLV.More(); aItLV.Next()) {
      Standard_Integer nV = aItLV.Value();
      //
      Standard_Integer nVSD = nV;
      myDS->HasShapeSD(nV, nVSD);
      //
      BOPDS_Pair aPair(nVSD, nE);
      TColStd_ListOfInteger* pLI = aDMVSD.ChangeSeek(aPair);
      if (pLI) {
        // Already added
        pLI->Append(nV);
        continue;
      }
      // New pair
      pLI = aDMVSD.Bound(aPair, TColStd_ListOfInteger());
      pLI->Append(nV);
      //
      const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nVSD));
      const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(nE));
      //
      BOPAlgo_VertexEdge& aVESolver = aVVE.Appended();
      aVESolver.SetIndices(nVSD, nE);
      aVESolver.SetVertex(aV);
      aVESolver.SetEdge(aE);
      aVESolver.SetPaveBlock(aPB);
      aVESolver.SetFuzzyValue(myFuzzyValue);
      aVESolver.SetProgressIndicator(myProgressIndicator);
    }
  }
  //
  // Perform intersection
  //=============================================================
  BOPAlgo_VertexEdgeCnt::Perform(myRunParallel, aVVE, myContext);
  //=============================================================
  //
  // Keep the modified edges for further update
  TColStd_MapOfInteger aMEdges;
  //
  // Analyze intersections
  aNbVE = aVVE.Length();
  for (i = 0; i < aNbVE; ++i) {
    const BOPAlgo_VertexEdge& aVESolver = aVVE(i);
    if (aVESolver.Flag() != 0) {
      if (aVESolver.HasErrors())
      {
        // Warn about failed intersection of sub-shapes
        AddIntersectionFailedWarning(aVESolver.Vertex(), aVESolver.Edge());
      }
      continue;
    }
    //
    Standard_Integer nV, nE;
    aVESolver.Indices(nV, nE);
    // Parameter of vertex on edge
    Standard_Real aT = aVESolver.Parameter();
    // 1. Update vertex V/E if necessary
    Standard_Real aTolVNew = aVESolver.VertexNewTolerance();
    Standard_Integer nVx = UpdateVertex(nV, aTolVNew);
    // 2. Create new pave and add it as extra pave to pave block
    //    for further splitting of the edge
    const Handle(BOPDS_PaveBlock)& aPB = aVESolver.PaveBlock();
    BOPDS_Pave aPave;
    aPave.SetIndex(nVx);
    aPave.SetParameter(aT);
    aPB->AppendExtPave(aPave);
    aMEdges.Add(nE);
    //
    if (theAddInterfs) {
      // Add interferences into DS
      BOPDS_Pair aPair(nV, nE);
      const TColStd_ListOfInteger& aLI = aDMVSD.Find(aPair);
      TColStd_ListIteratorOfListOfInteger aItLI(aLI);
      for (; aItLI.More(); aItLI.Next()) {
        const Standard_Integer nVOld = aItLI.Value();
        // 3. Create interference V/E
        BOPDS_InterfVE& aVE = aVEs.Appended();
        aVE.SetIndices(nVOld, nE);
        aVE.SetParameter(aT);
        // 2. Add a pair in the whole table of interferences
        myDS->AddInterf(nVOld, nE);
        // 4. Set index of new vertex in the interference
        if (myDS->IsNewShape(nVx)) {
          aVE.SetIndexNew(nVx);
        }
      }
    }
  }
  //
  // Split pave blocks of the intersected edges with the extra paves.
  // At the same time compute shrunk data for the new pave blocks
  // and in case there is no valid range for the pave block,
  // the vertices of this pave block should be unified.
  SplitPaveBlocks(aMEdges, theAddInterfs);
}

//=======================================================================
// function: MakeNewCommonBlock
// purpose: Make new Common Block from the given list of Pave Blocks
//=======================================================================
static
  void MakeNewCommonBlock(const BOPDS_ListOfPaveBlock& theLPB,
                          const TColStd_ListOfInteger& theLFaces,
                          BOPDS_PDS& theDS)
{
  // Make Common Block from the pave blocks in the list
  Handle(BOPDS_CommonBlock) aCBNew = new BOPDS_CommonBlock;
  aCBNew->SetPaveBlocks(theLPB);
  aCBNew->SetFaces(theLFaces);
  //
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB(theLPB);
  for (; aItLPB.More(); aItLPB.Next()) {
    theDS->SetCommonBlock(aItLPB.ChangeValue(), aCBNew);
  }
}

//=======================================================================
// function: SplitPaveBlocks
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::SplitPaveBlocks(const TColStd_MapOfInteger& theMEdges,
                                         const Standard_Boolean theAddInterfs)
{
  // Fence map to avoid unification of the same vertices twice
  BOPDS_MapOfPair aMPairs;
  // Map to treat the Common Blocks
  NCollection_IndexedDataMap<Handle(BOPDS_CommonBlock),
                             BOPDS_ListOfPaveBlock,
                             TColStd_MapTransientHasher> aMCBNewPB;
  //
  TColStd_MapIteratorOfMapOfInteger aItM(theMEdges);
  for (; aItM.More(); aItM.Next()) {
    Standard_Integer nE = aItM.Value();
    BOPDS_ListOfPaveBlock& aLPB = myDS->ChangePaveBlocks(nE);
    //
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPB);
    for (; aItLPB.More();) {
      Handle(BOPDS_PaveBlock)& aPB = aItLPB.ChangeValue();
      //
      if (!aPB->IsToUpdate()) {
        aItLPB.Next();
        continue;
      }
      //
      const Handle(BOPDS_CommonBlock)& aCB = myDS->CommonBlock(aPB);
      //
      // Compute new pave blocks
      BOPDS_ListOfPaveBlock aLPBN;
      aPB->Update(aLPBN);
      //
      // Make sure that each new pave block has a valid range,
      // otherwise unify the vertices of the pave block
      BOPDS_ListIteratorOfListOfPaveBlock aItLPBN(aLPBN);
      for (; aItLPBN.More(); aItLPBN.Next()) {
        Handle(BOPDS_PaveBlock)& aPBN = aItLPBN.ChangeValue();
        myDS->UpdatePaveBlockWithSDVertices(aPBN);
        FillShrunkData(aPBN);
        //
        Standard_Boolean bHasValidRange = aPBN->HasShrunkData();
        // Take into account that the edge could have really small valid range,
        // so that the Pave Block cannot be further split. In this case, check if
        // the vertices of the Pave Block do not interfere. And if they are, unify them.
        Standard_Boolean bCheckDist = (bHasValidRange && !aPBN->IsSplittable());
        if (!bHasValidRange || bCheckDist)
        {
          Standard_Integer nV1, nV2;
          aPBN->Indices(nV1, nV2);
          if (nV1 == nV2)
            // Same vertices -> no valid range, no need to unify vertices
            continue;

          // Decide whether to unify vertices or not
          if (bCheckDist)
          {
            const TopoDS_Vertex& aV1 = TopoDS::Vertex(myDS->Shape(nV1));
            const TopoDS_Vertex& aV2 = TopoDS::Vertex(myDS->Shape(nV2));
            if (BOPTools_AlgoTools::ComputeVV(aV1, aV2, myFuzzyValue) == 0)
              // vertices are interfering -> no valid range, unify vertices
              bHasValidRange = Standard_False;
          }

          if (!bHasValidRange)
          {
            BOPDS_Pair aPair;
            aPair.SetIndices(nV1, nV2);
            if (aMPairs.Add(aPair))
            {
              TColStd_ListOfInteger aLV;
              aLV.Append(nV1);
              aLV.Append(nV2);
              MakeSDVertices(aLV, theAddInterfs);
            }
            continue;
          }
        }
        //
        // Update the list with new pave block
        aLPB.Append(aPBN);
        // Treat the common block
        if (!aCB.IsNull()) {
          // Store the new pave block to make new common block
          BOPDS_ListOfPaveBlock* pLPBCB = aMCBNewPB.ChangeSeek(aCB);
          if (!pLPBCB) {
            pLPBCB = &aMCBNewPB(aMCBNewPB.Add(aCB, BOPDS_ListOfPaveBlock()));
          }
          pLPBCB->Append(aPBN);
        }
      }
      // Remove old pave block
      aLPB.Remove(aItLPB);
    }
  }
  //
  // Make Common Blocks
  Standard_Integer i, aNbCB = aMCBNewPB.Extent();
  for (i = 1; i <= aNbCB; ++i) {
    const Handle(BOPDS_CommonBlock)& aCB = aMCBNewPB.FindKey(i);
    const BOPDS_ListOfPaveBlock& aLPBN = aMCBNewPB(i);
    //
    // For each group of pave blocks with the same vertices make new common block
    NCollection_IndexedDataMap<BOPDS_Pair, BOPDS_ListOfPaveBlock, BOPDS_PairMapHasher> aMInds;
    BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPBN);
    for (; aItLPB.More(); aItLPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
      //
      BOPDS_Pair aPair;
      aPair.SetIndices(aPB->Pave1().Index(), aPB->Pave2().Index());
      //
      BOPDS_ListOfPaveBlock* pLPBx = aMInds.ChangeSeek(aPair);
      if (!pLPBx) {
        pLPBx = &aMInds(aMInds.Add(aPair, BOPDS_ListOfPaveBlock()));
      }
      pLPBx->Append(aPB);
    }
    //
    Standard_Integer nV1, nV2;
    aCB->PaveBlock1()->Indices(nV1, nV2);
    Standard_Boolean bIsClosed = (nV1 == nV2);
    //
    Standard_Integer j, aNbPairs = aMInds.Extent();
    for (j = 1; j <= aNbPairs; ++j) {
      BOPDS_ListOfPaveBlock& aLPB = aMInds(j);
      //
      if (!bIsClosed) {
        // Make Common Block from the pave blocks in the list
        MakeNewCommonBlock(aLPB, aCB->Faces(), myDS);
        continue;
      }
      //
      // Find coinciding pave blocks
      while (aLPB.Extent()) {
        // Pave blocks forming the common block
        BOPDS_ListOfPaveBlock aLPBCB;
        // Point in the middle of the first pave block in the common block
        gp_Pnt aPMFirst(0., 0., 0.);
        // Tolerance of the first edge in the common block
        Standard_Real aTolEFirst = 0.;
        //
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More();) {
          const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
          if (aLPBCB.IsEmpty()) {
            aLPBCB.Append(aPB);
            const TopoDS_Edge& aEFirst = TopoDS::Edge(myDS->Shape(aPB->OriginalEdge()));
            aTolEFirst = BRep_Tool::MaxTolerance(aEFirst, TopAbs_VERTEX);
            //
            Standard_Real aTmFirst = (aPB->Pave1().Parameter() + aPB->Pave2().Parameter()) / 2.;
            BOPTools_AlgoTools::PointOnEdge(aEFirst, aTmFirst, aPMFirst);
            //
            aLPB.Remove(aItLPB);
            continue;
          }
          //
          // Check pave blocks for coincidence
          const TopoDS_Edge& aE = TopoDS::Edge(myDS->Shape(aPB->OriginalEdge()));
          Standard_Real aTolE = BRep_Tool::MaxTolerance(aE, TopAbs_VERTEX);
          //
          Standard_Real aTOut, aDist;
          Standard_Integer iErr =
            myContext->ComputePE(aPMFirst, aTolEFirst + aTolE + myFuzzyValue, aE, aTOut, aDist);
          if (!iErr && ((aTOut > aPB->Pave1().Parameter()) && (aTOut < aPB->Pave2().Parameter()))) {
            aLPBCB.Append(aPB);
            aLPB.Remove(aItLPB);
            continue;
          }
          aItLPB.Next();
        }
        //
        // Make Common Block from the pave blocks in the list
        MakeNewCommonBlock(aLPBCB, aCB->Faces(), myDS);
      }
    }
  }
}

//=======================================================================
// function: AddIntersectionFailedWarning
// purpose: 
//=======================================================================
void BOPAlgo_PaveFiller::AddIntersectionFailedWarning(const TopoDS_Shape& theS1,
                                                      const TopoDS_Shape& theS2)
{
  // Create the warn shape
  TopoDS_Compound aWC;
  BRep_Builder().MakeCompound(aWC);
  BRep_Builder().Add(aWC, theS1);
  BRep_Builder().Add(aWC, theS2);
  // Add the warning
  AddWarning(new BOPAlgo_AlertIntersectionOfPairOfShapesFailed(aWC));
}