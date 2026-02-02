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

#include <MathUtils_Core.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_IndexRange.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <NCollection_Array1.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <IntTools_Tools.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Assert.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <algorithm>
#include <numeric>
#include <set>

namespace
{

//! Initial increment for myLines container.
//! This is an arbitrary large value to avoid multiple allocations during Init().
//! Previous implementation calculated it as the count of unique shapes (including sub-shapes)
//! multiplied by 2, but counting unique shapes causes significant overhead.
constexpr int THE_INITIAL_LINES_INCREMENT = 500;

//=================================================================================================

// Computes the parameter of @p theVertex on @p theEdge.
// If theVertex is not coincident with either end of theEdge, returns 0.0.
// Unlike BRep_Tool::Parameter, this does not require the vertex to be topologically
// connected to the edge.
// Note: this is generally unsafe function that is left to preserve previous behavior.
double ComputeParameter(const TopoDS_Vertex& theVertex, const TopoDS_Edge& theEdge)
{
  const double anEdgeToleranceSqr = MathUtils::Sqr(BRep_Tool::Tolerance(theEdge));
  const gp_Pnt aVertexPoint       = BRep_Tool::Pnt(theVertex);

  TopoDS_Edge aForwardEdge = theEdge;
  aForwardEdge.Orientation(TopAbs_FORWARD);
  double                        aCurveStart, aCurveEnd;
  const occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(aForwardEdge, aCurveStart, aCurveEnd);

  auto isMatch = [&aVertexPoint, &aCurve, anEdgeToleranceSqr](const double theParameter) -> bool {
    gp_Pnt aPointOnCurve;
    aCurve->D0(theParameter, aPointOnCurve);
    return (aPointOnCurve.SquareDistance(aVertexPoint) < anEdgeToleranceSqr);
  };

  // Note: only start and end parameters are checked.
  // It is unsafe to return 0. when point is not matching either end
  // but this is left to preserve previous behavior.
  return isMatch(aCurveStart) ? aCurveStart : isMatch(aCurveEnd) ? aCurveEnd : 0.;
}
} // namespace

//=================================================================================================

BOPDS_DS::BOPDS_DS()
    : BOPDS_DS(NCollection_BaseAllocator::CommonBaseAllocator())
{
}

//=================================================================================================

BOPDS_DS::BOPDS_DS(const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : myAllocator(theAllocator),
      myArguments(myAllocator),
      myRanges(0, myAllocator),
      myLines(0, myAllocator),
      myMapShapeIndex(100, myAllocator),
      myPaveBlocksPool(0, myAllocator),
      myMapPBCB(100, myAllocator),
      myFaceInfoPool(0, myAllocator),
      myShapesSD(100, myAllocator),
      myMapVE(100, myAllocator),
      myInterfTB(100, myAllocator),
      myInterfVV(0, myAllocator),
      myInterfVE(0, myAllocator),
      myInterfVF(0, myAllocator),
      myInterfEE(0, myAllocator),
      myInterfEF(0, myAllocator),
      myInterfFF(0, myAllocator),
      myInterfVZ(0, myAllocator),
      myInterfEZ(0, myAllocator),
      myInterfFZ(0, myAllocator),
      myInterfZZ(0, myAllocator),
      myInterfered(100, myAllocator)
{
  myNbShapes       = 0;
  myNbSourceShapes = 0;
}

//=================================================================================================

BOPDS_DS::~BOPDS_DS()
{
  Clear();
}

//=================================================================================================

void BOPDS_DS::Clear()
{
  myNbShapes       = 0;
  myNbSourceShapes = 0;
  //
  myArguments.Clear();
  myRanges.Clear();
  myLines.Clear();
  myMapShapeIndex.Clear();
  myPaveBlocksPool.Clear();
  myFaceInfoPool.Clear();
  myShapesSD.Clear();
  myMapVE.Clear();
  myMapPBCB.Clear();
  myInterfTB.Clear();
  myInterfVV.Clear();
  myInterfVE.Clear();
  myInterfVF.Clear();
  myInterfEE.Clear();
  myInterfEF.Clear();
  myInterfFF.Clear();
  myInterfVZ.Clear();
  myInterfEZ.Clear();
  myInterfFZ.Clear();
  myInterfZZ.Clear();
  myInterfered.Clear();
}

//=================================================================================================

void BOPDS_DS::SetArguments(const NCollection_List<TopoDS_Shape>& theArguments)
{
  myArguments = theArguments;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BOPDS_DS::Arguments() const
{
  return myArguments;
}

//=================================================================================================

const occ::handle<NCollection_BaseAllocator>& BOPDS_DS::Allocator() const
{
  return myAllocator;
}

//=================================================================================================

int BOPDS_DS::NbShapes() const
{
  return myLines.Size();
}

//=================================================================================================

int BOPDS_DS::NbSourceShapes() const
{
  return myNbSourceShapes;
}

//=================================================================================================

int BOPDS_DS::NbRanges() const
{
  return myRanges.Size();
}

//=================================================================================================

const BOPDS_IndexRange& BOPDS_DS::Range(const int theIndex) const
{
  return myRanges(theIndex);
}

//=================================================================================================

int BOPDS_DS::Rank(const int theIndex) const
{
  for (int aRangeIndex = 0; aRangeIndex < NbRanges(); ++aRangeIndex)
  {
    if (Range(aRangeIndex).Contains(theIndex))
    {
      return aRangeIndex;
    }
  }
  return -1;
}

//=================================================================================================

bool BOPDS_DS::IsNewShape(const int theIndex) const
{
  return theIndex >= NbSourceShapes();
}

//=================================================================================================

int BOPDS_DS::Append(const BOPDS_ShapeInfo& theShapeInfo)
{
  myLines.Appended()    = theShapeInfo;
  const int aShapeIndex = myLines.Length() - 1;
  myMapShapeIndex.Bind(theShapeInfo.Shape(), aShapeIndex);
  return aShapeIndex;
}

//=================================================================================================

int BOPDS_DS::Append(const TopoDS_Shape& theShape)
{
  myLines.Appended().SetShape(theShape);
  const int aShapeIndex = myLines.Length() - 1;
  myMapShapeIndex.Bind(theShape, aShapeIndex);
  return aShapeIndex;
}

//=================================================================================================

const BOPDS_ShapeInfo& BOPDS_DS::ShapeInfo(const int theIndex) const
{
  return myLines(theIndex);
}

//=================================================================================================

BOPDS_ShapeInfo& BOPDS_DS::ChangeShapeInfo(const int theIndex)
{
  return myLines(theIndex);
}

//=================================================================================================

const TopoDS_Shape& BOPDS_DS::Shape(const int theIndex) const
{
  return ShapeInfo(theIndex).Shape();
}

//=================================================================================================

int BOPDS_DS::Index(const TopoDS_Shape& theShape) const
{
  int anIndex = -1;
  myMapShapeIndex.Find(theShape, anIndex);
  return anIndex;
}

//=================================================================================================

void BOPDS_DS::Init(const double theFuzz)
{
  // 1. Append Source Shapes
  if (myArguments.IsEmpty())
  {
    return;
  }
  myRanges.SetIncrement(myArguments.Size());
  myLines.SetIncrement(THE_INITIAL_LINES_INCREMENT);

  int i1 = 0;
  for (const TopoDS_Shape& aShape : myArguments)
  {
    if (myMapShapeIndex.IsBound(aShape))
    {
      continue;
    }

    InitShape(Append(aShape), aShape);
    const int i2 = NbShapes() - 1;
    myRanges.Append(BOPDS_IndexRange(i1, i2));
    i1 = i2 + 1;
  }

  myNbSourceShapes = NbShapes();

  // 2. Prepare data for shapes. Includes updating bound boxes, updating sub-shapes.
  const double anAdditionalTolerance = std::max(theFuzz, Precision::Confusion()) * 0.5;
  prepareVertices(anAdditionalTolerance);                      // Vertex.
  const int anEdgeCount = prepareEdges(anAdditionalTolerance); // Edge.
  const int aFaceCount  = prepareFaces(anAdditionalTolerance); // Face.
  prepareSolids();                                             // Solid.

  // 3. Prepare Vertex-Edge connection map.
  buildVertexEdgeMap();

  // 4. Prepare pools.
  myPaveBlocksPool.SetIncrement(anEdgeCount);
  myFaceInfoPool.SetIncrement(aFaceCount);
}

//=================================================================================================

void BOPDS_DS::InitShape(const int theShapeInfoIndex, const TopoDS_Shape& theShape)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theShapeInfoIndex);
  aShapeInfo.SetShapeType(theShape.ShapeType());
  NCollection_List<int>& aSubShapeIndices = aShapeInfo.ChangeSubShapes();

  NCollection_Map<int> anExistingSubShapeIndices;
  for (const auto& aSubShapeIndex : aSubShapeIndices)
  {
    anExistingSubShapeIndices.Add(aSubShapeIndex);
  }

  for (TopoDS_Iterator aShapeIter(theShape); aShapeIter.More(); aShapeIter.Next())
  {
    const TopoDS_Shape& aSubShape       = aShapeIter.Value();
    const int*          anExistingIndex = myMapShapeIndex.Seek(aSubShape);
    const int           aSubShapeIndex  = (anExistingIndex ? *anExistingIndex : Append(aSubShape));

    InitShape(aSubShapeIndex, aSubShape);
    if (anExistingSubShapeIndices.Add(aSubShapeIndex))
    {
      aSubShapeIndices.Append(aSubShapeIndex);
    }
  }
}

