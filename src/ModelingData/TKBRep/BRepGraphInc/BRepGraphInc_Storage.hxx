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
#include <BRepGraph_UID.hxx>
#include <BRepGraphInc_Entity.hxx>
#include <BRepGraphInc_ReverseIndex.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>

//! @brief Central storage container for the incidence-table topology model.
//!
//! Holds all entity vectors (Vertex through Occurrence), representation
//! vectors (Surface, Curve3D, Curve2D, Triangulation, Polygon), reverse
//! indices for O(1) upward navigation, TShape deduplication maps, original
//! shape bindings, and per-kind UID vectors. Provides typed accessors
//! enforcing compile-time safety. BRepGraphInc_Populate has friend access
//! for efficient bulk writes during graph population.
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
  [[nodiscard]] const occ::handle<NCollection_BaseAllocator>& Allocator() const { return myAllocator; }

  //! @name Count accessors (total including removed)

  [[nodiscard]] int NbVertices() const { return myVertices.Nb(); }

  [[nodiscard]] int NbEdges() const { return myEdges.Nb(); }

  [[nodiscard]] int NbCoEdges() const { return myCoEdges.Nb(); }

  [[nodiscard]] int NbWires() const { return myWires.Nb(); }

  [[nodiscard]] int NbFaces() const { return myFaces.Nb(); }

  [[nodiscard]] int NbShells() const { return myShells.Nb(); }

  [[nodiscard]] int NbSolids() const { return mySolids.Nb(); }

  [[nodiscard]] int NbCompounds() const { return myCompounds.Nb(); }

  [[nodiscard]] int NbCompSolids() const { return myCompSolids.Nb(); }

  [[nodiscard]] int NbProducts() const { return myProducts.Nb(); }

  [[nodiscard]] int NbOccurrences() const { return myOccurrences.Nb(); }

  //! @name Representation count accessors

  [[nodiscard]] int NbSurfaces() const { return mySurfaces.Nb(); }

  [[nodiscard]] int NbCurves3D() const { return myCurves3D.Nb(); }

  [[nodiscard]] int NbCurves2D() const { return myCurves2D.Nb(); }

  [[nodiscard]] int NbTriangulations() const { return myTriangulationsRep.Nb(); }

  [[nodiscard]] int NbPolygons3D() const { return myPolygons3D.Nb(); }

  [[nodiscard]] int NbPolygons2D() const { return myPolygons2D.Nb(); }

  [[nodiscard]] int NbPolygonsOnTri() const { return myPolygonsOnTri.Nb(); }

  //! @name Representation active count accessors

  [[nodiscard]] int NbActiveSurfaces() const { return mySurfaces.NbActive; }

  [[nodiscard]] int NbActiveCurves3D() const { return myCurves3D.NbActive; }

  [[nodiscard]] int NbActiveCurves2D() const { return myCurves2D.NbActive; }

  [[nodiscard]] int NbActiveTriangulations() const { return myTriangulationsRep.NbActive; }

  [[nodiscard]] int NbActivePolygons3D() const { return myPolygons3D.NbActive; }

  [[nodiscard]] int NbActivePolygons2D() const { return myPolygons2D.NbActive; }

  [[nodiscard]] int NbActivePolygonsOnTri() const { return myPolygonsOnTri.NbActive; }

  //! @name Active count accessors (excluding removed nodes)

  [[nodiscard]] int NbActiveVertices() const { return myVertices.NbActive; }

  [[nodiscard]] int NbActiveEdges() const { return myEdges.NbActive; }

  [[nodiscard]] int NbActiveCoEdges() const { return myCoEdges.NbActive; }

  [[nodiscard]] int NbActiveWires() const { return myWires.NbActive; }

  [[nodiscard]] int NbActiveFaces() const { return myFaces.NbActive; }

  [[nodiscard]] int NbActiveShells() const { return myShells.NbActive; }

  [[nodiscard]] int NbActiveSolids() const { return mySolids.NbActive; }

  [[nodiscard]] int NbActiveCompounds() const { return myCompounds.NbActive; }

  [[nodiscard]] int NbActiveCompSolids() const { return myCompSolids.NbActive; }

  [[nodiscard]] int NbActiveProducts() const { return myProducts.NbActive; }

  [[nodiscard]] int NbActiveOccurrences() const { return myOccurrences.NbActive; }

  //! Decrement the active count for the given node kind.
  void DecrementActiveCount(const BRepGraph_NodeId::Kind theKind);

  //! @name Const representation access
  //! Each method returns a const reference to the representation entity at the given typed id.

  //! @param[in] theRep typed surface representation id
  [[nodiscard]] const BRepGraphInc::SurfaceRep& SurfaceRep(const BRepGraph_SurfaceRepId theRep) const
  {
    return mySurfaces.Get(theRep.Index);
  }

  //! @param[in] theRep typed curve-3D representation id
  [[nodiscard]] const BRepGraphInc::Curve3DRep& Curve3DRep(const BRepGraph_Curve3DRepId theRep) const
  {
    return myCurves3D.Get(theRep.Index);
  }

  //! @param[in] theRep typed curve-2D representation id
  [[nodiscard]] const BRepGraphInc::Curve2DRep& Curve2DRep(const BRepGraph_Curve2DRepId theRep) const
  {
    return myCurves2D.Get(theRep.Index);
  }

  //! @param[in] theRep typed triangulation representation id
  [[nodiscard]] const BRepGraphInc::TriangulationRep& TriangulationRep(
    const BRepGraph_TriangulationRepId theRep) const
  {
    return myTriangulationsRep.Get(theRep.Index);
  }

  //! @param[in] theRep typed polygon-3D representation id
  [[nodiscard]] const BRepGraphInc::Polygon3DRep& Polygon3DRep(const BRepGraph_Polygon3DRepId theRep) const
  {
    return myPolygons3D.Get(theRep.Index);
  }

  //! @param[in] theRep typed polygon-2D representation id
  [[nodiscard]] const BRepGraphInc::Polygon2DRep& Polygon2DRep(const BRepGraph_Polygon2DRepId theRep) const
  {
    return myPolygons2D.Get(theRep.Index);
  }

  //! @param[in] theRep typed polygon-on-triangulation representation id
  [[nodiscard]] const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(
    const BRepGraph_PolygonOnTriRepId theRep) const
  {
    return myPolygonsOnTri.Get(theRep.Index);
  }

  //! @name Mutable representation access
  //! Each method returns a mutable reference to the representation entity at the given typed id.

  //! @param[in] theRep typed surface representation id
  BRepGraphInc::SurfaceRep& ChangeSurfaceRep(const BRepGraph_SurfaceRepId theRep)
  {
    return mySurfaces.Change(theRep.Index);
  }

  //! @param[in] theRep typed curve-3D representation id
  BRepGraphInc::Curve3DRep& ChangeCurve3DRep(const BRepGraph_Curve3DRepId theRep)
  {
    return myCurves3D.Change(theRep.Index);
  }

  //! @param[in] theRep typed curve-2D representation id
  BRepGraphInc::Curve2DRep& ChangeCurve2DRep(const BRepGraph_Curve2DRepId theRep)
  {
    return myCurves2D.Change(theRep.Index);
  }

  //! @param[in] theRep typed triangulation representation id
  BRepGraphInc::TriangulationRep& ChangeTriangulationRep(const BRepGraph_TriangulationRepId theRep)
  {
    return myTriangulationsRep.Change(theRep.Index);
  }

  //! @param[in] theRep typed polygon-3D representation id
  BRepGraphInc::Polygon3DRep& ChangePolygon3DRep(const BRepGraph_Polygon3DRepId theRep)
  {
    return myPolygons3D.Change(theRep.Index);
  }

  //! @param[in] theRep typed polygon-2D representation id
  BRepGraphInc::Polygon2DRep& ChangePolygon2DRep(const BRepGraph_Polygon2DRepId theRep)
  {
    return myPolygons2D.Change(theRep.Index);
  }

  //! @param[in] theRep typed polygon-on-triangulation representation id
  BRepGraphInc::PolygonOnTriRep& ChangePolygonOnTriRep(const BRepGraph_PolygonOnTriRepId theRep)
  {
    return myPolygonsOnTri.Change(theRep.Index);
  }

  //! @name Append representation entities
  //! Each method creates a new representation entity, increments the active count,
  //! and returns a mutable reference to the appended entry for initialization.

  BRepGraphInc::SurfaceRep& AppendSurfaceRep() { return mySurfaces.Append(); }

  BRepGraphInc::Curve3DRep& AppendCurve3DRep() { return myCurves3D.Append(); }

  BRepGraphInc::Curve2DRep& AppendCurve2DRep() { return myCurves2D.Append(); }

  BRepGraphInc::TriangulationRep& AppendTriangulationRep() { return myTriangulationsRep.Append(); }

  BRepGraphInc::Polygon3DRep& AppendPolygon3DRep() { return myPolygons3D.Append(); }

  BRepGraphInc::Polygon2DRep& AppendPolygon2DRep() { return myPolygons2D.Append(); }

  BRepGraphInc::PolygonOnTriRep& AppendPolygonOnTriRep() { return myPolygonsOnTri.Append(); }

  //! @name Const entity access
  //! Each method returns a const reference to the entity at the given typed id.

  //! @param[in] theVertex typed vertex id
  [[nodiscard]] const BRepGraphInc::VertexEntity& Vertex(const BRepGraph_VertexId theVertex) const
  {
    return myVertices.Get(theVertex.Index);
  }

  //! @param[in] theEdge typed edge id
  [[nodiscard]] const BRepGraphInc::EdgeEntity& Edge(const BRepGraph_EdgeId theEdge) const
  {
    return myEdges.Get(theEdge.Index);
  }

  //! @param[in] theCoEdge typed coedge id
  [[nodiscard]] const BRepGraphInc::CoEdgeEntity& CoEdge(const BRepGraph_CoEdgeId theCoEdge) const
  {
    return myCoEdges.Get(theCoEdge.Index);
  }

  //! @param[in] theWire typed wire id
  [[nodiscard]] const BRepGraphInc::WireEntity& Wire(const BRepGraph_WireId theWire) const
  {
    return myWires.Get(theWire.Index);
  }

  //! @param[in] theFace typed face id
  [[nodiscard]] const BRepGraphInc::FaceEntity& Face(const BRepGraph_FaceId theFace) const
  {
    return myFaces.Get(theFace.Index);
  }

  //! @param[in] theShell typed shell id
  [[nodiscard]] const BRepGraphInc::ShellEntity& Shell(const BRepGraph_ShellId theShell) const
  {
    return myShells.Get(theShell.Index);
  }

  //! @param[in] theSolid typed solid id
  [[nodiscard]] const BRepGraphInc::SolidEntity& Solid(const BRepGraph_SolidId theSolid) const
  {
    return mySolids.Get(theSolid.Index);
  }

  //! @param[in] theCompound typed compound id
  [[nodiscard]] const BRepGraphInc::CompoundEntity& Compound(const BRepGraph_CompoundId theCompound) const
  {
    return myCompounds.Get(theCompound.Index);
  }

  //! @param[in] theCompSolid typed comp-solid id
  [[nodiscard]] const BRepGraphInc::CompSolidEntity& CompSolid(const BRepGraph_CompSolidId theCompSolid) const
  {
    return myCompSolids.Get(theCompSolid.Index);
  }

  //! @param[in] theProduct typed product id
  [[nodiscard]] const BRepGraphInc::ProductEntity& Product(const BRepGraph_ProductId theProduct) const
  {
    return myProducts.Get(theProduct.Index);
  }

  //! @param[in] theOccurrence typed occurrence id
  [[nodiscard]] const BRepGraphInc::OccurrenceEntity& Occurrence(const BRepGraph_OccurrenceId theOccurrence) const
  {
    return myOccurrences.Get(theOccurrence.Index);
  }

  //! @name Mutable entity access
  //! Each method returns a mutable reference to the entity at the given typed id.

  //! @param[in] theVertex typed vertex id
  BRepGraphInc::VertexEntity& ChangeVertex(const BRepGraph_VertexId theVertex)
  {
    return myVertices.Change(theVertex.Index);
  }

  //! @param[in] theEdge typed edge id
  BRepGraphInc::EdgeEntity& ChangeEdge(const BRepGraph_EdgeId theEdge)
  {
    return myEdges.Change(theEdge.Index);
  }

  //! @param[in] theCoEdge typed coedge id
  BRepGraphInc::CoEdgeEntity& ChangeCoEdge(const BRepGraph_CoEdgeId theCoEdge)
  {
    return myCoEdges.Change(theCoEdge.Index);
  }

  //! @param[in] theWire typed wire id
  BRepGraphInc::WireEntity& ChangeWire(const BRepGraph_WireId theWire)
  {
    return myWires.Change(theWire.Index);
  }

  //! @param[in] theFace typed face id
  BRepGraphInc::FaceEntity& ChangeFace(const BRepGraph_FaceId theFace)
  {
    return myFaces.Change(theFace.Index);
  }

  //! @param[in] theShell typed shell id
  BRepGraphInc::ShellEntity& ChangeShell(const BRepGraph_ShellId theShell)
  {
    return myShells.Change(theShell.Index);
  }

  //! @param[in] theSolid typed solid id
  BRepGraphInc::SolidEntity& ChangeSolid(const BRepGraph_SolidId theSolid)
  {
    return mySolids.Change(theSolid.Index);
  }

  //! @param[in] theCompound typed compound id
  BRepGraphInc::CompoundEntity& ChangeCompound(const BRepGraph_CompoundId theCompound)
  {
    return myCompounds.Change(theCompound.Index);
  }

  //! @param[in] theCompSolid typed comp-solid id
  BRepGraphInc::CompSolidEntity& ChangeCompSolid(const BRepGraph_CompSolidId theCompSolid)
  {
    return myCompSolids.Change(theCompSolid.Index);
  }

  //! @param[in] theProduct typed product id
  BRepGraphInc::ProductEntity& ChangeProduct(const BRepGraph_ProductId theProduct)
  {
    return myProducts.Change(theProduct.Index);
  }

  //! @param[in] theOccurrence typed occurrence id
  BRepGraphInc::OccurrenceEntity& ChangeOccurrence(const BRepGraph_OccurrenceId theOccurrence)
  {
    return myOccurrences.Change(theOccurrence.Index);
  }

  //! @name Append entity (returns mutable ref to newly created entity)
  //! Each method creates a new entity, increments the active count,
  //! initializes inner vectors with the storage allocator, and returns
  //! a mutable reference to the appended entry for initialization.

  BRepGraphInc::VertexEntity& AppendVertex() { return myVertices.Append(myAllocator); }

  BRepGraphInc::EdgeEntity& AppendEdge() { return myEdges.Append(myAllocator); }

  BRepGraphInc::CoEdgeEntity& AppendCoEdge() { return myCoEdges.Append(myAllocator); }

  BRepGraphInc::WireEntity& AppendWire() { return myWires.Append(myAllocator); }

  BRepGraphInc::FaceEntity& AppendFace() { return myFaces.Append(myAllocator); }

  BRepGraphInc::ShellEntity& AppendShell() { return myShells.Append(myAllocator); }

  BRepGraphInc::SolidEntity& AppendSolid() { return mySolids.Append(myAllocator); }

  BRepGraphInc::CompoundEntity& AppendCompound() { return myCompounds.Append(myAllocator); }

  BRepGraphInc::CompSolidEntity& AppendCompSolid() { return myCompSolids.Append(myAllocator); }

  BRepGraphInc::ProductEntity& AppendProduct() { return myProducts.Append(myAllocator); }

  BRepGraphInc::OccurrenceEntity& AppendOccurrence() { return myOccurrences.Append(myAllocator); }

  //! @name UID access

  //! Return the per-kind UID vector for a given Kind.
  [[nodiscard]] Standard_EXPORT const NCollection_Vector<BRepGraph_UID>& UIDs(
    const BRepGraph_NodeId::Kind theKind) const;

  //! Return the per-kind UID vector for a given Kind (mutable).
  Standard_EXPORT NCollection_Vector<BRepGraph_UID>& ChangeUIDs(
    const BRepGraph_NodeId::Kind theKind);

  //! Clear all UID vectors (reset lengths to 0).
  Standard_EXPORT void ResetAllUIDs();

  //! @name Reverse index

  [[nodiscard]] const BRepGraphInc_ReverseIndex& ReverseIndex() const { return myReverseIdx; }

  BRepGraphInc_ReverseIndex& ChangeReverseIndex() { return myReverseIdx; }

  //! @name TShape to NodeId map

  [[nodiscard]] const BRepGraph_NodeId* FindNodeByTShape(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.Seek(theTShape);
  }

  [[nodiscard]] bool HasTShapeBinding(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.IsBound(theTShape);
  }

  void BindTShapeToNode(const TopoDS_TShape* theTShape, const BRepGraph_NodeId theNodeId)
  {
    myTShapeToNodeId.Bind(theTShape, theNodeId);
  }

  //! @name Original shapes

  [[nodiscard]] const TopoDS_Shape* FindOriginal(const BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.Seek(theNodeId);
  }

  [[nodiscard]] bool HasOriginal(const BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.IsBound(theNodeId);
  }

  void BindOriginal(const BRepGraph_NodeId theNodeId, const TopoDS_Shape& theShape)
  {
    myOriginalShapes.Bind(theNodeId, theShape);
  }

  void UnBindOriginal(const BRepGraph_NodeId theNodeId) { myOriginalShapes.UnBind(theNodeId); }

  //! @name Population status

  [[nodiscard]] bool GetIsDone() const { return myIsDone; }

  void SetIsDone(const bool theVal) { myIsDone = theVal; }

  //! True if edge regularities were extracted during population.
  [[nodiscard]] bool HasRegularities() const { return myHasRegularities; }

  //! True if vertex point representations were extracted during population.
  [[nodiscard]] bool HasVertexPointReps() const { return myHasVertexPointReps; }

  //! Clear all storage.
  Standard_EXPORT void Clear();

  //! Build reverse indices from entity and relationship tables.
  //! Call after population is complete.
  Standard_EXPORT void BuildReverseIndex();

  //! Incrementally update reverse indices for entities appended after a previous Build.
  //! Only processes entities from the old counts to the current vector lengths.
  Standard_EXPORT void BuildDeltaReverseIndex(const int theOldNbEdges,
                                              const int theOldNbWires,
                                              const int theOldNbFaces,
                                              const int theOldNbShells,
                                              const int theOldNbSolids);

  //! Debug: verify reverse index consistency against entity tables.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool ValidateReverseIndex() const;

