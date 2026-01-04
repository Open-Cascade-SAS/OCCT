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

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <Standard_Handle.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <IntTools_Context.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
class BOPDS_PaveBlock;

//=================================================================================================

void BOPAlgo_Builder::FillImagesVertices(const Message_ProgressRange& theRange)
{
  Message_ProgressScope aPS(theRange, "Filling splits of vertices", myDS->ShapesSD().Size());
  NCollection_DataMap<int, int>::Iterator aIt(myDS->ShapesSD());
  for (; aIt.More(); aIt.Next(), aPS.Next())
  {
    if (UserBreak(aPS))
    {
      return;
    }
    int nV   = aIt.Key();
    int nVSD = aIt.Value();

    const TopoDS_Shape& aV   = myDS->Shape(nV);
    const TopoDS_Shape& aVSD = myDS->Shape(nVSD);
    // Add to Images map
    myImages.Bound(aV, NCollection_List<TopoDS_Shape>(myAllocator))->Append(aVSD);
    // Add to SD map
    myShapesSD.Bind(aV, aVSD);
    // Add to Origins map
    NCollection_List<TopoDS_Shape>* pLOr = myOrigins.ChangeSeek(aVSD);
    if (!pLOr)
      pLOr = myOrigins.Bound(aVSD, NCollection_List<TopoDS_Shape>());
    pLOr->Append(aV);
  }
}

//=================================================================================================

void BOPAlgo_Builder::FillImagesEdges(const Message_ProgressRange& theRange)
{
  int                   i, aNbS = myDS->NbSourceShapes();
  Message_ProgressScope aPS(theRange, "Filling splits of edges", aNbS);
  for (i = 0; i < aNbS; ++i, aPS.Next())
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }
    //
    // Check if the pave blocks for the edge have been initialized
    if (!aSI.HasReference())
    {
      continue;
    }
    //
    const TopoDS_Shape&                                   aE   = aSI.Shape();
    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = myDS->PaveBlocks(i);
    //
    // Fill the images of the edge from the list of its pave blocks.
    // The small edges, having no pave blocks, will have the empty list
    // of images and, thus, will be avoided in the result.
    NCollection_List<TopoDS_Shape>* pLS = myImages.Bound(aE, NCollection_List<TopoDS_Shape>());
    //
    NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB  = aItPB.Value();
      occ::handle<BOPDS_PaveBlock>        aPBR = myDS->RealPaveBlock(aPB);
      //
      int                 nSpR = aPBR->Edge();
      const TopoDS_Shape& aSpR = myDS->Shape(nSpR);
      pLS->Append(aSpR);
      //
      NCollection_List<TopoDS_Shape>* pLOr = myOrigins.ChangeSeek(aSpR);
      if (!pLOr)
      {
        pLOr = myOrigins.Bound(aSpR, NCollection_List<TopoDS_Shape>());
      }
      pLOr->Append(aE);
      //
      if (myDS->IsCommonBlockOnEdge(aPB))
      {
        int                 nSp = aPB->Edge();
        const TopoDS_Shape& aSp = myDS->Shape(nSp);
        myShapesSD.Bind(aSp, aSpR);
      }
    }
    if (UserBreak(aPS))
    {
      return;
    }
  }
}

//=================================================================================================

void BOPAlgo_Builder::BuildResult(const TopAbs_ShapeEnum theType)
{
  // Fence map
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFence;
  // Iterate on all arguments of given type
  // and add their images into result
  NCollection_List<TopoDS_Shape>::Iterator aItA(myArguments);
  for (; aItA.More(); aItA.Next())
  {
    const TopoDS_Shape& aS = aItA.Value();
    if (aS.ShapeType() != theType)
      continue;
    // Get images
    const NCollection_List<TopoDS_Shape>* pLSIm = myImages.Seek(aS);
    if (!pLSIm)
    {
      // No images -> add the argument shape itself into result
      if (aMFence.Add(aS))
        BRep_Builder().Add(myShape, aS);
    }
    else
    {
      // Add images of the argument shape into result
      NCollection_List<TopoDS_Shape>::Iterator aItIm(*pLSIm);
      for (; aItIm.More(); aItIm.Next())
      {
        const TopoDS_Shape& aSIm = aItIm.Value();
        if (aMFence.Add(aSIm))
          BRep_Builder().Add(myShape, aSIm);
      }
    }
  }
}

//=================================================================================================

