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

#include <BRepGraph.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_ProgramError.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

namespace
{

//! Per-vertex data extracted from TopoDS in Phase 2 (parallel).
struct ExtractedVertex
{
  TopoDS_Vertex Shape;
  gp_Pnt        Point;
  double        Tolerance = 0.0;
};

//! Per-edge data extracted from TopoDS in Phase 2 (parallel).
struct ExtractedEdge
{
  TopoDS_Edge          Shape;
  Handle(Geom_Curve)   Curve3d;
  double               ParamFirst   = 0.0;
  double               ParamLast    = 0.0;
  double               Tolerance    = 0.0;
  bool                 IsDegenerate = false;
  ExtractedVertex      StartVertex;
  ExtractedVertex      EndVertex;
  TopAbs_Orientation   OrientationInWire = TopAbs_FORWARD;
  Handle(Geom2d_Curve) PCurve2d;
  double               PCFirst = 0.0;
  double               PCLast  = 0.0;
};

//! Per-wire data extracted from TopoDS in Phase 2 (parallel).
struct ExtractedWire
{
  TopoDS_Wire                       Shape;
  bool                              IsOuter = false;
  NCollection_Vector<ExtractedEdge> Edges;
};

//! All data extracted from a single face -- filled in parallel, consumed sequentially.
struct FaceLocalData
{
  // Context from Phase 1 (sequential hierarchy scan).
  TopoDS_Face        Face;
  TopLoc_Location    ParentGlobalLoc;
  BRepGraph_UsageId  ParentShellUsage;

  // Geometry extracted in Phase 2 (parallel).
  Handle(Geom_Surface)                Surface;
  Handle(Poly_Triangulation)          Triangulation;
  double                              Tolerance   = 0.0;
  TopAbs_Orientation                  Orientation = TopAbs_FORWARD;
  NCollection_Vector<ExtractedWire>   Wires;
};

//! Extract per-face geometry/topology data from TopoDS.
void extractFaceData(FaceLocalData& theData)
{
  const TopoDS_Face& aFace = theData.Face;

  theData.Surface = BRep_Tool::Surface(aFace);
  TopLoc_Location aFaceLoc;
  theData.Triangulation = BRep_Tool::Triangulation(aFace, aFaceLoc);
  theData.Tolerance     = BRep_Tool::Tolerance(aFace);
  theData.Orientation   = aFace.Orientation();

  // Use a FORWARD-oriented face for wire/edge iteration so that edge orientations
  // and traversal order are stored relative to the face TShape, not composed with
  // the face's orientation in the parent shell/solid.  During reconstruction the
  // face orientation is re-applied, so storing TShape-relative data avoids
  // double-composition.
  const TopoDS_Face aForwardFace = TopoDS::Face(aFace.Oriented(TopAbs_FORWARD));
  const TopoDS_Wire anOuterWire  = BRepTools::OuterWire(aForwardFace);

  for (TopoDS_Iterator aWireIt(aForwardFace); aWireIt.More(); aWireIt.Next())
  {
    if (aWireIt.Value().ShapeType() != TopAbs_WIRE)
      continue;
    const TopoDS_Wire& aWire = TopoDS::Wire(aWireIt.Value());

    ExtractedWire aWireData;
    aWireData.Shape   = aWire;
    aWireData.IsOuter = aWire.IsSame(anOuterWire);

    for (BRepTools_WireExplorer anEdgeExp(aWire, aForwardFace); anEdgeExp.More(); anEdgeExp.Next())
    {
      const TopoDS_Edge& anEdge = anEdgeExp.Current();

      ExtractedEdge anEdgeData;
      anEdgeData.Shape             = anEdge;
      anEdgeData.Tolerance         = BRep_Tool::Tolerance(anEdge);
      anEdgeData.IsDegenerate      = BRep_Tool::Degenerated(anEdge);
      anEdgeData.OrientationInWire = anEdge.Orientation();

      double aFirst = 0.0, aLast = 0.0;
      anEdgeData.Curve3d    = BRep_Tool::Curve(anEdge, aFirst, aLast);
      anEdgeData.ParamFirst = aFirst;
      anEdgeData.ParamLast  = aLast;

      TopoDS_Vertex aVFirst, aVLast;
      TopExp::Vertices(anEdge, aVFirst, aVLast);

      if (!aVFirst.IsNull())
      {
        anEdgeData.StartVertex.Shape     = aVFirst;
        anEdgeData.StartVertex.Point     = BRep_Tool::Pnt(aVFirst);
        anEdgeData.StartVertex.Tolerance = BRep_Tool::Tolerance(aVFirst);
      }
      if (!aVLast.IsNull())
      {
        anEdgeData.EndVertex.Shape     = aVLast;
        anEdgeData.EndVertex.Point     = BRep_Tool::Pnt(aVLast);
        anEdgeData.EndVertex.Tolerance = BRep_Tool::Tolerance(aVLast);
      }

      double aPCFirst = 0.0, aPCLast = 0.0;
      anEdgeData.PCurve2d = BRep_Tool::CurveOnSurface(anEdge, aForwardFace, aPCFirst, aPCLast);
      anEdgeData.PCFirst  = aPCFirst;
      anEdgeData.PCLast   = aPCLast;

      aWireData.Edges.Append(anEdgeData);
    }

    theData.Wires.Append(aWireData);
  }
}

} // anonymous namespace

//=================================================================================================

BRepGraph::BRepGraph()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      mySolidDefs(16, myAllocator),
      myShellDefs(16, myAllocator),
      myFaceDefs(128, myAllocator),
      myWireDefs(128, myAllocator),
      myEdgeDefs(256, myAllocator),
      myVertexDefs(256, myAllocator),
      mySolidUsages(16, myAllocator),
      myShellUsages(16, myAllocator),
      myFaceUsages(128, myAllocator),
      myWireUsages(128, myAllocator),
      myEdgeUsages(256, myAllocator),
      myVertexUsages(256, myAllocator),
      mySurfaces(64, myAllocator),
      myCurves(64, myAllocator),
      myPCurves(128, myAllocator),
      mySurfRegistry(100, myAllocator),
      myCurveRegistry(100, myAllocator),
      myTShapeToDefId(100, myAllocator),
      myUIDToNodeId(BRepGraph_UID::Hasher(), 100, myAllocator),
      myHistory(256, myAllocator),
      myIsDone(false)
{
}

//=================================================================================================

BRepGraph::BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc)
    : myAllocator(!theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator()),
      mySolidDefs(16, myAllocator),
      myShellDefs(16, myAllocator),
      myFaceDefs(128, myAllocator),
      myWireDefs(128, myAllocator),
      myEdgeDefs(256, myAllocator),
      myVertexDefs(256, myAllocator),
      mySolidUsages(16, myAllocator),
      myShellUsages(16, myAllocator),
      myFaceUsages(128, myAllocator),
      myWireUsages(128, myAllocator),
      myEdgeUsages(256, myAllocator),
      myVertexUsages(256, myAllocator),
      mySurfaces(64, myAllocator),
      myCurves(64, myAllocator),
      myPCurves(128, myAllocator),
      mySurfRegistry(100, myAllocator),
      myCurveRegistry(100, myAllocator),
      myTShapeToDefId(100, myAllocator),
      myUIDToNodeId(BRepGraph_UID::Hasher(), 100, myAllocator),
      myHistory(256, myAllocator),
      myIsDone(false)
{
}

//=================================================================================================

