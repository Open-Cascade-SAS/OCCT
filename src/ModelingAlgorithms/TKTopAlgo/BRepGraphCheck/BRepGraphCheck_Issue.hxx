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

#ifndef _BRepGraphCheck_Issue_HeaderFile
#define _BRepGraphCheck_Issue_HeaderFile

#include <BRepCheck_Status.hxx>
#include <BRepGraph_NodeId.hxx>

//! Structured diagnostic record produced by BRepGraphCheck validation.
//!
//! Each issue identifies the graph node with the problem, an optional parent
//! context node (e.g. the face when checking an edge-in-face), the BRepCheck
//! status code, and a severity level.
struct BRepGraphCheck_Issue
{
  //! Severity classification for validation issues.
  enum class Severity : int
  {
    Warning = 0, //!< Non-critical issue that may be acceptable.
    Error   = 1, //!< Invalid topology/geometry that should be fixed.
    Fatal   = 2  //!< Severe corruption that prevents further processing.
  };

  BRepGraph_NodeId NodeId;        //!< Node with the problem.
  BRepGraph_NodeId ContextNodeId; //!< Parent context (e.g. face for edge-in-face check).
  BRepCheck_Status Status        = BRepCheck_NoError; //!< Error code from BRepCheck_Status enum.
  Severity         IssueSeverity = Severity::Error;   //!< Severity classification.
};

#endif // _BRepGraphCheck_Issue_HeaderFile
