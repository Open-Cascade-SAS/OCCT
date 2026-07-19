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

#ifndef _BRepGraphInc_Storage_HeaderFile
#define _BRepGraphInc_Storage_HeaderFile

#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraphInc_BitFlags.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Load.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Relations.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>

#include <atomic>
#include <shared_mutex>
#include <utility>

//! @brief Central backend storage container for the incidence-table topology model.
//!
//! Holds all entity vectors (Vertex through Occurrence), representation
//! vectors (Surface, Curve3D, Curve2D, Triangulation, Polygon), relation
//! tables for connectivity navigation, TShape deduplication maps, original
//! shape bindings, and per-kind UID vectors. Provides typed accessors
//! enforcing compile-time safety for backend code. External callers should
//! normally use the BRepGraph facade rather than reaching into this storage
//! directly. BRepGraphInc_Populate has friend access for efficient bulk writes
//! during graph population.
class BRepGraphInc_Storage
{
public:
  DEFINE_STANDARD_ALLOC

  //! Gen-validated shape cache entry.
  struct CachedShape
  {
    //! Reconstructed shape cached for a node id.
    TopoDS_Shape Shape;

    //! Subtree generation captured when the cached shape was built.
    uint32_t StoredSubtreeGen = 0;
  };

  //! Construct an empty storage with no entities or representations.
  Standard_EXPORT BRepGraphInc_Storage();

  //! Clear allocator-backed containers before member destructors walk them.
  Standard_EXPORT ~BRepGraphInc_Storage();

  //! Return the allocator used for backend storage.
  [[nodiscard]] const occ::handle<NCollection_BaseAllocator>& Allocator() const
  {
    return myAllocator;
  }

  //! Return products not referenced by any active occurrence.
  [[nodiscard]] const NCollection_LinearVector<BRepGraph_ProductId>& RootProductIds() const
  {
    return myRootProductIds;
  }

  //! Return products not referenced by any active occurrence.
  NCollection_LinearVector<BRepGraph_ProductId>& ChangeRootProductIds() { return myRootProductIds; }

  //! Return nodes accumulated during deferred invalidation.
  [[nodiscard]] const NCollection_LinearVector<BRepGraph_NodeId>& DeferredModified() const
  {
    return myDeferredModified;
  }

  //! Return nodes accumulated during deferred invalidation.
  NCollection_LinearVector<BRepGraph_NodeId>& ChangeDeferredModified()
  {
    return myDeferredModified;
  }

  //! Return refs accumulated during deferred invalidation.
  [[nodiscard]] const NCollection_LinearVector<BRepGraph_RefId>& DeferredRefModified() const
  {
    return myDeferredRefModified;
  }

  //! Return refs accumulated during deferred invalidation.
  NCollection_LinearVector<BRepGraph_RefId>& ChangeDeferredRefModified()
  {
    return myDeferredRefModified;
  }

  //! Return true when the graph contains no topology definitions.
  //! Checks whether any node kind (Vertex, Edge, Wire, Face, Shell, Solid,
  //! Compound, CompSolid, Product, Occurrence) has been allocated.
  [[nodiscard]] Standard_EXPORT bool IsEmpty() const;

  //! Return the next UID counter for a given node kind.
  [[nodiscard]] Standard_EXPORT uint32_t NextNodeUIDCounter(BRepGraph_NodeId::Kind theKind) const;

  //! Override the next UID counter for a given node kind.
  Standard_EXPORT void SetNextNodeUIDCounter(BRepGraph_NodeId::Kind theKind, uint32_t theCounter);

  //! Return the next UID counter for a given reference kind.
  [[nodiscard]] Standard_EXPORT uint32_t NextRefUIDCounter(BRepGraph_RefId::Kind theKind) const;

  //! Override the next UID counter for a given reference kind.
  Standard_EXPORT void SetNextRefUIDCounter(BRepGraph_RefId::Kind theKind, uint32_t theCounter);

  //! Allocate a node UID: write counter into the entity, bind reverse map, advance counter.
  Standard_EXPORT BRepGraph_UID AllocateNodeUID(BRepGraph_NodeId theNodeId);

  //! Allocate a reference UID: write counter into the ref, bind reverse map, advance counter.
  Standard_EXPORT BRepGraph_RefUID AllocateRefUID(BRepGraph_RefId theRefId);

  //! Return the current graph generation used by VersionStamp staleness checks.
  [[nodiscard]] uint32_t Generation() const { return myGeneration.load(std::memory_order_relaxed); }

  //! Override the current graph generation.
  void SetGeneration(const uint32_t theGeneration)
  {
    myGeneration.store(theGeneration, std::memory_order_relaxed);
  }

  //! Increment the graph generation after a structural mutation batch.
  void IncrementGeneration() { myGeneration.fetch_add(1, std::memory_order_relaxed); }

  //! Return the stable graph instance GUID.
  [[nodiscard]] const Standard_GUID& GraphGUID() const { return myGraphGUID; }

  //! Override the stable graph instance GUID.
  void SetGraphGUID(const Standard_GUID& theGuid) { myGraphGUID = theGuid; }

  //! Return whether invalidation is currently deferred.
  [[nodiscard]] bool DeferredMode() const { return myDeferredMode.load(std::memory_order_relaxed); }

  //! Enable or disable deferred invalidation mode.
  void SetDeferredMode(const bool theEnabled)
  {
    myDeferredMode.store(theEnabled, std::memory_order_relaxed);
  }

  //! Return the current propagation wave id used to avoid revisiting parents.
  [[nodiscard]] uint32_t PropagationWave() const
  {
    return myPropagationWave.load(std::memory_order_relaxed);
  }

  //! Increment the propagation wave and return the new value.
  [[nodiscard]] uint32_t AdvancePropagationWave()
  {
    return myPropagationWave.fetch_add(1, std::memory_order_relaxed) + 1;
  }

  //! Increment the propagation wave without reading it back.
  void IncrementPropagationWave() { myPropagationWave.fetch_add(1, std::memory_order_relaxed); }

  //! Return the recursion depth of the active RemoveSubgraph cascade.
  [[nodiscard]] uint32_t RemoveSubgraphDepth() const { return myRemoveSubgraphDepth; }

  //! Enter one nested RemoveSubgraph scope.
  void IncrementRemoveSubgraphDepth() { ++myRemoveSubgraphDepth; }

  //! Leave one nested RemoveSubgraph scope.
  void DecrementRemoveSubgraphDepth()
  {
    Standard_ASSERT_VOID(myRemoveSubgraphDepth > 0, "RemoveSubgraphDepth underflow");
    if (myRemoveSubgraphDepth > 0)
    {
      --myRemoveSubgraphDepth;
    }
  }

  //! Returns the total number of vertex entities (including removed).
  [[nodiscard]] uint32_t NbVertices() const { return myVertices.Nb(); }

  //! Returns the total number of edge entities (including removed).
  [[nodiscard]] uint32_t NbEdges() const { return myEdges.Nb(); }

  //! Returns the total number of coedge entities (including removed).
  [[nodiscard]] uint32_t NbCoEdges() const { return myCoEdges.Nb(); }

  //! Returns the total number of wire entities (including removed).
  [[nodiscard]] uint32_t NbWires() const { return myWires.Nb(); }

  //! Returns the total number of face entities (including removed).
  [[nodiscard]] uint32_t NbFaces() const { return myFaces.Nb(); }

  //! Returns the total number of shell entities (including removed).
  [[nodiscard]] uint32_t NbShells() const { return myShells.Nb(); }

  //! Returns the total number of solid entities (including removed).
  [[nodiscard]] uint32_t NbSolids() const { return mySolids.Nb(); }

  //! Returns the total number of compound entities (including removed).
  [[nodiscard]] uint32_t NbCompounds() const { return myCompounds.Nb(); }

  //! Returns the total number of compsolid entities (including removed).
  [[nodiscard]] uint32_t NbCompSolids() const { return myCompSolids.Nb(); }

  //! Returns the total number of product entities (including removed).
  [[nodiscard]] uint32_t NbProducts() const { return myProducts.Nb(); }

  //! Returns the total number of occurrence entities (including removed).
  [[nodiscard]] uint32_t NbOccurrences() const { return myOccurrences.Nb(); }

  //! Returns the total number of shell reference entries (including removed).
  [[nodiscard]] uint32_t NbShellRefs() const { return myShellRefs.Nb(); }

  //! Returns the total number of face reference entries (including removed).
  [[nodiscard]] uint32_t NbFaceRefs() const { return myFaceRefs.Nb(); }

  //! Returns the total number of wire reference entries (including removed).
  [[nodiscard]] uint32_t NbWireRefs() const { return myWireRefs.Nb(); }

  //! Returns the total number of vertex reference entries (including removed).
  [[nodiscard]] uint32_t NbVertexRefs() const { return myVertexRefs.Nb(); }

  //! Returns the total number of solid reference entries (including removed).
  [[nodiscard]] uint32_t NbSolidRefs() const { return mySolidRefs.Nb(); }

  //! Returns the total number of child reference entries (including removed).
  [[nodiscard]] uint32_t NbChildRefs() const { return myChildRefs.Nb(); }

  //! Returns the total number of occurrence reference entries (including removed).
  [[nodiscard]] uint32_t NbOccurrenceRefs() const { return myOccurrenceRefs.Nb(); }

  //! Returns the number of active vertex entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveVertices() const { return myVertices.NbActive; }

  //! Returns the number of active edge entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveEdges() const { return myEdges.NbActive; }

  //! Returns the number of active coedge entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveCoEdges() const { return myCoEdges.NbActive; }

  //! Returns the number of active wire entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveWires() const { return myWires.NbActive; }

  //! Returns the number of active face entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveFaces() const { return myFaces.NbActive; }

  //! Returns the number of active shell entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveShells() const { return myShells.NbActive; }

  //! Returns the number of active solid entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveSolids() const { return mySolids.NbActive; }

  //! Returns the number of active compound entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveCompounds() const { return myCompounds.NbActive; }

  //! Returns the number of active compsolid entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveCompSolids() const { return myCompSolids.NbActive; }

  //! Returns the number of active product entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveProducts() const { return myProducts.NbActive; }