void BRepGraph::Build(const TopoDS_Shape& theShape, bool theParallel)
{
  // Phase 0: Clear all storage, increment generation.
  mySolidDefs.Clear();
  myShellDefs.Clear();
  myFaceDefs.Clear();
  myWireDefs.Clear();
  myEdgeDefs.Clear();
  myVertexDefs.Clear();
  mySolidUsages.Clear();
  myShellUsages.Clear();
  myFaceUsages.Clear();
  myWireUsages.Clear();
  myEdgeUsages.Clear();
  myVertexUsages.Clear();
  mySurfaces.Clear();
  myCurves.Clear();
  myPCurves.Clear();
  myOutRelEdges.Clear();
  myInRelEdges.Clear();
  mySurfRegistry.Clear();
  myCurveRegistry.Clear();
  myTShapeToDefId.Clear();
  myNodeToUID.Clear();
  myUIDToNodeId.Clear();
  myEdgeToWires.Clear();
  myHistory.Clear();
  myDerivedToOriginal.Clear();
  myOriginalToDerived.Clear();
  myOriginalShapes.Clear();
  myCurrentShapes.Clear();
  ++myGeneration;
  myIsDone = false;

  // Early exit for null shapes.
  if (theShape.IsNull())
    return;

  // Phase 1 (sequential): Explore hierarchy, create Solid/Shell defs + usages,
  // collect face contexts for parallel extraction.
  NCollection_Vector<FaceLocalData> aFaceData(128, myAllocator);

  bool aHasSolids = false;
  for (TopExp_Explorer aSolidExp(theShape, TopAbs_SOLID); aSolidExp.More(); aSolidExp.Next())
  {
    aHasSolids                 = true;
    const TopoDS_Solid& aSolid = TopoDS::Solid(aSolidExp.Current());

    // Create SolidDef.
    BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Appended();
    const int aSolidDefIdx = mySolidDefs.Length() - 1;
    aSolidDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Solid, aSolidDefIdx);
    allocateUID(aSolidDef.Id);

    // Create SolidUsage.
    BRepGraph_TopoNode::SolidUsage& aSolidUsage = mySolidUsages.Appended();
    const int aSolidUsageIdx = mySolidUsages.Length() - 1;
    aSolidUsage.UsageId       = BRepGraph_UsageId(BRepGraph_NodeKind::Solid, aSolidUsageIdx);
    aSolidUsage.DefId         = aSolidDef.Id;
    aSolidUsage.LocalLocation = aSolid.Location();
    aSolidUsage.GlobalLocation = aSolid.Location();
    aSolidUsage.Orientation   = aSolid.Orientation();
    aSolidDef.Usages.Append(aSolidUsage.UsageId);
    myOriginalShapes.Bind(aSolidDef.Id, aSolid);

    for (TopoDS_Iterator aShellIt(aSolid); aShellIt.More(); aShellIt.Next())
    {
      if (aShellIt.Value().ShapeType() != TopAbs_SHELL)
        continue;

      const TopoDS_Shell& aShell = TopoDS::Shell(aShellIt.Value());

      // Create ShellDef.
      BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Appended();
      const int aShellDefIdx = myShellDefs.Length() - 1;
      aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Shell, aShellDefIdx);
      allocateUID(aShellDef.Id);

      // Create ShellUsage.
      BRepGraph_TopoNode::ShellUsage& aShellUsage = myShellUsages.Appended();
      const int aShellUsageIdx = myShellUsages.Length() - 1;
      aShellUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Shell, aShellUsageIdx);
      aShellUsage.DefId          = aShellDef.Id;
      aShellUsage.LocalLocation  = aShell.Location();
      aShellUsage.GlobalLocation = aSolidUsage.GlobalLocation * aShell.Location();
      aShellUsage.Orientation    = aShell.Orientation();
      aShellUsage.ParentUsage    = aSolidUsage.UsageId;
      aShellDef.Usages.Append(aShellUsage.UsageId);
      myOriginalShapes.Bind(aShellDef.Id, aShell);

      aSolidUsage.ShellUsages.Append(aShellUsage.UsageId);

      for (TopoDS_Iterator aFaceIt(aShell); aFaceIt.More(); aFaceIt.Next())
      {
        if (aFaceIt.Value().ShapeType() != TopAbs_FACE)
          continue;
        FaceLocalData& aData     = aFaceData.Appended();
        aData.Face               = TopoDS::Face(aFaceIt.Value());
        aData.ParentGlobalLoc    = aShellUsage.GlobalLocation;
        aData.ParentShellUsage   = aShellUsage.UsageId;
      }
    }
  }

  if (!aHasSolids)
  {
    bool aHasShells = false;
    for (TopExp_Explorer aShellExp(theShape, TopAbs_SHELL, TopAbs_SOLID); aShellExp.More();
         aShellExp.Next())
    {
      aHasShells                 = true;
      const TopoDS_Shell& aShell = TopoDS::Shell(aShellExp.Current());

      BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Appended();
      const int aShellDefIdx = myShellDefs.Length() - 1;
      aShellDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Shell, aShellDefIdx);
      allocateUID(aShellDef.Id);

      BRepGraph_TopoNode::ShellUsage& aShellUsage = myShellUsages.Appended();
      const int aShellUsageIdx = myShellUsages.Length() - 1;
      aShellUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Shell, aShellUsageIdx);
      aShellUsage.DefId          = aShellDef.Id;
      aShellUsage.LocalLocation  = aShell.Location();
      aShellUsage.GlobalLocation = aShell.Location();
      aShellUsage.Orientation    = aShell.Orientation();
      aShellDef.Usages.Append(aShellUsage.UsageId);
      myOriginalShapes.Bind(aShellDef.Id, aShell);

      for (TopoDS_Iterator aFaceIt(aShell); aFaceIt.More(); aFaceIt.Next())
      {
        if (aFaceIt.Value().ShapeType() != TopAbs_FACE)
          continue;
        FaceLocalData& aData     = aFaceData.Appended();
        aData.Face               = TopoDS::Face(aFaceIt.Value());
        aData.ParentGlobalLoc    = aShellUsage.GlobalLocation;
        aData.ParentShellUsage   = aShellUsage.UsageId;
      }
    }

    if (!aHasShells)
    {
      for (TopExp_Explorer aFaceExp(theShape, TopAbs_FACE, TopAbs_SHELL); aFaceExp.More();
           aFaceExp.Next())
      {
        FaceLocalData& aData = aFaceData.Appended();
        aData.Face           = TopoDS::Face(aFaceExp.Current());
      }
    }
  }

  // Phase 2 (parallel): Extract per-face geometry/topology data.
  OSD_Parallel::For(
    0,
    aFaceData.Length(),
    [&](int theIndex) { extractFaceData(aFaceData.ChangeValue(theIndex)); },
    !theParallel);

  // Pre-size maps.
  const int aNbFacesEst   = aFaceData.Length();
  const int anEstEntities = aNbFacesEst * 10;
  myTShapeToDefId.ReSize(anEstEntities);
  if (myUIDEnabled)
  {
    myNodeToUID.ReSize(anEstEntities);
    myUIDToNodeId.ReSize(anEstEntities);
  }

  // Phase 3 (sequential): Register definitions and usages from pre-extracted data.
  for (int aFaceDataIdx = 0; aFaceDataIdx < aFaceData.Length(); ++aFaceDataIdx)
  {
    const FaceLocalData& aData    = aFaceData.Value(aFaceDataIdx);
    const TopoDS_Face&   aCurFace = aData.Face;

    // Create or reuse FaceDef.
    void*                   aFaceTShapeKey   = aCurFace.TShape().get();
    const BRepGraph_NodeId* anExistingFaceDef = myTShapeToDefId.Seek(aFaceTShapeKey);

    BRepGraph_NodeId aFaceDefId;
    if (anExistingFaceDef != nullptr)
    {
      aFaceDefId = *anExistingFaceDef;
    }
    else
    {
      BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Appended();
      const int aFaceDefIdx = myFaceDefs.Length() - 1;
      aFaceDef.Id        = BRepGraph_NodeId(BRepGraph_NodeKind::Face, aFaceDefIdx);
      aFaceDef.Tolerance = aData.Tolerance;
      allocateUID(aFaceDef.Id);

      // Register surface.
      aFaceDef.SurfNodeId = registerSurface(aData.Surface, aData.Triangulation);
      if (aFaceDef.SurfNodeId.IsValid())
      {
        mySurfaces.ChangeValue(aFaceDef.SurfNodeId.Index).FaceDefUsers.Append(aFaceDef.Id);
      }

      myTShapeToDefId.Bind(aFaceTShapeKey, aFaceDef.Id);
      myOriginalShapes.Bind(aFaceDef.Id, aCurFace);
      aFaceDefId = aFaceDef.Id;
    }

    // Always create FaceUsage.
    BRepGraph_TopoNode::FaceUsage& aFaceUsage = myFaceUsages.Appended();
    const int aFaceUsageIdx = myFaceUsages.Length() - 1;
    aFaceUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Face, aFaceUsageIdx);
    aFaceUsage.DefId          = aFaceDefId;
    aFaceUsage.LocalLocation  = aCurFace.Location();
    aFaceUsage.GlobalLocation = aData.ParentGlobalLoc * aCurFace.Location();
    aFaceUsage.Orientation    = aData.Orientation;
    aFaceUsage.ParentUsage    = aData.ParentShellUsage;
    myFaceDefs.ChangeValue(aFaceDefId.Index).Usages.Append(aFaceUsage.UsageId);

    // Link to parent shell usage.
    if (aData.ParentShellUsage.IsValid())
    {
      myShellUsages.ChangeValue(aData.ParentShellUsage.Index).FaceUsages.Append(aFaceUsage.UsageId);
    }

    // Process wires.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      // Create WireDef (wires are not typically shared, always new).
      BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Appended();
      const int aWireDefIdx = myWireDefs.Length() - 1;
      aWireDef.Id = BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aWireDefIdx);
      allocateUID(aWireDef.Id);
      myOriginalShapes.Bind(aWireDef.Id, aWireData.Shape);

      // Create WireUsage.
      BRepGraph_TopoNode::WireUsage& aWireUsage = myWireUsages.Appended();
      const int aWireUsageIdx = myWireUsages.Length() - 1;
      aWireUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Wire, aWireUsageIdx);
      aWireUsage.DefId          = aWireDef.Id;
      aWireUsage.LocalLocation  = aWireData.Shape.Location();
      aWireUsage.GlobalLocation = aFaceUsage.GlobalLocation * aWireData.Shape.Location();
      aWireUsage.Orientation    = aWireData.Shape.Orientation();
      aWireUsage.ParentUsage    = aFaceUsage.UsageId;
      aWireUsage.OwnerFaceUsage = aFaceUsage.UsageId;
      aWireDef.Usages.Append(aWireUsage.UsageId);

      if (aWireData.IsOuter)
      {
        aFaceUsage.OuterWireUsage = aWireUsage.UsageId;
      }
      else
      {
        aFaceUsage.InnerWireUsages.Append(aWireUsage.UsageId);
      }

      // Pre-allocate ordered edges array.
      const int aNbWireEdges = aWireData.Edges.Length();
      if (aNbWireEdges > 0)
        aWireDef.OrderedEdges.Resize(0, aNbWireEdges - 1, false);

      BRepGraph_NodeId aFirstVertexDefId;
      BRepGraph_NodeId aLastVertexDefId;

      for (int anEdgeIter = 0; anEdgeIter < aWireData.Edges.Length(); ++anEdgeIter)
      {
        const ExtractedEdge& anEdgeData = aWireData.Edges.Value(anEdgeIter);
        const TopoDS_Edge&   anEdge     = anEdgeData.Shape;

        // Dedup edge definition by TShape.
        void*                   aTShapeKey         = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdgeDefId = myTShapeToDefId.Seek(aTShapeKey);

        BRepGraph_NodeId anEdgeDefId;

        if (anExistingEdgeDefId != nullptr)
        {
          anEdgeDefId = *anExistingEdgeDefId;
        }
        else
        {
          BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Appended();
          const int anEdgeDefIdx = myEdgeDefs.Length() - 1;
          anEdgeDef.Id          = BRepGraph_NodeId(BRepGraph_NodeKind::Edge, anEdgeDefIdx);
          anEdgeDef.Tolerance   = anEdgeData.Tolerance;
          anEdgeDef.IsDegenerate = anEdgeData.IsDegenerate;
          anEdgeDef.ParamFirst  = anEdgeData.ParamFirst;
          anEdgeDef.ParamLast   = anEdgeData.ParamLast;
          allocateUID(anEdgeDef.Id);
          myOriginalShapes.Bind(anEdgeDef.Id, anEdge);

          if (!anEdgeData.Curve3d.IsNull())
          {
            anEdgeDef.CurveNodeId = registerCurve(anEdgeData.Curve3d);
            if (anEdgeDef.CurveNodeId.IsValid())
            {
              myCurves.ChangeValue(anEdgeDef.CurveNodeId.Index).EdgeDefUsers.Append(anEdgeDef.Id);
            }
          }

          // Vertex definitions.
          auto processVertexDef = [&](const ExtractedVertex& theVtxData) -> BRepGraph_NodeId {
            if (theVtxData.Shape.IsNull())
              return BRepGraph_NodeId();

            void*                   aVTShapeKey   = theVtxData.Shape.TShape().get();
            const BRepGraph_NodeId* anExistingVtx = myTShapeToDefId.Seek(aVTShapeKey);
            if (anExistingVtx != nullptr)
              return *anExistingVtx;

            BRepGraph_TopoNode::VertexDef& aVtxDef = myVertexDefs.Appended();
            const int aVtxDefIdx = myVertexDefs.Length() - 1;
            aVtxDef.Id        = BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVtxDefIdx);
            aVtxDef.Point     = theVtxData.Point;
            aVtxDef.Tolerance = theVtxData.Tolerance;
            allocateUID(aVtxDef.Id);
            myOriginalShapes.Bind(aVtxDef.Id, theVtxData.Shape);

            myTShapeToDefId.Bind(aVTShapeKey, aVtxDef.Id);
            return aVtxDef.Id;
          };

          anEdgeDef.StartVertexDefId = processVertexDef(anEdgeData.StartVertex);
          anEdgeDef.EndVertexDefId   = processVertexDef(anEdgeData.EndVertex);

          myTShapeToDefId.Bind(aTShapeKey, anEdgeDef.Id);
          anEdgeDefId = anEdgeDef.Id;
        }

        // Always create EdgeUsage.
        BRepGraph_TopoNode::EdgeUsage& anEdgeUsage = myEdgeUsages.Appended();
        const int anEdgeUsageIdx = myEdgeUsages.Length() - 1;
        anEdgeUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Edge, anEdgeUsageIdx);
        anEdgeUsage.DefId          = anEdgeDefId;
        anEdgeUsage.LocalLocation  = anEdge.Location();
        anEdgeUsage.GlobalLocation = aWireUsage.GlobalLocation * anEdge.Location();
        anEdgeUsage.Orientation    = anEdgeData.OrientationInWire;
        anEdgeUsage.ParentUsage    = aWireUsage.UsageId;
        myEdgeDefs.ChangeValue(anEdgeDefId.Index).Usages.Append(anEdgeUsage.UsageId);

        // Create VertexUsages for this edge usage.
        auto createVertexUsage = [&](BRepGraph_NodeId              theVtxDefId,
                                     const ExtractedVertex&        theVtxData,
                                     const TopLoc_Location&        theEdgeGlobalLoc) -> BRepGraph_UsageId
        {
          if (!theVtxDefId.IsValid())
            return BRepGraph_UsageId();

          BRepGraph_TopoNode::VertexUsage& aVtxUsage = myVertexUsages.Appended();
          const int aVtxUsageIdx = myVertexUsages.Length() - 1;
          aVtxUsage.UsageId        = BRepGraph_UsageId(BRepGraph_NodeKind::Vertex, aVtxUsageIdx);
          aVtxUsage.DefId          = theVtxDefId;
          aVtxUsage.LocalLocation  = theVtxData.Shape.Location();
          aVtxUsage.GlobalLocation = theEdgeGlobalLoc * theVtxData.Shape.Location();
          aVtxUsage.Orientation    = theVtxData.Shape.Orientation();
          aVtxUsage.ParentUsage    = anEdgeUsage.UsageId;
          aVtxUsage.TransformedPoint = theVtxData.Point;
          myVertexDefs.ChangeValue(theVtxDefId.Index).Usages.Append(aVtxUsage.UsageId);
          return aVtxUsage.UsageId;
        };

        const BRepGraph_TopoNode::EdgeDef& anEdgeDefRef = myEdgeDefs.Value(anEdgeDefId.Index);
        anEdgeUsage.StartVertexUsage = createVertexUsage(
          anEdgeDefRef.StartVertexDefId, anEdgeData.StartVertex, anEdgeUsage.GlobalLocation);
        anEdgeUsage.EndVertexUsage = createVertexUsage(
          anEdgeDefRef.EndVertexDefId, anEdgeData.EndVertex, anEdgeUsage.GlobalLocation);

        aWireUsage.EdgeUsages.Append(anEdgeUsage.UsageId);

        // PCurve from pre-extracted data.
        if (!anEdgeData.PCurve2d.IsNull())
        {
          BRepGraph_NodeId aPCurveId = createPCurveNode(anEdgeData.PCurve2d,
                                                        anEdgeDefId,
                                                        aFaceDefId,
                                                        anEdgeData.PCFirst,
                                                        anEdgeData.PCLast);

          BRepGraph_TopoNode::EdgeDef::PCurveEntry aPCEntry;
          aPCEntry.PCurveNodeId    = aPCurveId;
          aPCEntry.FaceDefId       = aFaceDefId;
          aPCEntry.EdgeOrientation = anEdgeData.OrientationInWire;
          myEdgeDefs.ChangeValue(anEdgeDefId.Index).PCurves.Append(aPCEntry);
        }

        // WireDef::EdgeEntry.
        BRepGraph_TopoNode::WireDef::EdgeEntry aWEEntry;
        aWEEntry.EdgeDefId         = anEdgeDefId;
        aWEEntry.OrientationInWire = anEdgeData.OrientationInWire;
        aWireDef.OrderedEdges.SetValue(anEdgeIter, aWEEntry);

        // Populate edge-to-wire reverse index.
        if (!myEdgeToWires.IsBound(anEdgeDefId.Index))
          myEdgeToWires.Bind(anEdgeDefId.Index, NCollection_Vector<int>());
        myEdgeToWires.ChangeFind(anEdgeDefId.Index).Append(aWireDefIdx);

        // Track first/last vertex for closure check.
        if (!aFirstVertexDefId.IsValid())
        {
          if (anEdgeData.OrientationInWire == TopAbs_FORWARD)
            aFirstVertexDefId = anEdgeDefRef.StartVertexDefId;
          else
            aFirstVertexDefId = anEdgeDefRef.EndVertexDefId;
        }
        {
          if (anEdgeData.OrientationInWire == TopAbs_FORWARD)
            aLastVertexDefId = anEdgeDefRef.EndVertexDefId;
          else
            aLastVertexDefId = anEdgeDefRef.StartVertexDefId;
        }
      }

      // Set wire closure.
      aWireDef.IsClosed =
        aFirstVertexDefId.IsValid() && aLastVertexDefId.IsValid()
        && aFirstVertexDefId == aLastVertexDefId;
    }
  }

  // Phase 4: Set IsMultiLocated flags.
  for (int aSurfIdx = 0; aSurfIdx < mySurfaces.Length(); ++aSurfIdx)
  {
    mySurfaces.ChangeValue(aSurfIdx).IsMultiLocated =
      (mySurfaces.Value(aSurfIdx).FaceDefUsers.Length() > 1);
  }
  for (int aCurveIdx = 0; aCurveIdx < myCurves.Length(); ++aCurveIdx)
  {
    myCurves.ChangeValue(aCurveIdx).IsMultiLocated =
      (myCurves.Value(aCurveIdx).EdgeDefUsers.Length() > 1);
  }

  myIsDone = true;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::registerSurface(const Handle(Geom_Surface)&       theSurf,
                                            const Handle(Poly_Triangulation)& theTri)
{
  if (theSurf.IsNull())
    return BRepGraph_NodeId();

  void*      aKey       = theSurf.get();
  const int* anExisting = mySurfRegistry.Seek(aKey);
  if (anExisting != nullptr)
    return BRepGraph_NodeId(BRepGraph_NodeKind::Surface, *anExisting);

  BRepGraph_GeomNode::Surf& aSurfNode = mySurfaces.Appended();
  const int                 aSurfIdx  = mySurfaces.Length() - 1;
  aSurfNode.Id                        = BRepGraph_NodeId(BRepGraph_NodeKind::Surface, aSurfIdx);
  aSurfNode.Surface                   = theSurf;
  aSurfNode.Triangulation             = theTri;
  aSurfNode.IsMultiLocated            = false;
  allocateUID(aSurfNode.Id);

  mySurfRegistry.Add(aKey, aSurfIdx);
  return aSurfNode.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::registerCurve(const Handle(Geom_Curve)& theCrv)
{
  if (theCrv.IsNull())
    return BRepGraph_NodeId();

  void*      aKey       = theCrv.get();
  const int* anExisting = myCurveRegistry.Seek(aKey);
  if (anExisting != nullptr)
    return BRepGraph_NodeId(BRepGraph_NodeKind::Curve, *anExisting);

  BRepGraph_GeomNode::Curve& aCurveNode = myCurves.Appended();
  const int                  aCurveIdx  = myCurves.Length() - 1;
  aCurveNode.Id                         = BRepGraph_NodeId(BRepGraph_NodeKind::Curve, aCurveIdx);
  aCurveNode.CurveGeom                  = theCrv;
  aCurveNode.IsMultiLocated             = false;
  allocateUID(aCurveNode.Id);

  myCurveRegistry.Add(aKey, aCurveIdx);
  return aCurveNode.Id;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::createPCurveNode(const Handle(Geom2d_Curve)& theCrv2d,
                                             BRepGraph_NodeId            theEdgeDef,
                                             BRepGraph_NodeId            theFaceDef,
                                             double                      theFirst,
                                             double                      theLast)
{
  BRepGraph_GeomNode::PCurve& aNode = myPCurves.Appended();
  const int                   aIdx  = myPCurves.Length() - 1;
  aNode.Id                          = BRepGraph_NodeId(BRepGraph_NodeKind::PCurve, aIdx);
  aNode.Curve2d                     = theCrv2d;
  aNode.EdgeContext                 = theEdgeDef;
  aNode.FaceContext                 = theFaceDef;
  aNode.ParamFirst                  = theFirst;
  aNode.ParamLast                   = theLast;
  allocateUID(aNode.Id);

  return aNode.Id;
}

//=================================================================================================

BRepGraph_UID BRepGraph::allocateUID(BRepGraph_NodeId theNodeId)
{
  if (!myUIDEnabled)
    return BRepGraph_UID();

  const size_t  aCounter = myNextUIDCounter.fetch_add(1, std::memory_order_relaxed);
  BRepGraph_UID aUID(theNodeId.Kind, aCounter, myGeneration);
  myNodeToUID.Bind(theNodeId, aUID);
  myUIDToNodeId.Bind(aUID, theNodeId);
  return aUID;
}

//=================================================================================================

BRepGraph_NodeCache* BRepGraph::mutableCache(BRepGraph_NodeId theNode)
{
  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Solid:
      return &mySolidDefs.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Shell:
      return &myShellDefs.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Face:
      return &myFaceDefs.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Wire:
      return &myWireDefs.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Edge:
      return &myEdgeDefs.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Vertex:
      return &myVertexDefs.ChangeValue(theNode.Index).Cache;
    default:
      return nullptr;
  }
}

//=================================================================================================

bool BRepGraph::IsDone() const
{
  return myIsDone;
}

//=================================================================================================

int BRepGraph::NbSolidDefs() const { return mySolidDefs.Length(); }
int BRepGraph::NbShellDefs() const { return myShellDefs.Length(); }
int BRepGraph::NbFaceDefs() const { return myFaceDefs.Length(); }
int BRepGraph::NbWireDefs() const { return myWireDefs.Length(); }
int BRepGraph::NbEdgeDefs() const { return myEdgeDefs.Length(); }
int BRepGraph::NbVertexDefs() const { return myVertexDefs.Length(); }
int BRepGraph::NbSurfaces() const { return mySurfaces.Length(); }
int BRepGraph::NbCurves() const { return myCurves.Length(); }
int BRepGraph::NbPCurves() const { return myPCurves.Length(); }

int BRepGraph::NbSolidUsages() const { return mySolidUsages.Length(); }
int BRepGraph::NbShellUsages() const { return myShellUsages.Length(); }
int BRepGraph::NbFaceUsages() const { return myFaceUsages.Length(); }
int BRepGraph::NbWireUsages() const { return myWireUsages.Length(); }
int BRepGraph::NbEdgeUsages() const { return myEdgeUsages.Length(); }
int BRepGraph::NbVertexUsages() const { return myVertexUsages.Length(); }

//=================================================================================================

const BRepGraph_TopoNode::SolidDef& BRepGraph::SolidDef(int theIdx) const
{ return mySolidDefs.Value(theIdx); }

const BRepGraph_TopoNode::ShellDef& BRepGraph::ShellDef(int theIdx) const
{ return myShellDefs.Value(theIdx); }

const BRepGraph_TopoNode::FaceDef& BRepGraph::FaceDef(int theIdx) const
{ return myFaceDefs.Value(theIdx); }

const BRepGraph_TopoNode::WireDef& BRepGraph::WireDef(int theIdx) const
{ return myWireDefs.Value(theIdx); }

const BRepGraph_TopoNode::EdgeDef& BRepGraph::EdgeDef(int theIdx) const
{ return myEdgeDefs.Value(theIdx); }

const BRepGraph_TopoNode::VertexDef& BRepGraph::VertexDef(int theIdx) const
{ return myVertexDefs.Value(theIdx); }

const BRepGraph_GeomNode::Surf& BRepGraph::Surf(int theIdx) const
{ return mySurfaces.Value(theIdx); }

const BRepGraph_GeomNode::Curve& BRepGraph::Curve(int theIdx) const
{ return myCurves.Value(theIdx); }

const BRepGraph_GeomNode::PCurve& BRepGraph::PCurve(int theIdx) const
{ return myPCurves.Value(theIdx); }

//=================================================================================================

const BRepGraph_TopoNode::SolidUsage& BRepGraph::SolidUsage(int theIdx) const
{ return mySolidUsages.Value(theIdx); }

const BRepGraph_TopoNode::ShellUsage& BRepGraph::ShellUsage(int theIdx) const
{ return myShellUsages.Value(theIdx); }

const BRepGraph_TopoNode::FaceUsage& BRepGraph::FaceUsage(int theIdx) const
{ return myFaceUsages.Value(theIdx); }

const BRepGraph_TopoNode::WireUsage& BRepGraph::WireUsage(int theIdx) const
{ return myWireUsages.Value(theIdx); }

const BRepGraph_TopoNode::EdgeUsage& BRepGraph::EdgeUsage(int theIdx) const
{ return myEdgeUsages.Value(theIdx); }

const BRepGraph_TopoNode::VertexUsage& BRepGraph::VertexUsage(int theIdx) const
{ return myVertexUsages.Value(theIdx); }

//=================================================================================================

const NCollection_Vector<BRepGraph_UsageId>& BRepGraph::UsagesOf(BRepGraph_NodeId theDefId) const
{
  static const NCollection_Vector<BRepGraph_UsageId> THE_EMPTY;
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr)
    return THE_EMPTY;
  return aDef->Usages;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::DefOf(BRepGraph_UsageId theUsageId) const
{
  if (!theUsageId.IsValid())
    return BRepGraph_NodeId();

  switch (theUsageId.Kind)
  {
    case BRepGraph_NodeKind::Solid:  return mySolidUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Shell:  return myShellUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Face:   return myFaceUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Wire:   return myWireUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Edge:   return myEdgeUsages.Value(theUsageId.Index).DefId;
    case BRepGraph_NodeKind::Vertex: return myVertexUsages.Value(theUsageId.Index).DefId;
    default: return BRepGraph_NodeId();
  }
}

//=================================================================================================

const BRepGraph_TopoNode::BaseDef* BRepGraph::TopoDef(BRepGraph_NodeId theId) const
{
  if (!theId.IsValid())
    return nullptr;

  switch (theId.Kind)
  {
    case BRepGraph_NodeKind::Solid:
      return theId.Index < mySolidDefs.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&mySolidDefs.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Shell:
      return theId.Index < myShellDefs.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&myShellDefs.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Face:
      return theId.Index < myFaceDefs.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&myFaceDefs.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Wire:
      return theId.Index < myWireDefs.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&myWireDefs.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Edge:
      return theId.Index < myEdgeDefs.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&myEdgeDefs.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Vertex:
      return theId.Index < myVertexDefs.Length()
               ? static_cast<const BRepGraph_TopoNode::BaseDef*>(&myVertexDefs.Value(theId.Index))
               : nullptr;
    default:
      return nullptr;
  }
}

//=================================================================================================

size_t BRepGraph::NbNodes() const
{
  return static_cast<size_t>(mySolidDefs.Length()) + static_cast<size_t>(myShellDefs.Length())
         + static_cast<size_t>(myFaceDefs.Length()) + static_cast<size_t>(myWireDefs.Length())
         + static_cast<size_t>(myEdgeDefs.Length()) + static_cast<size_t>(myVertexDefs.Length())
         + static_cast<size_t>(mySurfaces.Length()) + static_cast<size_t>(myCurves.Length())
         + static_cast<size_t>(myPCurves.Length());
}

//=================================================================================================

void BRepGraph::SetUIDEnabled(bool theVal) { myUIDEnabled = theVal; }
bool BRepGraph::IsUIDEnabled() const { return myUIDEnabled; }

BRepGraph_UID BRepGraph::UIDOf(BRepGraph_NodeId theNode) const
{
  if (!myUIDEnabled)
    return BRepGraph_UID();
  const BRepGraph_UID* aPtr = myNodeToUID.Seek(theNode);
  return aPtr != nullptr ? *aPtr : BRepGraph_UID();
}

BRepGraph_NodeId BRepGraph::NodeIdFromUID(const BRepGraph_UID& theUID) const
{
  const BRepGraph_NodeId* aPtr = myUIDToNodeId.Seek(theUID);
  return aPtr != nullptr ? *aPtr : BRepGraph_NodeId();
}

bool BRepGraph::HasUID(const BRepGraph_UID& theUID) const
{
  if (!myUIDEnabled || !theUID.IsValid())
    return false;
  if (theUID.Generation() != myGeneration)
    return false;
  return myUIDToNodeId.IsBound(theUID);
}

uint32_t BRepGraph::Generation() const { return myGeneration; }

//=================================================================================================

int BRepGraph::AddRelEdge(BRepGraph_NodeId theFrom, BRepGraph_NodeId theTo, BRepGraph_RelKind theKind)
{
  BRepGraph_RelEdge anEdge;
  anEdge.Kind   = theKind;
  anEdge.Source = theFrom;
  anEdge.Target = theTo;

  if (!myOutRelEdges.IsBound(theFrom))
    myOutRelEdges.Bind(theFrom, NCollection_Vector<BRepGraph_RelEdge>());
  NCollection_Vector<BRepGraph_RelEdge>& anOutVec = myOutRelEdges.ChangeFind(theFrom);
  anOutVec.Append(anEdge);
  const int anIdx = anOutVec.Length() - 1;

  if (!myInRelEdges.IsBound(theTo))
    myInRelEdges.Bind(theTo, NCollection_Vector<BRepGraph_RelEdge>());
  myInRelEdges.ChangeFind(theTo).Append(anEdge);

  return anIdx;
}

//=================================================================================================

void BRepGraph::RemoveRelEdges(BRepGraph_NodeId theFrom, BRepGraph_NodeId theTo, BRepGraph_RelKind theKind)
{
  NCollection_Vector<BRepGraph_RelEdge>* anOutVec = myOutRelEdges.ChangeSeek(theFrom);
  if (anOutVec != nullptr)
  {
    for (int anIdx = anOutVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anOutVec->Value(anIdx);
      if (anEdge.Kind == theKind && anEdge.Target == theTo)
      {
        if (anIdx < anOutVec->Length() - 1)
          anOutVec->ChangeValue(anIdx) = anOutVec->Value(anOutVec->Length() - 1);
        anOutVec->EraseLast();
      }
    }
  }

  NCollection_Vector<BRepGraph_RelEdge>* anInVec = myInRelEdges.ChangeSeek(theTo);
  if (anInVec != nullptr)
  {
    for (int anIdx = anInVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anInVec->Value(anIdx);
      if (anEdge.Kind == theKind && anEdge.Source == theFrom)
      {
        if (anIdx < anInVec->Length() - 1)
          anInVec->ChangeValue(anIdx) = anInVec->Value(anInVec->Length() - 1);
        anInVec->EraseLast();
      }
    }
  }
}

//=================================================================================================

int BRepGraph::NbRelEdgesFrom(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myOutRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

int BRepGraph::NbRelEdgesTo(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myInRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SurfaceOf(BRepGraph_NodeId theFaceDef) const
{
  if (theFaceDef.Kind != BRepGraph_NodeKind::Face || !theFaceDef.IsValid())
    return BRepGraph_NodeId();
  return myFaceDefs.Value(theFaceDef.Index).SurfNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::FacesOnSurface(BRepGraph_NodeId theSurf) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theSurf.Kind != BRepGraph_NodeKind::Surface || !theSurf.IsValid()
      || theSurf.Index >= mySurfaces.Length())
    return THE_EMPTY_VEC;
  return mySurfaces.Value(theSurf.Index).FaceDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::CurveOf(BRepGraph_NodeId theEdgeDef) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();
  return myEdgeDefs.Value(theEdgeDef.Index).CurveNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::EdgesOnCurve(BRepGraph_NodeId theCurve) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theCurve.Kind != BRepGraph_NodeKind::Curve || !theCurve.IsValid()
      || theCurve.Index >= myCurves.Length())
    return THE_EMPTY_VEC;
  return myCurves.Value(theCurve.Index).EdgeDefUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PCurveOf(BRepGraph_NodeId theEdgeDef, BRepGraph_NodeId theFaceDef) const
{
  if (theEdgeDef.Kind != BRepGraph_NodeKind::Edge || !theEdgeDef.IsValid())
    return BRepGraph_NodeId();

  const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theEdgeDef.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeDef.PCurves.Length(); ++aPCurveIter)
  {
    if (anEdgeDef.PCurves.Value(aPCurveIter).FaceDefId == theFaceDef)
      return anEdgeDef.PCurves.Value(aPCurveIter).PCurveNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

gp_Trsf BRepGraph::GlobalTransform(BRepGraph_UsageId theUsage) const
{
  if (!theUsage.IsValid())
    return gp_Trsf();

  switch (theUsage.Kind)
  {
    case BRepGraph_NodeKind::Solid:  return mySolidUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Shell:  return myShellUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Face:   return myFaceUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Wire:   return myWireUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Edge:   return myEdgeUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    case BRepGraph_NodeKind::Vertex: return myVertexUsages.Value(theUsage.Index).GlobalLocation.Transformation();
    default: return gp_Trsf();
  }
}

//=================================================================================================

gp_Trsf BRepGraph::GlobalTransform(BRepGraph_NodeId theDefId) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr || aDef->Usages.IsEmpty())
    return gp_Trsf();
  return GlobalTransform(aDef->Usages.Value(0));
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SameDomainFaces(BRepGraph_NodeId theFaceDef) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFaceDef.Kind != BRepGraph_NodeKind::Face || !theFaceDef.IsValid())
    return aResult;

  const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theFaceDef.Index);
  if (!aFaceDef.SurfNodeId.IsValid())
    return aResult;

  const NCollection_Vector<BRepGraph_NodeId>& aFaceDefUsers =
    mySurfaces.Value(aFaceDef.SurfNodeId.Index).FaceDefUsers;
  for (int anIdx = 0; anIdx < aFaceDefUsers.Length(); ++anIdx)
  {
    if (aFaceDefUsers.Value(anIdx) != theFaceDef)
      aResult.Append(aFaceDefUsers.Value(anIdx));
  }
  return aResult;
}