private:
  friend class BRepGraphInc_Populate;

  //! @brief Template store for topology entity kinds.
  //! Groups the entity vector, per-kind UID vector, and active count
  //! into a single struct, eliminating repeated boilerplate.
  template <typename EntityT>
  struct EntityStore
  {
    NCollection_Vector<EntityT>       Entities;
    NCollection_Vector<BRepGraph_UID> UIDs;
    int                               NbActive = 0;

    EntityStore() = default;

    EntityStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Entities(theBlockSize, theAlloc),
          UIDs(theBlockSize, theAlloc)
    {
    }

    int Nb() const { return Entities.Length(); }

    const EntityT& Get(const int theIdx) const { return Entities.Value(theIdx); }

    EntityT& Change(const int theIdx) { return Entities.ChangeValue(theIdx); }

    EntityT& Append(const occ::handle<NCollection_BaseAllocator>& theAlloc)
    {
      ++NbActive;
      EntityT& anEntity = Entities.Appended();
      anEntity.InitVectors(theAlloc);
      return anEntity;
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0, "EntityStore::DecrementActive: underflow");
      --NbActive;
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
    NCollection_Vector<RepT> Entities;
    int                      NbActive = 0;

    RepStore() = default;

    RepStore(const int theBlockSize, const occ::handle<NCollection_BaseAllocator>& theAlloc)
        : Entities(theBlockSize, theAlloc)
    {
    }

    int Nb() const { return Entities.Length(); }

    const RepT& Get(const int theIdx) const { return Entities.Value(theIdx); }

    RepT& Change(const int theIdx) { return Entities.ChangeValue(theIdx); }

    RepT& Append()
    {
      ++NbActive;
      return Entities.Appended();
    }

    void DecrementActive()
    {
      Standard_ASSERT_VOID(NbActive > 0, "RepStore::DecrementActive: underflow");
      --NbActive;
    }

    void EraseLast()
    {
      Standard_ASSERT_VOID(NbActive > 0, "RepStore::EraseLast: underflow");
      Entities.EraseLast();
      --NbActive;
    }

    void Clear()
    {
      Entities.Clear();
      NbActive = 0;
    }
  };

  //! @name Topology entity stores
  EntityStore<BRepGraphInc::VertexEntity>     myVertices;
  EntityStore<BRepGraphInc::EdgeEntity>       myEdges;
  EntityStore<BRepGraphInc::CoEdgeEntity>     myCoEdges;
  EntityStore<BRepGraphInc::WireEntity>       myWires;
  EntityStore<BRepGraphInc::FaceEntity>       myFaces;
  EntityStore<BRepGraphInc::ShellEntity>      myShells;
  EntityStore<BRepGraphInc::SolidEntity>      mySolids;
  EntityStore<BRepGraphInc::CompoundEntity>   myCompounds;
  EntityStore<BRepGraphInc::CompSolidEntity>  myCompSolids;
  EntityStore<BRepGraphInc::ProductEntity>    myProducts;
  EntityStore<BRepGraphInc::OccurrenceEntity> myOccurrences;

  //! @name Representation entity stores
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

  bool myIsDone             = false;
  bool myHasRegularities    = false;
  bool myHasVertexPointReps = false;
};

#endif // _BRepGraphInc_Storage_HeaderFile
