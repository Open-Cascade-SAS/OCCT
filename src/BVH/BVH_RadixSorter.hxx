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

#include <algorithm>

#ifdef HAVE_TBB
  // On Windows, function TryEnterCriticalSection has appeared in Windows NT
  // and is surrounded by #ifdef in MS VC++ 7.1 headers.
  // Thus to use it we need to define appropriate macro saying that we will
  // run on Windows NT 4.0 at least
  #if defined(_WIN32) && !defined(_WIN32_WINNT)
    #define _WIN32_WINNT 0x0501
  #endif

  #include <tbb/parallel_invoke.h>
#endif

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
  virtual void Perform (BVH_Set<T, N>* theSet) Standard_OVERRIDE { Perform (theSet, 0, theSet->Size() - 1); }

  //! Sorts the given (inclusive) range in the set.
  virtual void Perform (BVH_Set<T, N>* theSet, const Standard_Integer theStart, const Standard_Integer theFinal) Standard_OVERRIDE;

  //! Returns Morton codes assigned to BVH primitives.
  const NCollection_Array1<BVH_EncodedLink>& EncodedLinks() const { return *myEncodedLinks; }

protected:

  //! Axis-aligned bounding box (AABB) to perform sorting.
  BVH_Box<T, N> myBox;

  //! Morton codes assigned to BVH primitives.
  Handle(NCollection_Shared<NCollection_Array1<BVH_EncodedLink> >) myEncodedLinks;

};

namespace BVH
{
  // Radix sort STL predicate for 32-bit integer.
  struct BitPredicate
  {
    Standard_Integer myBit;

    //! Creates new radix sort predicate.
    BitPredicate (const Standard_Integer theBit) : myBit (theBit) {}

    //! Returns predicate value.
    bool operator() (const BVH_EncodedLink theLink) const
    {
      const Standard_Integer aMask = 1 << myBit;
      return !(theLink.first & aMask); // 0-bit to the left side
    }
  };

  //! STL compare tool used in binary search algorithm.
  struct BitComparator
  {
    Standard_Integer myBit;

    //! Creates new STL comparator.
    BitComparator (const Standard_Integer theBit) : myBit (theBit) {}

    //! Checks left value for the given bit.
    bool operator() (BVH_EncodedLink theLink1, BVH_EncodedLink /*theLink2*/)
    {
      return !(theLink1.first & (1 << myBit));
    }
  };

  //! Tool object for sorting link array using radix sort algorithm.
  class RadixSorter
  {
  public:

    typedef NCollection_Array1<BVH_EncodedLink>::iterator LinkIterator;

  private:

    //! TBB functor class to run sorting.
    struct Functor
    {
      LinkIterator     myStart; //!< Start element of exclusive sorting range
      LinkIterator     myFinal; //!< Final element of exclusive sorting range
      Standard_Integer myDigit; //!< Bit number used for partition operation

      //! Creates new sorting functor.
      Functor (LinkIterator theStart, LinkIterator theFinal, Standard_Integer theDigit)
      : myStart (theStart), myFinal (theFinal), myDigit (theDigit) {}

      //! Runs sorting function for the given range.
      void operator() () const
      {
        RadixSorter::Sort (myStart, myFinal, myDigit);
      }
    };

  public:

    static void Sort (LinkIterator theStart, LinkIterator theFinal, Standard_Integer theDigit)
    {
    #ifdef HAVE_TBB
      if (theDigit < 24)
      {
        BVH::RadixSorter::perform (theStart, theFinal, theDigit);
      }
      else
      {
        LinkIterator anOffset = std::partition (theStart, theFinal, BitPredicate (theDigit));
        tbb::parallel_invoke (Functor (theStart, anOffset, theDigit - 1),
                              Functor (anOffset, theFinal, theDigit - 1));
      }
    #else
      BVH::RadixSorter::perform (theStart, theFinal, theDigit);
    #endif
    }

  protected:

