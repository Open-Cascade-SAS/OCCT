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
  const occ::handle<NCollection_BaseAllocator>& Allocator() const { return myAllocator; }

  //! @name Count accessors (total including removed)

  int NbVertices() const { return myVertices.Nb(); }

  int NbEdges() const { return myEdges.Nb(); }

  int NbCoEdges() const { return myCoEdges.Nb(); }

  int NbWires() const { return myWires.Nb(); }

  int NbFaces() const { return myFaces.Nb(); }

  int NbShells() const { return myShells.Nb(); }

  int NbSolids() const { return mySolids.Nb(); }

  int NbCompounds() const { return myCompounds.Nb(); }

  int NbCompSolids() const { return myCompSolids.Nb(); }

  int NbProducts() const { return myProducts.Nb(); }

  int NbOccurrences() const { return myOccurrences.Nb(); }

  //! @name Representation count accessors

  int NbSurfaces() const { return mySurfaces.Nb(); }

  int NbCurves3D() const { return myCurves3D.Nb(); }

  int NbCurves2D() const { return myCurves2D.Nb(); }

  int NbTriangulations() const { return myTriangulationsRep.Nb(); }

  int NbPolygons3D() const { return myPolygons3D.Nb(); }

  int NbPolygons2D() const { return myPolygons2D.Nb(); }

  int NbPolygonsOnTri() const { return myPolygonsOnTri.Nb(); }

  //! @name Representation active count accessors

  int NbActiveSurfaces() const { return mySurfaces.NbActive; }

  int NbActiveCurves3D() const { return myCurves3D.NbActive; }

  int NbActiveCurves2D() const { return myCurves2D.NbActive; }

  int NbActiveTriangulations() const { return myTriangulationsRep.NbActive; }

  int NbActivePolygons3D() const { return myPolygons3D.NbActive; }

  int NbActivePolygons2D() const { return myPolygons2D.NbActive; }

  int NbActivePolygonsOnTri() const { return myPolygonsOnTri.NbActive; }

  //! @name Active count accessors (excluding removed nodes)

  int NbActiveVertices() const { return myVertices.NbActive; }

  int NbActiveEdges() const { return myEdges.NbActive; }

  int NbActiveCoEdges() const { return myCoEdges.NbActive; }

  int NbActiveWires() const { return myWires.NbActive; }

  int NbActiveFaces() const { return myFaces.NbActive; }

  int NbActiveShells() const { return myShells.NbActive; }

  int NbActiveSolids() const { return mySolids.NbActive; }

  int NbActiveCompounds() const { return myCompounds.NbActive; }

  int NbActiveCompSolids() const { return myCompSolids.NbActive; }

  int NbActiveProducts() const { return myProducts.NbActive; }

  int NbActiveOccurrences() const { return myOccurrences.NbActive; }

  //! Decrement the active count for the given node kind.
  void DecrementActiveCount(const BRepGraph_NodeId::Kind theKind);

  //! @name Const representation access
  //! Each method returns a const reference to the representation entity at the given index.
  //! @param[in] theRepIdx zero-based index into the corresponding representation vector

  const BRepGraphInc::SurfaceRep& SurfaceRep(const int theRepIdx) const
  {
    return mySurfaces.Get(theRepIdx);
  }

  const BRepGraphInc::Curve3DRep& Curve3DRep(const int theRepIdx) const
  {
    return myCurves3D.Get(theRepIdx);
  }

  const BRepGraphInc::Curve2DRep& Curve2DRep(const int theRepIdx) const
  {
    return myCurves2D.Get(theRepIdx);
  }

  const BRepGraphInc::TriangulationRep& TriangulationRep(const int theRepIdx) const
  {
    return myTriangulationsRep.Get(theRepIdx);
  }

  const BRepGraphInc::Polygon3DRep& Polygon3DRep(const int theRepIdx) const
  {
    return myPolygons3D.Get(theRepIdx);
  }

  const BRepGraphInc::Polygon2DRep& Polygon2DRep(const int theRepIdx) const
  {
    return myPolygons2D.Get(theRepIdx);
  }

  const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(const int theRepIdx) const
  {
    return myPolygonsOnTri.Get(theRepIdx);
  }

  //! @name Mutable representation access
  //! Each method returns a mutable reference to the representation entity at the given index.
  //! @param[in] theRepIdx zero-based index into the corresponding representation vector

  BRepGraphInc::SurfaceRep& ChangeSurfaceRep(const int theRepIdx)
  {
    return mySurfaces.Change(theRepIdx);
  }

  BRepGraphInc::Curve3DRep& ChangeCurve3DRep(const int theRepIdx)
  {
    return myCurves3D.Change(theRepIdx);
  }

  BRepGraphInc::Curve2DRep& ChangeCurve2DRep(const int theRepIdx)
  {
    return myCurves2D.Change(theRepIdx);
  }

  BRepGraphInc::TriangulationRep& ChangeTriangulationRep(const int theRepIdx)
  {
    return myTriangulationsRep.Change(theRepIdx);
  }

  BRepGraphInc::Polygon3DRep& ChangePolygon3DRep(const int theRepIdx)
  {
    return myPolygons3D.Change(theRepIdx);
  }

  BRepGraphInc::Polygon2DRep& ChangePolygon2DRep(const int theRepIdx)
  {
    return myPolygons2D.Change(theRepIdx);
  }

  BRepGraphInc::PolygonOnTriRep& ChangePolygonOnTriRep(const int theRepIdx)
  {
    return myPolygonsOnTri.Change(theRepIdx);
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
  //! Each method returns a const reference to the entity at the given per-kind index.

  //! @param[in] theVertexIdx zero-based vertex entity index
  const BRepGraphInc::VertexEntity& Vertex(const int theVertexIdx) const
  {
    return myVertices.Get(theVertexIdx);
  }

  //! @param[in] theEdgeIdx zero-based edge entity index
  const BRepGraphInc::EdgeEntity& Edge(const int theEdgeIdx) const
  {
    return myEdges.Get(theEdgeIdx);
  }

  //! @param[in] theCoEdgeIdx zero-based coedge entity index
  const BRepGraphInc::CoEdgeEntity& CoEdge(const int theCoEdgeIdx) const
  {
    return myCoEdges.Get(theCoEdgeIdx);
  }

  //! @param[in] theWireIdx zero-based wire entity index
  const BRepGraphInc::WireEntity& Wire(const int theWireIdx) const
  {
    return myWires.Get(theWireIdx);
  }

  //! @param[in] theFaceIdx zero-based face entity index
  const BRepGraphInc::FaceEntity& Face(const int theFaceIdx) const
  {
    return myFaces.Get(theFaceIdx);
  }

  //! @param[in] theShellIdx zero-based shell entity index
  const BRepGraphInc::ShellEntity& Shell(const int theShellIdx) const
  {
    return myShells.Get(theShellIdx);
  }

  //! @param[in] theSolidIdx zero-based solid entity index
  const BRepGraphInc::SolidEntity& Solid(const int theSolidIdx) const
  {
    return mySolids.Get(theSolidIdx);
  }

  //! @param[in] theCompoundIdx zero-based compound entity index
  const BRepGraphInc::CompoundEntity& Compound(const int theCompoundIdx) const
  {
    return myCompounds.Get(theCompoundIdx);
  }

  //! @param[in] theCompSolidIdx zero-based comp-solid entity index
  const BRepGraphInc::CompSolidEntity& CompSolid(const int theCompSolidIdx) const
  {
    return myCompSolids.Get(theCompSolidIdx);
  }

  //! @param[in] theProductIdx zero-based product entity index
  const BRepGraphInc::ProductEntity& Product(const int theProductIdx) const
  {
    return myProducts.Get(theProductIdx);
  }

  //! @param[in] theOccurrenceIdx zero-based occurrence entity index
  const BRepGraphInc::OccurrenceEntity& Occurrence(const int theOccurrenceIdx) const
  {
    return myOccurrences.Get(theOccurrenceIdx);
  }

  //! @name Mutable entity access
  //! Each method returns a mutable reference to the entity at the given per-kind index.

  //! @param[in] theVertexIdx zero-based vertex entity index
  BRepGraphInc::VertexEntity& ChangeVertex(const int theVertexIdx)
  {
    return myVertices.Change(theVertexIdx);
  }

  //! @param[in] theEdgeIdx zero-based edge entity index
  BRepGraphInc::EdgeEntity& ChangeEdge(const int theEdgeIdx) { return myEdges.Change(theEdgeIdx); }

  //! @param[in] theCoEdgeIdx zero-based coedge entity index
  BRepGraphInc::CoEdgeEntity& ChangeCoEdge(const int theCoEdgeIdx)
  {
    return myCoEdges.Change(theCoEdgeIdx);
  }

  //! @param[in] theWireIdx zero-based wire entity index
  BRepGraphInc::WireEntity& ChangeWire(const int theWireIdx) { return myWires.Change(theWireIdx); }

  //! @param[in] theFaceIdx zero-based face entity index
  BRepGraphInc::FaceEntity& ChangeFace(const int theFaceIdx) { return myFaces.Change(theFaceIdx); }

  //! @param[in] theShellIdx zero-based shell entity index
  BRepGraphInc::ShellEntity& ChangeShell(const int theShellIdx)
  {
    return myShells.Change(theShellIdx);
  }

  //! @param[in] theSolidIdx zero-based solid entity index
  BRepGraphInc::SolidEntity& ChangeSolid(const int theSolidIdx)
  {
    return mySolids.Change(theSolidIdx);
  }

  //! @param[in] theCompoundIdx zero-based compound entity index
  BRepGraphInc::CompoundEntity& ChangeCompound(const int theCompoundIdx)
  {
    return myCompounds.Change(theCompoundIdx);
  }

  //! @param[in] theCompSolidIdx zero-based comp-solid entity index
  BRepGraphInc::CompSolidEntity& ChangeCompSolid(const int theCompSolidIdx)
  {
    return myCompSolids.Change(theCompSolidIdx);
  }

  //! @param[in] theProductIdx zero-based product entity index
  BRepGraphInc::ProductEntity& ChangeProduct(const int theProductIdx)
  {
    return myProducts.Change(theProductIdx);
  }

  //! @param[in] theOccurrenceIdx zero-based occurrence entity index
  BRepGraphInc::OccurrenceEntity& ChangeOccurrence(const int theOccurrenceIdx)
  {
    return myOccurrences.Change(theOccurrenceIdx);
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
  Standard_EXPORT const NCollection_Vector<BRepGraph_UID>& UIDs(
    const BRepGraph_NodeId::Kind theKind) const;

  //! Return the per-kind UID vector for a given Kind (mutable).
  Standard_EXPORT NCollection_Vector<BRepGraph_UID>& ChangeUIDs(
    const BRepGraph_NodeId::Kind theKind);

  //! Clear all UID vectors (reset lengths to 0).
  Standard_EXPORT void ResetAllUIDs();

  //! @name Reverse index

  const BRepGraphInc_ReverseIndex& ReverseIndex() const { return myReverseIdx; }

  BRepGraphInc_ReverseIndex& ChangeReverseIndex() { return myReverseIdx; }

  //! @name TShape to NodeId map

  const BRepGraph_NodeId* FindNodeByTShape(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.Seek(theTShape);
  }

  bool HasTShapeBinding(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.IsBound(theTShape);
  }

  void BindTShapeToNode(const TopoDS_TShape* theTShape, const BRepGraph_NodeId theNodeId)
  {
    myTShapeToNodeId.Bind(theTShape, theNodeId);
  }

  //! @name Original shapes

  const TopoDS_Shape* FindOriginal(const BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.Seek(theNodeId);
  }

  bool HasOriginal(const BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.IsBound(theNodeId);
  }

  void BindOriginal(const BRepGraph_NodeId theNodeId, const TopoDS_Shape& theShape)
  {
    myOriginalShapes.Bind(theNodeId, theShape);
  }

  void UnBindOriginal(const BRepGraph_NodeId theNodeId) { myOriginalShapes.UnBind(theNodeId); }

  //! @name Population status

  bool GetIsDone() const { return myIsDone; }

  void SetIsDone(const bool theVal) { myIsDone = theVal; }

  //! True if edge regularities were extracted during population.
  bool HasRegularities() const { return myHasRegularities; }

  //! True if vertex point representations were extracted during population.
  bool HasVertexPointReps() const { return myHasVertexPointReps; }

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
