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

#ifndef _BRepGraphInc_Instance_HeaderFile
#define _BRepGraphInc_Instance_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <Standard_HashUtils.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <functional>

//! @file BRepGraphInc_Instance.hxx
//! @brief Unified lightweight container binding a node identity with placement.
//!
//! Instance<T> is the BRepGraph analogue of TopoDS_Shape: a lightweight value type
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

//! @brief Unified instance container template.
//!
//! Bundles a typed definition id with location and orientation.
//!
//! @tparam TypedIdT typed definition id (e.g. BRepGraph_FaceId, BRepGraph_NodeId).
template <typename TypedIdT>
struct Instance
{
  TypedIdT           DefId;
  TopLoc_Location    Location;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;
};

using VertexInstance     = Instance<BRepGraph_VertexId>;
using CoEdgeInstance     = Instance<BRepGraph_CoEdgeId>;
using FaceInstance       = Instance<BRepGraph_FaceId>;
using ShellInstance      = Instance<BRepGraph_ShellId>;
using SolidInstance      = Instance<BRepGraph_SolidId>;
using OccurrenceInstance = Instance<BRepGraph_OccurrenceId>;
using CompoundInstance   = Instance<BRepGraph_CompoundId>;
using CompSolidInstance  = Instance<BRepGraph_CompSolidId>;
using ProductInstance    = Instance<BRepGraph_ProductId>;

//! NodeInstance is Instance<BRepGraph_NodeId>.
//! Returned by BRepGraph_ChildExplorer and BRepGraph_ParentExplorer
//! with accumulated transforms from traversal root to the current node.
//! Implicitly convertible from any typed Instance via BRepGraph_NodeId::Typed
//! implicit conversion to BRepGraph_NodeId.
using NodeInstance = Instance<BRepGraph_NodeId>;

//! Wire instance with an additional flag indicating whether this is the outer wire.
struct WireInstance : Instance<BRepGraph_WireId>
{
  bool IsOuter = false;
};

} // namespace BRepGraphInc

//! std::hash specialization for BRepGraphInc::Instance<T>.
template <typename TypedIdT>
struct std::hash<BRepGraphInc::Instance<TypedIdT>>
{
  size_t operator()(const BRepGraphInc::Instance<TypedIdT>& theInstance) const noexcept
  {
    size_t aCombination[3];
    aCombination[0] = std::hash<TypedIdT>{}(theInstance.DefId);
    aCombination[1] = theInstance.Location.HashCode();
    aCombination[2] = opencascade::hash(static_cast<int>(theInstance.Orientation));
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraphInc_Instance_HeaderFile
