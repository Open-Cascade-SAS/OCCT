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
#include <NCollection_Array1.hxx>
#include <NCollection_Shared.hxx>
#include <OSD_Parallel.hxx>

#include <algorithm>

//! Pair of Morton code and primitive ID.
typedef std::pair<unsigned int, Standard_Integer> BVH_EncodedLink;

//! Performs radix sort of a BVH primitive set using
//! 10-bit Morton codes (or 1024 x 1024 x 1024 grid).
template <class T, int N>
class BVH_RadixSorter : public BVH_Sorter<T, N>
{
public:
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:
  //! Creates new BVH radix sorter for the given AABB.
  BVH_RadixSorter(const BVH_Box<T, N>& theBox)
      : myBox(theBox)
  {
  }

  //! Sorts the set.
  virtual void Perform(BVH_Set<T, N>* theSet) Standard_OVERRIDE
  {
    Perform(theSet, 0, theSet->Size() - 1);
  }

  //! Sorts the given (inclusive) range in the set.
  virtual void Perform(BVH_Set<T, N>*         theSet,
                       const Standard_Integer theStart,
                       const Standard_Integer theFinal) Standard_OVERRIDE;

  //! Returns Morton codes assigned to BVH primitives.
  const NCollection_Array1<BVH_EncodedLink>& EncodedLinks() const { return *myEncodedLinks; }

protected:
  //! Axis-aligned bounding box (AABB) to perform sorting.
  BVH_Box<T, N> myBox;

  //! Morton codes assigned to BVH primitives.
  Handle(NCollection_Shared<NCollection_Array1<BVH_EncodedLink>>) myEncodedLinks;
};

namespace BVH
{
// Radix sort STL predicate for 32-bit integer.
struct BitPredicate
{
  unsigned int myBit;

  //! Creates new radix sort predicate.
  BitPredicate(const Standard_Integer theDigit)
      : myBit(1U << theDigit)
  {
  }

  //! Returns predicate value.
  bool operator()(const BVH_EncodedLink theLink) const
  {
    return !(theLink.first & myBit); // 0-bit to the left side
  }
};

//! STL compare tool used in binary search algorithm.
struct BitComparator
{
  unsigned int myBit;

  //! Creates new STL comparator.
  BitComparator(const Standard_Integer theDigit)
      : myBit(1U << theDigit)
  {
  }

  //! Checks left value for the given bit.
  bool operator()(BVH_EncodedLink theLink1, BVH_EncodedLink /*theLink2*/)
  {
    return !(theLink1.first & myBit);
  }
};

//! Tool object for sorting link array using radix sort algorithm.
class RadixSorter
{
public:
  typedef NCollection_Array1<BVH_EncodedLink>::iterator LinkIterator;

private:
  //! Structure defining sorting range.
  struct SortRange
  {
    LinkIterator     myStart; //!< Start element of exclusive sorting range
    LinkIterator     myFinal; //!< Final element of exclusive sorting range
    Standard_Integer myDigit; //!< Bit number used for partition operation
  };

  //! Functor class to run sorting in parallel.
  class Functor
  {
  public:
    Functor(const SortRange (&aSplits)[2], const Standard_Boolean isParallel)
        : mySplits(aSplits),
          myIsParallel(isParallel)
    {
    }

    //! Runs sorting function for the given range.
    void operator()(const Standard_Integer theIndex) const
    {
      RadixSorter::Sort(mySplits[theIndex].myStart,
                        mySplits[theIndex].myFinal,
                        mySplits[theIndex].myDigit,
                        myIsParallel);
    }

  private:
    void operator=(const Functor&);

