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

#ifndef _BRepGraph_UsageId_HeaderFile
#define _BRepGraph_UsageId_HeaderFile

#include <BRepGraph_NodeId.hxx>

#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed index into a per-kind usage vector inside BRepGraph.
//!
//! The pair (Kind, Index) identifies a unique usage entry within one graph
//! instance.  Usages represent individual occurrences of a shared TShape
//! definition in the containment hierarchy, each carrying its own
//! orientation, location, and parent context.
//!
//! Default-constructed UsageId has Index = -1 (invalid).
struct BRepGraph_UsageId
{
  BRepGraph_NodeId::Kind NodeKind;
  int                    Index;

  BRepGraph_UsageId()
    : NodeKind(BRepGraph_NodeId::Kind::Solid), Index(-1) {}

  BRepGraph_UsageId(BRepGraph_NodeId::Kind theKind, int theIdx)
    : NodeKind(theKind), Index(theIdx) {}

  //! True if this id points to an allocated usage slot.
  bool IsValid() const { return Index >= 0; }

  //! @name Static factory methods for readable UsageId construction.
  static BRepGraph_UsageId Solid(int theIdx)  { return {BRepGraph_NodeId::Kind::Solid, theIdx}; }
  static BRepGraph_UsageId Shell(int theIdx)  { return {BRepGraph_NodeId::Kind::Shell, theIdx}; }
  static BRepGraph_UsageId Face(int theIdx)   { return {BRepGraph_NodeId::Kind::Face, theIdx}; }
  static BRepGraph_UsageId Wire(int theIdx)   { return {BRepGraph_NodeId::Kind::Wire, theIdx}; }
  static BRepGraph_UsageId Edge(int theIdx)   { return {BRepGraph_NodeId::Kind::Edge, theIdx}; }
  static BRepGraph_UsageId Vertex(int theIdx) { return {BRepGraph_NodeId::Kind::Vertex, theIdx}; }

  bool operator==(const BRepGraph_UsageId& theOther) const
  { return NodeKind == theOther.NodeKind && Index == theOther.Index; }

  bool operator!=(const BRepGraph_UsageId& theOther) const
  { return !(*this == theOther); }
};

//! std::hash specialization for NCollection_DefaultHasher support.
template<>
struct std::hash<BRepGraph_UsageId>
{
  size_t operator()(const BRepGraph_UsageId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.NodeKind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraph_UsageId_HeaderFile
