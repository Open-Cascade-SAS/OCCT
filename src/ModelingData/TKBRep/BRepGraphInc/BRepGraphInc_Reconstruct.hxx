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

#ifndef _BRepGraphInc_Reconstruct_HeaderFile
#define _BRepGraphInc_Reconstruct_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>

#include <NCollection_DataMap.hxx>

class BRepGraphInc_Storage;

//! @brief Reconstruct TopoDS shapes from incidence-table storage.
//!
//! Converts BRepGraphInc_Storage entity data back into TopoDS shapes.
//! Supports single-node and cached multi-face reconstruction with
//! shared edge/vertex reuse via the Cache map.
class BRepGraphInc_Reconstruct
{
public:
  DEFINE_STANDARD_ALLOC

  //! Shared cache for edge/vertex shapes during multi-face reconstruction.
  using Cache = NCollection_DataMap<BRepGraph_NodeId, TopoDS_Shape>;

  //! Reconstruct a TopoDS_Shape from an entity node.
  //! Creates a local cache internally; shared vertices/edges are not reused
  //! across calls.
  //! @param[in] theStorage  incidence storage
  //! @param[in] theNode     entity node id
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(const BRepGraphInc_Storage& theStorage,
                                           const BRepGraph_NodeId      theNode);

  //! Reconstruct a TopoDS_Shape with a shared cache for sub-shape reuse.
  //! Vertices and edges already in theCache are returned directly.
  //! @param[in]     theStorage  incidence storage
  //! @param[in]     theNode     entity node id
  //! @param[in,out] theCache    shared cache for vertex/edge/face shapes
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(const BRepGraphInc_Storage& theStorage,
                                           const BRepGraph_NodeId      theNode,
                                           Cache&                      theCache);

  //! Reconstruct a face with shared edge/vertex cache for multi-face contexts.
  //! @param[in] theStorage    incidence storage
  //! @param[in] theFaceIdx    face entity index
  //! @param[in,out] theCache  shared cache for edge and vertex shapes
  //! @return reconstructed face shape
  static Standard_EXPORT TopoDS_Shape FaceWithCache(const BRepGraphInc_Storage& theStorage,
                                                    const int                   theFaceIdx,
                                                    Cache&                      theCache);

private:
  BRepGraphInc_Reconstruct() = delete;
};

#endif // _BRepGraphInc_Reconstruct_HeaderFile
