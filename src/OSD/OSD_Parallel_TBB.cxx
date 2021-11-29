// Created on: 2014-08-19
// Created by: Alexander Zaikin
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

// Version of parallel executor used when TBB is available
#ifdef HAVE_TBB

#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Standard_ProgramError.hxx>

Standard_DISABLE_DEPRECATION_WARNINGS
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>
#include <tbb/blocked_range.h>
#if TBB_VERSION_MAJOR < 2021
  #include <tbb/task_scheduler_init.h>
#endif
Standard_ENABLE_DEPRECATION_WARNINGS

//=======================================================================
//function : forEachExternal
//purpose  : 
//=======================================================================

void OSD_Parallel::forEachExternal (UniversalIterator& theBegin,
                                    UniversalIterator& theEnd,
                                    const FunctorInterface& theFunctor,
                                    Standard_Integer theNbItems)
{
#if TBB_VERSION_MAJOR >= 2021
  // task_scheduler_init is removed,
  // exceptions are captured without proxy tbb::captured_exception object
  (void )theNbItems;
  tbb::parallel_for_each (theBegin, theEnd, theFunctor);
#else
  try
  {
    const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
    const Standard_Integer aNbThreads = theNbItems > 0 ? aThreadPool->NbDefaultThreadsToLaunch() : -1;
    tbb::task_scheduler_init aScheduler (aNbThreads);
    tbb::parallel_for_each (theBegin, theEnd, theFunctor);
  }
  catch (tbb::captured_exception& anException)
  {
    throw Standard_ProgramError (anException.what());
  }
#endif
}

#endif /* HAVE_TBB */
