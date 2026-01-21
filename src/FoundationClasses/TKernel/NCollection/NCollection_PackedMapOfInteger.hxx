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

#ifndef NCollection_PackedMapOfInteger_HeaderFile
#define NCollection_PackedMapOfInteger_HeaderFile

#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Integer.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Handle.hxx>

/**
 * Optimized Map of integer values. Each block of 32 integers is stored in 8 bytes in memory.
 */
class NCollection_PackedMapOfInteger
{
public:
  DEFINE_STANDARD_ALLOC

private:
  //! 5 lower bits
  static const unsigned int MASK_LOW = 0x001f;

  //! 27 upper bits
  static const unsigned int MASK_HIGH = ~MASK_LOW;

  //! Class implementing a block of 32 consecutive integer values as a node of a Map collection.
  //! The data are stored in 64 bits as:
  //!  - bits  0 - 4 : (number of integers stored in the block) - 1;
  //!  - bits  5 - 31: base address of the block of integers (low bits assumed 0)
  //!  - bits 32 - 63: 32-bit field where each bit indicates the presence of the corresponding
  //!  integer in the block.
  //!                  Number of non-zero bits must be equal to the number expressed in bits 0-4.
  class NCollection_intMapNode
  {
  public:
    NCollection_intMapNode(NCollection_intMapNode* thePtr = nullptr)
        : myNext(thePtr),
          myMask(0),
          myData(0)
    {
    }

    NCollection_intMapNode(int theValue, NCollection_intMapNode*& thePtr)
        : myNext(thePtr),
          myMask((unsigned int)(theValue & MASK_HIGH)),
          myData(1 << (theValue & MASK_LOW))
    {
    }

    NCollection_intMapNode(unsigned int            theMask,
                           unsigned int            theData,
                           NCollection_intMapNode* thePtr)
        : myNext(thePtr),
          myMask(theMask),
          myData(theData)
    {
    }

    unsigned int Mask() const { return myMask; }

    unsigned int Data() const { return myData; }

    unsigned int& ChangeMask() { return myMask; }

    unsigned int& ChangeData() { return myData; }

    //! Compute the sequential index of this packed node in the map.
    int Key() const { return int(myMask & MASK_HIGH); }

    //! Return the number of set integer keys.
    size_t NbValues() const { return size_t(myMask & MASK_LOW) + 1; }

    //! Return TRUE if this packed node is not empty.
    bool HasValues() const { return (myData != 0); }

    //! Return TRUE if the given integer key is set within this packed node.
    int HasValue(int theValue) const { return (myData & (1 << (theValue & MASK_LOW))); }

    //! Add integer key to this packed node.
    //! @return TRUE if key has been added
    bool AddValue(int theValue)
    {
      const int aValInt = (1 << (theValue & MASK_LOW));
      if ((myData & aValInt) == 0)
      {
        myData ^= aValInt;
        ++myMask;
        return true;
      }
      return false;
    }

    //! Delete integer key from this packed node.
    //! @return TRUE if key has been deleted
    bool DelValue(int theValue)
    {
      const int aValInt = (1 << (theValue & MASK_LOW));
      if ((myData & aValInt) != 0)
      {
        myData ^= aValInt;
        myMask--;
        return true;
      }
      return false;
    }

    //! Find the smallest non-zero bit under the given mask. Outputs the new mask
    //! that does not contain the detected bit.
    int FindNext(unsigned int& theMask) const;

    //! Return the next node having the same hash code.
    NCollection_intMapNode* Next() const { return myNext; }

    //! Set the next node having the same hash code.
    void SetNext(NCollection_intMapNode* theNext) { myNext = theNext; }

  public:
    //! Support of Map interface.
    int HashCode(int theUpper) const { return (myMask >> 5) % theUpper + 1; }

    //! Support of Map interface.
    bool IsEqual(int theOther) const { return ((myMask >> 5) == (unsigned)theOther); }

