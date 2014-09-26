// Created on: 2007-01-23
// Created by: Andrey BETENEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef NCollection_SparseArrayBase_HeaderFile
#define NCollection_SparseArrayBase_HeaderFile

#include <Standard.hxx>
#include <Standard_OutOfRange.hxx>

typedef size_t Standard_Size;

/**
* Base class for NCollection_SparseArray;  
* provides non-template implementation of general mechanics
* of block allocation, items creation / deletion etc.
*/

class NCollection_SparseArrayBase 
{
public:
  //!@name Type-independent public interface 
  //!@{

  //! Clears all the data
  Standard_EXPORT void Clear ();

  //! Returns number of currently contained items
  Standard_Size Size () const { return mySize; }

  //! Check whether the value at given index is set
  Standard_EXPORT Standard_Boolean HasValue (const Standard_Size theIndex) const;

  //! Deletes the item from the array; 
  //! returns True if that item was defined
  Standard_EXPORT Standard_Boolean UnsetValue (const Standard_Size theIndex);

  //!@}

#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
public: // work-around against obsolete SUN WorkShop 5.3 compiler
#else
private:
#endif

  /**
   * The block of data contains array of items, counter 
   * and bit field, allocated as single piece of memory addressed
   * from the blocks array (myData).
   * 
   * The Block structure provides a logical view on the block,
   * and provides methods to work with bit map.
   *
   * Note that NCollection_SparseArrayBase class takes responsibility 
   * for correct allocation/deallocation of all the data.
   */

  class Block {
  public:

    typedef unsigned char Cell; //!< type of items used to hold bits

    //! Number of bits in each cell
    static Standard_Size BitsPerCell() { return sizeof(Cell) * 8/*BITSPERBYTE*/; } 

  public:

    //! Initializes the block by pointer to block data 
    Block (const Standard_Address theAddr, const Standard_Size theNbItems, 
           const Standard_Size theItemSize)
      : Count((Standard_Size*)theAddr),
        Array((char*)theAddr + sizeof(Standard_Size)), 
        Bits ((Cell*)((char*)theAddr + sizeof(Standard_Size) + theNbItems * theItemSize))
    {
    }

    //! Compute required size for block data, in bytes
    static Standard_Size Size (const Standard_Size theNbItems, 
			       const Standard_Size theItemSize) 
    {
      return sizeof(Standard_Size) + 
             sizeof(Cell) * ( (theNbItems + BitsPerCell() - 1) / BitsPerCell() ) +
             theNbItems * theItemSize;
    }

    //! Returns address of array from address of block
    static char* ToArray (const Standard_Address theAddress, 
			  const Standard_Size /*theNbItems*/, 
			  const Standard_Size /*theItemSize*/) 
    {
      return (char*)theAddress + sizeof(Standard_Size);
    }

  public:

    //! Set bit for i-th item; returns non-null if that bit has 
    //! not been set previously
    Cell Set (Standard_Size i) 
    {
      Cell* abyte = Bits + i / BitsPerCell();
      Cell  amask = (Cell)( '\1' << ( i % BitsPerCell() ) );
      Cell  anold = (Cell)( *abyte & amask );
      *abyte = (Cell)( *abyte | amask );
      return ! anold;
    }

    //! Check bit for i-th item; returns non-null if that bit is set
    Cell IsSet (Standard_Size i) 
    {
      Cell* abyte = Bits + i / BitsPerCell();
      Cell  amask = (Cell)( '\1' << ( i % BitsPerCell() ) );
      return (Cell)( *abyte & amask );
    }

    //! Unset bit for i-th item; returns non-null if that bit 
    //! has been set previously
    Cell Unset (Standard_Size i) 
    {
      Cell* abyte = Bits + i / BitsPerCell();
      Cell  amask = (Cell)( '\1' << ( i % BitsPerCell() ) );
      Cell  anold = (Cell)( *abyte & amask );
      *abyte = (Cell)( *abyte & ~amask );
      return anold;
    }

  public:
    Standard_Size*   Count; //!< items counter 
    Standard_Address Array; //!< pointer to the data items array
    Cell*            Bits;  //!< bit map for defined/undefined flags
  };

public:
  /**
   * Iterator 
   */

