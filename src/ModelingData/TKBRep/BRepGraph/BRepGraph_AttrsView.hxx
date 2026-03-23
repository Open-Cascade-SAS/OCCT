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

#ifndef _BRepGraph_AttrsView_HeaderFile
#define _BRepGraph_AttrsView_HeaderFile

#include <BRepGraph.hxx>

//! @brief Non-const view for managing user-defined attributes on nodes.
//!
//! Attributes are keyed by integer identifiers and stored as
//! Handle(BRepGraph_UserAttribute). Supports set, get, remove,
//! invalidate, and key enumeration per node. Attribute data is
//! stored in the node's BRepGraph_NodeCache.
//! Obtained via BRepGraph::Attrs().
class BRepGraph::AttrsView
{
public:
  //! Attach a user attribute to a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  //! @param[in] theAttr handle to the attribute
  Standard_EXPORT void Set(const BRepGraph_NodeId                       theNode,
                           const int                                    theKey,
                           const occ::handle<BRepGraph_UserAttribute>& theAttr);

  //! Retrieve a user attribute from a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  //! @return handle to the attribute, or null handle if not set
  Standard_EXPORT occ::handle<BRepGraph_UserAttribute> Get(const BRepGraph_NodeId theNode, const int theKey) const;

  //! Remove a user attribute from a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  //! @return true if the attribute was found and removed
  Standard_EXPORT bool Remove(const BRepGraph_NodeId theNode, const int theKey);

  //! Invalidate (but do not remove) a user attribute on a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  Standard_EXPORT void Invalidate(const BRepGraph_NodeId theNode, const int theKey);

  //! Return all user attribute keys on a node.
  //! @param[in] theNode node identifier
  //! @return vector of attribute keys (empty if node has no attributes or is a geometry node)
  Standard_EXPORT NCollection_Vector<int> AttributeKeys(const BRepGraph_NodeId theNode) const;

private:
  friend class BRepGraph;
  explicit AttrsView(BRepGraph* theGraph) : myGraph(theGraph) {}
  BRepGraph* myGraph;
};

inline BRepGraph::AttrsView BRepGraph::Attrs() { return AttrsView(this); }

#endif // _BRepGraph_AttrsView_HeaderFile
