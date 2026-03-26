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

#ifndef _BRepGraph_MutRef_HeaderFile
#define _BRepGraph_MutRef_HeaderFile

//! @brief RAII guard wrapping a mutable reference to a topology definition.
//!
//! Obtained via BRepGraph::Builder().MutEdge(), MutVertex(), etc.
//! Provides operator-> / operator* for direct field access.
//! Calls markModified() exactly once on scope exit (destruction),
//! regardless of how many fields were modified.
//!
//! Move-only; non-copyable. After a move, the source guard
//! becomes inert and will not call markModified().
//!
//! This header is included at the bottom of BRepGraph.hxx after the
//! BRepGraph class is fully defined, so markModified() is visible.
//!
//! @warning The stored pointer is valid only while no entities of the same
//! kind are appended to the graph. Appending (e.g., Builder().AddVertexDef(),
//! AddEdgeDef(), AddWireDef(), AddFaceDef(), etc.) may trigger internal
//! vector reallocation, invalidating all pointers. Callers must not invoke
//! any Builder().Add*Def() method while a BRepGraph_MutRef is alive.
//!
//! @code
//!   {
//!     BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge =
//!     theGraph.Builder().MutEdge(BRepGraph_EdgeId(42)); anEdge->Tolerance     = 0.5;
//!     anEdge->SameParameter = true;
//!   } // markModified called once here
//! @endcode
template <typename DefT>
class BRepGraph_MutRef
{
public:
  //! Construct a guard over a mutable definition.
  //! @param[in] theGraph  owning graph (used for markModified on destruction)
  //! @param[in] theDef    pointer to the mutable definition
  //! @param[in] theId     node identity for markModified
  BRepGraph_MutRef(BRepGraph* theGraph, DefT* theDef, const BRepGraph_NodeId theId)
      : myGraph(theGraph),
        myDef(theDef),
        myId(theId)
  {
  }

  //! Destructor: calls the optimized markModified(NodeId, BaseDef&) overload
  //! if the guard still owns the reference, skipping redundant storage dispatch.
  ~BRepGraph_MutRef()
  {
    if (myGraph != nullptr)
      myGraph->markModified(myId, *myDef);
  }

  //! Move constructor: transfers ownership; source becomes inert.
  BRepGraph_MutRef(BRepGraph_MutRef&& theOther) noexcept
      : myGraph(theOther.myGraph),
        myDef(theOther.myDef),
        myId(theOther.myId)
  {
    theOther.myGraph = nullptr;
  }

  //! Move assignment: flushes current guard, then transfers ownership.
  BRepGraph_MutRef& operator=(BRepGraph_MutRef&& theOther) noexcept
  {
    if (this != &theOther)
    {
      if (myGraph != nullptr)
        myGraph->markModified(myId, *myDef);
      myGraph          = theOther.myGraph;
      myDef            = theOther.myDef;
      myId             = theOther.myId;
      theOther.myGraph = nullptr;
    }
    return *this;
  }

  BRepGraph_MutRef(const BRepGraph_MutRef&)            = delete;
  BRepGraph_MutRef& operator=(const BRepGraph_MutRef&) = delete;

  //! Access the definition via pointer syntax.
  [[nodiscard]] DefT* operator->() { return myDef; }

  //! Dereference to the definition.
  [[nodiscard]] DefT& operator*() { return *myDef; }

private:
  BRepGraph*       myGraph; //!< Owning graph (nullptr after move).
  DefT*            myDef;   //!< Mutable definition pointer.
  BRepGraph_NodeId myId;    //!< Node identity for markModified.
};

#endif // _BRepGraph_MutRef_HeaderFile
