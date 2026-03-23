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

#ifndef _BRepGraphAlgo_BndLib_HeaderFile
#define _BRepGraphAlgo_BndLib_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraph;
class Bnd_Box;
class Bnd_OBB;

//! @brief Graph-based bounding box computation.
//!
//! Computes AABB and OBB bounding boxes from pre-built BRepGraph data,
//! avoiding TopExp_Explorer traversal and BRep_Tool adaptor construction.
//! For the common triangulation path, geometry handles and triangulations
//! are accessed directly from graph nodes.  For the rare geometry fallback,
//! face reconstruction from graph data is used.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   Bnd_Box aBox;
//!   BRepGraphAlgo_BndLib::Add(aGraph, aBox);
//! @endcode
class BRepGraphAlgo_BndLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! AABB of entire graph (all faces, free edges, free vertices).
  //! Uses triangulation if available, else geometry. Tolerances applied.
  //! @param[in] theGraph pre-built BRepGraph (IsDone() == true)
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  Standard_EXPORT static void Add(const BRepGraph& theGraph,
                                  Bnd_Box&         theBox,
                                  bool             theUseTriangulation = true);

  //! AABB of a single node (Face, Edge, Vertex, Shell, Solid, etc).
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in] theNode node identifier to compute bbox for
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  Standard_EXPORT static void Add(const BRepGraph& theGraph,
                                  BRepGraph_NodeId theNode,
                                  Bnd_Box&         theBox,
                                  bool             theUseTriangulation = true);

  //! Precise AABB using optimal curve/surface methods.
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  //! @param[in] theUseShapeTolerance if true, enlarge box by shape tolerances
  Standard_EXPORT static void AddOptimal(const BRepGraph& theGraph,
                                         Bnd_Box&         theBox,
                                         bool             theUseTriangulation  = true,
                                         bool             theUseShapeTolerance = false);

  //! Per-node precise AABB.
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in] theNode node identifier to compute bbox for
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  //! @param[in] theUseShapeTolerance if true, enlarge box by shape tolerances
  Standard_EXPORT static void AddOptimal(const BRepGraph& theGraph,
                                         BRepGraph_NodeId theNode,
                                         Bnd_Box&         theBox,
                                         bool             theUseTriangulation  = true,
                                         bool             theUseShapeTolerance = false);

  //! Oriented bounding box.
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in,out] theOBB oriented bounding box to fill
  //! @param[in] theIsTriangulationUsed if true, use triangulation
  //! @param[in] theIsOptimal if true, use optimal (tighter) computation
  //! @param[in] theIsShapeToleranceUsed if true, extend by shape tolerances
  Standard_EXPORT static void AddOBB(const BRepGraph& theGraph,
                                     Bnd_OBB&         theOBB,
                                     bool             theIsTriangulationUsed  = true,
                                     bool             theIsOptimal            = false,
                                     bool             theIsShapeToleranceUsed = true);

private:
  BRepGraphAlgo_BndLib() = delete;
};

#endif // _BRepGraphAlgo_BndLib_HeaderFile
