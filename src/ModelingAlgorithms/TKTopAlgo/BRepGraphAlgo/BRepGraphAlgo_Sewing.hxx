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

#ifndef _BRepGraphAlgo_Sewing_HeaderFile
#define _BRepGraphAlgo_Sewing_HeaderFile

#include <BRepGraph.hxx>

#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopoDS_Shape.hxx>

//! @brief Graph-based edge sewing algorithm (static API).
//!
//! BRepGraphAlgo_Sewing detects free (boundary) edges on a pre-built BRepGraph,
//! matches geometrically coincident free-edge pairs within the given tolerance,
//! and merges them in-place using BRepGraph mutation APIs.
//!
//! The algorithm follows these phases:
//! 0. Face Analysis (optional) - detect/remove small edges and faces
//! 1. Find Free Edges - boundary edge detection via face-reference counting
//! 2. Vertex Assembling - merge coincident free-edge vertices (union-find)
//! 3. Edge Cutting (optional) - split edges at T-vertex intersections
//! 4. Candidate Detection - BBox pre-filter + KDTree overlap detection
//! 5. Edge Matching - geometric validation + greedy best-score pairing
//! 6. Edge Merging - PCurve transfer, wire updates, tolerance merge
//! 7. Degenerate Conversion - convert remaining free degenerate wires
//! 8. Edge Processing - tolerance consistency + SameParameter
//! 9. Output - populate result diagnostics
//!
//! ## Typical usage (graph-level)
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(aCompound);
//!   auto aResult = BRepGraphAlgo_Sewing::Perform(aGraph, {.Tolerance = 1.0e-04});
//!   if (aResult.IsDone)
//!   {
//!     // graph is modified in-place; reconstruct shape if needed
//!   }
//! @endcode
//!
//! ## Convenience usage (shape-level)
//! @code
//!   TopoDS_Shape aResult = BRepGraphAlgo_Sewing::Sew(aShape, {.Tolerance = 1.0e-04});
//! @endcode
class BRepGraphAlgo_Sewing
{
public:
  DEFINE_STANDARD_ALLOC

  //! Configuration parameters for sewing.
  struct Options
  {
    double Tolerance         = 1.0e-06;              //!< Maximum distance for edge matching.
    bool   Cutting           = true;                 //!< Cut edges at T-vertex intersections.
    bool   SameParameterMode = true;                 //!< Enforce SameParameter on sewn edges.
    bool   NonManifoldMode   = false;                //!< Allow >2 faces per edge.
    bool   Parallel          = false;                //!< Enable parallel execution.
    bool   HistoryMode       = true;                 //!< Record history in the graph.
    bool   FaceAnalysis      = true;                 //!< Run face analysis before sewing (small-edge/face cleanup).
    bool   FloatingEdgesMode = false;                //!< Include edges with 0 adjacent faces.
    bool   LocalTolerancesMode = false;              //!< Adaptive tolerance: WorkTol = Tolerance + tolEdge1 + tolEdge2.
    double MinTolerance      = 0.0;                  //!< Minimum edge length threshold; 0 = auto (Tolerance * 1e-4).
    double MaxTolerance      = Precision::Infinite(); //!< Upper bound for merge tolerance.
  };

  //! Diagnostic result from a sewing operation.
  struct Result
  {
    bool IsDone            = false; //!< True if sewing completed successfully.
    int  NbFreeEdgesBefore = 0;     //!< Free edges detected before sewing.
    int  NbFreeEdgesAfter  = 0;     //!< Free edges remaining after sewing.
    int  NbSewnEdges       = 0;     //!< Edge pairs that were successfully sewn.
    int  NbMultipleEdges   = 0;     //!< Edges shared by >2 faces.
    int  NbDegeneratedEdges     = 0; //!< Degenerate edges detected or created.
    int  NbDeletedFaces         = 0; //!< Small faces removed by face analysis.
    int  NbRejectedByTolerance  = 0; //!< Candidate pairs rejected by MaxTolerance.

    NCollection_Vector<BRepGraph_NodeId> FreeEdges;        //!< Remaining free edges after sewing.
    NCollection_Vector<BRepGraph_NodeId> MultipleEdges;    //!< Edges shared by >2 faces.
    NCollection_Vector<BRepGraph_NodeId> SewnEdgePairs;    //!< Keep-edge IDs of sewn pairs.
    NCollection_Vector<BRepGraph_NodeId> DegeneratedEdges; //!< Degenerate edges found or created.
    NCollection_Vector<BRepGraph_NodeId> DeletedFaces;     //!< Faces removed by face analysis.
  };

  //! Primary API: sew free edges on a pre-built graph (in-place).
  //! Graph must be built (IsDone() == true). Uses default options.
  //! @param[in,out] theGraph the graph to modify
  //! @return result with diagnostics
  static Standard_EXPORT Result Perform(BRepGraph& theGraph);

  //! Primary API: sew free edges on a pre-built graph (in-place).
  //! Graph must be built (IsDone() == true).
  //! @param[in,out] theGraph   the graph to modify
  //! @param[in]     theOptions sewing parameters
  //! @return result with diagnostics
  static Standard_EXPORT Result Perform(BRepGraph&     theGraph,
                                        const Options& theOptions);

  //! Convenience: sew a shape, return the result shape.
  //! Builds graph internally, calls Perform(), reconstructs, encodes regularity.
  //! Uses default options.
  //! @param[in] theShape root shape (Face, Shell, Compound, etc.)
  //! @return sewn result shape (empty if failed)
  static Standard_EXPORT TopoDS_Shape Sew(const TopoDS_Shape& theShape);

  //! Convenience: sew a shape, return the result shape.
  //! Builds graph internally, calls Perform(), reconstructs, encodes regularity.
  //! @param[in] theShape   root shape (Face, Shell, Compound, etc.)
  //! @param[in] theOptions sewing parameters
  //! @return sewn result shape (empty if failed)
  static Standard_EXPORT TopoDS_Shape Sew(const TopoDS_Shape& theShape,
                                          const Options&       theOptions);

private:
  BRepGraphAlgo_Sewing() = delete;
};

#endif // _BRepGraphAlgo_Sewing_HeaderFile
