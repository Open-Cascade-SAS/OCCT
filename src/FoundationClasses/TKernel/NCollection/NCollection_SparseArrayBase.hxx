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

#include <cstddef>
#include <iterator>
#include <type_traits>

/**
 * Base class for NCollection_SparseArray;
 * provides non-template implementation of general mechanics
 * of block allocation, items creation / deletion etc.
 *
 * Type-specific item operations (construction, destruction, copy)
 * are provided by the derived template class via function pointers
 * passed as arguments to the protected methods.
 */

class NCollection_SparseArrayBase
{
public:
  //!@name Function pointer types for type-specific item operations
  //!@{

  //! Copy-construct a new item at theAddress from theOther
  using CreateItemFunc = void (*)(void* theAddress, void* theOther);
  //! Destroy the item at theAddress
  using DestroyItemFunc = void (*)(void* theAddress);
  //! Copy-assign the item at theAddress from theOther
  using CopyItemFunc = void (*)(void* theAddress, void* theOther);

  //!@}

public:
  //!@name Type-independent public interface
  //!@{

  //! Returns number of currently contained items
  size_t Size() const noexcept { return mySize; }

  //! Returns the index span covered by allocated blocks.
  size_t Capacity() const noexcept { return myNbBlocks * myBlockSize; }

  //! Check whether the value at given index is set
  Standard_EXPORT bool HasValue(const size_t theIndex) const;

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
    static constexpr size_t bitsPerCell() noexcept { return sizeof(Cell) * 8; }

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
      return sizeof(size_t) + sizeof(Cell) * ((theNbItems + bitsPerCell() - 1) / bitsPerCell())
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
      Cell* abyte = Bits + i / bitsPerCell();
      Cell  amask = (Cell)('\1' << (i % bitsPerCell()));
      Cell  anold = (Cell)(*abyte & amask);
      *abyte      = (Cell)(*abyte | amask);
      return !anold;
    }

    //! Check bit for i-th item; returns non-null if that bit is set
    Cell IsSet(size_t i) noexcept
    {
      Cell* abyte = Bits + i / bitsPerCell();
      Cell  amask = (Cell)('\1' << (i % bitsPerCell()));
      return (Cell)(*abyte & amask);
    }

    //! Unset bit for i-th item; returns non-null if that bit
    //! has been set previously
    Cell Unset(size_t i) noexcept
    {
      Cell* abyte = Bits + i / bitsPerCell();
      Cell  amask = (Cell)('\1' << (i % bitsPerCell()));
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

protected:
  //! STL-style iterator over the defined entries of a sparse array.
  template <class TheContainer, class TheValue, bool IsConstant>
  class BasicIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = TheValue;
    using difference_type   = std::ptrdiff_t;
    using pointer   = typename std::conditional<IsConstant, const TheValue*, TheValue*>::type;
    using reference = typename std::conditional<IsConstant, const TheValue&, TheValue&>::type;
    using container_pointer =
      typename std::conditional<IsConstant, const TheContainer*, TheContainer*>::type;

    BasicIterator() noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    explicit BasicIterator(TheContainer& theContainer) noexcept
        : myContainer(&theContainer),
          myEnd(theContainer.Capacity())
    {
      skipUnset();
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    explicit BasicIterator(const TheContainer& theContainer) noexcept
        : myContainer(&theContainer),
          myEnd(theContainer.Capacity())
    {
      skipUnset();
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    BasicIterator(TheContainer& theContainer, const size_t theIndex) noexcept
        : myContainer(&theContainer),
          myIndex(theIndex),
          myEnd(theContainer.Capacity())
    {
      skipUnset();
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(const TheContainer& theContainer, const size_t theIndex) noexcept
        : myContainer(&theContainer),
          myIndex(theIndex),
          myEnd(theContainer.Capacity())
    {
      skipUnset();
    }

    BasicIterator(const BasicIterator&) noexcept            = default;
    BasicIterator& operator=(const BasicIterator&) noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(const BasicIterator<TheContainer, TheValue, false>& theOther) noexcept
        : myHasMore(theOther.myHasMore),
          myContainer(theOther.myContainer),
          myIndex(theOther.myIndex),
          myEnd(theOther.myEnd),
          myIBlock(theOther.myIBlock),
          myInd(theOther.myInd),
          myBlock(theOther.myBlock)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator& operator=(const BasicIterator<TheContainer, TheValue, false>& theOther) noexcept
    {
      myHasMore   = theOther.myHasMore;
      myContainer = theOther.myContainer;
      myIndex     = theOther.myIndex;
      myEnd       = theOther.myEnd;
      myIBlock    = theOther.myIBlock;
      myInd       = theOther.myInd;
      myBlock     = theOther.myBlock;
      return *this;
    }

    reference operator*() const noexcept { return *itemPointer(); }

    pointer operator->() const noexcept { return itemPointer(); }

    size_t Key() const noexcept { return myIndex; }

    size_t Index() const noexcept { return myIndex; }

    bool More() const noexcept { return myHasMore; }

    void Restart() noexcept
    {
      myIndex = 0;
      skipUnset();
    }

    BasicIterator& operator++() noexcept
    {
      if (!More())
      {
        return *this;
      }

      ++myIndex;
      ++myInd;
      findNext();
      return *this;
    }

    BasicIterator operator++(int) noexcept
    {
      BasicIterator anOld(*this);
      ++(*this);
      return anOld;
    }

    template <bool theOtherIsConstant>
    bool operator==(
      const BasicIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return (!More() && !theOther.More())
             || (myContainer == theOther.myContainer && myIndex == theOther.myIndex);
    }

    template <bool theOtherIsConstant>
    bool operator!=(
      const BasicIterator<TheContainer, TheValue, theOtherIsConstant>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

  private:
    template <class, class, bool>
    friend class BasicIterator;

    void skipUnset() noexcept
    {
      if (myContainer == nullptr || myIndex >= myEnd)
      {
        myHasMore = false;
        return;
      }

      myIBlock = myIndex / myContainer->myBlockSize;
      myInd    = myIndex % myContainer->myBlockSize;
      findNext();
    }

    void findNext() noexcept
    {
      myHasMore = false;
      while (myContainer != nullptr && myIndex < myEnd && myIBlock < myContainer->myNbBlocks)
      {
        if (myContainer->myData[myIBlock] != nullptr)
        {
          myBlock =
            Block(myContainer->myData[myIBlock], myContainer->myBlockSize, myContainer->myItemSize);
          for (; myInd < myContainer->myBlockSize; ++myInd, ++myIndex)
          {
            if (myBlock.IsSet(myInd))
            {
              myHasMore = true;
              return;
            }
          }
        }

        ++myIBlock;
        myInd   = 0;
        myIndex = myIBlock * myContainer->myBlockSize;
      }

      myIndex = myEnd;
    }

    pointer itemPointer() const noexcept
    {
      return static_cast<pointer>(myContainer->getItem(myBlock, myInd));
    }

    bool              myHasMore   = false;
    container_pointer myContainer = nullptr;
    size_t            myIndex     = 0;
    size_t            myEnd       = 0;
    size_t            myIBlock    = 0;
    size_t            myInd       = 0;
    Block             myBlock{nullptr, 0, 0};
  };
  friend class Iterator;

private:
  // Copy constructor and assignment operator are deleted
  NCollection_SparseArrayBase(const NCollection_SparseArrayBase&)            = delete;
  NCollection_SparseArrayBase& operator=(const NCollection_SparseArrayBase&) = delete;

protected:
  // Object life

  //! Constructor; initialized by size of item, block size, and item destructor function.
  //! @param theDestroyItem is stored to enable proper item destruction in the base destructor
  NCollection_SparseArrayBase(size_t          theItemSize,
                              size_t          theBlockSize,
                              DestroyItemFunc theDestroyItem) noexcept
      : myItemSize(theItemSize),
        myBlockSize(theBlockSize),
        myNbBlocks(0),
        mySize(0),
        myData(nullptr),
        myDestroyItem(theDestroyItem)
  {
  }

  //! Destructor; properly destroys all items and frees all memory.
  //! Uses the stored DestroyItemFunc, so no virtual dispatch is needed.
  ~NCollection_SparseArrayBase() { clearItems(myDestroyItem); }

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

  //! Clears all items and frees all memory.
  //! @param theDestroyItem function to call destructor on each item
  Standard_EXPORT void clearItems(DestroyItemFunc theDestroyItem);

  //! Deletes the item at theIndex from the array;
  //! returns True if the item was defined.
  //! @param theDestroyItem function to call destructor on the item
  Standard_EXPORT bool unsetValue(const size_t theIndex, DestroyItemFunc theDestroyItem);

  //! Set a value to the specified item; returns address of the set item.
  //! @param theCreateItem function to copy-construct a new item
  //! @param theCopyItem function to copy-assign an existing item
  Standard_EXPORT void* setValue(const size_t   theIndex,
                                 void* const    theValue,
                                 CreateItemFunc theCreateItem,
                                 CopyItemFunc   theCopyItem);

  //! Copy contents of theOther to this;
  //! assumes that this and theOther have exactly the same type of arguments.
  //! @param theCreateItem function to copy-construct a new item
  //! @param theDestroyItem function to call destructor on an item
  //! @param theCopyItem function to copy-assign an existing item
  Standard_EXPORT void assign(const NCollection_SparseArrayBase& theOther,
                              CreateItemFunc                     theCreateItem,
                              DestroyItemFunc                    theDestroyItem,
                              CopyItemFunc                       theCopyItem);

  //! Exchange contents of theOther and this;
  //! assumes that this and theOther have exactly the same type of arguments
  Standard_EXPORT void exchange(NCollection_SparseArrayBase& theOther) noexcept;

private:
  // Implementation of memory allocation/deallocation and access mechanics

  //! Allocate space for at least iBlock+1 blocks
  void allocData(const size_t iBlock);

  //! Free specified block, destroying all items via theDestroyItem
  void freeBlock(const size_t iBlock, DestroyItemFunc theDestroyItem);

protected:
  size_t          myItemSize;    //!< size of item
  size_t          myBlockSize;   //!< block size (in items)
  size_t          myNbBlocks;    //!< allocated size of blocks table
  size_t          mySize;        //!< number of currently defined items
  void**          myData;        //!< array of pointers to data blocks
  DestroyItemFunc myDestroyItem; //!< function to call destructor on items
};

#endif
