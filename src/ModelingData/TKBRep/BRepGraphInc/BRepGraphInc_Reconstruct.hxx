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
#include <NCollection_IncAllocator.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>

class BRepGraph;

//! @brief Backend reconstruction helpers over incidence-table storage.
//!
//! Converts BRepGraphInc_Storage entity data back into TopoDS shapes.
//! This class is part of the BRepGraphInc backend; external callers should
//! prefer BRepGraph::Shapes() so reconstruction stays behind the facade.
//! Supports single-node and cached multi-face reconstruction with
//! shared edge/vertex reuse via the Cache.
class BRepGraphInc_Reconstruct
{
public:
  DEFINE_STANDARD_ALLOC

  //! Per-Kind dense vector cache for O(1) shape lookup by entity index.
  //! Replaces NCollection_DataMap to eliminate hash/equality overhead.
  struct Cache
  {
    //! Number of Kind slots used by BRepGraph_NodeId dense-kind indexing.
    //! Includes topology kinds, assembly kinds, and the reserved gap at kind 9.
    static constexpr int THE_KIND_COUNT        = BRepGraph_NodeId::THE_KIND_COUNT;
    static constexpr int THE_DEFAULT_INCREMENT = 32;

    occ::handle<NCollection_IncAllocator>  myAllocator;
    occ::handle<NCollection_IncAllocator>  myTempAllocator;
    NCollection_DynamicArray<TopoDS_Shape> myKinds[THE_KIND_COUNT];
    int                                    myTempScopeDepth = 0;

    struct TempScope
    {
      Cache& myCache;

      explicit TempScope(Cache& theCache);
      ~TempScope();
    };

    Cache();

    //! Seek a cached shape. Returns nullptr if not yet cached.
    [[nodiscard]] Standard_EXPORT const TopoDS_Shape* Seek(const BRepGraph_NodeId theNode) const;

    //! Bind a reconstructed shape to a node. Grows the vector as needed.
    Standard_EXPORT void Bind(const BRepGraph_NodeId theNode, const TopoDS_Shape& theShape);

    //! Check if a node is already cached.
    [[nodiscard]] bool IsBound(const BRepGraph_NodeId theNode) const
    {
      return Seek(theNode) != nullptr;
    }
  };

  //! Reconstruct a TopoDS_Shape from an entity node.
  //! Creates a local cache internally; shared vertices/edges are not reused
  //! across calls.
  //! @param[in] theGraph  graph owning the storage and caches
  //! @param[in] theNode   entity node id
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(BRepGraph& theGraph, const BRepGraph_NodeId theNode);

  //! Reconstruct a TopoDS_Shape with a shared cache for sub-shape reuse.
  //! Vertices and edges already in theCache are returned directly.
  //! @param[in]     theGraph  graph owning the storage and caches
  //! @param[in]     theNode   entity node id
  //! @param[in,out] theCache  shared cache for vertex/edge/face shapes
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(BRepGraph&             theGraph,
                                           const BRepGraph_NodeId theNode,
                                           Cache&                 theCache);

  //! Reconstruct a face with shared edge/vertex cache for multi-face contexts.
  //! @param[in]     theGraph   graph owning the storage and caches
  //! @param[in]     theFaceId  face entity id
  //! @param[in,out] theCache   shared cache for edge and vertex shapes
  //! @return reconstructed face shape
  static Standard_EXPORT TopoDS_Shape FaceWithCache(BRepGraph&             theGraph,
                                                    const BRepGraph_FaceId theFaceId,
                                                    Cache&                 theCache);

  BRepGraphInc_Reconstruct() = delete;
};

#endif // _BRepGraphInc_Reconstruct_HeaderFile