  private:
    NCollection_intMapNode* myNext;
    unsigned int            myMask;
    unsigned int            myData;
  };

public:
  //! Iterator of class NCollection_PackedMapOfInteger.
  class Iterator
  {
  public:
    /// Empty Constructor.
    Iterator()
        : myBuckets(nullptr),
          myNode(nullptr),
          myNbBuckets(-1),
          myBucket(-1),
          myIntMask(~0U),
          myKey(0)
    {
    }

    /// Constructor.
    Iterator(const NCollection_PackedMapOfInteger& theMap)
        : myBuckets(theMap.myData1),
          myNode(nullptr),
          myNbBuckets(theMap.myData1 != nullptr ? theMap.myNbBuckets : -1),
          myBucket(-1),
          myIntMask(~0U)
    {
      next();
      myKey = myNode != nullptr ? NCollection_intMapNode_findNext(myNode, myIntMask) : 0;
    }

    //! Re-initialize with the same or another Map instance.
    void Initialize(const NCollection_PackedMapOfInteger& theMap)
    {
      myBuckets   = theMap.myData1;
      myBucket    = -1;
      myNode      = nullptr;
      myNbBuckets = theMap.myData1 != nullptr ? theMap.myNbBuckets : -1;
      next();

      myIntMask = ~0U;
      myKey     = myNode != nullptr ? NCollection_intMapNode_findNext(myNode, myIntMask) : 0;
    }

    //! Restart the iteration
    void Reset()
    {
      myBucket = -1;
      myNode   = nullptr;
      next();

      myIntMask = ~0U;
      myKey     = myNode != nullptr ? NCollection_intMapNode_findNext(myNode, myIntMask) : 0;
    }

    //! Query the iterated key.
    int Key() const
    {
      Standard_NoSuchObject_Raise_if((myIntMask == ~0U),
                                     "NCollection_PackedMapOfInteger::Iterator::Key");
      return myKey;
    }

    //! Return TRUE if iterator points to the node.
    bool More() const { return myNode != nullptr; }

