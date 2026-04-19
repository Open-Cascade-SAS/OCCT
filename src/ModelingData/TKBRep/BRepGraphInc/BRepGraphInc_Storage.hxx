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

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RepId.hxx>
#include <BRepGraph_RefUID.hxx>
#include <BRepGraph_UID.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>

//! @brief Central backend storage container for the incidence-table topology model.
//!
//! Holds all entity vectors (Vertex through Occurrence), representation
//! vectors (Surface, Curve3D, Curve2D, Triangulation, Polygon), reverse
//! indices for O(1) upward navigation, TShape deduplication maps, original
//! shape bindings, and per-kind UID vectors. Provides typed accessors
//! enforcing compile-time safety for backend code. External callers should
//! normally use the BRepGraph facade rather than reaching into this storage
//! directly. BRepGraphInc_Populate has friend access for efficient bulk writes
//! during graph population.
class BRepGraph_Builder;

class BRepGraphInc_Storage
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct with allocator for internal collections.
  //! If null, uses CommonBaseAllocator.
  Standard_EXPORT explicit BRepGraphInc_Storage(
    const occ::handle<NCollection_BaseAllocator>& theAlloc =
      occ::handle<NCollection_BaseAllocator>());

  //! Return the allocator used for internal collections.
  [[nodiscard]] const occ::handle<NCollection_BaseAllocator>& Allocator() const
  {
    return myAllocator;
  }

  //! Returns the total number of vertex entities (including removed).
  [[nodiscard]] int NbVertices() const { return myVertices.Nb(); }

  //! Returns the total number of edge entities (including removed).
  [[nodiscard]] int NbEdges() const { return myEdges.Nb(); }

  //! Returns the total number of coedge entities (including removed).
  [[nodiscard]] int NbCoEdges() const { return myCoEdges.Nb(); }

  //! Returns the total number of wire entities (including removed).
  [[nodiscard]] int NbWires() const { return myWires.Nb(); }

  //! Returns the total number of face entities (including removed).
  [[nodiscard]] int NbFaces() const { return myFaces.Nb(); }

  //! Returns the total number of shell entities (including removed).
  [[nodiscard]] int NbShells() const { return myShells.Nb(); }

  //! Returns the total number of solid entities (including removed).
  [[nodiscard]] int NbSolids() const { return mySolids.Nb(); }

  //! Returns the total number of compound entities (including removed).
  [[nodiscard]] int NbCompounds() const { return myCompounds.Nb(); }

  //! Returns the total number of compsolid entities (including removed).
  [[nodiscard]] int NbCompSolids() const { return myCompSolids.Nb(); }

  //! Returns the total number of product entities (including removed).
  [[nodiscard]] int NbProducts() const { return myProducts.Nb(); }

  //! Returns the total number of occurrence entities (including removed).
  [[nodiscard]] int NbOccurrences() const { return myOccurrences.Nb(); }

  //! Returns the total number of shell reference entries (including removed).
  [[nodiscard]] int NbShellRefs() const { return myShellRefs.Nb(); }

  //! Returns the total number of face reference entries (including removed).
  [[nodiscard]] int NbFaceRefs() const { return myFaceRefs.Nb(); }

  //! Returns the total number of wire reference entries (including removed).
  [[nodiscard]] int NbWireRefs() const { return myWireRefs.Nb(); }

  //! Returns the total number of coedge reference entries (including removed).
  [[nodiscard]] int NbCoEdgeRefs() const { return myCoEdgeRefs.Nb(); }

  //! Returns the total number of vertex reference entries (including removed).
  [[nodiscard]] int NbVertexRefs() const { return myVertexRefs.Nb(); }

  //! Returns the total number of solid reference entries (including removed).
  [[nodiscard]] int NbSolidRefs() const { return mySolidRefs.Nb(); }

  //! Returns the total number of child reference entries (including removed).
  [[nodiscard]] int NbChildRefs() const { return myChildRefs.Nb(); }

  //! Returns the total number of occurrence reference entries (including removed).
  [[nodiscard]] int NbOccurrenceRefs() const { return myOccurrenceRefs.Nb(); }

  //! Returns the total number of surface representations.
  [[nodiscard]] int NbSurfaces() const { return mySurfaces.Nb(); }

  //! Returns the total number of 3D curve representations.
  [[nodiscard]] int NbCurves3D() const { return myCurves3D.Nb(); }

  //! Returns the total number of 2D curve representations.
  [[nodiscard]] int NbCurves2D() const { return myCurves2D.Nb(); }

  //! Returns the total number of triangulation representations.
  [[nodiscard]] int NbTriangulations() const { return myTriangulationsRep.Nb(); }

  //! Returns the total number of 3D polygon representations.
  [[nodiscard]] int NbPolygons3D() const { return myPolygons3D.Nb(); }

  //! Returns the total number of 2D polygon representations.
  [[nodiscard]] int NbPolygons2D() const { return myPolygons2D.Nb(); }

  //! Returns the total number of polygon-on-triangulation representations.
  [[nodiscard]] int NbPolygonsOnTri() const { return myPolygonsOnTri.Nb(); }

  //! Returns the number of active surface representations (excluding removed).
  [[nodiscard]] int NbActiveSurfaces() const { return mySurfaces.NbActive; }

  //! Returns the number of active 3D curve representations (excluding removed).
  [[nodiscard]] int NbActiveCurves3D() const { return myCurves3D.NbActive; }

  //! Returns the number of active 2D curve representations (excluding removed).
  [[nodiscard]] int NbActiveCurves2D() const { return myCurves2D.NbActive; }

  //! Returns the number of active triangulation representations (excluding removed).
  [[nodiscard]] int NbActiveTriangulations() const { return myTriangulationsRep.NbActive; }

  //! Returns the number of active 3D polygon representations (excluding removed).
  [[nodiscard]] int NbActivePolygons3D() const { return myPolygons3D.NbActive; }

  //! Returns the number of active 2D polygon representations (excluding removed).
  [[nodiscard]] int NbActivePolygons2D() const { return myPolygons2D.NbActive; }

  //! Returns the number of active polygon-on-triangulation representations (excluding removed).
  [[nodiscard]] int NbActivePolygonsOnTri() const { return myPolygonsOnTri.NbActive; }

  //! Returns the number of active vertex entities (excluding removed).
  [[nodiscard]] int NbActiveVertices() const { return myVertices.NbActive; }

  //! Returns the number of active edge entities (excluding removed).
  [[nodiscard]] int NbActiveEdges() const { return myEdges.NbActive; }

  //! Returns the number of active coedge entities (excluding removed).
  [[nodiscard]] int NbActiveCoEdges() const { return myCoEdges.NbActive; }

  //! Returns the number of active wire entities (excluding removed).
  [[nodiscard]] int NbActiveWires() const { return myWires.NbActive; }

  //! Returns the number of active face entities (excluding removed).
  [[nodiscard]] int NbActiveFaces() const { return myFaces.NbActive; }

  //! Returns the number of active shell entities (excluding removed).
  [[nodiscard]] int NbActiveShells() const { return myShells.NbActive; }

  //! Returns the number of active solid entities (excluding removed).
  [[nodiscard]] int NbActiveSolids() const { return mySolids.NbActive; }

  //! Returns the number of active compound entities (excluding removed).
  [[nodiscard]] int NbActiveCompounds() const { return myCompounds.NbActive; }

  //! Returns the number of active compsolid entities (excluding removed).
  [[nodiscard]] int NbActiveCompSolids() const { return myCompSolids.NbActive; }

  //! Returns the number of active product entities (excluding removed).
  [[nodiscard]] int NbActiveProducts() const { return myProducts.NbActive; }

  //! Returns the number of active occurrence entities (excluding removed).
  [[nodiscard]] int NbActiveOccurrences() const { return myOccurrences.NbActive; }

  //! Returns the number of active shell reference entries (excluding removed).
  [[nodiscard]] int NbActiveShellRefs() const { return myShellRefs.NbActive; }

  //! Returns the number of active face reference entries (excluding removed).
  [[nodiscard]] int NbActiveFaceRefs() const { return myFaceRefs.NbActive; }

  //! Returns the number of active wire reference entries (excluding removed).
  [[nodiscard]] int NbActiveWireRefs() const { return myWireRefs.NbActive; }

  //! Returns the number of active coedge reference entries (excluding removed).
  [[nodiscard]] int NbActiveCoEdgeRefs() const { return myCoEdgeRefs.NbActive; }

  //! Returns the number of active vertex reference entries (excluding removed).
  [[nodiscard]] int NbActiveVertexRefs() const { return myVertexRefs.NbActive; }

  //! Returns the number of active solid reference entries (excluding removed).
  [[nodiscard]] int NbActiveSolidRefs() const { return mySolidRefs.NbActive; }

  //! Returns the number of active child reference entries (excluding removed).
  [[nodiscard]] int NbActiveChildRefs() const { return myChildRefs.NbActive; }

  //! Returns the number of active occurrence reference entries (excluding removed).
  [[nodiscard]] int NbActiveOccurrenceRefs() const { return myOccurrenceRefs.NbActive; }

  //! Mark an entity node as removed and decrement its active counter once.
  //! @param[in] theNodeId typed entity id
  //! @return true if the node transitioned from active to removed
  Standard_EXPORT bool MarkRemoved(const BRepGraph_NodeId theNodeId);

  //! Mark a reference entry as removed and decrement its active counter once.
  //! @param[in] theRefId typed reference id
  //! @return true if the ref transitioned from active to removed
  Standard_EXPORT bool MarkRemovedRef(const BRepGraph_RefId theRefId);

  //! Mark a representation entry as removed and decrement its active counter once.
  //! @param[in] theRepId typed representation id
  //! @return true if the representation transitioned from active to removed
  Standard_EXPORT bool MarkRemovedRep(const BRepGraph_RepId theRepId);

  //! Returns the surface representation at the given typed id.
  //! @param[in] theRep typed surface representation id
  [[nodiscard]] const BRepGraphInc::SurfaceRep& SurfaceRep(
    const BRepGraph_SurfaceRepId theRep) const
  {
    return mySurfaces.Get(theRep);
  }

  //! Returns the 3D curve representation at the given typed id.
  //! @param[in] theRep typed curve-3D representation id
  [[nodiscard]] const BRepGraphInc::Curve3DRep& Curve3DRep(
    const BRepGraph_Curve3DRepId theRep) const
  {
    return myCurves3D.Get(theRep);
  }

  //! Returns the 2D curve representation at the given typed id.
  //! @param[in] theRep typed curve-2D representation id
  [[nodiscard]] const BRepGraphInc::Curve2DRep& Curve2DRep(
    const BRepGraph_Curve2DRepId theRep) const
  {
    return myCurves2D.Get(theRep);
  }

  //! Returns the triangulation representation at the given typed id.
  //! @param[in] theRep typed triangulation representation id
  [[nodiscard]] const BRepGraphInc::TriangulationRep& TriangulationRep(
    const BRepGraph_TriangulationRepId theRep) const
  {
    return myTriangulationsRep.Get(theRep);
  }

  //! Returns the 3D polygon representation at the given typed id.
  //! @param[in] theRep typed polygon-3D representation id
  [[nodiscard]] const BRepGraphInc::Polygon3DRep& Polygon3DRep(
    const BRepGraph_Polygon3DRepId theRep) const
  {
    return myPolygons3D.Get(theRep);
  }

  //! Returns the 2D polygon representation at the given typed id.
  //! @param[in] theRep typed polygon-2D representation id
  [[nodiscard]] const BRepGraphInc::Polygon2DRep& Polygon2DRep(
    const BRepGraph_Polygon2DRepId theRep) const
  {
    return myPolygons2D.Get(theRep);
  }

  //! Returns the polygon-on-triangulation representation at the given typed id.
  //! @param[in] theRep typed polygon-on-triangulation representation id
  [[nodiscard]] const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(
    const BRepGraph_PolygonOnTriRepId theRep) const
  {
    return myPolygonsOnTri.Get(theRep);
  }

  //! Returns a mutable reference to the surface representation at the given typed id.
  //! @param[in] theRep typed surface representation id
  BRepGraphInc::SurfaceRep& ChangeSurfaceRep(const BRepGraph_SurfaceRepId theRep)
  {
    return mySurfaces.Change(theRep);
  }

  //! Returns a mutable reference to the 3D curve representation at the given typed id.
  //! @param[in] theRep typed curve-3D representation id
  BRepGraphInc::Curve3DRep& ChangeCurve3DRep(const BRepGraph_Curve3DRepId theRep)
  {
    return myCurves3D.Change(theRep);
  }

  //! Returns a mutable reference to the 2D curve representation at the given typed id.
  //! @param[in] theRep typed curve-2D representation id
  BRepGraphInc::Curve2DRep& ChangeCurve2DRep(const BRepGraph_Curve2DRepId theRep)
  {
    return myCurves2D.Change(theRep);
  }

  //! Returns a mutable reference to the triangulation representation at the given typed id.
  //! @param[in] theRep typed triangulation representation id
  BRepGraphInc::TriangulationRep& ChangeTriangulationRep(const BRepGraph_TriangulationRepId theRep)
  {
    return myTriangulationsRep.Change(theRep);
  }

  //! Returns a mutable reference to the 3D polygon representation at the given typed id.
  //! @param[in] theRep typed polygon-3D representation id
  BRepGraphInc::Polygon3DRep& ChangePolygon3DRep(const BRepGraph_Polygon3DRepId theRep)
  {
    return myPolygons3D.Change(theRep);
  }

  //! Returns a mutable reference to the 2D polygon representation at the given typed id.
  //! @param[in] theRep typed polygon-2D representation id
  BRepGraphInc::Polygon2DRep& ChangePolygon2DRep(const BRepGraph_Polygon2DRepId theRep)
  {
    return myPolygons2D.Change(theRep);
  }

  //! Returns a mutable reference to the polygon-on-triangulation representation at the given typed
  //! id.
  //! @param[in] theRep typed polygon-on-triangulation representation id
  BRepGraphInc::PolygonOnTriRep& ChangePolygonOnTriRep(const BRepGraph_PolygonOnTriRepId theRep)
  {
    return myPolygonsOnTri.Change(theRep);
  }

  //! Appends a new surface representation slot and returns its typed id.
  BRepGraph_SurfaceRepId AppendSurfaceRep() { return mySurfaces.Append(); }

  //! Appends a new 3D curve representation slot and returns its typed id.
  BRepGraph_Curve3DRepId AppendCurve3DRep() { return myCurves3D.Append(); }

  //! Appends a new 2D curve representation slot and returns its typed id.
  BRepGraph_Curve2DRepId AppendCurve2DRep() { return myCurves2D.Append(); }

  //! Appends a new triangulation representation slot and returns its typed id.
  BRepGraph_TriangulationRepId AppendTriangulationRep()
  {
    return myTriangulationsRep.Append();
  }

  //! Appends a new 3D polygon representation slot and returns its typed id.
  BRepGraph_Polygon3DRepId AppendPolygon3DRep()
  {
    return myPolygons3D.Append();
  }

  //! Appends a new 2D polygon representation slot and returns its typed id.
  BRepGraph_Polygon2DRepId AppendPolygon2DRep()
  {
    return myPolygons2D.Append();
  }

  //! Appends a new polygon-on-triangulation representation slot and returns its typed id.
  BRepGraph_PolygonOnTriRepId AppendPolygonOnTriRep()
  {
    return myPolygonsOnTri.Append();
  }

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

  //! Returns the coedge reference entry at the given typed id.
  [[nodiscard]] const BRepGraphInc::CoEdgeRef& CoEdgeRef(const BRepGraph_CoEdgeRefId theRefId) const
  {
    return myCoEdgeRefs.Get(theRefId);
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

  //! Returns a mutable reference to the coedge reference entry at the given typed id.
  BRepGraphInc::CoEdgeRef& ChangeCoEdgeRef(const BRepGraph_CoEdgeRefId theRefId)
  {
    return myCoEdgeRefs.Change(theRefId);
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

  //! Appends a new vertex entity and returns its typed id.
  BRepGraph_VertexId AppendVertex() { return myVertices.Append(myAllocator); }

  //! Appends a new edge entity and returns its typed id.
  BRepGraph_EdgeId AppendEdge() { return myEdges.Append(myAllocator); }

  //! Appends a new coedge entity and returns its typed id.
  BRepGraph_CoEdgeId AppendCoEdge() { return myCoEdges.Append(myAllocator); }

  //! Appends a new wire entity and returns its typed id.
  BRepGraph_WireId AppendWire() { return myWires.Append(myAllocator); }

  //! Appends a new face entity and returns its typed id.
  BRepGraph_FaceId AppendFace() { return myFaces.Append(myAllocator); }

  //! Appends a new shell entity and returns its typed id.
  BRepGraph_ShellId AppendShell() { return myShells.Append(myAllocator); }

  //! Appends a new solid entity and returns its typed id.
  BRepGraph_SolidId AppendSolid() { return mySolids.Append(myAllocator); }

  //! Appends a new compound entity and returns its typed id.
  BRepGraph_CompoundId AppendCompound()
  {
    return myCompounds.Append(myAllocator);
  }

  //! Appends a new compsolid entity and returns its typed id.
  BRepGraph_CompSolidId AppendCompSolid()
  {
    return myCompSolids.Append(myAllocator);
  }

  //! Appends a new product entity and returns its typed id.
  BRepGraph_ProductId AppendProduct()
  {
    return myProducts.Append(myAllocator);
  }

  //! Appends a new occurrence entity and returns its typed id.
  BRepGraph_OccurrenceId AppendOccurrence()
  {
    return myOccurrences.Append(myAllocator);
  }

  //! Appends a new shell reference entry and returns its typed id.
  BRepGraph_ShellRefId AppendShellRef() { return myShellRefs.Append(); }

  //! Appends a new face reference entry and returns its typed id.
  BRepGraph_FaceRefId AppendFaceRef() { return myFaceRefs.Append(); }

  //! Appends a new wire reference entry and returns its typed id.
  BRepGraph_WireRefId AppendWireRef() { return myWireRefs.Append(); }

  //! Appends a new coedge reference entry and returns its typed id.
  BRepGraph_CoEdgeRefId AppendCoEdgeRef() { return myCoEdgeRefs.Append(); }

  //! Appends a new vertex reference entry and returns its typed id.
  BRepGraph_VertexRefId AppendVertexRef() { return myVertexRefs.Append(); }

  //! Appends a new solid reference entry and returns its typed id.
  BRepGraph_SolidRefId AppendSolidRef() { return mySolidRefs.Append(); }

  //! Appends a new child reference entry and returns its typed id.
  BRepGraph_ChildRefId AppendChildRef() { return myChildRefs.Append(); }

  //! Appends a new occurrence reference entry and returns its typed id.
  BRepGraph_OccurrenceRefId AppendOccurrenceRef()
  {
    return myOccurrenceRefs.Append();
  }

  //! Return the per-kind UID vector for a given Kind.
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_UID>& UIDs(
    const BRepGraph_NodeId::Kind theKind) const;

  //! Return the per-kind UID vector for a given Kind (mutable).
  Standard_EXPORT NCollection_Vector<BRepGraph_UID>& ChangeUIDs(
    const BRepGraph_NodeId::Kind theKind);

  //! Clear all UID vectors (reset lengths to 0).
  Standard_EXPORT void ResetAllUIDs();

  //! Return the BaseRef portion of any ref entry by generic RefId.
  //! @param[in] theRefId generic reference identifier
  //! @return const reference to the BaseRef base of the ref entry
  [[nodiscard]] Standard_EXPORT const BRepGraphInc::BaseRef& BaseRef(
    const BRepGraph_RefId theRefId) const;

  //! Return the mutable BaseRef portion of any ref entry by generic RefId.
  //! @param[in] theRefId generic reference identifier
  //! @return mutable reference to the BaseRef base of the ref entry
  Standard_EXPORT BRepGraphInc::BaseRef& ChangeBaseRef(const BRepGraph_RefId theRefId);

  //! Return the per-kind transitional reference UID vector.
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_RefUID>& RefUIDs(
    const BRepGraph_RefId::Kind theKind) const;

  //! Return the per-kind transitional reference UID vector (mutable).
  Standard_EXPORT NCollection_Vector<BRepGraph_RefUID>& ChangeRefUIDs(
    const BRepGraph_RefId::Kind theKind);

  //! Clear all transitional reference UID vectors.
  Standard_EXPORT void ResetAllRefUIDs();

  //! Returns the reverse index for parent-child relationship queries.
  [[nodiscard]] const BRepGraphInc_ReverseIndex& ReverseIndex() const { return myReverseIdx; }

  //! Returns a mutable reference to the reverse index.
  BRepGraphInc_ReverseIndex& ChangeReverseIndex() { return myReverseIdx; }

  //! Returns the node id bound to the given TShape, or nullptr if not bound.
  [[nodiscard]] const BRepGraph_NodeId* FindNodeByTShape(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.Seek(theTShape);
  }

  //! Returns true if the given TShape is bound to a node.
  [[nodiscard]] bool HasTShapeBinding(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.IsBound(theTShape);
  }

  //! Binds the given TShape to a node id.
  void BindTShapeToNode(const TopoDS_TShape* theTShape, const BRepGraph_NodeId theNodeId)
  {
    myTShapeToNodeId.Bind(theTShape, theNodeId);
  }

  //! Back-reference to the source `TopoDS_Shape` a node was built from.
  //! Only populated during Build; absent bindings are a valid state.

  //! Returns the original shape for the given node id, or nullptr if not bound.
  [[nodiscard]] const TopoDS_Shape* FindOriginal(const BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.Seek(theNodeId);
  }

  //! Returns true if the given node id has an original shape binding.
  [[nodiscard]] bool HasOriginal(const BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.IsBound(theNodeId);
  }

  //! Binds the given node id to its original shape.
  void BindOriginal(const BRepGraph_NodeId theNodeId, const TopoDS_Shape& theShape)
  {
    myOriginalShapes.Bind(theNodeId, theShape);
  }

  //! Removes the original shape binding for the given node id.
  void UnBindOriginal(const BRepGraph_NodeId theNodeId) { myOriginalShapes.UnBind(theNodeId); }

  //! Iterate all TShape-to-NodeId bindings, invoking theFunc(TShape*, NodeId) for each entry.
  //! Used by Compact to rebuild the map after the rebuild-and-swap.
  template <typename FuncT>
  void ForEachTShapeBinding(FuncT&& theFunc) const
  {
    for (NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId>::Iterator anIt(
           myTShapeToNodeId);
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
    for (NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>::Iterator anIt(myOriginalShapes);
         anIt.More();
         anIt.Next())
    {
      theFunc(anIt.Key(), anIt.Value());
    }
  }

  [[nodiscard]] bool GetIsDone() const { return myIsDone; }

  void SetIsDone(const bool theVal) { myIsDone = theVal; }

  //! Clear all storage.
  Standard_EXPORT void Clear();

  //! Build reverse indices from entity and relationship tables.
  //! Call after population is complete.
  Standard_EXPORT void BuildReverseIndex();

  //! Incrementally update reverse indices for entities appended after a previous
  //! BuildReverseIndex(). Only processes entities and refs from the old counts to the
  //! current vector lengths — the caller must snapshot ChildRef / SolidRef counts before
  //! any Append so this remains O(delta), not O(total).
  Standard_EXPORT void BuildDeltaReverseIndex(const int theOldNbEdges,
                                              const int theOldNbWires,
                                              const int theOldNbFaces,
                                              const int theOldNbShells,
                                              const int theOldNbSolids,
                                              const int theOldNbCompounds,
                                              const int theOldNbCompSolids,
                                              const int theOldNbChildRefs,
                                              const int theOldNbSolidRefs);

  //! Debug: verify reverse index consistency against entity tables.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool ValidateReverseIndex() const;

private:
  friend class BRepGraphInc_Populate;
  friend class BRepGraph;
  friend class BRepGraphInc_ReverseIndex;

  //! @brief Template store for topology entity kinds.
  //! Groups the entity vector, per-kind UID vector, and active count
  //! into a single struct, eliminating repeated boilerplate.
  template <typename EntityT>
  struct DefStore
  {
    using TypeId = typename EntityT::TypeId;
    using ValueType = EntityT;

    NCollection_Vector<EntityT>       Entities;
    NCollection_Vector<BRepGraph_UID> UIDs;
    int                               NbActive = 0;

    DefStore() = default;

    DefStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Entities(theBlockSize, theAlloc),
          UIDs(theBlockSize, theAlloc)
    {
    }

    int Nb() const { return Entities.Length(); }

    const EntityT& Get(const TypeId theId) const { return Entities.Value(theId.Index); }

    EntityT& Change(const TypeId theId) { return Entities.ChangeValue(theId.Index); }

    //! Append a default-constructed entity and return its typed slot id.
    TypeId Append(const occ::handle<NCollection_BaseAllocator>& theAlloc)
    {
      const TypeId anId(Entities.Length());
      ++NbActive;
      Entities.Appended().InitVectors(theAlloc);
      return anId;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0, "DefStore::DecrementActive: underflow");
      if (NbActive > 0)
        --NbActive;
    }

    bool MarkRemoved(const TypeId theId)
    {
      if (!theId.IsValid(Entities.Length()))
      {
        return false;
      }

      EntityT& anEntity = Change(theId);
      if (anEntity.IsRemoved)
      {
        return false;
      }
      anEntity.IsRemoved = true;
      DecrementActive();
      return true;
    }

    void Clear()
    {
      Entities.Clear();
      UIDs.Clear();
      NbActive = 0;
    }
  };

  //! @brief Template store for representation entity kinds.
  //! Groups the representation vector and active count into a single struct.
  template <typename RepT>
  struct RepStore
  {
    using TypeId = typename RepT::TypeId;
    using ValueType = RepT;

    NCollection_Vector<RepT> Entities;
    int                      NbActive = 0;

    RepStore() = default;

    RepStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Entities(theBlockSize, theAlloc)
    {
    }

    int Nb() const { return Entities.Length(); }

    const RepT& Get(const TypeId theId) const { return Entities.Value(theId.Index); }

    RepT& Change(const TypeId theId) { return Entities.ChangeValue(theId.Index); }

    //! Append a default-constructed rep and return its typed slot id.
    TypeId Append()
    {
      const TypeId anId(Entities.Length());
      ++NbActive;
      Entities.Appended();
      return anId;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0, "RepStore::DecrementActive: underflow");
      if (NbActive > 0)
        --NbActive;
    }

    bool MarkRemoved(const TypeId theId)
    {
      if (!theId.IsValid(Entities.Length()))
      {
        return false;
      }

      RepT& aRep = Change(theId);
      if (aRep.IsRemoved)
      {
        return false;
      }
      aRep.IsRemoved = true;
      DecrementActive();
      return true;
    }

    void EraseLast()
    {
      Standard_ASSERT_VOID(NbActive > 0, "RepStore::EraseLast: underflow");
      if (NbActive > 0)
      {
        Entities.EraseLast();
        --NbActive;
      }
    }

    void Clear()
    {
      Entities.Clear();
      NbActive = 0;
    }
  };

  //! @brief Template store for transitional reference entry kinds.
  //! Groups reference vectors and per-kind UID vectors into a single struct.
  template <typename RefT>
  struct RefStore
  {
    using TypeId = typename RefT::TypeId;
    using ValueType = RefT;

    NCollection_Vector<RefT>             Refs;
    NCollection_Vector<BRepGraph_RefUID> UIDs;
    int                                  NbActive = 0;

    RefStore() = default;

    RefStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Refs(theBlockSize, theAlloc),
          UIDs(theBlockSize, theAlloc)
    {
    }

    int Nb() const { return Refs.Length(); }

    const RefT& Get(const TypeId theId) const { return Refs.Value(theId.Index); }

    RefT& Change(const TypeId theId) { return Refs.ChangeValue(theId.Index); }

    //! Append a default-constructed ref entry and return its typed slot id.
    TypeId Append()
    {
      const TypeId anId(Refs.Length());
      ++NbActive;
      Refs.Appended();
      return anId;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0, "RefStore::DecrementActive: underflow");
      if (NbActive > 0)
        --NbActive;
    }

    bool MarkRemoved(const TypeId theId)
    {
      if (!theId.IsValid(Refs.Length()))
      {
        return false;
      }

      RefT& aRef = Change(theId);
      if (aRef.IsRemoved)
      {
        return false;
      }
      aRef.IsRemoved = true;
      DecrementActive();
      return true;
    }

    void Clear()
    {
      Refs.Clear();
      UIDs.Clear();
      NbActive = 0;
    }
  };

  // Topology entity stores
  DefStore<BRepGraphInc::VertexDef>     myVertices;
  DefStore<BRepGraphInc::EdgeDef>       myEdges;
  DefStore<BRepGraphInc::CoEdgeDef>     myCoEdges;
  DefStore<BRepGraphInc::WireDef>       myWires;
  DefStore<BRepGraphInc::FaceDef>       myFaces;
  DefStore<BRepGraphInc::ShellDef>      myShells;
  DefStore<BRepGraphInc::SolidDef>      mySolids;
  DefStore<BRepGraphInc::CompoundDef>   myCompounds;
  DefStore<BRepGraphInc::CompSolidDef>  myCompSolids;
  DefStore<BRepGraphInc::ProductDef>    myProducts;
  DefStore<BRepGraphInc::OccurrenceDef> myOccurrences;

  // Transitional reference entry stores
  RefStore<BRepGraphInc::ShellRef>      myShellRefs;
  RefStore<BRepGraphInc::FaceRef>       myFaceRefs;
  RefStore<BRepGraphInc::WireRef>       myWireRefs;
  RefStore<BRepGraphInc::CoEdgeRef>     myCoEdgeRefs;
  RefStore<BRepGraphInc::VertexRef>     myVertexRefs;
  RefStore<BRepGraphInc::SolidRef>      mySolidRefs;
  RefStore<BRepGraphInc::ChildRef>      myChildRefs;
  RefStore<BRepGraphInc::OccurrenceRef> myOccurrenceRefs;

  // Representation entity stores
  RepStore<BRepGraphInc::SurfaceRep>       mySurfaces;
  RepStore<BRepGraphInc::Curve3DRep>       myCurves3D;
  RepStore<BRepGraphInc::Curve2DRep>       myCurves2D;
  RepStore<BRepGraphInc::TriangulationRep> myTriangulationsRep;
  RepStore<BRepGraphInc::Polygon3DRep>     myPolygons3D;
  RepStore<BRepGraphInc::Polygon2DRep>     myPolygons2D;
  RepStore<BRepGraphInc::PolygonOnTriRep>  myPolygonsOnTri;

  BRepGraphInc_ReverseIndex myReverseIdx;

  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> myTShapeToNodeId;
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>         myOriginalShapes;

  occ::handle<NCollection_BaseAllocator> myAllocator;

  bool myIsDone = false;
};

#endif // _BRepGraphInc_Storage_HeaderFile