  private:
    const SortRange (&mySplits)[2];
    Standard_Boolean myIsParallel;
  };

public:
  static void Sort(LinkIterator           theStart,
                   LinkIterator           theFinal,
                   Standard_Integer       theDigit,
                   const Standard_Boolean isParallel)
  {
    if (theDigit < 24)
    {
      BVH::RadixSorter::perform(theStart, theFinal, theDigit);
    }
    else
    {
      LinkIterator anOffset   = std::partition(theStart, theFinal, BitPredicate(theDigit));
      SortRange    aSplits[2] = {{theStart, anOffset, theDigit - 1},
                                 {anOffset, theFinal, theDigit - 1}};

      OSD_Parallel::For(0, 2, Functor(aSplits, isParallel), !isParallel);
    }
  }

protected:
  // Performs MSD (most significant digit) radix sort.
  static void perform(LinkIterator theStart, LinkIterator theFinal, Standard_Integer theDigit = 29)
  {
    while (theStart != theFinal && theDigit >= 0)
    {
      LinkIterator anOffset = std::partition(theStart, theFinal, BitPredicate(theDigit--));
      perform(theStart, anOffset, theDigit);
      theStart = anOffset;
    }
  }
};
} // namespace BVH

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
template <class T, int N>
void BVH_RadixSorter<T, N>::Perform(BVH_Set<T, N>*         theSet,
                                    const Standard_Integer theStart,
                                    const Standard_Integer theFinal)
{
  Standard_STATIC_ASSERT(N == 2 || N == 3 || N == 4);

  const Standard_Integer aDimension = 1024;
  const Standard_Integer aNbEffComp = N == 2 ? 2 : 3; // 4th component is ignored

  const BVH_VecNt aSceneMin = myBox.CornerMin();
  const BVH_VecNt aSceneMax = myBox.CornerMax();

  BVH_VecNt aNodeMinSizeVecT(static_cast<T>(BVH::THE_NODE_MIN_SIZE));
  BVH::BoxMinMax<T, N>::CwiseMax(aNodeMinSizeVecT, aSceneMax - aSceneMin);

  const BVH_VecNt aReverseSize = BVH_VecNt(static_cast<T>(aDimension)) / aNodeMinSizeVecT;

  myEncodedLinks = new NCollection_Shared<NCollection_Array1<BVH_EncodedLink>>(theStart, theFinal);

  // Step 1 -- Assign Morton code to each primitive
  for (Standard_Integer aPrimIdx = theStart; aPrimIdx <= theFinal; ++aPrimIdx)
  {
    const BVH_VecNt aCenter = theSet->Box(aPrimIdx).Center();
    const BVH_VecNt aVoxelF = (aCenter - aSceneMin) * aReverseSize;

    unsigned int aMortonCode = 0;
    for (Standard_Integer aCompIter = 0; aCompIter < aNbEffComp; ++aCompIter)
    {
      const Standard_Integer aVoxelI = BVH::IntFloor(BVH::VecComp<T, N>::Get(aVoxelF, aCompIter));

      unsigned int aVoxel = static_cast<unsigned int>(Max(0, Min(aVoxelI, aDimension - 1)));

      aVoxel = (aVoxel | (aVoxel << 16)) & 0x030000FF;
      aVoxel = (aVoxel | (aVoxel << 8)) & 0x0300F00F;
      aVoxel = (aVoxel | (aVoxel << 4)) & 0x030C30C3;
      aVoxel = (aVoxel | (aVoxel << 2)) & 0x09249249;

      aMortonCode |= (aVoxel << aCompIter);
    }

    myEncodedLinks->ChangeValue(aPrimIdx) = BVH_EncodedLink(aMortonCode, aPrimIdx);
  }

  // Step 2 -- Sort primitives by their Morton codes using radix sort
  BVH::RadixSorter::Sort(myEncodedLinks->begin(), myEncodedLinks->end(), 29, this->IsParallel());

  NCollection_Array1<Standard_Integer> aLinkMap(theStart, theFinal);
  for (Standard_Integer aLinkIdx = theStart; aLinkIdx <= theFinal; ++aLinkIdx)
  {
    aLinkMap(myEncodedLinks->Value(aLinkIdx).second) = aLinkIdx;
  }

  // Step 3 -- Rearranging primitive list according to Morton codes (in place)
  Standard_Integer aPrimIdx = theStart;
  while (aPrimIdx <= theFinal)
  {
    const Standard_Integer aSortIdx = aLinkMap(aPrimIdx);
    if (aPrimIdx != aSortIdx)
    {
      theSet->Swap(aPrimIdx, aSortIdx);
      std::swap(aLinkMap(aPrimIdx), aLinkMap(aSortIdx));
    }
    else
    {
      ++aPrimIdx;
    }
  }
}

#endif // _BVH_RadixSorter_Header
