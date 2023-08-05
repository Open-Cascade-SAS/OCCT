// Created on: 2005-11-05
// Created by: Alexander GRIGORIEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef TColStd_PackedMapOfInteger_HeaderFile
#define TColStd_PackedMapOfInteger_HeaderFile

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
class TColStd_PackedMapOfInteger
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
  //!  - bits 32 - 63: 32-bit field where each bit indicates the presence of the corresponding integer in the block.
  //!                  Number of non-zero bits must be equal to the number expressed in bits 0-4.
  class TColStd_intMapNode
  {
  public:
    TColStd_intMapNode (TColStd_intMapNode* thePtr = NULL)
    : myNext (thePtr), myMask (0), myData (0) {}

    TColStd_intMapNode (Standard_Integer theValue, TColStd_intMapNode*& thePtr)
    : myNext (thePtr),
      myMask ((unsigned int) (theValue & MASK_HIGH)),
      myData (1 << (theValue & MASK_LOW)) {}

    TColStd_intMapNode (unsigned int theMask, unsigned int theData, TColStd_intMapNode* thePtr)
    : myNext (thePtr),
      myMask (theMask),
      myData (theData) {}

    unsigned int Mask() const { return myMask; }

    unsigned int Data() const { return myData; }

    unsigned int& ChangeMask() { return myMask; }

    unsigned int& ChangeData() { return myData; }

    //! Compute the sequential index of this packed node in the map.
    Standard_Integer Key() const { return Standard_Integer (myMask & MASK_HIGH); }

    //! Return the number of set integer keys.
    size_t NbValues() const { return size_t(myMask & MASK_LOW) + 1; }

    //! Return TRUE if this packed node is not empty.
    Standard_Boolean HasValues() const { return (myData != 0); }

    //! Return TRUE if the given integer key is set within this packed node.
    Standard_Integer HasValue (Standard_Integer theValue) const { return (myData & (1 << (theValue & MASK_LOW))); }

    //! Add integer key to this packed node.
    //! @return TRUE if key has been added
    Standard_Boolean AddValue (Standard_Integer theValue)
    {
      const Standard_Integer aValInt = (1 << (theValue & MASK_LOW));
      if ((myData & aValInt) == 0)
      {
        myData ^= aValInt;
        ++myMask;
        return Standard_True;
      }
      return Standard_False;
    }

    //! Delete integer key from this packed node.
    //! @return TRUE if key has been deleted
    Standard_Boolean DelValue (Standard_Integer theValue)
    {
      const Standard_Integer aValInt = (1 << (theValue & MASK_LOW));
      if ((myData & aValInt) != 0)
      {
        myData ^= aValInt;
        myMask--;
        return Standard_True;
      }
      return Standard_False;
    }

    //! Find the smallest non-zero bit under the given mask. Outputs the new mask
    //! that does not contain the detected bit.
    Standard_Integer FindNext (unsigned int& theMask) const;

    //! Return the next node having the same hash code.
    TColStd_intMapNode* Next() const { return myNext; }

    //! Set the next node having the same hash code.
    void SetNext (TColStd_intMapNode* theNext) { myNext = theNext; }

  public:
    //! Support of Map interface.
    Standard_Integer HashCode (Standard_Integer theUpper) const
    {
      return (myMask >> 5) % theUpper + 1;
    }

    //! Support of Map interface.
    Standard_Boolean IsEqual (Standard_Integer theOther) const
    {
      return ((myMask >> 5) == (unsigned)theOther);
    }

  private:
    TColStd_intMapNode* myNext;
    unsigned int myMask;
    unsigned int myData;
  };

