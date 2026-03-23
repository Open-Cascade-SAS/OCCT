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

#include <BRepGraph_BackRefManager.hxx>
#include <BRepGraph_Builder.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_TVertex.hxx>
#include <TopoDS.hxx>

//=================================================================================================

void BRepGraph_Builder::populateUsagesAndBridgeFields(BRepGraph& theGraph)
{
  BRepGraph_Data&       aData    = *theGraph.myData;
  BRepGraphInc_Storage& aStorage = aData.myIncStorage;

  if (!aStorage.IsDone)
    return;

  // --- Allocate UIDs for all definitions ---
  for (int i = 0; i < aStorage.Vertices.Length(); ++i)
    theGraph.allocateUID(aStorage.Vertices.Value(i).Id);
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
    theGraph.allocateUID(aStorage.Edges.Value(i).Id);
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
    theGraph.allocateUID(aStorage.Wires.Value(i).Id);
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
    theGraph.allocateUID(aStorage.Faces.Value(i).Id);
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
    theGraph.allocateUID(aStorage.Shells.Value(i).Id);
  for (int i = 0; i < aStorage.Solids.Length(); ++i)
    theGraph.allocateUID(aStorage.Solids.Value(i).Id);
  for (int i = 0; i < aStorage.Compounds.Length(); ++i)
    theGraph.allocateUID(aStorage.Compounds.Value(i).Id);
  for (int i = 0; i < aStorage.CompSolids.Length(); ++i)
    theGraph.allocateUID(aStorage.CompSolids.Value(i).Id);

  // --- PCurves: derive from EdgeFaceGeom table and populate on EdgeEntity ---
  for (int i = 0; i < aStorage.EdgeFaceGeoms.Length(); ++i)
  {
    const BRepGraphInc::EdgeFaceGeom& aGeom = aStorage.EdgeFaceGeoms.Value(i);
    if (aGeom.EdgeIdx < 0 || aGeom.EdgeIdx >= aStorage.Edges.Length())
      continue;

    BRepGraphInc::EdgeEntity& anEdge = aStorage.Edges.ChangeValue(aGeom.EdgeIdx);

    BRepGraphInc::EdgeEntity::PCurveEntry aPCEntry;
    aPCEntry.Curve2d         = aGeom.PCurve;
    aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(aGeom.FaceIdx);
    aPCEntry.ParamFirst      = aGeom.ParamFirst;
    aPCEntry.ParamLast       = aGeom.ParamLast;
    aPCEntry.Continuity      = aGeom.Continuity;
    aPCEntry.UV1             = aGeom.UV1;
    aPCEntry.UV2             = aGeom.UV2;
    aPCEntry.EdgeOrientation = aGeom.EdgeOrientation;
    anEdge.PCurves.Append(aPCEntry);

    // PolygonOnSurface from the same EdgeFaceGeom row.
    if (!aGeom.PolyOnSurf.IsNull())
    {
      BRepGraphInc::EdgeEntity::PolyOnSurfEntry aPolyEntry;
      aPolyEntry.Polygon2D       = aGeom.PolyOnSurf;
      aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(aGeom.FaceIdx);
      aPolyEntry.EdgeOrientation = aGeom.EdgeOrientation;
      anEdge.PolygonsOnSurf.Append(aPolyEntry);
    }
  }

  // PolygonOnTriangulation: derive from EdgeFaceTriGeom table.
  for (int i = 0; i < aStorage.EdgeFaceTriGeoms.Length(); ++i)
  {
    const BRepGraphInc::EdgeFaceTriGeom& aTriGeom = aStorage.EdgeFaceTriGeoms.Value(i);
    if (aTriGeom.EdgeIdx < 0 || aTriGeom.EdgeIdx >= aStorage.Edges.Length())
      continue;

    BRepGraphInc::EdgeEntity& anEdge = aStorage.Edges.ChangeValue(aTriGeom.EdgeIdx);

    BRepGraphInc::EdgeEntity::PolyOnTriEntry aPolyTriEntry;
    aPolyTriEntry.Polygon            = aTriGeom.PolyOnTri;
    aPolyTriEntry.FaceDefId          = BRepGraph_NodeId::Face(aTriGeom.FaceIdx);
    aPolyTriEntry.TriangulationIndex = aTriGeom.TriangulationIndex;
    aPolyTriEntry.EdgeOrientation    = aTriGeom.EdgeOrientation;
    anEdge.PolygonsOnTri.Append(aPolyTriEntry);
  }

  // --- Wires: create WireUsages + EdgeUsages ---
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
  {
    BRepGraphInc::WireEntity& aWireDef = aStorage.Wires.ChangeValue(i);

    BRepGraph_TopoNode::WireUsage& aWU = aData.myWires.Usages.Appended();
    const int aWUIdx = aData.myWires.Usages.Length() - 1;
    aWU.UsageId  = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWUIdx);
    aWU.DefId    = aWireDef.Id;
    aWU.IsClosed = aWireDef.IsClosed;
    aWireDef.Usages.Append(aWU.UsageId);

    // Create EdgeUsages from EdgeRefs.
    for (int e = 0; e < aWireDef.EdgeRefs.Length(); ++e)
    {
      const BRepGraphInc::EdgeRef& aER = aWireDef.EdgeRefs.Value(e);
      BRepGraph_TopoNode::EdgeUsage& aEU = aData.myEdges.Usages.Appended();
      const int aEUIdx = aData.myEdges.Usages.Length() - 1;
      aEU.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Edge, aEUIdx);
      aEU.DefId       = BRepGraph_NodeId::Edge(aER.EdgeIdx);
      aEU.Orientation = aER.Orientation;
      aEU.ParentUsage = aWU.UsageId;

      if (aER.EdgeIdx >= 0 && aER.EdgeIdx < aStorage.Edges.Length())
        aStorage.Edges.ChangeValue(aER.EdgeIdx).Usages.Append(aEU.UsageId);

      aWU.EdgeUsages.Append(aEU.UsageId);

      // Edge-to-wire reverse index.
      BRepGraph_BackRefManager::BindEdgeToWire(theGraph, aER.EdgeIdx, i);
    }
  }

  // --- Shells: create ShellUsages ---
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
  {
    BRepGraphInc::ShellEntity& aShellDef = aStorage.Shells.ChangeValue(i);

    BRepGraph_TopoNode::ShellUsage& aSU = aData.myShells.Usages.Appended();
    const int aSUIdx = aData.myShells.Usages.Length() - 1;
    aSU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aSUIdx);
    aSU.DefId   = aShellDef.Id;
    aShellDef.Usages.Append(aSU.UsageId);
  }

  // --- VertexUsages: create for each EdgeUsage ---
  for (int aEUIdx = 0; aEUIdx < aData.myEdges.Usages.Length(); ++aEUIdx)
  {
    BRepGraph_TopoNode::EdgeUsage& aEU = aData.myEdges.Usages.ChangeValue(aEUIdx);
    const BRepGraphInc::EdgeEntity& anEdgeDef = aStorage.Edges.Value(aEU.DefId.Index);

    auto makeVtxUsage = [&](BRepGraph_NodeId theVtxDefId) -> BRepGraph_UsageId {
      if (!theVtxDefId.IsValid())
        return BRepGraph_UsageId();
      BRepGraph_TopoNode::VertexUsage& aVU = aData.myVertices.Usages.Appended();
      const int aVUIdx = aData.myVertices.Usages.Length() - 1;
      aVU.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Vertex, aVUIdx);
      aVU.DefId       = theVtxDefId;
      aVU.ParentUsage = aEU.UsageId;
      if (theVtxDefId.Index >= 0 && theVtxDefId.Index < aStorage.Vertices.Length())
      {
        aVU.TransformedPoint = aStorage.Vertices.Value(theVtxDefId.Index).Point;
        aStorage.Vertices.ChangeValue(theVtxDefId.Index).Usages.Append(aVU.UsageId);
      }
      return aVU.UsageId;
    };

    aEU.StartVertexUsage = makeVtxUsage(anEdgeDef.StartVertexDefId);
    aEU.EndVertexUsage   = makeVtxUsage(anEdgeDef.EndVertexDefId);
  }

  // --- Compounds ---
  for (int i = 0; i < aStorage.Compounds.Length(); ++i)
  {
    BRepGraphInc::CompoundEntity& aCompDef = aStorage.Compounds.ChangeValue(i);

    // Populate ChildDefIds bridge field.
    for (int c = 0; c < aCompDef.ChildRefs.Length(); ++c)
    {
      const BRepGraphInc::ChildRef& aCR = aCompDef.ChildRefs.Value(c);
      aCompDef.ChildDefIds.Append(
        BRepGraph_NodeId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx));
    }

    BRepGraph_TopoNode::CompoundUsage& aCU = aData.myCompounds.Usages.Appended();
    const int aCUIdx = aData.myCompounds.Usages.Length() - 1;
    aCU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Compound, aCUIdx);
    aCU.DefId   = aCompDef.Id;
    aCompDef.Usages.Append(aCU.UsageId);

    // Set location/orientation from original shape.
    const TopoDS_Shape* anOrigComp = aStorage.OriginalShapes.Seek(aCompDef.Id);
    if (anOrigComp != nullptr)
    {
      aCU.LocalLocation  = anOrigComp->Location();
      aCU.GlobalLocation = anOrigComp->Location();
      aCU.Orientation    = anOrigComp->Orientation();
    }
  }

  // --- CompSolids ---
  for (int i = 0; i < aStorage.CompSolids.Length(); ++i)
  {
    BRepGraphInc::CompSolidEntity& aCSolDef = aStorage.CompSolids.ChangeValue(i);

    // Populate SolidDefIds bridge field.
    for (int s = 0; s < aCSolDef.SolidRefs.Length(); ++s)
    {
      const BRepGraphInc::SolidRef& aSR = aCSolDef.SolidRefs.Value(s);
      aCSolDef.SolidDefIds.Append(BRepGraph_NodeId::Solid(aSR.SolidIdx));
    }

    BRepGraph_TopoNode::CompSolidUsage& aCSU = aData.myCompSolids.Usages.Appended();
    const int aCSUIdx = aData.myCompSolids.Usages.Length() - 1;
    aCSU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::CompSolid, aCSUIdx);
    aCSU.DefId   = aCSolDef.Id;
    aCSolDef.Usages.Append(aCSU.UsageId);

    const TopoDS_Shape* anOrigCSol = aStorage.OriginalShapes.Seek(aCSolDef.Id);
    if (anOrigCSol != nullptr)
    {
      aCSU.LocalLocation  = anOrigCSol->Location();
      aCSU.GlobalLocation = anOrigCSol->Location();
      aCSU.Orientation    = anOrigCSol->Orientation();
    }
  }

  // --- Solids ---
  for (int i = 0; i < aStorage.Solids.Length(); ++i)
  {
    BRepGraphInc::SolidEntity& aSolidDef = aStorage.Solids.ChangeValue(i);

    BRepGraph_TopoNode::SolidUsage& aSoU = aData.mySolids.Usages.Appended();
    const int aSoUIdx = aData.mySolids.Usages.Length() - 1;
    aSoU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Solid, aSoUIdx);
    aSoU.DefId   = aSolidDef.Id;
    aSolidDef.Usages.Append(aSoU.UsageId);

    const TopoDS_Shape* anOrigSolid = aStorage.OriginalShapes.Seek(aSolidDef.Id);
    if (anOrigSolid != nullptr)
    {
      aSoU.LocalLocation  = anOrigSolid->Location();
      aSoU.Orientation    = anOrigSolid->Orientation();
    }

    // Compute GlobalLocation by composing with parent container location.
    TopLoc_Location aParentGlobalLoc;
    for (int c = 0; c < aStorage.Compounds.Length(); ++c)
    {
      const BRepGraphInc::CompoundEntity& aComp = aStorage.Compounds.Value(c);
      for (int cr = 0; cr < aComp.ChildRefs.Length(); ++cr)
      {
        const BRepGraphInc::ChildRef& aCR = aComp.ChildRefs.Value(cr);
        if (aCR.Kind == static_cast<int>(BRepGraph_NodeId::Kind::Solid)
            && aCR.ChildIdx == i)
        {
          const TopoDS_Shape* aCompOrig = aStorage.OriginalShapes.Seek(aComp.Id);
          if (aCompOrig != nullptr)
            aParentGlobalLoc = aCompOrig->Location() * aCR.LocalLocation;
          else
            aParentGlobalLoc = aCR.LocalLocation;
        }
      }
    }
    for (int cs = 0; cs < aStorage.CompSolids.Length(); ++cs)
    {
      const BRepGraphInc::CompSolidEntity& aCSol = aStorage.CompSolids.Value(cs);
      for (int sr = 0; sr < aCSol.SolidRefs.Length(); ++sr)
      {
        const BRepGraphInc::SolidRef& aSR2 = aCSol.SolidRefs.Value(sr);
        if (aSR2.SolidIdx == i)
        {
          const TopoDS_Shape* aCSolOrig = aStorage.OriginalShapes.Seek(aCSol.Id);
          if (aCSolOrig != nullptr)
            aParentGlobalLoc = aCSolOrig->Location() * aSR2.LocalLocation;
          else
            aParentGlobalLoc = aSR2.LocalLocation;
        }
      }
    }
    aSoU.GlobalLocation = aParentGlobalLoc * aSoU.LocalLocation;

    for (int s = 0; s < aSolidDef.ShellRefs.Length(); ++s)
    {
      const BRepGraphInc::ShellRef& aSR = aSolidDef.ShellRefs.Value(s);
      if (aSR.ShellIdx < 0 || aSR.ShellIdx >= aStorage.Shells.Length())
        continue;

      const BRepGraphInc::ShellEntity& aShellDef = aStorage.Shells.Value(aSR.ShellIdx);
      if (!aShellDef.Usages.IsEmpty())
      {
        BRepGraph_UsageId aShellUsageId = aShellDef.Usages.Value(0);
        BRepGraph_TopoNode::ShellUsage& aShU =
          aData.myShells.Usages.ChangeValue(aShellUsageId.Index);
        aShU.ParentUsage    = aSoU.UsageId;
        aShU.LocalLocation  = aSR.LocalLocation;
        aShU.Orientation    = aSR.Orientation;
        aShU.GlobalLocation = aSoU.GlobalLocation * aSR.LocalLocation;
        aSoU.ShellUsages.Append(aShellUsageId);
      }
    }
  }

  // Helper: create a FaceUsage for a face def, linking its wire usages.
  auto createFaceUsage = [&](int                    theFaceIdx,
                             TopAbs_Orientation      theOri,
                             BRepGraph_UsageId       theParentUsage,
                             const TopLoc_Location&  theLoc,
                             const TopLoc_Location&  theParentGlobalLoc) -> BRepGraph_UsageId {
    const BRepGraphInc::FaceEntity& aFaceSrc = aStorage.Faces.Value(theFaceIdx);

    BRepGraph_TopoNode::FaceUsage& aFU = aData.myFaces.Usages.Appended();
    const int aFUIdx = aData.myFaces.Usages.Length() - 1;
    aFU.UsageId        = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Face, aFUIdx);
    aFU.DefId          = BRepGraph_NodeId::Face(theFaceIdx);
    aFU.Orientation    = theOri;
    aFU.ParentUsage    = theParentUsage;
    aFU.LocalLocation  = theLoc;
    aFU.GlobalLocation = theParentGlobalLoc * theLoc;
    aStorage.Faces.ChangeValue(theFaceIdx).Usages.Append(aFU.UsageId);

    // Link WireUsages from WireRefs.
    for (int w = 0; w < aFaceSrc.WireRefs.Length(); ++w)
    {
      const BRepGraphInc::WireRef& aWR = aFaceSrc.WireRefs.Value(w);
      if (aWR.WireIdx < 0 || aWR.WireIdx >= aStorage.Wires.Length())
        continue;

      const BRepGraphInc::WireEntity& aWireDef = aStorage.Wires.Value(aWR.WireIdx);
      if (aWireDef.Usages.IsEmpty())
        continue;
      BRepGraph_UsageId aWireUsageId = aWireDef.Usages.Value(0);

      aData.myWires.Usages.ChangeValue(aWireUsageId.Index).OwnerFaceUsage = aFU.UsageId;

      if (aWR.IsOuter)
        aFU.OuterWireUsage = aWireUsageId;
      else
        aFU.InnerWireUsages.Append(aWireUsageId);
    }

    return aFU.UsageId;
  };

  // --- FaceUsages: create after all GlobalLocations are resolved ---
  NCollection_DataMap<int, bool> aFaceInShell;
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
  {
    const BRepGraphInc::ShellEntity& aShellSrc = aStorage.Shells.Value(i);
    BRepGraph_TopoNode::ShellUsage& aSU =
      aData.myShells.Usages.ChangeValue(aStorage.Shells.Value(i).Usages.Value(0).Index);

    for (int f = 0; f < aShellSrc.FaceRefs.Length(); ++f)
    {
      const BRepGraphInc::FaceRef& aFR = aShellSrc.FaceRefs.Value(f);
      if (aFR.FaceIdx < 0 || aFR.FaceIdx >= aStorage.Faces.Length())
        continue;

      BRepGraph_UsageId aFUId = createFaceUsage(
        aFR.FaceIdx, aFR.Orientation, aSU.UsageId, aFR.LocalLocation, aSU.GlobalLocation);
      aSU.FaceUsages.Append(aFUId);
      aFaceInShell.Bind(aFR.FaceIdx, true);
    }
  }

  // Create standalone FaceUsages for faces not in any shell.
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
  {
    if (aFaceInShell.IsBound(i))
      continue;
    createFaceUsage(i, TopAbs_FORWARD, BRepGraph_UsageId(), TopLoc_Location(), TopLoc_Location());
  }

  // --- Vertex point representations (post-pass) ---
  NCollection_DataMap<const Geom_Curve*, BRepGraph_NodeId> aCurveToEdgeDef;
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
  {
    const BRepGraphInc::EdgeEntity& anEdge = aStorage.Edges.Value(i);
    if (!anEdge.Curve3d.IsNull())
      aCurveToEdgeDef.TryBind(anEdge.Curve3d.get(), anEdge.Id);
  }

  NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDef;
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
  {
    const BRepGraphInc::FaceEntity& aFace = aStorage.Faces.Value(i);
    if (!aFace.Surface.IsNull())
      aSurfToFaceDef.TryBind(aFace.Surface.get(), aFace.Id);
  }

  for (int aVtxIdx = 0; aVtxIdx < aStorage.Vertices.Length(); ++aVtxIdx)
  {
    BRepGraphInc::VertexEntity& aVtxDef = aStorage.Vertices.ChangeValue(aVtxIdx);
    const TopoDS_Shape* aVtxShape = aStorage.OriginalShapes.Seek(aVtxDef.Id);
    if (aVtxShape == nullptr || aVtxShape->IsNull())
      continue;
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(*aVtxShape);
    const Handle(BRep_TVertex)& aTVertex =
      Handle(BRep_TVertex)::DownCast(aVertex.TShape());
    if (aTVertex.IsNull())
      continue;

    for (const Handle(BRep_PointRepresentation)& aPtRep : aTVertex->Points())
    {
      if (aPtRep.IsNull())
        continue;
      if (const Handle(BRep_PointOnCurve) aPOC = Handle(BRep_PointOnCurve)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* anEdgeId = aCurveToEdgeDef.Seek(aPOC->Curve().get());
        if (anEdgeId == nullptr)
          continue;
        BRepGraphInc::VertexEntity::PointOnCurveEntry anEntry;
        anEntry.Parameter = aPOC->Parameter();
        anEntry.EdgeDefId = *anEdgeId;
        aVtxDef.PointsOnCurve.Append(anEntry);
      }
      else if (const Handle(BRep_PointOnCurveOnSurface) aPOCS =
                 Handle(BRep_PointOnCurveOnSurface)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOCS->Surface().get());
        if (aFaceId == nullptr)
          continue;
        BRepGraphInc::VertexEntity::PointOnPCurveEntry anEntry;
        anEntry.Parameter = aPOCS->Parameter();
        anEntry.FaceDefId = *aFaceId;
        aVtxDef.PointsOnPCurve.Append(anEntry);
      }
      else if (const Handle(BRep_PointOnSurface) aPOS =
                 Handle(BRep_PointOnSurface)::DownCast(aPtRep))
      {
        const BRepGraph_NodeId* aFaceId = aSurfToFaceDef.Seek(aPOS->Surface().get());
        if (aFaceId == nullptr)
          continue;
        BRepGraphInc::VertexEntity::PointOnSurfaceEntry anEntry;
        anEntry.ParameterU = aPOS->Parameter();
        anEntry.ParameterV = aPOS->Parameter2();
        anEntry.FaceDefId  = *aFaceId;
        aVtxDef.PointsOnSurface.Append(anEntry);
      }
    }
  }
}