//=================================================================================================

Bnd_Box BRepGraph::BoundingBox(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return Bnd_Box();

  return aDef->Cache.BoundingBox.Get([&]() -> Bnd_Box {
    Bnd_Box aBox;
    TopoDS_Shape aShape = Shape(theNode);
    if (!aShape.IsNull())
    {
      for (TopExp_Explorer anExp(aShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
      {
        const TopoDS_Vertex& aVtx = TopoDS::Vertex(anExp.Current());
        aBox.Add(BRep_Tool::Pnt(aVtx));
      }
    }
    return aBox;
  });
}

//=================================================================================================

gp_Pnt BRepGraph::Centroid(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return gp_Pnt();

  return aDef->Cache.Centroid.Get([&]() -> gp_Pnt {
    Bnd_Box aBox = BoundingBox(theNode);
    if (aBox.IsVoid())
      return gp_Pnt();
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    return gp_Pnt((aXmin + aXmax) * 0.5, (aYmin + aYmax) * 0.5, (aZmin + aZmax) * 0.5);
  });
}

//=================================================================================================

void BRepGraph::Invalidate(BRepGraph_NodeId theNode)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateAll();
}

//=================================================================================================

void BRepGraph::InvalidateSubgraph(BRepGraph_NodeId theNode)
{
  invalidateSubgraphImpl(theNode);
}

//=================================================================================================

void BRepGraph::invalidateSubgraphImpl(BRepGraph_NodeId theNode)
{
  Invalidate(theNode);

  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Solid: {
      // Invalidate via first usage's shell usages.
      const BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aSolidDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::SolidUsage& aUsage =
          mySolidUsages.Value(aSolidDef.Usages.Value(aUsIdx).Index);
        for (int aShellIter = 0; aShellIter < aUsage.ShellUsages.Length(); ++aShellIter)
        {
          BRepGraph_NodeId aShellDefId = DefOf(aUsage.ShellUsages.Value(aShellIter));
          invalidateSubgraphImpl(aShellDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aShellDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::ShellUsage& aUsage =
          myShellUsages.Value(aShellDef.Usages.Value(aUsIdx).Index);
        for (int aFaceIter = 0; aFaceIter < aUsage.FaceUsages.Length(); ++aFaceIter)
        {
          BRepGraph_NodeId aFaceDefId = DefOf(aUsage.FaceUsages.Value(aFaceIter));
          invalidateSubgraphImpl(aFaceDefId);
        }
      }
      break;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theNode.Index);
      for (int aUsIdx = 0; aUsIdx < aFaceDef.Usages.Length(); ++aUsIdx)
      {
        const BRepGraph_TopoNode::FaceUsage& aUsage =
          myFaceUsages.Value(aFaceDef.Usages.Value(aUsIdx).Index);
        if (aUsage.OuterWireUsage.IsValid())
          invalidateSubgraphImpl(DefOf(aUsage.OuterWireUsage));
        for (int aWireIter = 0; aWireIter < aUsage.InnerWireUsages.Length(); ++aWireIter)
          invalidateSubgraphImpl(DefOf(aUsage.InnerWireUsages.Value(aWireIter)));
      }
      break;
    }
    case BRepGraph_NodeKind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theNode.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
        invalidateSubgraphImpl(aWireDef.OrderedEdges.Value(anEdgeIdx).EdgeDefId);
      break;
    }
    case BRepGraph_NodeKind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theNode.Index);
      if (anEdgeDef.StartVertexDefId.IsValid())
        Invalidate(anEdgeDef.StartVertexDefId);
      if (anEdgeDef.EndVertexDefId.IsValid())
        Invalidate(anEdgeDef.EndVertexDefId);
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph::SetUserAttribute(BRepGraph_NodeId theNode, int theKey,
                                 const BRepGraph_UserAttrPtr& theAttr)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->SetUserAttribute(theKey, theAttr);
}

