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

#ifndef _BRepGraph_UserAttribute_HeaderFile
#define _BRepGraph_UserAttribute_HeaderFile

#include <BRepGraph_AttrRegistry.hxx>

#include <Standard_GUID.hxx>

#include <shared_mutex>
#include <atomic>
#include <functional>
#include <memory>

//! Abstract base for user-defined per-node cached attributes.
//!
//! This is a plain C++ polymorphic base -- NOT a Standard_Transient subclass.
//! The graph stores attributes via std::shared_ptr<BRepGraph_UserAttribute>,
//! which gives straightforward ownership without pulling in OCCT Handle<>
//! machinery for types that are entirely internal to BRepGraph.
//!
//! Each attribute kind is identified by an int key for fast runtime lookups.
//! Keys can be obtained via GUID-based registration (recommended for public
//! attributes) or anonymous allocation (for private/ephemeral caches).
//!
//! @code
//!   // GUID-based (recommended):
//!   static const Standard_GUID MY_GUID("a3f1c4e2-7b09-4d5a-8e6f-1a2b3c4d5e6f");
//!   int KEY = BRepGraph_UserAttribute::AllocateKey(MY_GUID);
//!
//!   // Anonymous:
//!   static const int KEY = BRepGraph_UserAttribute::AllocateKey();
//! @endcode
class BRepGraph_UserAttribute
{
public:
  //! Allocate an anonymous integer key (no GUID association).
  //! Thread-safe (shared atomic counter with BRepGraph_AttrRegistry).
  static int AllocateKey()
  {
    return BRepGraph_AttrRegistry::AllocateAnonymous();
  }

  //! Allocate (or retrieve) an integer key associated with a GUID.
  //! Convenience wrapper around BRepGraph_AttrRegistry::Register().
  //! Idempotent: calling with the same GUID always returns the same integer.
  static int AllocateKey(const Standard_GUID& theGUID)
  {
    return BRepGraph_AttrRegistry::Register(theGUID);
  }

  //! Virtual destructor -- enables RTTI for dynamic_cast.
  virtual ~BRepGraph_UserAttribute() = default;

  //! Mark the attribute as needing recomputation.  Lock-free.
  void Invalidate() { myDirty.store(true, std::memory_order_release); }

  //! True if the attribute needs recomputation.
  bool IsDirty() const
  { return myDirty.load(std::memory_order_acquire); }

  //! Non-copyable, non-movable (held via shared_ptr only).
  BRepGraph_UserAttribute(const BRepGraph_UserAttribute&) = delete;
  BRepGraph_UserAttribute& operator=(const BRepGraph_UserAttribute&) = delete;

protected:
  BRepGraph_UserAttribute() : myDirty(true) {}

  //! Subclass calls after successful computation to clear the dirty flag.
  void MarkClean() { myDirty.store(false, std::memory_order_release); }

  //! Mutex for thread-safe Get() in subclasses.
  mutable std::shared_mutex myMutex;

private:
  std::atomic<bool> myDirty;
};

//! Shared pointer type alias for convenience.
using BRepGraph_UserAttrPtr = std::shared_ptr<BRepGraph_UserAttribute>;

#endif // _BRepGraph_UserAttribute_HeaderFile
