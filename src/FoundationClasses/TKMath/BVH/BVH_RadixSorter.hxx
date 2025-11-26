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

namespace BVH
{
//! Lookup table for expanding 8-bit value to 24-bit Morton code component.
//! Each bit is spread to every 3rd position for interleaving with other components.
constexpr unsigned int THE_MORTON_LUT[256] = {
  0x000000, 0x000001, 0x000008, 0x000009, 0x000040, 0x000041, 0x000048, 0x000049, 0x000200,
  0x000201, 0x000208, 0x000209, 0x000240, 0x000241, 0x000248, 0x000249, 0x001000, 0x001001,
  0x001008, 0x001009, 0x001040, 0x001041, 0x001048, 0x001049, 0x001200, 0x001201, 0x001208,
  0x001209, 0x001240, 0x001241, 0x001248, 0x001249, 0x008000, 0x008001, 0x008008, 0x008009,
  0x008040, 0x008041, 0x008048, 0x008049, 0x008200, 0x008201, 0x008208, 0x008209, 0x008240,
  0x008241, 0x008248, 0x008249, 0x009000, 0x009001, 0x009008, 0x009009, 0x009040, 0x009041,
  0x009048, 0x009049, 0x009200, 0x009201, 0x009208, 0x009209, 0x009240, 0x009241, 0x009248,
  0x009249, 0x040000, 0x040001, 0x040008, 0x040009, 0x040040, 0x040041, 0x040048, 0x040049,
  0x040200, 0x040201, 0x040208, 0x040209, 0x040240, 0x040241, 0x040248, 0x040249, 0x041000,
  0x041001, 0x041008, 0x041009, 0x041040, 0x041041, 0x041048, 0x041049, 0x041200, 0x041201,
  0x041208, 0x041209, 0x041240, 0x041241, 0x041248, 0x041249, 0x048000, 0x048001, 0x048008,
  0x048009, 0x048040, 0x048041, 0x048048, 0x048049, 0x048200, 0x048201, 0x048208, 0x048209,
  0x048240, 0x048241, 0x048248, 0x048249, 0x049000, 0x049001, 0x049008, 0x049009, 0x049040,
  0x049041, 0x049048, 0x049049, 0x049200, 0x049201, 0x049208, 0x049209, 0x049240, 0x049241,
  0x049248, 0x049249, 0x200000, 0x200001, 0x200008, 0x200009, 0x200040, 0x200041, 0x200048,
  0x200049, 0x200200, 0x200201, 0x200208, 0x200209, 0x200240, 0x200241, 0x200248, 0x200249,
  0x201000, 0x201001, 0x201008, 0x201009, 0x201040, 0x201041, 0x201048, 0x201049, 0x201200,
  0x201201, 0x201208, 0x201209, 0x201240, 0x201241, 0x201248, 0x201249, 0x208000, 0x208001,
  0x208008, 0x208009, 0x208040, 0x208041, 0x208048, 0x208049, 0x208200, 0x208201, 0x208208,
  0x208209, 0x208240, 0x208241, 0x208248, 0x208249, 0x209000, 0x209001, 0x209008, 0x209009,
  0x209040, 0x209041, 0x209048, 0x209049, 0x209200, 0x209201, 0x209208, 0x209209, 0x209240,
  0x209241, 0x209248, 0x209249, 0x240000, 0x240001, 0x240008, 0x240009, 0x240040, 0x240041,
  0x240048, 0x240049, 0x240200, 0x240201, 0x240208, 0x240209, 0x240240, 0x240241, 0x240248,
  0x240249, 0x241000, 0x241001, 0x241008, 0x241009, 0x241040, 0x241041, 0x241048, 0x241049,
  0x241200, 0x241201, 0x241208, 0x241209, 0x241240, 0x241241, 0x241248, 0x241249, 0x248000,
  0x248001, 0x248008, 0x248009, 0x248040, 0x248041, 0x248048, 0x248049, 0x248200, 0x248201,
  0x248208, 0x248209, 0x248240, 0x248241, 0x248248, 0x248249, 0x249000, 0x249001, 0x249008,
  0x249009, 0x249040, 0x249041, 0x249048, 0x249049, 0x249200, 0x249201, 0x249208, 0x249209,
  0x249240, 0x249241, 0x249248, 0x249249};

//! Encodes 10-bit voxel coordinates into 30-bit Morton code using LUT.
//! @param theVoxelX X coordinate (0-1023)
//! @param theVoxelY Y coordinate (0-1023)
//! @param theVoxelZ Z coordinate (0-1023)
//! @return 30-bit Morton code with interleaved bits
constexpr unsigned int EncodeMortonCode(unsigned int theVoxelX,
                                        unsigned int theVoxelY,
                                        unsigned int theVoxelZ)
{
  // Split each 10-bit coordinate into two 8-bit lookups (upper 2 bits + lower 8 bits)
  // For 10-bit values, we use lower 8 bits via LUT and handle upper 2 bits separately
  return (THE_MORTON_LUT[theVoxelX & 0xFF] | (THE_MORTON_LUT[(theVoxelX >> 8) & 0x03] << 24))
         | ((THE_MORTON_LUT[theVoxelY & 0xFF] | (THE_MORTON_LUT[(theVoxelY >> 8) & 0x03] << 24))
            << 1)
         | ((THE_MORTON_LUT[theVoxelZ & 0xFF] | (THE_MORTON_LUT[(theVoxelZ >> 8) & 0x03] << 24))
            << 2);
}
} // namespace BVH

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

//=================================================================================================

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

  // Step 1 -- Assign Morton code to each primitive using LUT for faster encoding
  for (Standard_Integer aPrimIdx = theStart; aPrimIdx <= theFinal; ++aPrimIdx)
  {
    const BVH_VecNt aCenter = theSet->Box(aPrimIdx).Center();
    const BVH_VecNt aVoxelF = (aCenter - aSceneMin) * aReverseSize;

    // Compute voxel coordinates clamped to valid range
    const Standard_Integer aVoxelX =
      std::clamp(BVH::IntFloor(BVH::VecComp<T, N>::Get(aVoxelF, 0)), 0, aDimension - 1);
    const Standard_Integer aVoxelY =
      std::clamp(BVH::IntFloor(BVH::VecComp<T, N>::Get(aVoxelF, 1)), 0, aDimension - 1);
    const Standard_Integer aVoxelZ =
      (aNbEffComp > 2)
        ? std::clamp(BVH::IntFloor(BVH::VecComp<T, N>::Get(aVoxelF, 2)), 0, aDimension - 1)
        : 0;

    // Use LUT-based Morton code encoding for better performance
    const unsigned int aMortonCode = BVH::EncodeMortonCode(static_cast<unsigned int>(aVoxelX),
                                                           static_cast<unsigned int>(aVoxelY),
                                                           static_cast<unsigned int>(aVoxelZ));

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
