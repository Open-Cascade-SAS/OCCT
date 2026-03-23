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
    const Handle(NCollection_BaseAllocator)& theAlloc = Handle(NCollection_BaseAllocator)());

  //! Return the allocator used for internal collections.
  const Handle(NCollection_BaseAllocator)& Allocator() const { return myAllocator; }

  // ------ Count accessors ------

  int NbVertices()   const { return myVertices.Length(); }
  int NbEdges()      const { return myEdges.Length(); }
  int NbWires()      const { return myWires.Length(); }
  int NbFaces()      const { return myFaces.Length(); }
  int NbShells()     const { return myShells.Length(); }
  int NbSolids()     const { return mySolids.Length(); }
  int NbCompounds()  const { return myCompounds.Length(); }
  int NbCompSolids() const { return myCompSolids.Length(); }

  // ------ Const entity access ------

  const BRepGraphInc::VertexEntity&    Vertex(int theIdx)    const { return myVertices.Value(theIdx); }
  const BRepGraphInc::EdgeEntity&      Edge(int theIdx)      const { return myEdges.Value(theIdx); }
  const BRepGraphInc::WireEntity&      Wire(int theIdx)      const { return myWires.Value(theIdx); }
  const BRepGraphInc::FaceEntity&      Face(int theIdx)      const { return myFaces.Value(theIdx); }
  const BRepGraphInc::ShellEntity&     Shell(int theIdx)     const { return myShells.Value(theIdx); }
  const BRepGraphInc::SolidEntity&     Solid(int theIdx)     const { return mySolids.Value(theIdx); }
  const BRepGraphInc::CompoundEntity&  Compound(int theIdx)  const { return myCompounds.Value(theIdx); }
  const BRepGraphInc::CompSolidEntity& CompSolid(int theIdx) const { return myCompSolids.Value(theIdx); }

  // ------ Mutable entity access ------

  BRepGraphInc::VertexEntity&    ChangeVertex(int theIdx)    { return myVertices.ChangeValue(theIdx); }
  BRepGraphInc::EdgeEntity&      ChangeEdge(int theIdx)      { return myEdges.ChangeValue(theIdx); }
  BRepGraphInc::WireEntity&      ChangeWire(int theIdx)      { return myWires.ChangeValue(theIdx); }
  BRepGraphInc::FaceEntity&      ChangeFace(int theIdx)      { return myFaces.ChangeValue(theIdx); }
  BRepGraphInc::ShellEntity&     ChangeShell(int theIdx)     { return myShells.ChangeValue(theIdx); }
  BRepGraphInc::SolidEntity&     ChangeSolid(int theIdx)     { return mySolids.ChangeValue(theIdx); }
  BRepGraphInc::CompoundEntity&  ChangeCompound(int theIdx)  { return myCompounds.ChangeValue(theIdx); }
  BRepGraphInc::CompSolidEntity& ChangeCompSolid(int theIdx) { return myCompSolids.ChangeValue(theIdx); }

  // ------ Append (returns mutable ref to newly created entity) ------
  // Inner vectors of each entity are initialized with the storage allocator.

  BRepGraphInc::VertexEntity&    AppendVertex()    { auto& e = myVertices.Appended();   e.InitVectors(myAllocator); return e; }
  BRepGraphInc::EdgeEntity&      AppendEdge()      { auto& e = myEdges.Appended();      e.InitVectors(myAllocator); return e; }
  BRepGraphInc::WireEntity&      AppendWire()      { auto& e = myWires.Appended();      e.InitVectors(myAllocator); return e; }
  BRepGraphInc::FaceEntity&      AppendFace()      { auto& e = myFaces.Appended();      e.InitVectors(myAllocator); return e; }
  BRepGraphInc::ShellEntity&     AppendShell()     { auto& e = myShells.Appended();     e.InitVectors(myAllocator); return e; }
  BRepGraphInc::SolidEntity&     AppendSolid()     { auto& e = mySolids.Appended();     e.InitVectors(myAllocator); return e; }
  BRepGraphInc::CompoundEntity&  AppendCompound()  { auto& e = myCompounds.Appended();  e.InitVectors(myAllocator); return e; }
  BRepGraphInc::CompSolidEntity& AppendCompSolid() { auto& e = myCompSolids.Appended(); e.InitVectors(myAllocator); return e; }

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

  //! Debug: verify reverse index consistency against entity tables.
  //! @return true if all forward refs have matching reverse entries
  Standard_EXPORT bool ValidateReverseIndex() const;

private:
  friend class BRepGraphInc_Populate;

  NCollection_Vector<BRepGraphInc::VertexEntity>    myVertices;
  NCollection_Vector<BRepGraphInc::EdgeEntity>      myEdges;
  NCollection_Vector<BRepGraphInc::WireEntity>      myWires;
  NCollection_Vector<BRepGraphInc::FaceEntity>      myFaces;
  NCollection_Vector<BRepGraphInc::ShellEntity>     myShells;
  NCollection_Vector<BRepGraphInc::SolidEntity>     mySolids;
  NCollection_Vector<BRepGraphInc::CompoundEntity>  myCompounds;
  NCollection_Vector<BRepGraphInc::CompSolidEntity> myCompSolids;

  BRepGraphInc_ReverseIndex myReverseIdx;

  NCollection_DataMap<const TopoDS_TShape*, BRepGraph_NodeId> myTShapeToNodeId;
  NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>         myOriginalShapes;

  NCollection_Vector<BRepGraph_UID> myVertexUIDs;
  NCollection_Vector<BRepGraph_UID> myEdgeUIDs;
  NCollection_Vector<BRepGraph_UID> myWireUIDs;
  NCollection_Vector<BRepGraph_UID> myFaceUIDs;
  NCollection_Vector<BRepGraph_UID> myShellUIDs;
  NCollection_Vector<BRepGraph_UID> mySolidUIDs;
  NCollection_Vector<BRepGraph_UID> myCompoundUIDs;
  NCollection_Vector<BRepGraph_UID> myCompSolidUIDs;

  Handle(NCollection_BaseAllocator) myAllocator;

  bool myIsDone              = false;
  bool myHasRegularities     = false;
  bool myHasVertexPointReps  = false;
};

#endif // _BRepGraphInc_Storage_HeaderFile
