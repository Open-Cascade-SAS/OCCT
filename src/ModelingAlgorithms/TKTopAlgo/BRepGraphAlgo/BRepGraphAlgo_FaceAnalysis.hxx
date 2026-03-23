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

#ifndef _BRepGraphAlgo_FaceAnalysis_HeaderFile
#define _BRepGraphAlgo_FaceAnalysis_HeaderFile

#include <BRepGraph.hxx>

#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Static utility for face analysis: detect and clean up small edges and faces.
//!
//! Analyzes all faces in a BRepGraph and:
//! - Detects small (degenerate) edges via a 5-point compactness criterion
//! - Marks small edges as degenerate (clears 3D curve, merges endpoints)
//! - Removes faces where all edges are degenerate (marks IsRemoved)
//! - Averages coordinates of merged vertex groups
//!
//! Ported from BRepBuilderAPI_Sewing::FaceAnalysis() to operate on BRepGraph.
class BRepGraphAlgo_FaceAnalysis
{
public:
  DEFINE_STANDARD_ALLOC

  //! Configuration for face analysis.
  struct Options
  {
    double MinTolerance = 0.0;  //!< Threshold for small-edge detection; 0 = auto.
    bool   Parallel     = false; //!< Enable parallel execution.
  };

  //! Result of face analysis.
  struct Result
  {
    bool IsDone = false; //!< True if analysis completed successfully.
    NCollection_Vector<BRepGraph_NodeId> DegeneratedEdges; //!< Edges marked degenerate.
    NCollection_Vector<BRepGraph_NodeId> DeletedFaces;     //!< Faces removed (all edges degenerate).
    int NbSmallEdges   = 0; //!< Number of small edges detected.
    int NbDeletedFaces = 0; //!< Number of deleted faces.
  };

  //! Perform face analysis on a pre-built graph.
  //! @param[in,out] theGraph   the graph to analyze and modify
  //! @param[in]     theOptions analysis parameters
  //! @return result with diagnostics
  static Standard_EXPORT Result Perform(BRepGraph& theGraph, const Options& theOptions);

private:
  BRepGraphAlgo_FaceAnalysis() = delete;
};

#endif // _BRepGraphAlgo_FaceAnalysis_HeaderFile
