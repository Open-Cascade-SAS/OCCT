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

namespace
{
//==================================================================================================

// Recursively collects all unique sub-shapes of theShape into theShapeMap.
void CollectShapesRecursively(const TopoDS_Shape&                                     theShape,
                              NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theShapeMap)
{
  if (!theShapeMap.Add(theShape))
  {
    return;
  }

  for (TopoDS_Iterator aIt(theShape); aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSubShape = aIt.Value();
    CollectShapesRecursively(aSubShape, theShapeMap);
  }
}

// Returns the count of unique sub-shapes of theShape.
// Return value is never less than 1 (theShape itself).
int CountUniqueShapes(const TopoDS_Shape& theShape)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aUniqueShapes;
  CollectShapesRecursively(theShape, aUniqueShapes);
  return aUniqueShapes.Extent();
}

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
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
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

void BOPDS_DS::SetArguments(const NCollection_List<TopoDS_Shape>& theLS)
{
  myArguments = theLS;
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
  NCollection_List<int>::Iterator aIt2;

  // 1 Append Source Shapes
  const int anArgCount = myArguments.Size();
  if (anArgCount == 0)
  {
    return;
  }
  myRanges.SetIncrement(anArgCount);

  // Note: this preserves original behavior.
  // Is is questionable if this calculation and call to SetIncrement actually optimizes anything
  // because another container is created in order to count unique shapes.
  // It potentially could be a pessimization, but additional tests are required to confirm this.
  const int aUniqueShapesCount = std::accumulate(
    myArguments.begin(),
    myArguments.end(),
    0,
    [](int aSum, const TopoDS_Shape& aShape) { return aSum + CountUniqueShapes(aShape); });
  myLines.SetIncrement(2 * aUniqueShapesCount);

  //-----------------------------------------------------scope_1 f
  occ::handle<NCollection_BaseAllocator> anAllocator =
    NCollection_BaseAllocator::CommonBaseAllocator();

  int i1 = 0;
  for (NCollection_List<TopoDS_Shape>::Iterator aIt(myArguments); aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    if (myMapShapeIndex.IsBound(aS))
    {
      continue;
    }
    int aI = Append(aS);

    InitShape(aI, aS);

    int i2 = NbShapes() - 1;
    myRanges.Append(BOPDS_IndexRange(i1, i2));
    i1 = i2 + 1;
  }

  const double aTolAdd = std::max(theFuzz, Precision::Confusion()) * 0.5;
  myNbSourceShapes     = NbShapes();

  // 2 Bounding Boxes

  // 2.1 Vertex
  for (int j = 0; j < myNbSourceShapes; ++j)
  {
    BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(j);
    if (aShapeInfo.ShapeType() != TopAbs_VERTEX)
    {
      continue;
    }

    const TopoDS_Vertex& aVertex = TopoDS::Vertex(aShapeInfo.Shape());

    Bnd_Box& aBox = aShapeInfo.ChangeBox();
    aBox.SetGap(BRep_Tool::Tolerance(aVertex) + aTolAdd);
    aBox.Add(BRep_Tool::Pnt(aVertex));
  }

  // 2.2 Edge
  int aNbE = 0;
  for (int j = 0; j < myNbSourceShapes; ++j)
  {
    BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(j);
    if (aShapeInfo.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }

    const TopoDS_Edge& anEdge          = TopoDS::Edge(aShapeInfo.Shape());
    const double       anEdgeTolerance = BRep_Tool::Tolerance(anEdge);

    if (!BRep_Tool::Degenerated(anEdge))
    {
      gp_Pnt aPx;

      NCollection_List<int>& aLI = aShapeInfo.ChangeSubShapes();

      TopoDS_Edge aEx = anEdge;
      aEx.Orientation(TopAbs_FORWARD);
      double                        aCurveStart, aCurveEnd;
      const occ::handle<Geom_Curve> aC3D = BRep_Tool::Curve(aEx, aCurveStart, aCurveEnd);

      if (Precision::IsNegativeInfinite(aCurveStart))
      {
        aC3D->D0(aCurveStart, aPx);
        TopoDS_Vertex aVx;
        BRep_Builder  aBB;
        aBB.MakeVertex(aVx, aPx, anEdgeTolerance);
        aVx.Orientation(TopAbs_FORWARD);

        BOPDS_ShapeInfo aSIx;
        aSIx.SetShape(aVx);
        aSIx.SetShapeType(TopAbs_VERTEX);
        aSIx.SetFlag(1); // infinite flag

        aLI.Append(Append(aSIx));
      }
      if (Precision::IsPositiveInfinite(aCurveEnd))
      {
        aC3D->D0(aCurveEnd, aPx);
        TopoDS_Vertex aVx;
        BRep_Builder  aBB;
        aBB.MakeVertex(aVx, aPx, anEdgeTolerance);
        aVx.Orientation(TopAbs_REVERSED);

        BOPDS_ShapeInfo aSIx;
        aSIx.SetShape(aVx);
        aSIx.SetShapeType(TopAbs_VERTEX);
        aSIx.SetFlag(1); // infinite flag

        aLI.Append(Append(aSIx));
      }
    }
    else
    {
      aShapeInfo.SetFlag(j);
    }

    Bnd_Box& aBox = aShapeInfo.ChangeBox();
    BRepBndLib::Add(anEdge, aBox);

    for (NCollection_List<int>::Iterator aIt1(aShapeInfo.SubShapes()); aIt1.More(); aIt1.Next())
    {
      BOPDS_ShapeInfo& aSIV = ChangeShapeInfo(aIt1.Value());
      Bnd_Box&         aBx  = aSIV.ChangeBox();
      aBox.Add(aBx);
    }
    aBox.SetGap(aBox.GetGap() + aTolAdd);
    ++aNbE;
  }

  // 2.3 Face
  NCollection_Map<int>           aMI(100, anAllocator);
  NCollection_Map<int>::Iterator aItMI;

  int aNbF = 0;
  for (int j = 0; j < myNbSourceShapes; ++j)
  {
    BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(j);
    if (aShapeInfo.ShapeType() != TopAbs_FACE)
    {
      continue;
    }

    const TopoDS_Shape& aFace = aShapeInfo.Shape();

    Bnd_Box& aBox = aShapeInfo.ChangeBox();
    BRepBndLib::Add(aFace, aBox);

    NCollection_List<int>& aLW = aShapeInfo.ChangeSubShapes();
    for (NCollection_List<int>::Iterator aIt1(aLW); aIt1.More(); aIt1.Next())
    {
      BOPDS_ShapeInfo& aSIW = ChangeShapeInfo(aIt1.Value());

      const NCollection_List<int>& aLE = aSIW.SubShapes();
      aIt2.Initialize(aLE);
      for (; aIt2.More(); aIt2.Next())
      {
        const int        nE   = aIt2.Value();
        BOPDS_ShapeInfo& aSIE = ChangeShapeInfo(nE);
        Bnd_Box&         aBx  = aSIE.ChangeBox();
        aBox.Add(aBx);
        aMI.Add(nE);

        const TopoDS_Edge& aE = *(TopoDS_Edge*)(&aSIE.Shape());
        if (BRep_Tool::Degenerated(aE))
        {
          aSIE.SetFlag(j);
        }

        const NCollection_List<int>& aLV = aSIE.SubShapes();
        for (NCollection_List<int>::Iterator aIt3(aLV); aIt3.More(); aIt3.Next())
        {
          aMI.Add(aIt3.Value());
        }
      }
    }

    // pure internal vertices on the face
    for (TopoDS_Iterator aItS(aFace); aItS.More(); aItS.Next())
    {
      const TopoDS_Shape& aSx = aItS.Value();
      if (aSx.ShapeType() == TopAbs_VERTEX)
      {
        aMI.Add(Index(aSx));
      }
    }

    // For a Face: change wires for BRep sub-shapes
    aLW.Clear();
    aItMI.Initialize(aMI);
    for (; aItMI.More(); aItMI.Next())
    {
      aLW.Append(aItMI.Value());
    }
    aMI.Clear();
    aBox.SetGap(aBox.GetGap() + aTolAdd);
    ++aNbF;
  }

  // For the check mode we need to compute the bounding box for solid.
  // Otherwise, it will be computed on the building stage
  bool bCheckMode = (myArguments.Extent() == 1);
  if (bCheckMode)
  {
    // 2.4 Solids
    for (int j = 0; j < myNbSourceShapes; ++j)
    {
      BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(j);
      if (aShapeInfo.ShapeType() != TopAbs_SOLID)
      {
        continue;
      }

      Bnd_Box& aBox = aShapeInfo.ChangeBox();
      BuildBndBoxSolid(j, aBox);
      // update sub-shapes by BRep comprising ones
      aMI.Clear();
      NCollection_List<int>& aLI1 = aShapeInfo.ChangeSubShapes();

      for (NCollection_List<int>::Iterator aIt1(aLI1); aIt1.More(); aIt1.Next())
      {
        BOPDS_ShapeInfo& aSI1 = ChangeShapeInfo(aIt1.Value());
        if (aSI1.ShapeType() != TopAbs_SHELL)
        {
          continue;
        }

        const NCollection_List<int>& aLI2 = aSI1.SubShapes();
        aIt2.Initialize(aLI2);
        for (; aIt2.More(); aIt2.Next())
        {
          const int        n2   = aIt2.Value();
          BOPDS_ShapeInfo& aSI2 = ChangeShapeInfo(n2);
          if (aSI2.ShapeType() != TopAbs_FACE)
          {
            continue;
          }

          aMI.Add(n2);

          const NCollection_List<int>& aLI3 = aSI2.SubShapes();
          for (NCollection_List<int>::Iterator aIt3(aLI3); aIt3.More(); aIt3.Next())
          {
            aMI.Add(aIt3.Value());
          }
        }
      }

      aLI1.Clear();
      aItMI.Initialize(aMI);
      for (; aItMI.More(); aItMI.Next())
      {
        aLI1.Append(aItMI.Value());
      }
      aMI.Clear();
    }
  }

  aMI.Clear();
  //-----------------------------------------------------

  // Prepare Vertex-Edge connection map
  for (int nE = 0; nE < myNbSourceShapes; ++nE)
  {
    BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(nE);
    if (aShapeInfo.ShapeType() != TopAbs_EDGE)
    {
      continue;
    }

    const NCollection_List<int>& aLV = aShapeInfo.SubShapes();
    for (NCollection_List<int>::Iterator aIt1(aLV); aIt1.More(); aIt1.Next())
    {
      const int              nV  = aIt1.Value();
      NCollection_List<int>* pLE = myMapVE.ChangeSeek(nV);
      if (!pLE)
      {
        pLE = myMapVE.Bound(nV, NCollection_List<int>(myAllocator));
        pLE->Append(nE);
      }
      else
      {
        // provide uniqueness of the edges in the list
        for (aIt2.Initialize(*pLE); aIt2.More(); aIt2.Next())
        {
          if (aIt2.Value() == nE)
            break;
        }
        if (!aIt2.More())
          pLE->Append(nE);
      }
    }
  }
  //-----------------------------------------------------scope_1 t
  // 3 myPaveBlocksPool
  // 4. myFaceInfoPool
  myPaveBlocksPool.SetIncrement(aNbE);
  myFaceInfoPool.SetIncrement(aNbF);
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
                                       const bool theFlag) const
{
  const NCollection_List<int>& aSubShapes = ShapeInfo(theIndex1).SubShapes();
  if (theFlag)
  {
    return std::any_of(
      aSubShapes.begin(),
      aSubShapes.end(),
      [this, theIndex2](int theSubShapeIndex) { return HasInterf(theSubShapeIndex, theIndex2); });
  }
  else
  {
    return std::all_of(
      aSubShapes.begin(),
      aSubShapes.end(),
      [this, theIndex2](int theSubShapeIndex) { return HasInterf(theSubShapeIndex, theIndex2); });
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

//
// PaveBlocks
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

  static NCollection_List<occ::handle<BOPDS_PaveBlock>> aStaticEmptyList;
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

void BOPDS_DS::InitPaveBlocks(const int theIndex)
{
  int                             nV = 0;
  TopoDS_Vertex                   aV;
  NCollection_List<int>::Iterator aIt;

  BOPDS_ShapeInfo&  aShapeInfo = ChangeShapeInfo(theIndex);
  const TopoDS_Edge anEdge     = TopoDS::Edge(aShapeInfo.Shape());

  const NCollection_List<int>& aSubShapeIndices = aShapeInfo.SubShapes();
  if (aSubShapeIndices.IsEmpty())
  {
    return;
  }

  occ::handle<BOPDS_PaveBlock> aPaveBlock = new BOPDS_PaveBlock;
  aPaveBlock->SetOriginalEdge(theIndex);

  if (anEdge.Orientation() != TopAbs_INTERNAL)
  {
    aIt.Initialize(aSubShapeIndices);
    for (; aIt.More(); aIt.Next())
    {
      nV = aIt.Value();

      const BOPDS_ShapeInfo& aSIV = ShapeInfo(nV);
      aV                          = *(TopoDS_Vertex*)(&aSIV.Shape());
      const double aT =
        aSIV.HasFlag() ? ComputeParameter(aV, anEdge) : BRep_Tool::Parameter(aV, anEdge);

      nV = GetSameDomainIndex(nV);
      BOPDS_Pave aPave(nV, aT);
      if (aShapeInfo.HasFlag()) // for a degenerated edge append pave unconditionally
        aPaveBlock->AppendExtPave1(aPave);
      else
        aPaveBlock->AppendExtPave(aPave);
    }

    if (aSubShapeIndices.Size() == 1)
    {
      aV.Reverse();
      const double aT = BRep_Tool::Parameter(aV, anEdge);
      aPaveBlock->AppendExtPave1(BOPDS_Pave(nV, aT));
    }
  }
  else
  {
    for (TopoDS_Iterator aItE(anEdge, false, true); aItE.More(); aItE.Next())
    {
      const TopoDS_Vertex&   aVertex        = TopoDS::Vertex(aItE.Value());
      const int              theVertexIndex = Index(aVertex);
      const BOPDS_ShapeInfo& aVertexInfo    = ShapeInfo(theVertexIndex);
      const double aVertexParam = aVertexInfo.HasFlag() ? ComputeParameter(aVertex, anEdge)
                                                        : BRep_Tool::Parameter(aVertex, anEdge);
      aPaveBlock->AppendExtPave1(BOPDS_Pave(GetSameDomainIndex(theVertexIndex), aVertexParam));
    }
  }

  aPaveBlock->Update(myPaveBlocksPool.Appended(), false);
  aShapeInfo.SetReference(myPaveBlocksPool.Length() - 1);
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

  NCollection_List<occ::handle<BOPDS_PaveBlock>>                      aLPBN(myAllocator);
  NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>& aPBP = myPaveBlocksPool;
  const BOPDS_ShapeInfo& aShapeInfo = ChangeShapeInfo(thePaveBlock->OriginalEdge());
  NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aPBP(aShapeInfo.Reference());

  for (NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB(aLPB); aItPB.More();
       aItPB.Next())
  {
    occ::handle<BOPDS_PaveBlock>& aPB = aItPB.ChangeValue();
    if (aPB == thePaveBlock)
    {
      aPB->Update(aLPBN);
      aLPB.Append(aLPBN);
      aLPB.Remove(aItPB);
      break;
    }
  }
}

//=================================================================================================

void BOPDS_DS::UpdateCommonBlock(const occ::handle<BOPDS_CommonBlock>& theCB, const double theFuzz)
{
  int                                                      nE, iRef, n1, n2;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB, aItPBCB, aItPBN;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>           aLPBN;
  NCollection_DataMap<BOPDS_Pair, NCollection_List<occ::handle<BOPDS_PaveBlock>>> aMPKLPB;
  NCollection_DataMap<BOPDS_Pair, NCollection_List<occ::handle<BOPDS_PaveBlock>>>::Iterator
                                 aItMPKLPB;
  occ::handle<BOPDS_PaveBlock>   aPB;
  occ::handle<BOPDS_CommonBlock> aCBx;
  BOPDS_Pair                     aPK;

  const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPBCB = theCB->PaveBlocks();
  if (!aLPBCB.First()->IsToUpdate())
  {
    return;
  }

  const NCollection_List<int>& aLF = theCB->Faces();

  NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>& aPBP = myPaveBlocksPool;

  aItPBCB.Initialize(aLPBCB);
  for (; aItPBCB.More(); aItPBCB.Next())
  {
    const occ::handle<BOPDS_PaveBlock>& aPBCB = aItPBCB.ChangeValue();

    nE                                                   = aPBCB->OriginalEdge();
    iRef                                                 = ChangeShapeInfo(nE).Reference();
    NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aPBP(iRef);

    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next())
    {
      aPB = aItPB.ChangeValue();
      if (aPB == aPBCB)
      {

        aLPBN.Clear();
        aPB->Update(aLPBN);

        aItPBN.Initialize(aLPBN);
        for (; aItPBN.More(); aItPBN.Next())
        {
          occ::handle<BOPDS_PaveBlock>& aPBN = aItPBN.ChangeValue();
          aLPB.Append(aPBN);

          aPBN->Indices(n1, n2);
          aPK.SetIndices(n1, n2);
          if (aMPKLPB.IsBound(aPK))
          {
            NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPBx = aMPKLPB.ChangeFind(aPK);
            aLPBx.Append(aPBN);
          }
          else
          {
            NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBx;
            aLPBx.Append(aPBN);
            aMPKLPB.Bind(aPK, aLPBx);
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

    while (aLPBx.Extent())
    {
      bool                                           bCoinside;
      NCollection_List<occ::handle<BOPDS_PaveBlock>> aLPBxN;

      aItPB.Initialize(aLPBx);
      for (; aItPB.More();)
      {
        const occ::handle<BOPDS_PaveBlock>& aPBx = aItPB.Value();
        if (aLPBxN.Extent())
        {
          const occ::handle<BOPDS_PaveBlock>& aPBCx = aLPBxN.First();
          bCoinside                                 = CheckCoincidence(aPBx, aPBCx, theFuzz);
          if (bCoinside)
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

      aCBx = new BOPDS_CommonBlock;
      aCBx->SetPaveBlocks(aLPBxN);
      aCBx->SetFaces(aLF);

      aItPB.Initialize(aLPBxN);
      for (; aItPB.More(); aItPB.Next())
      {
        aPB = aItPB.ChangeValue();
        SetCommonBlock(aPB, aCBx);
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

//
// FaceInfo
//

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
    static BOPDS_FaceInfo anEmptyFaceInfo;
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
  for (TopoDS_Iterator itS(Shape(theIndex)); itS.More(); itS.Next())
  {
    const TopoDS_Shape& aVertex = itS.Value();
    if (aVertex.ShapeType() == TopAbs_VERTEX)
    {
      int nV = Index(aVertex);
      nV     = GetSameDomainIndex(nV);
      aFaceInfo.ChangeVerticesIn().Add(nV);
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
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBIn = aFaceInfo.ChangePaveBlocksIn();
  NCollection_Map<int>&                                 aMVIn  = aFaceInfo.ChangeVerticesIn();
  aMPBIn.Clear();
  aMVIn.Clear();
  FaceInfoIn(theIndex, aMPBIn, aMVIn);
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
  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBOn = aFaceInfo.ChangePaveBlocksOn();
  NCollection_Map<int>&                                 aMVOn  = aFaceInfo.ChangeVerticesOn();
  aMPBOn.Clear();
  aMVOn.Clear();
  FaceInfoOn(theIndex, aMPBOn, aMVOn);
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
                          NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPB,
                          NCollection_Map<int>&                                 theMI)
{
  int nV;

  // 1. Pure internal vertices on the face
  const TopoDS_Shape& aFace = Shape(theFaceIndex);
  for (TopoDS_Iterator aFaceIter(aFace); aFaceIter.More(); aFaceIter.Next())
  {
    const TopoDS_Shape& aSubShape = aFaceIter.Value();
    if (aSubShape.ShapeType() == TopAbs_VERTEX)
    {
      nV = GetSameDomainIndex(Index(aSubShape));
      theMI.Add(nV);
    }
  }

  // 2. aVFs
  for (const auto& aVertexFaceInterference : InterfVF())
  {
    if (aVertexFaceInterference.Contains(theFaceIndex))
    {
      nV = GetSameDomainIndex(aVertexFaceInterference.OppositeIndex(theFaceIndex));
      theMI.Add(nV);
    }
  }

  // 3. aEFs
  for (const auto& anEdgeFaceInterference : InterfEF())
  {
    if (!anEdgeFaceInterference.Contains(theFaceIndex))
    {
      continue;
    }

    if (anEdgeFaceInterference.HasIndexNew(nV))
    {
      nV = GetSameDomainIndex(nV);
      theMI.Add(nV);
    }
    else
    {
      for (const auto& aPaveBlock : PaveBlocks(anEdgeFaceInterference.OppositeIndex(theFaceIndex)))
      {
        const occ::handle<BOPDS_CommonBlock> aCommonBlock = CommonBlock(aPaveBlock);
        if (aCommonBlock && aCommonBlock->Contains(theFaceIndex))
        {
          theMPB.Add(aCommonBlock->PaveBlock1());
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
    if (int nVNew; anEdgeFaceInterference.HasIndexNew(nVNew))
    {
      nVNew = GetSameDomainIndex(nVNew);
      aFaceInfo.ChangeVerticesIn().Add(nVNew);
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

void BOPDS_DS::SubShapesOnIn(const int                                             theNF1,
                             const int                                             theNF2,
                             NCollection_Map<int>&                                 theMVOnIn,
                             NCollection_Map<int>&                                 theMVCommon,
                             NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& thePBOnIn,
                             NCollection_Map<occ::handle<BOPDS_PaveBlock>>& theCommonPB) const
{

  const BOPDS_FaceInfo& aFI1 = FaceInfo(theNF1);
  const BOPDS_FaceInfo& aFI2 = FaceInfo(theNF2);

  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>> pMPB[4];
  pMPB[0] = aFI1.PaveBlocksOn();
  pMPB[1] = aFI1.PaveBlocksIn();
  pMPB[2] = aFI2.PaveBlocksOn();
  pMPB[3] = aFI2.PaveBlocksIn();

  for (int i = 0; i < 4; ++i)
  {
    for (int j = 1; j <= pMPB[i].Size(); ++j)
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = pMPB[i](j);
      thePBOnIn.Add(aPB);
      int nV1, nV2;
      aPB->Indices(nV1, nV2);

      theMVOnIn.Add(nV1);
      theMVOnIn.Add(nV2);

      if (i < 2)
      {
        if (pMPB[2].Contains(aPB) || pMPB[3].Contains(aPB))
        {
          theCommonPB.Add(aPB);
          theMVCommon.Add(nV1);
          theMVCommon.Add(nV2);
        }
      }
    }
  }

  const NCollection_Map<int>& aMVOn1 = aFI1.VerticesOn();
  const NCollection_Map<int>& aMVIn1 = aFI1.VerticesIn();
  const NCollection_Map<int>& aMVOn2 = aFI2.VerticesOn();
  const NCollection_Map<int>& aMVIn2 = aFI2.VerticesIn();

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

void BOPDS_DS::SharedEdges(const int                                     nF1,
                           const int                                     nF2,
                           NCollection_List<int>&                        theLI,
                           const occ::handle<NCollection_BaseAllocator>& aAllocator)
{
  int                                                      nE, nSp;
  NCollection_List<int>::Iterator                          aItLI;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItLPB;
  NCollection_Map<int>                                     aMI(100, aAllocator);
  //
  const BOPDS_ShapeInfo&       aSI1 = ShapeInfo(nF1);
  const NCollection_List<int>& aLI1 = aSI1.SubShapes();
  aItLI.Initialize(aLI1);
  for (; aItLI.More(); aItLI.Next())
  {
    nE                          = aItLI.Value();
    const BOPDS_ShapeInfo& aSIE = ChangeShapeInfo(nE);
    if (aSIE.ShapeType() == TopAbs_EDGE)
    {
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = PaveBlocks(nE);
      if (aLPB.IsEmpty())
      {
        aMI.Add(nE);
      }
      else
      {
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More(); aItLPB.Next())
        {
          const occ::handle<BOPDS_PaveBlock> aPB = RealPaveBlock(aItLPB.Value());
          nSp                                    = aPB->Edge();
          aMI.Add(nSp);
        }
      }
    }
  }
  //
  const BOPDS_ShapeInfo&       aSI2 = ShapeInfo(nF2);
  const NCollection_List<int>& aLI2 = aSI2.SubShapes();
  aItLI.Initialize(aLI2);
  for (; aItLI.More(); aItLI.Next())
  {
    nE                          = aItLI.Value();
    const BOPDS_ShapeInfo& aSIE = ChangeShapeInfo(nE);
    if (aSIE.ShapeType() == TopAbs_EDGE)
    {
      const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = PaveBlocks(nE);
      if (aLPB.IsEmpty())
      {
        if (aMI.Contains(nE))
        {
          theLI.Append(nE);
        }
      }
      else
      {
        aItLPB.Initialize(aLPB);
        for (; aItLPB.More(); aItLPB.Next())
        {
          const occ::handle<BOPDS_PaveBlock> aPB = RealPaveBlock(aItLPB.Value());
          nSp                                    = aPB->Edge();
          if (aMI.Contains(nSp))
          {
            theLI.Append(nSp);
          }
        }
      }
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// same domain shapes
//
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

bool BOPDS_DS::CheckCoincidence(const occ::handle<BOPDS_PaveBlock>& aPB1,
                                const occ::handle<BOPDS_PaveBlock>& aPB2,
                                const double                        theFuzz)
{
  bool   bRet;
  int    nE1, nE2, aNbPoints;
  double aT11, aT12, aT21, aT22, aT1m, aD, aTol, aT2x;
  gp_Pnt aP1m;
  //
  bRet = false;
  //
  aPB1->Range(aT11, aT12);
  aT1m                   = IntTools_Tools::IntermediatePoint(aT11, aT12);
  nE1                    = aPB1->OriginalEdge();
  const TopoDS_Edge& aE1 = (*(TopoDS_Edge*)(&Shape(nE1)));
  BOPTools_AlgoTools::PointOnEdge(aE1, aT1m, aP1m);
  //
  aPB2->Range(aT21, aT22);
  nE2                    = aPB2->OriginalEdge();
  const TopoDS_Edge& aE2 = (*(TopoDS_Edge*)(&Shape(nE2)));
  //
  double                      f, l;
  occ::handle<Geom_Curve>     aC2 = BRep_Tool::Curve(aE2, f, l);
  GeomAPI_ProjectPointOnCurve aPPC;
  aPPC.Init(aC2, f, l);
  aPPC.Perform(aP1m);
  aNbPoints = aPPC.NbPoints();
  if (aNbPoints)
  {
    aD = aPPC.LowerDistance();
    //
    aTol = BRep_Tool::MaxTolerance(aE1, TopAbs_VERTEX);
    aTol = aTol + BRep_Tool::MaxTolerance(aE2, TopAbs_VERTEX)
           + std::max(theFuzz, Precision::Confusion());
    if (aD < aTol)
    {
      aT2x = aPPC.LowerDistanceParameter();
      if (aT2x > aT21 && aT2x < aT22)
      {
        return !bRet;
      }
    }
  }
  return bRet;
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

void BOPDS_DS::Paves(const int theEdge, NCollection_List<BOPDS_Pave>& theLP)
{
  int                                                      aNb, i;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aIt;
  NCollection_Map<BOPDS_Pave>                              aMP;
  //
  const NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = PaveBlocks(theEdge);
  aNb                                                        = aLPB.Extent() + 1;
  if (aNb == 1)
  {
    return;
  }
  //
  NCollection_Array1<BOPDS_Pave> pPaves(1, aNb);
  //
  i = 1;
  aIt.Initialize(aLPB);
  for (; aIt.More(); aIt.Next())
  {
    const occ::handle<BOPDS_PaveBlock>& aPB    = aIt.Value();
    const BOPDS_Pave&                   aPave1 = aPB->Pave1();
    const BOPDS_Pave&                   aPave2 = aPB->Pave2();
    //
    if (aMP.Add(aPave1))
    {
      pPaves(i) = aPave1;
      ++i;
    }
    //
    if (aMP.Add(aPave2))
    {
      pPaves(i) = aPave2;
      ++i;
    }
  }
  //
  Standard_ASSERT_VOID(aNb == aMP.Extent(), "Abnormal number of paves");
  //
  std::sort(pPaves.begin(), pPaves.end());
  //
  for (i = 1; i <= aNb; ++i)
  {
    theLP.Append(pPaves(i));
  }
}

//=================================================================================================

void BOPDS_DS::BuildBndBoxSolid(const int theIndex, Bnd_Box& aBoxS, const bool theCheckInverted)
{
  bool                            bIsOpenBox, bIsInverted;
  int                             nSh, nFc;
  double                          aTolS, aTolFc;
  NCollection_List<int>::Iterator aItLI, aItLI1;
  //
  const BOPDS_ShapeInfo& aSI    = ShapeInfo(theIndex);
  const TopoDS_Shape&    aS     = aSI.Shape();
  const TopoDS_Solid&    aSolid = (*(TopoDS_Solid*)(&aS));
  //
  bIsOpenBox = false;
  //
  aTolS                              = 0.;
  const NCollection_List<int>& aLISh = aSI.SubShapes();
  aItLI.Initialize(aLISh);
  for (; aItLI.More(); aItLI.Next())
  {
    nSh                          = aItLI.Value();
    const BOPDS_ShapeInfo& aSISh = ShapeInfo(nSh);
    if (aSISh.ShapeType() != TopAbs_SHELL)
    {
      continue;
    }
    //
    const NCollection_List<int>& aLIFc = aSISh.SubShapes();
    aItLI1.Initialize(aLIFc);
    for (; aItLI1.More(); aItLI1.Next())
    {
      nFc                          = aItLI1.Value();
      const BOPDS_ShapeInfo& aSIFc = ShapeInfo(nFc);
      if (aSIFc.ShapeType() != TopAbs_FACE)
      {
        continue;
      }
      //
      const Bnd_Box& aBFc = aSIFc.Box();
      aBoxS.Add(aBFc);
      //
      if (!bIsOpenBox)
      {
        bIsOpenBox = (aBFc.IsOpenXmin() || aBFc.IsOpenXmax() || aBFc.IsOpenYmin()
                      || aBFc.IsOpenYmax() || aBFc.IsOpenZmin() || aBFc.IsOpenZmax());
        if (bIsOpenBox)
        {
          break;
        }
      }
      //
      const TopoDS_Face& aFc = *((TopoDS_Face*)&aSIFc.Shape());
      aTolFc                 = BRep_Tool::Tolerance(aFc);
      if (aTolFc > aTolS)
      {
        aTolS = aTolFc;
      }
    } // for (; aItLI1.More(); aItLI1.Next()) {
    if (bIsOpenBox)
    {
      break;
    }
    //
    const TopoDS_Shell& aSh = *((TopoDS_Shell*)&aSISh.Shape());
    bIsOpenBox              = BOPTools_AlgoTools::IsOpenShell(aSh);
    if (bIsOpenBox)
    {
      break;
    }
  } // for (; aItLI.More(); aItLI.Next()) {
  //
  if (bIsOpenBox)
  {
    aBoxS.SetWhole();
  }
  else if (theCheckInverted)
  {
    bIsInverted = BOPTools_AlgoTools::IsInvertedSolid(aSolid);
    if (bIsInverted)
    {
      aBoxS.SetWhole();
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
  BOPDS_Pave aPave1 = thePaveBlock->Pave1();
  int        nV1    = aPave1.Index();
  if (HasShapeSD(nV1, nV1))
  {
    aPave1.SetIndex(nV1);
    thePaveBlock->SetPave1(aPave1);
  }

  BOPDS_Pave aPave2 = thePaveBlock->Pave2();
  int        nV2    = aPave2.Index();
  if (HasShapeSD(nV2, nV2))
  {
    aPave2.SetIndex(nV2);
    thePaveBlock->SetPave2(aPave2);
  }
}

//=================================================================================================

void BOPDS_DS::UpdateCommonBlockWithSDVertices(const occ::handle<BOPDS_CommonBlock>& theCB)
{
  for (const auto& aPaveBlock : theCB->PaveBlocks())
  {
    UpdatePaveBlockWithSDVertices(aPaveBlock);
  }
}

//=================================================================================================

void BOPDS_DS::InitPaveBlocksForVertex(const int theNV)
{
  const NCollection_List<int>* anEdgeIndices = myMapVE.Seek(theNV);
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

  NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>& aPBP = ChangePaveBlocksPool();
  int                                                                 aNbPBP = aPBP.Length();
  if (!aNbPBP)
  {
    return;
  }
  //
  for (int i = 0; i < aNbPBP; ++i)
  {
    NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB = aPBP(i);
    if (aLPB.Extent() == 1)
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aLPB.First();
      if (!IsCommonBlock(aPB))
      {
        int nV1, nV2;
        aPB->Indices(nV1, nV2);
        if (!IsNewShape(nV1) && !IsNewShape(nV2))
        {
          // Both vertices are original, thus the PB is untouched.
          // Remove reference for the original edge
          int nE = aPB->OriginalEdge();
          if (nE >= 0)
          {
            ChangeShapeInfo(nE).SetReference(-1);
          }
          // Clear contents of the list
          aLPB.Clear();
        }
      }
    }
  }
}

//=================================================================================================

bool BOPDS_DS::IsValidShrunkData(const occ::handle<BOPDS_PaveBlock>& thePB)
{
  if (!thePB->HasShrunkData())
    return false;

  // Compare the distances from the bounds of the shrunk range to the vertices
  // with the tolerance values of vertices

  // Shrunk range
  double  aTS[2];
  Bnd_Box aBox;
  bool    bIsSplit;
  //
  thePB->ShrunkData(aTS[0], aTS[1], aBox, bIsSplit);
  //
  // Vertices
  int nV[2];
  thePB->Indices(nV[0], nV[1]);
  //
  const TopoDS_Edge& aE = TopoDS::Edge(Shape(thePB->OriginalEdge()));
  BRepAdaptor_Curve  aBAC(aE);
  //
  double anEps = BRep_Tool::Tolerance(aE) * 0.01;
  //
  for (int i = 0; i < 2; ++i)
  {
    const TopoDS_Vertex& aV   = TopoDS::Vertex(Shape(nV[i]));
    double               aTol = BRep_Tool::Tolerance(aV) + Precision::Confusion();
    // Bounding point
    gp_Pnt aP = BRep_Tool::Pnt(aV);
    //
    // Point on the end of shrunk range
    gp_Pnt aPS = aBAC.Value(aTS[i]);
    //
    double aDist = aP.Distance(aPS);
    if (aTol - aDist > anEps)
    {
      return false;
    }
  }
  return true;
}