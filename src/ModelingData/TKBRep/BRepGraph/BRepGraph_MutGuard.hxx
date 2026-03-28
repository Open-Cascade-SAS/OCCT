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

#ifndef _BRepGraph_MutGuard_HeaderFile
#define _BRepGraph_MutGuard_HeaderFile

#include <type_traits>

//! @brief RAII guard wrapping a mutable topology definition or reference entry.
//!
//! Obtained via BRepGraph::Builder().MutEdge(), MutVertex(), MutFaceRef(), etc.
//! Provides operator-> / operator* for direct field access.
//! Calls the appropriate markModified() or markRefModified() exactly once
//! on scope exit (destruction), regardless of how many fields were modified.
//!
//! Move-only; non-copyable. After a move, the source guard
//! becomes inert and will not trigger notification.
//!
//! Compile-time dispatch selects the ID type and notification method:
//! - For types derived from BRepGraphInc::BaseDef: BRepGraph_NodeId + markModified()
//! - For types derived from BRepGraphInc::BaseRef: BRepGraph_RefId + markRefModified()
//!
//! @warning The stored pointer is valid only while no entities of the same
//! kind are appended to the graph. Appending (e.g., Builder().AddVertex(),
//! AddEdge(), etc.) may trigger internal vector reallocation, invalidating
//! all pointers. Callers must not invoke any Builder().Add*() method
//! while a BRepGraph_MutGuard is alive.
//!
//! @code
//!   {
//!     BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
//!       theGraph.Builder().MutEdge(BRepGraph_EdgeId(42));
//!     anEdge->Tolerance     = 0.5;
//!     anEdge->SameParameter = true;
//!   } // markModified called once here
//! @endcode
template <typename T>
class BRepGraph_MutGuard
{
  static_assert(std::is_base_of_v<BRepGraphInc::BaseDef, T>
                  || std::is_base_of_v<BRepGraphInc::BaseRef, T>,
                "BRepGraph_MutGuard<T>: T must derive from BaseDef or BaseRef");

  //! ID type: BRepGraph_NodeId for definitions, BRepGraph_RefId for references.
  using IdType =
    std::conditional_t<std::is_base_of_v<BRepGraphInc::BaseDef, T>, BRepGraph_NodeId, BRepGraph_RefId>;

  //! Call the appropriate notification method on the graph.
  void notify()
  {
    if constexpr (std::is_base_of_v<BRepGraphInc::BaseDef, T>)
      myGraph->markModified(myId, *myEntity);
    else
      myGraph->markRefModified(myId, *myEntity);
  }

public:
  //! Construct a guard over a mutable entity.
  //! @param[in] theGraph  owning graph (used for notification on destruction)
  //! @param[in] theEntity pointer to the mutable entity
  //! @param[in] theId     identity for notification
  BRepGraph_MutGuard(BRepGraph* theGraph, T* theEntity, const IdType theId)
      : myGraph(theGraph),
        myEntity(theEntity),
        myId(theId)
  {
  }

  //! Destructor: notifies the graph if the guard still owns the reference.
  ~BRepGraph_MutGuard()
  {
    if (myGraph != nullptr)
      notify();
  }

  //! Move constructor: transfers ownership; source becomes inert.
  BRepGraph_MutGuard(BRepGraph_MutGuard&& theOther) noexcept
      : myGraph(theOther.myGraph),
        myEntity(theOther.myEntity),
        myId(theOther.myId)
  {
    theOther.myGraph = nullptr;
  }

  //! Move assignment: flushes current guard, then transfers ownership.
  BRepGraph_MutGuard& operator=(BRepGraph_MutGuard&& theOther) noexcept
  {
    if (this != &theOther)
    {
      if (myGraph != nullptr)
        notify();
      myGraph          = theOther.myGraph;
      myEntity         = theOther.myEntity;
      myId             = theOther.myId;
      theOther.myGraph = nullptr;
    }
    return *this;
  }

  BRepGraph_MutGuard(const BRepGraph_MutGuard&)            = delete;
  BRepGraph_MutGuard& operator=(const BRepGraph_MutGuard&) = delete;

  //! Access the entity via pointer syntax.
  [[nodiscard]] T* operator->() { return myEntity; }

  //! Dereference to the entity.
  [[nodiscard]] T& operator*() { return *myEntity; }

private:
  BRepGraph* myGraph;  //!< Owning graph (nullptr after move).
  T*         myEntity; //!< Mutable entity pointer.
  IdType     myId;     //!< Identity for notification.
};

#endif // _BRepGraph_MutGuard_HeaderFile
