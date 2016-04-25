// Created on: 2014-01-10
// Created by: Denis BOGOLEPOV
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

#ifndef _BVH_Sorter_Header
#define _BVH_Sorter_Header

#include <BVH_Set.hxx>

//! Tool object to sort abstract primitive set.
template<class T, int N>
class BVH_Sorter
{
public:

  //! Releases resources of BVH sorter.
  virtual ~BVH_Sorter() { }

  //! Sorts the set.
  virtual void Perform (BVH_Set<T, N>* theSet) = 0;

  //! Sorts the given (inclusive) range in the set.
  virtual void Perform (BVH_Set<T, N>* theSet, const Standard_Integer theStart, const Standard_Integer theFinal) = 0;

};

#endif // _BVH_Sorter_Header