  class Iterator {
  public:
    // Public interface

    //! Restart iterations on the same array
    void Restart () { init(myArr); }

    //! Returns True if current item is available
    Standard_Boolean More () const { return myHasMore; }

    //! Advances to the next item
    Standard_EXPORT void Next ();

    //! Returns current index
    Standard_EXPORT Standard_Size Index () const 
    { 
      return myIBlock * myArr->myBlockSize + myInd;
    }

  protected:
    // Methods for descendant

    //! Empty constructor
    Standard_EXPORT Iterator (const NCollection_SparseArrayBase* theArray=0);

    //! Initialize by the specified array
    Standard_EXPORT void init (const NCollection_SparseArrayBase* theArray);

    //! Returns address of the current item
    Standard_Address value () const 
    { 
      return myArr->getItem (myBlock, myInd);
    }
    
  private:
    const NCollection_SparseArrayBase *myArr;
    Standard_Boolean myHasMore;
    Standard_Size myIBlock;
    Standard_Size myInd;
    Block myBlock;
  };
  friend class Iterator;

private:
  // Copy constructor and assignment operator are private thus not accessible
  NCollection_SparseArrayBase (const NCollection_SparseArrayBase&) {}
  void operator = (const NCollection_SparseArrayBase&) {}

protected:
  // Object life

  //! Constructor; initialized by size of item and of block (in items)
  NCollection_SparseArrayBase (Standard_Size theItemSize,
			       Standard_Size theBlockSize)
    : myItemSize(theItemSize), myBlockSize(theBlockSize), 
      myNbBlocks(0), mySize(0), myData(0)
  {
  }

  //! Destructor
  virtual ~NCollection_SparseArrayBase ()
  {
    Clear();
  }

protected:
  // Data access interface for descendants

  //! Creates Block structure for block pointed by theAddr
  Block getBlock (const Standard_Address theAddr) const
  {
    return Block (theAddr, myBlockSize, myItemSize);
  }

  //! Find address of the item in the block by index (in the block)
  Standard_Address getItem (const Block &theBlock, Standard_Size theInd) const
  {
    return ((char*)theBlock.Array) + myItemSize * theInd;
  }

  //! Direct const access to the item
  Standard_Address getValue (const Standard_Size theIndex) const
  {
    Standard_OutOfRange_Raise_if (!HasValue(theIndex),"NCollection_SparseArray::Value()")
    return Block::ToArray(myData[theIndex/myBlockSize], myBlockSize, myItemSize) + 
           myItemSize * (theIndex % myBlockSize);
  }

  //! Set a value to the specified item; returns address of the set item
  Standard_EXPORT Standard_Address setValue (const Standard_Size theIndex, 
                                             const Standard_Address theValue);

  //! Copy contents of theOther to this; 
  //! assumes that this and theOther have exactly the same type of arguments 
  Standard_EXPORT void assign (const NCollection_SparseArrayBase& theOther);

  //! Exchange contents of theOther and this; 
  //! assumes that this and theOther have exactly the same type of arguments 
  Standard_EXPORT void exchange (NCollection_SparseArrayBase& theOther);

protected:
  // Methods to be provided by descendant 

  //! Create new item at the specified address with default constructor
//  virtual void createItem (Standard_Address theAddress) = 0;
  
  //! Create new item at the specified address with copy constructor
  //! from existing item
  virtual void createItem (Standard_Address theAddress, Standard_Address theOther) = 0;
  
  //! Call destructor to the item 
  virtual void destroyItem (Standard_Address theAddress) = 0;

  //! Call assignment operator to the item
  virtual void copyItem (Standard_Address theAddress, Standard_Address theOther) = 0;

private:
  // Implementation of memory allocation/deallocation and access mechanics

  //! Allocate space for at least iBlock+1 blocks
  void allocData (const Standard_Size iBlock);

  //! Free specified block
  void freeBlock (const Standard_Size iBlock);

protected:
  Standard_Size     myItemSize;  //!< size of item
  Standard_Size     myBlockSize; //!< block size (in items)
  Standard_Size     myNbBlocks;  //!< allocated size of blocks table
  Standard_Size     mySize;      //!< number of currently defined items
  Standard_Address *myData;      //!< array of pointers to data blocks
};

#endif

