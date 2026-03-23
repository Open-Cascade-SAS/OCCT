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

#include <NCollection_Vector.hxx>

class BRepGraph;

//! A non-owning view over a connected component of BRepGraph.
//!
//! Stores per-kind definition index sets referencing the parent graph's vectors.
class BRepGraph_SubGraph
{
public:
  const BRepGraph* ParentGraph() const { return myParent; }

  //! Definition index sets.
  const NCollection_Vector<int>& SolidDefIndices()  const { return mySolidDefIndices; }
  const NCollection_Vector<int>& ShellDefIndices()  const { return myShellDefIndices; }
  const NCollection_Vector<int>& FaceDefIndices()   const { return myFaceDefIndices; }
  const NCollection_Vector<int>& WireDefIndices()   const { return myWireDefIndices; }
  const NCollection_Vector<int>& EdgeDefIndices()   const { return myEdgeDefIndices; }
  const NCollection_Vector<int>& VertexDefIndices() const { return myVertexDefIndices; }

  int NbTopoNodes() const
  {
    return mySolidDefIndices.Length()  + myShellDefIndices.Length()
         + myFaceDefIndices.Length()   + myWireDefIndices.Length()
         + myEdgeDefIndices.Length()   + myVertexDefIndices.Length();
  }

private:
  friend class BRepGraph;
  friend class BRepGraph_Analyze;

  const BRepGraph* myParent = nullptr;

  NCollection_Vector<int> mySolidDefIndices;
  NCollection_Vector<int> myShellDefIndices;
  NCollection_Vector<int> myFaceDefIndices;
  NCollection_Vector<int> myWireDefIndices;
  NCollection_Vector<int> myEdgeDefIndices;
  NCollection_Vector<int> myVertexDefIndices;
};

#endif // _BRepGraph_SubGraph_HeaderFile