    // Performs MSD (most significant digit) radix sort.
    static void perform (LinkIterator theStart, LinkIterator theFinal, Standard_Integer theBit = 29)
    {
      while (theStart != theFinal && theBit >= 0)
      {
        LinkIterator anOffset = std::partition (theStart, theFinal, BitPredicate (theBit--));
        perform (theStart, anOffset, theBit);
        theStart = anOffset;
      }
    }
  };
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
template<class T, int N>
void BVH_RadixSorter<T, N>::Perform (BVH_Set<T, N>* theSet, const Standard_Integer theStart, const Standard_Integer theFinal)
{
  Standard_STATIC_ASSERT (N == 3 || N == 4);

  const Standard_Integer aDimensionX = 1024;
  const Standard_Integer aDimensionY = 1024;
  const Standard_Integer aDimensionZ = 1024;

  const BVH_VecNt aSceneMin = myBox.CornerMin();
  const BVH_VecNt aSceneMax = myBox.CornerMax();

  const T aReverseSizeX = static_cast<T> (aDimensionX) / Max (static_cast<T> (BVH::THE_NODE_MIN_SIZE), aSceneMax.x() - aSceneMin.x());
  const T aReverseSizeY = static_cast<T> (aDimensionY) / Max (static_cast<T> (BVH::THE_NODE_MIN_SIZE), aSceneMax.y() - aSceneMin.y());
  const T aReverseSizeZ = static_cast<T> (aDimensionZ) / Max (static_cast<T> (BVH::THE_NODE_MIN_SIZE), aSceneMax.z() - aSceneMin.z());

  myEncodedLinks = new NCollection_Shared<NCollection_Array1<BVH_EncodedLink> >(theStart, theFinal);

  // Step 1 -- Assign Morton code to each primitive
  for (Standard_Integer aPrimIdx = theStart; aPrimIdx <= theFinal; ++aPrimIdx)
  {
    const BVH_VecNt aCenter = theSet->Box (aPrimIdx).Center();

    Standard_Integer aVoxelX = BVH::IntFloor ((aCenter.x() - aSceneMin.x()) * aReverseSizeX);
    Standard_Integer aVoxelY = BVH::IntFloor ((aCenter.y() - aSceneMin.y()) * aReverseSizeY);
    Standard_Integer aVoxelZ = BVH::IntFloor ((aCenter.z() - aSceneMin.z()) * aReverseSizeZ);

    aVoxelX = Max (0, Min (aVoxelX, aDimensionX - 1));
    aVoxelY = Max (0, Min (aVoxelY, aDimensionY - 1));
    aVoxelZ = Max (0, Min (aVoxelZ, aDimensionZ - 1));

    aVoxelX = (aVoxelX | (aVoxelX << 16)) & 0x030000FF;
    aVoxelX = (aVoxelX | (aVoxelX <<  8)) & 0x0300F00F;
    aVoxelX = (aVoxelX | (aVoxelX <<  4)) & 0x030C30C3;
    aVoxelX = (aVoxelX | (aVoxelX <<  2)) & 0x09249249;

    aVoxelY = (aVoxelY | (aVoxelY << 16)) & 0x030000FF;
    aVoxelY = (aVoxelY | (aVoxelY <<  8)) & 0x0300F00F;
    aVoxelY = (aVoxelY | (aVoxelY <<  4)) & 0x030C30C3;
    aVoxelY = (aVoxelY | (aVoxelY <<  2)) & 0x09249249;

    aVoxelZ = (aVoxelZ | (aVoxelZ << 16)) & 0x030000FF;
    aVoxelZ = (aVoxelZ | (aVoxelZ <<  8)) & 0x0300F00F;
    aVoxelZ = (aVoxelZ | (aVoxelZ <<  4)) & 0x030C30C3;
    aVoxelZ = (aVoxelZ | (aVoxelZ <<  2)) & 0x09249249;

    myEncodedLinks->ChangeValue (aPrimIdx) = BVH_EncodedLink (
      aVoxelX | (aVoxelY << 1) | (aVoxelZ << 2), aPrimIdx);
  }

  // Step 2 -- Sort primitives by their Morton codes using radix sort
  BVH::RadixSorter::Sort (myEncodedLinks->begin(), myEncodedLinks->end(), 29);

  NCollection_Array1<Standard_Integer> aLinkMap (theStart, theFinal);
  for (Standard_Integer aLinkIdx = theStart; aLinkIdx <= theFinal; ++aLinkIdx)
  {
    aLinkMap (myEncodedLinks->Value (aLinkIdx).second) = aLinkIdx;
  }

  // Step 3 -- Rearranging primitive list according to Morton codes (in place)
  Standard_Integer aPrimIdx = theStart;
  while (aPrimIdx <= theFinal)
  {
    const Standard_Integer aSortIdx = aLinkMap (aPrimIdx);
    if (aPrimIdx != aSortIdx)
    {
      theSet->Swap (aPrimIdx, aSortIdx);
      std::swap (aLinkMap (aPrimIdx),
                 aLinkMap (aSortIdx));
    }
    else
    {
      ++aPrimIdx;
    }
  }
}

#endif // _BVH_RadixSorter_Header
