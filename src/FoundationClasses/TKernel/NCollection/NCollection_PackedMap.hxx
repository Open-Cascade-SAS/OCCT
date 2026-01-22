// Copyright (c) 2005-2026 OPEN CASCADE SAS
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

#ifndef NCollection_PackedMap_HeaderFile
#define NCollection_PackedMap_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OStream.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Primes.hxx>

#include <cstdint>
#include <cstring>
#include <type_traits>

//! @brief Optimized Map for integer values of various integral types.
//!
//! This template class provides a memory-efficient storage for sets of integers.
//! Each block of BitsPerBlock (32 or 64) consecutive integers is stored compactly
//! using bit manipulation. The block size is automatically selected based on
//! the integer type: 32 bits for int/unsigned, 64 bits for int64_t/size_t.
//!
//! @tparam IntType The integral type to store (int, unsigned int, int64_t, size_t, etc.)
template <typename IntType>
class NCollection_PackedMap
{
  static_assert(std::is_integral<IntType>::value,
                "NCollection_PackedMap requires an integral type");

public:
  DEFINE_STANDARD_ALLOC

  //! True if the integer type is larger than 32 bits
  static constexpr bool Is64Bit = sizeof(IntType) > 4;

  //! The block type for storing packed bits
  using BlockType = typename std::conditional<Is64Bit, uint64_t, uint32_t>::type;

  //! The index type for addressing blocks
  using IndexType = typename std::conditional<sizeof(IntType) <= 4, uint32_t, uint64_t>::type;

  //! Number of bits per block
  static constexpr int BitsPerBlock = Is64Bit ? 64 : 32;

private:
  //! Number of low bits used for position within a block
  static constexpr int MaskLowBits = Is64Bit ? 6 : 5;

  //! Mask for low bits (position within block)
  static constexpr IndexType MASK_LOW = (IndexType(1) << MaskLowBits) - 1;

  //! Mask for high bits (block base address)
  static constexpr IndexType MASK_HIGH = ~MASK_LOW;

  //! @brief Class implementing a block of consecutive integer values as a node.
  //!
  //! For 32-bit types: stores 32 consecutive values
  //! For 64-bit types: stores 64 consecutive values
  //!
  //! The data are stored as:
  //! - myMask: contains the count-1 in low bits and block base address in high bits
  //! - myData: bit field where each bit indicates presence of corresponding integer
  class PackedMapNode
  {
  public:
    PackedMapNode(PackedMapNode* thePtr = nullptr)
        : myNext(thePtr),
          myMask(0),
          myData(0)
    {
    }

    PackedMapNode(IntType theValue, PackedMapNode*& thePtr)
        : myNext(thePtr),
          myMask(static_cast<IndexType>(theValue) & MASK_HIGH),
          myData(BlockType(1) << (static_cast<IndexType>(theValue) & MASK_LOW))
    {
    }

    PackedMapNode(IndexType theMask, BlockType theData, PackedMapNode* thePtr)
        : myNext(thePtr),
          myMask(theMask),
          myData(theData)
    {
    }

    IndexType Mask() const { return myMask; }

    BlockType Data() const { return myData; }

    IndexType& ChangeMask() { return myMask; }

    BlockType& ChangeData() { return myData; }

    //! Compute the sequential index of this packed node in the map.
    IntType Key() const { return static_cast<IntType>(myMask & MASK_HIGH); }

    //! Return the number of set integer keys.
    size_t NbValues() const { return size_t(myMask & MASK_LOW) + 1; }

    //! Return TRUE if this packed node is not empty.
    bool HasValues() const { return (myData != 0); }

    //! Return TRUE if the given integer key is set within this packed node.
    bool HasValue(IntType theValue) const
    {
      return (myData & (BlockType(1) << (static_cast<IndexType>(theValue) & MASK_LOW))) != 0;
    }

