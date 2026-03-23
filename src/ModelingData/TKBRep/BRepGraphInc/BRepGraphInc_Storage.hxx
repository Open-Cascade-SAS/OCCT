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

//! @brief Central storage for the incidence-table model.
//!
//! Holds all entity vectors, reverse indices, TShape deduplication maps,
//! and UID vectors.  Typed accessor methods enforce invariants and provide
//! compile-time safety.  BRepGraphInc_Populate has friend access for
//! efficient bulk writes during population.
class BRepGraphInc_Storage
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct with allocator for internal collections.
  //! If null, uses CommonBaseAllocator.
  Standard_EXPORT explicit BRepGraphInc_Storage(
    const occ::handle<NCollection_BaseAllocator>& theAlloc = occ::handle<NCollection_BaseAllocator>());

  //! Return the allocator used for internal collections.
  const occ::handle<NCollection_BaseAllocator>& Allocator() const { return myAllocator; }

  // ------ Count accessors (total including removed) ------

  int NbVertices()   const { return myVertices.Length(); }
  int NbEdges()      const { return myEdges.Length(); }
  int NbCoEdges()    const { return myCoEdges.Length(); }
  int NbWires()      const { return myWires.Length(); }
  int NbFaces()      const { return myFaces.Length(); }
  int NbShells()     const { return myShells.Length(); }
  int NbSolids()     const { return mySolids.Length(); }
  int NbCompounds()   const { return myCompounds.Length(); }
  int NbCompSolids()  const { return myCompSolids.Length(); }
  int NbProducts()    const { return myProducts.Length(); }
  int NbOccurrences() const { return myOccurrences.Length(); }

  // ------ Representation count accessors ------

  int NbSurfaces()       const { return mySurfaces.Length(); }
  int NbCurves3D()       const { return myCurves3D.Length(); }
  int NbCurves2D()       const { return myCurves2D.Length(); }
  int NbTriangulations() const { return myTriangulationsRep.Length(); }
  int NbPolygons3D()     const { return myPolygons3D.Length(); }
  int NbPolygons2D()     const { return myPolygons2D.Length(); }
  int NbPolygonsOnTri()  const { return myPolygonsOnTri.Length(); }

  // ------ Representation active count accessors ------

  int NbActiveSurfaces()       const { return myNbActiveSurfaces; }
  int NbActiveCurves3D()       const { return myNbActiveCurves3D; }
  int NbActiveCurves2D()       const { return myNbActiveCurves2D; }
  int NbActiveTriangulations() const { return myNbActiveTriangulations; }
  int NbActivePolygons3D()     const { return myNbActivePolygons3D; }
  int NbActivePolygons2D()     const { return myNbActivePolygons2D; }
  int NbActivePolygonsOnTri()  const { return myNbActivePolygonsOnTri; }

  // ------ Active count accessors (excluding removed nodes) ------

  int NbActiveVertices()   const { return myNbActiveVertices; }
  int NbActiveEdges()      const { return myNbActiveEdges; }
  int NbActiveCoEdges()    const { return myNbActiveCoEdges; }
  int NbActiveWires()      const { return myNbActiveWires; }
  int NbActiveFaces()      const { return myNbActiveFaces; }
  int NbActiveShells()     const { return myNbActiveShells; }
  int NbActiveSolids()     const { return myNbActiveSolids; }
  int NbActiveCompounds()   const { return myNbActiveCompounds; }
  int NbActiveCompSolids()  const { return myNbActiveCompSolids; }
  int NbActiveProducts()    const { return myNbActiveProducts; }
  int NbActiveOccurrences() const { return myNbActiveOccurrences; }

  //! Decrement the active count for the given node kind.
  void DecrementActiveCount(BRepGraph_NodeId::Kind theKind);

  // ------ Const representation access ------

  const BRepGraphInc::SurfaceRep&       SurfaceRep(int theIdx)       const { return mySurfaces.Value(theIdx); }
  const BRepGraphInc::Curve3DRep&       Curve3DRep(int theIdx)       const { return myCurves3D.Value(theIdx); }
  const BRepGraphInc::Curve2DRep&       Curve2DRep(int theIdx)       const { return myCurves2D.Value(theIdx); }
  const BRepGraphInc::TriangulationRep& TriangulationRep(int theIdx) const { return myTriangulationsRep.Value(theIdx); }
  const BRepGraphInc::Polygon3DRep&     Polygon3DRep(int theIdx)     const { return myPolygons3D.Value(theIdx); }
  const BRepGraphInc::Polygon2DRep&     Polygon2DRep(int theIdx)     const { return myPolygons2D.Value(theIdx); }
  const BRepGraphInc::PolygonOnTriRep&  PolygonOnTriRep(int theIdx)  const { return myPolygonsOnTri.Value(theIdx); }

  // ------ Mutable representation access ------

  BRepGraphInc::SurfaceRep&       ChangeSurfaceRep(int theIdx)       { return mySurfaces.ChangeValue(theIdx); }
  BRepGraphInc::Curve3DRep&       ChangeCurve3DRep(int theIdx)       { return myCurves3D.ChangeValue(theIdx); }
  BRepGraphInc::Curve2DRep&       ChangeCurve2DRep(int theIdx)       { return myCurves2D.ChangeValue(theIdx); }
  BRepGraphInc::TriangulationRep& ChangeTriangulationRep(int theIdx) { return myTriangulationsRep.ChangeValue(theIdx); }
  BRepGraphInc::Polygon3DRep&     ChangePolygon3DRep(int theIdx)     { return myPolygons3D.ChangeValue(theIdx); }
  BRepGraphInc::Polygon2DRep&     ChangePolygon2DRep(int theIdx)     { return myPolygons2D.ChangeValue(theIdx); }
  BRepGraphInc::PolygonOnTriRep&  ChangePolygonOnTriRep(int theIdx)  { return myPolygonsOnTri.ChangeValue(theIdx); }

  // ------ Append representation entities ------

  BRepGraphInc::SurfaceRep&       AppendSurfaceRep()       { ++myNbActiveSurfaces;       auto& e = mySurfaces.Appended();       return e; }
  BRepGraphInc::Curve3DRep&       AppendCurve3DRep()       { ++myNbActiveCurves3D;       auto& e = myCurves3D.Appended();       return e; }
  BRepGraphInc::Curve2DRep&       AppendCurve2DRep()       { ++myNbActiveCurves2D;       auto& e = myCurves2D.Appended();       return e; }
  BRepGraphInc::TriangulationRep& AppendTriangulationRep() { ++myNbActiveTriangulations; auto& e = myTriangulationsRep.Appended(); return e; }
  BRepGraphInc::Polygon3DRep&     AppendPolygon3DRep()     { ++myNbActivePolygons3D;     auto& e = myPolygons3D.Appended();     return e; }
  BRepGraphInc::Polygon2DRep&     AppendPolygon2DRep()     { ++myNbActivePolygons2D;     auto& e = myPolygons2D.Appended();     return e; }
  BRepGraphInc::PolygonOnTriRep&  AppendPolygonOnTriRep()  { ++myNbActivePolygonsOnTri;  auto& e = myPolygonsOnTri.Appended();  return e; }

  // ------ Const entity access ------

  const BRepGraphInc::VertexEntity&    Vertex(int theIdx)    const { return myVertices.Value(theIdx); }
  const BRepGraphInc::EdgeEntity&      Edge(int theIdx)      const { return myEdges.Value(theIdx); }
  const BRepGraphInc::CoEdgeEntity&    CoEdge(int theIdx)    const { return myCoEdges.Value(theIdx); }
  const BRepGraphInc::WireEntity&      Wire(int theIdx)      const { return myWires.Value(theIdx); }
  const BRepGraphInc::FaceEntity&      Face(int theIdx)      const { return myFaces.Value(theIdx); }
  const BRepGraphInc::ShellEntity&     Shell(int theIdx)     const { return myShells.Value(theIdx); }
  const BRepGraphInc::SolidEntity&     Solid(int theIdx)     const { return mySolids.Value(theIdx); }
  const BRepGraphInc::CompoundEntity&   Compound(int theIdx)   const { return myCompounds.Value(theIdx); }
  const BRepGraphInc::CompSolidEntity&  CompSolid(int theIdx)  const { return myCompSolids.Value(theIdx); }
  const BRepGraphInc::ProductEntity&    Product(int theIdx)    const { return myProducts.Value(theIdx); }
  const BRepGraphInc::OccurrenceEntity& Occurrence(int theIdx) const { return myOccurrences.Value(theIdx); }

  // ------ Mutable entity access ------

  BRepGraphInc::VertexEntity&    ChangeVertex(int theIdx)    { return myVertices.ChangeValue(theIdx); }
  BRepGraphInc::EdgeEntity&      ChangeEdge(int theIdx)      { return myEdges.ChangeValue(theIdx); }
  BRepGraphInc::CoEdgeEntity&    ChangeCoEdge(int theIdx)    { return myCoEdges.ChangeValue(theIdx); }
  BRepGraphInc::WireEntity&      ChangeWire(int theIdx)      { return myWires.ChangeValue(theIdx); }
  BRepGraphInc::FaceEntity&      ChangeFace(int theIdx)      { return myFaces.ChangeValue(theIdx); }
  BRepGraphInc::ShellEntity&     ChangeShell(int theIdx)     { return myShells.ChangeValue(theIdx); }
  BRepGraphInc::SolidEntity&     ChangeSolid(int theIdx)     { return mySolids.ChangeValue(theIdx); }
  BRepGraphInc::CompoundEntity&   ChangeCompound(int theIdx)   { return myCompounds.ChangeValue(theIdx); }
  BRepGraphInc::CompSolidEntity&  ChangeCompSolid(int theIdx)  { return myCompSolids.ChangeValue(theIdx); }
  BRepGraphInc::ProductEntity&    ChangeProduct(int theIdx)    { return myProducts.ChangeValue(theIdx); }
  BRepGraphInc::OccurrenceEntity& ChangeOccurrence(int theIdx) { return myOccurrences.ChangeValue(theIdx); }

  // ------ Append (returns mutable ref to newly created entity) ------
  // Inner vectors of each entity are initialized with the storage allocator.

  BRepGraphInc::VertexEntity&    AppendVertex()    { ++myNbActiveVertices;   auto& e = myVertices.Appended();   e.InitVectors(myAllocator); return e; }
  BRepGraphInc::EdgeEntity&      AppendEdge()      { ++myNbActiveEdges;      auto& e = myEdges.Appended();      e.InitVectors(myAllocator); return e; }
  BRepGraphInc::CoEdgeEntity&    AppendCoEdge()    { ++myNbActiveCoEdges;    auto& e = myCoEdges.Appended();    e.InitVectors(myAllocator); return e; }
  BRepGraphInc::WireEntity&      AppendWire()      { ++myNbActiveWires;      auto& e = myWires.Appended();      e.InitVectors(myAllocator); return e; }
  BRepGraphInc::FaceEntity&      AppendFace()      { ++myNbActiveFaces;      auto& e = myFaces.Appended();      e.InitVectors(myAllocator); return e; }
  BRepGraphInc::ShellEntity&     AppendShell()     { ++myNbActiveShells;     auto& e = myShells.Appended();     e.InitVectors(myAllocator); return e; }
  BRepGraphInc::SolidEntity&     AppendSolid()     { ++myNbActiveSolids;     auto& e = mySolids.Appended();     e.InitVectors(myAllocator); return e; }
  BRepGraphInc::CompoundEntity&   AppendCompound()   { ++myNbActiveCompounds;   auto& e = myCompounds.Appended();   e.InitVectors(myAllocator); return e; }
  BRepGraphInc::CompSolidEntity&  AppendCompSolid()  { ++myNbActiveCompSolids;  auto& e = myCompSolids.Appended();  e.InitVectors(myAllocator); return e; }
  BRepGraphInc::ProductEntity&    AppendProduct()    { ++myNbActiveProducts;    auto& e = myProducts.Appended();    e.InitVectors(myAllocator); return e; }
  BRepGraphInc::OccurrenceEntity& AppendOccurrence() { ++myNbActiveOccurrences; auto& e = myOccurrences.Appended(); return e; }

  // ------ UID access (Kind-dispatched, eliminates 8-way switches in consumers) ------

  //! Return the per-kind UID vector for a given Kind.
  const NCollection_Vector<BRepGraph_UID>& UIDs(BRepGraph_NodeId::Kind theKind) const;

  //! Return the per-kind UID vector for a given Kind (mutable).
  NCollection_Vector<BRepGraph_UID>& ChangeUIDs(BRepGraph_NodeId::Kind theKind);

  //! Clear all UID vectors (reset lengths to 0).
  void ResetAllUIDs();

  // ------ Reverse index ------

  const BRepGraphInc_ReverseIndex& ReverseIndex() const { return myReverseIdx; }
  BRepGraphInc_ReverseIndex&       ChangeReverseIndex()  { return myReverseIdx; }

  // ------ TShape -> NodeId map ------

  const BRepGraph_NodeId* FindNodeByTShape(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.Seek(theTShape);
  }

  bool HasTShapeBinding(const TopoDS_TShape* theTShape) const
  {
    return myTShapeToNodeId.IsBound(theTShape);
  }

  void BindTShapeToNode(const TopoDS_TShape* theTShape, BRepGraph_NodeId theNodeId)
  {
    myTShapeToNodeId.Bind(theTShape, theNodeId);
  }

  // ------ Original shapes ------

  const TopoDS_Shape* FindOriginal(BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.Seek(theNodeId);
  }

  bool HasOriginal(BRepGraph_NodeId theNodeId) const
  {
    return myOriginalShapes.IsBound(theNodeId);
  }

  void BindOriginal(BRepGraph_NodeId theNodeId, const TopoDS_Shape& theShape)
  {
    myOriginalShapes.Bind(theNodeId, theShape);
  }

  void UnBindOriginal(BRepGraph_NodeId theNodeId)
  {
    myOriginalShapes.UnBind(theNodeId);
  }

  // ------ Population status ------

  bool GetIsDone() const { return myIsDone; }
  void SetIsDone(bool theVal) { myIsDone = theVal; }

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
  Standard_EXPORT void BuildDeltaReverseIndex(int theOldNbEdges,
                                              int theOldNbWires,
                                              int theOldNbFaces,
                                              int theOldNbShells,
                                              int theOldNbSolids);

  //! Debug: verify reverse index consistency against entity tables.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool ValidateReverseIndex() const;