//=================================================================================================

void BRepGraph_Builder::Perform(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  // Phase 0: Clear all storage.
  BRepGraph_BackRefManager::ClearAll(theGraph);

  theGraph.myData->mySolids.Clear();
  theGraph.myData->myShells.Clear();
  theGraph.myData->myFaces.Clear();
  theGraph.myData->myWires.Clear();
  theGraph.myData->myEdges.Clear();
  theGraph.myData->myVertices.Clear();
  theGraph.myData->myCompounds.Clear();
  theGraph.myData->myCompSolids.Clear();
  theGraph.myData->myTShapeToDefId.Clear();
  theGraph.myData->myHistoryLog.Clear();
  theGraph.myData->myOriginalShapes.Clear();
  theGraph.myData->myCurrentShapes.Clear();
  ++theGraph.myData->myGeneration;
  theGraph.myData->myIsDone = false;

  if (theShape.IsNull())
    return;

  // Phase 1: Populate incidence-table storage (sole source of truth).
  BRepGraphInc_Populate::Perform(theGraph.myData->myIncStorage, theShape, theParallel);
  if (!theGraph.myData->myIncStorage.IsDone)
  {
    theGraph.myData->myIncStorage.Clear();
    return;
  }

  // Phase 2: Copy unified TShape→NodeId and OriginalShapes from incidence storage.
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  theGraph.myData->myTShapeToDefId.ReSize(aStorage.TShapeToNodeId.Extent());
  for (NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId>::Iterator
         anIter(aStorage.TShapeToNodeId);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myTShapeToDefId.Bind(anIter.Key(), anIter.Value());
  }
  for (NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>::Iterator
         anIter(aStorage.OriginalShapes);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myOriginalShapes.Bind(anIter.Key(), anIter.Value());
  }

  // Phase 3: Populate Usage objects and bridge fields on incidence entities.
  populateUsagesAndBridgeFields(theGraph);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  if (theShape.IsNull())
    return;

  // Extend incidence storage incrementally.
  BRepGraphInc_Populate::Append(theGraph.myData->myIncStorage, theShape, theParallel);

  // Clear usage stores (Defs are references to incidence, no need to clear them).
  theGraph.myData->mySolids.Clear();
  theGraph.myData->myShells.Clear();
  theGraph.myData->myFaces.Clear();
  theGraph.myData->myWires.Clear();
  theGraph.myData->myEdges.Clear();
  theGraph.myData->myVertices.Clear();
  theGraph.myData->myCompounds.Clear();
  theGraph.myData->myCompSolids.Clear();
  theGraph.myData->myEdgeToWires.Clear();

  // Clear bridge fields on incidence entities before re-populating.
  BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
  {
    BRepGraphInc::EdgeEntity& anEdge = aStorage.Edges.ChangeValue(i);
    anEdge.PCurves.Clear();
    anEdge.PolygonsOnSurf.Clear();
    anEdge.PolygonsOnTri.Clear();
    anEdge.Usages.Clear();
  }
  for (int i = 0; i < aStorage.Vertices.Length(); ++i)
  {
    BRepGraphInc::VertexEntity& aVtx = aStorage.Vertices.ChangeValue(i);
    aVtx.PointsOnCurve.Clear();
    aVtx.PointsOnSurface.Clear();
    aVtx.PointsOnPCurve.Clear();
    aVtx.Usages.Clear();
  }
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
    aStorage.Wires.ChangeValue(i).Usages.Clear();
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
    aStorage.Faces.ChangeValue(i).Usages.Clear();
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
    aStorage.Shells.ChangeValue(i).Usages.Clear();
  for (int i = 0; i < aStorage.Solids.Length(); ++i)
    aStorage.Solids.ChangeValue(i).Usages.Clear();
  for (int i = 0; i < aStorage.Compounds.Length(); ++i)
  {
    BRepGraphInc::CompoundEntity& aComp = aStorage.Compounds.ChangeValue(i);
    aComp.ChildDefIds.Clear();
    aComp.Usages.Clear();
  }
  for (int i = 0; i < aStorage.CompSolids.Length(); ++i)
  {
    BRepGraphInc::CompSolidEntity& aCSol = aStorage.CompSolids.ChangeValue(i);
    aCSol.SolidDefIds.Clear();
    aCSol.Usages.Clear();
  }

  // Re-copy TShape→NodeId and OriginalShapes.
  theGraph.myData->myTShapeToDefId.Clear();
  theGraph.myData->myTShapeToDefId.ReSize(aStorage.TShapeToNodeId.Extent());
  for (NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId>::Iterator
         anIter(aStorage.TShapeToNodeId);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myTShapeToDefId.Bind(anIter.Key(), anIter.Value());
  }
  theGraph.myData->myOriginalShapes.Clear();
  for (NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>::Iterator
         anIter(aStorage.OriginalShapes);
       anIter.More(); anIter.Next())
  {
    theGraph.myData->myOriginalShapes.Bind(anIter.Key(), anIter.Value());
  }

  populateUsagesAndBridgeFields(theGraph);

  theGraph.myData->myIsDone = true;
}
