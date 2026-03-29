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

#ifndef _BRepGraph_DeferredScope_HeaderFile
#define _BRepGraph_DeferredScope_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>

//! @brief RAII guard for batch mutation scopes with deferred invalidation.
//!
//! Activates deferred invalidation on construction and flushes it on destruction,
//! followed by CommitMutation validation. Guarantees exception-safe cleanup:
//! when this guard owns deferred mode, it is always closed and boundary checks
//! are executed at scope exit.
//!
//! Re-entrant: if deferred mode is already active (e.g., nested guard),
//! the inner guard is a no-op - only the outermost guard flushes and commits.
//!
//! @warning This guard batches invalidation and propagation; it is NOT a
//! transaction and does not serialize mutation bodies. Concurrent `Mut*()`
//! usage still requires external synchronization.
//!
//! Usage:
//! @code
//!   {
//!     BRepGraph_DeferredScope aScope(theGraph);
//!     for (int i = 0; i < N; ++i)
//!     {
//!       // mutations
//!     }
//!   } // EndDeferredInvalidation + CommitMutation called here
//! @endcode
class BRepGraph_DeferredScope
{
public:
  //! Begin deferred invalidation if not already active.
  explicit BRepGraph_DeferredScope(BRepGraph& theGraph)
      : myGraph(theGraph),
        myOwnsScope(!theGraph.Builder().IsDeferredMode())
  {
    if (myOwnsScope)
      myGraph.Builder().BeginDeferredInvalidation();
  }

  //! End deferred invalidation and validate reverse index + active counts.
  ~BRepGraph_DeferredScope()
  {
    if (myOwnsScope)
    {
      myGraph.Builder().EndDeferredInvalidation();
      myGraph.Builder().CommitMutation();
    }
  }

  BRepGraph_DeferredScope(const BRepGraph_DeferredScope&)            = delete;
  BRepGraph_DeferredScope& operator=(const BRepGraph_DeferredScope&) = delete;

private:
  BRepGraph& myGraph;
  bool       myOwnsScope;
};

#endif // _BRepGraph_DeferredScope_HeaderFile