    //! Add integer key to this packed node.
    //! @return TRUE if key has been added
    bool AddValue(IntType theValue)
    {
      const BlockType aValBit = BlockType(1) << (static_cast<IndexType>(theValue) & MASK_LOW);
      if ((myData & aValBit) == 0)
      {
        myData ^= aValBit;
        ++myMask;
        return true;
      }
      return false;
    }

    //! Delete integer key from this packed node.
    //! @return TRUE if key has been deleted
    bool DelValue(IntType theValue)
    {
      const BlockType aValBit = BlockType(1) << (static_cast<IndexType>(theValue) & MASK_LOW);
      if ((myData & aValBit) != 0)
      {
        myData ^= aValBit;
        myMask--;
        return true;
      }
      return false;
    }

    //! Return the next node having the same hash code.
    PackedMapNode* Next() const { return myNext; }

    //! Set the next node having the same hash code.
    void SetNext(PackedMapNode* theNext) { myNext = theNext; }

  public:
    //! Support of Map interface.
    size_t HashCode(size_t theUpper) const
    {
      return static_cast<size_t>(myMask >> MaskLowBits) % theUpper + 1;
    }

    //! Support of Map interface.
    bool IsEqual(IndexType theOther) const
    {
      return (myMask >> MaskLowBits) == (static_cast<IndexType>(theOther));
    }

  private:
    PackedMapNode* myNext;
    IndexType      myMask;
    BlockType      myData;
  };

public:
  //! Iterator of class NCollection_PackedMap.
  class Iterator
  {
  public:
    //! Empty Constructor.
    Iterator()
        : myBuckets(nullptr),
          myNode(nullptr),
          myNbBuckets(-1),
          myBucket(-1),
          myIntMask(~BlockType(0)),
          myKey(0)
    {
    }

    //! Constructor.
    Iterator(const NCollection_PackedMap& theMap)
        : myBuckets(theMap.myData1),
          myNode(nullptr),
          myNbBuckets(theMap.myData1 != nullptr ? theMap.myNbBuckets : -1),
          myBucket(-1),
          myIntMask(~BlockType(0))
    {
      next();
      myKey = myNode != nullptr ? NCollection_PackedMap::findNext(myNode, myIntMask) : 0;
    }

    //! Re-initialize with the same or another Map instance.
    void Initialize(const NCollection_PackedMap& theMap)
    {
      myBuckets   = theMap.myData1;
      myBucket    = -1;
      myNode      = nullptr;
      myNbBuckets = theMap.myData1 != nullptr ? theMap.myNbBuckets : -1;
      next();

      myIntMask = ~BlockType(0);
      myKey     = myNode != nullptr ? findNext(myNode, myIntMask) : 0;
    }

    //! Restart the iteration
    void Reset()
    {
      myBucket = -1;
      myNode   = nullptr;
      next();

      myIntMask = ~BlockType(0);
      myKey     = myNode != nullptr ? findNext(myNode, myIntMask) : 0;
    }

    //! Query the iterated key.
    IntType Key() const
    {
      Standard_NoSuchObject_Raise_if((myIntMask == ~BlockType(0)),
                                     "NCollection_PackedMap::Iterator::Key");
      return myKey;
    }

    //! Return TRUE if iterator points to the node.
    bool More() const { return myNode != nullptr; }

    //! Increment the iterator
    void Next()
    {
      for (; myNode != nullptr; next())
      {
        myKey = NCollection_PackedMap::findNext(myNode, myIntMask);
        if (myIntMask != ~BlockType(0))
        {
          break;
        }
      }
    }

  private:
    //! Go to the next bucket in the map.
    void next()
    {
      if (myBuckets == nullptr)
      {
        return;
      }

      if (myNode != nullptr)
      {
        myNode = myNode->Next();
      }

      while (myNode == nullptr)
      {
        ++myBucket;
        if (myBucket > myNbBuckets)
        {
          return;
        }
        myNode = myBuckets[myBucket];
      }
    }

