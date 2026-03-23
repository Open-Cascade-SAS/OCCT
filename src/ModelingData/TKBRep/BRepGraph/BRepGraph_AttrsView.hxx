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

//! Lightweight non-const view over user attributes of a BRepGraph.
//! Obtained via BRepGraph::Attrs().
class BRepGraph::AttrsView
{
public:
  //! Attach a user attribute to a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  //! @param[in] theAttr shared pointer to the attribute
  void Set(BRepGraph_NodeId             theNode,
           int                          theKey,
           const BRepGraph_UserAttrPtr& theAttr)
  {
    myGraph->SetUserAttribute(theNode, theKey, theAttr);
  }

  //! Retrieve a user attribute from a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  //! @return shared pointer to the attribute, or nullptr if not set
  BRepGraph_UserAttrPtr Get(BRepGraph_NodeId theNode, int theKey) const
  {
    return myGraph->GetUserAttribute(theNode, theKey);
  }

  //! Remove a user attribute from a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  //! @return true if the attribute was found and removed
  bool Remove(BRepGraph_NodeId theNode, int theKey)
  {
    return myGraph->RemoveUserAttribute(theNode, theKey);
  }

  //! Invalidate (but do not remove) a user attribute on a node.
  //! @param[in] theNode node identifier
  //! @param[in] theKey  attribute key
  void Invalidate(BRepGraph_NodeId theNode, int theKey)
  {
    myGraph->InvalidateUserAttribute(theNode, theKey);
  }

private:
  friend class BRepGraph;
  explicit AttrsView(BRepGraph* theGraph) : myGraph(theGraph) {}
  BRepGraph* myGraph;
};

inline BRepGraph::AttrsView BRepGraph::Attrs() { return AttrsView(this); }

#endif // _BRepGraph_AttrsView_HeaderFile
