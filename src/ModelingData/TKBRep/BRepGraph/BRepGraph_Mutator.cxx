// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepGraph_Mutator.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <Standard_Assert.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>

namespace
{

const char* kindName(const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return "Vertices";
    case BRepGraph_NodeId::Kind::Edge:
      return "Edges";
    case BRepGraph_NodeId::Kind::CoEdge:
      return "CoEdges";
    case BRepGraph_NodeId::Kind::Wire:
      return "Wires";
    case BRepGraph_NodeId::Kind::Face:
      return "Faces";
    case BRepGraph_NodeId::Kind::Shell:
      return "Shells";
    case BRepGraph_NodeId::Kind::Solid:
      return "Solids";
    case BRepGraph_NodeId::Kind::Compound:
      return "Compounds";
    case BRepGraph_NodeId::Kind::CompSolid:
      return "CompSolids";
    case BRepGraph_NodeId::Kind::Product:
      return "Products";
    case BRepGraph_NodeId::Kind::Occurrence:
      return "Occurrences";
  }
  return "Unknown";
}

int countActiveByKind(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId::Kind theKind)
{
  int aCount = 0;
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      for (int i = 0; i < theStorage.NbVertices(); ++i)
        if (!theStorage.Vertex(BRepGraph_VertexId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Edge:
      for (int i = 0; i < theStorage.NbEdges(); ++i)
        if (!theStorage.Edge(BRepGraph_EdgeId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::CoEdge:
      for (int i = 0; i < theStorage.NbCoEdges(); ++i)
        if (!theStorage.CoEdge(BRepGraph_CoEdgeId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Wire:
      for (int i = 0; i < theStorage.NbWires(); ++i)
        if (!theStorage.Wire(BRepGraph_WireId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Face:
      for (int i = 0; i < theStorage.NbFaces(); ++i)
        if (!theStorage.Face(BRepGraph_FaceId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Shell:
      for (int i = 0; i < theStorage.NbShells(); ++i)
        if (!theStorage.Shell(BRepGraph_ShellId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Solid:
      for (int i = 0; i < theStorage.NbSolids(); ++i)
        if (!theStorage.Solid(BRepGraph_SolidId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Compound:
      for (int i = 0; i < theStorage.NbCompounds(); ++i)
        if (!theStorage.Compound(BRepGraph_CompoundId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::CompSolid:
      for (int i = 0; i < theStorage.NbCompSolids(); ++i)
        if (!theStorage.CompSolid(BRepGraph_CompSolidId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Product:
      for (int i = 0; i < theStorage.NbProducts(); ++i)
        if (!theStorage.Product(BRepGraph_ProductId(i)).IsRemoved)
          ++aCount;
      return aCount;
    case BRepGraph_NodeId::Kind::Occurrence:
      for (int i = 0; i < theStorage.NbOccurrences(); ++i)
        if (!theStorage.Occurrence(BRepGraph_OccurrenceId(i)).IsRemoved)
          ++aCount;
      return aCount;
  }
  return 0;
}

int cachedActiveByKind(const BRepGraphInc_Storage& theStorage, const BRepGraph_NodeId::Kind theKind)
{
  switch (theKind)
  {
    case BRepGraph_NodeId::Kind::Vertex:
      return theStorage.NbActiveVertices();
    case BRepGraph_NodeId::Kind::Edge:
      return theStorage.NbActiveEdges();
    case BRepGraph_NodeId::Kind::CoEdge:
      return theStorage.NbActiveCoEdges();
    case BRepGraph_NodeId::Kind::Wire:
      return theStorage.NbActiveWires();
    case BRepGraph_NodeId::Kind::Face:
      return theStorage.NbActiveFaces();
    case BRepGraph_NodeId::Kind::Shell:
      return theStorage.NbActiveShells();
    case BRepGraph_NodeId::Kind::Solid:
      return theStorage.NbActiveSolids();
    case BRepGraph_NodeId::Kind::Compound:
      return theStorage.NbActiveCompounds();
    case BRepGraph_NodeId::Kind::CompSolid:
      return theStorage.NbActiveCompSolids();
    case BRepGraph_NodeId::Kind::Product:
      return theStorage.NbActiveProducts();
    case BRepGraph_NodeId::Kind::Occurrence:
      return theStorage.NbActiveOccurrences();
  }
  return 0;
}

const NCollection_Vector<BRepGraph_CoEdgeRefId>& wireCoEdgeRefIds(
  const BRepGraphInc_Storage& theStorage,
  const BRepGraph_WireId      theWireId)
{
  return theStorage.Wire(theWireId).CoEdgeRefIds;
}

//! Initialize a sub-edge definition produced by SplitEdge.
//! Copies shared properties from the original edge and assigns boundary vertex ref ids.
//! Vertex ref entries must already exist in storage; only their RefId indices are passed.
void initSubEdgeEntity(BRepGraphInc::EdgeDef&       theSub,
                    const BRepGraph_Curve3DRepId    theCurve3DRepId,
                    const double                    theTolerance,
                    const bool                      theSameParameter,
                    const BRepGraph_VertexRefId     theStartVertexRefId,
                    const BRepGraph_VertexRefId     theEndVertexRefId,
                    const double                    theParamFirst,
                    const double                    theParamLast)
{
  theSub.Curve3DRepId      = theCurve3DRepId;
  theSub.Tolerance         = theTolerance;
  theSub.SameParameter     = theSameParameter;
  theSub.SameRange         = false;
  theSub.IsDegenerate      = false;
  theSub.StartVertexRefId  = theStartVertexRefId;
  theSub.EndVertexRefId    = theEndVertexRefId;
  theSub.ParamFirst        = theParamFirst;
  theSub.ParamLast         = theParamLast;
}

//! Initialize a sub-CoEdge definition produced by SplitEdge.
void initSubCoEdgeEntity(BRepGraphInc::CoEdgeDef&  theCE,
                      const BRepGraph_EdgeId       theEdgeId,
                      const BRepGraph_FaceId       theFaceId,
                      const TopAbs_Orientation     theSense,
                      const BRepGraph_Curve2DRepId theCurve2DRepId,
                      const double                 theParamFirst,
                      const double                 theParamLast,
                      const GeomAbs_Shape          theContinuity)
{
  theCE.EdgeEntityId    = theEdgeId;
  theCE.FaceEntityId    = theFaceId;
  theCE.Sense        = theSense;
  theCE.Curve2DRepId = theCurve2DRepId;
  theCE.ParamFirst   = theParamFirst;
  theCE.ParamLast    = theParamLast;
  theCE.Continuity   = theContinuity;
}

} // namespace

//=================================================================================================

void BRepGraph_Mutator::applyModificationImpl(BRepGraph&                            theGraph,
                                              const BRepGraph_NodeId                theTarget,
                                              NCollection_Vector<BRepGraph_NodeId>&& theReplacements,
                                              const TCollection_AsciiString&        theOpLabel)
{
  theGraph.myData->myHistoryLog.Record(theOpLabel, theTarget, theReplacements);
  theGraph.invalidateSubgraphImpl(theTarget);
}

//=================================================================================================

void BRepGraph_Mutator::SplitEdge(BRepGraph&             theGraph,
                                  const BRepGraph_NodeId theEdgeEntity,
                                  const BRepGraph_NodeId theSplitVertex,
                                  const double           theSplitParam,
                                  BRepGraph_NodeId&      theSubA,
                                  BRepGraph_NodeId&      theSubB)
{
  // Copy all data from the original EdgeDef before appending to vectors (which may reallocate).
  const BRepGraphInc::EdgeDef& anOrig =
    theGraph.myData->myIncStorage.Edge(BRepGraph_EdgeId::FromNodeId(theEdgeEntity));

  const BRepGraphInc_Storage&   aConstStorage         = theGraph.myData->myIncStorage;
  const BRepGraph_Curve3DRepId  aOrigCurve3DRepId     = anOrig.Curve3DRepId;
  const double                  aOrigTolerance        = anOrig.Tolerance;
  const bool                    aOrigSameParameter    = anOrig.SameParameter;
  const double                  aOrigParamFirst       = anOrig.ParamFirst;
  const double                  aOrigParamLast        = anOrig.ParamLast;
  const BRepGraph_VertexRefId   aOrigStartVertexRefId = anOrig.StartVertexRefId;
  const BRepGraph_VertexRefId   aOrigEndVertexRefId   = anOrig.EndVertexRefId;
  const bool                    aOrigSameRange        = anOrig.SameRange;

  // Resolve original vertex def ids through storage ref entries.
  const BRepGraph_VertexId aOrigStartVertexEntityId =
    aOrigStartVertexRefId.IsValid()
      ? aConstStorage.VertexRefEntry(aOrigStartVertexRefId).VertexEntityId
      : BRepGraph_VertexId();
  const BRepGraph_VertexId aOrigEndVertexEntityId =
    aOrigEndVertexRefId.IsValid()
      ? aConstStorage.VertexRefEntry(aOrigEndVertexRefId).VertexEntityId
      : BRepGraph_VertexId();

  // Copy wire indices: ReverseIdx may be rebuilt below.
  const NCollection_Vector<BRepGraph_WireId>* aOrigWiresPtr =
    theGraph.myData->myIncStorage.ReverseIndex().WiresOfEdge(BRepGraph_EdgeId(theEdgeEntity.Index));
  const NCollection_Vector<BRepGraph_WireId> aOrigWires =
    aOrigWiresPtr != nullptr ? *aOrigWiresPtr : NCollection_Vector<BRepGraph_WireId>();

  // Allocate SubA slot.
  BRepGraphInc::EdgeDef& aSubADef = theGraph.myData->myIncStorage.AppendEdge();
  const int                    aSubAIdx = theGraph.myData->myIncStorage.NbEdges() - 1;
  aSubADef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
  theSubA                               = aSubADef.Id;

  // Allocate SubB slot (note: Appended() may invalidate aSubADef reference - use index).
  BRepGraphInc::EdgeDef& aSubBDef = theGraph.myData->myIncStorage.AppendEdge();
  const int                    aSubBIdx = theGraph.myData->myIncStorage.NbEdges() - 1;
  aSubBDef.Id                           = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
  theSubB                               = aSubBDef.Id;

  // Build vertex ref entries for the split vertex (no Location since split vertex is new).
  const BRepGraph_VertexId aSplitVertexEntityId = theSplitVertex.IsValid()
                                                 ? BRepGraph_VertexId::FromNodeId(theSplitVertex)
                                                 : BRepGraph_VertexId();

  // Create start vertex ref entry for SubA (copy from original edge's start vertex ref).
  BRepGraph_VertexRefId aSubAStartRefId;
  if (aOrigStartVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRefEntry& aOrigStartRef =
      theGraph.myData->myIncStorage.VertexRefEntry(aOrigStartVertexRefId);
    const BRepGraph_VertexId aOrigStartVertexId = aOrigStartRef.VertexEntityId;
    const TopAbs_Orientation aOrigStartOri      = aOrigStartRef.Orientation;
    const TopLoc_Location    aOrigStartLoc      = aOrigStartRef.LocalLocation;

    BRepGraphInc::VertexRefEntry& aSubAStartRef =
      theGraph.myData->myIncStorage.AppendVertexRefEntry();
    const int aSubAStartRefIdx          = theGraph.myData->myIncStorage.NbVertexRefs() - 1;
    aSubAStartRef.RefId                 = BRepGraph_RefId::Vertex(aSubAStartRefIdx);
    aSubAStartRef.ParentId              = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
    aSubAStartRef.VertexEntityId           = aOrigStartVertexId;
    aSubAStartRef.Orientation           = aOrigStartOri;
    aSubAStartRef.LocalLocation         = aOrigStartLoc;
    theGraph.allocateRefUID(aSubAStartRef.RefId);
    aSubAStartRefId = BRepGraph_VertexRefId(aSubAStartRefIdx);
  }

  // Create end vertex ref entry for SubA (split vertex, REVERSED).
  BRepGraph_VertexRefId aSubAEndRefId;
  if (aSplitVertexEntityId.IsValid())
  {
    BRepGraphInc::VertexRefEntry& aSubAEndRef =
      theGraph.myData->myIncStorage.AppendVertexRefEntry();
    const int aSubAEndRefIdx          = theGraph.myData->myIncStorage.NbVertexRefs() - 1;
    aSubAEndRef.RefId                 = BRepGraph_RefId::Vertex(aSubAEndRefIdx);
    aSubAEndRef.ParentId              = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubAIdx);
    aSubAEndRef.VertexEntityId           = aSplitVertexEntityId;
    aSubAEndRef.Orientation           = TopAbs_REVERSED;
    theGraph.allocateRefUID(aSubAEndRef.RefId);
    aSubAEndRefId = BRepGraph_VertexRefId(aSubAEndRefIdx);
  }

  // Create start vertex ref entry for SubB (split vertex, FORWARD).
  BRepGraph_VertexRefId aSubBStartRefId;
  if (aSplitVertexEntityId.IsValid())
  {
    BRepGraphInc::VertexRefEntry& aSubBStartRef =
      theGraph.myData->myIncStorage.AppendVertexRefEntry();
    const int aSubBStartRefIdx          = theGraph.myData->myIncStorage.NbVertexRefs() - 1;
    aSubBStartRef.RefId                 = BRepGraph_RefId::Vertex(aSubBStartRefIdx);
    aSubBStartRef.ParentId              = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
    aSubBStartRef.VertexEntityId           = aSplitVertexEntityId;
    aSubBStartRef.Orientation           = TopAbs_FORWARD;
    theGraph.allocateRefUID(aSubBStartRef.RefId);
    aSubBStartRefId = BRepGraph_VertexRefId(aSubBStartRefIdx);
  }

  // Create end vertex ref entry for SubB (copy from original edge's end vertex ref).
  BRepGraph_VertexRefId aSubBEndRefId;
  if (aOrigEndVertexRefId.IsValid())
  {
    // Copy fields before append (which may reallocate and invalidate references).
    const BRepGraphInc::VertexRefEntry& aOrigEndRef =
      theGraph.myData->myIncStorage.VertexRefEntry(aOrigEndVertexRefId);
    const BRepGraph_VertexId aOrigEndVertexId = aOrigEndRef.VertexEntityId;
    const TopAbs_Orientation aOrigEndOri      = aOrigEndRef.Orientation;
    const TopLoc_Location    aOrigEndLoc      = aOrigEndRef.LocalLocation;

    BRepGraphInc::VertexRefEntry& aSubBEndRef =
      theGraph.myData->myIncStorage.AppendVertexRefEntry();
    const int aSubBEndRefIdx          = theGraph.myData->myIncStorage.NbVertexRefs() - 1;
    aSubBEndRef.RefId                 = BRepGraph_RefId::Vertex(aSubBEndRefIdx);
    aSubBEndRef.ParentId              = BRepGraph_NodeId(BRepGraph_NodeId::Kind::Edge, aSubBIdx);
    aSubBEndRef.VertexEntityId           = aOrigEndVertexId;
    aSubBEndRef.Orientation           = aOrigEndOri;
    aSubBEndRef.LocalLocation         = aOrigEndLoc;
    theGraph.allocateRefUID(aSubBEndRef.RefId);
    aSubBEndRefId = BRepGraph_VertexRefId(aSubBEndRefIdx);
  }

  // Set SubA: StartVertex -> SplitVertex, [ParamFirst, theSplitParam].
  {
    BRepGraphInc::EdgeDef& aSubA =
      theGraph.myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(aSubAIdx));
    initSubEdgeEntity(aSubA,
                   aOrigCurve3DRepId,
                   aOrigTolerance,
                   aOrigSameParameter,
                   aSubAStartRefId,
                   aSubAEndRefId,
                   aOrigParamFirst,
                   theSplitParam);
  }

  // Set SubB: SplitVertex -> EndVertex, [theSplitParam, ParamLast].
  {
    BRepGraphInc::EdgeDef& aSubB =
      theGraph.myData->myIncStorage.ChangeEdge(BRepGraph_EdgeId(aSubBIdx));
    initSubEdgeEntity(aSubB,
                   aOrigCurve3DRepId,
                   aOrigTolerance,
                   aOrigSameParameter,
                   aSubBStartRefId,
                   aSubBEndRefId,
                   theSplitParam,
                   aOrigParamLast);
  }

  theGraph.allocateUID(theSubA);
  theGraph.allocateUID(theSubB);

  // Update incidence: wire CoEdgeRefEntry rows and wire CoEdgeRefIds.
  {
    BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

    // Replace original edge's coedge with SubA+SubB coedges for each containing wire.
    const NCollection_Vector<BRepGraph_WireId>* aWireIndices =
      aStorage.ReverseIndex().WiresOfEdge(BRepGraph_EdgeId(theEdgeEntity.Index));
    if (aWireIndices != nullptr)
    {
      for (int aWIdx = 0; aWIdx < aWireIndices->Length(); ++aWIdx)
      {
        const int aWireIdx = aWireIndices->Value(aWIdx).Index;
        if (aWireIdx < 0 || aWireIdx >= aStorage.NbWires())
          continue;
        const NCollection_Vector<BRepGraph_CoEdgeRefId>& aWireRefIds =
          wireCoEdgeRefIds(aStorage, BRepGraph_WireId(aWireIdx));
        for (int aRefOrd = 0; aRefOrd < aWireRefIds.Length(); ++aRefOrd)
        {
          const BRepGraph_CoEdgeRefId       aRefId      = aWireRefIds.Value(aRefOrd);
          const BRepGraphInc::CoEdgeRefEntry& aRefEntry = aStorage.CoEdgeRefEntry(aRefId);
          if (aRefEntry.IsRemoved)
            continue;
          const int                         aOldCoEdgeIdx = aRefEntry.CoEdgeEntityId.Index;
          if (aOldCoEdgeIdx < 0 || aOldCoEdgeIdx >= aStorage.NbCoEdges())
            continue;

          BRepGraphInc::CoEdgeDef& aOldCoEdge =
            aStorage.ChangeCoEdge(BRepGraph_CoEdgeId(aOldCoEdgeIdx));
          if (aOldCoEdge.EdgeEntityId == theEdgeEntity)
          {
            const TopAbs_Orientation aOrigOri = aOldCoEdge.Sense;
            const BRepGraph_FaceId   aFaceDef = aOldCoEdge.FaceEntityId;
            const TopLoc_Location    aRefLoc  = aRefEntry.LocalLocation;

            // Replace in-place: update existing coedge to point to SubA.
            aOldCoEdge.EdgeEntityId = BRepGraph_EdgeId(aSubAIdx);

            // Create a new coedge for SubB and insert after SubA.
            BRepGraphInc::CoEdgeDef& aSubBCoEdge = aStorage.AppendCoEdge();
            const int                   aSubBCoEdgeIdx = aStorage.NbCoEdges() - 1;
            aSubBCoEdge.Id                          = BRepGraph_NodeId::CoEdge(aSubBCoEdgeIdx);
            aSubBCoEdge.EdgeEntityId                   = BRepGraph_EdgeId(aSubBIdx);
            aSubBCoEdge.Sense                       = aOrigOri;
            aSubBCoEdge.FaceEntityId                   = aFaceDef;
            theGraph.allocateUID(aSubBCoEdge.Id);

            // Append ref-entry row for the new coedge under this wire (append-only RefId policy).
            BRepGraphInc::CoEdgeRefEntry& aSubBRefEntry = aStorage.AppendCoEdgeRefEntry();
            const int                     aSubBRefIdx   = aStorage.NbCoEdgeRefs() - 1;
            aSubBRefEntry.RefId                         = BRepGraph_RefId::CoEdge(aSubBRefIdx);
            aSubBRefEntry.ParentId                      = BRepGraph_NodeId::Wire(aWireIdx);
            aSubBRefEntry.CoEdgeEntityId                   = BRepGraph_CoEdgeId(aSubBCoEdgeIdx);
            aSubBRefEntry.LocalLocation                 = aRefLoc;
            theGraph.allocateRefUID(aSubBRefEntry.RefId);

            // Add new CoEdgeRefId to wire entity's RefId vector right after the
            // replaced slot to preserve coedge walk order.
            BRepGraphInc::WireDef& aWireEnt = aStorage.ChangeWire(BRepGraph_WireId(aWireIdx));
            if (aRefOrd >= 0 && aRefOrd < aWireEnt.CoEdgeRefIds.Length())
            {
              aWireEnt.CoEdgeRefIds.InsertAfter(aRefOrd, BRepGraph_CoEdgeRefId(aSubBRefIdx));
            }
            else
            {
              aWireEnt.CoEdgeRefIds.Append(BRepGraph_CoEdgeRefId(aSubBRefIdx));
            }

            // Maintain coedge->wire reverse index for incremental queries.
            aStorage.ChangeReverseIndex().BindCoEdgeToWire(BRepGraph_CoEdgeId(aSubBCoEdgeIdx),
                                                           BRepGraph_WireId(aWireIdx));
            break;
          }
        }
      }
    }

    // Mark original edge as removed in incidence.
    theGraph.myData->myIncStorage.MarkRemoved(theEdgeEntity);
  }

  // Split PCurve entries for each CoEdge referencing the original edge.
  // Copy CoEdge data before mutation (vector may reallocate).
  NCollection_Vector<BRepGraphInc::CoEdgeDef> aOrigCoEdges;
  {
    const NCollection_Vector<BRepGraph_CoEdgeId>* aCoEdgeIdxs =
      theGraph.myData->myIncStorage.ReverseIndex().CoEdgesOfEdge(
        BRepGraph_EdgeId(theEdgeEntity.Index));
    if (aCoEdgeIdxs != nullptr)
    {
      for (int i = 0; i < aCoEdgeIdxs->Length(); ++i)
        aOrigCoEdges.Append(theGraph.myData->myIncStorage.CoEdge(aCoEdgeIdxs->Value(i)));
    }
  }

  const double aParamRange = aOrigParamLast - aOrigParamFirst;
  for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
  {
    const BRepGraphInc::CoEdgeDef& aCE = aOrigCoEdges.Value(aCEIdx);

    // Compute 2D split parameter.
    double aPCSplit;
    if (aOrigSameRange)
    {
      aPCSplit = theSplitParam;
    }
    else
    {
      const double aPCRange = aCE.ParamLast - aCE.ParamFirst;
      if (aParamRange > 0.0)
        aPCSplit = aCE.ParamFirst + ((theSplitParam - aOrigParamFirst) / aParamRange) * aPCRange;
      else
        aPCSplit = 0.5 * (aCE.ParamFirst + aCE.ParamLast);
    }

    // Create CoEdge for SubA.
    BRepGraphInc::CoEdgeDef& aCoEdgeSubA    = theGraph.myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeSubAIdx = theGraph.myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubA.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeSubAIdx);
    initSubCoEdgeEntity(aCoEdgeSubA,
                     BRepGraph_EdgeId(aSubAIdx),
                     aCE.FaceEntityId,
                     aCE.Sense,
                     aCE.Curve2DRepId,
                     aCE.ParamFirst,
                     aPCSplit,
                     aCE.Continuity);
    theGraph.allocateUID(aCoEdgeSubA.Id);

    // Create CoEdge for SubB.
    BRepGraphInc::CoEdgeDef& aCoEdgeSubB    = theGraph.myData->myIncStorage.AppendCoEdge();
    const int                   aCoEdgeSubBIdx = theGraph.myData->myIncStorage.NbCoEdges() - 1;
    aCoEdgeSubB.Id                             = BRepGraph_NodeId::CoEdge(aCoEdgeSubBIdx);
    initSubCoEdgeEntity(aCoEdgeSubB,
                     BRepGraph_EdgeId(aSubBIdx),
                     aCE.FaceEntityId,
                     aCE.Sense,
                     aCE.Curve2DRepId,
                     aPCSplit,
                     aCE.ParamLast,
                     aCE.Continuity);
    theGraph.allocateUID(aCoEdgeSubB.Id);
  }

  // Register TopoDS shapes for sub-edges so OriginalOf() works in downstream algorithms.
  if (aOrigCurve3DRepId.IsValid())
  {
    const occ::handle<Geom_Curve>& aOrigCurve3d =
      theGraph.myData->myIncStorage.Curve3DRep(aOrigCurve3DRepId).Curve;
    if (!aOrigCurve3d.IsNull())
    {
      BRep_Builder aBB;

      const TopoDS_Shape aStartVShape = aOrigStartVertexEntityId.IsValid()
                                          ? theGraph.Shapes().Shape(aOrigStartVertexEntityId)
                                          : TopoDS_Shape();
      const TopoDS_Shape aSplitVShape = theGraph.Shapes().Shape(theSplitVertex);
      const TopoDS_Shape aEndVShape   = aOrigEndVertexEntityId.IsValid()
                                          ? theGraph.Shapes().Shape(aOrigEndVertexEntityId)
                                          : TopoDS_Shape();

      TopoDS_Edge aSubAEdge;
      aBB.MakeEdge(aSubAEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
      aBB.Range(aSubAEdge, aOrigParamFirst, theSplitParam);
      if (!aStartVShape.IsNull())
        aBB.Add(aSubAEdge, aStartVShape.Oriented(TopAbs_FORWARD));
      if (!aSplitVShape.IsNull())
        aBB.Add(aSubAEdge, aSplitVShape.Oriented(TopAbs_REVERSED));
      theGraph.myData->myIncStorage.BindOriginal(theSubA, aSubAEdge);

      TopoDS_Edge aSubBEdge;
      aBB.MakeEdge(aSubBEdge, aOrigCurve3d, TopLoc_Location(), aOrigTolerance);
      aBB.Range(aSubBEdge, theSplitParam, aOrigParamLast);
      if (!aSplitVShape.IsNull())
        aBB.Add(aSubBEdge, aSplitVShape.Oriented(TopAbs_FORWARD));
      if (!aEndVShape.IsNull())
        aBB.Add(aSubBEdge, aEndVShape.Oriented(TopAbs_REVERSED));
      theGraph.myData->myIncStorage.BindOriginal(theSubB, aSubBEdge);
    }
  }

  // Update edge-to-wire reverse index incrementally.
  BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ChangeReverseIndex();
  for (int aWIdx = 0; aWIdx < aOrigWires.Length(); ++aWIdx)
  {
    const BRepGraph_WireId aWireId = aOrigWires.Value(aWIdx);
    aRevIdx.UnbindEdgeFromWire(BRepGraph_EdgeId(theEdgeEntity.Index), aWireId);
    aRevIdx.BindEdgeToWire(BRepGraph_EdgeId(aSubAIdx), aWireId);
    aRevIdx.BindEdgeToWire(BRepGraph_EdgeId(aSubBIdx), aWireId);
    theGraph.markModified(aWireId);
  }

  // Incremental vertex-to-edge updates: register sub-edge vertices.
  {
    const BRepGraphInc_Storage& aStorageRef = theGraph.myData->myIncStorage;
    const BRepGraphInc::EdgeDef& aSubAEnt =
      aStorageRef.Edge(BRepGraph_EdgeId(aSubAIdx));
    const BRepGraphInc::EdgeDef& aSubBEnt =
      aStorageRef.Edge(BRepGraph_EdgeId(aSubBIdx));
    BRepGraphInc_ReverseIndex& aRevIdxMut = theGraph.myData->myIncStorage.ChangeReverseIndex();

    // Resolve vertex def ids from the sub-edge ref entries.
    if (aSubAEnt.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRefEntry(aSubAEnt.StartVertexRefId).VertexEntityId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubAIdx));
    }
    if (aSubAEnt.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRefEntry(aSubAEnt.EndVertexRefId).VertexEntityId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubAIdx));
    }
    if (aSubBEnt.StartVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRefEntry(aSubBEnt.StartVertexRefId).VertexEntityId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubBIdx));
    }
    if (aSubBEnt.EndVertexRefId.IsValid())
    {
      const BRepGraph_VertexId aVtxId =
        aStorageRef.VertexRefEntry(aSubBEnt.EndVertexRefId).VertexEntityId;
      if (aVtxId.IsValid())
        aRevIdxMut.BindVertexToEdge(aVtxId, BRepGraph_EdgeId(aSubBIdx));
    }

    // Remove old edge from vertex-to-edge index.
    if (aOrigStartVertexEntityId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigStartVertexEntityId,
                                      BRepGraph_EdgeId(theEdgeEntity.Index));
    if (aOrigEndVertexEntityId.IsValid())
      aRevIdxMut.UnbindVertexFromEdge(aOrigEndVertexEntityId,
                                      BRepGraph_EdgeId(theEdgeEntity.Index));

    // Edge-to-face: derive from original edge's CoEdges (same faces apply to both sub-edges).
    for (int aCEIdx = 0; aCEIdx < aOrigCoEdges.Length(); ++aCEIdx)
    {
      const BRepGraphInc::CoEdgeDef& aCE = aOrigCoEdges.Value(aCEIdx);
      if (aCE.FaceEntityId.IsValid())
      {
        aRevIdxMut.BindEdgeToFace(BRepGraph_EdgeId(aSubAIdx), aCE.FaceEntityId);
        aRevIdxMut.BindEdgeToFace(BRepGraph_EdgeId(aSubBIdx), aCE.FaceEntityId);
      }
    }
  }

  theGraph.markModified(theEdgeEntity);
  theGraph.markModified(theSubA);
  theGraph.markModified(theSubB);

  Standard_ASSERT_VOID(theGraph.myData->myIncStorage.ValidateReverseIndex(),
                       "SplitEdge: post-mutation reverse index inconsistency");
}

//=================================================================================================

void BRepGraph_Mutator::ReplaceEdgeInWire(BRepGraph&             theGraph,
                                          const BRepGraph_WireId theWireEntityId,
                                          const BRepGraph_EdgeId theOldEdgeEntity,
                                          const BRepGraph_EdgeId theNewEdgeEntity,
                                          const bool             theReversed)
{
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  const NCollection_Vector<BRepGraph_CoEdgeRefId>& aWireRefIds =
    wireCoEdgeRefIds(aStorage, theWireEntityId);

  // Update incidence by scanning wire-owned coedge ref entries.
  for (int aRefIdx = 0; aRefIdx < aWireRefIds.Length(); ++aRefIdx)
  {
    const BRepGraphInc::CoEdgeRefEntry& aRefEntry = aStorage.CoEdgeRefEntry(aWireRefIds.Value(aRefIdx));
    if (aRefEntry.IsRemoved)
      continue;
    const int                           aCoEdgeEntIdx = aRefEntry.CoEdgeEntityId.Index;
    if (aCoEdgeEntIdx < 0 || aCoEdgeEntIdx >= aStorage.NbCoEdges())
      continue;

    BRepGraphInc::CoEdgeDef& aCoEdge =
      aStorage.ChangeCoEdge(BRepGraph_CoEdgeId(aCoEdgeEntIdx));
    if (aCoEdge.EdgeEntityId == theOldEdgeEntity)
    {
      aCoEdge.EdgeEntityId = theNewEdgeEntity;
      if (theReversed)
        aCoEdge.Sense = TopAbs::Reverse(aCoEdge.Sense);

      // Update reverse indices incrementally.
      BRepGraphInc_ReverseIndex& aRevIdx = theGraph.myData->myIncStorage.ChangeReverseIndex();
      aRevIdx.ReplaceEdgeInWireMap(theOldEdgeEntity, theNewEdgeEntity, theWireEntityId);
      aRevIdx.BindEdgeToCoEdge(theNewEdgeEntity, BRepGraph_CoEdgeId(aCoEdgeEntIdx));

      // Update edge-to-face: bind new edge, unbind old edge for all faces of this wire.
      // Wire-to-face mappings are built from FaceDef.WireRefs during Build() and are
      // stable across edge mutations - only face-level operations modify them.
      const NCollection_Vector<BRepGraph_FaceId>* aFaces = aRevIdx.FacesOfWire(theWireEntityId);
      if (aFaces != nullptr)
      {
        for (int aFIdx = 0; aFIdx < aFaces->Length(); ++aFIdx)
        {
          const BRepGraph_FaceId aFaceId = aFaces->Value(aFIdx);
          aRevIdx.BindEdgeToFace(theNewEdgeEntity, aFaceId);
          aRevIdx.UnbindEdgeFromFace(theOldEdgeEntity, aFaceId);
        }
      }
    }
  }

  theGraph.markModified(BRepGraph_NodeId::Wire(theWireEntityId.Index));

  Standard_ASSERT_VOID(theGraph.myData->myIncStorage.ValidateReverseIndex(),
                       "ReplaceEdgeInWire: post-mutation reverse index inconsistency");
}

//=================================================================================================

void BRepGraph_Mutator::CommitMutation(BRepGraph& theGraph)
{
  const bool isValid = ValidateMutationBoundary(theGraph);
  Standard_ASSERT_VOID(isValid, "CommitMutation: mutation boundary consistency check failed");
  (void)isValid;
}

//=================================================================================================

bool BRepGraph_Mutator::ValidateMutationBoundary(const BRepGraph&                         theGraph,
                                                 NCollection_Vector<BoundaryIssue>* const theIssues)
{
  bool                        isValid  = true;
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;

  if (!aStorage.ValidateReverseIndex())
  {
    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue anIssue;
      anIssue.NodeId      = BRepGraph_NodeId();
      anIssue.Description = "Mutation boundary reverse index inconsistency";
      theIssues->Append(anIssue);
    }
  }

  constexpr BRepGraph_NodeId::Kind THE_KINDS[] = {BRepGraph_NodeId::Kind::Vertex,
                                                  BRepGraph_NodeId::Kind::Edge,
                                                  BRepGraph_NodeId::Kind::CoEdge,
                                                  BRepGraph_NodeId::Kind::Wire,
                                                  BRepGraph_NodeId::Kind::Face,
                                                  BRepGraph_NodeId::Kind::Shell,
                                                  BRepGraph_NodeId::Kind::Solid,
                                                  BRepGraph_NodeId::Kind::Compound,
                                                  BRepGraph_NodeId::Kind::CompSolid,
                                                  BRepGraph_NodeId::Kind::Product,
                                                  BRepGraph_NodeId::Kind::Occurrence};
  for (int aKindIdx = 0; aKindIdx < static_cast<int>(sizeof(THE_KINDS) / sizeof(THE_KINDS[0]));
       ++aKindIdx)
  {
    const BRepGraph_NodeId::Kind aKind       = THE_KINDS[aKindIdx];
    const int                    aCachedCnt  = cachedActiveByKind(aStorage, aKind);
    const int                    anActualCnt = countActiveByKind(aStorage, aKind);
    if (aCachedCnt == anActualCnt)
      continue;

    isValid = false;
    if (theIssues != nullptr)
    {
      BoundaryIssue           anIssue;
      TCollection_AsciiString aDesc("Mutation boundary active count mismatch for ");
      aDesc += kindName(aKind);
      aDesc += ": cached=";
      aDesc += TCollection_AsciiString(aCachedCnt);
      aDesc += " actual=";
      aDesc += TCollection_AsciiString(anActualCnt);
      anIssue.NodeId      = BRepGraph_NodeId(aKind, -1);
      anIssue.Description = aDesc;
      theIssues->Append(anIssue);
    }
  }

  return isValid;
}