  private:
    PackedMapNode** myBuckets;
    PackedMapNode*  myNode;
    int             myNbBuckets;
    int             myBucket;

    BlockType myIntMask; //!< all bits set above the iterated position
    IntType   myKey;     //!< Currently iterated key
  };

public:
  //! Constructor
  NCollection_PackedMap(const int theNbBuckets = 1)
      : myData1(nullptr),
        myNbBuckets(theNbBuckets),
        myNbPackedMapNodes(0),
        myExtent(0)
  {
  }

  //! Copy constructor
  NCollection_PackedMap(const NCollection_PackedMap& theOther)
      : myData1(nullptr),
        myNbBuckets(1),
        myNbPackedMapNodes(0),
        myExtent(0)
  {
    Assign(theOther);
  }

  NCollection_PackedMap& operator=(const NCollection_PackedMap& theOther)
  {
    return Assign(theOther);
  }

  //! Move constructor
  NCollection_PackedMap(NCollection_PackedMap&& theOther) noexcept
      : myData1(theOther.myData1),
        myNbBuckets(theOther.myNbBuckets),
        myNbPackedMapNodes(theOther.myNbPackedMapNodes),
        myExtent(theOther.myExtent)
  {
    theOther.myData1            = nullptr;
    theOther.myNbBuckets        = 1;
    theOther.myNbPackedMapNodes = 0;
    theOther.myExtent           = 0;
  }

  //! Move assignment operator
  NCollection_PackedMap& operator=(NCollection_PackedMap&& theOther) noexcept
  {
    if (this != &theOther)
    {
      Clear();
      myData1                     = theOther.myData1;
      myNbBuckets                 = theOther.myNbBuckets;
      myNbPackedMapNodes          = theOther.myNbPackedMapNodes;
      myExtent                    = theOther.myExtent;
      theOther.myData1            = nullptr;
      theOther.myNbBuckets        = 1;
      theOther.myNbPackedMapNodes = 0;
      theOther.myExtent           = 0;
    }
    return *this;
  }

  //! Assignment operator
  NCollection_PackedMap& Assign(const NCollection_PackedMap& theOther)
  {
    if (this != &theOther)
    {
      Clear();
      if (!theOther.IsEmpty())
      {
        ReSize(theOther.myNbPackedMapNodes);
        const int nBucketsSrc = theOther.myNbBuckets;
        const int nBuckets    = myNbBuckets;
        for (int i = 0; i <= nBucketsSrc; i++)
        {
          for (const PackedMapNode* p = theOther.myData1[i]; p != nullptr;)
          {
            const size_t aHashCode = p->HashCode(nBuckets);
            myData1[aHashCode]     = new PackedMapNode(p->Mask(), p->Data(), myData1[aHashCode]);
            ++myNbPackedMapNodes;
            p = p->Next();
          }
        }
      }
      myExtent = theOther.myExtent;
    }
    return *this;
  }

  //! Resize the map
  void ReSize(const int theNbBuckets)
  {
    int aNewBuck = NCollection_Primes::NextPrimeForMap(theNbBuckets);
    if (aNewBuck <= myNbBuckets)
    {
      if (!IsEmpty())
      {
        return;
      }
      aNewBuck = myNbBuckets;
    }

    PackedMapNode** aNewData = reinterpret_cast<PackedMapNode**>(
      Standard::AllocateOptimal((aNewBuck + 1) * sizeof(PackedMapNode*)));
    memset(aNewData, 0, (aNewBuck + 1) * sizeof(PackedMapNode*));
    if (myData1 != nullptr)
    {
      PackedMapNode** anOldData = myData1;
      for (int i = 0; i <= myNbBuckets; ++i)
      {
        for (PackedMapNode* p = anOldData[i]; p != nullptr;)
        {
          size_t         k = p->HashCode(aNewBuck);
          PackedMapNode* q = p->Next();
          p->SetNext(aNewData[k]);
          aNewData[k] = p;
          p           = q;
        }
      }
    }

    Standard::Free(myData1);
    myNbBuckets = aNewBuck;
    myData1     = aNewData;
  }

