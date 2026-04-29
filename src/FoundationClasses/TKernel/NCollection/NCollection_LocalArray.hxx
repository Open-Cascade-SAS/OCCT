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
#include <Standard_TypeDef.hxx>

#include <cstring>
#include <algorithm>
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
  static constexpr bool IS_TRIVIAL = std::is_trivially_copyable_v<theItem>;

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

  ~NCollection_LocalArray()
  {
    if constexpr (!IS_TRIVIAL)
    {
      for (size_t i = 0; i < mySize; ++i)
        myPtr[i].~theItem();
    }
    Deallocate();
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
          myPtr[i].~theItem();
      }
      mySize = theNewSize;
      return;
    }

    const bool   aWasInline = isInline();
    const size_t aNewBytes  = theNewSize * sizeof(theItem);

    if (theNewSize <= static_cast<size_t>(MAX_ARRAY_SIZE))
    {
      // New size fits in inline buffer.
      if (!aWasInline)
      {
        if constexpr (IS_TRIVIAL)
        {
          if (theToCopy && mySize > 0)
          {
            std::memcpy(inlinePtr(), myPtr, std::min(mySize, theNewSize) * sizeof(theItem));
          }
          Standard::Free(myPtr);
        }
        else
        {
          theItem*     anOldPtr  = myPtr;
          const size_t anOldSize = mySize;
          const size_t aCopy     = theToCopy ? std::min(anOldSize, theNewSize) : 0;
          myPtr                  = inlinePtr();
          for (size_t i = 0; i < aCopy; ++i)
            new (myPtr + i) theItem(std::move(anOldPtr[i]));
          for (size_t i = aCopy; i < theNewSize; ++i)
            new (myPtr + i) theItem();
          for (size_t i = 0; i < anOldSize; ++i)
            anOldPtr[i].~theItem();
          Standard::Free(anOldPtr);
        }
        myPtr = inlinePtr();
      }
      else
      {
        // Already inline, growing within buffer.
        if constexpr (!IS_TRIVIAL)
        {
          for (size_t i = mySize; i < theNewSize; ++i)
            new (myPtr + i) theItem();
        }
      }
      mySize = theNewSize;
      return;
    }

    // Need heap allocation (theNewSize > MAX_ARRAY_SIZE).
    if constexpr (IS_TRIVIAL)
    {
      if (!aWasInline)
      {
        if (theToCopy)
        {
          myPtr = static_cast<theItem*>(Standard::Reallocate(myPtr, aNewBytes));
        }
        else
        {
          Standard::Free(myPtr);
          myPtr = static_cast<theItem*>(Standard::Allocate(aNewBytes));
        }
      }
      else
      {
        theItem* aNewPtr = static_cast<theItem*>(Standard::Allocate(aNewBytes));
        if (theToCopy && mySize > 0)
        {
          std::memcpy(aNewPtr, myPtr, std::min(mySize, theNewSize) * sizeof(theItem));
        }
        myPtr = aNewPtr;
      }
    }
    else
    {
      // Non-trivial: Standard::Reallocate (realloc) cannot be used because
      // it does not call constructors or destructors.
      theItem*     aNewPtr = static_cast<theItem*>(Standard::Allocate(aNewBytes));
      const size_t aCopy   = theToCopy ? std::min(mySize, theNewSize) : 0;
      for (size_t i = 0; i < aCopy; ++i)
        new (aNewPtr + i) theItem(std::move(myPtr[i]));
      for (size_t i = aCopy; i < theNewSize; ++i)
        new (aNewPtr + i) theItem();
      for (size_t i = 0; i < mySize; ++i)
        myPtr[i].~theItem();
      if (!aWasInline)
        Standard::Free(myPtr);
      myPtr = aNewPtr;
    }
    mySize = theNewSize;
  }

  size_t Size() const noexcept { return mySize; }

  operator theItem*() const noexcept { return myPtr; }

  NCollection_LocalArray(NCollection_LocalArray&& theOther) noexcept
      : myPtr(inlinePtr()),
        mySize(theOther.mySize)
  {
    if (theOther.isInline())
    {
      // When the source is inline, mySize is bounded by MAX_ARRAY_SIZE.
      const size_t aNb = std::min(mySize, static_cast<size_t>(MAX_ARRAY_SIZE));
      if constexpr (IS_TRIVIAL)
      {
        std::memcpy(inlinePtr(), theOther.inlinePtr(), aNb * sizeof(theItem));
      }
      else
      {
        for (size_t i = 0; i < aNb; ++i)
          new (inlinePtr() + i) theItem(std::move(theOther.inlinePtr()[i]));
        for (size_t i = 0; i < aNb; ++i)
          theOther.inlinePtr()[i].~theItem();
      }
    }
    else
    {
      myPtr          = theOther.myPtr;
      theOther.myPtr = theOther.inlinePtr();
    }
    theOther.mySize = 0;
  }

  NCollection_LocalArray& operator=(NCollection_LocalArray&& theOther) noexcept
  {
    if (this == &theOther)
      return *this;

    if constexpr (IS_TRIVIAL)
    {
      mySize = theOther.mySize;
      if (theOther.isInline())
      {
        Deallocate();
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
      for (size_t i = 0; i < mySize; ++i)
        myPtr[i].~theItem();

      if (theOther.isInline())
      {
        if (!isInline())
          Standard::Free(myPtr);
        myPtr  = inlinePtr();
        mySize = theOther.mySize;
        // When the source is inline, mySize is bounded by MAX_ARRAY_SIZE.
        const size_t aNb = std::min(mySize, static_cast<size_t>(MAX_ARRAY_SIZE));
        for (size_t i = 0; i < aNb; ++i)
          new (inlinePtr() + i) theItem(std::move(theOther.inlinePtr()[i]));
        for (size_t i = 0; i < aNb; ++i)
          theOther.inlinePtr()[i].~theItem();
      }
      else
      {
        // Take ownership of theOther's heap allocation directly.
        if (!isInline())
          Standard::Free(myPtr);
        myPtr          = theOther.myPtr;
        mySize         = theOther.mySize;
        theOther.myPtr = theOther.inlinePtr();
      }
    }
    theOther.mySize = 0;
    return *this;
  }

  NCollection_LocalArray(const NCollection_LocalArray&)            = delete;
  NCollection_LocalArray& operator=(const NCollection_LocalArray&) = delete;

protected:
  void Deallocate()
  {
    if (!isInline())
      Standard::Free(myPtr);
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
