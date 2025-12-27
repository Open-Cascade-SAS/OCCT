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
#include <NCollection_Vector.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_Parallel.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

class BOPAlgo_VertexEdge : public BOPAlgo_ParallelAlgo
{

public:
  DEFINE_STANDARD_ALLOC

  BOPAlgo_VertexEdge()
      : BOPAlgo_ParallelAlgo(),
        myIV(-1),
        myIE(-1),
        myFlag(-1),
        myT(-1.),
        myTolVNew(-1.) {};
  //
  virtual ~BOPAlgo_VertexEdge() {};

  //
  void SetIndices(const int nV, const int nE)
  {
    myIV = nV;
    myIE = nE;
  }

  //
  void Indices(int& nV, int& nE) const
  {
    nV = myIV;
    nE = myIE;
  }

  //
  void SetVertex(const TopoDS_Vertex& aV) { myV = aV; }

  //
  void SetEdge(const TopoDS_Edge& aE) { myE = aE; }

  //
  const TopoDS_Vertex& Vertex() const { return myV; }

  //
  const TopoDS_Edge& Edge() const { return myE; }

  //
  int Flag() const { return myFlag; }

  //
  double Parameter() const { return myT; }

  //
  double VertexNewTolerance() const { return myTolVNew; }

  //
  void SetContext(const occ::handle<IntTools_Context>& aContext) { myContext = aContext; }

  //
  const occ::handle<IntTools_Context>& Context() const { return myContext; }

  //
  void SetPaveBlock(const occ::handle<BOPDS_PaveBlock>& thePB) { myPB = thePB; }

  //
  const occ::handle<BOPDS_PaveBlock>& PaveBlock() const { return myPB; }

  //
  virtual void Perform()
  {
    Message_ProgressScope aPS(myProgressRange, NULL, 1);
    if (UserBreak(aPS))
    {
      return;
    }
    try
    {
      OCC_CATCH_SIGNALS

      myFlag = myContext->ComputeVE(myV, myE, myT, myTolVNew, myFuzzyValue);
    }
    catch (Standard_Failure const&)
    {
      AddError(new BOPAlgo_AlertIntersectionFailed);
    }
  };

  //
protected:
  int         myIV;
  int         myIE;
  int         myFlag;
  double            myT;
  double            myTolVNew;
  TopoDS_Vertex            myV;
  TopoDS_Edge              myE;
  occ::handle<IntTools_Context> myContext;
  occ::handle<BOPDS_PaveBlock>  myPB;
};

//=======================================================================
typedef NCollection_Vector<BOPAlgo_VertexEdge> BOPAlgo_VectorOfVertexEdge;

//=================================================================================================

void BOPAlgo_PaveFiller::PerformVE(const Message_ProgressRange& theRange)
{
  FillShrunkData(TopAbs_VERTEX, TopAbs_EDGE);
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_EDGE);
  Message_ProgressScope aPS(theRange, NULL, 1);

  int iSize = myIterator->ExpectedLength();
  if (!iSize)
  {
    return;
  }
  //
  // Prepare pairs for intersection
  NCollection_IndexedDataMap<occ::handle<BOPDS_PaveBlock>, NCollection_List<int>> aMVEPairs;
  for (; myIterator->More(); myIterator->Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    int nV, nE;
    myIterator->Value(nV, nE);
    //
    const BOPDS_ShapeInfo& aSIE = myDS->ShapeInfo(nE);
    if (aSIE.HasSubShape(nV))
    {
      continue;
    }
    //
    if (aSIE.HasFlag())
    {
      continue;
    }
    //
    if (myDS->HasInterf(nV, nE))
    {
      continue;
    }
    //
    if (myDS->HasInterfShapeSubShapes(nV, nE))
    {
      continue;
    }
    //
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = myDS->PaveBlocks(nE);
    if (aLPB.IsEmpty())
    {
      continue;
    }
    //
    const occ::handle<BOPDS_PaveBlock>& aPB = aLPB.First();
    if (!aPB->IsSplittable())
    {
      // this is a micro edge, ignore it
      continue;
    }
    //
    NCollection_List<int>* pLV = aMVEPairs.ChangeSeek(aPB);
    if (!pLV)
      pLV = &aMVEPairs(aMVEPairs.Add(aPB, NCollection_List<int>()));
    pLV->Append(nV);
  }
  //
  IntersectVE(aMVEPairs, aPS.Next());
}

//=================================================================================================

