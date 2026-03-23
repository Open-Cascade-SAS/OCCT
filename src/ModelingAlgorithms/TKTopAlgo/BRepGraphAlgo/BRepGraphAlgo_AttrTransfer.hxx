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

#ifndef _BRepGraphAlgo_AttrTransfer_HeaderFile
#define _BRepGraphAlgo_AttrTransfer_HeaderFile

#include <BRepGraph.hxx>

//! @brief Attribute transfer utility for BRepGraph history chains.
//!
//! Propagates user attributes from original topology nodes to their
//! replacement nodes as recorded by the graph's history subsystem.
//! This enables XDE-style attribute re-assignment after graph-modifying
//! operations (sewing, boolean ops, etc.).
//!
//! Only topology nodes (Face, Edge, Wire, etc.) carry user attributes.
//! Geometry nodes (Surface, Curve, PCurve) are skipped automatically.
class BRepGraphAlgo_AttrTransfer
{
public:

  //! Options controlling attribute transfer behavior.
  struct Options
  {
    bool OverwriteExisting = false; //!< Overwrite attrs already present on the target node?
  };

  //! Result counters for diagnostics and tests.
  struct Result
  {
    int NbTransferred = 0; //!< Number of attribute copies performed.
    int NbSkipped     = 0; //!< Attrs skipped because target already had them (OverwriteExisting=false).
  };

  //! Transfer attributes from original nodes to derived nodes using history.
  //! Uses default options (no overwrite).
  //! @param[in,out] theGraph graph with history and attributes
  //! @return transfer statistics
  Standard_EXPORT static Result Perform(BRepGraph& theGraph);

  //! Transfer attributes from original nodes to derived nodes using history.
  //! Walks history records in chronological order. For each mapping entry
  //! (original -> replacements), copies all user attributes from the original
  //! to each replacement node.
  //! @param[in,out] theGraph graph with history and attributes
  //! @param[in] theOptions transfer configuration
  //! @return transfer statistics
  Standard_EXPORT static Result Perform(BRepGraph& theGraph, const Options& theOptions);

private:
  BRepGraphAlgo_AttrTransfer() = delete;
};

#endif // _BRepGraphAlgo_AttrTransfer_HeaderFile
