// Created on: 2009-09-23
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _NCollection_LocalArray_HeaderFile
#define _NCollection_LocalArray_HeaderFile

#include <Standard.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_TypeDef.hxx>

#include <cstring>
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>

//! Auxiliary class optimizing creation of array buffer
//! (using stack allocation for small arrays).
//!
//! For trivially copyable types the fast memcpy / Standard::Reallocate path
//! is used.  For non-trivially-copyable types (Handle, TopLoc_Location, etc.)
//! the class uses placement new, move semantics, and explicit destructors
//! while keeping Standard::Allocate / Standard::Free for heap management.
//!
//! Non-trivially-copyable types must be default-constructible and
//! nothrow-move-constructible.
template <class theItem, int MAX_ARRAY_SIZE = 1024>
class NCollection_LocalArray
{
  static_assert(MAX_ARRAY_SIZE > 0, "NCollection_LocalArray requires positive inline capacity");
  static constexpr bool IS_TRIVIAL = std::is_trivially_copyable_v<theItem>;

public:
  using value_type      = theItem;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = theItem*;
  using const_pointer   = const theItem*;
  using reference       = theItem&;
  using const_reference = const theItem&;
  using iterator        = pointer;
  using const_iterator  = const_pointer;

public:
  explicit NCollection_LocalArray(const size_t theSize)
      : myPtr(inlinePtr()),
        mySize(0)
  {
    Allocate(theSize);
  }

  NCollection_LocalArray() noexcept
      : myPtr(inlinePtr()),
        mySize(0)
  {
  }

  ~NCollection_LocalArray() { Deallocate(); }

  //! Destroy elements, release heap storage, and reset to the empty inline state.
  void Deallocate() noexcept
  {
    if constexpr (!IS_TRIVIAL)
    {
      for (size_t i = 0; i < mySize; ++i)
      {
        myPtr[i].~theItem();
      }
    }
    deallocate();
    myPtr  = inlinePtr();
    mySize = 0;
  }

  void Allocate(const size_t theSize) { Reallocate(theSize, false); }

  //! Reallocate the array to a new size.
  //! @param[in] theNewSize new number of elements
  //! @param[in] theToCopy  if true, existing elements are copied/moved to the new buffer
  void Reallocate(const size_t theNewSize, bool theToCopy = true)
  {
    if (theNewSize <= mySize)
    {
      // Shrinking - just update the logical size, keep existing allocation.
      if constexpr (!IS_TRIVIAL)
      {
        for (size_t i = theNewSize; i < mySize; ++i)
        {
          myPtr[i].~theItem();
        }
      }
      mySize = theNewSize;
      return;
    }

    const bool   aWasInline = isInline();
    const size_t aCopy      = theToCopy ? std::min(mySize, theNewSize) : 0;
    const size_t aNewBytes  = theNewSize * sizeof(theItem);
    const bool   aUseInline = theNewSize <= static_cast<size_t>(MAX_ARRAY_SIZE);
    theItem*     anOldPtr   = myPtr;
    const size_t anOldSize  = mySize;

    if constexpr (IS_TRIVIAL)
    {
      if (!aUseInline && !aWasInline && theToCopy)
      {
        myPtr  = static_cast<theItem*>(Standard::Reallocate(myPtr, aNewBytes));
        mySize = theNewSize;
        return;
      }
    }

    if (aUseInline && aWasInline)
    {
      if constexpr (!IS_TRIVIAL)
      {
        for (size_t anIndex = mySize; anIndex < theNewSize; ++anIndex)
        {
          new (myPtr + anIndex) theItem();
        }
      }
      mySize = theNewSize;
      return;
    }

    theItem* aNewPtr =
      aUseInline ? inlinePtr() : static_cast<theItem*>(Standard::Allocate(aNewBytes));
    if constexpr (IS_TRIVIAL)
    {
      if (aCopy > 0)
      {
        std::memcpy(aNewPtr, anOldPtr, aCopy * sizeof(theItem));
      }
    }
    else
    {
      for (size_t anIndex = 0; anIndex < aCopy; ++anIndex)
      {
        new (aNewPtr + anIndex) theItem(std::move(anOldPtr[anIndex]));
      }
      for (size_t anIndex = aCopy; anIndex < theNewSize; ++anIndex)
      {
        new (aNewPtr + anIndex) theItem();
      }
    }

    if constexpr (!IS_TRIVIAL)
    {
      for (size_t anIndex = 0; anIndex < anOldSize; ++anIndex)
      {
        anOldPtr[anIndex].~theItem();
      }
    }
    if (!aWasInline)
    {
      Standard::Free(anOldPtr);
    }
    myPtr  = aNewPtr;
    mySize = theNewSize;
  }

  size_t Size() const noexcept { return mySize; }

  iterator begin() noexcept { return myPtr; }

  const_iterator begin() const noexcept { return myPtr; }

  const_iterator cbegin() const noexcept { return myPtr; }

  iterator end() noexcept { return myPtr + mySize; }

  const_iterator end() const noexcept { return myPtr + mySize; }

  const_iterator cend() const noexcept { return myPtr + mySize; }

  operator pointer() noexcept { return myPtr; }

  operator const_pointer() const noexcept { return myPtr; }

