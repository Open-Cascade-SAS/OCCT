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

#ifndef _BRepGraph_SubGraph_HeaderFile
#define _BRepGraph_SubGraph_HeaderFile

#include <NCollection_Sequence.hxx>

class BRepGraph;

//! A non-owning view over a connected component of BRepGraph.
//!
//! Stores per-kind index sets referencing the parent graph's node vectors.
//! Produced by BRepGraph::Decompose() and consumed by
//! BRepGraph::ParallelForEachFace/Edge().
//!
//! Thread safety: read-only access to the parent graph through a SubGraph
//! is safe.  The caller must ensure the parent graph outlives all SubGraphs.
class BRepGraph_SubGraph
{
public:
  //! Parent graph (non-owning pointer; must outlive this SubGraph).
  const BRepGraph* ParentGraph() const { return myParent; }

  //! Index sets -- each contains indices into the parent's per-kind vectors.
  const NCollection_Sequence<int>& SolidIndices()  const
  { return mySolidIndices; }
  const NCollection_Sequence<int>& ShellIndices()  const
  { return myShellIndices; }
  const NCollection_Sequence<int>& FaceIndices()   const
  { return myFaceIndices; }
  const NCollection_Sequence<int>& WireIndices()   const
  { return myWireIndices; }
  const NCollection_Sequence<int>& EdgeIndices()   const
  { return myEdgeIndices; }
  const NCollection_Sequence<int>& VertexIndices() const
  { return myVertexIndices; }

  //! Total number of topology nodes in this connected component.
  int NbTopoNodes() const
  {
    return mySolidIndices.Length()  + myShellIndices.Length()
         + myFaceIndices.Length()   + myWireIndices.Length()
         + myEdgeIndices.Length()   + myVertexIndices.Length();
  }

private:
  friend class BRepGraph;

  const BRepGraph*              myParent = nullptr;
  NCollection_Sequence<int>     mySolidIndices;
  NCollection_Sequence<int>     myShellIndices;
  NCollection_Sequence<int>     myFaceIndices;
  NCollection_Sequence<int>     myWireIndices;
  NCollection_Sequence<int>     myEdgeIndices;
  NCollection_Sequence<int>     myVertexIndices;
};

#endif // _BRepGraph_SubGraph_HeaderFile
