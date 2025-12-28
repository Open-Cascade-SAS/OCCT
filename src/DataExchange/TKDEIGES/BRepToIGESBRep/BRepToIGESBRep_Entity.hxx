// Created on: 1995-04-25
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepToIGESBRep_Entity_HeaderFile
#define _BRepToIGESBRep_Entity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <BRepToIGES_BREntity.hxx>
#include <Standard_Integer.hxx>
#include <Message_ProgressRange.hxx>

class IGESSolid_EdgeList;
class IGESSolid_VertexList;
class TopoDS_Vertex;
class TopoDS_Edge;
class IGESData_IGESEntity;
class TopoDS_Shape;
class TopoDS_Face;
class IGESSolid_Loop;
class TopoDS_Wire;
class IGESSolid_Face;
class IGESSolid_Shell;
class TopoDS_Shell;
class IGESSolid_ManifoldSolid;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;

//! provides methods to transfer BRep entity from CASCADE to IGESBRep.
class BRepToIGESBRep_Entity : public BRepToIGES_BREntity
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a tool Entity
  Standard_EXPORT BRepToIGESBRep_Entity();

  //! Clears the contents of the fields
  Standard_EXPORT void Clear();

  //! Create the VertexList entity
  Standard_EXPORT void TransferVertexList();

  //! Returns the index of <myvertex> in "myVertices"
  Standard_EXPORT int IndexVertex(const TopoDS_Vertex& myvertex) const;

  //! Stores <myvertex> in "myVertices"
  //! Returns the index of <myvertex>.
  Standard_EXPORT int AddVertex(const TopoDS_Vertex& myvertex);

  //! Transfer an Edge entity from TopoDS to IGES
  Standard_EXPORT void TransferEdgeList();

  //! Returns the index of <myedge> in "myEdges"
  Standard_EXPORT int IndexEdge(const TopoDS_Edge& myedge) const;

  //! Stores <myedge> in "myEdges" and <mycurve3d> in "myCurves".
  //! Returns the index of <myedge>.
  Standard_EXPORT int AddEdge(const TopoDS_Edge&                      myedge,
                              const occ::handle<IGESData_IGESEntity>& mycurve3d);

  //! Returns the result of the transfert of any Shape
  //! If the transfer has failed, this member returns a NullEntity.
  Standard_EXPORT virtual occ::handle<IGESData_IGESEntity> TransferShape(
    const TopoDS_Shape&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange()) override;

  //! Transfer an Edge entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferEdge(const TopoDS_Edge& myedge);

  //! Transfer an Edge entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferEdge(const TopoDS_Edge& myedge,
                                                                const TopoDS_Face& myface,
                                                                const double       length);

  //! Transfer a Wire entity from TopoDS to IGES.
  //! Returns the curve associated to mywire in the parametric space of myface.
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESSolid_Loop> TransferWire(const TopoDS_Wire& mywire,
                                                           const TopoDS_Face& myface,
                                                           const double       length);

  //! Transfer a Face entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESSolid_Face> TransferFace(const TopoDS_Face& start);

  //! Transfer an Shell entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESSolid_Shell> TransferShell(
    const TopoDS_Shell&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfer a Solid entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESSolid_ManifoldSolid> TransferSolid(
    const TopoDS_Solid&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfer an CompSolid entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferCompSolid(
    const TopoDS_CompSolid&      start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfer a Compound entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT occ::handle<IGESData_IGESEntity> TransferCompound(
    const TopoDS_Compound&       start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

private:
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myVertices;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myEdges;
  NCollection_Sequence<occ::handle<Standard_Transient>>         myCurves;
  occ::handle<IGESSolid_EdgeList>                               myEdgeList;
  occ::handle<IGESSolid_VertexList>                             myVertexList;
};

#endif // _BRepToIGESBRep_Entity_HeaderFile