BRepGraph_UserAttrPtr BRepGraph::GetUserAttribute(BRepGraph_NodeId theNode, int theKey) const
{
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef == nullptr)
    return nullptr;
  return aDef->Cache.GetUserAttribute(theKey);
}

bool BRepGraph::RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache == nullptr)
    return false;
  return aCache->RemoveUserAttribute(theKey);
}

void BRepGraph::InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateUserAttribute(theKey);
}

//=================================================================================================

int BRepGraph::NbHistoryRecords() const { return myHistory.Length(); }

const BRepGraph_HistoryRecord& BRepGraph::HistoryRecord(int theIdx) const
{ return myHistory.Value(theIdx); }

BRepGraph_NodeId BRepGraph::FindOriginal(BRepGraph_NodeId theModified) const
{
  const BRepGraph_NodeId* aPtr = myDerivedToOriginal.Seek(theModified);
  if (aPtr == nullptr)
    return theModified;
  return FindOriginal(*aPtr);
}

NCollection_Vector<BRepGraph_NodeId> BRepGraph::FindDerived(BRepGraph_NodeId theOriginal) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  const NCollection_Vector<BRepGraph_NodeId>* aDerived = myOriginalToDerived.Seek(theOriginal);
  if (aDerived == nullptr)
    return aResult;

  for (int anIdx = 0; anIdx < aDerived->Length(); ++anIdx)
  {
    const BRepGraph_NodeId& aDerivedId = aDerived->Value(anIdx);
    aResult.Append(aDerivedId);
    NCollection_Vector<BRepGraph_NodeId> aFurther = FindDerived(aDerivedId);
    for (int aFurtherIter = 0; aFurtherIter < aFurther.Length(); ++aFurtherIter)
      aResult.Append(aFurther.Value(aFurtherIter));
  }
  return aResult;
}

