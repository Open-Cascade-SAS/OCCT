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

#ifndef _BRepGraphAlgo_Deduplicate_HeaderFile
#define _BRepGraphAlgo_Deduplicate_HeaderFile

#include <BRepGraph.hxx>

#include <BRepGraph_NodeId.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Deep geometry deduplication algorithm over an existing BRepGraph.
//!
//! This algorithm canonicalizes deep-equal geometry references (surfaces and
//! 3D curves) using GeomHash hashers. It updates face/edge definition links to
//! canonical geometry nodes and can record lineage in graph history.
//!
//! First implementation intentionally does not merge edge/face definitions yet.
class BRepGraphAlgo_Deduplicate
{
public:
  DEFINE_STANDARD_ALLOC

  //! Configuration for graph deduplication run.
  struct Options
  {
    bool   AnalyzeOnly       = false;
    bool   HistoryMode       = true;
    bool   MergeEntitiesWhenSafe = false;
    double CompTolerance     = Precision::Angular();
    double HashTolerance     = Precision::Confusion();
  };

  //! Result counters for diagnostics and tests.
  struct Result
  {
    int  NbCanonicalSurfaces = 0;
    int  NbCanonicalCurves   = 0;
    int  NbSurfaceRewrites   = 0;
    int  NbCurveRewrites     = 0;
    int  NbNullifiedSurfaces = 0;
    int  NbNullifiedCurves   = 0;
    int  NbHistoryRecords    = 0;
    bool IsEntityMergeApplied   = false;

    //! Topology definition merge counters (active when MergeEntitiesWhenSafe = true).
    int NbMergedVertices = 0;
    int NbMergedEdges    = 0;
    int NbMergedWires    = 0;
    int NbMergedFaces    = 0;

    NCollection_Vector<BRepGraph_FaceId> AffectedFaces; //!< Faces whose SurfNodeId changed.
    NCollection_Vector<BRepGraph_EdgeId> AffectedEdges; //!< Edges whose CurveNodeId changed.
  };

  //! Run deduplication on a built graph.
  //! @param[in,out] theGraph graph to update
  //! @return dedup statistics
  [[nodiscard]] Standard_EXPORT static Result Perform(BRepGraph& theGraph);

  //! Run deduplication on a built graph.
  //! @param[in,out] theGraph graph to update
  //! @param[in] theOptions dedup configuration
  //! @return dedup statistics
  [[nodiscard]] Standard_EXPORT static Result Perform(BRepGraph&     theGraph,
                                                      const Options& theOptions);

private:
  BRepGraphAlgo_Deduplicate() = delete;
};

#endif // _BRepGraphAlgo_Deduplicate_HeaderFile