void BOPAlgo_Builder::FillImagesContainers(const TopAbs_ShapeEnum       theType,
                                           const Message_ProgressRange& theRange)
{
  int                                                    i, aNbS;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFP(100, myAllocator);
  //
  aNbS = myDS->NbSourceShapes();
  Message_ProgressScope aPS(theRange, "Building splits of containers", 1);
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() == theType)
    {
      const TopoDS_Shape& aC = aSI.Shape();
      FillImagesContainer(aC, theType);
    }
    if (UserBreak(aPS))
    {
      return;
    }
  } // for (; aItS.More(); aItS.Next()) {
}

//=================================================================================================

void BOPAlgo_Builder::FillImagesCompounds(const Message_ProgressRange& theRange)
{
  int                                                    i, aNbS;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMFP(100, myAllocator);
  //
  aNbS = myDS->NbSourceShapes();
  Message_ProgressScope aPS(theRange, "Building splits of compounds", aNbS);
  for (i = 0; i < aNbS; ++i, aPS.Next())
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (aSI.ShapeType() == TopAbs_COMPOUND)
    {
      const TopoDS_Shape& aC = aSI.Shape();
      FillImagesCompound(aC, aMFP);
    }
    if (UserBreak(aPS))
    {
      return;
    }
  } // for (; aItS.More(); aItS.Next()) {
}

//=================================================================================================

void BOPAlgo_Builder::FillImagesContainer(const TopoDS_Shape& theS, const TopAbs_ShapeEnum theType)
{
  // Check if any of the sub-shapes of the container have been modified
  TopoDS_Iterator aIt(theS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape&                   aSS   = aIt.Value();
    const NCollection_List<TopoDS_Shape>* pLFIm = myImages.Seek(aSS);
    if (pLFIm && ((pLFIm->Extent() != 1) || !pLFIm->First().IsSame(aSS)))
      break;
  }

  if (!aIt.More())
  {
    // Non of the sub-shapes have been modified.
    // No need to create the new container.
    return;
  }

  BRep_Builder aBB;
  // Make the new container of the splits of its sub-shapes
  TopoDS_Shape aCIm;
  BOPTools_AlgoTools::MakeContainer(theType, aCIm);

  aIt.Initialize(theS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape&                   aSS    = aIt.Value();
    const NCollection_List<TopoDS_Shape>* pLSSIm = myImages.Seek(aSS);

    if (!pLSSIm)
    {
      // No splits, add the sub-shape itself
      aBB.Add(aCIm, aSS);
      continue;
    }

    // Add the splits
    NCollection_List<TopoDS_Shape>::Iterator aItIm(*pLSSIm);
    for (; aItIm.More(); aItIm.Next())
    {
      TopoDS_Shape aSSIm = aItIm.Value();
      if (!aSSIm.IsEqual(aSS)
          && BOPTools_AlgoTools::IsSplitToReverseWithWarn(aSSIm, aSS, myContext, myReport))
      {
        aSSIm.Reverse();
      }
      aBB.Add(aCIm, aSSIm);
    }
  }

  aCIm.Closed(BRep_Tool::IsClosed(aCIm));
  myImages.Bound(theS, NCollection_List<TopoDS_Shape>(myAllocator))->Append(aCIm);
}

//=================================================================================================

void BOPAlgo_Builder::FillImagesCompound(
  const TopoDS_Shape&                                     theS,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMFP)
{
  bool                                     bInterferred;
  TopAbs_Orientation                       aOrX;
  TopoDS_Iterator                          aIt;
  BRep_Builder                             aBB;
  NCollection_List<TopoDS_Shape>::Iterator aItIm;
  //
  if (!theMFP.Add(theS))
  {
    return;
  }
  //
  bInterferred = false;
  aIt.Initialize(theS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSx = aIt.Value();
    if (aSx.ShapeType() == TopAbs_COMPOUND)
    {
      FillImagesCompound(aSx, theMFP);
    }
    if (myImages.IsBound(aSx))
    {
      bInterferred = true;
    }
  }
  if (!bInterferred)
  {
    return;
  }
  //
  TopoDS_Shape aCIm;
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aCIm);
  //
  aIt.Initialize(theS);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSX = aIt.Value();
    aOrX                    = aSX.Orientation();
    if (myImages.IsBound(aSX))
    {
      const NCollection_List<TopoDS_Shape>& aLFIm = myImages.Find(aSX);
      aItIm.Initialize(aLFIm);
      for (; aItIm.More(); aItIm.Next())
      {
        TopoDS_Shape aSXIm = aItIm.Value();
        aSXIm.Orientation(aOrX);
        aBB.Add(aCIm, aSXIm);
      }
    }
    else
    {
      aBB.Add(aCIm, aSX);
    }
  }
  //
  NCollection_List<TopoDS_Shape> aLSIm(myAllocator);
  aLSIm.Append(aCIm);
  myImages.Bind(theS, aLSIm);
}