  //! Returns the number of active occurrence entities (excluding removed).
  [[nodiscard]] uint32_t NbActiveOccurrences() const { return myOccurrences.NbActive; }

  //! Returns the number of active shell reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveShellRefs() const { return myShellRefs.NbActive; }

  //! Returns the number of active face reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveFaceRefs() const { return myFaceRefs.NbActive; }

  //! Returns the number of active wire reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveWireRefs() const { return myWireRefs.NbActive; }

  //! Returns the number of active vertex reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveVertexRefs() const { return myVertexRefs.NbActive; }

  //! Returns the number of active solid reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveSolidRefs() const { return mySolidRefs.NbActive; }

  //! Returns the number of active child reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveChildRefs() const { return myChildRefs.NbActive; }

  //! Returns the number of active occurrence reference entries (excluding removed).
  [[nodiscard]] uint32_t NbActiveOccurrenceRefs() const { return myOccurrenceRefs.NbActive; }

  //! Mark an entity node as removed and decrement its active counter once.
  //! @param[in] theNodeId typed entity id
  //! @return true if the node transitioned from active to removed
  Standard_EXPORT bool MarkRemoved(const BRepGraph_NodeId theNodeId);

  //! Mark a reference entry as removed and decrement its active counter once.
  //! @param[in] theRefId typed reference id
  //! @return true if the ref transitioned from active to removed
  Standard_EXPORT bool MarkRemovedRef(const BRepGraph_RefId theRefId);

  //! Returns the number of edge 3D curve use records.
  [[nodiscard]] uint32_t NbEdgeCurves3D() const { return myEdgeCurves3D.Nb(); }

  //! Returns the number of edge 3D polygon use records.
  [[nodiscard]] uint32_t NbEdgePolygons3D() const { return myEdgePolygons3D.Nb(); }

  //! Returns the number of coedge 2D curve use records.
  [[nodiscard]] uint32_t NbCoEdgeCurves2D() const { return myCoEdgeCurves2D.Nb(); }

  //! Returns the number of coedge 2D polygon use records.
  [[nodiscard]] uint32_t NbCoEdgePolygons2D() const { return myCoEdgePolygons2D.Nb(); }

  //! Returns the number of coedge polygon-on-triangulation use records.
  [[nodiscard]] uint32_t NbCoEdgePolygonsOnTri() const { return myCoEdgePolygonsOnTri.Nb(); }

  //! Returns the number of face surface use records.
  [[nodiscard]] uint32_t NbFaceSurfaces() const { return myFaceSurfaces.Nb(); }

  //! Returns the number of face triangulation use records.
  [[nodiscard]] uint32_t NbFaceTriangulations() const { return myFaceTriangulations.Nb(); }

  //! Returns the number of active (parent-valid) edge 3D curve use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveEdgeCurves3D() const;

  //! Returns the number of active (parent-valid) coedge 2D curve use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveCoEdgeCurves2D() const;

  //! Returns the number of active (parent-valid) face surface use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveFaceSurfaces() const;

  //! Returns the number of active (parent-valid) face triangulation use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveFaceTriangulations() const;

  //! Returns the number of active (parent-valid) edge 3D polygon use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveEdgePolygons3D() const;

  //! Returns the number of active (parent-valid) coedge 2D polygon use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveCoEdgePolygons2D() const;

  //! Returns the number of active (parent-valid) coedge polygon-on-triangulation use records.
  [[nodiscard]] Standard_EXPORT uint32_t NbActiveCoEdgePolygonsOnTri() const;

  //! Returns the edge 3D curve use at the given id.
  [[nodiscard]] const BRepGraphInc::EdgeCurve3DRep& EdgeCurve3DRep(
    const BRepGraph_EdgeCurve3DRepId theId) const
  {
    return myEdgeCurves3D.Get(theId);
  }

  //! Returns a mutable reference to the edge 3D curve use at the given id.
  BRepGraphInc::EdgeCurve3DRep& ChangeEdgeCurve3DRep(const BRepGraph_EdgeCurve3DRepId theId)
  {
    return myEdgeCurves3D.Change(theId);
  }

  //! Returns the edge 3D polygon use at the given id.
  [[nodiscard]] const BRepGraphInc::EdgePolygon3DRep& EdgePolygon3DRep(
    const BRepGraph_EdgePolygon3DRepId theId) const
  {
    return myEdgePolygons3D.Get(theId);
  }

  //! Returns a mutable reference to the edge 3D polygon use at the given id.
  BRepGraphInc::EdgePolygon3DRep& ChangeEdgePolygon3DRep(const BRepGraph_EdgePolygon3DRepId theId)
  {
    return myEdgePolygons3D.Change(theId);
  }

  //! Returns the coedge 2D curve use at the given id.
  [[nodiscard]] const BRepGraphInc::CoEdgeCurve2DRep& CoEdgeCurve2DRep(
    const BRepGraph_CoEdgeCurve2DRepId theId) const
  {
    return myCoEdgeCurves2D.Get(theId);
  }

  //! Returns a mutable reference to the coedge 2D curve use at the given id.
  BRepGraphInc::CoEdgeCurve2DRep& ChangeCoEdgeCurve2DRep(const BRepGraph_CoEdgeCurve2DRepId theId)
  {
    return myCoEdgeCurves2D.Change(theId);
  }

  //! Returns the coedge 2D polygon use at the given id.
  [[nodiscard]] const BRepGraphInc::CoEdgePolygon2DRep& CoEdgePolygon2DRep(
    const BRepGraph_CoEdgePolygon2DRepId theId) const
  {
    return myCoEdgePolygons2D.Get(theId);
  }

  //! Returns a mutable reference to the coedge 2D polygon use at the given id.
  BRepGraphInc::CoEdgePolygon2DRep& ChangeCoEdgePolygon2DRep(
    const BRepGraph_CoEdgePolygon2DRepId theId)
  {
    return myCoEdgePolygons2D.Change(theId);
  }

  //! Returns the coedge polygon-on-triangulation use at the given id.
  [[nodiscard]] const BRepGraphInc::CoEdgePolygonOnTriRep& CoEdgePolygonOnTriRep(
    const BRepGraph_CoEdgePolygonOnTriRepId theId) const
  {
    return myCoEdgePolygonsOnTri.Get(theId);
  }

  //! Returns a mutable reference to the coedge polygon-on-triangulation use at the given id.
  BRepGraphInc::CoEdgePolygonOnTriRep& ChangeCoEdgePolygonOnTriRep(
    const BRepGraph_CoEdgePolygonOnTriRepId theId)
  {
    return myCoEdgePolygonsOnTri.Change(theId);
  }

  //! Returns the face surface use at the given id.
  [[nodiscard]] const BRepGraphInc::FaceSurfaceRep& FaceSurfaceRep(
    const BRepGraph_FaceSurfaceRepId theId) const
  {
    return myFaceSurfaces.Get(theId);
  }

  //! Returns a mutable reference to the face surface use at the given id.
  BRepGraphInc::FaceSurfaceRep& ChangeFaceSurfaceRep(const BRepGraph_FaceSurfaceRepId theId)
  {
    return myFaceSurfaces.Change(theId);
  }

  //! Returns the face triangulation use at the given id.
  [[nodiscard]] const BRepGraphInc::FaceTriangulationRep& FaceTriangulationRep(
    const BRepGraph_FaceTriangulationRepId theId) const
  {
    return myFaceTriangulations.Get(theId);
  }

  //! Returns a mutable reference to the face triangulation use at the given id.
  BRepGraphInc::FaceTriangulationRep& ChangeFaceTriangulationRep(
    const BRepGraph_FaceTriangulationRepId theId)
  {
    return myFaceTriangulations.Change(theId);
  }

  //! Appends a new edge 3D curve use record and returns its id.
  BRepGraph_EdgeCurve3DRepId AppendEdgeCurve3DRep() { return myEdgeCurves3D.Append(); }

  //! Appends a new edge 3D polygon use record and returns its id.
  BRepGraph_EdgePolygon3DRepId AppendEdgePolygon3DRep() { return myEdgePolygons3D.Append(); }

  //! Appends a new coedge 2D curve use record and returns its id.
  BRepGraph_CoEdgeCurve2DRepId AppendCoEdgeCurve2DRep() { return myCoEdgeCurves2D.Append(); }

  //! Appends a new coedge 2D polygon use record and returns its id.
  BRepGraph_CoEdgePolygon2DRepId AppendCoEdgePolygon2DRep() { return myCoEdgePolygons2D.Append(); }

  //! Appends a new coedge polygon-on-triangulation use record and returns its id.
  BRepGraph_CoEdgePolygonOnTriRepId AppendCoEdgePolygonOnTriRep()
  {
    return myCoEdgePolygonsOnTri.Append();
  }

  //! Appends a new face surface use record and returns its id.
  BRepGraph_FaceSurfaceRepId AppendFaceSurfaceRep() { return myFaceSurfaces.Append(); }

  //! Appends a new face triangulation use record and returns its id.
  BRepGraph_FaceTriangulationRepId AppendFaceTriangulationRep()
  {
    return myFaceTriangulations.Append();
  }

  //! Mark a representation-use record as removed and decrement its active counter once.
  //! @param[in] theRepId typed use id
  //! @return true if the use transitioned from active to removed
  Standard_EXPORT bool MarkRemoved(const BRepGraph_RepId theRepId);

  //! Set or clear the soft-removal flag for a representation-use record.
  //! @param[in] theRepId typed use id
  //! @param[in] theVal true to mark removed, false to mark active
  Standard_EXPORT void SetRemoved(const BRepGraph_RepId theRepId, const bool theVal);

  //! Returns the vertex entity at the given typed id.
  //! @param[in] theVertex typed vertex id
  [[nodiscard]] const BRepGraphInc::VertexDef& Vertex(const BRepGraph_VertexId theVertex) const
  {
    return myVertices.Get(theVertex);
  }

  //! Returns the edge entity at the given typed id.
  //! @param[in] theEdge typed edge id
  [[nodiscard]] const BRepGraphInc::EdgeDef& Edge(const BRepGraph_EdgeId theEdge) const
  {
    return myEdges.Get(theEdge);
  }