private:
  friend class BRepGraphInc_Populate;

  // Representation entity vectors
  NCollection_Vector<BRepGraphInc::SurfaceRep>       mySurfaces;
  NCollection_Vector<BRepGraphInc::Curve3DRep>       myCurves3D;
  NCollection_Vector<BRepGraphInc::Curve2DRep>       myCurves2D;
  NCollection_Vector<BRepGraphInc::TriangulationRep> myTriangulationsRep;
  NCollection_Vector<BRepGraphInc::Polygon3DRep>     myPolygons3D;
  NCollection_Vector<BRepGraphInc::Polygon2DRep>     myPolygons2D;
  NCollection_Vector<BRepGraphInc::PolygonOnTriRep>  myPolygonsOnTri;

  // Topology entity vectors
  NCollection_Vector<BRepGraphInc::VertexEntity>    myVertices;
  NCollection_Vector<BRepGraphInc::EdgeEntity>      myEdges;
  NCollection_Vector<BRepGraphInc::CoEdgeEntity>    myCoEdges;
  NCollection_Vector<BRepGraphInc::WireEntity>      myWires;
  NCollection_Vector<BRepGraphInc::FaceEntity>      myFaces;
  NCollection_Vector<BRepGraphInc::ShellEntity>     myShells;
  NCollection_Vector<BRepGraphInc::SolidEntity>     mySolids;
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

  int myNbActiveVertices   = 0;
  int myNbActiveEdges      = 0;
  int myNbActiveCoEdges    = 0;
  int myNbActiveWires      = 0;
  int myNbActiveFaces      = 0;
  int myNbActiveShells     = 0;
  int myNbActiveSolids     = 0;
  int myNbActiveCompounds   = 0;
  int myNbActiveCompSolids  = 0;
  int myNbActiveProducts    = 0;
  int myNbActiveOccurrences = 0;

  bool myIsDone              = false;
  bool myHasRegularities     = false;
  bool myHasVertexPointReps  = false;
};

#endif // _BRepGraphInc_Storage_HeaderFile