//=================================================================================================

void BRepGraph::ApplyModification(
  BRepGraph_NodeId                                                                  theTarget,
  std::function<NCollection_Vector<BRepGraph_NodeId>(BRepGraph&, BRepGraph_NodeId)> theModifier,
  const TCollection_AsciiString&                                                    theOpLabel)
{
  NCollection_Vector<BRepGraph_NodeId> aReplacements = theModifier(*this, theTarget);

  if (myIsHistoryEnabled)
  {
    BRepGraph_HistoryRecord aRecord;
    aRecord.OperationName  = theOpLabel;
    aRecord.SequenceNumber = myHistory.Length();
    NCollection_Vector<BRepGraph_NodeId> aReplSeq;
    for (int aReplIter = 0; aReplIter < aReplacements.Length(); ++aReplIter)
      aReplSeq.Append(aReplacements.Value(aReplIter));
    aRecord.Mapping.Bind(theTarget, aReplSeq);
    myHistory.Append(aRecord);

    for (int aReplIter = 0; aReplIter < aReplacements.Length(); ++aReplIter)
    {
      if (aReplacements.Value(aReplIter) != theTarget)
      {
        const BRepGraph_NodeId aDerived = aReplacements.Value(aReplIter);
        myDerivedToOriginal.Bind(aDerived, theTarget);
        if (!myOriginalToDerived.IsBound(theTarget))
          myOriginalToDerived.Bind(theTarget, NCollection_Vector<BRepGraph_NodeId>());
        myOriginalToDerived.ChangeFind(theTarget).Append(aDerived);
      }
    }
  }

  InvalidateSubgraph(theTarget);
}

