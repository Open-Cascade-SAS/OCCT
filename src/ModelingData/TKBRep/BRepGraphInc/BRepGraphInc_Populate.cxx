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

#include <BRepGraphInc_Populate.hxx>

#include <BRepGraph.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_ParallelPolicy.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <algorithm>

#include "BRepGraphInc_BoundaryBuilder.pxx"

namespace
{
constexpr size_t THE_ARRAY_BUCKET       = 256;
constexpr size_t THE_SMALL_ARRAY_BUCKET = 8;
constexpr size_t THE_SMALL_MAP_BUCKET   = 1;

struct BuildCounts
{
  uint32_t NbVertices   = 0;
  uint32_t NbEdges      = 0;
  uint32_t NbWires      = 0;
  uint32_t NbFaces      = 0;
  uint32_t NbShells     = 0;
  uint32_t NbSolids     = 0;
  uint32_t NbCompounds  = 0;
  uint32_t NbCompSolids = 0;
  uint32_t NbCoEdges    = 0;
  uint32_t NbChildRefs  = 0;
  uint32_t NbSolidRefs  = 0;
  uint32_t NbFaceRefs   = 0;
  uint32_t NbShellRefs  = 0;
  uint32_t NbWireRefs   = 0;
};

static void extractEdgeSupplementAttachments(BRepGraph_LayerTopoSupplement& theLayer,
                                             const BRepGraph_NodeId         theOwner,
                                             const TopoDS_Shape&            theOwnerShape)
{
  size_t aLastForwardIdx  = 0;
  size_t aLastReversedIdx = 0;
  size_t aChildIdx        = 0;
  bool   hasForward       = false;
  bool   hasReversed      = false;
  for (TopoDS_Iterator aChildIt(theOwnerShape, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() == TopAbs_VERTEX && aChild.Orientation() == TopAbs_FORWARD)
    {
      aLastForwardIdx = aChildIdx;
      hasForward      = true;
    }
    else if (aChild.ShapeType() == TopAbs_VERTEX && aChild.Orientation() == TopAbs_REVERSED)
    {
      aLastReversedIdx = aChildIdx;
      hasReversed      = true;
    }
    ++aChildIdx;
  }

  aChildIdx = 0;
  for (TopoDS_Iterator aChildIt(theOwnerShape, false, false); aChildIt.More();
       aChildIt.Next(), ++aChildIdx)
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() != TopAbs_VERTEX)
    {
      theLayer.AddAttachment(theOwner,
                             BRepGraph_LayerTopoSupplement::AttachmentKind::GenericSupplementShape,
                             aChild);
      continue;
    }

    const bool isBoundaryForward =
      hasForward && aChild.Orientation() == TopAbs_FORWARD && aChildIdx == aLastForwardIdx;
    const bool isBoundaryReversed =
      hasReversed && aChild.Orientation() == TopAbs_REVERSED && aChildIdx == aLastReversedIdx;
    if (!isBoundaryForward && !isBoundaryReversed)
    {
      theLayer.AddAttachment(theOwner,
                             BRepGraph_LayerTopoSupplement::AttachmentKind::EdgeInternalVertex,
                             aChild);
    }
  }
}

static void extractOwnerSupplementAttachments(BRepGraph_LayerTopoSupplement& theLayer,
                                              const BRepGraph_NodeId         theOwner,
                                              const TopoDS_Shape&            theOwnerShape)
{
  switch (theOwner.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Edge:
      extractEdgeSupplementAttachments(theLayer, theOwner, theOwnerShape);
      return;
    case BRepGraph_NodeId::Kind::Face:
      for (TopoDS_Iterator aChildIt(theOwnerShape, false, false); aChildIt.More(); aChildIt.Next())
      {
        const TopoDS_Shape& aChild = aChildIt.Value();
        if (aChild.ShapeType() == TopAbs_WIRE)
        {
          continue;
        }

        const BRepGraph_LayerTopoSupplement::AttachmentKind aKind =
          aChild.ShapeType() == TopAbs_VERTEX
            ? BRepGraph_LayerTopoSupplement::AttachmentKind::FaceDirectVertex
            : BRepGraph_LayerTopoSupplement::AttachmentKind::GenericSupplementShape;
        theLayer.AddAttachment(theOwner, aKind, aChild);
      }
      return;
    default:
      return;
  }
}

static void attachSupplement(const occ::handle<BRepGraph_LayerTopoSupplement>& theLayer,
                             const BRepGraph_NodeId                            theOwner,
                             const TopoDS_Shape&                               theOwnerShape)
{
  if (theLayer.IsNull())
  {
    return;
  }
  extractOwnerSupplementAttachments(*theLayer, theOwner, theOwnerShape);
}

//=================================================================================================

void appendFaceRef(BRepGraphInc_Storage&    theStorage,
                   const BRepGraph_ShellId  theOwnerShellId,
                   const BRepGraph_FaceId   theFaceId,
                   const TopAbs_Orientation theOrientation)
{
  theStorage.AttachFaceToShell(theOwnerShellId, theFaceId, theOrientation);
}

//=================================================================================================

void appendWireRef(BRepGraphInc_Storage&    theStorage,
                   const BRepGraph_FaceId   theOwnerFaceId,
                   const BRepGraph_WireId   theWireId,
                   const TopAbs_Orientation theOrientation)
{
  theStorage.AttachWireToFace(theOwnerFaceId, theWireId, theOrientation);
}

//=================================================================================================

BRepGraph_VertexRefId appendVertexRef(BRepGraphInc_Storage&    theStorage,
                                      const BRepGraph_VertexId theVertexId,
                                      const TopAbs_Orientation theOrientation,
                                      const BRepGraph_EdgeId   theParentEdgeId = BRepGraph_EdgeId())
{
  const BRepGraph_VertexRefId aRefId  = theStorage.AppendVertexRef();
  BRepGraphInc::VertexRef&    anEntry = theStorage.ChangeVertexRef(aRefId);
  anEntry.ChildVertexId               = theVertexId;
  anEntry.ParentEdgeId                = theParentEdgeId;
  anEntry.Orientation                 = theOrientation;
  return aRefId;
}

//=================================================================================================

void appendShellRef(BRepGraphInc_Storage&    theStorage,
                    const BRepGraph_SolidId  theOwnerSolidId,
                    const BRepGraph_ShellId  theShellId,
                    const TopAbs_Orientation theOrientation)
{
  theStorage.AttachShellToSolid(theOwnerSolidId, theShellId, theOrientation);
}

//=================================================================================================

void appendSolidRef(BRepGraphInc_Storage&       theStorage,
                    const BRepGraph_CompSolidId theOwnerCompSolidId,
                    const BRepGraph_SolidId     theSolidId,
                    const TopAbs_Orientation    theOrientation)
{
  theStorage.AttachSolidToCompSolid(theOwnerCompSolidId, theSolidId, theOrientation);
}

//=================================================================================================

void appendChildRef(BRepGraphInc_Storage&      theStorage,
                    const BRepGraph_CompoundId theOwnerCompoundId,
                    const BRepGraph_NodeId     theChildId,
                    const TopAbs_Orientation   theOrientation,
                    const TopLoc_Location&     theLocation)
{
  theStorage.AttachChildToCompound(theOwnerCompoundId, theChildId, theLocation, theOrientation);
}

//=================================================================================================

struct ExtractedVertex
{
  TopoDS_Vertex Shape;
  gp_Pnt        Point;
  double        Tolerance = 0.0;
  TopLoc_Location BakedLocation;
  bool          IsGenerated = false;
};

struct ExtractedEdge
{
  TopoDS_Edge                              Shape;
  occ::handle<Geom_Curve>                  Curve3d;
  double                                   ParamFirst    = 0.0;
  double                                   ParamLast     = 0.0;
  double                                   Tolerance     = 0.0;
  ExtractedVertex                          StartVertex;
  ExtractedVertex                          EndVertex;
  TopAbs_Orientation                       OrientationInWire = TopAbs_FORWARD;
  occ::handle<Geom2d_Curve>                PCurve2d;
  double                                   PCFirst = 0.0;
  double                                   PCLast  = 0.0;
  occ::handle<Poly_Polygon3D>              Polygon3D;
  occ::handle<Poly_Polygon2D>              PolyOnSurf;
  occ::handle<Poly_PolygonOnTriangulation> PolyOnTri;
  TopLoc_Location                          BakedLocation;
  bool                                     IsGenerated = false;
};

struct ExtractedWire
{
  TopoDS_Wire                             Shape;
  NCollection_DynamicArray<ExtractedEdge> Edges;
  TopLoc_Location                         BakedLocation;
  bool                                    IsGenerated = false;
};

