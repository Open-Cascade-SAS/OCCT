// Created on: 2016-04-13
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2016 OPEN CASCADE SAS
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

#ifndef _BVH_RadixSorter_Header
#define _BVH_RadixSorter_Header

#include <BVH_Sorter.hxx>
#include <BVH_Builder.hxx>

#include <NCollection_Handle.hxx>
#include <NCollection_Array1.hxx>

//! Pair of Morton code and primitive ID.
typedef std::pair<Standard_Integer, Standard_Integer> BVH_EncodedLink;

//! Performs radix sort of a BVH primitive set using
//! 10-bit Morton codes (or 1024 x 1024 x 1024 grid).
template<class T, int N>
class BVH_RadixSorter : public BVH_Sorter<T, N>
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates new BVH radix sorter for the given AABB.
  BVH_RadixSorter (const BVH_Box<T, N>& theBox) : myBox (theBox) { }

  //! Sorts the set.
  virtual void Perform (BVH_Set<T, N>* theSet);

  //! Sorts the given (inclusive) range in the set.
  virtual void Perform (BVH_Set<T, N>* theSet, const Standard_Integer theStart, const Standard_Integer theFinal);

  //! Returns Morton codes assigned to BVH primitives.
  const NCollection_Array1<BVH_EncodedLink>& EncodedLinks() const { return *myEncodedLinks; }

protected:

  //! Axis-aligned bounding box (AABB) to perform sorting.
  BVH_Box<T, N> myBox;

  //! Morton codes assigned to BVH primitives.
  NCollection_Handle<NCollection_Array1<BVH_EncodedLink> > myEncodedLinks;

};

#include <BVH_RadixSorter.lxx>

#endif // _BVH_RadixSorter_Header
