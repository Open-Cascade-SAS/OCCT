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

#ifndef _BRepGraphCheck_Analyzer_HeaderFile
#define _BRepGraphCheck_Analyzer_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraphCheck_Report.hxx>
#include <Standard_DefineAlloc.hxx>

//! Graph-based shape validity checker.
//!
//! Takes a const reference to a pre-built BRepGraph and runs validation
//! checks on all topology nodes, populating a BRepGraphCheck_Report with
//! structured diagnostics.  Supports parallel execution and incremental
//! per-node checking.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   BRepGraphCheck_Analyzer anAnalyzer(aGraph);
//!   anAnalyzer.Perform();
//!   if (!anAnalyzer.IsValid())
//!   {
//!     for (int i = 0; i < anAnalyzer.Report().Issues().Length(); ++i)
//!       // handle issue...
//!   }
//! @endcode
class BRepGraphCheck_Analyzer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct analyzer for the given graph.
  //! @param[in] theGraph pre-built BRepGraph (must have IsDone() == true)
  Standard_EXPORT explicit BRepGraphCheck_Analyzer(const BRepGraph& theGraph);

  //! Enable/disable geometric controls (SameParameter, curve deviation, etc.).
  //! Default: true.
  Standard_EXPORT void SetGeometricControls(bool theVal);

  //! Enable/disable parallel execution of checks.
  //! Default: false.
  Standard_EXPORT void SetParallel(bool theVal);

  //! Enable/disable exact method for BRepLib_ValidateEdge.
  //! Default: false.
  Standard_EXPORT void SetExactMethod(bool theVal);

  //! Run all validation checks and populate the report.
  Standard_EXPORT void Perform();

  //! Incremental: check a single vertex definition.
  //! @param[in] theVertex typed vertex definition identifier
  Standard_EXPORT void CheckVertex(const BRepGraph_VertexId theVertex);

  //! Incremental: check a single edge definition.
  //! @param[in] theEdge typed edge definition identifier
  Standard_EXPORT void CheckEdge(const BRepGraph_EdgeId theEdge);

  //! Incremental: check a wire on a face.
  //! @param[in] theWire typed wire definition identifier
  //! @param[in] theFace typed face definition identifier (invalid for standalone wire check)
  Standard_EXPORT void CheckWire(const BRepGraph_WireId theWire, const BRepGraph_FaceId theFace);

  //! Incremental: check a single face definition.
  //! @param[in] theFace typed face definition identifier
  Standard_EXPORT void CheckFace(const BRepGraph_FaceId theFace);

  //! Incremental: check a single shell definition.
  //! @param[in] theShell typed shell definition identifier
  Standard_EXPORT void CheckShell(const BRepGraph_ShellId theShell);

  //! Incremental: check a single solid definition.
  //! @param[in] theSolid typed solid definition identifier
  Standard_EXPORT void CheckSolid(const BRepGraph_SolidId theSolid);

  //! True if no Error or Fatal issues exist.
  Standard_EXPORT bool IsValid() const;

  //! True if no Error or Fatal issues exist for a specific node.
  //! @param[in] theNode graph node to query
  Standard_EXPORT bool IsValid(BRepGraph_NodeId theNode) const;

  //! Access the accumulated report.
  Standard_EXPORT const BRepGraphCheck_Report& Report() const;

private:
  const BRepGraph*      myGraph;
  BRepGraphCheck_Report myReport;
  bool                  myGeomControls = true;
  bool                  myIsParallel   = false;
  bool                  myIsExact      = false;
};

#endif // _BRepGraphCheck_Analyzer_HeaderFile
