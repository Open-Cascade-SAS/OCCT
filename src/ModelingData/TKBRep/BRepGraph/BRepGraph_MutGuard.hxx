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

#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <Standard_ProgramError.hxx>

#include <type_traits>

//! @brief RAII scope token batching mutation notifications for a single entity.
//!
//! Obtained via BRepGraph::Editor().<Ops>().Mut() / MutRef() / MutSurface() etc.
//! Reads via `operator->()` / `operator*()`; writes via Editor's typed setters
//! (or `Internal()` for in-tree structural remaps). Any call to `Internal()`
//! flags the guard dirty and the destructor fires `markModified` /
//! `markRefModified` once on scope exit.
//!
//! The guard registers itself as active on the guarded item at construction
//! and deregisters on destruction. This prevents double-mutation: attempting
//! to acquire a second guard on the same item while the first is still alive
//! will throw. Move-only; after a move, the source guard becomes inert and
//! does not deregister.
//!
//! Compile-time dispatch selects the ID type and notification method:
//! - For types derived from BRepGraphInc::BaseDef: BRepGraph_NodeId + markModified()
//! - For types derived from BRepGraphInc::BaseRef: BRepGraph_RefId + markRefModified()
//!
//! @code
//!   {
//!     BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge =
//!       theGraph.Editor().Edges().Mut(BRepGraph_EdgeId(42));
//!     theGraph.Editor().Edges().SetTolerance(anEdge, 0.5);
//!   } // markModified called once here, guard deregistered
//! @endcode
template <typename T>
class BRepGraph_MutGuard
{
  static_assert(std::is_base_of_v<BRepGraphInc::BaseDef, T>
                  || std::is_base_of_v<BRepGraphInc::BaseRef, T>,
                "BRepGraph_MutGuard<T>: T must derive from BaseDef or BaseRef");

  using TypeId = typename T::TypeId;

  //! Call the appropriate notification method on the graph.
  void notify() noexcept
  {
    try
    {
      if constexpr (std::is_base_of_v<BRepGraphInc::BaseDef, T>)
      {
        myGraph->markModified(myId, *myEntity);
      }
      else if constexpr (std::is_base_of_v<BRepGraphInc::BaseRef, T>)
      {
        myGraph->markRefModified(myId);
      }
    }
    catch (...)
    {
    }
  }

public:
  //! Construct a guard over a mutable entity.
  //! Registers the item via the storage bit-plane. The Mut() factory pre-validates
  //! that no guard is active, so this assertion should never fire in normal use.
  //! @param[in] theGraph   owning graph (used for notification)
  //! @param[in] theStorage storage instance (for bit-plane guard tracking)
  //! @param[in] theEntity  pointer to the mutable entity
  //! @param[in] theId      identity for notification and guard registration
  BRepGraph_MutGuard(BRepGraph&            theGraph,
                     BRepGraphInc_Storage& theStorage,
                     T*                    theEntity,
                     const TypeId          theId)
      : myGraph(&theGraph),
        myStorage(&theStorage),
        myEntity(theEntity),
        myId(theId),
        myDirty(false)
  {
    Standard_ProgramError_Raise_if(myStorage->IsGuarded(BRepGraph_ItemId(myId)),
                                   "BRepGraph_MutGuard: guard already active on this item");
    myStorage->SetGuarded(BRepGraph_ItemId(myId));
  }

  //! Destructor: clears the guard bit-plane and notifies the graph if the
  //! guard owns an entity AND at least one setter (or `MarkDirty`) flagged it modified.
  //! Guard clearance happens BEFORE notification so that markModified() propagation
  //! does not see a stale guard registration on the same item.
  ~BRepGraph_MutGuard()
  {
    if (myEntity != nullptr)
    {
      myStorage->ClearGuarded(BRepGraph_ItemId(myId));
      if (myDirty)
      {
        notify();
      }
    }
  }

  //! Move constructor: transfers guard ownership. The source becomes inert
  //! and will not deregister on its destruction.
  BRepGraph_MutGuard(BRepGraph_MutGuard&& theOther) noexcept
      : myGraph(theOther.myGraph),
        myStorage(theOther.myStorage),
        myEntity(theOther.myEntity),
        myId(theOther.myId),
        myDirty(theOther.myDirty)
  {
    theOther.myEntity  = nullptr;
    theOther.myDirty   = false;
    theOther.myGraph   = nullptr;
    theOther.myStorage = nullptr;
  }

  //! Move assignment: deregisters current guard (if active), then transfers
  //! ownership from the source. The source becomes inert.
  BRepGraph_MutGuard& operator=(BRepGraph_MutGuard&& theOther) noexcept
  {
    if (this != &theOther)
    {
      if (myEntity != nullptr)
      {
        myStorage->ClearGuarded(BRepGraph_ItemId(myId));
        if (myDirty)
        {
          notify();
        }
      }
      myGraph            = theOther.myGraph;
      myStorage          = theOther.myStorage;
      myEntity           = theOther.myEntity;
      myId               = theOther.myId;
      myDirty            = theOther.myDirty;
      theOther.myEntity  = nullptr;
      theOther.myDirty   = false;
      theOther.myGraph   = nullptr;
      theOther.myStorage = nullptr;
    }
    return *this;
  }

  BRepGraph_MutGuard(const BRepGraph_MutGuard&)            = delete;
  BRepGraph_MutGuard& operator=(const BRepGraph_MutGuard&) = delete;

  //! True when the guard still owns an entity; false after a move or when
  //! constructed in an inert state.
  [[nodiscard]] explicit operator bool() const noexcept { return myEntity != nullptr; }

  //! Read-only access via pointer syntax. Field writes go through the
  //! Editor's typed setters.
  [[nodiscard]] const T* operator->() const
  {
    Standard_ProgramError_Raise_if(
      myEntity == nullptr,
      "BRepGraph_MutGuard::operator->(): guard is empty or moved-from");
    return myEntity;
  }

  //! Read-only dereference.
  [[nodiscard]] const T& operator*() const
  {
    Standard_ProgramError_Raise_if(myEntity == nullptr,
                                   "BRepGraph_MutGuard::operator*(): guard is empty or moved-from");
    return *myEntity;
  }

  //! Identity for notification.
  [[nodiscard]] TypeId Id() const noexcept { return myId; }

  //! Owning graph handle.
  [[nodiscard]] BRepGraph& Graph() const { return *myGraph; }

  //! Flag the guarded entity as modified without writing through `Internal()`.
  //! Use when an external mutation (e.g. in-place geometry transform on a shared
  //! Geom handle) is not visible to the guard.
  void MarkDirty() noexcept { myDirty = true; }

  //! True if `Internal()` or `MarkDirty()` flagged the entity modified.
  [[nodiscard]] bool IsDirty() const noexcept { return myDirty; }

  //! INTERNAL USE ONLY. Mutable accessor; auto-flags dirty. External code MUST go
  //! through Editor's typed setters. Use `operator->()` / `operator*()` for reads.
  [[nodiscard]] T& Internal() noexcept
  {
    myDirty = true;
    return *myEntity;
  }

private:
  BRepGraph*            myGraph;   //!< Owning graph, non-owning.
  BRepGraphInc_Storage* myStorage; //!< Storage instance for bit-plane guard tracking, non-owning.
  T*                    myEntity;  //!< Mutable entity pointer; access via Editor setters.
  TypeId                myId;      //!< Identity for notification and guard registration.
  bool                  myDirty;   //!< True once a setter has modified the entity in this scope.
};

#endif // _BRepGraph_MutGuard_HeaderFile
