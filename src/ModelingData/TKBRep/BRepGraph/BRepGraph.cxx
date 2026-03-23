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
  TopoDS_Wire                         Shape;
  bool                                IsOuter = false;
  NCollection_Vector<ExtractedEdge> Edges;
};

//! All data extracted from a single face -- filled in parallel, consumed sequentially.
struct FaceLocalData
{
  // Context from Phase 1 (sequential hierarchy scan).
  TopoDS_Face      Face;
  TopLoc_Location  ParentGlobalLoc;
  BRepGraph_NodeId ParentShellId;

  // Geometry extracted in Phase 2 (parallel).
  Handle(Geom_Surface)                Surface;
  Handle(Poly_Triangulation)          Triangulation;
  double                              Tolerance   = 0.0;
  TopAbs_Orientation                  Orientation = TopAbs_FORWARD;
  NCollection_Vector<ExtractedWire> Wires;
};

//! Extract per-face geometry/topology data from TopoDS.
//! Pure read-only BRep_Tool access -- no shared state, fully parallelizable.
void extractFaceData(FaceLocalData& theData)
{
  const TopoDS_Face& aFace = theData.Face;

  theData.Surface = BRep_Tool::Surface(aFace);
  TopLoc_Location aFaceLoc;
  theData.Triangulation = BRep_Tool::Triangulation(aFace, aFaceLoc);
  theData.Tolerance     = BRep_Tool::Tolerance(aFace);
  theData.Orientation   = aFace.Orientation();

  const TopoDS_Wire anOuterWire = BRepTools::OuterWire(aFace);

  for (TopoDS_Iterator aWireIt(aFace); aWireIt.More(); aWireIt.Next())
  {
    if (aWireIt.Value().ShapeType() != TopAbs_WIRE)
      continue;
    const TopoDS_Wire& aWire = TopoDS::Wire(aWireIt.Value());

    ExtractedWire aWireData;
    aWireData.Shape   = aWire;
    aWireData.IsOuter = aWire.IsSame(anOuterWire);

    for (BRepTools_WireExplorer anEdgeExp(aWire, aFace); anEdgeExp.More(); anEdgeExp.Next())
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
      anEdgeData.PCurve2d = BRep_Tool::CurveOnSurface(anEdge, aFace, aPCFirst, aPCLast);
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
      mySolids(16, myAllocator),
      myShells(16, myAllocator),
      myFaces(128, myAllocator),
      myWires(128, myAllocator),
      myEdges(256, myAllocator),
      myVertices(256, myAllocator),
      mySurfaces(64, myAllocator),
      myCurves(64, myAllocator),
      myPCurves(128, myAllocator),
      mySurfRegistry(100, myAllocator),
      myCurveRegistry(100, myAllocator),
      myTShapeToNodeId(100, myAllocator),
      myUIDToNodeId(BRepGraph_UID::Hasher(), 100, myAllocator),
      myHistory(256, myAllocator),
      myIsDone(false)
{
}

//=================================================================================================

BRepGraph::BRepGraph(const Handle(NCollection_BaseAllocator)& theAlloc)
    : myAllocator(!theAlloc.IsNull() ? theAlloc : NCollection_BaseAllocator::CommonBaseAllocator()),
      mySolids(16, myAllocator),
      myShells(16, myAllocator),
      myFaces(128, myAllocator),
      myWires(128, myAllocator),
      myEdges(256, myAllocator),
      myVertices(256, myAllocator),
      mySurfaces(64, myAllocator),
      myCurves(64, myAllocator),
      myPCurves(128, myAllocator),
      mySurfRegistry(100, myAllocator),
      myCurveRegistry(100, myAllocator),
      myTShapeToNodeId(100, myAllocator),
      myUIDToNodeId(BRepGraph_UID::Hasher(), 100, myAllocator),
      myHistory(256, myAllocator),
      myIsDone(false)
{
}

//=================================================================================================

void BRepGraph::Build(const TopoDS_Shape& theShape, bool theParallel)
{
  // Phase 0: Clear all storage, increment generation.
  mySolids.Clear();
  myShells.Clear();
  myFaces.Clear();
  myWires.Clear();
  myEdges.Clear();
  myVertices.Clear();
  mySurfaces.Clear();
  myCurves.Clear();
  myPCurves.Clear();
  myOutRelEdges.Clear();
  myInRelEdges.Clear();
  mySurfRegistry.Clear();
  myCurveRegistry.Clear();
  myTShapeToNodeId.Clear();
  myNodeToUID.Clear();
  myUIDToNodeId.Clear();
  myEdgeToWires.Clear();
  myHistory.Clear();
  myDerivedToOriginal.Clear();
  myOriginalToDerived.Clear();
  ++myGeneration;
  myIsDone = false;

  // Phase 1 (sequential): Explore hierarchy, create Solid/Shell nodes,
  // collect face contexts for parallel extraction.
  NCollection_Vector<FaceLocalData> aFaceData(128, myAllocator);

  bool aHasSolids = false;
  for (TopExp_Explorer aSolidExp(theShape, TopAbs_SOLID); aSolidExp.More(); aSolidExp.Next())
  {
    aHasSolids                 = true;
    const TopoDS_Solid& aSolid = TopoDS::Solid(aSolidExp.Current());

    BRepGraph_TopoNode::Solid& aSolidNode = mySolids.Appended();
    const int                  aSolidIdx  = mySolids.Length() - 1;
    aSolidNode.Id                         = BRepGraph_NodeId(BRepGraph_NodeKind::Solid, aSolidIdx);
    aSolidNode.OriginalShape              = aSolid;
    aSolidNode.LocalLocation              = aSolid.Location();
    aSolidNode.GlobalLocation             = aSolid.Location();
    allocateUID(aSolidNode.Id);

    for (TopoDS_Iterator aShellIt(aSolid); aShellIt.More(); aShellIt.Next())
    {
      if (aShellIt.Value().ShapeType() != TopAbs_SHELL)
        continue;

      const TopoDS_Shell& aShell = TopoDS::Shell(aShellIt.Value());

      BRepGraph_TopoNode::Shell& aShellNode = myShells.Appended();
      const int                  aShellIdx  = myShells.Length() - 1;
      aShellNode.Id             = BRepGraph_NodeId(BRepGraph_NodeKind::Shell, aShellIdx);
      aShellNode.OriginalShape  = aShell;
      aShellNode.LocalLocation  = aShell.Location();
      aShellNode.GlobalLocation = aSolidNode.GlobalLocation * aShell.Location();
      aShellNode.Parent         = aSolidNode.Id;
      allocateUID(aShellNode.Id);

      aSolidNode.Shells.Append(aShellNode.Id);

      for (TopoDS_Iterator aFaceIt(aShell); aFaceIt.More(); aFaceIt.Next())
      {
        if (aFaceIt.Value().ShapeType() != TopAbs_FACE)
          continue;
        FaceLocalData& aData  = aFaceData.Appended();
        aData.Face            = TopoDS::Face(aFaceIt.Value());
        aData.ParentGlobalLoc = aShellNode.GlobalLocation;
        aData.ParentShellId   = aShellNode.Id;
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

      BRepGraph_TopoNode::Shell& aShellNode = myShells.Appended();
      const int                  aShellIdx  = myShells.Length() - 1;
      aShellNode.Id             = BRepGraph_NodeId(BRepGraph_NodeKind::Shell, aShellIdx);
      aShellNode.OriginalShape  = aShell;
      aShellNode.LocalLocation  = aShell.Location();
      aShellNode.GlobalLocation = aShell.Location();
      allocateUID(aShellNode.Id);

      for (TopoDS_Iterator aFaceIt(aShell); aFaceIt.More(); aFaceIt.Next())
      {
        if (aFaceIt.Value().ShapeType() != TopAbs_FACE)
          continue;
        FaceLocalData& aData  = aFaceData.Appended();
        aData.Face            = TopoDS::Face(aFaceIt.Value());
        aData.ParentGlobalLoc = aShellNode.GlobalLocation;
        aData.ParentShellId   = aShellNode.Id;
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

  // Pre-size maps based on estimated entity count to reduce rehashing.
  const int aNbFacesEst   = aFaceData.Length();
  const int anEstEntities = aNbFacesEst * 10;
  myTShapeToNodeId.ReSize(anEstEntities);
  if (myUIDEnabled)
  {
    myNodeToUID.ReSize(anEstEntities);
    myUIDToNodeId.ReSize(anEstEntities);
  }

  // Phase 3 (sequential): Register nodes from pre-extracted data into the graph.
  // No RelEdge creation -- topology relationships are expressed via direct node fields.
  for (int aFaceDataIdx = 0; aFaceDataIdx < aFaceData.Length(); ++aFaceDataIdx)
  {
    const FaceLocalData& aData    = aFaceData.Value(aFaceDataIdx);
    const TopoDS_Face&   aCurFace = aData.Face;

    BRepGraph_TopoNode::Face& aFaceNode = myFaces.Appended();
    const int                 aFaceIdx  = myFaces.Length() - 1;
    aFaceNode.Id                        = BRepGraph_NodeId(BRepGraph_NodeKind::Face, aFaceIdx);
    aFaceNode.OriginalShape             = aCurFace;
    aFaceNode.LocalLocation             = aCurFace.Location();
    aFaceNode.GlobalLocation            = aData.ParentGlobalLoc * aCurFace.Location();
    aFaceNode.Orientation               = aData.Orientation;
    aFaceNode.Tolerance                 = aData.Tolerance;
    aFaceNode.Parent                    = aData.ParentShellId;
    allocateUID(aFaceNode.Id);

    myTShapeToNodeId.Bind(aCurFace.TShape().get(), aFaceNode.Id);

    // Register surface from pre-extracted data.
    aFaceNode.SurfNodeId = registerSurface(aData.Surface, aData.Triangulation);

    // Incrementally populate FaceUsers on the surface node.
    if (aFaceNode.SurfNodeId.IsValid())
    {
      mySurfaces.ChangeValue(aFaceNode.SurfNodeId.Index).FaceUsers.Append(aFaceNode.Id);
    }

    // Populate Shell.Faces via direct field (no Contains RelEdge).
    if (aData.ParentShellId.IsValid())
    {
      myShells.ChangeValue(aData.ParentShellId.Index).Faces.Append(aFaceNode.Id);
    }

    // Process wires from pre-extracted data.
    for (int aWireIter = 0; aWireIter < aData.Wires.Length(); ++aWireIter)
    {
      const ExtractedWire& aWireData = aData.Wires.Value(aWireIter);

      BRepGraph_TopoNode::Wire& aWireNode = myWires.Appended();
      const int                 aWireIdx  = myWires.Length() - 1;
      aWireNode.Id                        = BRepGraph_NodeId(BRepGraph_NodeKind::Wire, aWireIdx);
      aWireNode.OriginalShape             = aWireData.Shape;
      aWireNode.LocalLocation             = aWireData.Shape.Location();
      aWireNode.GlobalLocation            = aFaceNode.GlobalLocation * aWireData.Shape.Location();
      aWireNode.OwnerFaceId               = aFaceNode.Id;
      aWireNode.Parent                    = aFaceNode.Id;
      allocateUID(aWireNode.Id);

      // Wire relationship stored via direct Face fields (no OuterWire/InnerWire RelEdge).
      if (aWireData.IsOuter)
      {
        aFaceNode.OuterWireId = aWireNode.Id;
      }
      else
      {
        aFaceNode.InnerWireIds.Append(aWireNode.Id);
      }

      // Pre-allocate ordered edges array.
      const int aNbWireEdges = aWireData.Edges.Length();
      if (aNbWireEdges > 0)
        aWireNode.OrderedEdges.Resize(0, aNbWireEdges - 1, false);

      BRepGraph_NodeId aFirstVertexId;
      BRepGraph_NodeId aLastVertexId;

      for (int anEdgeIter = 0; anEdgeIter < aWireData.Edges.Length(); ++anEdgeIter)
      {
        const ExtractedEdge& anEdgeData = aWireData.Edges.Value(anEdgeIter);
        const TopoDS_Edge&   anEdge     = anEdgeData.Shape;

        // Dedup edge by TShape.
        void*                   aTShapeKey       = anEdge.TShape().get();
        const BRepGraph_NodeId* anExistingEdgeId = myTShapeToNodeId.Seek(aTShapeKey);

        BRepGraph_NodeId anEdgeNodeId;

        if (anExistingEdgeId != nullptr)
        {
          anEdgeNodeId = *anExistingEdgeId;
        }
        else
        {
          BRepGraph_TopoNode::Edge& anEdgeNode = myEdges.Appended();
          const int                 anEdgeIdx  = myEdges.Length() - 1;
          anEdgeNode.Id             = BRepGraph_NodeId(BRepGraph_NodeKind::Edge, anEdgeIdx);
          anEdgeNode.OriginalShape  = anEdge;
          anEdgeNode.LocalLocation  = anEdge.Location();
          anEdgeNode.GlobalLocation = aWireNode.GlobalLocation * anEdge.Location();
          anEdgeNode.Tolerance      = anEdgeData.Tolerance;
          anEdgeNode.IsDegenerate   = anEdgeData.IsDegenerate;
          anEdgeNode.ParamFirst     = anEdgeData.ParamFirst;
          anEdgeNode.ParamLast      = anEdgeData.ParamLast;
          allocateUID(anEdgeNode.Id);

          if (!anEdgeData.Curve3d.IsNull())
          {
            anEdgeNode.CurveNodeId = registerCurve(anEdgeData.Curve3d);

            // Incrementally populate EdgeUsers on the curve node.
            if (anEdgeNode.CurveNodeId.IsValid())
            {
              myCurves.ChangeValue(anEdgeNode.CurveNodeId.Index).EdgeUsers.Append(anEdgeNode.Id);
            }
          }

          // Vertices from pre-extracted data.
          auto processVertex = [&](const ExtractedVertex& theVtxData) -> BRepGraph_NodeId {
            if (theVtxData.Shape.IsNull())
              return BRepGraph_NodeId();

            void*                   aVTShapeKey   = theVtxData.Shape.TShape().get();
            const BRepGraph_NodeId* anExistingVtx = myTShapeToNodeId.Seek(aVTShapeKey);
            if (anExistingVtx != nullptr)
              return *anExistingVtx;

            BRepGraph_TopoNode::Vertex& aVtxNode = myVertices.Appended();
            const int                   aVtxIdx  = myVertices.Length() - 1;
            aVtxNode.Id             = BRepGraph_NodeId(BRepGraph_NodeKind::Vertex, aVtxIdx);
            aVtxNode.OriginalShape  = theVtxData.Shape;
            aVtxNode.LocalLocation  = theVtxData.Shape.Location();
            aVtxNode.GlobalLocation = anEdgeNode.GlobalLocation * theVtxData.Shape.Location();
            aVtxNode.Point          = theVtxData.Point;
            aVtxNode.Tolerance      = theVtxData.Tolerance;
            aVtxNode.Parent         = anEdgeNode.Id;
            allocateUID(aVtxNode.Id);

            myTShapeToNodeId.Bind(aVTShapeKey, aVtxNode.Id);
            return aVtxNode.Id;
          };

          anEdgeNode.StartVertexId = processVertex(anEdgeData.StartVertex);
          anEdgeNode.EndVertexId   = processVertex(anEdgeData.EndVertex);

          myTShapeToNodeId.Bind(aTShapeKey, anEdgeNode.Id);
          anEdgeNodeId = anEdgeNode.Id;
        }

        // PCurve from pre-extracted data.
        if (!anEdgeData.PCurve2d.IsNull())
        {
          BRepGraph_NodeId aPCurveId = createPCurveNode(anEdgeData.PCurve2d,
                                                        anEdgeNodeId,
                                                        aFaceNode.Id,
                                                        anEdgeData.PCFirst,
                                                        anEdgeData.PCLast);

          BRepGraph_TopoNode::Edge::PCurveEntry aPCEntry;
          aPCEntry.PCurveNodeId = aPCurveId;
          aPCEntry.FaceNodeId   = aFaceNode.Id;
          myEdges.ChangeValue(anEdgeNodeId.Index).PCurves.Append(aPCEntry);
        }

        // EdgeEntry in wire.
        BRepGraph_TopoNode::Wire::EdgeEntry aWEEntry;
        aWEEntry.EdgeId            = anEdgeNodeId;
        aWEEntry.OrientationInWire = anEdgeData.OrientationInWire;
        aWireNode.OrderedEdges.SetValue(anEdgeIter, aWEEntry);

        // Populate edge-to-wire reverse index.
        if (!myEdgeToWires.IsBound(anEdgeNodeId.Index))
          myEdgeToWires.Bind(anEdgeNodeId.Index, NCollection_Vector<int>());
        myEdgeToWires.ChangeFind(anEdgeNodeId.Index).Append(aWireIdx);

        // Track first/last vertex for closure check.
        if (!aFirstVertexId.IsValid())
        {
          BRepGraph_TopoNode::Edge& aFirstEdgeRef = myEdges.ChangeValue(anEdgeNodeId.Index);
          if (anEdgeData.OrientationInWire == TopAbs_FORWARD)
            aFirstVertexId = aFirstEdgeRef.StartVertexId;
          else
            aFirstVertexId = aFirstEdgeRef.EndVertexId;
        }
        {
          BRepGraph_TopoNode::Edge& aLastEdgeRef = myEdges.ChangeValue(anEdgeNodeId.Index);
          if (anEdgeData.OrientationInWire == TopAbs_FORWARD)
            aLastVertexId = aLastEdgeRef.EndVertexId;
          else
            aLastVertexId = aLastEdgeRef.StartVertexId;
        }
      }

      // Set wire closure.
      aWireNode.IsClosed =
        aFirstVertexId.IsValid() && aLastVertexId.IsValid() && aFirstVertexId == aLastVertexId;
    }
  }

  // Phase 4: Set IsMultiLocated flags (replaces buildSameDomainEdges).
  for (int aSurfIdx = 0; aSurfIdx < mySurfaces.Length(); ++aSurfIdx)
  {
    mySurfaces.ChangeValue(aSurfIdx).IsMultiLocated =
      (mySurfaces.Value(aSurfIdx).FaceUsers.Length() > 1);
  }
  for (int aCurveIdx = 0; aCurveIdx < myCurves.Length(); ++aCurveIdx)
  {
    myCurves.ChangeValue(aCurveIdx).IsMultiLocated =
      (myCurves.Value(aCurveIdx).EdgeUsers.Length() > 1);
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
                                             BRepGraph_NodeId            theEdge,
                                             BRepGraph_NodeId            theFace,
                                             double                      theFirst,
                                             double                      theLast)
{
  BRepGraph_GeomNode::PCurve& aNode = myPCurves.Appended();
  const int                   aIdx  = myPCurves.Length() - 1;
  aNode.Id                          = BRepGraph_NodeId(BRepGraph_NodeKind::PCurve, aIdx);
  aNode.Curve2d                     = theCrv2d;
  aNode.EdgeContext                 = theEdge;
  aNode.FaceContext                 = theFace;
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
      return &mySolids.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Shell:
      return &myShells.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Face:
      return &myFaces.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Wire:
      return &myWires.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Edge:
      return &myEdges.ChangeValue(theNode.Index).Cache;
    case BRepGraph_NodeKind::Vertex:
      return &myVertices.ChangeValue(theNode.Index).Cache;
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

int BRepGraph::NbSolids() const
{
  return mySolids.Length();
}

int BRepGraph::NbShells() const
{
  return myShells.Length();
}

int BRepGraph::NbFaces() const
{
  return myFaces.Length();
}

int BRepGraph::NbWires() const
{
  return myWires.Length();
}

int BRepGraph::NbEdges() const
{
  return myEdges.Length();
}

int BRepGraph::NbVertices() const
{
  return myVertices.Length();
}

int BRepGraph::NbSurfaces() const
{
  return mySurfaces.Length();
}

int BRepGraph::NbCurves() const
{
  return myCurves.Length();
}

int BRepGraph::NbPCurves() const
{
  return myPCurves.Length();
}

//=================================================================================================

const BRepGraph_TopoNode::Solid& BRepGraph::Solid(int theIdx) const
{
  return mySolids.Value(theIdx);
}

const BRepGraph_TopoNode::Shell& BRepGraph::Shell(int theIdx) const
{
  return myShells.Value(theIdx);
}

const BRepGraph_TopoNode::Face& BRepGraph::Face(int theIdx) const
{
  return myFaces.Value(theIdx);
}

const BRepGraph_TopoNode::Wire& BRepGraph::Wire(int theIdx) const
{
  return myWires.Value(theIdx);
}

const BRepGraph_TopoNode::Edge& BRepGraph::Edge(int theIdx) const
{
  return myEdges.Value(theIdx);
}

const BRepGraph_TopoNode::Vertex& BRepGraph::Vertex(int theIdx) const
{
  return myVertices.Value(theIdx);
}

const BRepGraph_GeomNode::Surf& BRepGraph::Surf(int theIdx) const
{
  return mySurfaces.Value(theIdx);
}

const BRepGraph_GeomNode::Curve& BRepGraph::Curve(int theIdx) const
{
  return myCurves.Value(theIdx);
}

const BRepGraph_GeomNode::PCurve& BRepGraph::PCurve(int theIdx) const
{
  return myPCurves.Value(theIdx);
}

//=================================================================================================

const BRepGraph_TopoNode::Base* BRepGraph::TopoNode(BRepGraph_NodeId theId) const
{
  if (!theId.IsValid())
    return nullptr;

  switch (theId.Kind)
  {
    case BRepGraph_NodeKind::Solid:
      return theId.Index < mySolids.Length()
               ? static_cast<const BRepGraph_TopoNode::Base*>(&mySolids.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Shell:
      return theId.Index < myShells.Length()
               ? static_cast<const BRepGraph_TopoNode::Base*>(&myShells.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Face:
      return theId.Index < myFaces.Length()
               ? static_cast<const BRepGraph_TopoNode::Base*>(&myFaces.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Wire:
      return theId.Index < myWires.Length()
               ? static_cast<const BRepGraph_TopoNode::Base*>(&myWires.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Edge:
      return theId.Index < myEdges.Length()
               ? static_cast<const BRepGraph_TopoNode::Base*>(&myEdges.Value(theId.Index))
               : nullptr;
    case BRepGraph_NodeKind::Vertex:
      return theId.Index < myVertices.Length()
               ? static_cast<const BRepGraph_TopoNode::Base*>(&myVertices.Value(theId.Index))
               : nullptr;
    default:
      return nullptr;
  }
}

//=================================================================================================

size_t BRepGraph::NbNodes() const
{
  return static_cast<size_t>(mySolids.Length()) + static_cast<size_t>(myShells.Length())
         + static_cast<size_t>(myFaces.Length()) + static_cast<size_t>(myWires.Length())
         + static_cast<size_t>(myEdges.Length()) + static_cast<size_t>(myVertices.Length())
         + static_cast<size_t>(mySurfaces.Length()) + static_cast<size_t>(myCurves.Length())
         + static_cast<size_t>(myPCurves.Length());
}

//=================================================================================================

void BRepGraph::SetUIDEnabled(bool theVal)
{
  myUIDEnabled = theVal;
}

//=================================================================================================

bool BRepGraph::IsUIDEnabled() const
{
  return myUIDEnabled;
}

//=================================================================================================

BRepGraph_UID BRepGraph::UIDOf(BRepGraph_NodeId theNode) const
{
  if (!myUIDEnabled)
    return BRepGraph_UID();
  const BRepGraph_UID* aPtr = myNodeToUID.Seek(theNode);
  return aPtr != nullptr ? *aPtr : BRepGraph_UID();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::NodeIdFromUID(const BRepGraph_UID& theUID) const
{
  const BRepGraph_NodeId* aPtr = myUIDToNodeId.Seek(theUID);
  if (aPtr == nullptr)
    return BRepGraph_NodeId();
  return *aPtr;
}

//=================================================================================================

bool BRepGraph::HasUID(const BRepGraph_UID& theUID) const
{
  if (!myUIDEnabled || !theUID.IsValid())
    return false;
  if (theUID.Generation() != myGeneration)
    return false;
  return myUIDToNodeId.IsBound(theUID);
}

//=================================================================================================

uint32_t BRepGraph::Generation() const
{
  return myGeneration;
}

//=================================================================================================

int BRepGraph::AddRelEdge(BRepGraph_NodeId  theFrom,
                          BRepGraph_NodeId  theTo,
                          BRepGraph_RelKind theKind)
{
  BRepGraph_RelEdge anEdge;
  anEdge.Kind   = theKind;
  anEdge.Source = theFrom;
  anEdge.Target = theTo;

  // Add to outgoing map.
  if (!myOutRelEdges.IsBound(theFrom))
    myOutRelEdges.Bind(theFrom, NCollection_Vector<BRepGraph_RelEdge>());
  NCollection_Vector<BRepGraph_RelEdge>& anOutVec = myOutRelEdges.ChangeFind(theFrom);
  anOutVec.Append(anEdge);
  const int anIdx = anOutVec.Length() - 1;

  // Add to incoming map.
  if (!myInRelEdges.IsBound(theTo))
    myInRelEdges.Bind(theTo, NCollection_Vector<BRepGraph_RelEdge>());
  myInRelEdges.ChangeFind(theTo).Append(anEdge);

  return anIdx;
}

//=================================================================================================

void BRepGraph::RemoveRelEdges(BRepGraph_NodeId  theFrom,
                               BRepGraph_NodeId  theTo,
                               BRepGraph_RelKind theKind)
{
  // Remove from outgoing map.
  NCollection_Vector<BRepGraph_RelEdge>* anOutVec = myOutRelEdges.ChangeSeek(theFrom);
  if (anOutVec != nullptr)
  {
    for (int anIdx = anOutVec->Length() - 1; anIdx >= 0; --anIdx)
    {
      const BRepGraph_RelEdge& anEdge = anOutVec->Value(anIdx);
      if (anEdge.Kind == theKind && anEdge.Target == theTo)
      {
        // Swap with last and erase.
        if (anIdx < anOutVec->Length() - 1)
          anOutVec->ChangeValue(anIdx) = anOutVec->Value(anOutVec->Length() - 1);
        anOutVec->EraseLast();
      }
    }
  }

  // Remove from incoming map.
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

//=================================================================================================

int BRepGraph::NbRelEdgesTo(BRepGraph_NodeId theNode) const
{
  const NCollection_Vector<BRepGraph_RelEdge>* aEdges = myInRelEdges.Seek(theNode);
  return aEdges != nullptr ? aEdges->Length() : 0;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::SurfaceOf(BRepGraph_NodeId theFace) const
{
  if (theFace.Kind != BRepGraph_NodeKind::Face || !theFace.IsValid())
    return BRepGraph_NodeId();
  return myFaces.Value(theFace.Index).SurfNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::FacesOnSurface(
  BRepGraph_NodeId theSurf) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theSurf.Kind != BRepGraph_NodeKind::Surface || !theSurf.IsValid()
      || theSurf.Index >= mySurfaces.Length())
    return THE_EMPTY_VEC;
  return mySurfaces.Value(theSurf.Index).FaceUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::CurveOf(BRepGraph_NodeId theEdge) const
{
  if (theEdge.Kind != BRepGraph_NodeKind::Edge || !theEdge.IsValid())
    return BRepGraph_NodeId();
  return myEdges.Value(theEdge.Index).CurveNodeId;
}

//=================================================================================================

const NCollection_Vector<BRepGraph_NodeId>& BRepGraph::EdgesOnCurve(
  BRepGraph_NodeId theCurve) const
{
  static const NCollection_Vector<BRepGraph_NodeId> THE_EMPTY_VEC;
  if (theCurve.Kind != BRepGraph_NodeKind::Curve || !theCurve.IsValid()
      || theCurve.Index >= myCurves.Length())
    return THE_EMPTY_VEC;
  return myCurves.Value(theCurve.Index).EdgeUsers;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::PCurveOf(BRepGraph_NodeId theEdge, BRepGraph_NodeId theFace) const
{
  if (theEdge.Kind != BRepGraph_NodeKind::Edge || !theEdge.IsValid())
    return BRepGraph_NodeId();

  const BRepGraph_TopoNode::Edge& anEdgeNode = myEdges.Value(theEdge.Index);
  for (int aPCurveIter = 0; aPCurveIter < anEdgeNode.PCurves.Length(); ++aPCurveIter)
  {
    if (anEdgeNode.PCurves.Value(aPCurveIter).FaceNodeId == theFace)
      return anEdgeNode.PCurves.Value(aPCurveIter).PCurveNodeId;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

gp_Trsf BRepGraph::GlobalTransform(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::Base* aBase = TopoNode(theNode);
  if (aBase == nullptr)
    return gp_Trsf();
  return aBase->GlobalLocation.Transformation();
}

//=================================================================================================

NCollection_Vector<BRepGraph_NodeId> BRepGraph::SameDomainFaces(BRepGraph_NodeId theFace) const
{
  NCollection_Vector<BRepGraph_NodeId> aResult;
  if (theFace.Kind != BRepGraph_NodeKind::Face || !theFace.IsValid())
    return aResult;

  const BRepGraph_TopoNode::Face& aFaceNode = myFaces.Value(theFace.Index);
  if (!aFaceNode.SurfNodeId.IsValid())
    return aResult;

  const NCollection_Vector<BRepGraph_NodeId>& aFaceUsers =
    mySurfaces.Value(aFaceNode.SurfNodeId.Index).FaceUsers;
  for (int anIdx = 0; anIdx < aFaceUsers.Length(); ++anIdx)
  {
    if (aFaceUsers.Value(anIdx) != theFace)
      aResult.Append(aFaceUsers.Value(anIdx));
  }
  return aResult;
}

//=================================================================================================

Bnd_Box BRepGraph::BoundingBox(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::Base* aBase = TopoNode(theNode);
  if (aBase == nullptr)
    return Bnd_Box();

  return aBase->Cache.BoundingBox.Get([&]() -> Bnd_Box {
    Bnd_Box aBox;
    for (TopExp_Explorer anExp(aBase->OriginalShape, TopAbs_VERTEX); anExp.More(); anExp.Next())
    {
      const TopoDS_Vertex& aVtx = TopoDS::Vertex(anExp.Current());
      aBox.Add(BRep_Tool::Pnt(aVtx));
    }
    return aBox;
  });
}

//=================================================================================================

gp_Pnt BRepGraph::Centroid(BRepGraph_NodeId theNode) const
{
  const BRepGraph_TopoNode::Base* aBase = TopoNode(theNode);
  if (aBase == nullptr)
    return gp_Pnt();

  return aBase->Cache.Centroid.Get([&]() -> gp_Pnt {
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

  // Direct typed child field traversal (faster than RelEdge-based traversal).
  switch (theNode.Kind)
  {
    case BRepGraph_NodeKind::Solid: {
      const BRepGraph_TopoNode::Solid& aSolid = mySolids.Value(theNode.Index);
      for (int aShellIter = 0; aShellIter < aSolid.Shells.Length(); ++aShellIter)
        invalidateSubgraphImpl(aSolid.Shells.Value(aShellIter));
      break;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::Shell& aShell = myShells.Value(theNode.Index);
      for (int aFaceIter = 0; aFaceIter < aShell.Faces.Length(); ++aFaceIter)
        invalidateSubgraphImpl(aShell.Faces.Value(aFaceIter));
      break;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::Face& aFace = myFaces.Value(theNode.Index);
      if (aFace.OuterWireId.IsValid())
        invalidateSubgraphImpl(aFace.OuterWireId);
      for (int aWireIter = 0; aWireIter < aFace.InnerWireIds.Length(); ++aWireIter)
        invalidateSubgraphImpl(aFace.InnerWireIds.Value(aWireIter));
      break;
    }
    case BRepGraph_NodeKind::Wire: {
      const BRepGraph_TopoNode::Wire& aWire = myWires.Value(theNode.Index);
      for (int anEdgeIdx = 0; anEdgeIdx < aWire.OrderedEdges.Length(); ++anEdgeIdx)
        invalidateSubgraphImpl(aWire.OrderedEdges.Value(anEdgeIdx).EdgeId);
      break;
    }
    case BRepGraph_NodeKind::Edge: {
      const BRepGraph_TopoNode::Edge& anEdge = myEdges.Value(theNode.Index);
      if (anEdge.StartVertexId.IsValid())
        Invalidate(anEdge.StartVertexId);
      if (anEdge.EndVertexId.IsValid())
        Invalidate(anEdge.EndVertexId);
      break;
    }
    default:
      break;
  }
}

//=================================================================================================

void BRepGraph::SetUserAttribute(BRepGraph_NodeId             theNode,
                                 int                          theKey,
                                 const BRepGraph_UserAttrPtr& theAttr)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->SetUserAttribute(theKey, theAttr);
}

//=================================================================================================

BRepGraph_UserAttrPtr BRepGraph::GetUserAttribute(BRepGraph_NodeId theNode, int theKey) const
{
  const BRepGraph_TopoNode::Base* aBase = TopoNode(theNode);
  if (aBase == nullptr)
    return nullptr;
  return aBase->Cache.GetUserAttribute(theKey);
}

//=================================================================================================

bool BRepGraph::RemoveUserAttribute(BRepGraph_NodeId theNode, int theKey)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache == nullptr)
    return false;
  return aCache->RemoveUserAttribute(theKey);
}

//=================================================================================================

void BRepGraph::InvalidateUserAttribute(BRepGraph_NodeId theNode, int theKey)
{
  BRepGraph_NodeCache* aCache = mutableCache(theNode);
  if (aCache != nullptr)
    aCache->InvalidateUserAttribute(theKey);
}

//=================================================================================================

int BRepGraph::NbHistoryRecords() const
{
  return myHistory.Length();
}

//=================================================================================================

const BRepGraph_HistoryRecord& BRepGraph::HistoryRecord(int theIdx) const
{
  return myHistory.Value(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::FindOriginal(BRepGraph_NodeId theModified) const
{
  const BRepGraph_NodeId* aPtr = myDerivedToOriginal.Seek(theModified);
  if (aPtr == nullptr)
    return theModified;
  return FindOriginal(*aPtr);
}

//=================================================================================================

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

    // Populate reverse maps for O(1) FindOriginal/FindDerived.
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
  BRep_Builder aBuilder;

  switch (theRoot.Kind)
  {
    case BRepGraph_NodeKind::Solid: {
      const BRepGraph_TopoNode::Solid& aSolid = mySolids.Value(theRoot.Index);
      TopoDS_Solid                     aNewSolid;
      aBuilder.MakeSolid(aNewSolid);
      for (int aShellIter = 0; aShellIter < aSolid.Shells.Length(); ++aShellIter)
      {
        TopoDS_Shape aShellShape = ReconstructShape(aSolid.Shells.Value(aShellIter));
        aBuilder.Add(aNewSolid, aShellShape);
      }
      return aNewSolid;
    }
    case BRepGraph_NodeKind::Shell: {
      const BRepGraph_TopoNode::Shell& aShell = myShells.Value(theRoot.Index);
      TopoDS_Shell                     aNewShell;
      aBuilder.MakeShell(aNewShell);
      for (int aFaceIter = 0; aFaceIter < aShell.Faces.Length(); ++aFaceIter)
      {
        TopoDS_Shape aFaceShape = ReconstructShape(aShell.Faces.Value(aFaceIter));
        aBuilder.Add(aNewShell, aFaceShape);
      }
      return aNewShell;
    }
    case BRepGraph_NodeKind::Face: {
      const BRepGraph_TopoNode::Face& aFaceNode = myFaces.Value(theRoot.Index);
      return aFaceNode.OriginalShape;
    }
    default: {
      throw Standard_ProgramError(
        "BRepGraph::ReconstructShape() -- root must be Solid, Shell, or Face kind.");
    }
  }
}

//=================================================================================================

TopoDS_Shape BRepGraph::ReconstructFace(int theFaceIdx) const
{
  const BRepGraph_TopoNode::Face& aFaceNode = myFaces.Value(theFaceIdx);
  const BRepGraph_GeomNode::Surf& aSurfNode = mySurfaces.Value(aFaceNode.SurfNodeId.Index);

  BRep_Builder aBB;

  auto buildWireFromGraph = [&](BRepGraph_NodeId theWireId) -> TopoDS_Wire {
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    const BRepGraph_TopoNode::Wire& aWireNode = myWires.Value(theWireId.Index);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireNode.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::Wire::EdgeEntry& anEntry = aWireNode.OrderedEdges.Value(anEdgeIdx);
      const BRepGraph_TopoNode::Edge&            anEdgeNode = myEdges.Value(anEntry.EdgeId.Index);
      TopoDS_Shape anEdge = anEdgeNode.OriginalShape.Oriented(anEntry.OrientationInWire);
      aBB.Add(aNewWire, anEdge);
    }

    if (aWireNode.IsClosed)
      aNewWire.Closed(true);

    return aNewWire;
  };

  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aSurfNode.Surface, aFaceNode.GlobalLocation, aFaceNode.Tolerance);

  if (aFaceNode.OuterWireId.IsValid())
  {
    TopoDS_Wire anOuterWire = buildWireFromGraph(aFaceNode.OuterWireId);
    aBB.Add(aNewFace, anOuterWire);
  }

  for (int aWireIdx = 0; aWireIdx < aFaceNode.InnerWireIds.Length(); ++aWireIdx)
  {
    TopoDS_Wire anInnerWire = buildWireFromGraph(aFaceNode.InnerWireIds.Value(aWireIdx));
    aBB.Add(aNewFace, anInnerWire);
  }

  aNewFace.Orientation(aFaceNode.Orientation);
  return aNewFace;
}

//=================================================================================================

BRepGraph_TopoNode::Edge& BRepGraph::MutableEdge(int theIdx)
{
  return myEdges.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::Wire& BRepGraph::MutableWire(int theIdx)
{
  return myWires.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_TopoNode::Vertex& BRepGraph::MutableVertex(int theIdx)
{
  return myVertices.ChangeValue(theIdx);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph::AddPCurveToEdge(BRepGraph_NodeId            theEdge,
                                            BRepGraph_NodeId            theFace,
                                            const Handle(Geom2d_Curve)& theCurve2d,
                                            double                      theFirst,
                                            double                      theLast)
{
  BRepGraph_NodeId aPCId = createPCurveNode(theCurve2d, theEdge, theFace, theFirst, theLast);

  BRepGraph_TopoNode::Edge&             anEdge = myEdges.ChangeValue(theEdge.Index);
  BRepGraph_TopoNode::Edge::PCurveEntry aNewEntry;
  aNewEntry.PCurveNodeId = aPCId;
  aNewEntry.FaceNodeId   = theFace;
  anEdge.PCurves.Append(aNewEntry);

  return aPCId;
}

//=================================================================================================

void BRepGraph::ReplaceEdgeInWire(int              theWireIdx,
                                  BRepGraph_NodeId theOldEdge,
                                  BRepGraph_NodeId theNewEdge,
                                  bool             theReversed)
{
  BRepGraph_TopoNode::Wire& aWire = myWires.ChangeValue(theWireIdx);
  for (int anEdgeIdx = 0; anEdgeIdx < aWire.OrderedEdges.Length(); ++anEdgeIdx)
  {
    BRepGraph_TopoNode::Wire::EdgeEntry& anEntry = aWire.OrderedEdges.ChangeValue(anEdgeIdx);
    if (anEntry.EdgeId == theOldEdge)
    {
      anEntry.EdgeId = theNewEdge;
      if (theReversed)
      {
        anEntry.OrientationInWire = TopAbs::Reverse(anEntry.OrientationInWire);
      }

      // Update edge-to-wire reverse index.
      if (myEdgeToWires.IsBound(theOldEdge.Index))
      {
        NCollection_Vector<int>& anOldWires = myEdgeToWires.ChangeFind(theOldEdge.Index);
        for (int aWIdx = 0; aWIdx < anOldWires.Length(); ++aWIdx)
        {
          if (anOldWires.Value(aWIdx) == theWireIdx)
          {
            if (aWIdx < anOldWires.Length() - 1)
              anOldWires.ChangeValue(aWIdx) = anOldWires.Value(anOldWires.Length() - 1);
            anOldWires.EraseLast();
            break;
          }
        }
      }
      if (!myEdgeToWires.IsBound(theNewEdge.Index))
        myEdgeToWires.Bind(theNewEdge.Index, NCollection_Vector<int>());
      myEdgeToWires.ChangeFind(theNewEdge.Index).Append(theWireIdx);
    }
  }
}

//=================================================================================================

int BRepGraph::FaceCountForEdge(int theEdgeIdx) const
{
  const NCollection_Vector<int>& aWires = WiresOfEdge(theEdgeIdx);
  NCollection_Map<int>           aFaceSet;
  for (int aWIdx = 0; aWIdx < aWires.Length(); ++aWIdx)
  {
    const BRepGraph_TopoNode::Wire& aWire = myWires.Value(aWires.Value(aWIdx));
    if (aWire.OwnerFaceId.IsValid())
      aFaceSet.Add(aWire.OwnerFaceId.Index);
  }
  return aFaceSet.Extent();
}

//=================================================================================================

const NCollection_Vector<int>& BRepGraph::WiresOfEdge(int theEdgeIdx) const
{
  static const NCollection_Vector<int> THE_EMPTY;
  const NCollection_Vector<int>*       aResult = myEdgeToWires.Seek(theEdgeIdx);
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

  // Populate reverse maps.
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

  mySolids         = NCollection_Vector<BRepGraph_TopoNode::Solid>(16, myAllocator);
  myShells         = NCollection_Vector<BRepGraph_TopoNode::Shell>(16, myAllocator);
  myFaces          = NCollection_Vector<BRepGraph_TopoNode::Face>(128, myAllocator);
  myWires          = NCollection_Vector<BRepGraph_TopoNode::Wire>(128, myAllocator);
  myEdges          = NCollection_Vector<BRepGraph_TopoNode::Edge>(256, myAllocator);
  myVertices       = NCollection_Vector<BRepGraph_TopoNode::Vertex>(256, myAllocator);
  mySurfaces       = NCollection_Vector<BRepGraph_GeomNode::Surf>(64, myAllocator);
  myCurves         = NCollection_Vector<BRepGraph_GeomNode::Curve>(64, myAllocator);
  myPCurves        = NCollection_Vector<BRepGraph_GeomNode::PCurve>(128, myAllocator);
  mySurfRegistry   = NCollection_IndexedDataMap<void*, int>(100, myAllocator);
  myCurveRegistry  = NCollection_IndexedDataMap<void*, int>(100, myAllocator);
  myTShapeToNodeId = NCollection_DataMap<void*, BRepGraph_NodeId>(100, myAllocator);
  myUIDToNodeId    = NCollection_DataMap<BRepGraph_UID, BRepGraph_NodeId, BRepGraph_UID::Hasher>(
    BRepGraph_UID::Hasher(),
    100,
    myAllocator);
  myEdgeToWires = NCollection_DataMap<int, NCollection_Vector<int>>(100, myAllocator);
  myHistory     = NCollection_Vector<BRepGraph_HistoryRecord>(256, myAllocator);
}

//=================================================================================================

const Handle(NCollection_BaseAllocator)& BRepGraph::Allocator() const
{
  return myAllocator;
}

//=================================================================================================

void BRepGraph::SetHistoryEnabled(bool theVal)
{
  myIsHistoryEnabled = theVal;
}

//=================================================================================================

bool BRepGraph::IsHistoryEnabled() const
{
  return myIsHistoryEnabled;
}