//=================================================================================================

bool BOPDS_DS::HasInterfShapeSubShapes(const int  theIndex1,
                                       const int  theIndex2,
                                       const bool theAnyInterference) const
{
  const NCollection_List<int>& aSubShapes = ShapeInfo(theIndex2).SubShapes();
  if (theAnyInterference)
  {
    return std::any_of(
      aSubShapes.begin(),
      aSubShapes.end(),
      [this, theIndex1](int theSubShapeIndex) { return HasInterf(theIndex1, theSubShapeIndex); });
  }
  else
  {
    return std::all_of(
      aSubShapes.begin(),
      aSubShapes.end(),
      [this, theIndex1](int theSubShapeIndex) { return HasInterf(theIndex1, theSubShapeIndex); });
  }
}

//=================================================================================================

bool BOPDS_DS::HasInterfSubShapes(const int theIndex1, const int theIndex2) const
{
  const NCollection_List<int>& aSubShapes = ShapeInfo(theIndex1).SubShapes();
  return std::any_of(aSubShapes.begin(), aSubShapes.end(), [this, theIndex2](int theSubShapeIndex) {
    return HasInterfShapeSubShapes(theSubShapeIndex, theIndex2);
  });
}

//=================================================================================================

const NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>& BOPDS_DS::PaveBlocksPool()
  const
{
  return myPaveBlocksPool;
}

//=================================================================================================

NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>& BOPDS_DS::ChangePaveBlocksPool()
{
  return myPaveBlocksPool;
}

//=================================================================================================

bool BOPDS_DS::HasPaveBlocks(const int theIndex) const
{
  return ShapeInfo(theIndex).HasReference();
}

//=================================================================================================

const NCollection_List<occ::handle<BOPDS_PaveBlock>>& BOPDS_DS::PaveBlocks(const int theIndex) const
{
  if (HasPaveBlocks(theIndex))
  {
    return myPaveBlocksPool(ShapeInfo(theIndex).Reference());
  }

  static const NCollection_List<occ::handle<BOPDS_PaveBlock>> aStaticEmptyList;
  return aStaticEmptyList;
}

//=================================================================================================

NCollection_List<occ::handle<BOPDS_PaveBlock>>& BOPDS_DS::ChangePaveBlocks(const int theIndex)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theIndex);
  if (!aShapeInfo.HasReference())
  {
    InitPaveBlocks(theIndex);
  }
  return myPaveBlocksPool(aShapeInfo.Reference());
}

//=================================================================================================