struct FaceBuildData
{
  TopoDS_Face      Face;
  BRepGraph_FaceId FaceId;

  occ::handle<Geom_Surface>               Surface;
  occ::handle<Poly_Triangulation>         RawTriangulation;
  occ::handle<Poly_Triangulation>         ActiveTriangulation;
  TopLoc_Location                         BakedLocation;
  double                                  Tolerance = 0.0;
  bool                                    UnsupportedNaturalBoundary = false;
  bool                                    NeedsSynthesis = false;
  NCollection_DynamicArray<ExtractedWire> Wires;
};

struct FacePCurveContext
{
  occ::handle<Geom_Surface> RawSurface;
  TopLoc_Location           SurfaceLocation;
};

static bool findStoredPCurve(
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>& theCurveReps,
  const occ::handle<Geom_Surface>&                               theSurface,
  const TopLoc_Location&                                         theLocation,
  const bool                                                     theReversed,
  occ::handle<Geom2d_Curve>&                                     thePCurve,
  double&                                                        theFirst,
  double&                                                        theLast)
{
  for (const occ::handle<BRep_CurveRepresentation>& aCurveRep : theCurveReps)
  {
    if (!aCurveRep->IsCurveOnSurface(theSurface, theLocation))
    {
      continue;
    }
    const occ::handle<BRep_GCurve> aGCurve = occ::down_cast<BRep_GCurve>(aCurveRep);
    if (aGCurve.IsNull())
    {
      return false;
    }

    aGCurve->Range(theFirst, theLast);
    thePCurve =
      aGCurve->IsCurveOnClosedSurface() && theReversed ? aGCurve->PCurve2() : aGCurve->PCurve();
    return true;
  }
  return false;
}

static bool extractStoredPCurve(const TopoDS_Edge&         theEdge,
                                const FacePCurveContext&   theContext,
                                occ::handle<Geom2d_Curve>& thePCurve,
                                double&                    theFirst,
                                double&                    theLast)
{
  if (theContext.RawSurface.IsNull())
  {
    return false;
  }
  const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(theEdge.TShape());
  if (aTEdge.IsNull())
  {
    return false;
  }
  const NCollection_List<occ::handle<BRep_CurveRepresentation>>& aCurves = aTEdge->Curves();
  const bool            aReversed    = (theEdge.Orientation() == TopAbs_REVERSED);
  const TopLoc_Location aExpectedLoc = theContext.SurfaceLocation.Predivided(theEdge.Location());
  return findStoredPCurve(aCurves,
                          theContext.RawSurface,
                          aExpectedLoc,
                          aReversed,
                          thePCurve,
                          theFirst,
                          theLast);
}

bool rawVertexPoint(const TopoDS_Vertex& theVertex, gp_Pnt& thePoint)
{
  const occ::handle<Standard_Transient>& aTShape = theVertex.TShape();
  if (aTShape.IsNull())
  {
    return false;
  }
  thePoint = static_cast<const BRep_TVertex*>(aTShape.get())->Pnt();
  return true;
}

bool isForwardChildType(const TopAbs_ShapeEnum theOwnerType, const TopAbs_ShapeEnum theChildType)
{
  switch (theOwnerType)
  {
    case TopAbs_COMPOUND:
      return theChildType != TopAbs_SHAPE;
    case TopAbs_COMPSOLID:
      return theChildType == TopAbs_SOLID;
    case TopAbs_SOLID:
      return theChildType == TopAbs_SHELL;
    case TopAbs_SHELL:
      return theChildType == TopAbs_FACE;
    case TopAbs_WIRE:
      return theChildType == TopAbs_EDGE;
    default:
      return false;
  }
}

bool isCoreParityOrientation(const TopAbs_Orientation theOrientation)
{
  return theOrientation == TopAbs_FORWARD || theOrientation == TopAbs_REVERSED;
}

BRepGraph_LayerTopoSupplement::AttachmentKind containerSupplementKind(
  const TopAbs_ShapeEnum theOwnerType)
{
  switch (theOwnerType)
  {
    case TopAbs_COMPOUND:
      return BRepGraph_LayerTopoSupplement::AttachmentKind::CompoundAuxShape;
    case TopAbs_COMPSOLID:
      return BRepGraph_LayerTopoSupplement::AttachmentKind::CompSolidAuxShape;
    case TopAbs_SOLID:
      return BRepGraph_LayerTopoSupplement::AttachmentKind::SolidAuxShape;
    case TopAbs_SHELL:
      return BRepGraph_LayerTopoSupplement::AttachmentKind::ShellAuxShape;
    default:
      return BRepGraph_LayerTopoSupplement::AttachmentKind::GenericSupplementShape;
  }
}

template <typename T>
static occ::handle<T> applyBakedLocation(const occ::handle<T>& theGeom,
                                         const TopLoc_Location& theLocation)
{
  if (theGeom.IsNull() || theLocation.IsIdentity())
  {
    return theGeom;
  }
  return occ::down_cast<T>(theGeom->Transformed(theLocation.Transformation()));
}

static gp_Pnt applyBakedLocation(const gp_Pnt& thePoint, const TopLoc_Location& theLocation)
{
  return theLocation.IsIdentity() ? thePoint : thePoint.Transformed(theLocation.Transformation());
}

static occ::handle<Poly_Polygon3D> applyBakedLocationToPolygon3D(
  const occ::handle<Poly_Polygon3D>& thePolygon3D,
  const TopLoc_Location&             theLocation)
{
  if (thePolygon3D.IsNull() || theLocation.IsIdentity())
  {
    return thePolygon3D;
  }

  const gp_Trsf&                    aTrsf  = theLocation.Transformation();
  const NCollection_Array1<gp_Pnt>& aNodes = thePolygon3D->Nodes();
  NCollection_Array1<gp_Pnt>        aNewNodes(aNodes.Size());
  for (size_t aNodeIdx = 0; aNodeIdx < aNodes.Size(); ++aNodeIdx)
  {
    aNewNodes.ChangeAt(aNodeIdx) = aNodes.At(aNodeIdx).Transformed(aTrsf);
  }
  occ::handle<Poly_Polygon3D> aTransPoly;
  if (thePolygon3D->HasParameters())
  {
    const NCollection_Array1<double>& aParams = thePolygon3D->Parameters();
    NCollection_Array1<double>        aNewParams(aParams.Size());
    for (size_t aParamIdx = 0; aParamIdx < aParams.Size(); ++aParamIdx)
    {
      aNewParams.ChangeAt(aParamIdx) = aParams.At(aParamIdx);
    }
    aTransPoly = new Poly_Polygon3D(aNewNodes, aNewParams);
  }
  else
  {
    aTransPoly = new Poly_Polygon3D(aNewNodes);
  }
  aTransPoly->Deflection(thePolygon3D->Deflection());
  return aTransPoly;
}

static occ::handle<Poly_Triangulation> applyBakedLocationToTriangulation(
  const occ::handle<Poly_Triangulation>& theTriangulation,
  const TopLoc_Location&                 theLocation)
{
  if (theTriangulation.IsNull() || theLocation.IsIdentity())
  {
    return theTriangulation;
  }

  occ::handle<Poly_Triangulation> aCopy = theTriangulation->Copy();
  const gp_Trsf&                  aTrsf = theLocation.Transformation();
  Poly_ArrayOfNodes&              aNodes = aCopy->InternalNodes();
  for (int aNodeIdx = 0; aNodeIdx < aNodes.Length(); ++aNodeIdx)
  {
    aNodes.SetValue(aNodeIdx, aNodes.Value(aNodeIdx).Transformed(aTrsf));
  }
  if (aCopy->HasNormals())
  {
    for (int aNodeIdx = 1; aNodeIdx <= aCopy->NbNodes(); ++aNodeIdx)
    {
      aCopy->SetNormal(aNodeIdx, aCopy->Normal(aNodeIdx).Transformed(aTrsf));
    }
  }
  return aCopy;
}

struct LocatedNodeBinding
{
  const TopoDS_TShape* TShape = nullptr;
  TopLoc_Location     Location;
  BRepGraph_NodeId    Node;
};

enum class RootRole
{
  Nested,
  Root
};

struct BuildContext;

BRepGraph_NodeId findExistingNode(const BuildContext&        theBuild,
                                  const TopoDS_Shape&        theShape,
                                  BRepGraph_NodeId::Kind     theExpectedKind,
                                  const TopLoc_Location&     theBakedLocation);

