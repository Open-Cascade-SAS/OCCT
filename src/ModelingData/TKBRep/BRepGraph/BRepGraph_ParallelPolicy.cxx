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

#include <BRepGraph_ParallelPolicy.hxx>

//=================================================================================================

bool BRepGraph_ParallelPolicy::ShouldRun(const bool      theAllowParallel,
                                         const int       theWorkers,
                                         const Workload& theWorkload)
{
  if (!theAllowParallel || theWorkers <= 1)
  {
    return false;
  }

  const int aPrimaryItems = static_cast<int>(std::max(theWorkload.PrimaryItems, 0u));
  if (aPrimaryItems <= 1)
  {
    return false;
  }

  if (aPrimaryItems <= theWorkers)
  {
    return false;
  }

  const int64_t aTotalWorkUnits =
    static_cast<int64_t>(aPrimaryItems)
    + static_cast<int64_t>(std::max(theWorkload.AuxiliaryItems, 0u))
    + static_cast<int64_t>(std::max(theWorkload.InteractionCount, 0u));
  const int64_t aRequiredWorkUnits =
    static_cast<int64_t>(theWorkers) * static_cast<int64_t>(theWorkers);
  return aTotalWorkUnits > aRequiredWorkUnits;
}