//=================================================================================================

TopoDS_Shape BRepGraph::ReconstructShape(BRepGraph_NodeId theRoot) const
{
  return reconstructNode(theRoot);
}

TopoDS_Shape BRepGraph::ReconstructFace(int theFaceDefIdx) const
{
  return reconstructNode(BRepGraph_NodeId(BRepGraph_NodeKind::Face, theFaceDefIdx));
}

TopoDS_Shape BRepGraph::ReconstructFromUsage(BRepGraph_UsageId theRoot) const
{
  return reconstructUsage(theRoot);
}

//=================================================================================================

BRepGraph_TopoNode::EdgeDef& BRepGraph::MutableEdgeDef(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Edge, theIdx));
  return myEdgeDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::WireDef& BRepGraph::MutableWireDef(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, theIdx));
  return myWireDefs.ChangeValue(theIdx);
}

BRepGraph_TopoNode::VertexDef& BRepGraph::MutableVertexDef(int theIdx)
{
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, theIdx));
  return myVertexDefs.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddPCurveToEdge(BRepGraph_NodeId            theEdgeDef,
                                            BRepGraph_NodeId            theFaceDef,
                                            const Handle(Geom2d_Curve)& theCurve2d,
                                            double                      theFirst,
                                            double                      theLast)
{
  BRepGraph_NodeId aPCId = createPCurveNode(theCurve2d, theEdgeDef, theFaceDef, theFirst, theLast);

  BRepGraph_TopoNode::EdgeDef&             anEdgeDef = myEdgeDefs.ChangeValue(theEdgeDef.Index);
  BRepGraph_TopoNode::EdgeDef::PCurveEntry aNewEntry;
  aNewEntry.PCurveNodeId = aPCId;
  aNewEntry.FaceDefId    = theFaceDef;
  anEdgeDef.PCurves.Append(aNewEntry);

  markModified(theEdgeDef);
  return aPCId;
}

//=================================================================================================

void BRepGraph::ReplaceEdgeInWire(int              theWireDefIdx,
                                  BRepGraph_NodeId theOldEdgeDef,
                                  BRepGraph_NodeId theNewEdgeDef,
                                  bool             theReversed)
{
  BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.ChangeValue(theWireDefIdx);
  for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
  {
    BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry = aWireDef.OrderedEdges.ChangeValue(anEdgeIdx);
    if (anEntry.EdgeDefId == theOldEdgeDef)
    {
      anEntry.EdgeDefId = theNewEdgeDef;
      if (theReversed)
      {
        anEntry.OrientationInWire = TopAbs::Reverse(anEntry.OrientationInWire);
      }

      // Update edge-to-wire reverse index.
      if (myEdgeToWires.IsBound(theOldEdgeDef.Index))
      {
        NCollection_Vector<int>& anOldWires = myEdgeToWires.ChangeFind(theOldEdgeDef.Index);
        for (int aWIdx = 0; aWIdx < anOldWires.Length(); ++aWIdx)
        {
          if (anOldWires.Value(aWIdx) == theWireDefIdx)
          {
            if (aWIdx < anOldWires.Length() - 1)
              anOldWires.ChangeValue(aWIdx) = anOldWires.Value(anOldWires.Length() - 1);
            anOldWires.EraseLast();
            break;
          }
        }
      }
      if (!myEdgeToWires.IsBound(theNewEdgeDef.Index))
        myEdgeToWires.Bind(theNewEdgeDef.Index, NCollection_Vector<int>());
      myEdgeToWires.ChangeFind(theNewEdgeDef.Index).Append(theWireDefIdx);
    }
  }
  markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, theWireDefIdx));
}

//=================================================================================================

int BRepGraph::FaceCountForEdge(int theEdgeDefIdx) const
{
  const NCollection_Vector<int>& aWires = WiresOfEdge(theEdgeDefIdx);
  NCollection_Map<int>           aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(aWires.Value(aWIdx));
    // Find face def that owns this wire def via wire usage -> face usage -> face def.
    for (int aUsIdx = 0; aUsIdx < aWireDef.Usages.Length(); ++aUsIdx)
    {
      const BRepGraph_TopoNode::WireUsage& aWireUsage =
        myWireUsages.Value(aWireDef.Usages.Value(aUsIdx).Index);
      if (aWireUsage.OwnerFaceUsage.IsValid())
      {
        BRepGraph_NodeId aFaceDefId = DefOf(aWireUsage.OwnerFaceUsage);
        if (aFaceDefId.IsValid())
          aFaceSet.Add(aFaceDefId.Index);
      }
    }
  }
  return aFaceSet.Extent();
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::WiresOfEdge(int theEdgeDefIdx) const
{
  static const NCollection_Vector<int> THE_EMPTY;
  const NCollection_Vector<int>*       aResult = myEdgeToWires.Seek(theEdgeDefIdx);
  return aResult != nullptr ? *aResult : THE_EMPTY;
}

//=================================================================================================

void BRepGraph::RecordHistory(const TCollection_AsciiString&              theOpLabel,
                              BRepGraph_NodeId                            theOriginal,
                              const NCollection_Vector<BRepGraph_NodeId>& theReplacements)
{
  if (!myIsHistoryEnabled)
    return;

  BRepGraph_HistoryRecord aRecord;
  aRecord.OperationName  = theOpLabel;
  aRecord.SequenceNumber = myHistory.Length();
  aRecord.Mapping.Bind(theOriginal, theReplacements);
  myHistory.Append(aRecord);

  for (int aReplIter = 0; aReplIter < theReplacements.Length(); ++aReplIter)
  {
    const BRepGraph_NodeId aDerived = theReplacements.Value(aReplIter);
    if (aDerived != theOriginal)
    {
      myDerivedToOriginal.Bind(aDerived, theOriginal);
      if (!myOriginalToDerived.IsBound(theOriginal))
        myOriginalToDerived.Bind(theOriginal, NCollection_Vector<BRepGraph_NodeId>());
      myOriginalToDerived.ChangeFind(theOriginal).Append(aDerived);
    }
  }
}

//=================================================================================================

void BRepGraph::SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc)
{
  myAllocator = !theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator();

  mySolidDefs      = NCollection_Vector<BRepGraph_TopoNode::SolidDef>(16, myAllocator);
  myShellDefs      = NCollection_Vector<BRepGraph_TopoNode::ShellDef>(16, myAllocator);
  myFaceDefs       = NCollection_Vector<BRepGraph_TopoNode::FaceDef>(128, myAllocator);
  myWireDefs       = NCollection_Vector<BRepGraph_TopoNode::WireDef>(128, myAllocator);
  myEdgeDefs       = NCollection_Vector<BRepGraph_TopoNode::EdgeDef>(256, myAllocator);
  myVertexDefs     = NCollection_Vector<BRepGraph_TopoNode::VertexDef>(256, myAllocator);
  mySolidUsages    = NCollection_Vector<BRepGraph_TopoNode::SolidUsage>(16, myAllocator);
  myShellUsages    = NCollection_Vector<BRepGraph_TopoNode::ShellUsage>(16, myAllocator);
  myFaceUsages     = NCollection_Vector<BRepGraph_TopoNode::FaceUsage>(128, myAllocator);
  myWireUsages     = NCollection_Vector<BRepGraph_TopoNode::WireUsage>(128, myAllocator);
  myEdgeUsages     = NCollection_Vector<BRepGraph_TopoNode::EdgeUsage>(256, myAllocator);
  myVertexUsages   = NCollection_Vector<BRepGraph_TopoNode::VertexUsage>(256, myAllocator);
  mySurfaces       = NCollection_Vector<BRepGraph_GeomNode::Surf>(64, myAllocator);
  myCurves         = NCollection_Vector<BRepGraph_GeomNode::Curve>(64, myAllocator);
  myPCurves        = NCollection_Vector<BRepGraph_GeomNode::PCurve>(128, myAllocator);
  mySurfRegistry   = NCollection_IndexedDataMap<void*, int>(100, myAllocator);
  myCurveRegistry  = NCollection_IndexedDataMap<void*, int>(100, myAllocator);
  myTShapeToDefId  = NCollection_DataMap<void*, BRepGraph_NodeId>(100, myAllocator);
  myUIDToNodeId    = NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId, BRepGraph_UID::Hasher>(
    BRepGraph_UID::Hasher(), 100, myAllocator);
  myEdgeToWires = NCollection_DataMap<int, NCollection_Vector<int>>(100, myAllocator);
  myHistory     = NCollection_Vector<BRepGraph_HistoryRecord>(256, myAllocator);
}

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const { return myAllocator; }
void BRepGraph::SetHistoryEnabled(bool theVal) { myIsHistoryEnabled = theVal; }
bool BRepGraph::IsHistoryEnabled() const { return myIsHistoryEnabled; }

//=================================================================================================

void BRepGraph::markModified(BRepGraph_NodeId theDefId)
{
  if (!theDefId.IsValid())
    return;

  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theDefId);
  if (aDef == nullptr)
    return;

  const_cast<BRepGraph_TopoNode::BaseDef*>(aDef)->IsModified = true;
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myCurrentShapesMutex);
    myCurrentShapes.UnBind(theDefId);
  }

  // Propagate upward via usages' parent chain.
  for (int aUsIdx = 0; aUsIdx < aDef->Usages.Length(); ++aUsIdx)
  {
    BRepGraph_UsageId aParent;
    switch (theDefId.Kind)
    {
      case BRepGraph_NodeKind::Edge: {
        // For edges, propagate through wire defs via edge-to-wire index.
        const NCollection_Vector<int>* aWires = myEdgeToWires.Seek(theDefId.Index);
        if (aWires != nullptr)
        {
          for (int aWIdx = 0; aWIdx < aWires->Length(); ++aWIdx)
            markModified(BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aWires->Value(aWIdx)));
        }
        return;
      }
      case BRepGraph_NodeKind::Vertex: {
        // Vertices propagate through edge defs that reference them.
        // Just mark done — parent edge will handle further propagation.
        return;
      }
      default: {
        const BRepGraph_UsageId& aUsageId = aDef->Usages.Value(aUsIdx);
        switch (aUsageId.Kind)
        {
          case BRepGraph_NodeKind::Solid:  aParent = mySolidUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeKind::Shell:  aParent = myShellUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeKind::Face:   aParent = myFaceUsages.Value(aUsageId.Index).ParentUsage; break;
          case BRepGraph_NodeKind::Wire: {
            const BRepGraph_TopoNode::WireUsage& aWu = myWireUsages.Value(aUsageId.Index);
            if (aWu.OwnerFaceUsage.IsValid())
              markModified(DefOf(aWu.OwnerFaceUsage));
            continue;
          }
          default: continue;
        }
        if (aParent.IsValid())
          markModified(DefOf(aParent));
      }
    }
  }
}

