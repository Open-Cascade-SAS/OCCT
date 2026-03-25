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

#ifndef _BRepGraphAlgo_SameParameter_HeaderFile
#define _BRepGraphAlgo_SameParameter_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_NodeId.hxx>

#include <NCollection_IndexedMap.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Graph-level SameParameter enforcement utility.
//!
//! Works purely on graph data (Curve3d, PCurves, Surfaces, tolerances) --
//! no TopoDS objects are accessed. For each edge and each of its PCurves,
//! the 3D curve is sampled at uniform parameters and compared against the
//! PCurve evaluated on the face surface. If the maximum deviation exceeds
//! the edge tolerance, the tolerance is updated accordingly.
class BRepGraphAlgo_SameParameter
{
public:
  DEFINE_STANDARD_ALLOC

  //! Diagnostic result from SameParameter enforcement.
  struct Result
  {
    int NbC0Fallbacks = 0; //!< Edges where C0 BSpline PCurve could not be promoted to C1.
    int NbApproxFallbacks =
      0; //!< Edges where Approx_SameParameter failed and SameRange fallback was used.
  };

  //! Enforce SameParameter on a single edge.
  //! Checks each PCurve against the 3D curve; adjusts tolerance if needed.
  //! @param[in,out] theGraph     the graph containing the edge
  //! @param[in]     theEdgeId    edge node to process
  //! @param[in]     theTolerance reference tolerance for pass/fail
  //! @return true if all PCurves are within tolerance
  [[nodiscard]] static Standard_EXPORT bool Enforce(BRepGraph&             theGraph,
                                                    const BRepGraph_EdgeId theEdgeId,
                                                    const double           theTolerance);

  //! Enforce SameParameter on a set of edges.
  //! Each edge is independent; mutations are batched under a single MutationGuard.
  //! @param[in,out] theGraph     the graph containing the edges
  //! @param[in]     theEdgeIds   edge definition identifiers to process
  //! @param[in]     theTolerance reference tolerance
  //! @return diagnostic counters
  [[nodiscard]] static Standard_EXPORT Result
    Perform(BRepGraph&                                      theGraph,
            const NCollection_IndexedMap<BRepGraph_EdgeId>& theEdgeIds,
            const double                                    theTolerance);

private:
  BRepGraphAlgo_SameParameter() = delete;
};

#endif // _BRepGraphAlgo_SameParameter_HeaderFile
