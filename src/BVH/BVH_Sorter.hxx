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

//! Performs centroid-based sorting of abstract set.
template<class T, int N>
class BVH_Sorter
{
public:

  //! Sorts the set by centroids coordinates in specified axis.
  static void Perform (BVH_Set<T, N>*         theSet,
                       const Standard_Integer theAxis);

  //! Sorts the set by centroids coordinates in specified axis.
  static void Perform (BVH_Set<T, N>*         theSet,
                       const Standard_Integer theAxis,
                       const Standard_Integer theBegElement,
                       const Standard_Integer theEndElement);

};

#include <BVH_Sorter.lxx>

#endif // _BVH_Sorter_Header
