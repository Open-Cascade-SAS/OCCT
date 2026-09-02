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
#include <BRepGraph_RefId.hxx>
#include <Standard_HashUtils.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Trsf.hxx>

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

  //! Returns true if the instance references an existing definition id.
  [[nodiscard]] bool IsValid() const { return DefId.IsValid(); }

  //! Compare complete occurrence identity, including orientation and placement.
  [[nodiscard]] friend bool operator==(const Instance& theLeft, const Instance& theRight)
  {
    return theLeft.DefId == theRight.DefId && theLeft.Orientation == theRight.Orientation
           && theLeft.Location.IsEqual(theRight.Location);
  }

  [[nodiscard]] friend bool operator!=(const Instance& theLeft, const Instance& theRight)
  {
    return !(theLeft == theRight);
  }
};

//! Deterministic geometric-placement ordering for instance presentation.
//!
//! Orders definition id, the complete 3x4 placement transform, then orientation.
//! Distinct TopLoc datum chains producing the same transform intentionally
//! compare equivalent; use operator== or std::hash when exact occurrence
//! identity is required.
template <typename TypedIdT>
struct InstancePlacementLess
{
  [[nodiscard]] bool operator()(const Instance<TypedIdT>& theLeft,
                                const Instance<TypedIdT>& theRight) const
  {
    if (theLeft.DefId != theRight.DefId)
    {
      return theLeft.DefId < theRight.DefId;
    }
    const gp_Trsf& aLeft  = theLeft.Location.Transformation();
    const gp_Trsf& aRight = theRight.Location.Transformation();
    for (int aRow = 1; aRow <= 3; ++aRow)
    {
      for (int aColumn = 1; aColumn <= 4; ++aColumn)
      {
        if (aLeft.Value(aRow, aColumn) != aRight.Value(aRow, aColumn))
        {
          return aLeft.Value(aRow, aColumn) < aRight.Value(aRow, aColumn);
        }
      }
    }
    return static_cast<int>(theLeft.Orientation) < static_cast<int>(theRight.Orientation);
  }
};

using VertexInstance     = Instance<BRepGraph_VertexId>;
using EdgeInstance       = Instance<BRepGraph_EdgeId>;
using CoEdgeInstance     = Instance<BRepGraph_CoEdgeId>;
using WireInstance       = Instance<BRepGraph_WireId>;
using FaceInstance       = Instance<BRepGraph_FaceId>;
using ShellInstance      = Instance<BRepGraph_ShellId>;
using SolidInstance      = Instance<BRepGraph_SolidId>;
using OccurrenceInstance = Instance<BRepGraph_OccurrenceId>;
using CompoundInstance   = Instance<BRepGraph_CompoundId>;
using CompSolidInstance  = Instance<BRepGraph_CompSolidId>;
using ProductInstance    = Instance<BRepGraph_ProductId>;

//! @brief Unified instance container for a typed reference identity.
//!
//! Ref instances preserve the concrete relation selected by an algorithm while
//! carrying its local location and orientation. The referenced definition can be
//! resolved directly through BRepGraph::Refs().
//!
//! @tparam TypedRefIdT typed reference id (e.g. BRepGraph_WireRefId).
template <typename TypedRefIdT>
struct ReferenceInstance
{
  TypedRefIdT        RefId;
  TopLoc_Location    Location;
  TopAbs_Orientation Orientation = TopAbs_FORWARD;

  //! Returns true if the instance references a valid reference id.
  [[nodiscard]] bool IsValid() const { return RefId.IsValid(); }

  //! Compare complete reference occurrence identity.
  [[nodiscard]] friend bool operator==(const ReferenceInstance& theLeft,
                                       const ReferenceInstance& theRight)
  {
    return theLeft.RefId == theRight.RefId && theLeft.Orientation == theRight.Orientation
           && theLeft.Location.IsEqual(theRight.Location);
  }

  [[nodiscard]] friend bool operator!=(const ReferenceInstance& theLeft,
                                       const ReferenceInstance& theRight)
  {
    return !(theLeft == theRight);
  }
};

using VertexRefInstance     = ReferenceInstance<BRepGraph_VertexRefId>;
using WireRefInstance       = ReferenceInstance<BRepGraph_WireRefId>;
using FaceRefInstance       = ReferenceInstance<BRepGraph_FaceRefId>;
using ShellRefInstance      = ReferenceInstance<BRepGraph_ShellRefId>;
using SolidRefInstance      = ReferenceInstance<BRepGraph_SolidRefId>;
using ChildRefInstance      = ReferenceInstance<BRepGraph_ChildRefId>;
using OccurrenceRefInstance = ReferenceInstance<BRepGraph_OccurrenceRefId>;

//! RefInstance is ReferenceInstance<BRepGraph_RefId> for generic reference traversal.
using RefInstance = ReferenceInstance<BRepGraph_RefId>;

//! NodeInstance is Instance<BRepGraph_NodeId> for generic node traversal.
//! Child and parent explorers return NodeInstance with accumulated location and orientation.
using NodeInstance = Instance<BRepGraph_NodeId>;

} // namespace BRepGraphInc

//! std::hash specialization for BRepGraphInc::Instance<T>.
template <typename TypedIdT>
struct std::hash<BRepGraphInc::Instance<TypedIdT>>
{
  size_t operator()(const BRepGraphInc::Instance<TypedIdT>& theInstance) const noexcept
  {
    const BRepGraph_NodeId anId(theInstance.DefId);
    const uint64_t aCombination[] = {(static_cast<uint64_t>(anId.NodeKind) << 32) | anId.Index,
                                     static_cast<uint64_t>(theInstance.Location.HashCode()),
                                     static_cast<uint64_t>(theInstance.Orientation)};
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

//! std::hash specialization for BRepGraphInc::ReferenceInstance<T>.
template <typename TypedRefIdT>
struct std::hash<BRepGraphInc::ReferenceInstance<TypedRefIdT>>
{
  size_t operator()(const BRepGraphInc::ReferenceInstance<TypedRefIdT>& theInstance) const noexcept
  {
    const BRepGraph_RefId anId(theInstance.RefId);
    const uint64_t aCombination[] = {(static_cast<uint64_t>(anId.RefKind) << 32) | anId.Index,
                                     static_cast<uint64_t>(theInstance.Location.HashCode()),
                                     static_cast<uint64_t>(theInstance.Orientation)};
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};

#endif // _BRepGraphInc_Instance_HeaderFile