  NCollection_LocalArray(NCollection_LocalArray&& theOther) noexcept
      : myPtr(inlinePtr()),
        mySize(0)
  {
    if (theOther.isInline())
    {
      // When the source is inline, mySize is bounded by MAX_ARRAY_SIZE.
      const size_t aNb = std::min(theOther.mySize, static_cast<size_t>(MAX_ARRAY_SIZE));
      if constexpr (IS_TRIVIAL)
      {
        std::memcpy(inlinePtr(), theOther.inlinePtr(), theOther.mySize * sizeof(theItem));
      }
      else
      {
        for (size_t i = 0; i < aNb; ++i)
        {
          new (inlinePtr() + i) theItem(std::move(theOther.inlinePtr()[i]));
        }
        for (size_t i = 0; i < aNb; ++i)
        {
          theOther.inlinePtr()[i].~theItem();
        }
      }
      mySize = theOther.mySize;
    }
    else
    {
      myPtr          = theOther.myPtr;
      mySize         = theOther.mySize;
      theOther.myPtr = theOther.inlinePtr();
    }
    theOther.mySize = 0;
  }

  NCollection_LocalArray& operator=(NCollection_LocalArray&& theOther) noexcept
  {
    if (this == &theOther)
    {
      return *this;
    }

    if constexpr (IS_TRIVIAL)
    {
      mySize = theOther.mySize;
      if (theOther.isInline())
      {
        deallocate();
        myPtr = inlinePtr();
        // When the source is inline, mySize is bounded by MAX_ARRAY_SIZE.
        const size_t aNb = std::min(mySize, static_cast<size_t>(MAX_ARRAY_SIZE));
        std::memcpy(inlinePtr(), theOther.inlinePtr(), aNb * sizeof(theItem));
      }
      else if (!isInline())
      {
        // Both on heap: swap pointers, theOther frees our old allocation on destruction.
        theItem* anOldPtr = myPtr;
        myPtr             = theOther.myPtr;
        theOther.myPtr    = anOldPtr;
      }
      else
      {
        // this on inline, theOther on heap: take the pointer.
        myPtr          = theOther.myPtr;
        theOther.myPtr = theOther.inlinePtr();
      }
    }
    else
    {
      // Destroy our current elements.
      const bool aWasInline = isInline();
      theItem*   anOldPtr   = myPtr;
      for (size_t i = 0; i < mySize; ++i)
      {
        myPtr[i].~theItem();
      }
      if (!aWasInline)
      {
        Standard::Free(anOldPtr);
      }
      myPtr  = inlinePtr();
      mySize = 0;

      if (theOther.isInline())
      {
        // When the source is inline, mySize is bounded by MAX_ARRAY_SIZE.
        const size_t aNb = std::min(theOther.mySize, static_cast<size_t>(MAX_ARRAY_SIZE));
        for (size_t i = 0; i < aNb; ++i)
        {
          new (inlinePtr() + i) theItem(std::move(theOther.inlinePtr()[i]));
        }
        mySize = theOther.mySize;
        for (size_t i = 0; i < aNb; ++i)
        {
          theOther.inlinePtr()[i].~theItem();
        }
      }
      else
      {
        // Take ownership of theOther's heap allocation directly.
        myPtr          = theOther.myPtr;
        mySize         = theOther.mySize;
        theOther.myPtr = theOther.inlinePtr();
      }
    }
    theOther.mySize = 0;
    return *this;
  }

  //! Returns a span as Array1 with shared memory.
  //! Modifying the local array or the array view may invalidate the shared buffer.
  //! @return array view of the local array data
  NCollection_Array1<theItem> ToArray1() { return NCollection_Array1<theItem>(myPtr, mySize); }

  NCollection_Array1<theItem> ToArray1() const
  {
    return NCollection_Array1<theItem>(myPtr, mySize);
  }

  NCollection_LocalArray(const NCollection_LocalArray&)            = delete;
  NCollection_LocalArray& operator=(const NCollection_LocalArray&) = delete;

protected:
  void deallocate()
  {
    if (!isInline())
    {
      Standard::Free(myPtr);
    }
  }

  //! Pointer to inline buffer storage.
  theItem* inlinePtr() noexcept { return myStorage.ptr(); }

  const theItem* inlinePtr() const noexcept { return myStorage.ptr(); }

  //! True if currently using inline (stack) storage.
  bool isInline() const noexcept { return myPtr == myStorage.ptr(); }

  //! Inline storage for trivial types: typed array with direct element access.
  struct InlineStorageTrivial
  {
    theItem myData[MAX_ARRAY_SIZE];

    theItem* ptr() noexcept { return myData; }

    const theItem* ptr() const noexcept { return myData; }
  };

  //! Inline storage for non-trivial types: raw aligned bytes for placement new.
  struct InlineStorageNonTrivial
  {
    alignas(theItem) char myData[MAX_ARRAY_SIZE * sizeof(theItem)];

    theItem* ptr() noexcept { return reinterpret_cast<theItem*>(myData); }

    const theItem* ptr() const noexcept { return reinterpret_cast<const theItem*>(myData); }
  };

  using InlineStorage =
    std::conditional_t<IS_TRIVIAL, InlineStorageTrivial, InlineStorageNonTrivial>;

protected:
  InlineStorage myStorage; //!< Inline buffer
  theItem*      myPtr;     //!< Points to inline or heap buffer
  size_t        mySize;    //!< Logical element count
};

#endif // _NCollection_LocalArray_HeaderFile
