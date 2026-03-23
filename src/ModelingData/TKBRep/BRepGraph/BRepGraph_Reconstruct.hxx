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

#ifndef _BRepGraph_Reconstruct_HeaderFile
#define _BRepGraph_Reconstruct_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_UsageId.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>

class BRepGraph;

//! @brief Static helper that reconstructs TopoDS shapes from graph definitions.
//!
//! BRepGraph_Reconstruct extracts the shape reconstruction logic out of
//! BRepGraph itself, keeping the graph class focused on queries and mutation.
//! It is declared as a friend of BRepGraph to access private storage.
class BRepGraph_Reconstruct
{
public:
  DEFINE_STANDARD_ALLOC

  //! Reconstruct a TopoDS_Shape from a graph definition node.
  //! @param[in] theGraph the graph to reconstruct from
  //! @param[in] theNode  definition node id
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Node(const BRepGraph&  theGraph,
                                           BRepGraph_NodeId  theNode);

  //! Reconstruct a face with shared edge/vertex cache for multi-face contexts.
  //! @param[in] theGraph      the graph to reconstruct from
  //! @param[in] theNode       face definition node id
  //! @param[in,out] theEdgeCache  shared cache for edge and vertex shapes
  //! @return reconstructed face shape
  static Standard_EXPORT TopoDS_Shape FaceWithCache(const BRepGraph&  theGraph,
                                                    BRepGraph_NodeId  theNode,
                                                    NCollection_DataMap<BRepGraph_NodeId,
                                                                       TopoDS_Shape>& theEdgeCache);

  //! Reconstruct a TopoDS_Shape from a usage node.
  //! @param[in] theGraph  the graph to reconstruct from
  //! @param[in] theUsage  usage node id
  //! @return reconstructed shape
  static Standard_EXPORT TopoDS_Shape Usage(const BRepGraph&   theGraph,
                                            BRepGraph_UsageId  theUsage);

private:
  BRepGraph_Reconstruct() = delete;
};

#endif // _BRepGraph_Reconstruct_HeaderFile
