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

#ifndef _BRepGraph_ParallelPolicy_HeaderFile
#define _BRepGraph_ParallelPolicy_HeaderFile

#include <OSD_Parallel.hxx>

#include <algorithm>
#include <cstdint>

//! Lightweight workload-aware policy for deciding whether an internal phase
//! should actually launch parallel work when parallel mode is allowed.
//!
//! The goal is to avoid forcing every short-lived loop onto the thread pool.
//! Decisions are based on available worker capacity and on the amount of work
//! already visible to the phase, instead of on buried per-loop split sizes.
class BRepGraph_ParallelPolicy
{
public:
  //! Simple workload estimate for an execution phase.
  struct Workload
  {
    uint32_t PrimaryItems     = 0; //!< Main loop range.
    uint32_t AuxiliaryItems   = 0; //!< Additional independent items participating in the phase.
    uint32_t InteractionCount = 0; //!< Pairwise or adjacency work discovered for the phase.
  };

  //! Return the effective logical worker count reported by OSD_Parallel.
  [[nodiscard]] static int WorkerCount()
  {
    static const int THE_WORKER_COUNT = std::max(1, OSD_Parallel::NbLogicalProcessors());
    return THE_WORKER_COUNT;
  }

  //! Check whether parallel execution is allowed and meaningful at all.
  [[nodiscard]] static bool IsParallelAllowed(const bool theAllowParallel)
  {
    return theAllowParallel && WorkerCount() > 1;
  }

  //! Decide whether the estimated workload is large enough to amortize
  //! thread-pool launch and synchronization overhead.
  //! @param[in] theAllowParallel  whether parallel mode is allowed by the caller
  //! @param[in] theWorkers        effective logical worker count
  //! @param[in] theWorkload       estimated workload for the phase
  //! @return true if parallel execution should be used
  [[nodiscard]] Standard_EXPORT static bool ShouldRun(const bool      theAllowParallel,
                                                      const int       theWorkers,
                                                      const Workload& theWorkload);

  //! Overload that queries the active worker count lazily.
  //! @param[in] theAllowParallel  whether parallel mode is allowed by the caller
  //! @param[in] theWorkload       estimated workload for the phase
  //! @return true if parallel execution should be used
  [[nodiscard]] static bool ShouldRun(const bool theAllowParallel, const Workload& theWorkload)
  {
    return ShouldRun(theAllowParallel, WorkerCount(), theWorkload);
  }
};

#endif // _BRepGraph_ParallelPolicy_HeaderFile