void bindLocatedNode(BuildContext&            theBuild,
                     const TopoDS_Shape&      theShape,
                     const BRepGraph_NodeId   theNodeId,
                     const TopLoc_Location&   theBakedLocation,
                     const bool               theBindTShape);

enum class TopologyBuildMode
{
  FullHierarchy,
  Flattened
};

struct BuildContext
{
  BuildContext(BRepGraphInc_Storage&                             theStorage,
               const bool                                        theParallel,
               const TopologyBuildMode                           theMode,
               const occ::handle<BRepGraph_LayerTopoSupplement>& theSupplementLayer,
               const occ::handle<NCollection_IncAllocator>&      theScratchAlloc,
               NCollection_LinearVector<BRepGraph_NodeId>*       theAppendedRoots = nullptr)
      : Storage(theStorage),
        Parallel(theParallel),
        Mode(theMode),
        SupplementLayer(theSupplementLayer),
        ScratchAlloc(theScratchAlloc),
        AppendedRoots(theAppendedRoots),
        AppendedRootSet(THE_SMALL_MAP_BUCKET, theScratchAlloc),
        PendingFaces(THE_ARRAY_BUCKET, theScratchAlloc),
        LocatedNodes(THE_ARRAY_BUCKET, theScratchAlloc)
  {
  }

  BRepGraphInc_Storage&                       Storage;
  bool                                        Parallel;
  TopologyBuildMode                           Mode;
  occ::handle<BRepGraph_LayerTopoSupplement>  SupplementLayer;
  occ::handle<NCollection_IncAllocator>       ScratchAlloc;
  NCollection_LinearVector<BRepGraph_NodeId>* AppendedRoots = nullptr;
  NCollection_Map<BRepGraph_NodeId>           AppendedRootSet;
  NCollection_DynamicArray<FaceBuildData>     PendingFaces;
  NCollection_DynamicArray<LocatedNodeBinding> LocatedNodes;
  NCollection_FlatDataMap<const TopoDS_TShape*, NCollection_LinearVector<size_t>> LocatedNodeIndex;
};

BRepGraph_NodeId findExistingNode(const BuildContext&        theBuild,
                                  const TopoDS_Shape&        theShape,
                                  BRepGraph_NodeId::Kind     theExpectedKind,
                                  const TopLoc_Location&     theBakedLocation)
{
  const TopoDS_TShape* aTShape = theShape.TShape().get();
  if (theBakedLocation.IsIdentity())
  {
    const BRepGraph_NodeId* aStoredNode = theBuild.Storage.FindNodeByTShape(aTShape);
    if (aStoredNode != nullptr && aStoredNode->NodeKind == theExpectedKind)
    {
      return *aStoredNode;
    }
  }

  const NCollection_LinearVector<size_t>* anIndices = theBuild.LocatedNodeIndex.Seek(aTShape);
  if (anIndices == nullptr)
  {
    return BRepGraph_NodeId();
  }
  for (const size_t aBindingIndex : *anIndices)
  {
    const LocatedNodeBinding& aBinding = theBuild.LocatedNodes.Value(aBindingIndex);
    if (aBinding.TShape == aTShape && aBinding.Node.NodeKind == theExpectedKind
        && aBinding.Location.IsEqual(theBakedLocation))
    {
      return aBinding.Node;
    }
  }
  return BRepGraph_NodeId();
}

void bindLocatedNode(BuildContext&            theBuild,
                     const TopoDS_Shape&      theShape,
                     const BRepGraph_NodeId   theNodeId,
                     const TopLoc_Location&   theBakedLocation,
                     const bool               theBindTShape)
{
  const TopoDS_TShape* aTShape = theShape.TShape().get();
  const size_t         aBindingIndex = theBuild.LocatedNodes.Size();
  LocatedNodeBinding& aBinding = theBuild.LocatedNodes.Appended();
  aBinding.TShape              = aTShape;
  aBinding.Location            = theBakedLocation;
  aBinding.Node                = theNodeId;

  if (!theBuild.LocatedNodeIndex.IsBound(aTShape))
  {
    theBuild.LocatedNodeIndex.Bind(aTShape, NCollection_LinearVector<size_t>());
  }
  theBuild.LocatedNodeIndex.ChangeFind(aTShape).Append(aBindingIndex);

  if (theBindTShape && theBakedLocation.IsIdentity() && !theBuild.Storage.HasTShapeBinding(aTShape))
  {
    theBuild.Storage.BindTShapeToNode(aTShape, theNodeId);
  }
}

BRepGraph_VertexId registerOrReuseVertex(BuildContext&        theBuild,
                                          const TopoDS_Vertex& theVertex,
                                          const gp_Pnt&        thePoint,
                                          const double         theTolerance,
                                          const TopLoc_Location& theBakedLocation,
                                          const bool             theIsGenerated = false)
{
  if (theVertex.IsNull())
  {
    return BRepGraph_VertexId();
  }
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theVertex, BRepGraph_NodeId::Kind::Vertex, theBakedLocation);
  if (anExisting.IsValid())
  {
    return BRepGraph_VertexId(anExisting);
  }

  const BRepGraph_VertexId aVtxId  = theBuild.Storage.AppendVertex();
  BRepGraphInc::VertexDef& aVtxEnt = theBuild.Storage.ChangeVertex(aVtxId);
  aVtxEnt.Point                    = thePoint;
  aVtxEnt.Tolerance                = theTolerance;
  if (!theIsGenerated)
  {
    bindLocatedNode(theBuild, theVertex, aVtxId, theBakedLocation, true);
    theBuild.Storage.BindOriginal(aVtxId, theVertex);
  }
  return aVtxId;
}

BRepGraph_VertexId registerOrReuseVertex(BuildContext&        theBuild,
                                          const TopoDS_Vertex& theVertex,
                                          const TopLoc_Location& theBakedLocation)
{
  if (theVertex.IsNull())
  {
    return BRepGraph_VertexId();
  }
  gp_Pnt aPoint;
  if (!rawVertexPoint(theVertex, aPoint))
  {
    return BRepGraph_VertexId();
  }
  return registerOrReuseVertex(theBuild,
                               theVertex,
                               applyBakedLocation(aPoint, theBakedLocation),
                               BRep_Tool::Tolerance(theVertex),
                               theBakedLocation);
}

bool attachNonCoreContainerChild(BuildContext&             theBuild,
                                 const BRepGraph_NodeId   theOwner,
                                 const TopAbs_ShapeEnum   theOwnerType,
                                 const TopoDS_Shape&      theChild)
{
  if (theBuild.SupplementLayer.IsNull())
  {
    return false;
  }
  return theBuild.SupplementLayer->AddAttachment(theOwner,
                                                 containerSupplementKind(theOwnerType),
                                                 theChild)
         != 0;
}

BuildCounts captureBuildCounts(const BRepGraphInc_Storage& theStorage)
{
  BuildCounts aCounts;
  aCounts.NbVertices   = theStorage.NbVertices();
  aCounts.NbEdges      = theStorage.NbEdges();
  aCounts.NbWires      = theStorage.NbWires();
  aCounts.NbFaces      = theStorage.NbFaces();
  aCounts.NbShells     = theStorage.NbShells();
  aCounts.NbSolids     = theStorage.NbSolids();
  aCounts.NbCompounds  = theStorage.NbCompounds();
  aCounts.NbCompSolids = theStorage.NbCompSolids();
  aCounts.NbCoEdges    = theStorage.NbCoEdges();
  aCounts.NbChildRefs  = theStorage.NbChildRefs();
  aCounts.NbSolidRefs  = theStorage.NbSolidRefs();
  aCounts.NbFaceRefs   = theStorage.NbFaceRefs();
  aCounts.NbShellRefs  = theStorage.NbShellRefs();
  aCounts.NbWireRefs   = theStorage.NbWireRefs();
  return aCounts;
}

static BRepGraph_VertexRefId appendEdgeVertexRef(BuildContext&             theBuild,
                                                 const ExtractedVertex&   theVertex,
                                                 const TopAbs_Orientation theOrientation,
                                                 const BRepGraph_EdgeId   theParentEdgeId)
{
  if (theVertex.Shape.IsNull())
  {
    return BRepGraph_VertexRefId();
  }

  const BRepGraph_VertexId aVertexId =
    registerOrReuseVertex(theBuild,
                          theVertex.Shape,
                          theVertex.Point,
                          theVertex.Tolerance,
                          theVertex.BakedLocation,
                          theVertex.IsGenerated);
  return appendVertexRef(theBuild.Storage, aVertexId, theOrientation, theParentEdgeId);
}