//=================================================================================================

TopoDS_Shape BRepGraph::Shape(BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Fast path: check unmodified originals (read-only map, no lock needed).
  const BRepGraph_TopoNode::BaseDef* aDef = TopoDef(theNode);
  if (aDef != nullptr && !aDef->IsModified)
  {
    const TopoDS_Shape* anOrig = myOriginalShapes.Seek(theNode);
    if (anOrig != nullptr)
      return *anOrig;
  }

  // Check mutable cache under shared lock.
  {
    std::shared_lock<std::shared_mutex> aReadLock(myCurrentShapesMutex);
    const TopoDS_Shape* aCached = myCurrentShapes.Seek(theNode);
    if (aCached != nullptr)
      return *aCached;
  }

  // Reconstruct outside the lock to avoid holding it during expensive computation.
  TopoDS_Shape aReconstructed = reconstructNode(theNode);

  // Store under exclusive lock with double-check.
  if (!aReconstructed.IsNull())
  {
    std::unique_lock<std::shared_mutex> aWriteLock(myCurrentShapesMutex);
    if (!myCurrentShapes.IsBound(theNode))
      myCurrentShapes.Bind(theNode, aReconstructed);
  }
  return aReconstructed;
}

bool BRepGraph::HasOriginalShape(BRepGraph_NodeId theNode) const
{ return myOriginalShapes.IsBound(theNode); }

const TopoDS_Shape& BRepGraph::OriginalOf(BRepGraph_NodeId theNode) const
{
  const TopoDS_Shape* aShape = myOriginalShapes.Seek(theNode);
  if (aShape == nullptr)
    throw Standard_ProgramError("BRepGraph::OriginalOf() -- no original shape for this node.");
  return *aShape;
}

//=================================================================================================

TopoDS_Shape BRepGraph::reconstructNode(BRepGraph_NodeId theNode) const
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  BRep_Builder aBB;

  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Vertex: {
      const BRepGraph_TopoNode::VertexDef& aVtxDef = myVertexDefs.Value(theNode.Index);
      TopoDS_Vertex aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtxDef.Point, aVtxDef.Tolerance);
      return aNewVtx;
    }
    case BRepGraph_NodeKind::Edge: {
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theNode.Index);
      TopoDS_Edge aNewEdge;
      if (anEdgeDef.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdgeDef.CurveNodeId.IsValid())
      {
        const BRepGraph_GeomNode::Curve& aCurveNode =
          myCurves.Value(anEdgeDef.CurveNodeId.Index);
        // Use location from first usage if available.
        TopLoc_Location aEdgeLoc;
        if (!anEdgeDef.Usages.IsEmpty())
          aEdgeLoc = myEdgeUsages.Value(anEdgeDef.Usages.Value(0).Index).LocalLocation;
        aBB.MakeEdge(aNewEdge, aCurveNode.CurveGeom, aEdgeLoc, anEdgeDef.Tolerance);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
      aBB.Range(aNewEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);

      if (anEdgeDef.StartVertexDefId.IsValid())
      {
        TopoDS_Shape aStartVtx = Shape(anEdgeDef.StartVertexDefId);
        if (!aStartVtx.IsNull())
          aBB.Add(aNewEdge, aStartVtx.Oriented(TopAbs_FORWARD));
      }
      if (anEdgeDef.EndVertexDefId.IsValid())
      {
        TopoDS_Shape anEndVtx = Shape(anEdgeDef.EndVertexDefId);
        if (!anEndVtx.IsNull())
          aBB.Add(aNewEdge, anEndVtx.Oriented(TopAbs_REVERSED));
      }
      return aNewEdge;
    }
    case BRepGraph_NodeKind::Wire: {
      const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theNode.Index);
      TopoDS_Wire aNewWire;
      aBB.MakeWire(aNewWire);
      for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
      {
        const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
          aWireDef.OrderedEdges.Value(anEdgeIdx);
        TopoDS_Shape anEdge = Shape(anEntry.EdgeDefId);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(anEntry.OrientationInWire);
          aBB.Add(aNewWire, anEdge);
        }
      }
      if (aWireDef.IsClosed)
        aNewWire.Closed(true);
      return aNewWire;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theNode.Index);
      if (!aFaceDef.SurfNodeId.IsValid())
        return TopoDS_Shape();
      const BRepGraph_GeomNode::Surf& aSurfNode = mySurfaces.Value(aFaceDef.SurfNodeId.Index);

      // Get global location from first usage.
      TopLoc_Location aFaceGlobalLoc;
      TopAbs_Orientation aFaceOri = TopAbs_FORWARD;
      if (!aFaceDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);
        aFaceGlobalLoc = aFaceUsage.GlobalLocation;
        aFaceOri = aFaceUsage.Orientation;
      }

      TopoDS_Face aNewFace;
      aBB.MakeFace(aNewFace, aSurfNode.Surface, aFaceGlobalLoc, aFaceDef.Tolerance);

      auto buildWireForFace = [&](BRepGraph_NodeId theWireDefId) -> TopoDS_Wire {
        TopoDS_Wire aNewWire;
        aBB.MakeWire(aNewWire);

        const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theWireDefId.Index);
        for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
        {
          const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
            aWireDef.OrderedEdges.Value(anEdgeIdx);
          const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(anEntry.EdgeDefId.Index);

          TopoDS_Edge anEdge;
          if (anEdgeDef.IsDegenerate)
          {
            aBB.MakeEdge(anEdge);
            aBB.Degenerated(anEdge, true);
          }
          else if (anEdgeDef.CurveNodeId.IsValid())
          {
            const BRepGraph_GeomNode::Curve& aCurveNode =
              myCurves.Value(anEdgeDef.CurveNodeId.Index);
            TopLoc_Location aEdgeLoc;
            if (!anEdgeDef.Usages.IsEmpty())
              aEdgeLoc = myEdgeUsages.Value(anEdgeDef.Usages.Value(0).Index).LocalLocation;
            aBB.MakeEdge(anEdge, aCurveNode.CurveGeom, aEdgeLoc, anEdgeDef.Tolerance);
          }
          else
          {
            aBB.MakeEdge(anEdge);
          }
          aBB.Range(anEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);

          if (anEdgeDef.StartVertexDefId.IsValid())
          {
            TopoDS_Shape aStartVtx = Shape(anEdgeDef.StartVertexDefId);
            if (!aStartVtx.IsNull())
              aBB.Add(anEdge, aStartVtx.Oriented(TopAbs_FORWARD));
          }
          if (anEdgeDef.EndVertexDefId.IsValid())
          {
            TopoDS_Shape anEndVtx = Shape(anEdgeDef.EndVertexDefId);
            if (!anEndVtx.IsNull())
              aBB.Add(anEdge, anEndVtx.Oriented(TopAbs_REVERSED));
          }

          // Attach PCurve(s) for THIS face context.
          // Seam edges have two PCurves on the same face — one for FORWARD,
          // one for REVERSED edge orientation.  Use EdgeOrientation to assign
          // C1 (FORWARD) and C2 (REVERSED) correctly.
          Handle(Geom2d_Curve) aPC1, aPC2;
          double aPCFirst = 0.0, aPCLast = 0.0;
          for (int aPCIdx = 0; aPCIdx < anEdgeDef.PCurves.Length(); ++aPCIdx)
          {
            const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
              anEdgeDef.PCurves.Value(aPCIdx);
            if (aPCEntry.FaceDefId == aFaceDef.Id)
            {
              const BRepGraph_GeomNode::PCurve& aPCNode =
                myPCurves.Value(aPCEntry.PCurveNodeId.Index);
              if (!aPCNode.Curve2d.IsNull())
              {
                if (aPCEntry.EdgeOrientation == TopAbs_FORWARD)
                {
                  aPC1 = aPCNode.Curve2d;
                  aPCFirst = aPCNode.ParamFirst;
                  aPCLast  = aPCNode.ParamLast;
                }
                else
                {
                  aPC2 = aPCNode.Curve2d;
                  if (aPC1.IsNull())
                  {
                    aPCFirst = aPCNode.ParamFirst;
                    aPCLast  = aPCNode.ParamLast;
                  }
                }
              }
            }
          }
          if (!aPC1.IsNull() && !aPC2.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPC1, aPC2,
                           aSurfNode.Surface, aFaceGlobalLoc,
                           anEdgeDef.Tolerance);
            aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                      aPCFirst, aPCLast);
          }
          else if (!aPC1.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPC1,
                           aSurfNode.Surface, aFaceGlobalLoc,
                           anEdgeDef.Tolerance);
            aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                      aPCFirst, aPCLast);
          }
          else if (!aPC2.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPC2,
                           aSurfNode.Surface, aFaceGlobalLoc,
                           anEdgeDef.Tolerance);
            aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                      aPCFirst, aPCLast);
          }

          anEdge.Orientation(anEntry.OrientationInWire);
          aBB.Add(aNewWire, anEdge);
        }

        if (aWireDef.IsClosed)
          aNewWire.Closed(true);
        return aNewWire;
      };

      // Find wire defs associated with this face via usages.
      if (!aFaceDef.Usages.IsEmpty())
      {
        const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
          myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);

        if (aFaceUsage.OuterWireUsage.IsValid())
        {
          BRepGraph_NodeId aWireDefId = DefOf(aFaceUsage.OuterWireUsage);
          aBB.Add(aNewFace, buildWireForFace(aWireDefId));
        }

        for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
        {
          BRepGraph_NodeId aWireDefId = DefOf(aFaceUsage.InnerWireUsages.Value(aWireIdx));
          aBB.Add(aNewFace, buildWireForFace(aWireDefId));
        }
      }

      aNewFace.Orientation(aFaceOri);
      return aNewFace;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::ShellDef& aShellDef = myShellDefs.Value(theNode.Index);
      TopoDS_Shell aNewShell;
      aBB.MakeShell(aNewShell);
      if (!aShellDef.Usages.IsEmpty())
      {
        ReconstructCache anEdgeCache;
        const BRepGraph_TopoNode::ShellUsage& aShellUsage =
          myShellUsages.Value(aShellDef.Usages.Value(0).Index);
        for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
        {
          BRepGraph_NodeId aFaceDefId = DefOf(aShellUsage.FaceUsages.Value(aFaceIter));
          TopoDS_Shape aFaceShape = reconstructFaceWithCache(aFaceDefId, anEdgeCache);
          aBB.Add(aNewShell, aFaceShape);
        }
      }
      return aNewShell;
    }
    case BRepGraph_NodeKind::Solid: {
      const BRepGraph_TopoNode::SolidDef& aSolidDef = mySolidDefs.Value(theNode.Index);
      TopoDS_Solid aNewSolid;
      aBB.MakeSolid(aNewSolid);
      if (!aSolidDef.Usages.IsEmpty())
      {
        ReconstructCache anEdgeCache;
        const BRepGraph_TopoNode::SolidUsage& aSolidUsage =
          mySolidUsages.Value(aSolidDef.Usages.Value(0).Index);
        for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
        {
          BRepGraph_NodeId aShellDefId = DefOf(aSolidUsage.ShellUsages.Value(aShellIter));
          const BRepGraph_TopoNode::ShellDef& aShellDef2 = myShellDefs.Value(aShellDefId.Index);
          TopoDS_Shell aNewShell2;
          aBB.MakeShell(aNewShell2);
          if (!aShellDef2.Usages.IsEmpty())
          {
            const BRepGraph_TopoNode::ShellUsage& aShellUsage =
              myShellUsages.Value(aShellDef2.Usages.Value(0).Index);
            for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
            {
              BRepGraph_NodeId aFaceDefId = DefOf(aShellUsage.FaceUsages.Value(aFaceIter));
              TopoDS_Shape aFaceShape = reconstructFaceWithCache(aFaceDefId, anEdgeCache);
              aBB.Add(aNewShell2, aFaceShape);
            }
          }
          aBB.Add(aNewSolid, aNewShell2);
        }
      }
      return aNewSolid;
    }
    default:
      return TopoDS_Shape();
  }
}

