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

#include <Bnd_Box.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Tools.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

//=================================================================================================

void BOPAlgo_PaveFiller::PerformVV(const Message_ProgressRange& theRange)
{
  int                                    n1, n2, iFlag, aSize;
  occ::handle<NCollection_BaseAllocator> aAllocator;
  //
  myIterator->Initialize(TopAbs_VERTEX, TopAbs_VERTEX);
  aSize = myIterator->ExpectedLength();
  Message_ProgressScope aPS(theRange, NULL, 2.);
  if (!aSize)
  {
    return;
  }
  //
  NCollection_Vector<BOPDS_InterfVV>& aVVs = myDS->InterfVV();
  aVVs.SetIncrement(aSize);
  //
  //-----------------------------------------------------scope f
  aAllocator = NCollection_BaseAllocator::CommonBaseAllocator();
  NCollection_IndexedDataMap<int, NCollection_List<int>> aMILI(100, aAllocator);
  NCollection_List<NCollection_List<int>>                aMBlocks(aAllocator);
  //
  // 1. Map V/LV
  // Split progress range on intersection stage and making blocks. Display only intersection stage.
  Message_ProgressScope aPSLoop(aPS.Next(1.), "Performing Vertex-Vertex intersection", aSize);
  for (; myIterator->More(); myIterator->Next(), aPSLoop.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    myIterator->Value(n1, n2);
    //
    if (myDS->HasInterf(n1, n2))
    {
      BOPAlgo_Tools::FillMap(n1, n2, aMILI, aAllocator);
      continue;
    }

    // Check for SD vertices
    int n1SD = n1;
    myDS->HasShapeSD(n1, n1SD);

    int n2SD = n2;
    myDS->HasShapeSD(n2, n2SD);

    const TopoDS_Vertex& aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(n1SD)));
    const TopoDS_Vertex& aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(n2SD)));

    iFlag = BOPTools_AlgoTools::ComputeVV(aV1, aV2, myFuzzyValue);
    if (!iFlag)
    {
      BOPAlgo_Tools::FillMap(n1, n2, aMILI, aAllocator);
    }
  }
  //
  // 2. Make blocks
  BOPAlgo_Tools::MakeBlocks(aMILI, aMBlocks, aAllocator);
  //
  // 3. Make vertices
  NCollection_List<NCollection_List<int>>::Iterator aItB(aMBlocks);
  for (; aItB.More(); aItB.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    const NCollection_List<int>& aLI = aItB.Value();
    MakeSDVertices(aLI);
  }
  //
  NCollection_DataMap<int, int>::Iterator aItDMII;
  //
  NCollection_DataMap<int, int>& aDMII = myDS->ShapesSD();
  aItDMII.Initialize(aDMII);
  for (; aItDMII.More(); aItDMII.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    n1 = aItDMII.Key();
    myDS->InitPaveBlocksForVertex(n1);
  }
  //
  //-----------------------------------------------------scope t
  aMBlocks.Clear();
  aMILI.Clear();
}

//=================================================================================================

int BOPAlgo_PaveFiller::MakeSDVertices(const NCollection_List<int>& theVertIndices,
                                       const bool                   theAddInterfs)
{
  TopoDS_Vertex                   aVSD, aVn;
  int                             nSD = -1;
  NCollection_List<int>::Iterator aItLI(theVertIndices);
  NCollection_List<TopoDS_Shape>  aLV;
  for (; aItLI.More(); aItLI.Next())
  {
    int nX = aItLI.Value(), nSD1;
    if (myDS->HasShapeSD(nX, nSD1))
    {
      const TopoDS_Shape& aVSD1 = myDS->Shape(nSD1);
      if (nSD == -1)
      {
        aVSD = TopoDS::Vertex(aVSD1);
        nSD  = nSD1;
      }
      else
      {
        aLV.Append(aVSD1);
      }
    }
    const TopoDS_Shape& aV = myDS->Shape(nX);
    aLV.Append(aV);
  }
  BOPTools_AlgoTools::MakeVertex(aLV, aVn);
  int nV;
  if (nSD != -1)
  {
    // update old SD vertex with new value
    BRep_TVertex* aTVertex = static_cast<BRep_TVertex*>(aVSD.TShape().get());
    aTVertex->Pnt(BRep_Tool::Pnt(aVn));
    aTVertex->Tolerance(BRep_Tool::Tolerance(aVn));
    aVn = aVSD;
    nV  = nSD;
  }
  else
  {
    // Append new vertex to the DS
    BOPDS_ShapeInfo aSIn;
    aSIn.SetShapeType(TopAbs_VERTEX);
    aSIn.SetShape(aVn);
    nV = myDS->Append(aSIn);
  }
  BOPDS_ShapeInfo& aSIDS = myDS->ChangeShapeInfo(nV);
  Bnd_Box&         aBox  = aSIDS.ChangeBox();
  aBox.Add(BRep_Tool::Pnt(aVn));
  aBox.SetGap(BRep_Tool::Tolerance(aVn) + Precision::Confusion());
  //
  // Fill ShapesSD
  NCollection_Vector<BOPDS_InterfVV>& aVVs = myDS->InterfVV();
  if (theAddInterfs)
    aVVs.SetIncrement(theVertIndices.Extent());
  //
  aItLI.Initialize(theVertIndices);
  for (; aItLI.More(); aItLI.Next())
  {
    int n1 = aItLI.Value();
    myDS->AddShapeSD(n1, nV);
    //
    int                 iR1 = myDS->Rank(n1);
    const TopoDS_Shape& aV1 = myDS->Shape(n1);
    //
    NCollection_List<int>::Iterator aItLI2 = aItLI;
    aItLI2.Next();
    for (; aItLI2.More(); aItLI2.Next())
    {
      int n2 = aItLI2.Value();
      //
      if (iR1 >= 0 && iR1 == myDS->Rank(n2))
      {
        // add warning status
        const TopoDS_Shape& aV2 = myDS->Shape(n2);
        //
        TopoDS_Compound aWC;
        BRep_Builder().MakeCompound(aWC);
        BRep_Builder().Add(aWC, aV1);
        BRep_Builder().Add(aWC, aV2);
        //
        AddWarning(new BOPAlgo_AlertSelfInterferingShape(aWC));
      }
      //
      if (theAddInterfs)
      {
        if (myDS->AddInterf(n1, n2))
        {
          BOPDS_InterfVV& aVV = aVVs.Appended();
          aVV.SetIndices(n1, n2);
          aVV.SetIndexNew(nV);
        }
      }
    }
  }
  return nV;
}