BRepGraph_EdgeId registerEdge(BuildContext&                                      theBuild,
                              const ExtractedEdge&                              theEdgeData,
                              const occ::handle<BRepGraph_LayerTopoSupplement>& theSupplementLayer)
{
  if (theEdgeData.IsGenerated)
  {
    // Generated edges are not deduplicated; each gets its own representation.
  }
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild,
                     theEdgeData.Shape,
                     BRepGraph_NodeId::Kind::Edge,
                     theEdgeData.BakedLocation);
  if (anExisting.IsValid())
  {
    const BRepGraph_EdgeId anEdgeId(anExisting);
    BRepGraphInc::EdgeDef& anEdgeEnt = theBuild.Storage.ChangeEdge(anEdgeId);
    if (!anEdgeEnt.Polygon3DRepId.IsValid() && !theEdgeData.Polygon3D.IsNull())
    {
      const BRepGraph_EdgePolygon3DRepId aRepId = theBuild.Storage.AppendEdgePolygon3DRep();
      theBuild.Storage.ChangeEdgePolygon3DRep(aRepId).Polygon = theEdgeData.Polygon3D;
      anEdgeEnt.Polygon3DRepId = aRepId;
      if (anEdgeEnt.Polygon3DRepId.IsValid())
      {
        theBuild.Storage.ChangeEdgePolygon3DRep(anEdgeEnt.Polygon3DRepId).ParentEdgeId = anEdgeId;
      }
    }
    return anEdgeId;
  }

  const BRepGraph_EdgeId anEdgeId  = theBuild.Storage.AppendEdge();
  BRepGraphInc::EdgeDef& anEdgeEnt = theBuild.Storage.ChangeEdge(anEdgeId);
  anEdgeEnt.Tolerance              = theEdgeData.Tolerance;

  if (!theEdgeData.Curve3d.IsNull())
  {
    const BRepGraph_EdgeCurve3DRepId aRepId = theBuild.Storage.AppendEdgeCurve3DRep();
    theBuild.Storage.ChangeEdgeCurve3DRep(aRepId).Curve = theEdgeData.Curve3d;
    anEdgeEnt.Curve3DRepId = aRepId;
    if (anEdgeEnt.Curve3DRepId.IsValid())
    {
      BRepGraphInc::EdgeCurve3DRep& aUse = theBuild.Storage.ChangeEdgeCurve3DRep(anEdgeEnt.Curve3DRepId);
      aUse.ParentEdgeId = anEdgeId;
      aUse.ParamFirst   = theEdgeData.ParamFirst;
      aUse.ParamLast    = theEdgeData.ParamLast;
    }
  }
  anEdgeEnt.StartVertexRefId =
    appendEdgeVertexRef(theBuild, theEdgeData.StartVertex, TopAbs_FORWARD, anEdgeId);
  anEdgeEnt.EndVertexRefId =
    appendEdgeVertexRef(theBuild, theEdgeData.EndVertex, TopAbs_REVERSED, anEdgeId);
  const auto anAppendVertexRelation = [&](const BRepGraph_VertexRefId theRefId) {
    if (!theRefId.IsValid())
    {
      return;
    }
    const BRepGraph_VertexId aVertexId = theBuild.Storage.VertexRef(theRefId).ChildVertexId;
    if (aVertexId.IsValid())
    {
      theBuild.Storage.AttachEdgeToVertex(anEdgeId, aVertexId);
    }
  };
  anAppendVertexRelation(anEdgeEnt.StartVertexRefId);
  anAppendVertexRelation(anEdgeEnt.EndVertexRefId);
  if (!theEdgeData.Polygon3D.IsNull())
  {
    const BRepGraph_EdgePolygon3DRepId aPolyRepId = theBuild.Storage.AppendEdgePolygon3DRep();
    theBuild.Storage.ChangeEdgePolygon3DRep(aPolyRepId).Polygon = theEdgeData.Polygon3D;
    anEdgeEnt.Polygon3DRepId = aPolyRepId;
    if (anEdgeEnt.Polygon3DRepId.IsValid())
    {
      theBuild.Storage.ChangeEdgePolygon3DRep(anEdgeEnt.Polygon3DRepId).ParentEdgeId = anEdgeId;
    }
  }

  if (!theEdgeData.IsGenerated)
  {
    bindLocatedNode(theBuild, theEdgeData.Shape, anEdgeId, theEdgeData.BakedLocation, true);
    theBuild.Storage.BindOriginal(anEdgeId, theEdgeData.Shape);
    attachSupplement(theSupplementLayer, anEdgeId, theEdgeData.Shape);
  }
  return anEdgeId;
}

BRepGraph_WireId appendWireDef(BuildContext&          theBuild,
                               const TopoDS_Wire&    theWire,
                               const bool            theBindTShape,
                               const TopLoc_Location& theBakedLocation,
                               const bool            theIsGenerated = false)
{
  const BRepGraph_WireId aWireId          = theBuild.Storage.AppendWire();
  if (!theIsGenerated && theBindTShape)
  {
    bindLocatedNode(theBuild, theWire, aWireId, theBakedLocation, true);
  }
  if (!theIsGenerated)
  {
    theBuild.Storage.BindOriginal(aWireId, theWire);
  }
  return aWireId;
}

void appendFaceCoEdge(BRepGraphInc_Storage&              theStorage,
                      const BRepGraph_WireId             theWireId,
                      const BRepGraph_FaceId             theFaceId,
                      const ExtractedEdge&               theEdgeData,
                      const BRepGraph_EdgeId             theEdgeId)
{
  const BRepGraph_CoEdgeId aCoEdgeId = theStorage.CreateCoEdgeUse(theWireId,
                                                                  theEdgeId,
                                                                  theFaceId,
                                                                  theEdgeData.OrientationInWire);
  BRepGraphInc::CoEdgeDef& aCoEdge   = theStorage.ChangeCoEdge(aCoEdgeId);
  if (!theEdgeData.PolyOnSurf.IsNull())
  {
    const BRepGraph_CoEdgePolygon2DRepId aRepId = theStorage.AppendCoEdgePolygon2DRep();
    theStorage.ChangeCoEdgePolygon2DRep(aRepId).Polygon = theEdgeData.PolyOnSurf;
    aCoEdge.Polygon2DRepId = aRepId;
    if (aCoEdge.Polygon2DRepId.IsValid())
    {
      theStorage.ChangeCoEdgePolygon2DRep(aCoEdge.Polygon2DRepId).ParentCoEdgeId = aCoEdgeId;
    }
  }

  if (!theEdgeData.PCurve2d.IsNull())
  {
    const BRepGraph_CoEdgeCurve2DRepId aCurveRepId = theStorage.AppendCoEdgeCurve2DRep();
    theStorage.ChangeCoEdgeCurve2DRep(aCurveRepId).Curve = theEdgeData.PCurve2d;
    aCoEdge.Curve2DRepId = aCurveRepId;
    if (aCoEdge.Curve2DRepId.IsValid())
    {
      BRepGraphInc::CoEdgeCurve2DRep& aUse = theStorage.ChangeCoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
      aUse.ParentCoEdgeId = aCoEdgeId;
      aUse.ParamFirst     = theEdgeData.PCFirst;
      aUse.ParamLast      = theEdgeData.PCLast;
    }
  }

  if (!theEdgeData.PolyOnTri.IsNull())
  {
    const BRepGraph_CoEdgePolygonOnTriRepId aTriRepId = theStorage.AppendCoEdgePolygonOnTriRep();
    theStorage.ChangeCoEdgePolygonOnTriRep(aTriRepId).Polygon = theEdgeData.PolyOnTri;
    aCoEdge.PolygonOnTriRepId = aTriRepId;
    if (aCoEdge.PolygonOnTriRepId.IsValid())
    {
      theStorage.ChangeCoEdgePolygonOnTriRep(aCoEdge.PolygonOnTriRepId).ParentCoEdgeId = aCoEdgeId;
    }
  }
}

void appendWireCoEdge(BRepGraphInc_Storage&  theStorage,
                      const BRepGraph_WireId theWireId,
                      const BRepGraph_EdgeId theEdgeId,
                      const TopoDS_Edge&     theEdge)
{
  theStorage.CreateCoEdgeUse(theWireId,
                             theEdgeId,
                             BRepGraph_FaceId(),
                             theEdge.Orientation());
}

