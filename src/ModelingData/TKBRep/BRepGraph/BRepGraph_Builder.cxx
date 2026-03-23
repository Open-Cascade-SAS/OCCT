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

void BRepGraph_Builder::deriveLegacyFromIncidence(BRepGraph& theGraph)
{
  BRepGraph_Data&             aData    = *theGraph.myData;
  const BRepGraphInc_Storage& aStorage = aData.myIncStorage;

  if (!aStorage.IsDone)
    return;

  // --- Vertices ---
  for (int i = 0; i < aStorage.Vertices.Length(); ++i)
  {
    const BRepGraphInc::VertexEntity& aSrc = aStorage.Vertices.Value(i);
    BRepGraph_TopoNode::VertexDef& aDef = aData.myVertices.Defs.Appended();
    aDef.Id        = aSrc.Id;
    aDef.Cache     = aSrc.Cache;
    aDef.IsModified = aSrc.IsModified;
    aDef.IsRemoved  = aSrc.IsRemoved;
    aDef.Point     = aSrc.Point;
    aDef.Tolerance = aSrc.Tolerance;
    theGraph.allocateUID(aDef.Id);
  }

  // --- Edges ---
  for (int i = 0; i < aStorage.Edges.Length(); ++i)
  {
    const BRepGraphInc::EdgeEntity& aSrc = aStorage.Edges.Value(i);
    BRepGraph_TopoNode::EdgeDef& aDef = aData.myEdges.Defs.Appended();
    aDef.Id            = aSrc.Id;
    aDef.Cache         = aSrc.Cache;
    aDef.IsModified    = aSrc.IsModified;
    aDef.IsRemoved     = aSrc.IsRemoved;
    aDef.Curve3d       = aSrc.Curve3d;
    aDef.ParamFirst    = aSrc.ParamFirst;
    aDef.ParamLast     = aSrc.ParamLast;
    aDef.Tolerance     = aSrc.Tolerance;
    aDef.IsDegenerate  = aSrc.IsDegenerate;
    aDef.SameParameter = aSrc.SameParameter;
    aDef.SameRange     = aSrc.SameRange;
    aDef.Polygon3D     = aSrc.Polygon3D;

    // Convert vertex int indices to NodeId with bounds validation.
    const int aNbVtx = aStorage.Vertices.Length();
    if (aSrc.StartVertexIdx >= 0 && aSrc.StartVertexIdx < aNbVtx)
      aDef.StartVertexDefId = BRepGraph_NodeId::Vertex(aSrc.StartVertexIdx);
    if (aSrc.EndVertexIdx >= 0 && aSrc.EndVertexIdx < aNbVtx)
      aDef.EndVertexDefId = BRepGraph_NodeId::Vertex(aSrc.EndVertexIdx);

    // Regularities: convert int face indices to NodeId.
    for (int r = 0; r < aSrc.Regularities.Length(); ++r)
    {
      const BRepGraphInc::EdgeEntity::RegularityEntry& aReg = aSrc.Regularities.Value(r);
      BRepGraph_TopoNode::EdgeDef::RegularityEntry aLegacyReg;
      aLegacyReg.FaceDef1   = BRepGraph_NodeId::Face(aReg.FaceIdx1);
      aLegacyReg.FaceDef2   = BRepGraph_NodeId::Face(aReg.FaceIdx2);
      aLegacyReg.Continuity = aReg.Continuity;
      aDef.Regularities.Append(aLegacyReg);
    }

    theGraph.allocateUID(aDef.Id);
  }

  // PCurves: derive from EdgeFaceGeom table.
  for (int i = 0; i < aStorage.EdgeFaceGeoms.Length(); ++i)
  {
    const BRepGraphInc::EdgeFaceGeom& aGeom = aStorage.EdgeFaceGeoms.Value(i);
    if (aGeom.EdgeIdx < 0 || aGeom.EdgeIdx >= aData.myEdges.Defs.Length())
      continue;

    BRepGraph_TopoNode::EdgeDef& aDef = aData.myEdges.Defs.ChangeValue(aGeom.EdgeIdx);

    BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCEntry;
    aPCEntry.Curve2d         = aGeom.PCurve;
    aPCEntry.FaceDefId       = BRepGraph_NodeId::Face(aGeom.FaceIdx);
    aPCEntry.ParamFirst      = aGeom.ParamFirst;
    aPCEntry.ParamLast       = aGeom.ParamLast;
    aPCEntry.Continuity      = aGeom.Continuity;
    aPCEntry.UV1             = aGeom.UV1;
    aPCEntry.UV2             = aGeom.UV2;
    aPCEntry.EdgeOrientation = aGeom.EdgeOrientation;
    aDef.PCurves.Append(aPCEntry);

    // PolygonOnSurface from the same EdgeFaceGeom row.
    if (!aGeom.PolyOnSurf.IsNull())
    {
      BRepGraph_TopoNode::EdgeDef::PolyOnSurfEntry aPolyEntry;
      aPolyEntry.Polygon2D       = aGeom.PolyOnSurf;
      aPolyEntry.FaceDefId       = BRepGraph_NodeId::Face(aGeom.FaceIdx);
      aPolyEntry.EdgeOrientation = aGeom.EdgeOrientation;
      aDef.PolygonsOnSurf.Append(aPolyEntry);
    }
  }

  // PolygonOnTriangulation: derive from EdgeFaceTriGeom table.
  for (int i = 0; i < aStorage.EdgeFaceTriGeoms.Length(); ++i)
  {
    const BRepGraphInc::EdgeFaceTriGeom& aTriGeom = aStorage.EdgeFaceTriGeoms.Value(i);
    if (aTriGeom.EdgeIdx < 0 || aTriGeom.EdgeIdx >= aData.myEdges.Defs.Length())
      continue;

    BRepGraph_TopoNode::EdgeDef& aDef = aData.myEdges.Defs.ChangeValue(aTriGeom.EdgeIdx);

    BRepGraph_TopoNode::EdgeDef::PolyOnTriEntry aPolyTriEntry;
    aPolyTriEntry.Polygon            = aTriGeom.PolyOnTri;
    aPolyTriEntry.FaceDefId          = BRepGraph_NodeId::Face(aTriGeom.FaceIdx);
    aPolyTriEntry.TriangulationIndex = aTriGeom.TriangulationIndex;
    aPolyTriEntry.EdgeOrientation    = aTriGeom.EdgeOrientation;
    aDef.PolygonsOnTri.Append(aPolyTriEntry);
  }

  // --- Wires ---
  for (int i = 0; i < aStorage.Wires.Length(); ++i)
  {
    const BRepGraphInc::WireEntity& aSrc = aStorage.Wires.Value(i);
    BRepGraph_TopoNode::WireDef& aDef = aData.myWires.Defs.Appended();
    aDef.Id        = aSrc.Id;
    aDef.Cache     = aSrc.Cache;
    aDef.IsModified = aSrc.IsModified;
    aDef.IsRemoved  = aSrc.IsRemoved;
    theGraph.allocateUID(aDef.Id);

    // Create WireUsage.
    BRepGraph_TopoNode::WireUsage& aWU = aData.myWires.Usages.Appended();
    const int aWUIdx = aData.myWires.Usages.Length() - 1;
    aWU.UsageId  = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Wire, aWUIdx);
    aWU.DefId    = aDef.Id;
    aWU.IsClosed = aSrc.IsClosed;
    aDef.Usages.Append(aWU.UsageId);

    // Create EdgeUsages from EdgeRefs.
    for (int e = 0; e < aSrc.EdgeRefs.Length(); ++e)
    {
      const BRepGraphInc::EdgeRef& aER = aSrc.EdgeRefs.Value(e);
      BRepGraph_TopoNode::EdgeUsage& aEU = aData.myEdges.Usages.Appended();
      const int aEUIdx = aData.myEdges.Usages.Length() - 1;
      aEU.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Edge, aEUIdx);
      aEU.DefId       = BRepGraph_NodeId::Edge(aER.EdgeIdx);
      aEU.Orientation = aER.Orientation;
      aEU.ParentUsage = aWU.UsageId;

      if (aER.EdgeIdx >= 0 && aER.EdgeIdx < aData.myEdges.Defs.Length())
        aData.myEdges.Defs.ChangeValue(aER.EdgeIdx).Usages.Append(aEU.UsageId);

      aWU.EdgeUsages.Append(aEU.UsageId);

      // Edge-to-wire reverse index.
      BRepGraph_BackRefManager::BindEdgeToWire(theGraph, aER.EdgeIdx, i);
    }
  }

  // --- Faces (definition only, no usage yet) ---
  for (int i = 0; i < aStorage.Faces.Length(); ++i)
  {
    const BRepGraphInc::FaceEntity& aSrc = aStorage.Faces.Value(i);
    BRepGraph_TopoNode::FaceDef& aDef = aData.myFaces.Defs.Appended();
    aDef.Id                      = aSrc.Id;
    aDef.Cache                   = aSrc.Cache;
    aDef.IsModified              = aSrc.IsModified;
    aDef.IsRemoved               = aSrc.IsRemoved;
    aDef.Surface                 = aSrc.Surface;
    aDef.Tolerance               = aSrc.Tolerance;
    aDef.NaturalRestriction      = aSrc.NaturalRestriction;
    aDef.ActiveTriangulationIndex = aSrc.ActiveTriangulationIndex;
    for (int t = 0; t < aSrc.Triangulations.Length(); ++t)
      aDef.Triangulations.Append(aSrc.Triangulations.Value(t));
    theGraph.allocateUID(aDef.Id);
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
    aData.myFaces.Defs.ChangeValue(theFaceIdx).Usages.Append(aFU.UsageId);

    // Link WireUsages from WireRefs.
    for (int w = 0; w < aFaceSrc.WireRefs.Length(); ++w)
    {
      const BRepGraphInc::WireRef& aWR = aFaceSrc.WireRefs.Value(w);
      if (aWR.WireIdx < 0 || aWR.WireIdx >= aData.myWires.Defs.Length())
        continue;

      const BRepGraph_TopoNode::WireDef& aWireDef = aData.myWires.Defs.Value(aWR.WireIdx);
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

  // --- Shells (definitions and usages, without face linkage yet) ---
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
  {
    const BRepGraphInc::ShellEntity& aSrc = aStorage.Shells.Value(i);
    BRepGraph_TopoNode::ShellDef& aDef = aData.myShells.Defs.Appended();
    aDef.Id        = aSrc.Id;
    aDef.Cache     = aSrc.Cache;
    aDef.IsModified = aSrc.IsModified;
    aDef.IsRemoved  = aSrc.IsRemoved;
    theGraph.allocateUID(aDef.Id);

    BRepGraph_TopoNode::ShellUsage& aSU = aData.myShells.Usages.Appended();
    const int aSUIdx = aData.myShells.Usages.Length() - 1;
    aSU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Shell, aSUIdx);
    aSU.DefId   = aDef.Id;
    aDef.Usages.Append(aSU.UsageId);
  }

  // --- VertexUsages: create for each EdgeUsage ---
  for (int aEUIdx = 0; aEUIdx < aData.myEdges.Usages.Length(); ++aEUIdx)
  {
    BRepGraph_TopoNode::EdgeUsage& aEU = aData.myEdges.Usages.ChangeValue(aEUIdx);
    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = aData.myEdges.Defs.Value(aEU.DefId.Index);

    auto makeVtxUsage = [&](BRepGraph_NodeId theVtxDefId) -> BRepGraph_UsageId {
      if (!theVtxDefId.IsValid())
        return BRepGraph_UsageId();
      BRepGraph_TopoNode::VertexUsage& aVU = aData.myVertices.Usages.Appended();
      const int aVUIdx = aData.myVertices.Usages.Length() - 1;
      aVU.UsageId     = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Vertex, aVUIdx);
      aVU.DefId       = theVtxDefId;
      aVU.ParentUsage = aEU.UsageId;
      if (theVtxDefId.Index >= 0 && theVtxDefId.Index < aData.myVertices.Defs.Length())
      {
        aVU.TransformedPoint = aData.myVertices.Defs.Value(theVtxDefId.Index).Point;
        aData.myVertices.Defs.ChangeValue(theVtxDefId.Index).Usages.Append(aVU.UsageId);
      }
      return aVU.UsageId;
    };

    aEU.StartVertexUsage = makeVtxUsage(anEdgeDef.StartVertexDefId);
    aEU.EndVertexUsage   = makeVtxUsage(anEdgeDef.EndVertexDefId);
  }

  // --- Compounds (before Solids so that compound→solid parent chain can propagate) ---
  // Compounds are built first so child usage parent links can be set when solids are created.
  for (int i = 0; i < aStorage.Compounds.Length(); ++i)
  {
    const BRepGraphInc::CompoundEntity& aSrc = aStorage.Compounds.Value(i);
    BRepGraph_TopoNode::CompoundDef& aDef = aData.myCompounds.Defs.Appended();
    aDef.Id        = aSrc.Id;
    aDef.Cache     = aSrc.Cache;
    aDef.IsModified = aSrc.IsModified;
    aDef.IsRemoved  = aSrc.IsRemoved;
    theGraph.allocateUID(aDef.Id);

    for (int c = 0; c < aSrc.ChildRefs.Length(); ++c)
    {
      const BRepGraphInc::ChildRef& aCR = aSrc.ChildRefs.Value(c);
      aDef.ChildDefIds.Append(
        BRepGraph_NodeId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx));
    }

    BRepGraph_TopoNode::CompoundUsage& aCU = aData.myCompounds.Usages.Appended();
    const int aCUIdx = aData.myCompounds.Usages.Length() - 1;
    aCU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Compound, aCUIdx);
    aCU.DefId   = aDef.Id;
    aDef.Usages.Append(aCU.UsageId);

    // Set location/orientation from original shape.
    const TopoDS_Shape* anOrigComp = aStorage.OriginalShapes.Seek(aSrc.Id);
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
    const BRepGraphInc::CompSolidEntity& aSrc = aStorage.CompSolids.Value(i);
    BRepGraph_TopoNode::CompSolidDef& aDef = aData.myCompSolids.Defs.Appended();
    aDef.Id        = aSrc.Id;
    aDef.Cache     = aSrc.Cache;
    aDef.IsModified = aSrc.IsModified;
    aDef.IsRemoved  = aSrc.IsRemoved;
    theGraph.allocateUID(aDef.Id);

    for (int s = 0; s < aSrc.SolidRefs.Length(); ++s)
    {
      const BRepGraphInc::SolidRef& aSR = aSrc.SolidRefs.Value(s);
      aDef.SolidDefIds.Append(BRepGraph_NodeId::Solid(aSR.SolidIdx));
    }

    BRepGraph_TopoNode::CompSolidUsage& aCSU = aData.myCompSolids.Usages.Appended();
    const int aCSUIdx = aData.myCompSolids.Usages.Length() - 1;
    aCSU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::CompSolid, aCSUIdx);
    aCSU.DefId   = aDef.Id;
    aDef.Usages.Append(aCSU.UsageId);

    // Set location/orientation from original shape.
    const TopoDS_Shape* anOrigCSol = aStorage.OriginalShapes.Seek(aSrc.Id);
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
    const BRepGraphInc::SolidEntity& aSrc = aStorage.Solids.Value(i);
    BRepGraph_TopoNode::SolidDef& aDef = aData.mySolids.Defs.Appended();
    aDef.Id        = aSrc.Id;
    aDef.Cache     = aSrc.Cache;
    aDef.IsModified = aSrc.IsModified;
    aDef.IsRemoved  = aSrc.IsRemoved;
    theGraph.allocateUID(aDef.Id);

    BRepGraph_TopoNode::SolidUsage& aSoU = aData.mySolids.Usages.Appended();
    const int aSoUIdx = aData.mySolids.Usages.Length() - 1;
    aSoU.UsageId = BRepGraph_UsageId(BRepGraph_NodeId::Kind::Solid, aSoUIdx);
    aSoU.DefId   = aDef.Id;
    aDef.Usages.Append(aSoU.UsageId);

    // Set location/orientation from original shape.
    const TopoDS_Shape* anOrigSolid = aStorage.OriginalShapes.Seek(aSrc.Id);
    if (anOrigSolid != nullptr)
    {
      aSoU.LocalLocation  = anOrigSolid->Location();
      aSoU.Orientation    = anOrigSolid->Orientation();
    }

    // Compute GlobalLocation by composing with parent container location.
    // Walk compound/compsolid ChildRefs to find this solid's parent location.
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
          // Compose: compound's own location * child's local location in compound.
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

    for (int s = 0; s < aSrc.ShellRefs.Length(); ++s)
    {
      const BRepGraphInc::ShellRef& aSR = aSrc.ShellRefs.Value(s);
      if (aSR.ShellIdx < 0 || aSR.ShellIdx >= aData.myShells.Defs.Length())
        continue;

      const BRepGraph_TopoNode::ShellDef& aShellDef = aData.myShells.Defs.Value(aSR.ShellIdx);
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

  // --- FaceUsages: create after all GlobalLocations are resolved ---
  NCollection_DataMap<int, bool> aFaceInShell;
  for (int i = 0; i < aStorage.Shells.Length(); ++i)
  {
    const BRepGraphInc::ShellEntity& aShellSrc = aStorage.Shells.Value(i);
    BRepGraph_TopoNode::ShellUsage& aSU =
      aData.myShells.Usages.ChangeValue(aData.myShells.Defs.Value(i).Usages.Value(0).Index);

    for (int f = 0; f < aShellSrc.FaceRefs.Length(); ++f)
    {
      const BRepGraphInc::FaceRef& aFR = aShellSrc.FaceRefs.Value(f);
      if (aFR.FaceIdx < 0 || aFR.FaceIdx >= aData.myFaces.Defs.Length())
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
  for (int i = 0; i < aData.myEdges.Defs.Length(); ++i)
  {
    const BRepGraph_TopoNode::EdgeDef& anEdge = aData.myEdges.Defs.Value(i);
    if (!anEdge.Curve3d.IsNull())
      aCurveToEdgeDef.TryBind(anEdge.Curve3d.get(), anEdge.Id);
  }

  NCollection_DataMap<const Geom_Surface*, BRepGraph_NodeId> aSurfToFaceDef;
  for (int i = 0; i < aData.myFaces.Defs.Length(); ++i)
  {
    const BRepGraph_TopoNode::FaceDef& aFace = aData.myFaces.Defs.Value(i);
    if (!aFace.Surface.IsNull())
      aSurfToFaceDef.TryBind(aFace.Surface.get(), aFace.Id);
  }

  for (int aVtxIdx = 0; aVtxIdx < aData.myVertices.Defs.Length(); ++aVtxIdx)
  {
    BRepGraph_TopoNode::VertexDef& aVtxDef = aData.myVertices.Defs.ChangeValue(aVtxIdx);
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
        BRepGraph_TopoNode::VertexDef::PointOnCurveEntry anEntry;
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
        BRepGraph_TopoNode::VertexDef::PointOnPCurveEntry anEntry;
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
        BRepGraph_TopoNode::VertexDef::PointOnSurfaceEntry anEntry;
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

  // Phase 1: Populate incidence-table storage (single source of truth).
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

  // Phase 3: Derive legacy Def/Usage stores from incidence entities.
  // This is temporary compatibility glue until all consumers migrate to incidence types.
  deriveLegacyFromIncidence(theGraph);

  theGraph.myData->myIsDone = true;
}

//=================================================================================================

void BRepGraph_Builder::Append(BRepGraph& theGraph, const TopoDS_Shape& theShape, bool theParallel)
{
  if (theShape.IsNull())
    return;

  // Extend incidence storage incrementally.
  BRepGraphInc_Populate::Append(theGraph.myData->myIncStorage, theShape, theParallel);

  // Re-derive legacy stores from updated incidence storage.
  // Clear existing legacy stores first.
  theGraph.myData->mySolids.Clear();
  theGraph.myData->myShells.Clear();
  theGraph.myData->myFaces.Clear();
  theGraph.myData->myWires.Clear();
  theGraph.myData->myEdges.Clear();
  theGraph.myData->myVertices.Clear();
  theGraph.myData->myCompounds.Clear();
  theGraph.myData->myCompSolids.Clear();
  theGraph.myData->myEdgeToWires.Clear();

  // Re-copy TShape→NodeId and OriginalShapes.
  theGraph.myData->myTShapeToDefId.Clear();
  const BRepGraphInc_Storage& aStorage = theGraph.myData->myIncStorage;
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

  deriveLegacyFromIncidence(theGraph);

  theGraph.myData->myIsDone = true;
}
