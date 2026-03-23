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

//! Enumeration of node kinds within a BRepGraph.
//!
//! Numeric ordering: topology kinds 0-5 match TopoDS hierarchy depth
//! (Solid = outermost container, Vertex = leaf).
//! Geometry kinds start at 10, leaving room for future topology extensions.
enum class BRepGraph_NodeKind : int
{
  Solid   = 0,
  Shell   = 1,
  Face    = 2,
  Wire    = 3,
  Edge    = 4,
  Vertex  = 5,
  Surface = 10,
  Curve   = 11,
  PCurve  = 12
};

//! Lightweight typed index into a per-kind node vector inside BRepGraph.
//!
//! The pair (Kind, Index) forms a unique node identifier within one graph
//! instance.  Default-constructed NodeId has Index = -1 (invalid).
//!
//! NodeId is a value type: cheap to copy, compare, hash.  It carries no
//! pointer back to the owning graph; the caller is responsible for using
//! it with the correct BRepGraph instance.
struct BRepGraph_NodeId
{
  BRepGraph_NodeKind Kind;
  int                Index;

  BRepGraph_NodeId()
    : Kind(BRepGraph_NodeKind::Solid), Index(-1) {}

  BRepGraph_NodeId(BRepGraph_NodeKind theKind, int theIdx)
    : Kind(theKind), Index(theIdx) {}

  //! True if this id points to an allocated node slot.
  bool IsValid() const { return Index >= 0; }

  bool operator==(const BRepGraph_NodeId& theOther) const
  { return Kind == theOther.Kind && Index == theOther.Index; }

  bool operator!=(const BRepGraph_NodeId& theOther) const
  { return !(*this == theOther); }

  //! Hasher for NCollection_DataMap / NCollection_IndexedDataMap.
  struct Hasher
  {
    size_t operator()(const BRepGraph_NodeId& theId) const noexcept
    {
      size_t aCombination[2];
      aCombination[0] = opencascade::hash(static_cast<int>(theId.Kind));
      aCombination[1] = opencascade::hash(theId.Index);
      return opencascade::hashBytes(aCombination, sizeof(aCombination));
    }
    bool operator()(const BRepGraph_NodeId& theA,
                    const BRepGraph_NodeId& theB) const noexcept
    { return theA == theB; }
  };
};

//! std::hash specialization for NCollection_DefaultHasher support.
template<>
struct std::hash<BRepGraph_NodeId>
{
  size_t operator()(const BRepGraph_NodeId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.Kind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraph_NodeId_HeaderFile