static void edgeVertices(const TopoDS_Edge& theEdge,
                         TopoDS_Vertex&     theFirst,
                         TopoDS_Vertex&     theLast)
{
  for (TopoDS_Iterator aVIt(theEdge, false, false); aVIt.More(); aVIt.Next())
  {
    if (aVIt.Value().ShapeType() != TopAbs_VERTEX)
    {
      continue;
    }
    const TopoDS_Vertex aVertex = TopoDS::Vertex(aVIt.Value());
    if (aVertex.Orientation() == TopAbs_FORWARD)
    {
      theFirst = aVertex;
    }
    else if (aVertex.Orientation() == TopAbs_REVERSED)
    {
      theLast = aVertex;
    }
  }
}

static void extractEdgeDefinition(ExtractedEdge&         theEdgeData,
                                  const TopoDS_Edge&     theEdge,
                                  const TopLoc_Location& theParentLocation)
{
  theEdgeData.Shape             = theEdge;
  theEdgeData.Tolerance         = BRep_Tool::Tolerance(theEdge);
  theEdgeData.OrientationInWire = theEdge.Orientation();
  theEdgeData.BakedLocation     = theParentLocation * theEdge.Location();

  {
    double          aFirst = 0.0, aLast = 0.0;
    TopLoc_Location aCurveCombinedLoc;
    theEdgeData.Curve3d    = BRep_Tool::Curve(theEdge, aCurveCombinedLoc, aFirst, aLast);
    theEdgeData.ParamFirst = aFirst;
    theEdgeData.ParamLast  = aLast;
    theEdgeData.Curve3d =
      applyBakedLocation<Geom_Curve>(theEdgeData.Curve3d, theParentLocation * aCurveCombinedLoc);
  }

  TopoDS_Vertex aVFirst, aVLast;
  edgeVertices(TopoDS::Edge(theEdge.Oriented(TopAbs_FORWARD)), aVFirst, aVLast);
  gp_Pnt aVertexPoint;
  if (!aVFirst.IsNull() && rawVertexPoint(aVFirst, aVertexPoint))
  {
    theEdgeData.StartVertex.Shape     = aVFirst;
    theEdgeData.StartVertex.BakedLocation = theEdgeData.BakedLocation * aVFirst.Location();
    theEdgeData.StartVertex.Point =
      applyBakedLocation(aVertexPoint, theEdgeData.StartVertex.BakedLocation);
    theEdgeData.StartVertex.Tolerance = BRep_Tool::Tolerance(aVFirst);
  }
  if (!aVLast.IsNull() && rawVertexPoint(aVLast, aVertexPoint))
  {
    theEdgeData.EndVertex.Shape     = aVLast;
    theEdgeData.EndVertex.BakedLocation = theEdgeData.BakedLocation * aVLast.Location();
    theEdgeData.EndVertex.Point =
      applyBakedLocation(aVertexPoint, theEdgeData.EndVertex.BakedLocation);
    theEdgeData.EndVertex.Tolerance = BRep_Tool::Tolerance(aVLast);
  }

  TopLoc_Location aPoly3DLoc;
  theEdgeData.Polygon3D = BRep_Tool::Polygon3D(theEdge, aPoly3DLoc);
  theEdgeData.Polygon3D =
    applyBakedLocationToPolygon3D(theEdgeData.Polygon3D, theParentLocation * aPoly3DLoc);
}

static void extractEdgeInFace(ExtractedEdge&                         theEdgeData,
                              const TopoDS_Edge&                     theEdge,
                              const TopoDS_Face&                     theForwardFace,
                              const occ::handle<Poly_Triangulation>& theTriangulation,
                              const FacePCurveContext&               thePCurveContext,
                              const TopLoc_Location&                 theParentLocation,
                              const bool                             theIsGenerated = false)
{
  extractEdgeDefinition(theEdgeData, theEdge, theParentLocation);
  theEdgeData.IsGenerated             = theIsGenerated;
  theEdgeData.StartVertex.IsGenerated = theIsGenerated;
  theEdgeData.EndVertex.IsGenerated   = theIsGenerated;
  extractStoredPCurve(theEdge,
                      thePCurveContext,
                      theEdgeData.PCurve2d,
                      theEdgeData.PCFirst,
                      theEdgeData.PCLast);

  theEdgeData.PolyOnSurf = BRep_Tool::PolygonOnSurface(theEdge, theForwardFace);
  if (!theTriangulation.IsNull())
  {
    TopLoc_Location aPolyTriLoc;
    theEdgeData.PolyOnTri =
      BRep_Tool::PolygonOnTriangulation(theEdge, theTriangulation, aPolyTriLoc);
  }
}

static bool hasInfiniteRequiredBound(const occ::handle<Geom_Surface>& theSurface)
{
  double aU1 = 0.0;
  double aU2 = 0.0;
  double aV1 = 0.0;
  double aV2 = 0.0;
  theSurface->Bounds(aU1, aU2, aV1, aV2);

  if (!theSurface->IsUPeriodic() && (Precision::IsInfinite(aU1) || Precision::IsInfinite(aU2)))
  {
    return true;
  }
  if (!theSurface->IsVPeriodic() && (Precision::IsInfinite(aV1) || Precision::IsInfinite(aV2)))
  {
    return true;
  }
  return false;
}

