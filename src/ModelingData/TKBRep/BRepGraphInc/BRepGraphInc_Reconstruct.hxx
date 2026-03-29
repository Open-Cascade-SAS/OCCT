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
#include <NCollection_Vector.hxx>

class BRepGraphInc_Storage;
class BRepGraph_ParamLayer;
class BRepGraph_RegularityLayer;

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
    //! Number of Kind slots (Vertex=5, Edge=4, Wire=3, Face=2, Shell=1, Solid=0,
    //! Compound=6, CompSolid=7, CoEdge=8).
    static constexpr int THE_KIND_COUNT =
      static_cast<int>(BRepGraph_NodeId::Kind::Occurrence) + 1;

    NCollection_Vector<TopoDS_Shape> myKinds[THE_KIND_COUNT];

    //! Seek a cached shape. Returns nullptr if not yet cached.
    const TopoDS_Shape* Seek(const BRepGraph_NodeId theNode) const
    {
      const int aKindIdx = static_cast<int>(theNode.NodeKind);
      if (aKindIdx < 0 || aKindIdx >= THE_KIND_COUNT)
        return nullptr;
      const NCollection_Vector<TopoDS_Shape>& aVec = myKinds[aKindIdx];
      if (theNode.Index >= aVec.Length())
        return nullptr;
      const TopoDS_Shape& aShape = aVec.Value(theNode.Index);
      return aShape.IsNull() ? nullptr : &aShape;
    }

    //! Bind a reconstructed shape to a node. Grows the vector as needed.
    void Bind(const BRepGraph_NodeId theNode, const TopoDS_Shape& theShape)
    {
      const int aKindIdx = static_cast<int>(theNode.NodeKind);
      if (aKindIdx < 0 || aKindIdx >= THE_KIND_COUNT)
        return;
      NCollection_Vector<TopoDS_Shape>& aVec = myKinds[aKindIdx];
      while (theNode.Index >= aVec.Length())
        aVec.Append(TopoDS_Shape());
      aVec.ChangeValue(theNode.Index) = theShape;
    }

    //! Check if a node is already cached.
    bool IsBound(const BRepGraph_NodeId theNode) const
    {
      return Seek(theNode) != nullptr;
    }
  };

  //! Reconstruct a TopoDS_Shape from an entity node.
  //! Creates a local cache internally; shared vertices/edges are not reused
  //! across calls.
  //! @param[in] theStorage  incidence storage
  //! @param[in] theNode     entity node id
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(const BRepGraphInc_Storage&          theStorage,
                                           const BRepGraph_NodeId               theNode,
                                           const BRepGraph_ParamLayer*          theParams = nullptr,
                                           const BRepGraph_RegularityLayer*     theRegularities = nullptr);

  //! Reconstruct a TopoDS_Shape with a shared cache for sub-shape reuse.
  //! Vertices and edges already in theCache are returned directly.
  //! @param[in]     theStorage  incidence storage
  //! @param[in]     theNode     entity node id
  //! @param[in,out] theCache    shared cache for vertex/edge/face shapes
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(const BRepGraphInc_Storage&          theStorage,
                                           const BRepGraph_NodeId               theNode,
                                           Cache&                               theCache,
                                           const BRepGraph_ParamLayer*          theParams = nullptr,
                                           const BRepGraph_RegularityLayer*     theRegularities = nullptr);

  //! Reconstruct a face with shared edge/vertex cache for multi-face contexts.
  //! @param[in] theStorage    incidence storage
  //! @param[in] theFaceIdx    face entity index
  //! @param[in,out] theCache  shared cache for edge and vertex shapes
  //! @return reconstructed face shape
  static Standard_EXPORT TopoDS_Shape FaceWithCache(const BRepGraphInc_Storage&          theStorage,
                                                    const int                            theFaceIdx,
                                                    Cache&                               theCache,
                                                    const BRepGraph_ParamLayer*          theParams = nullptr,
                                                    const BRepGraph_RegularityLayer*     theRegularities = nullptr);

private:
  BRepGraphInc_Reconstruct() = delete;
};

#endif // _BRepGraphInc_Reconstruct_HeaderFile
