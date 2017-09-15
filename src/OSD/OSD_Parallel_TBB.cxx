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
#include <Standard_ProgramError.hxx>

#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>
#include <tbb/blocked_range.h>

//=======================================================================
//function : forEach
//purpose  : 
//=======================================================================

void OSD_Parallel::forEach (UniversalIterator& theBegin,
                            UniversalIterator& theEnd,
                            const FunctorInterface& theFunctor)
{
  try
  {
    tbb::parallel_for_each(theBegin, theEnd, theFunctor);
  }
  catch (tbb::captured_exception& anException)
  {
    throw Standard_ProgramError(anException.what());
  }
}

#endif /* HAVE_TBB */