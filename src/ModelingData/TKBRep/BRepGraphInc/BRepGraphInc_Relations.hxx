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

#ifndef _BRepGraphInc_Relations_HeaderFile
#define _BRepGraphInc_Relations_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <NCollection_LinearVector.hxx>

//! @brief Centralized topology relation representations for BRepGraph incidence storage.
//!
//! Relation structs hold ordered child-use lists and incoming incidence indexes
//! outside definition records. Definitions stay focused on intrinsic geometry
//! and flags; reusable child/parent edges live in reference records or coedge
//! use records.
namespace BRepGraphInc
{

//! @brief Topology relations for face definitions.
struct FaceRelations
{
  NCollection_LinearVector<BRepGraph_WireRefId> WireRefIds; //!< Wire references owned by this face
  NCollection_LinearVector<BRepGraph_FaceRefId>
    ParentFaceRefIds; //!< Upstream face references (compound hierarchy)
};

//! @brief Topology relations for wire definitions.
struct WireRelations
{
  NCollection_LinearVector<BRepGraph_CoEdgeId>  CoEdgeIds; //!< Coedge identifiers in this wire
  NCollection_LinearVector<BRepGraph_WireRefId> ParentWireRefIds; //!< Upstream wire references
};

//! @brief Topology relations for edge definitions.
struct EdgeRelations
{
  NCollection_LinearVector<BRepGraph_CoEdgeId> CoEdgeIds; //!< Coedge identifiers using this edge
};

//! @brief Topology relations for shell definitions.
struct ShellRelations
{
  NCollection_LinearVector<BRepGraph_FaceRefId>  FaceRefIds; //!< Face references in this shell
  NCollection_LinearVector<BRepGraph_ShellRefId> ParentShellRefIds; //!< Upstream shell references
};

//! @brief Topology relations for solid definitions.
struct SolidRelations
{
  NCollection_LinearVector<BRepGraph_ShellRefId> ShellRefIds; //!< Shell references in this solid
  NCollection_LinearVector<BRepGraph_SolidRefId> ParentSolidRefIds; //!< Upstream solid references
};

//! @brief Topology relations for compound definitions.
struct CompoundRelations
{
  NCollection_LinearVector<BRepGraph_ChildRefId> ChildRefIds; //!< Child references in this compound
};

//! @brief Topology relations for compsolid definitions.
struct CompSolidRelations
{
  NCollection_LinearVector<BRepGraph_SolidRefId>
    SolidRefIds; //!< Solid references in this compsolid
};

//! @brief Topology relations for vertex definitions.
struct VertexRelations
{
  NCollection_LinearVector<BRepGraph_EdgeId> EdgeIds; //!< Edge identifiers sharing this vertex
};

//! @brief Topology relations for product definitions.
struct ProductRelations
{
  NCollection_LinearVector<BRepGraph_OccurrenceRefId>
    OccurrenceRefIds; //!< Occurrence references under this product
};

//! @brief Topology relations for occurrence definitions.
struct OccurrenceRelations
{
  NCollection_LinearVector<BRepGraph_OccurrenceRefId>
    ParentOccurrenceRefIds; //!< Upstream occurrence references
};

} // namespace BRepGraphInc

#endif // _BRepGraphInc_Relations_HeaderFile
