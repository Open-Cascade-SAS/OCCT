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

#ifndef _BRepGraph_MutationGuard_HeaderFile
#define _BRepGraph_MutationGuard_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Mutator.hxx>

//! @brief RAII guard for batch mutation scopes.
//!
//! Activates deferred invalidation on construction and flushes it on destruction,
//! followed by CommitMutation validation. Guarantees exception-safe cleanup:
//! no partial mutations are visible outside the guard scope.
//!
//! Re-entrant: if deferred mode is already active (e.g., nested guard),
//! the inner guard is a no-op - only the outermost guard flushes and commits.
//!
//! Usage:
//! @code
//!   {
//!     BRepGraph_MutationGuard aGuard(theGraph);
//!     OSD_Parallel::For(0, N, [&](int i) { /* mutations */ }, !parallel);
//!   } // EndDeferredInvalidation + CommitMutation called here
//! @endcode
class BRepGraph_MutationGuard
{
public:
  //! Begin deferred invalidation if not already active.
  explicit BRepGraph_MutationGuard(BRepGraph& theGraph)
      : myGraph(theGraph),
        myOwnsScope(!theGraph.Builder().IsDeferredMode())
  {
    if (myOwnsScope)
      myGraph.Builder().BeginDeferredInvalidation();
  }

  //! End deferred invalidation and validate reverse index + active counts.
  ~BRepGraph_MutationGuard()
  {
    if (myOwnsScope)
    {
      myGraph.Builder().EndDeferredInvalidation();
      BRepGraph_Mutator::CommitMutation(myGraph);
    }
  }

  BRepGraph_MutationGuard(const BRepGraph_MutationGuard&)            = delete;
  BRepGraph_MutationGuard& operator=(const BRepGraph_MutationGuard&) = delete;

private:
  BRepGraph& myGraph;
  bool       myOwnsScope;
};

#endif // _BRepGraph_MutationGuard_HeaderFile
