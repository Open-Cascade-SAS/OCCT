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

#ifndef _BRepGraphInc_Load_HeaderFile
#define _BRepGraphInc_Load_HeaderFile

#include <cstdint>

//! Internal storage-side types for fixed-size indexed load preparation.
namespace BRepGraphInc_Load
{

//! Final section counts needed to prepare `BRepGraphInc_Storage` for indexed load.
struct Counts
{
  uint32_t NbVertices               = 0; //!< Number of `VertexDef` slots.
  uint32_t NbEdges                  = 0; //!< Number of `EdgeDef` slots.
  uint32_t NbCoEdges                = 0; //!< Number of `CoEdgeDef` slots.
  uint32_t NbWires                  = 0; //!< Number of `WireDef` slots.
  uint32_t NbFaces                  = 0; //!< Number of `FaceDef` slots.
  uint32_t NbShells                 = 0; //!< Number of `ShellDef` slots.
  uint32_t NbSolids                 = 0; //!< Number of `SolidDef` slots.
  uint32_t NbCompounds              = 0; //!< Number of `CompoundDef` slots.
  uint32_t NbCompSolids             = 0; //!< Number of `CompSolidDef` slots.
  uint32_t NbProducts               = 0; //!< Number of `ProductDef` slots.
  uint32_t NbOccurrences            = 0; //!< Number of `OccurrenceDef` slots.
  uint32_t NbShellRefs              = 0; //!< Number of `ShellRef` slots.
  uint32_t NbFaceRefs               = 0; //!< Number of `FaceRef` slots.
  uint32_t NbWireRefs               = 0; //!< Number of `WireRef` slots.
  uint32_t NbVertexRefs             = 0; //!< Number of `VertexRef` slots.
  uint32_t NbSolidRefs              = 0; //!< Number of `SolidRef` slots.
  uint32_t NbChildRefs              = 0; //!< Number of `ChildRef` slots.
  uint32_t NbOccurrenceRefs         = 0; //!< Number of `OccurrenceRef` slots.
  uint32_t NbFaceSurfaceReps        = 0; //!< Number of `FaceSurfaceRep` slots.
  uint32_t NbEdgeCurve3DReps        = 0; //!< Number of `EdgeCurve3DRep` slots.
  uint32_t NbCoEdgeCurve2DReps      = 0; //!< Number of `CoEdgeCurve2DRep` slots.
  uint32_t NbFaceTriangulationReps  = 0; //!< Number of `FaceTriangulationRep` slots.
  uint32_t NbEdgePolygon3DReps      = 0; //!< Number of `EdgePolygon3DRep` slots.
  uint32_t NbCoEdgePolygon2DReps    = 0; //!< Number of `CoEdgePolygon2DRep` slots.
  uint32_t NbCoEdgePolygonOnTriReps = 0; //!< Number of `CoEdgePolygonOnTriRep` slots.
  uint32_t NbRootProducts           = 0; //!< Number of root product ids outside storage tables.
};

} // namespace BRepGraphInc_Load

#endif // _BRepGraphInc_Load_HeaderFile