void extractFaceData(FaceBuildData&                               theData,
                     const occ::handle<NCollection_IncAllocator>& theScratchAlloc)
{
  const TopoDS_Face& aFace = theData.Face;
  const TopLoc_Location aFaceParentLocation =
    theData.BakedLocation * aFace.Location().Inverted();

  TopLoc_Location           aSurfCombinedLoc;
  occ::handle<Geom_Surface> aRawSurface = BRep_Tool::Surface(aFace, aSurfCombinedLoc);
  theData.Surface =
    applyBakedLocation<Geom_Surface>(aRawSurface, aFaceParentLocation * aSurfCombinedLoc);

  TopLoc_Location aTriangulationLoc;
  theData.RawTriangulation    = BRep_Tool::Triangulation(aFace, aTriangulationLoc);
  theData.ActiveTriangulation =
    applyBakedLocationToTriangulation(theData.RawTriangulation,
                                      aFaceParentLocation * aTriangulationLoc);

  theData.Tolerance = BRep_Tool::Tolerance(aFace);

  const TopoDS_Face aForwardFace = TopoDS::Face(aFace.Oriented(TopAbs_FORWARD));
  FacePCurveContext aPCurveContext;
  aPCurveContext.RawSurface      = aRawSurface;
  aPCurveContext.SurfaceLocation = aSurfCombinedLoc;

  for (TopoDS_Iterator aChildIt(aForwardFace, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (aChild.ShapeType() != TopAbs_WIRE)
    {
      continue;
    }
    const TopoDS_Wire& aWire = TopoDS::Wire(aChild);

    ExtractedWire& aWireData = theData.Wires.Appended();
    aWireData.Shape          = aWire;
    aWireData.BakedLocation  = theData.BakedLocation * aWire.Location();
    aWireData.Edges =
      NCollection_DynamicArray<ExtractedEdge>(THE_SMALL_ARRAY_BUCKET, theScratchAlloc);

    for (TopoDS_Iterator anEdgeIt(aWire, false, false); anEdgeIt.More(); anEdgeIt.Next())
    {
      const TopoDS_Shape& anEdgeShape = anEdgeIt.Value();
      if (anEdgeShape.ShapeType() != TopAbs_EDGE)
      {
        continue;
      }
      ExtractedEdge& anEdgeData = aWireData.Edges.Appended();
      extractEdgeInFace(anEdgeData,
                        TopoDS::Edge(anEdgeShape),
                        aForwardFace,
                        theData.RawTriangulation,
                        aPCurveContext,
                        aWireData.BakedLocation);
    }
  }

  if (theData.Wires.IsEmpty() && BRep_Tool::NaturalRestriction(aFace))
  {
    if (theData.Surface.IsNull() || hasInfiniteRequiredBound(theData.Surface))
    {
      theData.UnsupportedNaturalBoundary = true;
    }
    else
    {
      theData.NeedsSynthesis = true;
    }
  }
}

void registerFaceData(BuildContext&                                      theBuild,
                      const NCollection_DynamicArray<FaceBuildData>&    theFaceData,
                      const occ::handle<BRepGraph_LayerTopoSupplement>& theSupplementLayer)
{
  BRepGraphInc_Storage& theStorage = theBuild.Storage;
  for (const FaceBuildData& aData : theFaceData)
  {
    const BRepGraph_FaceId aFaceId = aData.FaceId;
    BRepGraphInc::FaceDef& aFace   = theStorage.ChangeFace(aFaceId);
    aFace.Tolerance    = aData.Tolerance;
    if (!aData.Surface.IsNull())
    {
      const BRepGraph_FaceSurfaceRepId aRepId = theStorage.AppendFaceSurfaceRep();
      theStorage.ChangeFaceSurfaceRep(aRepId).Surface = aData.Surface;
      aFace.SurfaceRepId = aRepId;
      if (aFace.SurfaceRepId.IsValid())
      {
        theStorage.ChangeFaceSurfaceRep(aFace.SurfaceRepId).ParentFaceId = aFaceId;
      }
    }

    if (!aData.ActiveTriangulation.IsNull())
    {
      const BRepGraph_FaceTriangulationRepId aTriRepId = theStorage.AppendFaceTriangulationRep();
      theStorage.ChangeFaceTriangulationRep(aTriRepId).Triangulation = aData.ActiveTriangulation;
      aFace.TriangulationRepId = aTriRepId;
      if (aFace.TriangulationRepId.IsValid())
      {
        theStorage.ChangeFaceTriangulationRep(aFace.TriangulationRepId).ParentFaceId = aFaceId;
      }
    }
    for (const ExtractedWire& aWireData : aData.Wires)
    {
      const BRepGraph_WireId aWireId =
        appendWireDef(theBuild, aWireData.Shape, false, aWireData.BakedLocation, aWireData.IsGenerated);
      appendWireRef(theStorage,
                    aFaceId,
                    aWireId,
                    aWireData.Shape.Orientation());

      for (const ExtractedEdge& anEdgeData : aWireData.Edges)
      {
        const BRepGraph_EdgeId anEdgeId =
          registerEdge(theBuild, anEdgeData, theSupplementLayer);
        appendFaceCoEdge(theStorage,
                         aWireId,
                         aFaceId,
                          anEdgeData,
                          anEdgeId);
      }
      theStorage.CanonicalizeWireCoEdgeOrder(aWireId);
    }
  }
}

void synthesizeFaceBoundaries(BRepGraphInc_Storage&                             theStorage,
                               NCollection_DynamicArray<FaceBuildData>&          theFaceData)
{
  for (FaceBuildData& aData : theFaceData)
  {
    if (!aData.NeedsSynthesis)
    {
      continue;
    }

    const occ::handle<Geom_Surface>& aSurf = aData.Surface;
    const double                     aTol  = std::max(aData.Tolerance, Precision::Confusion());

    double aU1 = 0.0;
    double aU2 = 0.0;
    double aV1 = 0.0;
    double aV2 = 0.0;
    aSurf->Bounds(aU1, aU2, aV1, aV2);

    // For planes PCurves are trivially derivable from the 3D curve.
    // Check if surface is a plane or a trimmed surface with a plane basis.
    const bool isPlane = aSurf->IsKind(STANDARD_TYPE(Geom_Plane))
                      || (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))
                          && static_cast<const Geom_RectangularTrimmedSurface*>(aSurf.get())
                               ->BasisSurface()
                               ->IsKind(STANDARD_TYPE(Geom_Plane)));

    BRepGraphInc_BoundaryBuilder::SurfaceBoundary aBoundary;
    if (!BRepGraphInc_BoundaryBuilder::BuildSurfaceBoundary(aBoundary,
                                                            aSurf,
                                                            aU1,
                                                            aU2,
                                                            aV1,
                                                            aV2,
                                                            aTol))
    {
      continue;
    }

    NCollection_LinearVector<BRepGraph_VertexId> aVertices;
    aVertices.Reserve(aBoundary.Vertices.Size());
    for (const BRepGraphInc_BoundaryBuilder::SurfaceBoundaryVertex& aBoundaryVertex
         : aBoundary.Vertices)
    {
      const BRepGraph_VertexId aVtxId = theStorage.AppendVertex();
      BRepGraphInc::VertexDef& aVtxEnt = theStorage.ChangeVertex(aVtxId);
      aVtxEnt.Point     = aBoundaryVertex.Point;
      aVtxEnt.Tolerance = aTol;
      aVertices.Append(aVtxId);
    }

    NCollection_LinearVector<BRepGraph_EdgeId> aEdges;
    aEdges.Reserve(aBoundary.Edges.Size());
    for (const BRepGraphInc_BoundaryBuilder::SurfaceBoundaryEdge& aBoundaryEdge
         : aBoundary.Edges)
    {
      BRepGraph_EdgeCurve3DRepId aCurveRepId;
      if (!aBoundaryEdge.Curve3D.IsNull())
      {
        aCurveRepId = theStorage.AppendEdgeCurve3DRep();
        theStorage.ChangeEdgeCurve3DRep(aCurveRepId).Curve = aBoundaryEdge.Curve3D;
      }

      const BRepGraph_VertexId aStartVertex = aVertices.Value(aBoundaryEdge.StartVertex);
      const BRepGraph_VertexId anEndVertex  = aVertices.Value(aBoundaryEdge.EndVertex);
      const BRepGraph_EdgeId anEdgeId = theStorage.AppendEdge();
      BRepGraphInc::EdgeDef& anEdgeEnt = theStorage.ChangeEdge(anEdgeId);
      anEdgeEnt.Curve3DRepId  = aCurveRepId;
      anEdgeEnt.Tolerance     = aTol;
      if (anEdgeEnt.Curve3DRepId.IsValid())
      {
        BRepGraphInc::EdgeCurve3DRep& aUse = theStorage.ChangeEdgeCurve3DRep(anEdgeEnt.Curve3DRepId);
        aUse.ParentEdgeId = anEdgeId;
        aUse.ParamFirst   = aBoundaryEdge.First;
        aUse.ParamLast    = aBoundaryEdge.Last;
      }

      anEdgeEnt.StartVertexRefId =
        appendVertexRef(theStorage, aStartVertex, TopAbs_FORWARD, anEdgeId);
      anEdgeEnt.EndVertexRefId =
        appendVertexRef(theStorage, anEndVertex, TopAbs_REVERSED, anEdgeId);

      theStorage.AttachEdgeToVertex(anEdgeId, aStartVertex);
      if (aStartVertex != anEndVertex)
      {
        theStorage.AttachEdgeToVertex(anEdgeId, anEndVertex);
      }
      aEdges.Append(anEdgeId);
    }

    // Create wire.
    const BRepGraph_WireId aWireId = theStorage.AppendWire();

    // Create CoEdges and attach to wire.
    for (const BRepGraphInc_BoundaryBuilder::SurfaceBoundaryCoEdge& aBoundaryCoEdge
         : aBoundary.CoEdges)
    {
      const BRepGraph_CoEdgeId aCoEdgeId =
        theStorage.CreateCoEdgeUse(aWireId,
                                   aEdges.Value(aBoundaryCoEdge.EdgeIndex),
                                   aData.FaceId,
                                   aBoundaryCoEdge.Orientation);
      BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.ChangeCoEdge(aCoEdgeId);

      if (!isPlane && !aBoundaryCoEdge.PCurve.IsNull())
      {
        const BRepGraph_CoEdgeCurve2DRepId aPCurveRepId = theStorage.AppendCoEdgeCurve2DRep();
        theStorage.ChangeCoEdgeCurve2DRep(aPCurveRepId).Curve = aBoundaryCoEdge.PCurve;
        aCoEdge.Curve2DRepId = aPCurveRepId;
        if (aCoEdge.Curve2DRepId.IsValid())
        {
          BRepGraphInc::CoEdgeCurve2DRep& aUse = theStorage.ChangeCoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
          aUse.ParentCoEdgeId = aCoEdgeId;
          aUse.ParamFirst     = aBoundaryCoEdge.First;
          aUse.ParamLast      = aBoundaryCoEdge.Last;
        }
      }
    }

    // Attach wire to face.
    theStorage.CanonicalizeWireCoEdgeOrder(aWireId);
    theStorage.AttachWireToFace(aData.FaceId, aWireId, TopAbs_FORWARD);
  }
}

void appendBuildRoot(BuildContext& theBuild, const BRepGraph_NodeId theNodeId)
{
  if (theBuild.AppendedRoots != nullptr && theNodeId.IsValid()
      && theBuild.AppendedRootSet.Add(theNodeId))
  {
    theBuild.AppendedRoots->Append(theNodeId);
  }
}

BRepGraph_NodeId enqueueFace(BuildContext&          theBuild,
                             const TopoDS_Face&    theFace,
                             const TopLoc_Location& theParentLocation)
{
  const TopLoc_Location aBakedLocation = theParentLocation * theFace.Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theFace, BRepGraph_NodeId::Kind::Face, aBakedLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  const BRepGraph_FaceId aFaceId = theBuild.Storage.AppendFace();
  bindLocatedNode(theBuild, theFace, aFaceId, aBakedLocation, true);
  theBuild.Storage.BindOriginal(aFaceId, theFace);
  attachSupplement(theBuild.SupplementLayer, aFaceId, theFace);

  FaceBuildData& aFaceData = theBuild.PendingFaces.Appended();
  aFaceData.Face           = theFace;
  aFaceData.FaceId         = aFaceId;
  aFaceData.BakedLocation  = aBakedLocation;
  aFaceData.Wires =
    NCollection_DynamicArray<ExtractedWire>(THE_SMALL_ARRAY_BUCKET, theBuild.ScratchAlloc);
  return BRepGraph_NodeId(aFaceId);
}