  //! Clear the map
  void Clear()
  {
    if (!IsEmpty())
    {
      for (int aBucketIter = 0; aBucketIter <= myNbBuckets; ++aBucketIter)
      {
        if (myData1[aBucketIter])
        {
          for (PackedMapNode* aSubNodeIter = myData1[aBucketIter]; aSubNodeIter != nullptr;)
          {
            PackedMapNode* q = aSubNodeIter->Next();
            delete aSubNodeIter;
            aSubNodeIter = q;
          }
        }
      }
    }

    myNbPackedMapNodes = 0;
    Standard::Free(myData1);
    myData1  = nullptr;
    myExtent = 0;
  }

  ~NCollection_PackedMap() { Clear(); }

  //! Add a key to the map
  //! @param[in] theKey the key to add
  //! @return true if the key was added, false if it already existed
  bool Add(const IntType theKey)
  {
    if (Resizable())
    {
      ReSize(myNbPackedMapNodes);
    }

    const IndexType aKeyInt     = packedKeyIndex(theKey);
    const size_t    aHashCode   = hashCode(aKeyInt, myNbBuckets);
    PackedMapNode*  aBucketHead = myData1[aHashCode];
    for (PackedMapNode* p = aBucketHead; p != nullptr; p = p->Next())
    {
      if (p->IsEqual(aKeyInt))
      {
        if (p->AddValue(theKey))
        {
          ++myExtent;
          return true;
        }
        return false;
      }
    }

    myData1[aHashCode] = new PackedMapNode(theKey, aBucketHead);
    ++myNbPackedMapNodes;
    ++myExtent;
    return true;
  }

  //! Check if the map contains a key
  //! @param[in] theKey the key to check
  //! @return true if the key is in the map
  bool Contains(const IntType theKey) const
  {
    if (IsEmpty())
    {
      return false;
    }

    const IndexType aKeyInt = packedKeyIndex(theKey);
    for (PackedMapNode* p = myData1[hashCode(aKeyInt, myNbBuckets)]; p != nullptr;)
    {
      if (p->IsEqual(aKeyInt))
      {
        return p->HasValue(theKey);
      }
      p = p->Next();
    }
    return false;
  }

  //! Remove a key from the map
  //! @param[in] theKey the key to remove
  //! @return true if the key was removed, false if it was not present
  bool Remove(const IntType theKey)
  {
    if (IsEmpty())
    {
      return false;
    }

    const IndexType aKeyInt     = packedKeyIndex(theKey);
    PackedMapNode*& aBucketHead = myData1[hashCode(aKeyInt, myNbBuckets)];
    PackedMapNode*  p           = aBucketHead;
    PackedMapNode*  q           = nullptr;
    while (p)
    {
      if (p->IsEqual(aKeyInt))
      {
        bool aResult = p->DelValue(theKey);
        if (aResult)
        {
          --myExtent;
          if (!p->HasValues())
          {
            --myNbPackedMapNodes;
            if (q != nullptr)
            {
              q->SetNext(p->Next());
            }
            else
            {
              aBucketHead = p->Next();
            }
            delete p;
          }
        }
        return aResult;
      }
      q = p;
      p = p->Next();
    }
    return false;
  }

  //! Returns the number of map buckets.
  int NbBuckets() const { return myNbBuckets; }

  //! Returns map extent.
  int Extent() const { return static_cast<int>(myExtent); }

  //! Returns TRUE if map is empty.
  bool IsEmpty() const { return myNbPackedMapNodes == 0; }