  //! Returns the coedge entity at the given typed id.
  //! @param[in] theCoEdge typed coedge id
  [[nodiscard]] const BRepGraphInc::CoEdgeDef& CoEdge(const BRepGraph_CoEdgeId theCoEdge) const
  {
    return myCoEdges.Get(theCoEdge);
  }

  //! Returns the wire entity at the given typed id.
  //! @param[in] theWire typed wire id
  [[nodiscard]] const BRepGraphInc::WireDef& Wire(const BRepGraph_WireId theWire) const
  {
    return myWires.Get(theWire);
  }

  //! Returns the face entity at the given typed id.
  //! @param[in] theFace typed face id
  [[nodiscard]] const BRepGraphInc::FaceDef& Face(const BRepGraph_FaceId theFace) const
  {
    return myFaces.Get(theFace);
  }

  //! Returns the shell entity at the given typed id.
  //! @param[in] theShell typed shell id
  [[nodiscard]] const BRepGraphInc::ShellDef& Shell(const BRepGraph_ShellId theShell) const
  {
    return myShells.Get(theShell);
  }

  //! Returns the solid entity at the given typed id.
  //! @param[in] theSolid typed solid id
  [[nodiscard]] const BRepGraphInc::SolidDef& Solid(const BRepGraph_SolidId theSolid) const
  {
    return mySolids.Get(theSolid);
  }

  //! Returns the compound entity at the given typed id.
  //! @param[in] theCompound typed compound id
  [[nodiscard]] const BRepGraphInc::CompoundDef& Compound(
    const BRepGraph_CompoundId theCompound) const
  {
    return myCompounds.Get(theCompound);
  }

  //! Returns the compsolid entity at the given typed id.
  //! @param[in] theCompSolid typed comp-solid id
  [[nodiscard]] const BRepGraphInc::CompSolidDef& CompSolid(
    const BRepGraph_CompSolidId theCompSolid) const
  {
    return myCompSolids.Get(theCompSolid);
  }

  //! Returns the product entity at the given typed id.
  //! @param[in] theProduct typed product id
  [[nodiscard]] const BRepGraphInc::ProductDef& Product(const BRepGraph_ProductId theProduct) const
  {
    return myProducts.Get(theProduct);
  }

  //! Returns the occurrence entity at the given typed id.
  //! @param[in] theOccurrence typed occurrence id
  [[nodiscard]] const BRepGraphInc::OccurrenceDef& Occurrence(
    const BRepGraph_OccurrenceId theOccurrence) const
  {
    return myOccurrences.Get(theOccurrence);
  }

  //! Returns the shell reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::ShellRef& ShellRef(const BRepGraph_ShellRefId theRefId) const
  {
    return myShellRefs.Get(theRefId);
  }

  //! Returns the face reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::FaceRef& FaceRef(const BRepGraph_FaceRefId theRefId) const
  {
    return myFaceRefs.Get(theRefId);
  }

  //! Returns the wire reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::WireRef& WireRef(const BRepGraph_WireRefId theRefId) const
  {
    return myWireRefs.Get(theRefId);
  }

  //! Returns the vertex reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::VertexRef& VertexRef(const BRepGraph_VertexRefId theRefId) const
  {
    return myVertexRefs.Get(theRefId);
  }

  //! Returns the solid reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::SolidRef& SolidRef(const BRepGraph_SolidRefId theRefId) const
  {
    return mySolidRefs.Get(theRefId);
  }

  //! Returns the child reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::ChildRef& ChildRef(const BRepGraph_ChildRefId theRefId) const
  {
    return myChildRefs.Get(theRefId);
  }

