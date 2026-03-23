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

#ifndef _BRepGraph_RepId_HeaderFile
#define _BRepGraph_RepId_HeaderFile

#include <Standard_HashUtils.hxx>

#include <cstddef>
#include <functional>

//! Lightweight typed index into a per-kind representation vector inside BRepGraph.
//!
//! The pair (RepKind, Index) forms a unique representation identifier within one
//! graph instance.  Default-constructed RepId has Index = -1 (invalid).
//!
//! Representations are NOT topology nodes — they hold geometry or mesh data
//! referenced by topology entities.  They do not participate in BFS traversal,
//! reverse index, or parent-child relationships.
//!
//! RepId is a value type: cheap to copy, compare, hash.
//! Fields are intentionally public (unlike BRepGraph_UID) because RepId
//! has no validity invariant — any (RepKind, Index) combination is valid
//! as a POD-like address, similar to BRepGraph_NodeId.
struct BRepGraph_RepId
{
  //! Categories of representation data.
  enum class RepKind : int
  {
    // Geometry (exact mathematical definition)
    Surface       = 0,  //!< Geom_Surface for faces
    Curve3D       = 1,  //!< Geom_Curve for edges
    Curve2D       = 2,  //!< Geom2d_Curve for coedges (PCurve geometry)

    // Mesh (discrete approximation)
    Triangulation = 3,  //!< Poly_Triangulation for faces
    Polygon3D     = 4,  //!< Poly_Polygon3D for edges
    Polygon2D     = 5,  //!< Poly_Polygon2D for coedges (polygon-on-surface)
    PolygonOnTri  = 6,  //!< Poly_PolygonOnTriangulation for coedges

    // Reserved 7-19 for future built-in types
    // Custom plugin types start at 100+
  };

  RepKind Kind;
  int     Index;

  BRepGraph_RepId()
    : Kind(RepKind::Surface), Index(-1) {}

  BRepGraph_RepId(const RepKind theKind, const int theIdx)
    : Kind(theKind), Index(theIdx) {}

  //! True if this id points to an allocated representation slot.
  bool IsValid() const { return Index >= 0; }

  //! @name Static factory methods for readable RepId construction.
  static BRepGraph_RepId Surface(const int theIdx)       { return {RepKind::Surface, theIdx}; }
  static BRepGraph_RepId Curve3D(const int theIdx)       { return {RepKind::Curve3D, theIdx}; }
  static BRepGraph_RepId Curve2D(const int theIdx)       { return {RepKind::Curve2D, theIdx}; }
  static BRepGraph_RepId Triangulation(const int theIdx) { return {RepKind::Triangulation, theIdx}; }
  static BRepGraph_RepId Polygon3D(const int theIdx)     { return {RepKind::Polygon3D, theIdx}; }
  static BRepGraph_RepId Polygon2D(const int theIdx)     { return {RepKind::Polygon2D, theIdx}; }
  static BRepGraph_RepId PolygonOnTri(const int theIdx)  { return {RepKind::PolygonOnTri, theIdx}; }

  bool operator==(const BRepGraph_RepId& theOther) const
  { return Kind == theOther.Kind && Index == theOther.Index; }

  bool operator!=(const BRepGraph_RepId& theOther) const
  { return !(*this == theOther); }
};

//! std::hash specialization for NCollection_DefaultHasher support.
template<>
struct std::hash<BRepGraph_RepId>
{
  size_t operator()(const BRepGraph_RepId& theId) const noexcept
  {
    size_t aCombination[2];
    aCombination[0] = opencascade::hash(static_cast<int>(theId.Kind));
    aCombination[1] = opencascade::hash(theId.Index);
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraph_RepId_HeaderFile