  //! Query the minimal contained key value.
  IntType GetMinimalMapped() const
  {
    if (IsEmpty())
    {
      return std::numeric_limits<IntType>::max();
    }

    IntType              aResult    = std::numeric_limits<IntType>::max();
    const PackedMapNode* pFoundNode = nullptr;
    for (int i = 0; i <= myNbBuckets; i++)
    {
      for (const PackedMapNode* p = myData1[i]; p != nullptr; p = p->Next())
      {
        const IntType aKey = p->Key();
        if (aResult > aKey)
        {
          aResult    = aKey;
          pFoundNode = p;
        }
      }
    }
    if (pFoundNode)
    {
      BlockType aFullMask = ~BlockType(0);
      aResult             = findNext(pFoundNode, aFullMask);
    }
    return aResult;
  }

  //! Query the maximal contained key value.
  IntType GetMaximalMapped() const
  {
    if (IsEmpty())
    {
      return std::numeric_limits<IntType>::lowest();
    }

    IntType              aResult    = std::numeric_limits<IntType>::lowest();
    const PackedMapNode* pFoundNode = nullptr;
    for (int i = 0; i <= myNbBuckets; i++)
    {
      for (const PackedMapNode* p = myData1[i]; p != nullptr; p = p->Next())
      {
        const IntType aKey = p->Key();
        if (aResult < aKey)
        {
          aResult    = aKey;
          pFoundNode = p;
        }
      }
    }
    if (pFoundNode)
    {
      BlockType aFullMask = ~BlockType(0);
      aResult             = findPrev(pFoundNode, aFullMask);
    }
    return aResult;
  }

public:
  //! @name Deprecated boolean operations (use NCollection_PackedMapAlgo instead)

  //! @deprecated Use NCollection_PackedMapAlgo::Union() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  void Union(const NCollection_PackedMap& theLeft, const NCollection_PackedMap& theRight);

  //! @deprecated Use NCollection_PackedMapAlgo::Unite() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool Unite(const NCollection_PackedMap& theOther);

  //! @deprecated Use NCollection_PackedMapAlgo::Intersection() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  void Intersection(const NCollection_PackedMap& theLeft, const NCollection_PackedMap& theRight);

  //! @deprecated Use NCollection_PackedMapAlgo::Intersect() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool Intersect(const NCollection_PackedMap& theOther);

  //! @deprecated Use NCollection_PackedMapAlgo::Subtraction() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  void Subtraction(const NCollection_PackedMap& theLeft, const NCollection_PackedMap& theRight);

  //! @deprecated Use NCollection_PackedMapAlgo::Subtract() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool Subtract(const NCollection_PackedMap& theOther);

  //! @deprecated Use NCollection_PackedMapAlgo::Difference() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  void Difference(const NCollection_PackedMap& theLeft, const NCollection_PackedMap& theRight);

  //! @deprecated Use NCollection_PackedMapAlgo::Differ() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool Differ(const NCollection_PackedMap& theOther);

  //! @deprecated Use NCollection_PackedMapAlgo::IsEqual() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool IsEqual(const NCollection_PackedMap& theOther) const;

  //! @deprecated Use NCollection_PackedMapAlgo::IsSubset() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool IsSubset(const NCollection_PackedMap& theOther) const;

  //! @deprecated Use NCollection_PackedMapAlgo::HasIntersection() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool HasIntersection(const NCollection_PackedMap& theOther) const;

  //! @deprecated Use NCollection_PackedMapAlgo::Contains() instead
  Standard_DEPRECATED("This method will be removed after OCCT 7.9 release. Use methods from "
                      "NCollection_PackedMapAlgo.hxx instead.")
  bool Contains(const NCollection_PackedMap& theOther) const;

protected:
  //! Returns TRUE if resizing the map should be considered.
  bool Resizable() const { return IsEmpty() || (myNbPackedMapNodes > myNbBuckets); }

  //! Return an integer index for specified key.
  static IndexType packedKeyIndex(IntType theKey)
  {
    return static_cast<IndexType>(theKey) >> MaskLowBits;
  }