//=================================================================================================

TopoDS_Shape BRepGraph::reconstructFaceWithCache(BRepGraph_NodeId  theNode,
                                                  ReconstructCache& theEdgeCache) const
{
  if (!theNode.IsValid() || theNode.Kind != BRepGraph_NodeKind::Face)
    return reconstructNode(theNode);

  BRep_Builder aBB;
  const BRepGraph_TopoNode::FaceDef& aFaceDef = myFaceDefs.Value(theNode.Index);
  if (!aFaceDef.SurfNodeId.IsValid())
    return TopoDS_Shape();
  const BRepGraph_GeomNode::Surf& aSurfNode = mySurfaces.Value(aFaceDef.SurfNodeId.Index);

  TopLoc_Location aFaceGlobalLoc;
  TopAbs_Orientation aFaceOri = TopAbs_FORWARD;
  if (!aFaceDef.Usages.IsEmpty())
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);
    aFaceGlobalLoc = aFaceUsage.GlobalLocation;
    aFaceOri = aFaceUsage.Orientation;
  }

  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aSurfNode.Surface, aFaceGlobalLoc, aFaceDef.Tolerance);

  auto getOrBuildEdge = [&](BRepGraph_NodeId theEdgeDefId) -> TopoDS_Edge {
    const TopoDS_Shape* aCached = theEdgeCache.Seek(theEdgeDefId);
    if (aCached != nullptr)
      return TopoDS::Edge(*aCached);

    const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(theEdgeDefId.Index);
    TopoDS_Edge anEdge;
    if (anEdgeDef.IsDegenerate)
    {
      aBB.MakeEdge(anEdge);
      aBB.Degenerated(anEdge, true);
    }
    else if (anEdgeDef.CurveNodeId.IsValid())
    {
      const BRepGraph_GeomNode::Curve& aCurveNode =
        myCurves.Value(anEdgeDef.CurveNodeId.Index);
      TopLoc_Location aEdgeLoc;
      if (!anEdgeDef.Usages.IsEmpty())
        aEdgeLoc = myEdgeUsages.Value(anEdgeDef.Usages.Value(0).Index).LocalLocation;
      aBB.MakeEdge(anEdge, aCurveNode.CurveGeom, aEdgeLoc, anEdgeDef.Tolerance);
    }
    else
    {
      aBB.MakeEdge(anEdge);
    }
    aBB.Range(anEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);

    // Add vertices (also cached).
    if (anEdgeDef.StartVertexDefId.IsValid())
    {
      const TopoDS_Shape* aVtxCached = theEdgeCache.Seek(anEdgeDef.StartVertexDefId);
      TopoDS_Shape aStartVtx;
      if (aVtxCached != nullptr)
        aStartVtx = *aVtxCached;
      else
      {
        aStartVtx = Shape(anEdgeDef.StartVertexDefId);
        if (!aStartVtx.IsNull())
          theEdgeCache.Bind(anEdgeDef.StartVertexDefId, aStartVtx);
      }
      if (!aStartVtx.IsNull())
        aBB.Add(anEdge, aStartVtx.Oriented(TopAbs_FORWARD));
    }
    if (anEdgeDef.EndVertexDefId.IsValid())
    {
      const TopoDS_Shape* aVtxCached = theEdgeCache.Seek(anEdgeDef.EndVertexDefId);
      TopoDS_Shape anEndVtx;
      if (aVtxCached != nullptr)
        anEndVtx = *aVtxCached;
      else
      {
        anEndVtx = Shape(anEdgeDef.EndVertexDefId);
        if (!anEndVtx.IsNull())
          theEdgeCache.Bind(anEdgeDef.EndVertexDefId, anEndVtx);
      }
      if (!anEndVtx.IsNull())
        aBB.Add(anEdge, anEndVtx.Oriented(TopAbs_REVERSED));
    }

    theEdgeCache.Bind(theEdgeDefId, anEdge);
    return anEdge;
  };

  auto buildWireForFace = [&](BRepGraph_NodeId theWireDefId) -> TopoDS_Wire {
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    const BRepGraph_TopoNode::WireDef& aWireDef = myWireDefs.Value(theWireDefId.Index);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWireDef.OrderedEdges.Value(anEdgeIdx);
      TopoDS_Edge anEdge = getOrBuildEdge(anEntry.EdgeDefId);

      // Attach PCurve(s) for THIS face context.
      // Seam edges have two PCurves on the same face — one for FORWARD,
      // one for REVERSED edge orientation.
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = myEdgeDefs.Value(anEntry.EdgeDefId.Index);
      Handle(Geom2d_Curve) aPC1, aPC2;
      double aPCFirst = 0.0, aPCLast = 0.0;
      for (int aPCIdx = 0; aPCIdx < anEdgeDef.PCurves.Length(); ++aPCIdx)
      {
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
          anEdgeDef.PCurves.Value(aPCIdx);
        if (aPCEntry.FaceDefId == aFaceDef.Id)
        {
          const BRepGraph_GeomNode::PCurve& aPCNode =
            myPCurves.Value(aPCEntry.PCurveNodeId.Index);
          if (!aPCNode.Curve2d.IsNull())
          {
            if (aPCEntry.EdgeOrientation == TopAbs_FORWARD)
            {
              aPC1 = aPCNode.Curve2d;
              aPCFirst = aPCNode.ParamFirst;
              aPCLast  = aPCNode.ParamLast;
            }
            else
            {
              aPC2 = aPCNode.Curve2d;
              if (aPC1.IsNull())
              {
                aPCFirst = aPCNode.ParamFirst;
                aPCLast  = aPCNode.ParamLast;
              }
            }
          }
        }
      }
      if (!aPC1.IsNull() && !aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC1, aPC2,
                       aSurfNode.Surface, aFaceGlobalLoc,
                       anEdgeDef.Tolerance);
        aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                  aPCFirst, aPCLast);
      }
      else if (!aPC1.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC1,
                       aSurfNode.Surface, aFaceGlobalLoc,
                       anEdgeDef.Tolerance);
        aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                  aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2,
                       aSurfNode.Surface, aFaceGlobalLoc,
                       anEdgeDef.Tolerance);
        aBB.Range(anEdge, aSurfNode.Surface, aFaceGlobalLoc,
                  aPCFirst, aPCLast);
      }

      anEdge.Orientation(anEntry.OrientationInWire);
      aBB.Add(aNewWire, anEdge);
    }

    if (aWireDef.IsClosed)
      aNewWire.Closed(true);
    return aNewWire;
  };

  if (!aFaceDef.Usages.IsEmpty())
  {
    const BRepGraph_TopoNode::FaceUsage& aFaceUsage =
      myFaceUsages.Value(aFaceDef.Usages.Value(0).Index);

    if (aFaceUsage.OuterWireUsage.IsValid())
    {
      BRepGraph_NodeId aWireDefId = DefOf(aFaceUsage.OuterWireUsage);
      aBB.Add(aNewFace, buildWireForFace(aWireDefId));
    }

    for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
    {
      BRepGraph_NodeId aWireDefId = DefOf(aFaceUsage.InnerWireUsages.Value(aWireIdx));
      aBB.Add(aNewFace, buildWireForFace(aWireDefId));
    }
  }

  aNewFace.Orientation(aFaceOri);
  return aNewFace;
}

//=================================================================================================

TopoDS_Shape BRepGraph::reconstructUsage(BRepGraph_UsageId theUsage) const
{
  if (!theUsage.IsValid())
    return TopoDS_Shape();

  // Delegate to definition-based reconstruction for now.
  BRepGraph_NodeId aDefId = DefOf(theUsage);
  return reconstructNode(aDefId);
}