  //! Returns the occurrence reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::OccurrenceRef& OccurrenceRef(
    const BRepGraph_OccurrenceRefId theRefId) const
  {
    return myOccurrenceRefs.Get(theRefId);
  }

  //! Returns a mutable reference to the vertex entity at the given typed id.
  //! @param[in] theVertex typed vertex id
  BRepGraphInc::VertexDef& ChangeVertex(const BRepGraph_VertexId theVertex)
  {
    return myVertices.Change(theVertex);
  }

  //! Returns a mutable reference to the edge entity at the given typed id.
  //! @param[in] theEdge typed edge id
  BRepGraphInc::EdgeDef& ChangeEdge(const BRepGraph_EdgeId theEdge)
  {
    return myEdges.Change(theEdge);
  }

  //! Returns a mutable reference to the coedge entity at the given typed id.
  //! @param[in] theCoEdge typed coedge id
  BRepGraphInc::CoEdgeDef& ChangeCoEdge(const BRepGraph_CoEdgeId theCoEdge)
  {
    return myCoEdges.Change(theCoEdge);
  }

  //! Returns a mutable reference to the wire entity at the given typed id.
  //! @param[in] theWire typed wire id
  BRepGraphInc::WireDef& ChangeWire(const BRepGraph_WireId theWire)
  {
    return myWires.Change(theWire);
  }

  //! Returns a mutable reference to the face entity at the given typed id.
  //! @param[in] theFace typed face id
  BRepGraphInc::FaceDef& ChangeFace(const BRepGraph_FaceId theFace)
  {
    return myFaces.Change(theFace);
  }

  //! Returns a mutable reference to the shell entity at the given typed id.
  //! @param[in] theShell typed shell id
  BRepGraphInc::ShellDef& ChangeShell(const BRepGraph_ShellId theShell)
  {
    return myShells.Change(theShell);
  }

  //! Returns a mutable reference to the solid entity at the given typed id.
  //! @param[in] theSolid typed solid id
  BRepGraphInc::SolidDef& ChangeSolid(const BRepGraph_SolidId theSolid)
  {
    return mySolids.Change(theSolid);
  }

  //! Returns a mutable reference to the compound entity at the given typed id.
  //! @param[in] theCompound typed compound id
  BRepGraphInc::CompoundDef& ChangeCompound(const BRepGraph_CompoundId theCompound)
  {
    return myCompounds.Change(theCompound);
  }

  //! Returns a mutable reference to the compsolid entity at the given typed id.
  //! @param[in] theCompSolid typed comp-solid id
  BRepGraphInc::CompSolidDef& ChangeCompSolid(const BRepGraph_CompSolidId theCompSolid)
  {
    return myCompSolids.Change(theCompSolid);
  }

  //! Returns a mutable reference to the product entity at the given typed id.
  //! @param[in] theProduct typed product id
  BRepGraphInc::ProductDef& ChangeProduct(const BRepGraph_ProductId theProduct)
  {
    return myProducts.Change(theProduct);
  }

  //! Returns a mutable reference to the occurrence entity at the given typed id.
  //! @param[in] theOccurrence typed occurrence id
  BRepGraphInc::OccurrenceDef& ChangeOccurrence(const BRepGraph_OccurrenceId theOccurrence)
  {
    return myOccurrences.Change(theOccurrence);
  }

  //! Returns a mutable reference to the shell reference entry at the given typed id.
  BRepGraphInc::ShellRef& ChangeShellRef(const BRepGraph_ShellRefId theRefId)
  {
    return myShellRefs.Change(theRefId);
  }

  //! Returns a mutable reference to the face reference entry at the given typed id.
  BRepGraphInc::FaceRef& ChangeFaceRef(const BRepGraph_FaceRefId theRefId)
  {
    return myFaceRefs.Change(theRefId);
  }

  //! Returns a mutable reference to the wire reference entry at the given typed id.
  BRepGraphInc::WireRef& ChangeWireRef(const BRepGraph_WireRefId theRefId)
  {
    return myWireRefs.Change(theRefId);
  }

  //! Returns a mutable reference to the vertex reference entry at the given typed id.
  BRepGraphInc::VertexRef& ChangeVertexRef(const BRepGraph_VertexRefId theRefId)
  {
    return myVertexRefs.Change(theRefId);
  }

  //! Returns a mutable reference to the solid reference entry at the given typed id.
  BRepGraphInc::SolidRef& ChangeSolidRef(const BRepGraph_SolidRefId theRefId)
  {
    return mySolidRefs.Change(theRefId);
  }

  //! Returns a mutable reference to the child reference entry at the given typed id.
  BRepGraphInc::ChildRef& ChangeChildRef(const BRepGraph_ChildRefId theRefId)
  {
    return myChildRefs.Change(theRefId);
  }

  //! Returns a mutable reference to the occurrence reference entry at the given typed id.
  BRepGraphInc::OccurrenceRef& ChangeOccurrenceRef(const BRepGraph_OccurrenceRefId theRefId)
  {
    return myOccurrenceRefs.Change(theRefId);
  }

  //! Return the face relations for a given face identifier.
  //! @param[in] theId face identifier
  //! @return const reference to the face relation representation
  [[nodiscard]] const BRepGraphInc::FaceRelations& FaceRelations(const BRepGraph_FaceId theId) const
  {
    return myFaceRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the wire relations for a given wire identifier.
  //! @param[in] theId wire identifier
  //! @return const reference to the wire relation representation
  [[nodiscard]] const BRepGraphInc::WireRelations& WireRelations(const BRepGraph_WireId theId) const
  {
    return myWireRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the edge relations for a given edge identifier.
  //! @param[in] theId edge identifier
  //! @return const reference to the edge relation representation
  [[nodiscard]] const BRepGraphInc::EdgeRelations& EdgeRelations(const BRepGraph_EdgeId theId) const
  {
    return myEdgeRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the shell relations for a given shell identifier.
  //! @param[in] theId shell identifier
  //! @return const reference to the shell relation representation
  [[nodiscard]] const BRepGraphInc::ShellRelations& ShellRelations(
    const BRepGraph_ShellId theId) const
  {
    return myShellRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the solid relations for a given solid identifier.
  //! @param[in] theId solid identifier
  //! @return const reference to the solid relation representation
  [[nodiscard]] const BRepGraphInc::SolidRelations& SolidRelations(
    const BRepGraph_SolidId theId) const
  {
    return mySolidRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the compound relations for a given compound identifier.
  //! @param[in] theId compound identifier
  //! @return const reference to the compound relation representation
  [[nodiscard]] const BRepGraphInc::CompoundRelations& CompoundRelations(
    const BRepGraph_CompoundId theId) const
  {
    return myCompoundRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the compsolid relations for a given compsolid identifier.
  //! @param[in] theId compsolid identifier
  //! @return const reference to the compsolid relation representation
  [[nodiscard]] const BRepGraphInc::CompSolidRelations& CompSolidRelations(
    const BRepGraph_CompSolidId theId) const
  {
    return myCompSolidRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the vertex relations for a given vertex identifier.
  //! @param[in] theId vertex identifier
  //! @return const reference to the vertex relation representation
  [[nodiscard]] const BRepGraphInc::VertexRelations& VertexRelations(
    const BRepGraph_VertexId theId) const
  {
    return myVertexRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the product relations for a given product identifier.
  //! @param[in] theId product identifier
  //! @return const reference to the product relation representation
  [[nodiscard]] const BRepGraphInc::ProductRelations& ProductRelations(
    const BRepGraph_ProductId theId) const
  {
    return myProductRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the occurrence relations for a given occurrence identifier.
  //! @param[in] theId occurrence identifier
  //! @return const reference to the occurrence relation representation
  [[nodiscard]] const BRepGraphInc::OccurrenceRelations& OccurrenceRelations(
    const BRepGraph_OccurrenceId theId) const
  {
    return myOccurrenceRelations.Value(static_cast<size_t>(theId.Index));
  }

  //! Return the compound child reference identifiers that point to a given node.
  //! @param[in] theNode node identifier
  //! @return const reference to the list of child reference identifiers
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_ChildRefId>&
                                      CompoundRefsOfNode(const BRepGraph_NodeId theNode) const;

  //! Return the occurrence reference identifiers that point to a given node.
  //! @param[in] theNode node identifier
  //! @return const reference to the list of occurrence reference identifiers
  [[nodiscard]] Standard_EXPORT const NCollection_LinearVector<BRepGraph_OccurrenceRefId>&
                                      OccurrenceRefsOfNode(const BRepGraph_NodeId theNode) const;

  //! Appends a new vertex entity and returns its typed id.
  BRepGraph_VertexId AppendVertex()
  {
    const BRepGraph_VertexId anId = myVertices.Append();
    myVertexRelations.Appended();
    return anId;
  }

  //! Appends a new edge entity and returns its typed id.
  BRepGraph_EdgeId AppendEdge()
  {
    const BRepGraph_EdgeId anId = myEdges.Append();
    myEdgeRelations.Appended();
    return anId;
  }

  //! Appends a new coedge entity and returns its typed id.
  BRepGraph_CoEdgeId AppendCoEdge() { return myCoEdges.Append(); }

  //! Appends a new wire entity and returns its typed id.
  BRepGraph_WireId AppendWire()
  {
    const BRepGraph_WireId anId = myWires.Append();
    myWireRelations.Appended();
    return anId;
  }

  //! Appends a new face entity and returns its typed id.
  BRepGraph_FaceId AppendFace()
  {
    const BRepGraph_FaceId anId = myFaces.Append();
    myFaceRelations.Appended();
    return anId;
  }

  //! Appends a new shell entity and returns its typed id.
  BRepGraph_ShellId AppendShell()
  {
    const BRepGraph_ShellId anId = myShells.Append();
    myShellRelations.Appended();
    return anId;
  }

  //! Appends a new solid entity and returns its typed id.
  BRepGraph_SolidId AppendSolid()
  {
    const BRepGraph_SolidId anId = mySolids.Append();
    mySolidRelations.Appended();
    return anId;
  }

  //! Appends a new compound entity and returns its typed id.
  BRepGraph_CompoundId AppendCompound()
  {
    const BRepGraph_CompoundId anId = myCompounds.Append();
    myCompoundRelations.Appended();
    return anId;
  }

  //! Appends a new compsolid entity and returns its typed id.
  BRepGraph_CompSolidId AppendCompSolid()
  {
    const BRepGraph_CompSolidId anId = myCompSolids.Append();
    myCompSolidRelations.Appended();
    return anId;
  }

  //! Appends a new product entity and returns its typed id.
  BRepGraph_ProductId AppendProduct()
  {
    const BRepGraph_ProductId anId = myProducts.Append();
    myProductRelations.Appended();
    return anId;
  }

  //! Appends a new occurrence entity and returns its typed id.
  BRepGraph_OccurrenceId AppendOccurrence()
  {
    const BRepGraph_OccurrenceId anId = myOccurrences.Append();
    myOccurrenceRelations.Appended();
    return anId;
  }

  //! Appends a new shell reference entry and returns its typed id.
  BRepGraph_ShellRefId AppendShellRef() { return myShellRefs.Append(); }

  //! Appends a new face reference entry and returns its typed id.
  BRepGraph_FaceRefId AppendFaceRef() { return myFaceRefs.Append(); }

  //! Appends a new wire reference entry and returns its typed id.
  BRepGraph_WireRefId AppendWireRef() { return myWireRefs.Append(); }

  //! Appends a new vertex reference entry and returns its typed id.
  BRepGraph_VertexRefId AppendVertexRef() { return myVertexRefs.Append(); }

  //! Appends a new solid reference entry and returns its typed id.
  BRepGraph_SolidRefId AppendSolidRef() { return mySolidRefs.Append(); }

  //! Appends a new child reference entry and returns its typed id.
  BRepGraph_ChildRefId AppendChildRef() { return myChildRefs.Append(); }

  //! Appends a new occurrence reference entry and returns its typed id.
  BRepGraph_OccurrenceRefId AppendOccurrenceRef() { return myOccurrenceRefs.Append(); }

  //! Create a coedge use record binding an edge to a wire within a face context.
  //! @param[in] theParentWireId owning wire identifier
  //! @param[in] theChildEdgeId  referenced edge identifier
  //! @param[in] theFaceId       face context identifier
  //! @param[in] theOrientation  orientation of the coedge
  //! @return the newly created coedge identifier
  Standard_EXPORT BRepGraph_CoEdgeId
    CreateCoEdgeUse(const BRepGraph_WireId                theParentWireId,
                    const BRepGraph_EdgeId                theChildEdgeId,
                    const BRepGraph_FaceId                theFaceId,
                    const BRepGraphInc::ParityOrientation theOrientation);

  //! Attach an edge to a vertex by creating a vertex reference.
  //! @param[in] theEdgeId   edge identifier
  //! @param[in] theVertexId vertex identifier
  Standard_EXPORT void AttachEdgeToVertex(const BRepGraph_EdgeId   theEdgeId,
                                          const BRepGraph_VertexId theVertexId);

  //! Attach a wire to a face by creating a wire reference.
  //! @param[in] theParentFaceId parent face identifier
  //! @param[in] theChildWireId  child wire identifier
  //! @param[in] theOrientation  orientation within parent
  //! @return the newly created wire reference identifier
  Standard_EXPORT BRepGraph_WireRefId
    AttachWireToFace(const BRepGraph_FaceId                theParentFaceId,
                     const BRepGraph_WireId                theChildWireId,
                     const BRepGraphInc::ParityOrientation theOrientation = TopAbs_FORWARD);

  //! Attach a face to a shell by creating a face reference.
  //! @param[in] theParentShellId parent shell identifier
  //! @param[in] theChildFaceId   child face identifier
  //! @param[in] theOrientation   orientation within parent
  //! @return the newly created face reference identifier
  Standard_EXPORT BRepGraph_FaceRefId
    AttachFaceToShell(const BRepGraph_ShellId               theParentShellId,
                      const BRepGraph_FaceId                theChildFaceId,
                      const BRepGraphInc::ParityOrientation theOrientation = TopAbs_FORWARD);

  //! Attach a shell to a solid by creating a shell reference.
  //! @param[in] theParentSolidId parent solid identifier
  //! @param[in] theChildShellId  child shell identifier
  //! @param[in] theOrientation   orientation within parent
  //! @return the newly created shell reference identifier
  Standard_EXPORT BRepGraph_ShellRefId
    AttachShellToSolid(const BRepGraph_SolidId               theParentSolidId,
                       const BRepGraph_ShellId               theChildShellId,
                       const BRepGraphInc::ParityOrientation theOrientation = TopAbs_FORWARD);

  //! Attach a solid to a compsolid by creating a solid reference.
  //! @param[in] theParentCompSolidId parent compsolid identifier
  //! @param[in] theChildSolidId      child solid identifier
  //! @param[in] theOrientation       orientation within parent
  //! @return the newly created solid reference identifier
  Standard_EXPORT BRepGraph_SolidRefId
    AttachSolidToCompSolid(const BRepGraph_CompSolidId           theParentCompSolidId,
                           const BRepGraph_SolidId               theChildSolidId,
                           const BRepGraphInc::ParityOrientation theOrientation = TopAbs_FORWARD);

  //! Attach a child node to a compound by creating a child reference.
  //! @param[in] theParentCompoundId parent compound identifier
  //! @param[in] theChildNodeId      child node identifier
  //! @param[in] theLocation         optional location transformation
  //! @param[in] theOrientation      orientation within parent
  //! @return the newly created child reference identifier
  Standard_EXPORT BRepGraph_ChildRefId
    AttachChildToCompound(const BRepGraph_CompoundId            theParentCompoundId,
                          const BRepGraph_NodeId                theChildNodeId,
                          const TopLoc_Location&                theLocation    = TopLoc_Location(),
                          const BRepGraphInc::ParityOrientation theOrientation = TopAbs_FORWARD);

  //! Attach an occurrence to a product by creating an occurrence reference.
  //! @param[in] theParentProductId     parent product identifier
  //! @param[in] theChildOccurrenceId   child occurrence identifier
  //! @param[in] theLocation            optional location transformation
  //! @return the newly created occurrence reference identifier
  Standard_EXPORT BRepGraph_OccurrenceRefId
    AttachOccurrenceToProduct(const BRepGraph_ProductId    theParentProductId,
                              const BRepGraph_OccurrenceId theChildOccurrenceId,
                              const TopLoc_Location&       theLocation = TopLoc_Location());

  //! Detach a coedge use from its parent wire.
  //! @param[in] theParentWireId owning wire identifier
  //! @param[in] theCoEdgeId     coedge identifier to detach
  //! @return true if the coedge was found and removed
  Standard_EXPORT bool DetachCoEdgeUse(const BRepGraph_WireId   theParentWireId,
                                       const BRepGraph_CoEdgeId theCoEdgeId);

  //! Replace a single coedge with a pair of new coedges in a wire.
  //! @param[in] theParentWireId    owning wire identifier
  //! @param[in] theOldCoEdgeId     coedge to replace
  //! @param[in] theNewFirstCoEdgeId  first replacement coedge
  //! @param[in] theNewSecondCoEdgeId second replacement coedge
  //! @return true if the replacement succeeded
  Standard_EXPORT bool ReplaceCoEdgeUseWithPair(const BRepGraph_WireId   theParentWireId,
                                                const BRepGraph_CoEdgeId theOldCoEdgeId,
                                                const BRepGraph_CoEdgeId theNewFirstCoEdgeId,
                                                const BRepGraph_CoEdgeId theNewSecondCoEdgeId);

  //! Detach a wire reference from its parent face.
  //! @param[in] theParentFaceId parent face identifier
  //! @param[in] theRefId        wire reference identifier to detach
  //! @return true if the reference was found and removed
  Standard_EXPORT bool DetachWireFromFace(const BRepGraph_FaceId    theParentFaceId,
                                          const BRepGraph_WireRefId theRefId);

  //! Detach a face reference from its parent shell.
  //! @param[in] theParentShellId parent shell identifier
  //! @param[in] theRefId         face reference identifier to detach
  //! @return true if the reference was found and removed
  Standard_EXPORT bool DetachFaceFromShell(const BRepGraph_ShellId   theParentShellId,
                                           const BRepGraph_FaceRefId theRefId);

  //! Detach a shell reference from its parent solid.
  //! @param[in] theParentSolidId parent solid identifier
  //! @param[in] theRefId         shell reference identifier to detach
  //! @return true if the reference was found and removed
  Standard_EXPORT bool DetachShellFromSolid(const BRepGraph_SolidId    theParentSolidId,
                                            const BRepGraph_ShellRefId theRefId);

  //! Detach a solid reference from its parent compsolid.
  //! @param[in] theParentCompSolidId parent compsolid identifier
  //! @param[in] theRefId             solid reference identifier to detach
  //! @return true if the reference was found and removed
  Standard_EXPORT bool DetachSolidFromCompSolid(const BRepGraph_CompSolidId theParentCompSolidId,
                                                const BRepGraph_SolidRefId  theRefId);

  //! Detach a child reference from its parent compound.
  //! @param[in] theParentCompoundId parent compound identifier
  //! @param[in] theRefId            child reference identifier to detach
  //! @return true if the reference was found and removed
  Standard_EXPORT bool DetachChildFromCompound(const BRepGraph_CompoundId theParentCompoundId,
                                               const BRepGraph_ChildRefId theRefId);

  //! Detach an occurrence reference from its parent product.
  //! @param[in] theParentProductId parent product identifier
  //! @param[in] theRefId           occurrence reference identifier to detach
  //! @return true if the reference was found and removed
  Standard_EXPORT bool DetachOccurrenceFromProduct(const BRepGraph_ProductId theParentProductId,
                                                   const BRepGraph_OccurrenceRefId theRefId);

  //! Rebind the child node of an occurrence to a new node.
  //! @param[in] theOccurrence occurrence identifier
  //! @param[in] theOldChild   old child node identifier
  //! @param[in] theNewChild   new child node identifier
  Standard_EXPORT void RebindOccurrenceChild(const BRepGraph_OccurrenceId theOccurrence,
                                             const BRepGraph_NodeId       theOldChild,
                                             const BRepGraph_NodeId       theNewChild);

  //! Rebind vertex edge references from one vertex to another, excluding a specific ref.
  //! @param[in] theOldVertex   old vertex identifier
  //! @param[in] theNewVertex   new vertex identifier
  //! @param[in] theEdge        edge identifier
  //! @param[in] theExcludingRef reference identifier to exclude from rebinding
  Standard_EXPORT void RebindVertexEdge(const BRepGraph_VertexId    theOldVertex,
                                        const BRepGraph_VertexId    theNewVertex,
                                        const BRepGraph_EdgeId      theEdge,
                                        const BRepGraph_VertexRefId theExcludingRef);

  //! Rebind a vertex reference to point to a new vertex.
  //! @param[in] theRefId     vertex reference identifier
  //! @param[in] theOldVertex old vertex identifier
  //! @param[in] theNewVertex new vertex identifier
  Standard_EXPORT void RebindVertexRef(const BRepGraph_VertexRefId theRefId,
                                       const BRepGraph_VertexId    theOldVertex,
                                       const BRepGraph_VertexId    theNewVertex);

  //! Rebind a coedge to reference a different edge.
  //! @param[in] theCoEdge  coedge identifier
  //! @param[in] theOldEdge old edge identifier
  //! @param[in] theNewEdge new edge identifier
  Standard_EXPORT void RebindCoEdgeEdge(const BRepGraph_CoEdgeId theCoEdge,
                                        const BRepGraph_EdgeId   theOldEdge,
                                        const BRepGraph_EdgeId   theNewEdge);

  //! Rebind a wire reference to point to a new wire.
  //! @param[in] theRefId   wire reference identifier
  //! @param[in] theOldWire old wire identifier
  //! @param[in] theNewWire new wire identifier
  Standard_EXPORT void RebindWireRef(const BRepGraph_WireRefId theRefId,
                                     const BRepGraph_WireId    theOldWire,
                                     const BRepGraph_WireId    theNewWire);

  //! Rebind a face reference to point to a new face.
  //! @param[in] theRefId   face reference identifier
  //! @param[in] theOldFace old face identifier
  //! @param[in] theNewFace new face identifier
  Standard_EXPORT void RebindFaceRef(const BRepGraph_FaceRefId theRefId,
                                     const BRepGraph_FaceId    theOldFace,
                                     const BRepGraph_FaceId    theNewFace);

  //! Rebind a shell reference to point to a new shell.
  //! @param[in] theRefId    shell reference identifier
  //! @param[in] theOldShell old shell identifier
  //! @param[in] theNewShell new shell identifier
  Standard_EXPORT void RebindShellRef(const BRepGraph_ShellRefId theRefId,
                                      const BRepGraph_ShellId    theOldShell,
                                      const BRepGraph_ShellId    theNewShell);

  //! Rebind a solid reference to point to a new solid.
  //! @param[in] theRefId    solid reference identifier
  //! @param[in] theOldSolid old solid identifier
  //! @param[in] theNewSolid new solid identifier
  Standard_EXPORT void RebindSolidRef(const BRepGraph_SolidRefId theRefId,
                                      const BRepGraph_SolidId    theOldSolid,
                                      const BRepGraph_SolidId    theNewSolid);

  //! Rebind a child reference to point to a new child node.
  //! @param[in] theRefId   child reference identifier
  //! @param[in] theOldChild old child node identifier
  //! @param[in] theNewChild new child node identifier
  Standard_EXPORT void RebindChildRef(const BRepGraph_ChildRefId theRefId,
                                      const BRepGraph_NodeId     theOldChild,
                                      const BRepGraph_NodeId     theNewChild);

  //! Rebind an occurrence reference to point to a new occurrence.
  //! @param[in] theRefId         occurrence reference identifier
  //! @param[in] theOldOccurrence old occurrence identifier
  //! @param[in] theNewOccurrence new occurrence identifier
  Standard_EXPORT void RebindOccurrenceRef(const BRepGraph_OccurrenceRefId theRefId,
                                           const BRepGraph_OccurrenceId    theOldOccurrence,
                                           const BRepGraph_OccurrenceId    theNewOccurrence);

  //! Reverse the order of coedges in a wire.
  //! @param[in] theWireId wire identifier
  Standard_EXPORT void ReverseWireCoEdges(const BRepGraph_WireId theWireId);

  //! Replace the coedge list of a wire with a new set.
  //! @param[in] theWireId    wire identifier
  //! @param[in] theCoEdgeIds new coedge identifiers
  Standard_EXPORT void SetWireCoEdges(const BRepGraph_WireId                        theWireId,
                                      const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds);

  //! Replace the wire reference list of a face with a new set.
  //! @param[in] theFaceId    face identifier
  //! @param[in] theWireRefIds new wire reference identifiers
  Standard_EXPORT void SetFaceWireRefs(
    const BRepGraph_FaceId                         theFaceId,
    const NCollection_Array1<BRepGraph_WireRefId>& theWireRefIds);

  //! Replace the face reference list of a shell with a new set.
  //! @param[in] theShellId   shell identifier
  //! @param[in] theFaceRefIds new face reference identifiers
  Standard_EXPORT void SetShellFaceRefs(
    const BRepGraph_ShellId                        theShellId,
    const NCollection_Array1<BRepGraph_FaceRefId>& theFaceRefIds);

  //! Replace the shell reference list of a solid with a new set.
  //! @param[in] theSolidId    solid identifier
  //! @param[in] theShellRefIds new shell reference identifiers
  Standard_EXPORT void SetSolidShellRefs(
    const BRepGraph_SolidId                         theSolidId,
    const NCollection_Array1<BRepGraph_ShellRefId>& theShellRefIds);

  //! Replace the solid reference list of a compsolid with a new set.
  //! @param[in] theCompSolidId compsolid identifier
  //! @param[in] theSolidRefIds new solid reference identifiers
  Standard_EXPORT void SetCompSolidSolidRefs(
    const BRepGraph_CompSolidId                     theCompSolidId,
    const NCollection_Array1<BRepGraph_SolidRefId>& theSolidRefIds);

  //! Replace the child reference list of a compound with a new set.
  //! @param[in] theCompoundId compound identifier
  //! @param[in] theChildRefIds new child reference identifiers
  Standard_EXPORT void SetCompoundChildRefs(
    const BRepGraph_CompoundId                      theCompoundId,
    const NCollection_Array1<BRepGraph_ChildRefId>& theChildRefIds);

  //! Replace the occurrence reference list of a product with a new set.
  //! @param[in] theProductId       product identifier
  //! @param[in] theOccurrenceRefIds new occurrence reference identifiers
  Standard_EXPORT void SetProductOccurrenceRefs(
    const BRepGraph_ProductId                            theProductId,
    const NCollection_Array1<BRepGraph_OccurrenceRefId>& theOccurrenceRefIds);

  //! Return the BaseRef portion of any ref entry by generic RefId.
  //! @param[in] theRefId generic reference identifier
  //! @return const reference to the BaseRef base of the ref entry
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::BaseRef& BaseRef(
    const BRepGraph_RefId theRefId) const;

  //! Return the mutable BaseRef portion of any ref entry by generic RefId.
  //! @param[in] theRefId generic reference identifier
  //! @return mutable pointer to the BaseRef base of the ref entry, or nullptr if not found
  [[nodiscard]] Standard_EXPORT BRepGraphInc::BaseRef* ChangeBaseRef(
    const BRepGraph_RefId theRefId);

  //! Resolve an active node UID through storage reverse maps.
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId FindNodeIdByUID(const BRepGraph_UID& theUID) const;

  //! Resolve an active reference UID through storage reverse maps.
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId
    FindRefIdByUID(const BRepGraph_RefUID& theUID) const;

  //! Returns the node id bound to the given shape definition key, or invalid if not bound.
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    FindDefinitionByShape(const TopoDS_Shape& theShape) const;

  //! Returns true if the given shape definition key is bound to a node.
  [[nodiscard]] Standard_EXPORT bool HasShapeBinding(const TopoDS_Shape& theShape) const;

  //! Set or update the shape-to-node binding. Uses replacement semantics:
  //! binds if absent, updates if already bound.
  Standard_EXPORT void SetDefinitionShapeBinding(const TopoDS_Shape&    theShape,
                                                 const BRepGraph_NodeId theNodeId);

  //! Remove the shape-to-node binding only if it points to the expected node.
  //! Returns true if the binding was removed.
  Standard_EXPORT bool RemoveDefinitionShapeBinding(const TopoDS_Shape&    theShape,
                                                    const BRepGraph_NodeId theExpectedNodeId);

  //! Back-reference to the construction-time `TopoDS_Shape` key a node was built from.
  //! Only populated during graph construction; absent bindings are a valid state.

  //! Returns the original shape for the given node id, or a null shape if not bound.
  [[nodiscard]] Standard_EXPORT TopoDS_Shape FindOriginal(const BRepGraph_NodeId theNodeId) const;

  //! Returns true if the given node id has an original shape binding.
  [[nodiscard]] Standard_EXPORT bool HasOriginal(const BRepGraph_NodeId theNodeId) const;

  //! Binds the given node id to its construction-time shape key.
  Standard_EXPORT void BindOriginal(const BRepGraph_NodeId theNodeId, const TopoDS_Shape& theShape);

  //! Removes the original shape binding for the given node id.
  Standard_EXPORT void UnBindOriginal(const BRepGraph_NodeId theNodeId);

  //! Iterate all shape-to-NodeId bindings, invoking theFunc(shape, NodeId) for each entry.
  //! Used by Compact to rebuild the map after the rebuild-and-swap.
  template <typename FuncT>
  void ForEachShapeBinding(FuncT&& theFunc) const
  {
    std::shared_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
    for (NCollection_FlatDataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>::Iterator
           anIt(myShapeToNodeId);
         anIt.More();
         anIt.Next())
    {
      theFunc(anIt.Key(), anIt.Value());
    }
  }

  //! Iterate all NodeId-to-OriginalShape bindings, invoking theFunc(NodeId, Shape) for each entry.
  //! Used by Compact to rebuild the map after the rebuild-and-swap.
  template <typename FuncT>
  void ForEachOriginalBinding(FuncT&& theFunc) const
  {
    std::shared_lock<std::shared_mutex> aLock(myShapeBindingsMutex);
    for (NCollection_FlatDataMap<BRepGraph_NodeId, TopoDS_Shape>::Iterator anIt(myOriginalShapes);
         anIt.More();
         anIt.Next())
    {
      theFunc(anIt.Key(), anIt.Value());
    }
  }

  //! Copy shape-to-NodeId and Original shape bindings from another storage.
  //! Used by identity copy to preserve shape reconstruction bindings.
  Standard_EXPORT void CopyShapeBindingsFrom(const BRepGraphInc_Storage& theSource);

  //! Return the generation-validated node-to-shape reconstruction cache.
  [[nodiscard]] const NCollection_FlatDataMap<BRepGraph_NodeId, CachedShape>& CurrentShapes() const
  {
    return myCurrentShapes;
  }

  //! Return the mutable generation-validated node-to-shape reconstruction cache.
  NCollection_FlatDataMap<BRepGraph_NodeId, CachedShape>& ChangeCurrentShapes()
  {
    return myCurrentShapes;
  }

  //! Return the mutex protecting the reconstruction cache.
  [[nodiscard]] std::shared_mutex& CurrentShapesMutex() const { return myCurrentShapesMutex; }

  //! Clear the generation-validated shape reconstruction cache.
  Standard_EXPORT void ClearCurrentShapes();

  //! Remove one entry from the generation-validated shape reconstruction cache.
  Standard_EXPORT void UnbindCurrentShape(const BRepGraph_NodeId theNode);

  //! Clear deferred invalidation queues and release their batch allocator.
  Standard_EXPORT void ClearDeferredQueues();

  //! Clear all storage.
  Standard_EXPORT void Clear();

  //! Prepare fixed-size destination ranges for indexed load.
  //!
  //! This is an internal backend preparation API intended for persistence read
  //! paths that know final section sizes in advance. It clears previous content,
  //! pre-sizes defs/refs/reps and UID vectors, and initializes relation tables
  //! exactly once. The load path then restores serialized relation lists and
  //! calls RebuildDerivedRelations() once to refresh derived incoming maps.
  //! @param theCounts final per-section slot counts.
  Standard_EXPORT void PrepareForLoad(const BRepGraphInc_Load::Counts& theCounts);

  //! Override active-slot counters after a trusted indexed load path.
  //!
  //! This is intended for persistence backends that already touched every slot
  //! during load and therefore know exact active counts without rescanning
  //! storage after relation construction.
  //! @param theCounts trusted active per-section counts.
  Standard_EXPORT void SetActiveCounts(const BRepGraphInc_Load::Counts& theCounts);

  //! Build a Counts struct from current allocated slot counts.
  [[nodiscard]] Standard_EXPORT BRepGraphInc_Load::Counts Counts() const;

  //! Build a Counts struct from current active (non-removed) counts.
  [[nodiscard]] Standard_EXPORT BRepGraphInc_Load::Counts ActiveCounts() const;

  //! Recount active-slot counters from current `IsRemoved` flags without rebuilding indexes.
  Standard_EXPORT void RecountActiveCounts();

  //! Rebuild centralized relation tables from entity and reference endpoints.
  //! This is intended for raw load, compact, and explicit repair paths only;
  //! editor mutations maintain relation containers incrementally.
  Standard_EXPORT void RebuildDerivedRelations();

  //! Rebuild relation maps after a trusted load already restored active counts.
  Standard_EXPORT void RebuildDerivedRelationsPreservingActiveCounts();

  //! Bulk-copy all RemovedFlags bit-planes from theSource.
  //! Source must have been loaded with the same entity counts (identity copy path).
  Standard_EXPORT void CopyRemovedFlagsFrom(const BRepGraphInc_Storage& theSource);

  //! Debug: verify relation-table consistency against entity/reference endpoints.
  //! @return true if all relations are consistent
  Standard_EXPORT bool ValidateRelations() const;

  //! Verify coedge ordering consistency for a specific wire.
  //! @param[in] theWireId wire identifier
  //! @return true if the coedge order is valid
  Standard_EXPORT bool ValidateWireCoEdgeOrder(const BRepGraph_WireId theWireId) const;

  //! Verify coedge ordering consistency for all wires.
  //! @return true if all wire coedge orders are valid
  Standard_EXPORT bool ValidateWireCoEdgeOrders() const;

  //! Result of wire coedge order canonicalization.
  enum class WireCoEdgeOrderStatus
  {
    Connected,        //!< Stored order was already connected.
    Reordered,        //!< Stored coedges were reordered into an exactly connected chain.
    ToleranceOrdered, //!< Stored coedges were ordered using tolerance-equivalent endpoints.
    Partial,          //!< Stored coedges were grouped into best-effort connected runs.
    InvalidInput      //!< Wire or coedge ownership/input data is invalid.
  };

  //! Canonicalize the coedge ordering of a wire and report the achieved order quality.
  //! @param[in] theWireId wire identifier
  //! @return canonicalization status
  Standard_EXPORT WireCoEdgeOrderStatus
    CanonicalizeWireCoEdgeOrderStatus(const BRepGraph_WireId theWireId);

  //! Canonicalize the coedge ordering of a wire to a consistent form.
  //! @param[in] theWireId wire identifier
  //! @return true if canonicalization succeeded
  Standard_EXPORT bool CanonicalizeWireCoEdgeOrder(const BRepGraph_WireId theWireId);

  //! Rebuild UID reverse indexes (UID->NodeId, RefUID->RefId)
  //! from the current UID vectors. Clears indexes and resets allocators before rebuilding.
  //! Called after Compact, Load, etc. where UID vectors have been modified externally.
  Standard_EXPORT void RebuildUIDReverseIndexes();

  //! Mark UID reverse indexes stale after bulk UID-vector replacement.
  Standard_EXPORT void MarkUIDReverseIndexesDirty();

  //! Lazily rebuild the node UID reverse index if it is stale.
  Standard_EXPORT void EnsureUIDReverseIndex() const;

  //! Lazily rebuild the reference UID reverse index if it is stale.
  Standard_EXPORT void EnsureRefUIDReverseIndex() const;

  //! Copy all forward/reverse relation vectors directly from theSource.
  //! Used by identity copy to avoid the clear+rebuild cycle.
  Standard_EXPORT void CopyDerivedRelationsFrom(const BRepGraphInc_Storage& theSource);

private:
  friend class BRepGraphInc_Populate;
  friend class BRepGraph;

  Standard_EXPORT void ClearStorageForReuse();
  Standard_EXPORT void ClearUIDIndexes();
  Standard_EXPORT void ClearShapeCache();
  Standard_EXPORT void ClearRelations();

  BRepGraphInc::FaceRelations& ChangeFaceRelationsInternal(const BRepGraph_FaceId theId)
  {
    return myFaceRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::WireRelations& ChangeWireRelationsInternal(const BRepGraph_WireId theId)
  {
    return myWireRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::EdgeRelations& ChangeEdgeRelationsInternal(const BRepGraph_EdgeId theId)
  {
    return myEdgeRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::ShellRelations& ChangeShellRelationsInternal(const BRepGraph_ShellId theId)
  {
    return myShellRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::SolidRelations& ChangeSolidRelationsInternal(const BRepGraph_SolidId theId)
  {
    return mySolidRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::CompoundRelations& ChangeCompoundRelationsInternal(const BRepGraph_CompoundId theId)
  {
    return myCompoundRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::CompSolidRelations& ChangeCompSolidRelationsInternal(
    const BRepGraph_CompSolidId theId)
  {
    return myCompSolidRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::VertexRelations& ChangeVertexRelationsInternal(const BRepGraph_VertexId theId)
  {
    return myVertexRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::ProductRelations& ChangeProductRelationsInternal(const BRepGraph_ProductId theId)
  {
    return myProductRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  BRepGraphInc::OccurrenceRelations& ChangeOccurrenceRelationsInternal(
    const BRepGraph_OccurrenceId theId)
  {
    return myOccurrenceRelations.ChangeValue(static_cast<size_t>(theId.Index));
  }

  Standard_EXPORT NCollection_LinearVector<BRepGraph_ChildRefId>& ChangeCompoundRefsOfNodeInternal(
    const BRepGraph_NodeId theNode);

  Standard_EXPORT NCollection_LinearVector<BRepGraph_OccurrenceRefId>&
                  ChangeOccurrenceRefsOfNodeInternal(const BRepGraph_NodeId theNode);

  Standard_EXPORT void rebuildDerivedRelationsInternal(const bool theRecountActiveCounts);

  //! Return true if the typed entity has at least one parent compound (internal).
  template <typename T>
  [[nodiscard]] bool HasCompoundParentTyped(const T theId) const;

  //! Set or clear the "has parent compound" flag for a typed entity (internal).
  template <typename T>
  void SetHasCompoundParentTyped(const T theId, const bool theVal);

  //! Return true if the typed entity has at least one parent occurrence (internal).
  template <typename T>
  [[nodiscard]] bool HasOccurrenceParentTyped(const T theId) const;

  //! Set or clear the "has parent occurrence" flag for a typed entity (internal).
  template <typename T>
  void SetHasOccurrenceParentTyped(const T theId, const bool theVal);

  //! Set or clear the "has parent compound" flag for a generic NodeId (internal dispatch).
  Standard_EXPORT void SetHasCompoundParent(const BRepGraph_NodeId theNode, bool theVal);

  //! Set or clear the "has parent occurrence" flag for a generic NodeId (internal dispatch).
  Standard_EXPORT void SetHasOccurrenceParent(const BRepGraph_NodeId theNode, bool theVal);

  //! Template store for topology entity kinds.
  template <typename EntityT>
  struct DefStore
  {
    using TypeId    = typename EntityT::TypeId;
    using ValueType = EntityT;

    //! Entity representations stored by typed id index.
    NCollection_DynamicArray<EntityT> Entities;

    //! Bit-flag plane for soft-removal status.
    BRepGraphInc_BitFlags RemovedFlags;

    //! Bit-flag plane for ownership status.
    BRepGraphInc_BitFlags OwnedFlags;

    //! Bit-flag plane for active MutGuard tracking.
    BRepGraphInc_BitFlags GuardFlags;

    //! Bit-flag plane: node has at least one parent compound (ChildRef).
    BRepGraphInc_BitFlags HasCompoundParentFlags;

    //! Bit-flag plane: node has at least one parent occurrence (OccurrenceRef).
    BRepGraphInc_BitFlags HasOccurrenceParentFlags;

    //! Number of non-removed entities currently present in the store.
    uint32_t NbActive = 0;

    //! Per-kind monotonic UID counter. Valid UIDs start at 1.
    std::atomic<uint32_t> NextUIDCounter{1};

    DefStore() = delete;

    DefStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Entities(theBlockSize, theAlloc)
    {
    }

    uint32_t Nb() const { return static_cast<uint32_t>(Entities.Size()); }

    const EntityT& Get(const TypeId theId) const
    {
      return Entities.Value(static_cast<size_t>(theId.Index));
    }

    EntityT& Change(const TypeId theId)
    {
      return Entities.ChangeValue(static_cast<size_t>(theId.Index));
    }

    //! Append a default-constructed entity and return its typed slot id.
    TypeId Append()
    {
      const TypeId anId(static_cast<uint32_t>(Entities.Size()));
      ++NbActive;
      Entities.Appended();
      RemovedFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      OwnedFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      GuardFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      HasCompoundParentFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      HasOccurrenceParentFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      return anId;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0u, "DefStore::DecrementActive: underflow");
      if (NbActive > 0u)
      {
        --NbActive;
      }
    }

    bool MarkRemoved(const TypeId theId)
    {
      if (!theId.IsValid(Nb()))
      {
        return false;
      }

      if (RemovedFlags.Test(theId.Index))
      {
        return false;
      }
      RemovedFlags.Set(theId.Index);
      DecrementActive();
      return true;
    }

    void Clear(const bool theReleaseMemory = false)
    {
      Entities.Clear(theReleaseMemory);
      RemovedFlags.ClearAll();
      OwnedFlags.ClearAll();
      GuardFlags.ClearAll();
      HasCompoundParentFlags.ClearAll();
      HasOccurrenceParentFlags.ClearAll();
      NbActive = 0;
      // Note: NextUIDCounter is NOT reset. UIDs stay monotonic across Clear() cycles.
      // Generation + GraphGUID protect against stale UID aliasing.
    }
  };

  //! Template store for transitional reference kinds.
  template <typename RefT>
  struct RefStore
  {
    using TypeId    = typename RefT::TypeId;
    using ValueType = RefT;

    //! Reference representations stored by typed id index.
    NCollection_DynamicArray<RefT> Refs;

    //! Bit-flag plane for soft-removal status.
    BRepGraphInc_BitFlags RemovedFlags;

    //! Bit-flag plane for ownership status.
    BRepGraphInc_BitFlags OwnedFlags;

    //! Bit-flag plane for active MutGuard tracking.
    BRepGraphInc_BitFlags GuardFlags;

    //! Bit-flag plane: ref has at least one parent compound (unused for refs, kept for macro
    //! uniformity).
    BRepGraphInc_BitFlags HasCompoundParentFlags;

    //! Bit-flag plane: ref has at least one parent occurrence (unused for refs, kept for macro
    //! uniformity).
    BRepGraphInc_BitFlags HasOccurrenceParentFlags;

    //! Number of non-removed references currently present in the store.
    uint32_t NbActive = 0;

    //! Per-kind monotonic UID counter. Valid UIDs start at 1.
    std::atomic<uint32_t> NextUIDCounter{1};

    RefStore() = delete;

    RefStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Refs(theBlockSize, theAlloc)
    {
    }

    uint32_t Nb() const { return static_cast<uint32_t>(Refs.Size()); }

    const RefT& Get(const TypeId theId) const
    {
      return Refs.Value(static_cast<size_t>(theId.Index));
    }

    RefT& Change(const TypeId theId) { return Refs.ChangeValue(static_cast<size_t>(theId.Index)); }

    //! Append a default-constructed ref entry and return its typed slot id.
    TypeId Append()
    {
      const TypeId anId(static_cast<uint32_t>(Refs.Size()));
      ++NbActive;
      Refs.Appended();
      RemovedFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      OwnedFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      GuardFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      HasCompoundParentFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      HasOccurrenceParentFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      return anId;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0u, "RefStore::DecrementActive: underflow");
      if (NbActive > 0u)
      {
        --NbActive;
      }
    }

    bool MarkRemoved(const TypeId theId)
    {
      if (!theId.IsValid(Nb()))
      {
        return false;
      }

      if (RemovedFlags.Test(theId.Index))
      {
        return false;
      }
      RemovedFlags.Set(theId.Index);
      DecrementActive();
      return true;
    }

    void Clear(const bool theReleaseMemory = false)
    {
      Refs.Clear(theReleaseMemory);
      RemovedFlags.ClearAll();
      OwnedFlags.ClearAll();
      GuardFlags.ClearAll();
      HasCompoundParentFlags.ClearAll();
      HasOccurrenceParentFlags.ClearAll();
      NbActive = 0;
      // Note: NextUIDCounter is NOT reset. UIDs stay monotonic across Clear() cycles.
    }
  };

  //! Primary allocator for backend arrays, stores, and transient backend maps.
  occ::handle<NCollection_IncAllocator> myAllocator = new NCollection_IncAllocator;

  //! Backend-owned root products and deferred invalidation queues.
  NCollection_LinearVector<BRepGraph_ProductId> myRootProductIds;
  NCollection_LinearVector<BRepGraph_NodeId>    myDeferredModified;
  NCollection_LinearVector<BRepGraph_RefId>     myDeferredRefModified;

  //! Vertex definition store.
  DefStore<BRepGraphInc::VertexDef> myVertices;

  //! Edge definition store.
  DefStore<BRepGraphInc::EdgeDef> myEdges;

  //! Coedge definition store.
  DefStore<BRepGraphInc::CoEdgeDef> myCoEdges;

  //! Wire definition store.
  DefStore<BRepGraphInc::WireDef> myWires;

  //! Face definition store.
  DefStore<BRepGraphInc::FaceDef> myFaces;

  //! Shell definition store.
  DefStore<BRepGraphInc::ShellDef> myShells;

  //! Solid definition store.
  DefStore<BRepGraphInc::SolidDef> mySolids;

  //! Compound definition store.
  DefStore<BRepGraphInc::CompoundDef> myCompounds;

  //! CompSolid definition store.
  DefStore<BRepGraphInc::CompSolidDef> myCompSolids;

  //! Product definition store.
  DefStore<BRepGraphInc::ProductDef> myProducts;

  //! Occurrence definition store.
  DefStore<BRepGraphInc::OccurrenceDef> myOccurrences;

  //! Shell reference store.
  RefStore<BRepGraphInc::ShellRef> myShellRefs;

  //! Face reference store.
  RefStore<BRepGraphInc::FaceRef> myFaceRefs;

  //! Wire reference store.
  RefStore<BRepGraphInc::WireRef> myWireRefs;

  //! Vertex reference store.
  RefStore<BRepGraphInc::VertexRef> myVertexRefs;

  //! Solid reference store.
  RefStore<BRepGraphInc::SolidRef> mySolidRefs;

  //! Child reference store.
  RefStore<BRepGraphInc::ChildRef> myChildRefs;

  //! Occurrence reference store.
  RefStore<BRepGraphInc::OccurrenceRef> myOccurrenceRefs;

  //! Centralized relation tables parallel to entity stores.
  NCollection_DynamicArray<BRepGraphInc::FaceRelations>       myFaceRelations;
  NCollection_DynamicArray<BRepGraphInc::WireRelations>       myWireRelations;
  NCollection_DynamicArray<BRepGraphInc::EdgeRelations>       myEdgeRelations;
  NCollection_DynamicArray<BRepGraphInc::ShellRelations>      myShellRelations;
  NCollection_DynamicArray<BRepGraphInc::SolidRelations>      mySolidRelations;
  NCollection_DynamicArray<BRepGraphInc::CompoundRelations>   myCompoundRelations;
  NCollection_DynamicArray<BRepGraphInc::CompSolidRelations>  myCompSolidRelations;
  NCollection_DynamicArray<BRepGraphInc::VertexRelations>     myVertexRelations;
  NCollection_DynamicArray<BRepGraphInc::ProductRelations>    myProductRelations;
  NCollection_DynamicArray<BRepGraphInc::OccurrenceRelations> myOccurrenceRelations;

  //! Sparse incoming compound child refs keyed by referenced node.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_ChildRefId>>
    myNodeToCompounds;

  //! Sparse incoming product occurrence refs keyed by occurrence child node.
  NCollection_DataMap<BRepGraph_NodeId, NCollection_LinearVector<BRepGraph_OccurrenceRefId>>
    myNodeToOccurrences;

  //! Representation-use store with removal tracking.
  template <typename UseT>
  struct RepStore
  {
    using TypeId = typename UseT::TypeId;

    NCollection_DynamicArray<UseT> Uses;
    BRepGraphInc_BitFlags          RemovedFlags;
    uint32_t                       NbActive = 0;

    RepStore() = delete;

    RepStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Uses(theBlockSize, theAlloc)
    {
    }

    uint32_t Nb() const { return static_cast<uint32_t>(Uses.Size()); }

    const UseT& Get(const TypeId theId) const
    {
      return Uses.Value(static_cast<size_t>(theId.Index));
    }

    UseT& Change(const TypeId theId) { return Uses.ChangeValue(static_cast<size_t>(theId.Index)); }

    TypeId Append()
    {
      const TypeId anId(static_cast<uint32_t>(Uses.Size()));
      ++NbActive;
      Uses.Appended();
      RemovedFlags.Resize(static_cast<size_t>(anId.Index) + 1);
      return anId;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0u, "RepStore::DecrementActive: underflow");
      if (NbActive > 0u)
      {
        --NbActive;
      }
    }

    bool MarkRemoved(const TypeId theId)
    {
      if (!theId.IsValid(Nb()))
      {
        return false;
      }
      if (RemovedFlags.Test(theId.Index))
      {
        return false;
      }
      RemovedFlags.Set(theId.Index);
      DecrementActive();
      return true;
    }

    bool IsRemoved(const TypeId theId) const
    {
      return theId.IsValid(Nb()) && RemovedFlags.Test(theId.Index);
    }

    void Clear(const bool theReleaseMemory = false)
    {
      Uses.Clear(theReleaseMemory);
      RemovedFlags.ClearAll();
      NbActive = 0;
    }
  };

  //! Edge 3D curve use store.
  RepStore<BRepGraphInc::EdgeCurve3DRep> myEdgeCurves3D;

  //! Edge 3D polygon use store.
  RepStore<BRepGraphInc::EdgePolygon3DRep> myEdgePolygons3D;

  //! CoEdge 2D curve use store.
  RepStore<BRepGraphInc::CoEdgeCurve2DRep> myCoEdgeCurves2D;

  //! CoEdge 2D polygon use store.
  RepStore<BRepGraphInc::CoEdgePolygon2DRep> myCoEdgePolygons2D;

  //! CoEdge polygon-on-triangulation use store.
  RepStore<BRepGraphInc::CoEdgePolygonOnTriRep> myCoEdgePolygonsOnTri;

  //! Face surface use store.
  RepStore<BRepGraphInc::FaceSurfaceRep> myFaceSurfaces;

  //! Face triangulation use store.
  RepStore<BRepGraphInc::FaceTriangulationRep> myFaceTriangulations;

  //! UID reverse indexes: eagerly maintained on allocate/remove, rebuilt on compact/load.
  mutable NCollection_FlatDataMap<BRepGraph_UID, BRepGraph_NodeId>   myUIDToNodeId;
  mutable std::shared_mutex                                          myUIDToNodeIdMutex;
  mutable NCollection_FlatDataMap<BRepGraph_RefUID, BRepGraph_RefId> myRefUIDToRefId;
  mutable std::shared_mutex                                          myRefUIDToRefIdMutex;
  mutable std::atomic<bool>                                          myUIDToNodeIdDirty{false};
  mutable std::atomic<bool>                                          myRefUIDToRefIdDirty{false};

  //! Bindings from reconstructed / source OCCT shapes back to backend ids.
  NCollection_FlatDataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> myShapeToNodeId;
  NCollection_FlatDataMap<BRepGraph_NodeId, TopoDS_Shape>                          myOriginalShapes;
  mutable std::shared_mutex myShapeBindingsMutex;

  //! Persistent backend identity state.
  std::atomic<uint32_t> myGeneration{0};
  Standard_GUID         myGraphGUID;

  //! Transient mutation-control state used by EditorView invalidation paths.
  std::atomic<bool>     myDeferredMode{false};
  std::atomic<uint32_t> myPropagationWave{0};
  uint32_t              myRemoveSubgraphDepth = 0;

  //! Transient generation-validated shape reconstruction cache.
  mutable NCollection_FlatDataMap<BRepGraph_NodeId, CachedShape> myCurrentShapes;
  mutable std::shared_mutex                                      myCurrentShapesMutex;

private:
  //! Trait mapping a typed identifier to its store's bit-flag planes.
  //! One specialization per typed ID type provides O(1) compile-time dispatch.
  template <typename T>
  struct TypedStorePlanes;

  template <typename T>
  friend struct TypedStorePlanes;

  template <typename T>
  [[nodiscard]] bool isInRange(const T theId) const;

  template <typename FuncT>
  [[nodiscard]] bool dispatchItemId(const BRepGraph_ItemId& theId, FuncT&& theFunc) const;

public:
  //! Return true if the entity identified by the given typed ID is soft-removed.
  //! @param[in] theId typed entity identifier
  template <typename T>
  [[nodiscard]] bool IsRemoved(const T theId) const;

  //! Set or clear the soft-removal flag for the entity identified by the given typed ID.
  //! @param[in] theId typed entity identifier
  //! @param[in] theVal true to mark removed, false to mark active
  template <typename T>
  void SetRemoved(const T theId, const bool theVal);

  //! Return true if the entity identified by the given typed ID has a registered owner.
  //! @param[in] theId typed entity identifier
  template <typename T>
  [[nodiscard]] bool IsOwned(const T theId) const;

  //! Set or clear the ownership flag for the entity identified by the given typed ID.
  //! @param[in] theId typed entity identifier
  //! @param[in] theVal true to mark owned, false to mark unowned
  template <typename T>
  void SetOwned(const T theId, const bool theVal);

  //! Return true if the entity identified by the given typed ID has an active MutGuard.
  //! @param[in] theId typed entity identifier
  template <typename T>
  [[nodiscard]] bool IsGuarded(const T theId) const;

  //! Register an active MutGuard on the entity identified by the given typed ID.
  //! @param[in] theId typed entity identifier
  template <typename T>
  void SetGuarded(const T theId);

  //! Deregister an active MutGuard from the entity identified by the given typed ID.
  //! @param[in] theId typed entity identifier
  template <typename T>
  void ClearGuarded(const T theId);

  //! Return true if the node identified by the given generic NodeId has a parent compound.
  //! Dispatches by node kind to the appropriate per-kind bitset.
  //! @param[in] theNode generic node identifier
  [[nodiscard]] Standard_EXPORT bool HasCompoundParent(const BRepGraph_NodeId theNode) const;

  //! Return true if the node identified by the given generic NodeId has a parent occurrence.
  //! Dispatches by node kind to the appropriate per-kind bitset.
  //! @param[in] theNode generic node identifier
  [[nodiscard]] Standard_EXPORT bool HasOccurrenceParent(const BRepGraph_NodeId theNode) const;

  //! Return true if the entity identified by the given generic item id has an active MutGuard.
  //! @param[in] theId generic item identifier (node, reference, or representation)
  [[nodiscard]] bool IsGuarded(const BRepGraph_ItemId& theId) const;

  //! Register an active MutGuard on the entity identified by the given generic item id.
  //! @param[in] theId generic item identifier (node, reference, or representation)
  void SetGuarded(const BRepGraph_ItemId& theId);

  //! Deregister an active MutGuard from the entity identified by the given generic item id.
  //! @param[in] theId generic item identifier (node, reference, or representation)
  void ClearGuarded(const BRepGraph_ItemId& theId);

  //! Return true if any entity in any store has an active MutGuard.
  //! Used to assert no guards are active before Clear().
  [[nodiscard]] Standard_EXPORT bool HasAnyGuard() const;
};

#include <BRepGraphInc_Storage.lxx>

#endif // _BRepGraphInc_Storage_HeaderFile
