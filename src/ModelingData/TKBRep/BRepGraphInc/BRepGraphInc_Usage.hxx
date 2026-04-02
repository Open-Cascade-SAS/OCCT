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

#ifndef _BRepGraphInc_Usage_HeaderFile
#define _BRepGraphInc_Usage_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <Standard_HashUtils.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <functional>

//! @file BRepGraphInc_Usage.hxx
//! @brief Unified lightweight container binding a node identity with placement.
//!
//! Usage<T> is the BRepGraph analogue of TopoDS_Shape: a lightweight value type
//! that bundles a definition id with its location and orientation in context.
//!
//! Supports C++17 structured bindings (aggregate type):
//! @code
//!   for (auto [aDefId, aLoc, anOri] : BRepGraph_ChildExplorer(aGraph, aRoot, Kind::Face))
//!   {
//!     // ...
//!   }
//! @endcode
namespace BRepGraphInc
{

//! @brief Unified usage container template.
//!
//! Bundles a typed definition id with location and orientation.
//!
//! @tparam TypedIdT typed definition id (e.g. BRepGraph_FaceId, BRepGraph_NodeId).
template <typename TypedIdT>
struct Usage
{
  TypedIdT           DefId;
  TopLoc_Location    Location;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
};

using VertexUsage     = Usage<BRepGraph_VertexId>;
using CoEdgeUsage     = Usage<BRepGraph_CoEdgeId>;
using FaceUsage       = Usage<BRepGraph_FaceId>;
using ShellUsage      = Usage<BRepGraph_ShellId>;
using SolidUsage      = Usage<BRepGraph_SolidId>;
using OccurrenceUsage = Usage<BRepGraph_OccurrenceId>;
using CompoundUsage   = Usage<BRepGraph_CompoundId>;
using CompSolidUsage  = Usage<BRepGraph_CompSolidId>;
using ProductUsage    = Usage<BRepGraph_ProductId>;

//! NodeUsage is Usage<BRepGraph_NodeId>.
//! Returned by BRepGraph_ChildExplorer and BRepGraph_ParentExplorer
//! with accumulated transforms from traversal root to the current node.
//! Implicitly convertible from any typed Usage via BRepGraph_NodeId::Typed
//! implicit conversion to BRepGraph_NodeId.
using NodeUsage = Usage<BRepGraph_NodeId>;

//! Wire usage with an additional flag indicating whether this is the outer wire.
struct WireUsage : Usage<BRepGraph_WireId>
{
  bool IsOuter = false;
};

} // namespace BRepGraphInc

//! std::hash specialization for BRepGraphInc::Usage<T>.
template <typename TypedIdT>
struct std::hash<BRepGraphInc::Usage<TypedIdT>>
{
  size_t operator()(const BRepGraphInc::Usage<TypedIdT>& theUsage) const noexcept
  {
    size_t aCombination[3];
    aCombination[0] = std::hash<TypedIdT>{}(theUsage.DefId);
    aCombination[1] = theUsage.Location.HashCode();
    aCombination[2] = opencascade::hash(static_cast<int>(theUsage.Orientation));
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraphInc_Usage_HeaderFile
