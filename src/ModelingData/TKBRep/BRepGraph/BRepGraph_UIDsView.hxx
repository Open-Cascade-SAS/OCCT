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

#ifndef _BRepGraph_UIDsView_HeaderFile
#define _BRepGraph_UIDsView_HeaderFile

#include <BRepGraph.hxx>

//! Lightweight const view over UID-related queries of a BRepGraph.
//! Obtained via BRepGraph::UIDs().
class BRepGraph::UIDsView
{
public:
  //! Check if UID generation is enabled.
  Standard_EXPORT bool IsEnabled() const;

  //! Return the UID assigned to a node (empty if UIDs are disabled).
  //! @param[in] theNode node identifier
  Standard_EXPORT BRepGraph_UID Of(BRepGraph_NodeId theNode) const;

  //! Resolve a UID back to a NodeId.
  //! @param[in] theUID unique identifier to resolve
  Standard_EXPORT BRepGraph_NodeId NodeIdFrom(const BRepGraph_UID& theUID) const;

  //! Check if a UID is valid and exists in this graph generation.
  //! @param[in] theUID unique identifier to check
  Standard_EXPORT bool Has(const BRepGraph_UID& theUID) const;

  //! Current generation counter (incremented on each Build).
  Standard_EXPORT uint32_t Generation() const;

private:
  friend class BRepGraph;
  explicit UIDsView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::UIDsView BRepGraph::UIDs() const { return UIDsView(this); }

#endif // _BRepGraph_UIDsView_HeaderFile