  //! Compute hash code for a key index.
  static size_t hashCode(IndexType theKeyIndex, int theNbBuckets)
  {
    return static_cast<size_t>(theKeyIndex) % theNbBuckets + 1;
  }

  //! Compute the population (i.e., the number of non-zero bits) of the block.
  //! The population is stored decremented as it is defined in PackedMapNode.
  static size_t population(IndexType& theMask, BlockType theData)
  {
    if constexpr (Is64Bit)
    {
      // 64-bit population count
      uint64_t aRes = theData - ((theData >> 1) & 0x5555555555555555ULL);
      aRes          = (aRes & 0x3333333333333333ULL) + ((aRes >> 2) & 0x3333333333333333ULL);
      aRes          = (aRes + (aRes >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
      aRes          = aRes + (aRes >> 8);
      aRes          = aRes + (aRes >> 16);
      aRes          = aRes + (aRes >> 32);
      theMask       = (theMask & MASK_HIGH) | ((static_cast<IndexType>(aRes) - 1) & MASK_LOW);
      return size_t(aRes & 0x7f);
    }
    else
    {
      // 32-bit population count
      uint32_t aRes =
        static_cast<uint32_t>(theData) - ((static_cast<uint32_t>(theData) >> 1) & 0x55555555);
      aRes    = (aRes & 0x33333333) + ((aRes >> 2) & 0x33333333);
      aRes    = (aRes + (aRes >> 4)) & 0x0f0f0f0f;
      aRes    = aRes + (aRes >> 8);
      aRes    = aRes + (aRes >> 16);
      theMask = (theMask & MASK_HIGH) | ((aRes - 1) & MASK_LOW);
      return size_t(aRes & 0x3f);
    }
  }

  //! Find the smallest non-zero bit under the given mask.
  //! Outputs the new mask that does not contain the detected bit.
  static IntType findNext(const PackedMapNode* theNode, BlockType& theMask)
  {
    BlockType val    = theNode->Data() & theMask;
    int       nZeros = 0;
    if (val == 0)
    {
      theMask = ~BlockType(0); // void, nothing to do
    }
    else
    {
      BlockType aMask = ~BlockType(0);
      if constexpr (Is64Bit)
      {
        if ((val & 0x00000000ffffffffULL) == 0)
        {
          aMask  = 0xffffffff00000000ULL;
          nZeros = 32;
          val >>= 32;
        }
      }
      if ((val & 0x0000ffff) == 0)
      {
        aMask <<= 16;
        nZeros += 16;
        val >>= 16;
      }
      if ((val & 0x000000ff) == 0)
      {
        aMask <<= 8;
        nZeros += 8;
        val >>= 8;
      }
      if ((val & 0x0000000f) == 0)
      {
        aMask <<= 4;
        nZeros += 4;
        val >>= 4;
      }
      if ((val & 0x00000003) == 0)
      {
        aMask <<= 2;
        nZeros += 2;
        val >>= 2;
      }
      if ((val & 0x00000001) == 0)
      {
        aMask <<= 1;
        nZeros++;
      }
      theMask = (aMask << 1);
    }
    return static_cast<IntType>(nZeros) + theNode->Key();
  }

  //! Find the highest non-zero bit under the given mask.
  //! Outputs the new mask that does not contain the detected bit.
  static IntType findPrev(const PackedMapNode* theNode, BlockType& theMask)
  {
    BlockType val    = theNode->Data() & theMask;
    int       nZeros = 0;
    if (val == 0)
    {
      theMask = ~BlockType(0); // void, nothing to do
    }
    else
    {
      BlockType aMask = ~BlockType(0);
      if constexpr (Is64Bit)
      {
        if ((val & 0xffffffff00000000ULL) == 0)
        {
          aMask  = 0x00000000ffffffffULL;
          nZeros = 32;
          val <<= 32;
        }
      }
      if ((val & BlockType(0xffff0000) << (Is64Bit ? 32 : 0)) == 0)
      {
        aMask >>= 16;
        nZeros += 16;
        val <<= 16;
      }
      if ((val & BlockType(0xff000000) << (Is64Bit ? 32 : 0)) == 0)
      {
        aMask >>= 8;
        nZeros += 8;
        val <<= 8;
      }
      if ((val & BlockType(0xf0000000) << (Is64Bit ? 32 : 0)) == 0)
      {
        aMask >>= 4;
        nZeros += 4;
        val <<= 4;
      }
      if ((val & BlockType(0xc0000000) << (Is64Bit ? 32 : 0)) == 0)
      {
        aMask >>= 2;
        nZeros += 2;
        val <<= 2;
      }
      if ((val & BlockType(0x80000000) << (Is64Bit ? 32 : 0)) == 0)
      {
        aMask >>= 1;
        nZeros++;
      }
      theMask = (aMask >> 1);
    }
    return static_cast<IntType>((BitsPerBlock - 1) - nZeros) + theNode->Key();
  }

private:
  PackedMapNode** myData1;            //!< data array
  int             myNbBuckets;        //!< number of buckets (size of data array)
  int             myNbPackedMapNodes; //!< amount of packed map nodes
  size_t          myExtent;           //!< extent of this map (number of unpacked integer keys)
};

// Include algorithm header after class definition to avoid circular dependency
#include <NCollection_PackedMapAlgo.hxx>

// Implementation of deprecated methods
template <typename IntType>
void NCollection_PackedMap<IntType>::Union(const NCollection_PackedMap& theLeft,
                                           const NCollection_PackedMap& theRight)
{
  NCollection_PackedMapAlgo::Union(*this, theLeft, theRight);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::Unite(const NCollection_PackedMap& theOther)
{
  return NCollection_PackedMapAlgo::Unite(*this, theOther);
}

template <typename IntType>
void NCollection_PackedMap<IntType>::Intersection(const NCollection_PackedMap& theLeft,
                                                  const NCollection_PackedMap& theRight)
{
  NCollection_PackedMapAlgo::Intersection(*this, theLeft, theRight);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::Intersect(const NCollection_PackedMap& theOther)
{
  return NCollection_PackedMapAlgo::Intersect(*this, theOther);
}

template <typename IntType>
void NCollection_PackedMap<IntType>::Subtraction(const NCollection_PackedMap& theLeft,
                                                 const NCollection_PackedMap& theRight)
{
  NCollection_PackedMapAlgo::Subtraction(*this, theLeft, theRight);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::Subtract(const NCollection_PackedMap& theOther)
{
  return NCollection_PackedMapAlgo::Subtract(*this, theOther);
}

template <typename IntType>
void NCollection_PackedMap<IntType>::Difference(const NCollection_PackedMap& theLeft,
                                                const NCollection_PackedMap& theRight)
{
  NCollection_PackedMapAlgo::Difference(*this, theLeft, theRight);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::Differ(const NCollection_PackedMap& theOther)
{
  return NCollection_PackedMapAlgo::Differ(*this, theOther);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::IsEqual(const NCollection_PackedMap& theOther) const
{
  return NCollection_PackedMapAlgo::IsEqual(*this, theOther);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::IsSubset(const NCollection_PackedMap& theOther) const
{
  return NCollection_PackedMapAlgo::IsSubset(*this, theOther);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::HasIntersection(const NCollection_PackedMap& theOther) const
{
  return NCollection_PackedMapAlgo::HasIntersection(*this, theOther);
}

template <typename IntType>
bool NCollection_PackedMap<IntType>::Contains(const NCollection_PackedMap& theOther) const
{
  return NCollection_PackedMapAlgo::Contains(*this, theOther);
}

#endif // NCollection_PackedMap_HeaderFile
