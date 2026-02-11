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
#include <Standard_TypeDef.hxx>
#include <Standard_OutOfRange.hxx>

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
  Standard_EXPORT void Clear();

  //! Returns number of currently contained items
  size_t Size() const noexcept { return mySize; }

  //! Check whether the value at given index is set
  Standard_EXPORT bool HasValue(const size_t theIndex) const;

  //! Deletes the item from the array;
  //! returns True if that item was defined
  Standard_EXPORT bool UnsetValue(const size_t theIndex);

  //!@}

private:

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

  class Block
  {
  public:
    typedef unsigned char Cell; //!< type of items used to hold bits

    //! Number of bits in each cell
    static constexpr size_t BitsPerCell() noexcept { return sizeof(Cell) * 8; }

  public:
    //! Initializes the block by pointer to block data
    Block(void* const theAddr, const size_t theNbItems, const size_t theItemSize)
        : Count((size_t*)theAddr),
          Array((char*)theAddr + sizeof(size_t)),
          Bits((Cell*)((char*)theAddr + sizeof(size_t) + theNbItems * theItemSize))
    {
    }

    //! Compute required size for block data, in bytes
    static constexpr size_t Size(const size_t theNbItems, const size_t theItemSize) noexcept
    {
      return sizeof(size_t) + sizeof(Cell) * ((theNbItems + BitsPerCell() - 1) / BitsPerCell())
             + theNbItems * theItemSize;
    }

    //! Returns address of array from address of block
    static char* ToArray(void* const theAddress,
                         const size_t /*theNbItems*/,
                         const size_t /*theItemSize*/) noexcept
    {
      return (char*)theAddress + sizeof(size_t);
    }

  public:
    //! Set bit for i-th item; returns non-null if that bit has
    //! not been set previously
    Cell Set(size_t i) noexcept
    {
      Cell* abyte = Bits + i / BitsPerCell();
      Cell  amask = (Cell)('\1' << (i % BitsPerCell()));
      Cell  anold = (Cell)(*abyte & amask);
      *abyte      = (Cell)(*abyte | amask);
      return !anold;
    }

    //! Check bit for i-th item; returns non-null if that bit is set
    Cell IsSet(size_t i) noexcept
    {
      Cell* abyte = Bits + i / BitsPerCell();
      Cell  amask = (Cell)('\1' << (i % BitsPerCell()));
      return (Cell)(*abyte & amask);
    }

    //! Unset bit for i-th item; returns non-null if that bit
    //! has been set previously
    Cell Unset(size_t i) noexcept
    {
      Cell* abyte = Bits + i / BitsPerCell();
      Cell  amask = (Cell)('\1' << (i % BitsPerCell()));
      Cell  anold = (Cell)(*abyte & amask);
      *abyte      = (Cell)(*abyte & ~amask);
      return anold;
    }

  public:
    size_t* Count; //!< items counter
    void*   Array; //!< pointer to the data items array
    Cell*   Bits;  //!< bit map for defined/undefined flags
  };

public:
  /**
   * Iterator
   */

  class Iterator
  {
  public:
    // Public interface

    //! Restart iterations on the same array
    void Restart() { init(myArr); }

    //! Returns True if current item is available
    bool More() const noexcept { return myHasMore; }

    //! Advances to the next item
    Standard_EXPORT void Next();

    //! Returns current index
    size_t Index() const noexcept { return myIBlock * myArr->myBlockSize + myInd; }

  protected:
    // Methods for descendant

    //! Empty constructor
    Standard_EXPORT Iterator(const NCollection_SparseArrayBase* theArray = nullptr);

    //! Initialize by the specified array
    Standard_EXPORT void init(const NCollection_SparseArrayBase* theArray);

    //! Returns address of the current item
    void* value() const noexcept { return myArr->getItem(myBlock, myInd); }

  private:
    const NCollection_SparseArrayBase* myArr;
    bool                               myHasMore;
    size_t                             myIBlock;
    size_t                             myInd;
    Block                              myBlock;
  };
  friend class Iterator;

private:
  // Copy constructor and assignment operator are deleted
  NCollection_SparseArrayBase(const NCollection_SparseArrayBase&)            = delete;
  NCollection_SparseArrayBase& operator=(const NCollection_SparseArrayBase&) = delete;

protected:
  // Object life

  //! Constructor; initialized by size of item and of block (in items)
  NCollection_SparseArrayBase(size_t theItemSize, size_t theBlockSize) noexcept
      : myItemSize(theItemSize),
        myBlockSize(theBlockSize),
        myNbBlocks(0),
        mySize(0),
        myData(nullptr)
  {
  }

  //! Destructor
  virtual ~NCollection_SparseArrayBase() { Clear(); }

protected:
  // Data access interface for descendants

  //! Creates Block structure for block pointed by theAddr
  Block getBlock(void* const theAddr) const noexcept
  {
    return Block(theAddr, myBlockSize, myItemSize);
  }

  //! Find address of the item in the block by index (in the block)
  void* getItem(const Block& theBlock, size_t theInd) const noexcept
  {
    return ((char*)theBlock.Array) + myItemSize * theInd;
  }

  //! Direct const access to the item
  void* getValue(const size_t theIndex) const
  {
    Standard_OutOfRange_Raise_if(
      !HasValue(theIndex),
      "NCollection_SparseArray::Value()") return Block::ToArray(myData[theIndex / myBlockSize],
                                                                myBlockSize,
                                                                myItemSize)
      + myItemSize * (theIndex % myBlockSize);
  }

  //! Set a value to the specified item; returns address of the set item
  Standard_EXPORT void* setValue(const size_t theIndex, void* const theValue);

  //! Copy contents of theOther to this;
  //! assumes that this and theOther have exactly the same type of arguments
  Standard_EXPORT void assign(const NCollection_SparseArrayBase& theOther);

  //! Exchange contents of theOther and this;
  //! assumes that this and theOther have exactly the same type of arguments
  Standard_EXPORT void exchange(NCollection_SparseArrayBase& theOther) noexcept;

protected:
  // Methods to be provided by descendant

  //! Create new item at the specified address with default constructor
  //  virtual void createItem (void* theAddress) = 0;

  //! Create new item at the specified address with copy constructor
  //! from existing item
  virtual void createItem(void* theAddress, void* theOther) = 0;

  //! Call destructor to the item
  virtual void destroyItem(void* theAddress) = 0;

  //! Call assignment operator to the item
  virtual void copyItem(void* theAddress, void* theOther) = 0;

private:
  // Implementation of memory allocation/deallocation and access mechanics

  //! Allocate space for at least iBlock+1 blocks
  void allocData(const size_t iBlock);

  //! Free specified block
  void freeBlock(const size_t iBlock);

protected:
  size_t myItemSize;  //!< size of item
  size_t myBlockSize; //!< block size (in items)
  size_t myNbBlocks;  //!< allocated size of blocks table
  size_t mySize;      //!< number of currently defined items
  void** myData;      //!< array of pointers to data blocks
};

#endif