    //! Increment the iterator
    void Next()
    {
      for (; myNode != nullptr; next())
      {
        myKey = NCollection_intMapNode_findNext(myNode, myIntMask);
        if (myIntMask != ~0u)
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
    NCollection_intMapNode** myBuckets;
    NCollection_intMapNode*  myNode;
    int                      myNbBuckets;
    int                      myBucket;

    unsigned int myIntMask; //!< all bits set above the iterated position
    int          myKey;     //!< Currently iterated key
  };

public:
  //! Constructor
  NCollection_PackedMapOfInteger(const int theNbBuckets = 1)
      : myData1(nullptr),
        myNbBuckets(theNbBuckets),
        myNbPackedMapNodes(0),
        myExtent(0)
  {
  }

  //! Copy constructor
  NCollection_PackedMapOfInteger(const NCollection_PackedMapOfInteger& theOther)
      : myData1(nullptr),
        myNbBuckets(1),
        myNbPackedMapNodes(0),
        myExtent(0)
  {
    Assign(theOther);
  }

  inline NCollection_PackedMapOfInteger& operator=(const NCollection_PackedMapOfInteger& Other)
  {
    return Assign(Other);
  }

  //! Move constructor
  NCollection_PackedMapOfInteger(NCollection_PackedMapOfInteger&& theOther) noexcept
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
  NCollection_PackedMapOfInteger& operator=(NCollection_PackedMapOfInteger&& theOther) noexcept
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

  Standard_EXPORT NCollection_PackedMapOfInteger& Assign(const NCollection_PackedMapOfInteger&);
  Standard_EXPORT void                            ReSize(const int NbBuckets);
  Standard_EXPORT void                            Clear();

  ~NCollection_PackedMapOfInteger() { Clear(); }

  Standard_EXPORT bool Add(const int aKey);
  Standard_EXPORT bool Contains(const int aKey) const;
  Standard_EXPORT bool Remove(const int aKey);

  //! Returns the number of map buckets (not that since integers are packed in this map, the number
  //! is smaller than extent).
  int NbBuckets() const { return myNbBuckets; }

  //! Returns map extent.
  int Extent() const { return int(myExtent); }

  //! Returns TRUE if map is empty.
  bool IsEmpty() const { return myNbPackedMapNodes == 0; }

  /**
   * Query the minimal contained key value.
   */
  Standard_EXPORT int GetMinimalMapped() const;

  /**
   * Query the maximal contained key value.
   */
  Standard_EXPORT int GetMaximalMapped() const;

  //! Prints useful statistics about the map.
  //! It can be used to test the quality of the hashcoding.
  Standard_EXPORT void Statistics(Standard_OStream& theStream) const;

public:
  //!@name Boolean operations with maps as sets of integers
  //!@{

  /**
   * Sets this Map to be the result of union (aka addition, fuse, merge, boolean OR) operation
   * between two given Maps. The new Map contains the values that are contained either in the first
   * map or in the second map or in both. All previous contents of this Map is cleared. This map
   * (result of the boolean operation) can also be passed as one of operands.
   */
  Standard_EXPORT void Union(const NCollection_PackedMapOfInteger&,
                             const NCollection_PackedMapOfInteger&);

  /**
   * Apply to this Map the boolean operation union (aka addition, fuse, merge, boolean OR) with
   * another (given) Map. The result contains the values that were previously contained in this map
   * or contained in the given (operand) map. This algorithm is similar to method Union().
   * @return True if content of this map is changed
   */
  Standard_EXPORT bool Unite(const NCollection_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Unite().
   */
  NCollection_PackedMapOfInteger& operator|=(const NCollection_PackedMapOfInteger& MM)
  {
    Unite(MM);
    return *this;
  }

  /**
   * Sets this Map to be the result of intersection (aka multiplication, common, boolean AND)
   * operation between two given Maps. The new Map contains only the values that are contained in
   * both map operands. All previous contents of this Map is cleared. This same map (result of the
   * boolean operation) can also be used as one of operands. The order of operands makes no
   * difference; the method minimizes internally the number of iterations using the smallest map for
   * the loop.
   */
  Standard_EXPORT void Intersection(const NCollection_PackedMapOfInteger&,
                                    const NCollection_PackedMapOfInteger&);

  /**
   * Apply to this Map the intersection operation (aka multiplication, common,  boolean AND) with
   * another (given) Map. The result contains only the values that are contained in both this and
   * the given maps. This algorithm is similar to method Intersection().
   * @return True if content of this map is changed
   */
  Standard_EXPORT bool Intersect(const NCollection_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Intersect().
   */
  NCollection_PackedMapOfInteger& operator&=(const NCollection_PackedMapOfInteger& MM)
  {
    Intersect(MM);
    return *this;
  }

  /**
   * Sets this Map to be the result of subtraction
   * (aka set-theoretic difference, relative complement, exclude, cut, boolean NOT) operation
   * between two given Maps. The new Map contains only the values that are contained in the first
   * map operands and not contained in the second one. All previous contents of this Map is cleared.
   * This map (result of the boolean operation) can also be used as the first operand.
   */
  Standard_EXPORT void Subtraction(const NCollection_PackedMapOfInteger&,
                                   const NCollection_PackedMapOfInteger&);

  /**
   * Apply to this Map the subtraction (aka set-theoretic difference, relative complement, exclude,
   * cut, boolean NOT) operation with another (given) Map. The result contains only the values that
   * were previously contained in this map and not contained in this map. This algorithm is similar
   * to method Subtract() with two operands.
   * @return True if contents of this map is changed
   */
  Standard_EXPORT bool Subtract(const NCollection_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Subtract().
   */
  NCollection_PackedMapOfInteger& operator-=(const NCollection_PackedMapOfInteger& MM)
  {
    Subtract(MM);
    return *this;
  }

  /**
   * Sets this Map to be the result of symmetric difference (aka exclusive disjunction, boolean XOR)
   * operation between two given Maps. The new Map contains the values that are contained only in
   * the first or the second operand maps but not in both. All previous contents of this Map is
   * cleared. This map (result of the boolean operation) can also be used as one of operands.
   */
  Standard_EXPORT void Difference(const NCollection_PackedMapOfInteger&,
                                  const NCollection_PackedMapOfInteger&);

  /**
   * Apply to this Map the symmetric difference (aka exclusive disjunction, boolean XOR) operation
   * with another (given) Map. The result contains the values that are contained only in this or the
   * operand map, but not in both. This algorithm is similar to method Difference().
   * @return True if contents of this map is changed
   */
  Standard_EXPORT bool Differ(const NCollection_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Differ().
   */
  NCollection_PackedMapOfInteger& operator^=(const NCollection_PackedMapOfInteger& MM)
  {
    Differ(MM);
    return *this;
  }

  /**
   * Returns True if this map is equal to the given one, i.e. they contain the
   * same sets of elements
   */
  Standard_EXPORT bool IsEqual(const NCollection_PackedMapOfInteger&) const;

  /**
   * Overloaded operator version of IsEqual().
   */
  bool operator==(const NCollection_PackedMapOfInteger& MM) const { return IsEqual(MM); }

  /**
   * Returns True if this map is subset of the given one, i.e. all elements
   * contained in this map is contained also in the operand map.
   * if this map is empty that this method returns true for any operand map.
   */
  Standard_EXPORT bool IsSubset(const NCollection_PackedMapOfInteger&) const;

  /**
   * Overloaded operator version of IsSubset().
   */
  bool operator<=(const NCollection_PackedMapOfInteger& MM) const { return IsSubset(MM); }

  /**
   * Returns True if this map has common items with the given one.
   */
  Standard_EXPORT bool HasIntersection(const NCollection_PackedMapOfInteger&) const;

  //!@}

protected:
  //! Returns TRUE if resizing the map should be considered.
  bool Resizable() const { return IsEmpty() || (myNbPackedMapNodes > myNbBuckets); }

  //! Return an integer index for specified key.
  static int packedKeyIndex(int theKey) { return (unsigned)theKey >> 5; }

private:
  //! Find the smallest non-zero bit under the given mask.
  //! Outputs the new mask that does not contain the detected bit.
  Standard_EXPORT static int NCollection_intMapNode_findNext(const NCollection_intMapNode* theNode,
                                                             unsigned int&                 theMask);

  //! Find the highest non-zero bit under the given mask.
  //! Outputs the new mask that does not contain the detected bit.
  Standard_EXPORT static int NCollection_intMapNode_findPrev(const NCollection_intMapNode* theNode,
                                                             unsigned int&                 theMask);

  //! Compute the population (i.e., the number of non-zero bits) of the 32-bit word theData.
  //! The population is stored decremented as it is defined in NCollection_intMapNode.
  //! Source: H.S.Warren, Hacker's Delight, Addison-Wesley Inc. 2002, Ch.5.1
  static size_t NCollection_Population(unsigned int& theMask, unsigned int theData)
  {
    unsigned int aRes = theData - ((theData >> 1) & 0x55555555);
    aRes              = (aRes & 0x33333333) + ((aRes >> 2) & 0x33333333);
    aRes              = (aRes + (aRes >> 4)) & 0x0f0f0f0f;
    aRes              = aRes + (aRes >> 8);
    aRes              = aRes + (aRes >> 16);
    theMask           = (theMask & NCollection_PackedMapOfInteger::MASK_HIGH)
              | ((aRes - 1) & NCollection_PackedMapOfInteger::MASK_LOW);
    return size_t(aRes & 0x3f);
  }

private:
  NCollection_intMapNode** myData1;            //!< data array
  int                      myNbBuckets;        //!< number of buckets (size of data array)
  int                      myNbPackedMapNodes; //!< amount of packed map nodes
  size_t                   myExtent; //!< extent of this map (number of unpacked integer keys)
};

#endif
