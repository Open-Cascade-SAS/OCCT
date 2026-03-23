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

  int NbVertices() const { return myVertices.Length(); }

  int NbEdges() const { return myEdges.Length(); }

  int NbCoEdges() const { return myCoEdges.Length(); }

  int NbWires() const { return myWires.Length(); }

  int NbFaces() const { return myFaces.Length(); }

  int NbShells() const { return myShells.Length(); }

  int NbSolids() const { return mySolids.Length(); }

  int NbCompounds() const { return myCompounds.Length(); }

  int NbCompSolids() const { return myCompSolids.Length(); }

  int NbProducts() const { return myProducts.Length(); }

  int NbOccurrences() const { return myOccurrences.Length(); }

  //! @name Representation count accessors

  int NbSurfaces() const { return mySurfaces.Length(); }

  int NbCurves3D() const { return myCurves3D.Length(); }

  int NbCurves2D() const { return myCurves2D.Length(); }

  int NbTriangulations() const { return myTriangulationsRep.Length(); }

  int NbPolygons3D() const { return myPolygons3D.Length(); }

  int NbPolygons2D() const { return myPolygons2D.Length(); }

  int NbPolygonsOnTri() const { return myPolygonsOnTri.Length(); }

  //! @name Representation active count accessors

  int NbActiveSurfaces() const { return myNbActiveSurfaces; }

  int NbActiveCurves3D() const { return myNbActiveCurves3D; }

  int NbActiveCurves2D() const { return myNbActiveCurves2D; }

  int NbActiveTriangulations() const { return myNbActiveTriangulations; }

  int NbActivePolygons3D() const { return myNbActivePolygons3D; }

  int NbActivePolygons2D() const { return myNbActivePolygons2D; }

  int NbActivePolygonsOnTri() const { return myNbActivePolygonsOnTri; }

  //! @name Active count accessors (excluding removed nodes)

  int NbActiveVertices() const { return myNbActiveVertices; }

  int NbActiveEdges() const { return myNbActiveEdges; }

  int NbActiveCoEdges() const { return myNbActiveCoEdges; }

  int NbActiveWires() const { return myNbActiveWires; }

  int NbActiveFaces() const { return myNbActiveFaces; }

  int NbActiveShells() const { return myNbActiveShells; }

  int NbActiveSolids() const { return myNbActiveSolids; }

  int NbActiveCompounds() const { return myNbActiveCompounds; }

  int NbActiveCompSolids() const { return myNbActiveCompSolids; }

  int NbActiveProducts() const { return myNbActiveProducts; }

  int NbActiveOccurrences() const { return myNbActiveOccurrences; }

  //! Decrement the active count for the given node kind.
  void DecrementActiveCount(const BRepGraph_NodeId::Kind theKind);

  //! @name Const representation access
  //! Each method returns a const reference to the representation entity at the given index.
  //! @param[in] theRepIdx zero-based index into the corresponding representation vector

  const BRepGraphInc::SurfaceRep& SurfaceRep(const int theRepIdx) const
  {
    return mySurfaces.Value(theRepIdx);
  }

  const BRepGraphInc::Curve3DRep& Curve3DRep(const int theRepIdx) const
  {
    return myCurves3D.Value(theRepIdx);
  }

  const BRepGraphInc::Curve2DRep& Curve2DRep(const int theRepIdx) const
  {
    return myCurves2D.Value(theRepIdx);
  }

  const BRepGraphInc::TriangulationRep& TriangulationRep(const int theRepIdx) const
  {
    return myTriangulationsRep.Value(theRepIdx);
  }

  const BRepGraphInc::Polygon3DRep& Polygon3DRep(const int theRepIdx) const
  {
    return myPolygons3D.Value(theRepIdx);
  }

  const BRepGraphInc::Polygon2DRep& Polygon2DRep(const int theRepIdx) const
  {
    return myPolygons2D.Value(theRepIdx);
  }

  const BRepGraphInc::PolygonOnTriRep& PolygonOnTriRep(const int theRepIdx) const
  {
    return myPolygonsOnTri.Value(theRepIdx);
  }

  //! @name Mutable representation access
  //! Each method returns a mutable reference to the representation entity at the given index.
  //! @param[in] theRepIdx zero-based index into the corresponding representation vector

  BRepGraphInc::SurfaceRep& ChangeSurfaceRep(const int theRepIdx)
  {
    return mySurfaces.ChangeValue(theRepIdx);
  }

  BRepGraphInc::Curve3DRep& ChangeCurve3DRep(const int theRepIdx)
  {
    return myCurves3D.ChangeValue(theRepIdx);
  }

  BRepGraphInc::Curve2DRep& ChangeCurve2DRep(const int theRepIdx)
  {
    return myCurves2D.ChangeValue(theRepIdx);
  }

  BRepGraphInc::TriangulationRep& ChangeTriangulationRep(const int theRepIdx)
  {
    return myTriangulationsRep.ChangeValue(theRepIdx);
  }

  BRepGraphInc::Polygon3DRep& ChangePolygon3DRep(const int theRepIdx)
  {
    return myPolygons3D.ChangeValue(theRepIdx);
  }

  BRepGraphInc::Polygon2DRep& ChangePolygon2DRep(const int theRepIdx)
  {
    return myPolygons2D.ChangeValue(theRepIdx);
  }

  BRepGraphInc::PolygonOnTriRep& ChangePolygonOnTriRep(const int theRepIdx)
  {
    return myPolygonsOnTri.ChangeValue(theRepIdx);
  }

  //! @name Append representation entities
  //! Each method creates a new representation entity, increments the active count,
  //! and returns a mutable reference to the appended entry for initialization.

  BRepGraphInc::SurfaceRep& AppendSurfaceRep()
  {
    ++myNbActiveSurfaces;
    auto& e = mySurfaces.Appended();
    return e;
  }

  BRepGraphInc::Curve3DRep& AppendCurve3DRep()
  {
    ++myNbActiveCurves3D;
    auto& e = myCurves3D.Appended();
    return e;
  }

  BRepGraphInc::Curve2DRep& AppendCurve2DRep()
  {
    ++myNbActiveCurves2D;
    auto& e = myCurves2D.Appended();
    return e;
  }

  BRepGraphInc::TriangulationRep& AppendTriangulationRep()
  {
    ++myNbActiveTriangulations;
    auto& e = myTriangulationsRep.Appended();
    return e;
  }

  BRepGraphInc::Polygon3DRep& AppendPolygon3DRep()
  {
    ++myNbActivePolygons3D;
    auto& e = myPolygons3D.Appended();
    return e;
  }

  BRepGraphInc::Polygon2DRep& AppendPolygon2DRep()
  {
    ++myNbActivePolygons2D;
    auto& e = myPolygons2D.Appended();
    return e;
  }

  BRepGraphInc::PolygonOnTriRep& AppendPolygonOnTriRep()
  {
    ++myNbActivePolygonsOnTri;
    auto& e = myPolygonsOnTri.Appended();
    return e;
  }

  //! @name Const entity access
  //! Each method returns a const reference to the entity at the given per-kind index.

  //! @param[in] theVertexIdx zero-based vertex entity index
  const BRepGraphInc::VertexEntity& Vertex(const int theVertexIdx) const
  {
    return myVertices.Value(theVertexIdx);
  }

  //! @param[in] theEdgeIdx zero-based edge entity index
  const BRepGraphInc::EdgeEntity& Edge(const int theEdgeIdx) const
  {
    return myEdges.Value(theEdgeIdx);
  }

  //! @param[in] theCoEdgeIdx zero-based coedge entity index
  const BRepGraphInc::CoEdgeEntity& CoEdge(const int theCoEdgeIdx) const
  {
    return myCoEdges.Value(theCoEdgeIdx);
  }

  //! @param[in] theWireIdx zero-based wire entity index
  const BRepGraphInc::WireEntity& Wire(const int theWireIdx) const
  {
    return myWires.Value(theWireIdx);
  }

  //! @param[in] theFaceIdx zero-based face entity index
  const BRepGraphInc::FaceEntity& Face(const int theFaceIdx) const
  {
    return myFaces.Value(theFaceIdx);
  }

  //! @param[in] theShellIdx zero-based shell entity index
  const BRepGraphInc::ShellEntity& Shell(const int theShellIdx) const
  {
    return myShells.Value(theShellIdx);
  }

  //! @param[in] theSolidIdx zero-based solid entity index
  const BRepGraphInc::SolidEntity& Solid(const int theSolidIdx) const
  {
    return mySolids.Value(theSolidIdx);
  }

  //! @param[in] theCompoundIdx zero-based compound entity index
  const BRepGraphInc::CompoundEntity& Compound(const int theCompoundIdx) const
  {
    return myCompounds.Value(theCompoundIdx);
  }

  //! @param[in] theCompSolidIdx zero-based comp-solid entity index
  const BRepGraphInc::CompSolidEntity& CompSolid(const int theCompSolidIdx) const
  {
    return myCompSolids.Value(theCompSolidIdx);
  }

  //! @param[in] theProductIdx zero-based product entity index
  const BRepGraphInc::ProductEntity& Product(const int theProductIdx) const
  {
    return myProducts.Value(theProductIdx);
  }

  //! @param[in] theOccurrenceIdx zero-based occurrence entity index
  const BRepGraphInc::OccurrenceEntity& Occurrence(const int theOccurrenceIdx) const
  {
    return myOccurrences.Value(theOccurrenceIdx);
  }

  //! @name Mutable entity access
  //! Each method returns a mutable reference to the entity at the given per-kind index.

  //! @param[in] theVertexIdx zero-based vertex entity index
  BRepGraphInc::VertexEntity& ChangeVertex(const int theVertexIdx)
  {
    return myVertices.ChangeValue(theVertexIdx);
  }

  //! @param[in] theEdgeIdx zero-based edge entity index
  BRepGraphInc::EdgeEntity& ChangeEdge(const int theEdgeIdx)
  {
    return myEdges.ChangeValue(theEdgeIdx);
  }

  //! @param[in] theCoEdgeIdx zero-based coedge entity index
  BRepGraphInc::CoEdgeEntity& ChangeCoEdge(const int theCoEdgeIdx)
  {
    return myCoEdges.ChangeValue(theCoEdgeIdx);
  }

  //! @param[in] theWireIdx zero-based wire entity index
  BRepGraphInc::WireEntity& ChangeWire(const int theWireIdx)
  {
    return myWires.ChangeValue(theWireIdx);
  }

  //! @param[in] theFaceIdx zero-based face entity index
  BRepGraphInc::FaceEntity& ChangeFace(const int theFaceIdx)
  {
    return myFaces.ChangeValue(theFaceIdx);
  }

  //! @param[in] theShellIdx zero-based shell entity index
  BRepGraphInc::ShellEntity& ChangeShell(const int theShellIdx)
  {
    return myShells.ChangeValue(theShellIdx);
  }

  //! @param[in] theSolidIdx zero-based solid entity index
  BRepGraphInc::SolidEntity& ChangeSolid(const int theSolidIdx)
  {
    return mySolids.ChangeValue(theSolidIdx);
  }

  //! @param[in] theCompoundIdx zero-based compound entity index
  BRepGraphInc::CompoundEntity& ChangeCompound(const int theCompoundIdx)
  {
    return myCompounds.ChangeValue(theCompoundIdx);
  }

  //! @param[in] theCompSolidIdx zero-based comp-solid entity index
  BRepGraphInc::CompSolidEntity& ChangeCompSolid(const int theCompSolidIdx)
  {
    return myCompSolids.ChangeValue(theCompSolidIdx);
  }

  //! @param[in] theProductIdx zero-based product entity index
  BRepGraphInc::ProductEntity& ChangeProduct(const int theProductIdx)
  {
    return myProducts.ChangeValue(theProductIdx);
  }

  //! @param[in] theOccurrenceIdx zero-based occurrence entity index
  BRepGraphInc::OccurrenceEntity& ChangeOccurrence(const int theOccurrenceIdx)
  {
    return myOccurrences.ChangeValue(theOccurrenceIdx);
  }

  //! @name Append entity (returns mutable ref to newly created entity)
  //! Each method creates a new entity, increments the active count,
  //! initializes inner vectors with the storage allocator, and returns
  //! a mutable reference to the appended entry for initialization.

  BRepGraphInc::VertexEntity& AppendVertex()
  {
    ++myNbActiveVertices;
    auto& e = myVertices.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::EdgeEntity& AppendEdge()
  {
    ++myNbActiveEdges;
    auto& e = myEdges.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::CoEdgeEntity& AppendCoEdge()
  {
    ++myNbActiveCoEdges;
    auto& e = myCoEdges.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::WireEntity& AppendWire()
  {
    ++myNbActiveWires;
    auto& e = myWires.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::FaceEntity& AppendFace()
  {
    ++myNbActiveFaces;
    auto& e = myFaces.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::ShellEntity& AppendShell()
  {
    ++myNbActiveShells;
    auto& e = myShells.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::SolidEntity& AppendSolid()
  {
    ++myNbActiveSolids;
    auto& e = mySolids.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::CompoundEntity& AppendCompound()
  {
    ++myNbActiveCompounds;
    auto& e = myCompounds.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::CompSolidEntity& AppendCompSolid()
  {
    ++myNbActiveCompSolids;
    auto& e = myCompSolids.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::ProductEntity& AppendProduct()
  {
    ++myNbActiveProducts;
    auto& e = myProducts.Appended();
    e.InitVectors(myAllocator);
    return e;
  }

  BRepGraphInc::OccurrenceEntity& AppendOccurrence()
  {
    ++myNbActiveOccurrences;
    auto& e = myOccurrences.Appended();
    return e;
  }

  //! @name UID access

  //! Return the per-kind UID vector for a given Kind.
  const NCollection_Vector<BRepGraph_UID>& UIDs(const BRepGraph_NodeId::Kind theKind) const;

  //! Return the per-kind UID vector for a given Kind (mutable).
  NCollection_Vector<BRepGraph_UID>& ChangeUIDs(const BRepGraph_NodeId::Kind theKind);

  //! Clear all UID vectors (reset lengths to 0).
  void ResetAllUIDs();

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

  //! @name Representation entity vectors
  NCollection_Vector<BRepGraphInc::SurfaceRep>       mySurfaces;
  NCollection_Vector<BRepGraphInc::Curve3DRep>       myCurves3D;
  NCollection_Vector<BRepGraphInc::Curve2DRep>       myCurves2D;
  NCollection_Vector<BRepGraphInc::TriangulationRep> myTriangulationsRep;
  NCollection_Vector<BRepGraphInc::Polygon3DRep>     myPolygons3D;
  NCollection_Vector<BRepGraphInc::Polygon2DRep>     myPolygons2D;
  NCollection_Vector<BRepGraphInc::PolygonOnTriRep>  myPolygonsOnTri;

  //! @name Topology entity vectors
  NCollection_Vector<BRepGraphInc::VertexEntity>     myVertices;
  NCollection_Vector<BRepGraphInc::EdgeEntity>       myEdges;
  NCollection_Vector<BRepGraphInc::CoEdgeEntity>     myCoEdges;
  NCollection_Vector<BRepGraphInc::WireEntity>       myWires;
  NCollection_Vector<BRepGraphInc::FaceEntity>       myFaces;
  NCollection_Vector<BRepGraphInc::ShellEntity>      myShells;
  NCollection_Vector<BRepGraphInc::SolidEntity>      mySolids;
  NCollection_Vector<BRepGraphInc::CompoundEntity>   myCompounds;
  NCollection_Vector<BRepGraphInc::CompSolidEntity>  myCompSolids;
  NCollection_Vector<BRepGraphInc::ProductEntity>    myProducts;
  NCollection_Vector<BRepGraphInc::OccurrenceEntity> myOccurrences;

  BRepGraphInc_ReverseIndex myReverseIdx;

  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> myTShapeToNodeId;
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>         myOriginalShapes;

  NCollection_Vector<BRepGraph_UID> myVertexUIDs;
  NCollection_Vector<BRepGraph_UID> myEdgeUIDs;
  NCollection_Vector<BRepGraph_UID> myCoEdgeUIDs;
  NCollection_Vector<BRepGraph_UID> myWireUIDs;
  NCollection_Vector<BRepGraph_UID> myFaceUIDs;
  NCollection_Vector<BRepGraph_UID> myShellUIDs;
  NCollection_Vector<BRepGraph_UID> mySolidUIDs;
  NCollection_Vector<BRepGraph_UID> myCompoundUIDs;
  NCollection_Vector<BRepGraph_UID> myCompSolidUIDs;
  NCollection_Vector<BRepGraph_UID> myProductUIDs;
  NCollection_Vector<BRepGraph_UID> myOccurrenceUIDs;

  occ::handle<NCollection_BaseAllocator> myAllocator;

  int myNbActiveSurfaces       = 0;
  int myNbActiveCurves3D       = 0;
  int myNbActiveCurves2D       = 0;
  int myNbActiveTriangulations = 0;
  int myNbActivePolygons3D     = 0;
  int myNbActivePolygons2D     = 0;
  int myNbActivePolygonsOnTri  = 0;

  int myNbActiveVertices    = 0;
  int myNbActiveEdges       = 0;
  int myNbActiveCoEdges     = 0;
  int myNbActiveWires       = 0;
  int myNbActiveFaces       = 0;
  int myNbActiveShells      = 0;
  int myNbActiveSolids      = 0;
  int myNbActiveCompounds   = 0;
  int myNbActiveCompSolids  = 0;
  int myNbActiveProducts    = 0;
  int myNbActiveOccurrences = 0;

  bool myIsDone             = false;
  bool myHasRegularities    = false;
  bool myHasVertexPointReps = false;
};

#endif // _BRepGraphInc_Storage_HeaderFile