BRepGraph_NodeId traverseTopology(BuildContext&           theBuild,
                                  const TopoDS_Shape&     theCurrentShape,
                                  const TopLoc_Location&  theParentLocation,
                                  const RootRole          theRootRole = RootRole::Nested);

BRepGraph_NodeId traverseCompound(BuildContext&           theBuild,
                                  const TopoDS_Compound&  theCompound,
                                  const RootRole          theRootRole)
{
  const TopLoc_Location aDefinitionLocation =
    theRootRole == RootRole::Root ? theCompound.Location() : TopLoc_Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theCompound, BRepGraph_NodeId::Kind::Compound, aDefinitionLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  const BRepGraph_CompoundId aCompId = theBuild.Storage.AppendCompound();
  bindLocatedNode(theBuild, theCompound, aCompId, aDefinitionLocation, true);
  theBuild.Storage.BindOriginal(aCompId, theCompound);

  const TopLoc_Location aChildRootLocation = aDefinitionLocation;

  for (TopoDS_Iterator aChildIt(theCompound, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    const TopAbs_Orientation aChildOri = aChild.Orientation();
    if (!isForwardChildType(TopAbs_COMPOUND, aChild.ShapeType())
        || !isCoreParityOrientation(aChildOri))
    {
      attachNonCoreContainerChild(theBuild, BRepGraph_NodeId(aCompId), TopAbs_COMPOUND, aChild);
      continue;
    }
    const BRepGraph_NodeId aChildNode =
      traverseTopology(theBuild, aChild, TopLoc_Location(), RootRole::Nested);
    if (aChildNode.IsValid())
    {
      appendChildRef(theBuild.Storage,
                     aCompId,
                     aChildNode,
                     aChildOri,
                     aChildRootLocation * aChild.Location());
    }
  }
  return BRepGraph_NodeId(aCompId);
}

BRepGraph_NodeId traverseCompSolid(BuildContext&              theBuild,
                                   const TopoDS_CompSolid&    theCompSolid,
                                   const TopLoc_Location&     theParentLocation)
{
  const TopLoc_Location aBakedLocation = theParentLocation * theCompSolid.Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theCompSolid, BRepGraph_NodeId::Kind::CompSolid, aBakedLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  const BRepGraph_CompSolidId aCSolidId = theBuild.Storage.AppendCompSolid();
  bindLocatedNode(theBuild, theCompSolid, aCSolidId, aBakedLocation, true);
  theBuild.Storage.BindOriginal(aCSolidId, theCompSolid);

  for (TopoDS_Iterator aChildIt(theCompSolid, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    const TopAbs_Orientation aChildOri = aChild.Orientation();
    if (!isForwardChildType(TopAbs_COMPSOLID, aChild.ShapeType())
        || !isCoreParityOrientation(aChildOri))
    {
      attachNonCoreContainerChild(theBuild, BRepGraph_NodeId(aCSolidId), TopAbs_COMPSOLID, aChild);
      continue;
    }

    const BRepGraph_NodeId aChildNode = traverseTopology(theBuild, aChild, aBakedLocation);
    if (!aChildNode.IsValid() || aChildNode.NodeKind != BRepGraph_NodeId::Kind::Solid)
    {
      continue;
    }

    appendSolidRef(theBuild.Storage,
                   aCSolidId,
                   BRepGraph_SolidId::FromNodeId(aChildNode),
                   aChildOri);
  }
  return BRepGraph_NodeId(aCSolidId);
}

BRepGraph_NodeId traverseSolid(BuildContext&          theBuild,
                               const TopoDS_Solid&   theSolid,
                               const TopLoc_Location& theParentLocation)
{
  const TopLoc_Location aBakedLocation = theParentLocation * theSolid.Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theSolid, BRepGraph_NodeId::Kind::Solid, aBakedLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  const BRepGraph_SolidId aSolidId = theBuild.Storage.AppendSolid();
  bindLocatedNode(theBuild, theSolid, aSolidId, aBakedLocation, true);
  theBuild.Storage.BindOriginal(aSolidId, theSolid);

  for (TopoDS_Iterator aChildIt(theSolid, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    const TopAbs_Orientation aChildOri = aChild.Orientation();
    if (!isForwardChildType(TopAbs_SOLID, aChild.ShapeType())
        || !isCoreParityOrientation(aChildOri))
    {
      attachNonCoreContainerChild(theBuild, BRepGraph_NodeId(aSolidId), TopAbs_SOLID, aChild);
      continue;
    }

    const BRepGraph_NodeId aChildNode = traverseTopology(theBuild, aChild, aBakedLocation);
    if (!aChildNode.IsValid() || aChildNode.NodeKind != BRepGraph_NodeId::Kind::Shell)
    {
      continue;
    }

    appendShellRef(theBuild.Storage,
                   aSolidId,
                   BRepGraph_ShellId::FromNodeId(aChildNode),
                   aChildOri);
  }
  return BRepGraph_NodeId(aSolidId);
}

BRepGraph_NodeId traverseShell(BuildContext&          theBuild,
                               const TopoDS_Shell&   theShell,
                               const TopLoc_Location& theParentLocation)
{
  const TopLoc_Location aBakedLocation = theParentLocation * theShell.Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theShell, BRepGraph_NodeId::Kind::Shell, aBakedLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  const BRepGraph_ShellId aShellId  = theBuild.Storage.AppendShell();
  bindLocatedNode(theBuild, theShell, aShellId, aBakedLocation, true);
  theBuild.Storage.BindOriginal(aShellId, theShell);

  for (TopoDS_Iterator aChildIt(theShell, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    const TopAbs_Orientation aChildOri = aChild.Orientation();
    if (!isForwardChildType(TopAbs_SHELL, aChild.ShapeType())
        || !isCoreParityOrientation(aChildOri))
    {
      attachNonCoreContainerChild(theBuild, BRepGraph_NodeId(aShellId), TopAbs_SHELL, aChild);
      continue;
    }

    const TopoDS_Face      aFace     = TopoDS::Face(aChild);
    const BRepGraph_NodeId aFaceNode = enqueueFace(theBuild, aFace, aBakedLocation);
    appendFaceRef(theBuild.Storage,
                  aShellId,
                  BRepGraph_FaceId::FromNodeId(aFaceNode),
                  aChildOri);
  }
  return BRepGraph_NodeId(aShellId);
}

BRepGraph_NodeId traverseWire(BuildContext&          theBuild,
                              const TopoDS_Wire&    theWire,
                              const TopLoc_Location& theParentLocation)
{
  const TopLoc_Location aBakedLocation = theParentLocation * theWire.Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theWire, BRepGraph_NodeId::Kind::Wire, aBakedLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  const BRepGraph_WireId aWireId = appendWireDef(theBuild, theWire, true, aBakedLocation);
  for (TopoDS_Iterator anEdgeIt(theWire, false, false); anEdgeIt.More(); anEdgeIt.Next())
  {
    const TopoDS_Shape& anEdgeShape = anEdgeIt.Value();
    if (!isForwardChildType(TopAbs_WIRE, anEdgeShape.ShapeType()))
    {
      continue;
    }

    const TopoDS_Edge      anEdge     = TopoDS::Edge(anEdgeShape);
    const BRepGraph_NodeId aChildNode = traverseTopology(theBuild, anEdgeShape, aBakedLocation);
    if (!aChildNode.IsValid() || aChildNode.NodeKind != BRepGraph_NodeId::Kind::Edge)
    {
      continue;
    }

    appendWireCoEdge(theBuild.Storage, aWireId, BRepGraph_EdgeId::FromNodeId(aChildNode), anEdge);
  }
  theBuild.Storage.CanonicalizeWireCoEdgeOrder(aWireId);
  return BRepGraph_NodeId(aWireId);
}

BRepGraph_NodeId traverseEdge(BuildContext&          theBuild,
                              const TopoDS_Edge&    theEdge,
                              const TopLoc_Location& theParentLocation)
{
  const TopLoc_Location aBakedLocation = theParentLocation * theEdge.Location();
  const BRepGraph_NodeId anExisting =
    findExistingNode(theBuild, theEdge, BRepGraph_NodeId::Kind::Edge, aBakedLocation);
  if (anExisting.IsValid())
  {
    return anExisting;
  }

  ExtractedEdge anEdgeData;
  extractEdgeDefinition(anEdgeData, theEdge, theParentLocation);
  return BRepGraph_NodeId(
    registerEdge(theBuild, anEdgeData, theBuild.SupplementLayer));
}

BRepGraph_NodeId registerTopology(BuildContext&          theBuild,
                                  const TopoDS_Shape&    theCurrentShape,
                                  const TopLoc_Location& theParentLocation,
                                  const RootRole         theRootRole)
{
  switch (theCurrentShape.ShapeType())
  {
    case TopAbs_COMPOUND:
      return traverseCompound(theBuild, TopoDS::Compound(theCurrentShape), theRootRole);
    case TopAbs_COMPSOLID:
      return traverseCompSolid(theBuild, TopoDS::CompSolid(theCurrentShape), theParentLocation);
    case TopAbs_SOLID:
      return traverseSolid(theBuild, TopoDS::Solid(theCurrentShape), theParentLocation);
    case TopAbs_SHELL:
      return traverseShell(theBuild, TopoDS::Shell(theCurrentShape), theParentLocation);
    case TopAbs_FACE:
      return enqueueFace(theBuild, TopoDS::Face(theCurrentShape), theParentLocation);
    case TopAbs_WIRE:
      return traverseWire(theBuild, TopoDS::Wire(theCurrentShape), theParentLocation);
    case TopAbs_EDGE:
      return traverseEdge(theBuild, TopoDS::Edge(theCurrentShape), theParentLocation);
    case TopAbs_VERTEX:
      return BRepGraph_NodeId(
        registerOrReuseVertex(theBuild,
                              TopoDS::Vertex(theCurrentShape),
                              theParentLocation * theCurrentShape.Location()));
    default:
      return BRepGraph_NodeId();
  }
}

void traverseFlattenedChildren(BuildContext&          theBuild,
                               const TopoDS_Shape&    theContainer,
                               const TopLoc_Location& theParentLocation)
{
  const TopAbs_ShapeEnum aContainerType = theContainer.ShapeType();
  const TopLoc_Location aContainerLocation = theParentLocation * theContainer.Location();
  for (TopoDS_Iterator aChildIt(theContainer, false, false); aChildIt.More(); aChildIt.Next())
  {
    const TopoDS_Shape& aChild = aChildIt.Value();
    if (isForwardChildType(aContainerType, aChild.ShapeType()))
    {
      traverseTopology(theBuild, aChild, aContainerLocation);
    }
  }
}

BRepGraph_NodeId traverseTopology(BuildContext&          theBuild,
                                  const TopoDS_Shape&    theCurrentShape,
                                  const TopLoc_Location& theParentLocation,
                                  const RootRole         theRootRole)
{
  if (theCurrentShape.IsNull())
  {
    return BRepGraph_NodeId();
  }

  const TopAbs_ShapeEnum aShapeType = theCurrentShape.ShapeType();
  if (theBuild.Mode == TopologyBuildMode::Flattened
      && (aShapeType == TopAbs_COMPOUND || aShapeType == TopAbs_COMPSOLID
          || aShapeType == TopAbs_SOLID || aShapeType == TopAbs_SHELL))
  {
    traverseFlattenedChildren(theBuild, theCurrentShape, theParentLocation);
    return BRepGraph_NodeId();
  }

  const BRepGraph_NodeId aNode =
    registerTopology(theBuild, theCurrentShape, theParentLocation, theRootRole);
  if (theBuild.Mode == TopologyBuildMode::Flattened)
  {
    appendBuildRoot(theBuild, aNode);
  }
  return aNode;
}

BRepGraphInc_Populate::BuildStatus runTopologyBuild(BuildContext&        theBuild,
                                                    const TopoDS_Shape& theRootShape)
{
  traverseTopology(theBuild, theRootShape, TopLoc_Location(), RootRole::Root);

  const uint32_t aNbPendingFaces = static_cast<uint32_t>(theBuild.PendingFaces.Size());
  if (aNbPendingFaces == 0)
  {
    return BRepGraphInc_Populate::BuildStatus::Success;
  }

  BRepGraph_ParallelPolicy::Workload aFaceExtraction;
  aFaceExtraction.PrimaryItems = aNbPendingFaces;
  const bool isParallelExtraction =
    BRepGraph_ParallelPolicy::ShouldRun(theBuild.Parallel, aFaceExtraction);
  OSD_Parallel::For(
    0,
    static_cast<int>(aNbPendingFaces),
    [&](const int theIndex) {
      extractFaceData(theBuild.PendingFaces.ChangeValue(theIndex), theBuild.ScratchAlloc);
    },
    !isParallelExtraction);

  BRepGraphInc_Populate::BuildStatus aStatus = BRepGraphInc_Populate::BuildStatus::Success;
  for (const FaceBuildData& aFaceData : theBuild.PendingFaces)
  {
    if (aFaceData.UnsupportedNaturalBoundary)
    {
      aStatus = BRepGraphInc_Populate::BuildStatus::SuccessWithWarnings;
    }
  }

  registerFaceData(theBuild,
                   theBuild.PendingFaces,
                   theBuild.SupplementLayer);

  synthesizeFaceBoundaries(theBuild.Storage,
                            theBuild.PendingFaces);

  return aStatus;
}

BRepGraphInc_Populate::BuildStatus buildTopology(BRepGraph&                               theGraph,
                                                     BRepGraphInc_Storage&                    theStorage,
                                                     const TopoDS_Shape&               theShape,
                                                     const bool                        theParallel,
                                                     const TopologyBuildMode           theMode,
                                                     const BRepGraphInc_Populate::Options& theOptions,
                                                     const BuildCounts&                theOldCounts,
                                                     NCollection_LinearVector<BRepGraph_NodeId>*
                                                       theAppendedRoots = nullptr)
{
  occ::handle<NCollection_IncAllocator> aScratchAlloc = new NCollection_IncAllocator(1024 * 1024);
  if (theParallel)
  {
    aScratchAlloc->SetThreadSafe(true);
  }

  const occ::handle<BRepGraph_LayerTopoSupplement> aSupplementLayer =
    theGraph.LayerRegistry().Find<BRepGraph_LayerTopoSupplement>();

  BuildContext aBuild(theStorage,
                      theParallel,
                      theMode,
                      aSupplementLayer,
                      aScratchAlloc,
                      theAppendedRoots);

  BRepGraphInc_Populate::BuildStatus aStatus = runTopologyBuild(aBuild, theShape);
  (void)theOptions;
  (void)theOldCounts;
  return aStatus;
}

} // anonymous namespace

//=================================================================================================

BRepGraphInc_Populate::BuildStatus BRepGraphInc_Populate::Perform(BRepGraph&          theGraph,
                                                                    const TopoDS_Shape& theShape,
                                                                    bool theParallel,
                                                                    const Options& theOptions)
{
  theGraph.incStorage().Clear();

  if (theShape.IsNull())
  {
    return BuildStatus::Failed;
  }

  return buildTopology(theGraph, theGraph.incStorage(), theShape, theParallel,
                       TopologyBuildMode::FullHierarchy, theOptions, BuildCounts{});
}

//=================================================================================================

BRepGraphInc_Populate::BuildStatus BRepGraphInc_Populate::AppendFlattened(
  BRepGraph&                                  theGraph,
  const TopoDS_Shape&                         theShape,
  bool                                        theParallel,
  NCollection_LinearVector<BRepGraph_NodeId>& theAppendedRoots,
  const Options&                              theOptions)
{
  if (theShape.IsNull())
  {
    return BuildStatus::Failed;
  }

  return buildTopology(theGraph,
                       theGraph.incStorage(),
                       theShape,
                       theParallel,
                       TopologyBuildMode::Flattened,
                       theOptions,
                       captureBuildCounts(theGraph.incStorage()),
                       &theAppendedRoots);
}

//=================================================================================================

BRepGraphInc_Populate::BuildStatus BRepGraphInc_Populate::Append(BRepGraph&          theGraph,
                                                                  const TopoDS_Shape& theShape,
                                                                  bool theParallel,
                                                                  const Options& theOptions)
{
  if (theShape.IsNull())
  {
    return BuildStatus::Failed;
  }

  return buildTopology(theGraph,
                       theGraph.incStorage(),
                       theShape,
                       theParallel,
                       TopologyBuildMode::FullHierarchy,
                       theOptions,
                       captureBuildCounts(theGraph.incStorage()));
}
