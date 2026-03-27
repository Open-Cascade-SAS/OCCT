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
  const NCollection_Vector<BRepGraph_SolidId>& SolidEntityIds() const { return mySolidEntityIds; }

  const NCollection_Vector<BRepGraph_ShellId>& ShellEntityIds() const { return myShellEntityIds; }

  const NCollection_Vector<BRepGraph_FaceId>& FaceEntityIds() const { return myFaceEntityIds; }

  const NCollection_Vector<BRepGraph_WireId>& WireEntityIds() const { return myWireEntityIds; }

  const NCollection_Vector<BRepGraph_EdgeId>& EdgeEntityIds() const { return myEdgeEntityIds; }

  const NCollection_Vector<BRepGraph_VertexId>& VertexEntityIds() const { return myVertexEntityIds; }

  int NbTopoNodes() const
  {
    return mySolidEntityIds.Length() + myShellEntityIds.Length() + myFaceEntityIds.Length()
           + myWireEntityIds.Length() + myEdgeEntityIds.Length() + myVertexEntityIds.Length();
  }

private:
  friend class BRepGraph;
  friend class BRepGraph_Analyze;

  const BRepGraph* myParent = nullptr;

  NCollection_Vector<BRepGraph_SolidId>  mySolidEntityIds;
  NCollection_Vector<BRepGraph_ShellId>  myShellEntityIds;
  NCollection_Vector<BRepGraph_FaceId>   myFaceEntityIds;
  NCollection_Vector<BRepGraph_WireId>   myWireEntityIds;
  NCollection_Vector<BRepGraph_EdgeId>   myEdgeEntityIds;
  NCollection_Vector<BRepGraph_VertexId> myVertexEntityIds;
};

#endif // _BRepGraph_SubGraph_HeaderFile