public:

  //! Iterator of class TColStd_PackedMapOfInteger.
  class Iterator
  {
  public:

    /// Empty Constructor.
    Iterator()
    : myBuckets (NULL),
      myNode (NULL),
      myNbBuckets (-1),
      myBucket (-1),
      myIntMask (~0U),
      myKey (0) {}

    /// Constructor.
    Iterator (const TColStd_PackedMapOfInteger& theMap)
    : myBuckets (theMap.myData1),
      myNode (NULL),
      myNbBuckets (theMap.myData1 != NULL ? theMap.myNbBuckets : -1),
      myBucket (-1),
      myIntMask (~0U)
    {
      next();
      myKey = myNode != NULL ? TColStd_intMapNode_findNext (myNode, myIntMask) : 0;
    }

    //! Re-initialize with the same or another Map instance.
    void Initialize (const TColStd_PackedMapOfInteger& theMap)
    {
      myBuckets = theMap.myData1;
      myBucket = -1;
      myNode = NULL;
      myNbBuckets = theMap.myData1 != NULL ? theMap.myNbBuckets : -1;
      next();

      myIntMask = ~0U;
      myKey = myNode != NULL ? TColStd_intMapNode_findNext (myNode, myIntMask) : 0;
    }

    //! Restart the iteration
    void Reset()
    {
      myBucket = -1;
      myNode = NULL;
      next();

      myIntMask = ~0U;
      myKey = myNode != NULL ? TColStd_intMapNode_findNext (myNode, myIntMask) : 0;
    }

    //! Query the iterated key.
    Standard_Integer Key() const
    {
      Standard_NoSuchObject_Raise_if ((myIntMask == ~0U), "TColStd_MapIteratorOfPackedMapOfInteger::Key");
      return myKey;
    }

    //! Return TRUE if iterator points to the node.
    Standard_Boolean More() const { return myNode != NULL; }

    //! Increment the iterator
    void Next()
    {
      for (; myNode != NULL; next())
      {
        myKey = TColStd_intMapNode_findNext (myNode, myIntMask);
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
      if (myBuckets == NULL)
      {
        return;
      }

      if (myNode != NULL)
      {
        myNode = myNode->Next();
      }

      while (myNode == NULL)
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
    TColStd_intMapNode** myBuckets;
    TColStd_intMapNode*  myNode;
    Standard_Integer myNbBuckets;
    Standard_Integer myBucket;

    unsigned int     myIntMask; //!< all bits set above the iterated position
    Standard_Integer myKey;     //!< Currently iterated key
  };

public:

  //! Constructor
  TColStd_PackedMapOfInteger (const Standard_Integer theNbBuckets = 1)
  : myData1 (NULL),
    myNbBuckets (theNbBuckets),
    myNbPackedMapNodes (0),
    myExtent (0) {}

  //! Copy constructor
  TColStd_PackedMapOfInteger (const TColStd_PackedMapOfInteger& theOther)
  : myData1 (NULL),
    myNbBuckets (1),
    myNbPackedMapNodes (0),
    myExtent (0)
  {
    Assign (theOther);
  }

  inline TColStd_PackedMapOfInteger&
                          operator =  (const TColStd_PackedMapOfInteger& Other) 
  { return Assign(Other); }

  Standard_EXPORT TColStd_PackedMapOfInteger&
                          Assign        (const TColStd_PackedMapOfInteger&);
  Standard_EXPORT  void   ReSize        (const Standard_Integer NbBuckets);
  Standard_EXPORT  void   Clear         ();
  ~TColStd_PackedMapOfInteger() { Clear(); }
  Standard_EXPORT  Standard_Boolean
                          Add           (const Standard_Integer aKey);
  Standard_EXPORT  Standard_Boolean
                          Contains      (const Standard_Integer aKey) const;
  Standard_EXPORT  Standard_Boolean
                          Remove        (const Standard_Integer aKey);

  //! Returns the number of map buckets (not that since integers are packed in this map, the number is smaller than extent).
  Standard_Integer NbBuckets() const { return myNbBuckets; }

  //! Returns map extent.
  Standard_Integer Extent() const { return Standard_Integer (myExtent); }

  //! Returns TRUE if map is empty.
  Standard_Boolean IsEmpty() const { return myNbPackedMapNodes == 0; }

  /**
   * Query the minimal contained key value.
   */
  Standard_EXPORT Standard_Integer GetMinimalMapped () const;

  /**
   * Query the maximal contained key value.
   */
  Standard_EXPORT Standard_Integer GetMaximalMapped () const;

  //! Prints useful statistics about the map.
  //! It can be used to test the quality of the hashcoding.
  Standard_EXPORT void Statistics (Standard_OStream& theStream) const;

public:
  //!@name Boolean operations with maps as sets of integers
  //!@{

  /**
   * Sets this Map to be the result of union (aka addition, fuse, merge, boolean OR) operation between two given Maps.
   * The new Map contains the values that are contained either in the first map or in the second map or in both.
   * All previous contents of this Map is cleared. This map (result of the boolean operation) can also be passed as one of operands.
   */
  Standard_EXPORT void Union (const TColStd_PackedMapOfInteger&,
                              const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the boolean operation union (aka addition, fuse, merge, boolean OR) with another (given) Map.
   * The result contains the values that were previously contained in this map or contained in the given (operand) map.
   * This algorithm is similar to method Union().
   * @return True if content of this map is changed
   */
  Standard_EXPORT Standard_Boolean Unite (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Unite().
   */
  TColStd_PackedMapOfInteger& operator |= (const TColStd_PackedMapOfInteger& MM)
  { Unite(MM); return *this; }

  /**
   * Sets this Map to be the result of intersection (aka multiplication, common, boolean AND) operation between two given Maps.
   * The new Map contains only the values that are contained in both map operands.
   * All previous contents of this Map is cleared. This same map (result of the boolean operation) can also be used as one of operands.
   * The order of operands makes no difference; the method minimizes internally the number of iterations using the smallest map for the loop.
   */
  Standard_EXPORT void Intersection (const TColStd_PackedMapOfInteger&,
                                     const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the intersection operation (aka multiplication, common,  boolean AND) with another (given) Map.
   * The result contains only the values that are contained in both this and the given maps.
   * This algorithm is similar to method Intersection().
   * @return True if content of this map is changed
   */
  Standard_EXPORT Standard_Boolean Intersect (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Intersect().
   */
  TColStd_PackedMapOfInteger& operator &= (const TColStd_PackedMapOfInteger& MM)
  { Intersect(MM); return *this; }

  /**
   * Sets this Map to be the result of subtraction
   * (aka set-theoretic difference, relative complement, exclude, cut, boolean NOT) operation between two given Maps.
   * The new Map contains only the values that are contained in the first map operands and not contained in the second one.
   * All previous contents of this Map is cleared.
   * This map (result of the boolean operation) can also be used as the first operand.
   */
  Standard_EXPORT void Subtraction (const TColStd_PackedMapOfInteger&,
                                    const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the subtraction (aka set-theoretic difference, relative complement, exclude, cut, boolean NOT) operation with another (given) Map.
   * The result contains only the values that were previously contained in this map and not contained in this map.
   * This algorithm is similar to method Subtract() with two operands.
   * @return True if contents of this map is changed
   */
  Standard_EXPORT Standard_Boolean Subtract (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Subtract().
   */
  TColStd_PackedMapOfInteger& operator -= (const TColStd_PackedMapOfInteger& MM)
  { Subtract(MM); return *this; }

  /**
   * Sets this Map to be the result of symmetric difference (aka exclusive disjunction, boolean XOR) operation between two given Maps.
   * The new Map contains the values that are contained only in the first or the second operand maps but not in both.
   * All previous contents of this Map is cleared.
   * This map (result of the boolean operation) can also be used as one of operands.
   */
  Standard_EXPORT void Difference (const TColStd_PackedMapOfInteger&,
                                   const TColStd_PackedMapOfInteger&);

  /**
   * Apply to this Map the symmetric difference (aka exclusive disjunction, boolean XOR) operation with another (given) Map.
   * The result contains the values that are contained only in this or the operand map, but not in both.
   * This algorithm is similar to method Difference().
   * @return True if contents of this map is changed
   */
  Standard_EXPORT Standard_Boolean Differ (const TColStd_PackedMapOfInteger&);

  /**
   * Overloaded operator version of Differ().
   */
  TColStd_PackedMapOfInteger& operator ^= (const TColStd_PackedMapOfInteger& MM)
  { Differ(MM); return *this; }

  /**
   * Returns True if this map is equal to the given one, i.e. they contain the
   * same sets of elements
   */
  Standard_EXPORT Standard_Boolean IsEqual (const TColStd_PackedMapOfInteger&) const;

  /**
   * Overloaded operator version of IsEqual().
   */
  Standard_Boolean operator == (const TColStd_PackedMapOfInteger& MM) const
  { return IsEqual(MM); }

  /**
   * Returns True if this map is subset of the given one, i.e. all elements 
   * contained in this map is contained also in the operand map.
   * if this map is empty that this method returns true for any operand map.
   */
  Standard_EXPORT Standard_Boolean IsSubset (const TColStd_PackedMapOfInteger&) const;

  /**
   * Overloaded operator version of IsSubset().
   */
  Standard_Boolean operator <= (const TColStd_PackedMapOfInteger& MM) const
  { return IsSubset(MM); }

  /**
   * Returns True if this map has common items with the given one.
   */
  Standard_EXPORT Standard_Boolean HasIntersection (const TColStd_PackedMapOfInteger&) const;

  //!@}
  
 protected:

   //! Returns TRUE if resizing the map should be considered.
   Standard_Boolean Resizable() const { return IsEmpty() || (myNbPackedMapNodes > myNbBuckets); }

   //! Return an integer index for specified key.
   static Standard_Integer packedKeyIndex (Standard_Integer theKey) { return (unsigned)theKey >> 5; }

private:

  //! Find the smallest non-zero bit under the given mask.
  //! Outputs the new mask that does not contain the detected bit.
  Standard_EXPORT static Standard_Integer TColStd_intMapNode_findNext (const TColStd_intMapNode* theNode,
                                                                       unsigned int& theMask);

  //! Find the highest non-zero bit under the given mask.
  //! Outputs the new mask that does not contain the detected bit.
  Standard_EXPORT static Standard_Integer TColStd_intMapNode_findPrev (const TColStd_intMapNode* theNode,
                                                                       unsigned int& theMask);

  //! Compute the population (i.e., the number of non-zero bits) of the 32-bit word theData.
  //! The population is stored decremented as it is defined in TColStd_intMapNode.
  //! Source: H.S.Warren, Hacker's Delight, Addison-Wesley Inc. 2002, Ch.5.1
  static size_t TColStd_Population (unsigned int& theMask, unsigned int theData)
  {
    unsigned int aRes = theData - ((theData>>1) & 0x55555555);
    aRes  = (aRes & 0x33333333) + ((aRes>>2)    & 0x33333333);
    aRes  = (aRes + (aRes>>4)) & 0x0f0f0f0f;
    aRes  = aRes + (aRes>>8);
    aRes  = aRes + (aRes>>16);
    theMask = (theMask & TColStd_PackedMapOfInteger::MASK_HIGH) | ((aRes - 1) & TColStd_PackedMapOfInteger::MASK_LOW);
    return size_t(aRes & 0x3f);
  }

private:

  TColStd_intMapNode** myData1;            //!< data array
  Standard_Integer     myNbBuckets;        //!< number of buckets (size of data array)
  Standard_Integer     myNbPackedMapNodes; //!< amount of packed map nodes
  Standard_Size        myExtent;           //!< extent of this map (number of unpacked integer keys)
};

#endif
