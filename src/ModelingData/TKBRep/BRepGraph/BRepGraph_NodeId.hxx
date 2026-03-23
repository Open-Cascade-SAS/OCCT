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

#ifndef _BRepGraph_NodeId_HeaderFile
#define _BRepGraph_NodeId_HeaderFile

#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed index into a per-kind node vector inside BRepGraph.
//!
//! The pair (NodeKind, Index) forms a unique node identifier within one graph
//! instance.  Default-constructed NodeId has Index = -1 (invalid).
//!
//! NodeId is a value type: cheap to copy, compare, hash.  It carries no
//! pointer back to the owning graph; the caller is responsible for using
//! it with the correct BRepGraph instance.
struct BRepGraph_NodeId
{
  //! Enumeration of node kinds within a BRepGraph.
  //!
  //! Topology kinds 0-5 cover core hierarchy; Compound(6)/CompSolid(7)
  //! are container kinds.  Note: ordering does NOT match TopAbs_ShapeEnum.
  //! Geometry kinds start at 10, leaving room for future topology extensions.
  enum class Kind : int
  {
    Solid      = 0,
    Shell      = 1,
    Face       = 2,
    Wire       = 3,
    Edge       = 4,
    Vertex     = 5,
    Compound   = 6,  //!< TopoDS_Compound container
    CompSolid  = 7,  //!< TopoDS_CompSolid container
    CoEdge     = 8,  //!< Use of an edge on a face (owns PCurve data)
    Product    = 10,  //!< Reusable shape definition (part or assembly)
    Occurrence = 11   //!< Placed instance of a product within a parent product
  };

  //! True if the kind is a core topology kind (Solid..CoEdge).
  static bool IsTopologyKind(const Kind theKind) { return static_cast<int>(theKind) <= 8; }

  //! True if the kind is an assembly kind (Product or Occurrence).
  static bool IsAssemblyKind(const Kind theKind) { return theKind == Kind::Product || theKind == Kind::Occurrence; }

  Kind NodeKind;
  int  Index;

  BRepGraph_NodeId()
    : NodeKind(Kind::Solid), Index(-1) {}

  BRepGraph_NodeId(const Kind theKind, const int theIdx)
    : NodeKind(theKind), Index(theIdx) {}

  //! True if this id points to an allocated node slot.
  bool IsValid() const { return Index >= 0; }

  //! @name Static factory methods for readable NodeId construction.
  static BRepGraph_NodeId Solid(const int theIdx)      { return {Kind::Solid, theIdx}; }
  static BRepGraph_NodeId Shell(const int theIdx)      { return {Kind::Shell, theIdx}; }
  static BRepGraph_NodeId Face(const int theIdx)       { return {Kind::Face, theIdx}; }
  static BRepGraph_NodeId Wire(const int theIdx)       { return {Kind::Wire, theIdx}; }
  static BRepGraph_NodeId Edge(const int theIdx)       { return {Kind::Edge, theIdx}; }
  static BRepGraph_NodeId Vertex(const int theIdx)     { return {Kind::Vertex, theIdx}; }
  static BRepGraph_NodeId Compound(const int theIdx)    { return {Kind::Compound, theIdx}; }
  static BRepGraph_NodeId CompSolid(const int theIdx)   { return {Kind::CompSolid, theIdx}; }
  static BRepGraph_NodeId CoEdge(const int theIdx)      { return {Kind::CoEdge, theIdx}; }
  static BRepGraph_NodeId Product(const int theIdx)     { return {Kind::Product, theIdx}; }
  static BRepGraph_NodeId Occurrence(const int theIdx)  { return {Kind::Occurrence, theIdx}; }

  bool operator==(const BRepGraph_NodeId& theOther) const
  { return NodeKind == theOther.NodeKind && Index == theOther.Index; }

  bool operator!=(const BRepGraph_NodeId& theOther) const
  { return !(*this == theOther); }
};

//! std::hash specialization for NCollection_DefaultHasher support.
template<>
struct std::hash<BRepGraph_NodeId>
{
  size_t operator()(const BRepGraph_NodeId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.NodeKind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraph_NodeId_HeaderFile