void BOPAlgo_PaveFiller::IntersectVE(const NCollection_IndexedDataMap<occ::handle<BOPDS_PaveBlock>, NCollection_List<int>>& theVEPairs,
                                     const Message_ProgressRange&                        theRange,
                                     const bool theAddInterfs)
{
  int i, aNbVE = theVEPairs.Extent();
  if (!aNbVE)
  {
    return;
  }
  //
  NCollection_Vector<BOPDS_InterfVE>& aVEs = myDS->InterfVE();
  if (theAddInterfs)
  {
    aVEs.SetIncrement(aNbVE);
  }
  //
  // Prepare for intersection.
  BOPAlgo_VectorOfVertexEdge aVVE;
  // Map to collect all SD connections to add interferences
  // for all vertices having the same SD vertex.
  // It will also be used as a Fence map to avoid repeated
  // intersection of the same SD vertex with edge
  NCollection_DataMap<BOPDS_Pair, NCollection_List<int>> aDMVSD;
  //
  Message_ProgressScope aPSOuter(theRange, NULL, 10);
  for (i = 1; i <= aNbVE; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const occ::handle<BOPDS_PaveBlock>& aPB = theVEPairs.FindKey(i);
    int               nE  = aPB->OriginalEdge();
    //
    NCollection_Map<int>         aMVPB;
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = myDS->PaveBlocks(nE);
    for (NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator itPB(aLPB); itPB.More(); itPB.Next())
    {
      aMVPB.Add(itPB.Value()->Pave1().Index());
      aMVPB.Add(itPB.Value()->Pave2().Index());
    }

    const NCollection_List<int>&        aLV = theVEPairs(i);
    NCollection_List<int>::Iterator aItLV(aLV);
    for (; aItLV.More(); aItLV.Next())
    {
      int nV = aItLV.Value();
      //
      int nVSD = nV;
      myDS->HasShapeSD(nV, nVSD);
      //
      if (aMVPB.Contains(nVSD))
        continue;

      BOPDS_Pair             aPair(nVSD, nE);
      NCollection_List<int>* pLI = aDMVSD.ChangeSeek(aPair);
      if (pLI)
      {
        // Already added
        pLI->Append(nV);
        continue;
      }
      // New pair
      pLI = aDMVSD.Bound(aPair, NCollection_List<int>());
      pLI->Append(nV);
      //
      const TopoDS_Vertex& aV = TopoDS::Vertex(myDS->Shape(nVSD));
      const TopoDS_Edge&   aE = TopoDS::Edge(myDS->Shape(nE));
      //
      BOPAlgo_VertexEdge& aVESolver = aVVE.Appended();
      aVESolver.SetIndices(nVSD, nE);
      aVESolver.SetVertex(aV);
      aVESolver.SetEdge(aE);
      aVESolver.SetPaveBlock(aPB);
      aVESolver.SetFuzzyValue(myFuzzyValue);
    }
  }
  //
  aNbVE = aVVE.Length();

  Message_ProgressScope aPS(aPSOuter.Next(9), "Performing Vertex-Edge intersection", aNbVE);
  for (i = 0; i < aNbVE; i++)
  {
    BOPAlgo_VertexEdge& aVESolver = aVVE.ChangeValue(i);
    aVESolver.SetProgressRange(aPS.Next());
  }
  // Perform intersection
  //=============================================================
  BOPTools_Parallel::Perform(myRunParallel, aVVE, myContext);
  //=============================================================
  if (UserBreak(aPSOuter))
  {
    return;
  }
  //
  // Keep the modified edges for further update
  NCollection_Map<int> aMEdges;
  //
  // Analyze intersections
  for (i = 0; i < aNbVE; ++i)
  {
    if (UserBreak(aPSOuter))
    {
      return;
    }
    const BOPAlgo_VertexEdge& aVESolver = aVVE(i);
    if (aVESolver.Flag() != 0)
    {
      if (aVESolver.HasErrors())
      {
        // Warn about failed intersection of sub-shapes
        AddIntersectionFailedWarning(aVESolver.Vertex(), aVESolver.Edge());
      }
      continue;
    }
    //
    int nV, nE;
    aVESolver.Indices(nV, nE);
    // Parameter of vertex on edge
    double aT = aVESolver.Parameter();
    // 1. Update vertex V/E if necessary
    double    aTolVNew = aVESolver.VertexNewTolerance();
    int nVx      = UpdateVertex(nV, aTolVNew);
    // 2. Create new pave and add it as extra pave to pave block
    //    for further splitting of the edge
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = myDS->PaveBlocks(nE);
    // Find the appropriate one
    occ::handle<BOPDS_PaveBlock>         aPB;
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator itPB(aLPB);
    for (; itPB.More(); itPB.Next())
    {
      aPB = itPB.Value();
      double aT1, aT2;
      aPB->Range(aT1, aT2);
      if (aT > aT1 && aT < aT2)
        break;
    }
    if (!itPB.More())
      continue;

    BOPDS_Pave aPave;
    aPave.SetIndex(nVx);
    aPave.SetParameter(aT);
    aPB->AppendExtPave(aPave);
    aMEdges.Add(nE);
    //
    if (theAddInterfs)
    {
      // Add interferences into DS
      BOPDS_Pair                          aPair(nV, nE);
      const NCollection_List<int>&        aLI = aDMVSD.Find(aPair);
      NCollection_List<int>::Iterator aItLI(aLI);
      for (; aItLI.More(); aItLI.Next())
      {
        const int nVOld = aItLI.Value();
        // 3. Create interference V/E
        BOPDS_InterfVE& aVE = aVEs.Appended();
        aVE.SetIndices(nVOld, nE);
        aVE.SetParameter(aT);
        // 2. Add a pair in the whole table of interferences
        myDS->AddInterf(nVOld, nE);
        // 4. Set index of new vertex in the interference
        if (myDS->IsNewShape(nVx))
        {
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
static void MakeNewCommonBlock(const NCollection_List<occ::handle<BOPDS_PaveBlock>>& theLPB,
                               const NCollection_List<int>& theLFaces,
                               BOPDS_PDS&                   theDS)
{
  // Make Common Block from the pave blocks in the list
  occ::handle<BOPDS_CommonBlock> aCBNew = new BOPDS_CommonBlock;
  aCBNew->SetPaveBlocks(theLPB);
  aCBNew->SetFaces(theLFaces);
  //
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB(theLPB);
  for (; aItLPB.More(); aItLPB.Next())
  {
    theDS->SetCommonBlock(aItLPB.ChangeValue(), aCBNew);
  }
}

//=================================================================================================

void BOPAlgo_PaveFiller::SplitPaveBlocks(const NCollection_Map<int>& theMEdges,
                                         const bool      theAddInterfs)
{
  // Fence map to avoid unification of the same vertices twice
  NCollection_Map<BOPDS_Pair> aMPairs;
  // Map to treat the Common Blocks
  NCollection_IndexedDataMap<occ::handle<BOPDS_CommonBlock>, NCollection_List<occ::handle<BOPDS_PaveBlock>>> aMCBNewPB;
  //
  // Map of vertices to init the pave blocks for them
  NCollection_Map<int> aMVerticesToInitPB;

  NCollection_Map<int>::Iterator aItM(theMEdges);
  for (; aItM.More(); aItM.Next())
  {
    int       nE   = aItM.Value();
    NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = myDS->ChangePaveBlocks(nE);
    //
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB(aLPB);
    for (; aItLPB.More();)
    {
      occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.ChangeValue();
      //
      if (!aPB->IsToUpdate())
      {
        aItLPB.Next();
        continue;
      }
      //
      const occ::handle<BOPDS_CommonBlock>& aCB = myDS->CommonBlock(aPB);
      //
      // Compute new pave blocks
      NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBN;
      aPB->Update(aLPBN);
      //
      // Make sure that each new pave block has a valid range,
      // otherwise unify the vertices of the pave block
      NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPBN(aLPBN);
      for (; aItLPBN.More(); aItLPBN.Next())
      {
        occ::handle<BOPDS_PaveBlock>& aPBN = aItLPBN.ChangeValue();
        myDS->UpdatePaveBlockWithSDVertices(aPBN);
        FillShrunkData(aPBN);
        //
        bool bHasValidRange = aPBN->HasShrunkData();
        // Take into account that the edge could have really small valid range,
        // so that the Pave Block cannot be further split. In this case, check if
        // the vertices of the Pave Block do not interfere. And if they are, unify them.
        bool bCheckDist = (bHasValidRange && !aPBN->IsSplittable());
        if (!bHasValidRange || bCheckDist)
        {
          int nV1, nV2;
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
              bHasValidRange = false;
          }

          if (!bHasValidRange)
          {
            BOPDS_Pair aPair;
            aPair.SetIndices(nV1, nV2);
            if (aMPairs.Add(aPair))
            {
              NCollection_List<int> aLV;
              aLV.Append(nV1);
              aLV.Append(nV2);
              MakeSDVertices(aLV, theAddInterfs);

              // Save vertices to init pave blocks
              aMVerticesToInitPB.Add(nV1);
              aMVerticesToInitPB.Add(nV2);
            }
            continue;
          }
        }
        //
        // Update the list with new pave block
        aLPB.Append(aPBN);
        // Treat the common block
        if (!aCB.IsNull())
        {
          // Store the new pave block to make new common block
          NCollection_List<occ::handle<BOPDS_PaveBlock>>* pLPBCB = aMCBNewPB.ChangeSeek(aCB);
          if (!pLPBCB)
          {
            pLPBCB = &aMCBNewPB(aMCBNewPB.Add(aCB, NCollection_List<occ::handle<BOPDS_PaveBlock>>()));
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
  int i, aNbCB = aMCBNewPB.Extent();
  for (i = 1; i <= aNbCB; ++i)
  {
    const occ::handle<BOPDS_CommonBlock>& aCB   = aMCBNewPB.FindKey(i);
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>&     aLPBN = aMCBNewPB(i);
    //
    // For each group of pave blocks with the same vertices make new common block
    NCollection_IndexedDataMap<BOPDS_Pair, NCollection_List<occ::handle<BOPDS_PaveBlock>>> aMInds;
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator                           aItLPB(aLPBN);
    for (; aItLPB.More(); aItLPB.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.Value();
      //
      BOPDS_Pair aPair;
      aPair.SetIndices(aPB->Pave1().Index(), aPB->Pave2().Index());
      //
      NCollection_List<occ::handle<BOPDS_PaveBlock>>* pLPBx = aMInds.ChangeSeek(aPair);
      if (!pLPBx)
      {
        pLPBx = &aMInds(aMInds.Add(aPair, NCollection_List<occ::handle<BOPDS_PaveBlock>>()));
      }
      pLPBx->Append(aPB);
    }
    //
    int nV1, nV2;
    aCB->PaveBlock1()->Indices(nV1, nV2);
    bool bIsClosed = (nV1 == nV2);
    //
    int j, aNbPairs = aMInds.Extent();
    for (j = 1; j <= aNbPairs; ++j)
    {
      NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aMInds(j);
      //
      if (!bIsClosed)
      {
        // Make Common Block from the pave blocks in the list
        MakeNewCommonBlock(aLPB, aCB->Faces(), myDS);
        continue;
      }
      //
      // Find coinciding pave blocks
      while (aLPB.Extent())
      {
        // Pave blocks forming the common block
        NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBCB;
        // Point in the middle of the first pave block in the common block
        gp_Pnt aPMFirst(0., 0., 0.);
        // Tolerance of the first edge in the common block
        double aTolEFirst = 0.;
        //
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More();)
        {
          const occ::handle<BOPDS_PaveBlock>& aPB = aItLPB.Value();
          if (aLPBCB.IsEmpty())
          {
            aLPBCB.Append(aPB);
            const TopoDS_Edge& aEFirst = TopoDS::Edge(myDS->Shape(aPB->OriginalEdge()));
            aTolEFirst                 = BRep_Tool::MaxTolerance(aEFirst, TopAbs_VERTEX);
            //
            double aTmFirst = (aPB->Pave1().Parameter() + aPB->Pave2().Parameter()) / 2.;
            BOPTools_AlgoTools::PointOnEdge(aEFirst, aTmFirst, aPMFirst);
            //
            aLPB.Remove(aItLPB);
            continue;
          }
          //
          // Check pave blocks for coincidence
          const TopoDS_Edge& aE    = TopoDS::Edge(myDS->Shape(aPB->OriginalEdge()));
          double      aTolE = BRep_Tool::MaxTolerance(aE, TopAbs_VERTEX);
          //
          double    aTOut, aDist;
          int iErr =
            myContext->ComputePE(aPMFirst, aTolEFirst + aTolE + myFuzzyValue, aE, aTOut, aDist);
          if (!iErr && ((aTOut > aPB->Pave1().Parameter()) && (aTOut < aPB->Pave2().Parameter())))
          {
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

  // Init pave blocks for vertices which have acquired SD vertex
  aItM.Initialize(aMVerticesToInitPB);
  for (; aItM.More(); aItM.Next())
    myDS->InitPaveBlocksForVertex(aItM.Value());
}

//=================================================================================================

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
