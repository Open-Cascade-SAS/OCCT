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

#include <BRepGraph_NodeId.hxx>
#include <NCollection_Vector.hxx>

class BRepGraph;

//! @brief Non-owning view over a connected component of BRepGraph.
//!
//! Stores per-kind typed definition id sets referencing the parent graph's
//! entity vectors. Created by BRepGraph_Analyze::Decompose() to partition
//! a graph into independent connected components for parallel processing.
class BRepGraph_SubGraph
{
public:
  const BRepGraph* ParentGraph() const { return myParent; }

  //! Typed definition id sets.
  const NCollection_Vector<BRepGraph_SolidId>& SolidDefIds() const { return mySolidDefIds; }

  const NCollection_Vector<BRepGraph_ShellId>& ShellDefIds() const { return myShellDefIds; }

  const NCollection_Vector<BRepGraph_FaceId>& FaceDefIds() const { return myFaceDefIds; }

  const NCollection_Vector<BRepGraph_WireId>& WireDefIds() const { return myWireDefIds; }

  const NCollection_Vector<BRepGraph_EdgeId>& EdgeDefIds() const { return myEdgeDefIds; }

  const NCollection_Vector<BRepGraph_VertexId>& VertexDefIds() const { return myVertexDefIds; }

  int NbTopoNodes() const
  {
    return mySolidDefIds.Length() + myShellDefIds.Length() + myFaceDefIds.Length()
           + myWireDefIds.Length() + myEdgeDefIds.Length() + myVertexDefIds.Length();
  }

private:
  friend class BRepGraph;
  friend class BRepGraph_Analyze;

  const BRepGraph* myParent = nullptr;

  NCollection_Vector<BRepGraph_SolidId>  mySolidDefIds;
  NCollection_Vector<BRepGraph_ShellId>  myShellDefIds;
  NCollection_Vector<BRepGraph_FaceId>   myFaceDefIds;
  NCollection_Vector<BRepGraph_WireId>   myWireDefIds;
  NCollection_Vector<BRepGraph_EdgeId>   myEdgeDefIds;
  NCollection_Vector<BRepGraph_VertexId> myVertexDefIds;
};

#endif // _BRepGraph_SubGraph_HeaderFile