void BOPDS_DS::InitPaveBlocks(const int theEdgeIndex)
{

  BOPDS_ShapeInfo&  anEdgeInfo = ChangeShapeInfo(theEdgeIndex);
  const TopoDS_Edge anEdge     = TopoDS::Edge(anEdgeInfo.Shape());

  const NCollection_List<int>& aVertexIndices = anEdgeInfo.SubShapes();
  if (aVertexIndices.IsEmpty())
  {
    return;
  }

  occ::handle<BOPDS_PaveBlock> aPaveBlock = new BOPDS_PaveBlock;
  aPaveBlock->SetOriginalEdge(theEdgeIndex);

  if (anEdge.Orientation() != TopAbs_INTERNAL)
  {
    for (auto aVertexIndex : aVertexIndices)
    {
      const BOPDS_ShapeInfo& aVertexInfo = ShapeInfo(aVertexIndex);
      TopoDS_Vertex          aVertex     = TopoDS::Vertex(aVertexInfo.Shape());
      // Important: for vertices HasFlag() means that the original edge is located on the infinite
      // curve and vertex was created during initialization of this class object.
      // Such vertices are not topologically connected to the edge so we cannot use
      // BRep_Tool::Parameter.
      const double aVertexParam = aVertexInfo.HasFlag() ? ComputeParameter(aVertex, anEdge)
                                                        : BRep_Tool::Parameter(aVertex, anEdge);

      aVertexIndex = GetSameDomainIndex(aVertexIndex);
      BOPDS_Pave aPave(aVertexIndex, aVertexParam);
      if (anEdgeInfo.HasFlag()) // for a degenerated edge append pave unconditionally
        aPaveBlock->AppendExtPave1(aPave);
      else
        aPaveBlock->AppendExtPave(aPave);

      // Handle closed edges (seam edges) that have a single vertex shared by both ends.
      // In this case, we need to add two paves: one for the start and one for the end
      // of the edge, even though they reference the same vertex.
      if (aVertexIndices.Size() == 1)
      {
        aVertex.Reverse();
        aPaveBlock->AppendExtPave1(BOPDS_Pave(aVertexIndex, BRep_Tool::Parameter(aVertex, anEdge)));
      }
    }
  }
  else
  {
    for (TopoDS_Iterator anEdgeIter(anEdge, false, true); anEdgeIter.More(); anEdgeIter.Next())
    {
      const TopoDS_Vertex&   aVertex      = TopoDS::Vertex(anEdgeIter.Value());
      const int              aVertexIndex = Index(aVertex);
      const BOPDS_ShapeInfo& aVertexInfo  = ShapeInfo(aVertexIndex);
      const double aVertexParam = aVertexInfo.HasFlag() ? ComputeParameter(aVertex, anEdge)
                                                        : BRep_Tool::Parameter(aVertex, anEdge);
      aPaveBlock->AppendExtPave1(BOPDS_Pave(GetSameDomainIndex(aVertexIndex), aVertexParam));
    }
  }

  aPaveBlock->Update(myPaveBlocksPool.Appended(), false);
  anEdgeInfo.SetReference(myPaveBlocksPool.Length() - 1);
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlocks()
{
  for (auto& aPaveBlockList : myPaveBlocksPool)
  {
    for (NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB(aPaveBlockList);
         aItPB.More();)
    {
      occ::handle<BOPDS_PaveBlock>& aPaveBlock = aItPB.ChangeValue();

      if (!aPaveBlock->IsToUpdate())
      {
        aItPB.Next();
        continue;
      }

      NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBN(myAllocator);
      aPaveBlock->Update(aLPBN);
      aPaveBlockList.Remove(aItPB);
      aPaveBlockList.Append(aLPBN);
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlock(const occ::handle<BOPDS_PaveBlock>& thePaveBlock)
{
  if (!thePaveBlock->IsToUpdate())
  {
    return;
  }

  const BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(thePaveBlock->OriginalEdge());
  NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = myPaveBlocksPool(aShapeInfo.Reference());

  for (NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB(aLPB); aItPB.More();
       aItPB.Next())
  {
    occ::handle<BOPDS_PaveBlock>& aPaveBlock = aItPB.ChangeValue();
    if (aPaveBlock == thePaveBlock)
    {
      NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBN(myAllocator);
      aPaveBlock->Update(aLPBN);
      aLPB.Append(aLPBN);
      aLPB.Remove(aItPB);
      break;
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateCommonBlock(const occ::handle<BOPDS_CommonBlock>& theCommonBlock,
                                 const double                          theFuzzyValue)
{
  const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aPaveBlocksOfCommonBlock =
    theCommonBlock->PaveBlocks();
  if (!aPaveBlocksOfCommonBlock.First()->IsToUpdate())
  {
    return;
  }

  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator                        aItPB;
  NCollection_DataMap<BOPDS_Pair, NCollection_List<occ::handle<BOPDS_PaveBlock>>> aMPKLPB;
  NCollection_DataMap<BOPDS_Pair, NCollection_List<occ::handle<BOPDS_PaveBlock>>>::Iterator
    aItMPKLPB;

  for (const auto& aPaveBlockOfCommonBlock : aPaveBlocksOfCommonBlock)
  {
    const int anEdgeIndex = aPaveBlockOfCommonBlock->OriginalEdge();
    NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB =
      myPaveBlocksPool(ChangeShapeInfo(anEdgeIndex).Reference());

    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      occ::handle<BOPDS_PaveBlock>& aPB = aItPB.ChangeValue();
      if (aPB == aPaveBlockOfCommonBlock)
      {

        NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBN;
        aPB->Update(aLPBN);
        for (const auto& aPBN : aLPBN)
        {
          aLPB.Append(aPBN);

          int n1, n2;
          aPBN->Indices(n1, n2);
          BOPDS_Pair aPK(n1, n2);
          if (NCollection_List<occ::handle<BOPDS_PaveBlock>>* aPaveBlocks = aMPKLPB.ChangeSeek(aPK))
          {
            aPaveBlocks->Append(aPBN);
          }
          else
          {
            NCollection_List<occ::handle<BOPDS_PaveBlock>> aNewLPBx;
            aNewLPBx.Append(aPBN);
            aMPKLPB.Bind(aPK, aNewLPBx);
          }
        }
        aLPB.Remove(aItPB);
        break;
      }
    }
  }

  aItMPKLPB.Initialize(aMPKLPB);
  for (; aItMPKLPB.More(); aItMPKLPB.Next())
  {
    NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPBx = aItMPKLPB.ChangeValue();

    while (!aLPBx.IsEmpty())
    {
      NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBxN;

      aItPB.Initialize(aLPBx);
      for (; aItPB.More();)
      {
        const occ::handle<BOPDS_PaveBlock>& aPBx = aItPB.Value();
        if (aLPBxN.Extent())
        {
          const occ::handle<BOPDS_PaveBlock>& aPBCx = aLPBxN.First();
          if (CheckCoincidence(aPBx, aPBCx, theFuzzyValue))
          {
            aLPBxN.Append(aPBx);
            aLPBx.Remove(aItPB);
            continue;
          }
        }
        else
        {
          aLPBxN.Append(aPBx);
          aLPBx.Remove(aItPB);
          continue;
        }
        aItPB.Next();
      }

      occ::handle<BOPDS_CommonBlock> aCommonBlock = new BOPDS_CommonBlock;
      aCommonBlock->SetPaveBlocks(aLPBxN);
      aCommonBlock->SetFaces(theCommonBlock->Faces());

      aItPB.Initialize(aLPBxN);
      for (; aItPB.More(); aItPB.Next())
      {
        SetCommonBlock(aItPB.ChangeValue(), aCommonBlock);
      }
    }
  }
}

//=================================================================================================

occ::handle<BOPDS_PaveBlock> BOPDS_DS::RealPaveBlock(
  const occ::handle<BOPDS_PaveBlock>& thePaveBlock) const
{
  const occ::handle<BOPDS_CommonBlock>& aCommonBlock = CommonBlock(thePaveBlock);
  return aCommonBlock ? aCommonBlock->PaveBlock1() : thePaveBlock;
}

//=================================================================================================

bool BOPDS_DS::IsCommonBlockOnEdge(const occ::handle<BOPDS_PaveBlock>& thePaveBlock) const
{
  const occ::handle<BOPDS_CommonBlock>& aCommonBlock = CommonBlock(thePaveBlock);
  return aCommonBlock && aCommonBlock->PaveBlocks().Size() > 1;
}

//=================================================================================================

bool BOPDS_DS::IsCommonBlock(const occ::handle<BOPDS_PaveBlock>& thePaveBlock) const
{
  return myMapPBCB.IsBound(thePaveBlock);
}

//=================================================================================================

occ::handle<BOPDS_CommonBlock> BOPDS_DS::CommonBlock(
  const occ::handle<BOPDS_PaveBlock>& thePaveBlock) const
{
  const occ::handle<BOPDS_CommonBlock>* aCommonBlock = myMapPBCB.Seek(thePaveBlock);
  return aCommonBlock ? *aCommonBlock : nullptr;
}

//=================================================================================================

void BOPDS_DS::SetCommonBlock(const occ::handle<BOPDS_PaveBlock>&   thePaveBlock,
                              const occ::handle<BOPDS_CommonBlock>& theCommonBlock)
{
  myMapPBCB.Bind(thePaveBlock, theCommonBlock);
}

//=================================================================================================

const NCollection_Vector<BOPDS_FaceInfo>& BOPDS_DS::FaceInfoPool() const
{
  return myFaceInfoPool;
}

//=================================================================================================

bool BOPDS_DS::HasFaceInfo(const int theIndex) const
{
  return ShapeInfo(theIndex).HasReference();
}

//=================================================================================================

const BOPDS_FaceInfo& BOPDS_DS::FaceInfo(const int theIndex) const
{
  if (!HasFaceInfo(theIndex))
  {
    static const BOPDS_FaceInfo anEmptyFaceInfo;
    return anEmptyFaceInfo;
  }

  return myFaceInfoPool(ShapeInfo(theIndex).Reference());
}

//=================================================================================================

BOPDS_FaceInfo& BOPDS_DS::ChangeFaceInfo(const int theIndex)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theIndex);
  if (!aShapeInfo.HasReference())
  {
    InitFaceInfo(theIndex);
  }
  return myFaceInfoPool(aShapeInfo.Reference());
}

//=================================================================================================

void BOPDS_DS::InitFaceInfo(const int theIndex)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theIndex);
  BOPDS_FaceInfo&  aFaceInfo  = myFaceInfoPool.Appended();
  aShapeInfo.SetReference(myFaceInfoPool.Length() - 1);
  aFaceInfo.SetIndex(theIndex);

  InitFaceInfoIn(theIndex);
  UpdateFaceInfoOn(theIndex);
}

//=================================================================================================

void BOPDS_DS::InitFaceInfoIn(const int theIndex)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theIndex);
  if (!aShapeInfo.HasReference())
  {
    return;
  }

  BOPDS_FaceInfo& aFaceInfo = myFaceInfoPool(aShapeInfo.Reference());
  for (TopoDS_Iterator aShapeIter(Shape(theIndex)); aShapeIter.More(); aShapeIter.Next())
  {
    const TopoDS_Shape& aVertex = aShapeIter.Value();
    if (aVertex.ShapeType() == TopAbs_VERTEX)
    {
      const int aVertexIndex = GetSameDomainIndex(Index(aVertex));
      aFaceInfo.ChangeVerticesIn().Add(aVertexIndex);
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoIn(const int theIndex)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theIndex);
  if (!aShapeInfo.HasReference())
  {
    return;
  }

  BOPDS_FaceInfo& aFaceInfo = myFaceInfoPool(aShapeInfo.Reference());
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPaveBlocksIn =
    aFaceInfo.ChangePaveBlocksIn();
  NCollection_Map<int>& aVerticesIn = aFaceInfo.ChangeVerticesIn();
  aPaveBlocksIn.Clear();
  aVerticesIn.Clear();
  FaceInfoIn(theIndex, aPaveBlocksIn, aVerticesIn);
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoOn(const int theIndex)
{
  BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(theIndex);
  if (!aShapeInfo.HasReference())
  {
    return;
  }

  BOPDS_FaceInfo& aFaceInfo = myFaceInfoPool(aShapeInfo.Reference());
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPaveBlocksOn =
    aFaceInfo.ChangePaveBlocksOn();
  NCollection_Map<int>& aVerticesOn = aFaceInfo.ChangeVerticesOn();
  aPaveBlocksOn.Clear();
  aVerticesOn.Clear();
  FaceInfoOn(theIndex, aPaveBlocksOn, aVerticesOn);
}

//=================================================================================================

void BOPDS_DS::FaceInfoOn(const int                                             theIndex,
                          NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPB,
                          NCollection_Map<int>&                                 theMVP)
{
  for (const auto& aSubShapeIndex : ShapeInfo(theIndex).SubShapes())
  {
    if (ShapeInfo(aSubShapeIndex).ShapeType() == TopAbs_EDGE)
    {
      for (const auto& aPaveBlock : PaveBlocks(aSubShapeIndex))
      {
        int nV1, nV2;
        aPaveBlock->Indices(nV1, nV2);
        theMVP.Add(nV1);
        theMVP.Add(nV2);
        theMPB.Add(RealPaveBlock(aPaveBlock));
      }
    }
    else // if (ShapeInfo(aSubShapeIndex).ShapeType() == TopAbs_VERTEX)
    {
      theMVP.Add(GetSameDomainIndex(aSubShapeIndex));
    }
  }
}

//=================================================================================================

void BOPDS_DS::FaceInfoIn(const int                                             theFaceIndex,
                          NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& thePaveBlocks,
                          NCollection_Map<int>&                                 theIndexMap)
{
  // 1. Pure internal vertices on the face
  // Adding to theIndexMap all the indicies of vertices belonging to the face.
  const TopoDS_Shape& aFace = Shape(theFaceIndex);
  for (TopoDS_Iterator aFaceIter(aFace); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Shape& aSubShape = aFaceIter.Value();
    if (aSubShape.ShapeType() == TopAbs_VERTEX)
    {
      const int aVertexIndex = GetSameDomainIndex(Index(aSubShape));
      theIndexMap.Add(aVertexIndex);
    }
  }

  // 2. Vertex-Face interferences
  // Adding to theIndexMap all the indicies of vertices having interferences with the face.
  for (const auto& aVertexFaceInterference : InterfVF())
  {
    if (aVertexFaceInterference.Contains(theFaceIndex))
    {
      const int aVertexIndex =
        GetSameDomainIndex(aVertexFaceInterference.OppositeIndex(theFaceIndex));
      theIndexMap.Add(aVertexIndex);
    }
  }

  // 3. Edge-Face interferences
  for (const auto& anEdgeFaceInterference : InterfEF())
  {
    if (!anEdgeFaceInterference.Contains(theFaceIndex))
    {
      continue;
    }

    if (std::optional<int> anIndexNew = anEdgeFaceInterference.GetIndexNew())
    {
      theIndexMap.Add(GetSameDomainIndex(*anIndexNew));
    }
    else
    {
      for (const auto& aPaveBlock : PaveBlocks(anEdgeFaceInterference.OppositeIndex(theFaceIndex)))
      {
        const occ::handle<BOPDS_CommonBlock> aCommonBlock = CommonBlock(aPaveBlock);
        if (aCommonBlock && aCommonBlock->Contains(theFaceIndex))
        {
          thePaveBlocks.Add(aCommonBlock->PaveBlock1());
        }
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoIn(const NCollection_Map<int>& theFaces)
{
  for (NCollection_Map<int>::Iterator aMapIter(theFaces); aMapIter.More(); aMapIter.Next())
  {
    const int        aFaceIndex = aMapIter.Value();
    BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(aFaceIndex);
    if (!aShapeInfo.HasReference())
    {
      myFaceInfoPool.Appended().SetIndex(aFaceIndex);
      aShapeInfo.SetReference(myFaceInfoPool.Length() - 1);
    }
    BOPDS_FaceInfo& aFaceInfo = myFaceInfoPool(aShapeInfo.Reference());
    aFaceInfo.ChangePaveBlocksIn().Clear();
    aFaceInfo.ChangeVerticesIn().Clear();

    // 1. Add pure internal vertices on the face
    InitFaceInfoIn(aFaceIndex);
  }

  // 2. Analyze Vertex-Face interferences
  for (const auto& aVertexFaceInterference : InterfVF())
  {
    const int aFaceIndex = aVertexFaceInterference.Index2();
    if (theFaces.Contains(aFaceIndex))
    {
      const int aSameDomainIndex = GetSameDomainIndex(aVertexFaceInterference.Index1());
      myFaceInfoPool(ShapeInfo(aFaceIndex).Reference()).ChangeVerticesIn().Add(aSameDomainIndex);
    }
  }

  // 3. Analyze Edge-Face interferences
  for (const auto& anEdgeFaceInterference : InterfEF())
  {
    const int aFaceIndex = anEdgeFaceInterference.Index2();
    if (!theFaces.Contains(aFaceIndex))
    {
      continue;
    }

    BOPDS_FaceInfo& aFaceInfo = myFaceInfoPool(ShapeInfo(aFaceIndex).Reference());
    if (std::optional<int> anIndexNew = anEdgeFaceInterference.GetIndexNew())
    {
      aFaceInfo.ChangeVerticesIn().Add(GetSameDomainIndex(*anIndexNew));
    }
    else
    {
      for (const auto& aPaveBlock : PaveBlocks(anEdgeFaceInterference.Index1()))
      {
        const occ::handle<BOPDS_CommonBlock>& aCommonBlock = CommonBlock(aPaveBlock);
        if (aCommonBlock && aCommonBlock->Contains(aFaceIndex))
        {
          aFaceInfo.ChangePaveBlocksIn().Add(aCommonBlock->PaveBlock1());
        }
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateFaceInfoOn(const NCollection_Map<int>& theFaces)
{
  for (NCollection_Map<int>::Iterator aMapIterM(theFaces); aMapIterM.More(); aMapIterM.Next())
  {
    const int        aShapeIndex = aMapIterM.Value();
    BOPDS_ShapeInfo& aShapeInfo  = ChangeShapeInfo(aShapeIndex);
    if (!aShapeInfo.HasReference())
    {
      myFaceInfoPool.Appended().SetIndex(aShapeIndex);
      aShapeInfo.SetReference(myFaceInfoPool.Length() - 1);
    }
    BOPDS_FaceInfo& aFaceInfo = myFaceInfoPool(aShapeInfo.Reference());
    aFaceInfo.ChangePaveBlocksOn().Clear();
    aFaceInfo.ChangeVerticesOn().Clear();

    FaceInfoOn(aShapeIndex, aFaceInfo.ChangePaveBlocksOn(), aFaceInfo.ChangeVerticesOn());
  }
}

//=================================================================================================

void BOPDS_DS::RefineFaceInfoOn()
{
  for (auto& aFaceInfo : myFaceInfoPool)
  {
    UpdateFaceInfoOn(aFaceInfo.Index());
    NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPaveBlocksOn =
      aFaceInfo.ChangePaveBlocksOn();
    for (int aPaveBlockIndex = aPaveBlocksOn.Size(); aPaveBlockIndex >= 1; --aPaveBlockIndex)
    {
      const occ::handle<BOPDS_PaveBlock>& aPaveBlock = aPaveBlocksOn(aPaveBlockIndex);
      if (!aPaveBlock->HasEdge())
      {
        aPaveBlocksOn.RemoveFromIndex(aPaveBlockIndex);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::RefineFaceInfoIn()
{
  for (int i = 0; i < myNbSourceShapes; ++i)
  {
    const BOPDS_ShapeInfo& aShapeInfo = ShapeInfo(i);
    if (aShapeInfo.ShapeType() != TopAbs_FACE || !aShapeInfo.HasReference())
    {
      continue;
    }

    BOPDS_FaceInfo&                                             aFaceInfo = ChangeFaceInfo(i);
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPaveBlocksOn =
      aFaceInfo.PaveBlocksOn();
    NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPaveBlocksIn =
      aFaceInfo.ChangePaveBlocksIn();
    if (aPaveBlocksIn.IsEmpty() || aPaveBlocksOn.IsEmpty())
    {
      continue;
    }

    for (int aPaveBlockIndex = aPaveBlocksIn.Size(); aPaveBlockIndex >= 1; --aPaveBlockIndex)
    {
      const occ::handle<BOPDS_PaveBlock>& aPaveBlock = aPaveBlocksIn(aPaveBlockIndex);
      if (aPaveBlocksOn.Contains(aPaveBlock))
      {
        aPaveBlocksIn.RemoveFromIndex(aPaveBlockIndex);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::AloneVertices(const int theFaceIndex, NCollection_List<int>& theVertexList) const
{
  if (!HasFaceInfo(theFaceIndex))
  {
    return;
  }

  NCollection_Map<int>  aMI(100, myAllocator);
  const BOPDS_FaceInfo& aFaceInfo = FaceInfo(theFaceIndex);

  for (const auto& aPaveBlocks : {aFaceInfo.PaveBlocksIn(), aFaceInfo.PaveBlocksSc()})
  {
    for (int aPaveBlockIndex = 1; aPaveBlockIndex <= aPaveBlocks.Size(); ++aPaveBlockIndex)
    {
      const occ::handle<BOPDS_PaveBlock>& aPaveBlock = aPaveBlocks(aPaveBlockIndex);
      int                                 nV1, nV2;
      aPaveBlock->Indices(nV1, nV2);
      aMI.Add(nV1);
      aMI.Add(nV2);
    }
  }

  for (const auto& aVertexMap : {aFaceInfo.VerticesIn(), aFaceInfo.VerticesSc()})
  {
    for (NCollection_Map<int>::Iterator aVertexIndexIter(aVertexMap); aVertexIndexIter.More();
         aVertexIndexIter.Next())
    {
      const int aVertexIndex = aVertexIndexIter.Value();
      if (aVertexIndex >= 0 && aMI.Add(aVertexIndex))
      {
        theVertexList.Append(aVertexIndex);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::SubShapesOnIn(
  const int                                             theFaceIndex1,
  const int                                             theFaceIndex2,
  NCollection_Map<int>&                                 theMVOnIn,
  NCollection_Map<int>&                                 theMVCommon,
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& thePBOnIn,
  NCollection_Map<occ::handle<BOPDS_PaveBlock>>&        theCommonPaveBlocks) const
{
  const BOPDS_FaceInfo& aFaceInfo1 = FaceInfo(theFaceIndex1);
  const BOPDS_FaceInfo& aFaceInfo2 = FaceInfo(theFaceIndex2);

  // Use const references to avoid copying large IndexedMaps.
  // Previous implementation copied 4 maps per call, causing significant memory overhead.
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPBOn1 = aFaceInfo1.PaveBlocksOn();
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPBIn1 = aFaceInfo1.PaveBlocksIn();
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPBOn2 = aFaceInfo2.PaveBlocksOn();
  const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aPBIn2 = aFaceInfo2.PaveBlocksIn();

  // Helper lambda to process pave blocks from a map
  auto processMap =
    [&thePBOnIn, &theMVOnIn](const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMap) {
      for (int anIdx = 1; anIdx <= theMap.Size(); ++anIdx)
      {
        const occ::handle<BOPDS_PaveBlock>& aPaveBlock = theMap(anIdx);
        thePBOnIn.Add(aPaveBlock);
        int aV1, aV2;
        aPaveBlock->Indices(aV1, aV2);
        theMVOnIn.Add(aV1);
        theMVOnIn.Add(aV2);
      }
    };

  // Process all four maps
  processMap(aPBOn1);
  processMap(aPBIn1);
  processMap(aPBOn2);
  processMap(aPBIn2);

  // Find common pave blocks (those in Face1 that are also in Face2)
  auto findCommon = [&theCommonPaveBlocks, &theMVCommon, &aPBOn2, &aPBIn2](
                      const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMap) {
    for (int anIdx = 1; anIdx <= theMap.Size(); ++anIdx)
    {
      const occ::handle<BOPDS_PaveBlock>& aPaveBlock = theMap(anIdx);
      if (aPBOn2.Contains(aPaveBlock) || aPBIn2.Contains(aPaveBlock))
      {
        theCommonPaveBlocks.Add(aPaveBlock);
        int aV1, aV2;
        aPaveBlock->Indices(aV1, aV2);
        theMVCommon.Add(aV1);
        theMVCommon.Add(aV2);
      }
    }
  };

  findCommon(aPBOn1);
  findCommon(aPBIn1);

  const NCollection_Map<int>& aMVOn1 = aFaceInfo1.VerticesOn();
  const NCollection_Map<int>& aMVIn1 = aFaceInfo1.VerticesIn();
  const NCollection_Map<int>& aMVOn2 = aFaceInfo2.VerticesOn();
  const NCollection_Map<int>& aMVIn2 = aFaceInfo2.VerticesIn();

  for (const auto& aMV1 : {aMVOn1, aMVIn1})
  {
    for (NCollection_Map<int>::Iterator aIt(aMV1); aIt.More(); aIt.Next())
    {
      const int aVertexIndex = aIt.Value();
      if (aMVOn2.Contains(aVertexIndex) || aMVIn2.Contains(aVertexIndex))
      {
        theMVOnIn.Add(aVertexIndex);

        // Vertex taken from the 1st face is in the 2nd one.
        theMVCommon.Add(aVertexIndex);
      }
    }
  }
}

//=================================================================================================

void BOPDS_DS::SharedEdges(const int                                     theFaceIndex1,
                           const int                                     theFaceIndex2,
                           NCollection_List<int>&                        theEdgeList,
                           const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  NCollection_Map<int> aFirstFaceEdges(100, theAllocator);

  // Collect edges of the first face.
  for (const auto& aSubShapeIndex : ShapeInfo(theFaceIndex1).SubShapes())
  {
    const BOPDS_ShapeInfo& aSubShapeInfo = ChangeShapeInfo(aSubShapeIndex);
    if (aSubShapeInfo.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }

    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aSubShapePaveBlocks =
      PaveBlocks(aSubShapeIndex);
    if (aSubShapePaveBlocks.IsEmpty())
    {
      aFirstFaceEdges.Add(aSubShapeIndex);
    }
    else
    {
      for (const auto& aPaveBlock : aSubShapePaveBlocks)
      {
        aFirstFaceEdges.Add(RealPaveBlock(aPaveBlock)->Edge());
      }
    }
  }

  // Add edges of the second face if they are contained in the first one.
  for (const auto& aSubShapeIndex : ShapeInfo(theFaceIndex2).SubShapes())
  {
    const BOPDS_ShapeInfo& aSubShapeInfo = ChangeShapeInfo(aSubShapeIndex);
    if (aSubShapeInfo.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }

    const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aSubShapePaveBlocks =
      PaveBlocks(aSubShapeIndex);
    if (aSubShapePaveBlocks.IsEmpty())
    {
      if (aFirstFaceEdges.Contains(aSubShapeIndex))
      {
        theEdgeList.Append(aSubShapeIndex);
      }
    }
    else
    {
      for (const auto& aPaveBlock : aSubShapePaveBlocks)
      {
        const int anEdgeIndex = RealPaveBlock(aPaveBlock)->Edge();
        if (aFirstFaceEdges.Contains(anEdgeIndex))
        {
          theEdgeList.Append(anEdgeIndex);
        }
      }
    }
  }
}

//=================================================================================================

NCollection_DataMap<int, int>& BOPDS_DS::ShapesSD()
{
  return myShapesSD;
}

//=================================================================================================

void BOPDS_DS::AddShapeSD(const int theIndex, const int theIndexSD)
{
  if (theIndex != theIndexSD)
  {
    myShapesSD.Bind(theIndex, theIndexSD);
  }
}

//=================================================================================================

bool BOPDS_DS::HasShapeSD(const int theIndex, int& theIndexSD) const
{
  bool       bHasSD = false;
  const int* pSD    = myShapesSD.Seek(theIndex);
  while (pSD)
  {
    theIndexSD = *pSD;
    bHasSD     = true;
    pSD        = myShapesSD.Seek(theIndexSD);
  }
  return bHasSD;
}

//=================================================================================================

int BOPDS_DS::GetSameDomainIndex(const int theIndex) const
{
  int aResult = theIndex;
  for (const int* aSameDomainIndex = myShapesSD.Seek(aResult); aSameDomainIndex;
       aSameDomainIndex            = myShapesSD.Seek(aResult))
  {
    aResult = *aSameDomainIndex;
  }
  return aResult;
}

//=================================================================================================

void BOPDS_DS::Dump() const
{
  printf(" *** DS ***\n");
  const int aRangesCount = NbRanges();
  printf(" Ranges:%d\n", aRangesCount);
  for (int i = 0; i < aRangesCount; ++i)
  {
    const BOPDS_IndexRange& aRange = Range(i);
    aRange.Dump();
    printf("\n");
  }
  //
  const int aSourceShapesCount = NbSourceShapes();
  printf(" Shapes:%d\n", aSourceShapesCount);
  const int aShapesCount = NbShapes();
  for (int i = 0; i < aShapesCount; ++i)
  {
    const BOPDS_ShapeInfo& aShapeInfo = ShapeInfo(i);
    printf(" %d :", i);
    aShapeInfo.Dump();
    printf("\n");
    if (i == aSourceShapesCount - 1)
    {
      printf(" ****** adds\n");
    }
  }
  printf(" ******\n");
}

//=================================================================================================

bool BOPDS_DS::CheckCoincidence(const occ::handle<BOPDS_PaveBlock>& thePaveBlock1,
                                const occ::handle<BOPDS_PaveBlock>& thePaveBlock2,
                                const double                        theFuzzyValue)
{
  // Find an intermediate point of the first pave block.
  // Seems like it doesn't matter where exactly this point is located on the pave block.
  gp_Pnt aPaveBlock1IntermediatePoint;
  double aPaveBlock1First, aPaveBlock1Last;
  thePaveBlock1->Range(aPaveBlock1First, aPaveBlock1Last);
  const double aPaveBlock1Intermediate =
    IntTools_Tools::IntermediatePoint(aPaveBlock1First, aPaveBlock1Last);
  const TopoDS_Edge& anEdge1 = TopoDS::Edge(Shape(thePaveBlock1->OriginalEdge()));
  BOPTools_AlgoTools::PointOnEdge(anEdge1, aPaveBlock1Intermediate, aPaveBlock1IntermediatePoint);

  // Obtain the curve of the second edge and project the point onto it.
  double aPaveBlock2First, aPaveBlock2Last;
  thePaveBlock2->Range(aPaveBlock2First, aPaveBlock2Last);
  const TopoDS_Edge&            anEdge2 = TopoDS::Edge(Shape(thePaveBlock2->OriginalEdge()));
  double                        anEdge2ParamFirst, anEdge2ParamLast;
  const occ::handle<Geom_Curve> anEdgeCurve2 =
    BRep_Tool::Curve(anEdge2, anEdge2ParamFirst, anEdge2ParamLast);
  GeomAPI_ProjectPointOnCurve aProjector;
  aProjector.Init(anEdgeCurve2, anEdge2ParamFirst, anEdge2ParamLast);
  aProjector.Perform(aPaveBlock1IntermediatePoint);
  // Check the results of projection.
  if (aProjector.NbPoints() != 0)
  {
    const double aDistance  = aProjector.LowerDistance();
    const double aTolerance = BRep_Tool::MaxTolerance(anEdge1, TopAbs_VERTEX)
                              + BRep_Tool::MaxTolerance(anEdge2, TopAbs_VERTEX)
                              + std::max(theFuzzyValue, Precision::Confusion());
    // If the distance is less than tolerance, check that the projection
    // point lies within the parameter range of the second pave block.
    if (aDistance < aTolerance)
    {
      const double aDistanceParam = aProjector.LowerDistanceParameter();
      if (aDistanceParam > aPaveBlock2First && aDistanceParam < aPaveBlock2Last)
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool BOPDS_DS::IsSubShape(const int theCandidate, const int theParent)
{
  const NCollection_List<int>& aSubShapeIndices = ShapeInfo(theParent).SubShapes();
  return std::any_of(
    aSubShapeIndices.begin(),
    aSubShapeIndices.end(),
    [theCandidate](int theSubShapeIndex) { return theSubShapeIndex == theCandidate; });
}

//=================================================================================================

void BOPDS_DS::Paves(const int theEdge, NCollection_List<BOPDS_Pave>& theResultPaves)
{
  // Note: This algorithm is supposed to return unique sorted paves.
  // It seemed like it would be reasonable to use std::set for that purpose.
  // However, BOPDS_Pave's operators < and == are not consistent (essentially it means that
  // if a < b == false and b < a == false, it does not imply a == b), which may lead to unexpected
  // results.
  // To preserve the original behavior, uniqueness should be defined by operator ==, which
  // checks for equality for both index and parameter, while sorting should be done
  // by operator <, which compares only parameters.

  const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aPaveBlocks = PaveBlocks(theEdge);
  if (aPaveBlocks.IsEmpty())
  {
    return;
  }

  NCollection_Array1<BOPDS_Pave> pPaves(1, aPaveBlocks.Size() + 1);
  int                            i = 1;
  NCollection_Map<BOPDS_Pave>    aVisitedPaves;
  for (const auto& aPaveBlock : aPaveBlocks)
  {
    for (const auto& aPave : {aPaveBlock->Pave1(), aPaveBlock->Pave2()})
    {
      if (aVisitedPaves.Add(aPave))
      {
        pPaves(i) = aPave;
        ++i;
      }
    }
  }

  Standard_ASSERT_VOID(aPaveBlocks.Size() + 1 == aVisitedPaves.Size(), "Abnormal number of paves");

  std::sort(pPaves.begin(), pPaves.end());
  for (const auto& aPave : pPaves)
  {
    theResultPaves.Append(aPave);
  }
}

//=================================================================================================

void BOPDS_DS::BuildBndBoxSolid(const int theIndex, Bnd_Box& theBox, const bool theCheckInverted)
{
  const BOPDS_ShapeInfo& aShapeInfo = ShapeInfo(theIndex);

  bool anIsOpenBox = false;
  for (const auto& aSubShapeIndex : aShapeInfo.SubShapes())
  {
    const BOPDS_ShapeInfo& aShellInfo = ShapeInfo(aSubShapeIndex);
    if (aShellInfo.ShapeType() != TopAbs_SHELL)
    {
      continue;
    }

    for (const auto& aFaceIndex : aShellInfo.SubShapes())
    {
      const BOPDS_ShapeInfo& aFaceInfo = ShapeInfo(aFaceIndex);
      if (aFaceInfo.ShapeType() != TopAbs_FACE)
      {
        continue;
      }

      const Bnd_Box& aFaceBoundBox = aFaceInfo.Box();
      theBox.Add(aFaceBoundBox);

      anIsOpenBox = aFaceBoundBox.IsOpen();
      if (anIsOpenBox)
      {
        break;
      }
    }

    if (!anIsOpenBox)
    {
      const TopoDS_Shell& aShell = TopoDS::Shell(aShellInfo.Shape());
      anIsOpenBox                = BOPTools_AlgoTools::IsOpenShell(aShell);
    }

    if (anIsOpenBox)
    {
      break;
    }
  }

  if (anIsOpenBox)
  {
    theBox.SetWhole();
  }
  else if (theCheckInverted)
  {
    const TopoDS_Solid& aSolid = TopoDS::Solid(aShapeInfo.Shape());
    if (BOPTools_AlgoTools::IsInvertedSolid(aSolid))
    {
      theBox.SetWhole();
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlocksWithSDVertices()
{
  for (const auto& aPaveBlockList : myPaveBlocksPool)
  {
    for (const auto& aPaveBlock : aPaveBlockList)
    {
      UpdatePaveBlockWithSDVertices(aPaveBlock);
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdatePaveBlockWithSDVertices(const occ::handle<BOPDS_PaveBlock>& thePaveBlock)
{
  BOPDS_Pave aPave1   = thePaveBlock->Pave1();
  const int  anIndex1 = GetSameDomainIndex(aPave1.Index());
  aPave1.SetIndex(anIndex1);
  thePaveBlock->SetPave1(aPave1);

  BOPDS_Pave aPave2   = thePaveBlock->Pave2();
  const int  anIndex2 = GetSameDomainIndex(aPave2.Index());
  aPave2.SetIndex(anIndex2);
  thePaveBlock->SetPave2(aPave2);
}

//=================================================================================================

void BOPDS_DS::UpdateCommonBlockWithSDVertices(const occ::handle<BOPDS_CommonBlock>& theCommonBlock)
{
  for (const auto& aPaveBlock : theCommonBlock->PaveBlocks())
  {
    UpdatePaveBlockWithSDVertices(aPaveBlock);
  }
}

//=================================================================================================

void BOPDS_DS::InitPaveBlocksForVertex(const int theVertexIndex)
{
  const NCollection_List<int>* anEdgeIndices = myMapVE.Seek(theVertexIndex);
  if (!anEdgeIndices)
  {
    return;
  }

  for (const auto& anEdgeIndex : *anEdgeIndices)
  {
    ChangePaveBlocks(anEdgeIndex);
  }
}

//=================================================================================================

void BOPDS_DS::ReleasePaveBlocks()
{
  // It is necessary to remove the reference to PaveBlocks for the untouched
  // edges to avoid creation of the same images for them.
  // Pave blocks for this reference should be cleared.
  // This will allow to differ the small edges, for which it is
  // impossible to even build a pave block from the normal edges for which the
  // pave block have been created, but stayed untouched.
  // The small edge, for which no pave blocks have been created,
  // should be avoided in the result, thus the reference to empty list
  // of pave blocks will stay to mark the edge as Deleted.

  for (auto& aPaveBlockList : ChangePaveBlocksPool())
  {
    if (aPaveBlockList.Size() != 1)
    {
      continue;
    }

    const occ::handle<BOPDS_PaveBlock>& aPaveBlock = aPaveBlockList.First();
    if (IsCommonBlock(aPaveBlock))
    {
      continue;
    }

    int aShapeIndex1, aShapeIndex2;
    aPaveBlock->Indices(aShapeIndex1, aShapeIndex2);
    if (!IsNewShape(aShapeIndex1) && !IsNewShape(aShapeIndex2))
    {
      // Both vertices are original, thus the PB is untouched.
      // Remove reference for the original edge
      const int anOrigEdgeIndex = aPaveBlock->OriginalEdge();
      if (anOrigEdgeIndex >= 0)
      {
        ChangeShapeInfo(anOrigEdgeIndex).SetReference(-1);
      }
      // Clear contents of the list
      aPaveBlockList.Clear();
    }
  }
}

//=================================================================================================

bool BOPDS_DS::IsValidShrunkData(const occ::handle<BOPDS_PaveBlock>& thePaveBlock)
{
  if (!thePaveBlock->HasShrunkData())
  {
    return false;
  }

  // Compare the distances from the bounds of the shrunk range to the vertices
  // with the tolerance values of vertices

  // Shrunk range
  Bnd_Box aDummyBox;  // Unused.
  bool    aDummyFlag; // Unused.
  double  aParameter[2];
  int     aVertexIndex[2];
  thePaveBlock->ShrunkData(aParameter[0], aParameter[1], aDummyBox, aDummyFlag);
  thePaveBlock->Indices(aVertexIndex[0], aVertexIndex[1]);

  const TopoDS_Edge& anEdge = TopoDS::Edge(Shape(thePaveBlock->OriginalEdge()));
  BRepAdaptor_Curve  aCurveAdaptor(anEdge);
  const double       anEpsilon = BRep_Tool::Tolerance(anEdge) * 0.01;

  for (int i = 0; i < 2; ++i)
  {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(Shape(aVertexIndex[i]));
    const double         aTol    = BRep_Tool::Tolerance(aVertex) + Precision::Confusion();
    // Bounding point
    const gp_Pnt aVertexPoint = BRep_Tool::Pnt(aVertex);

    // Point on the end of shrunk range
    const gp_Pnt aPointOnEdge = aCurveAdaptor.Value(aParameter[i]);

    if (aTol - aVertexPoint.Distance(aPointOnEdge) > anEpsilon)
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

int BOPDS_DS::prepareVertices(const double theAdditionalTolerance)
{
  int aVertexCount = 0;

  for (int aVertexIndex = 0; aVertexIndex < myNbSourceShapes; ++aVertexIndex)
  {
    BOPDS_ShapeInfo& aVertexInfo = ChangeShapeInfo(aVertexIndex);
    if (aVertexInfo.ShapeType() != TopAbs_VERTEX)
    {
      continue;
    }
    ++aVertexCount;

    const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVertexInfo.Shape());

    Bnd_Box& aVertexBoundBox = aVertexInfo.ChangeBox();
    aVertexBoundBox.SetGap(BRep_Tool::Tolerance(aVertex) + theAdditionalTolerance);
    aVertexBoundBox.Add(BRep_Tool::Pnt(aVertex));
  }

  return aVertexCount;
}

//=================================================================================================

int BOPDS_DS::prepareEdges(const double theAdditionalTolerance)
{
  int anEdgeCount = 0;

  for (int anEdgeIndex = 0; anEdgeIndex < myNbSourceShapes; ++anEdgeIndex)
  {
    BOPDS_ShapeInfo& anEdgeInfo = ChangeShapeInfo(anEdgeIndex);
    if (anEdgeInfo.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }
    ++anEdgeCount;

    const TopoDS_Edge& anEdge          = TopoDS::Edge(anEdgeInfo.Shape());
    const double       anEdgeTolerance = BRep_Tool::Tolerance(anEdge);

    if (!BRep_Tool::Degenerated(anEdge))
    {
      // This is processing of special case when edge has infinite curve.
      // In this case vertices are created at the infinite positions
      // and added to the edge sub-shape list.

      NCollection_List<int>& aVertexIndices = anEdgeInfo.ChangeSubShapes();

      TopoDS_Edge aForwardEdge = anEdge;
      aForwardEdge.Orientation(TopAbs_FORWARD);
      double                        aCurveStart, aCurveEnd;
      const occ::handle<Geom_Curve> aForwardEdgeCurve =
        BRep_Tool::Curve(aForwardEdge, aCurveStart, aCurveEnd);

      // Process infinite values of the edge curve parameters.
      // Creates vertices at the infinite positions and add them to the edge sub-shape list.
      auto processInfinite =
        [this, &aForwardEdgeCurve, &aVertexIndices, anEdgeTolerance](double theParam) {
          gp_Pnt aPoint;
          aForwardEdgeCurve->D0(theParam, aPoint);
          TopoDS_Vertex aVertex;
          BRep_Builder  aBuilder;
          aBuilder.MakeVertex(aVertex, aPoint, anEdgeTolerance);
          aVertex.Orientation(TopAbs_FORWARD);

          BOPDS_ShapeInfo aVertexInfo;
          aVertexInfo.SetShape(aVertex);
          aVertexInfo.SetShapeType(TopAbs_VERTEX);
          aVertexInfo.SetFlag(1); // infinite flag

          aVertexIndices.Append(Append(aVertexInfo));
        };

      if (Precision::IsNegativeInfinite(aCurveStart))
      {
        processInfinite(aCurveStart);
      }
      if (Precision::IsPositiveInfinite(aCurveEnd))
      {
        processInfinite(aCurveEnd);
      }
    }
    else
    {
      anEdgeInfo.SetFlag(anEdgeIndex);
    }

    // Update edge bounding box with its own bounding box.
    Bnd_Box& anEdgeBoundBox = anEdgeInfo.ChangeBox();
    BRepBndLib::Add(anEdge, anEdgeBoundBox);

    // Add bounding boxes of vertices to the edge bounding box.
    for (const auto& aVertexIndex : anEdgeInfo.SubShapes())
    {
      BOPDS_ShapeInfo& aVertexInfo = ChangeShapeInfo(aVertexIndex);
      anEdgeBoundBox.Add(aVertexInfo.Box());
    }

    anEdgeBoundBox.SetGap(anEdgeBoundBox.GetGap() + theAdditionalTolerance);
  }

  return anEdgeCount;
}

//=================================================================================================

int BOPDS_DS::prepareFaces(const double theAdditionalTolerance)
{
  int aFaceCount = 0;

  for (int aFaceIndex = 0; aFaceIndex < myNbSourceShapes; ++aFaceIndex)
  {
    // Iterate over all faces.
    BOPDS_ShapeInfo& aFaceInfo = ChangeShapeInfo(aFaceIndex);
    if (aFaceInfo.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    ++aFaceCount;

    // Map of sub-shape indices for the face that will replace the current list of wire indices.
    NCollection_Map<int> aNewSubShapeIndices(100, NCollection_BaseAllocator::CommonBaseAllocator());

    const TopoDS_Shape& aFace = aFaceInfo.Shape();

    // Update face bounding box with its own bounding box.
    Bnd_Box& aFaceBoundBox = aFaceInfo.ChangeBox();
    BRepBndLib::Add(aFace, aFaceBoundBox);

    // Container of the face sub-shape indices. Currently contains wire indices.
    // Will be updated to contain edge and vertex indices instead.
    NCollection_List<int>& aFaceSubShapes = aFaceInfo.ChangeSubShapes();

    // Iterate over edges and vertices of the face and fill the new map of sub-shape indices.
    // Also marks degenerated edges on the face.
    for (const auto& aWireIndex : aFaceSubShapes)
    {
      for (const auto& anEdgeIndex : ChangeShapeInfo(aWireIndex).SubShapes())
      {
        // Update face bounding box with edge bounding boxes.
        BOPDS_ShapeInfo& anEdgeInfo     = ChangeShapeInfo(anEdgeIndex);
        Bnd_Box&         anEdgeBoundBox = anEdgeInfo.ChangeBox();
        aFaceBoundBox.Add(anEdgeBoundBox);

        // Add edge index to the map of indices.
        aNewSubShapeIndices.Add(anEdgeIndex);

        // Mark degenerated edges on the face.
        // It is unclear what this flag means and how exactly it is used.
        // Presumably, it means different things in different contexts.
        // Setting this flag (if it is even a flag at this point) for degenerated edges
        // to the value of the face index is weird but such is the original code.
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeInfo.Shape());
        if (BRep_Tool::Degenerated(anEdge))
        {
          anEdgeInfo.SetFlag(aFaceIndex);
        }

        // Add vertices of the edge to the map of indices.
        for (const auto& aVertexIndex : anEdgeInfo.SubShapes())
        {
          aNewSubShapeIndices.Add(aVertexIndex);
        }
      }
    }

    // Add vertices of the face to the map of indices.
    for (TopoDS_Iterator aFaceIter(aFace); aFaceIter.More(); aFaceIter.Next())
    {
      const TopoDS_Shape& aSubShape = aFaceIter.Value();
      if (aSubShape.ShapeType() == TopAbs_VERTEX)
      {
        aNewSubShapeIndices.Add(Index(aSubShape));
      }
    }

    // Remove wire indices and fill in edge and vertex indices instead.
    aFaceSubShapes.Clear();
    for (NCollection_Map<int>::Iterator aSubShapeIndicesIter(aNewSubShapeIndices);
         aSubShapeIndicesIter.More();
         aSubShapeIndicesIter.Next())
    {
      aFaceSubShapes.Append(aSubShapeIndicesIter.Value());
    }

    aFaceBoundBox.SetGap(aFaceBoundBox.GetGap() + theAdditionalTolerance);
  }

  return aFaceCount;
}

//=================================================================================================

int BOPDS_DS::prepareSolids()
{
  int aSolidCount = 0;

  // For the check mode we need to compute the bounding box for solid.
  // Otherwise, it will be computed on the building stage
  if (myArguments.Size() != 1)
  {
    return 0;
  }

  for (int aSolidIndex = 0; aSolidIndex < myNbSourceShapes; ++aSolidIndex)
  {
    BOPDS_ShapeInfo& aSolidInfo = ChangeShapeInfo(aSolidIndex);
    if (aSolidInfo.ShapeType() != TopAbs_SOLID)
    {
      continue;
    }
    ++aSolidCount;

    // Map of sub-shape indices for the face that will replace the current list of wire indices.
    NCollection_Map<int> aNewSubShapeIndices(100, NCollection_BaseAllocator::CommonBaseAllocator());

    // Update solid bounding box with its own bounding box.
    Bnd_Box& aSolidBoundBox = aSolidInfo.ChangeBox();
    BuildBndBoxSolid(aSolidIndex, aSolidBoundBox);

    // Container of the solid sub-shape indices. Currently contains shell indices.
    // Will be updated to contain face and edge indices instead.
    NCollection_List<int>& aSolidSubShapes = aSolidInfo.ChangeSubShapes();

    // Iterate over faces and edges of the solid and fill the new map of sub-shape indices.
    for (const auto& aShellIndex : aSolidSubShapes)
    {
      BOPDS_ShapeInfo& aShellInfo = ChangeShapeInfo(aShellIndex);
      if (aShellInfo.ShapeType() != TopAbs_SHELL)
      {
        continue;
      }

      for (const auto& aFaceIndex : aShellInfo.SubShapes())
      {
        BOPDS_ShapeInfo& aFaceInfo = ChangeShapeInfo(aFaceIndex);
        if (aFaceInfo.ShapeType() != TopAbs_FACE)
        {
          continue;
        }
        // Add face index to the map of indices.
        aNewSubShapeIndices.Add(aFaceIndex);

        for (const auto& anEdgeIndex : aFaceInfo.SubShapes())
        {
          // Add edge index to the map of indices.
          aNewSubShapeIndices.Add(anEdgeIndex);
        }
      }
    }

    // Remove shell indices and fill in face and edge indices instead.
    aSolidSubShapes.Clear();
    for (NCollection_Map<int>::Iterator aSubShapeIndicesIter(aNewSubShapeIndices);
         aSubShapeIndicesIter.More();
         aSubShapeIndicesIter.Next())
    {
      aSolidSubShapes.Append(aSubShapeIndicesIter.Value());
    }
  }

  return aSolidCount;
}

//=================================================================================================

void BOPDS_DS::buildVertexEdgeMap()
{
  for (int aShapeIndex = 0; aShapeIndex < myNbSourceShapes; ++aShapeIndex)
  {
    BOPDS_ShapeInfo& anEdgeInfo = ChangeShapeInfo(aShapeIndex);
    if (anEdgeInfo.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }

    for (const auto& aSubShapeIndex : anEdgeInfo.SubShapes())
    {
      NCollection_List<int>* aVertexIndices = myMapVE.ChangeSeek(aSubShapeIndex);
      if (!aVertexIndices)
      {
        aVertexIndices = myMapVE.Bound(aSubShapeIndex, NCollection_List<int>(myAllocator));
        aVertexIndices->Append(aShapeIndex);
      }
      else
      {
        // provide uniqueness of the edges in the list
        if (std::none_of(aVertexIndices->begin(),
                         aVertexIndices->end(),
                         [aShapeIndex](int theEdgeIndex) { return theEdgeIndex == aShapeIndex; }))
        {
          aVertexIndices->Append(aShapeIndex);
        }
      }
    }
  }
}
