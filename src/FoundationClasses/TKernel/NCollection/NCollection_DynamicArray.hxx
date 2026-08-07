// Copyright (c) 2002-2023 OPEN CASCADE SAS
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

#ifndef NCollection_DynamicArray_HeaderFile
#define NCollection_DynamicArray_HeaderFile

#include <NCollection_Allocator.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

#include <NCollection_DefineAlloc.hxx>
#include <NCollection_OccAllocator.hxx>
#include <StdFail_NotDone.hxx>

#include <cstddef>
#include <cstring>
#include <iterator>
#include <locale>
#include <type_traits>

//! Class NCollection_DynamicArray (dynamic array of objects)
//!
//! The array's indices always start at 0.
//!
//! The Vector is always created with 0 length. It can be enlarged by two means:
//!  1. Calling the method Append (val) - then "val" is added to the end of the
//!     vector (the vector length is incremented)
//!  2. Calling the method SetValue (i, val) - if "i" is greater than or equal
//!     to the current length of the vector, the vector is enlarged to accomo-
//!     date this index
//!
//! The methods Append and SetValue return a non-const reference to the copied
//! object inside the vector. This reference is guaranteed to be valid until
//! the vector is destroyed. It can be used to access the vector member directly
//! or to pass its address to other data structures.
//!
//! The vector iterator remembers the length of the vector at the moment of the
//! creation or initialisation of the iterator. Therefore the iteration begins
//! at index 0 and stops at the index equal to (remembered_length-1). It is OK
//! to enlarge the vector during the iteration.
template <class TheItemType>
class NCollection_DynamicArray
{
public:
  //! Memory allocation
  DEFINE_STANDARD_ALLOC;
  DEFINE_NCOLLECTION_ALLOC;

public:
  typedef NCollection_OccAllocator<TheItemType>  allocator_type;
  typedef NCollection_LinearVector<TheItemType*> vector;

public:
  // Define various type aliases for convenience
  using value_type      = TheItemType;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = TheItemType*;
  using const_pointer   = const TheItemType*;
  using reference       = TheItemType&;
  using const_reference = const TheItemType&;

private:
  //! STL-style iterator over the segmented array.
  //! The current element pointer is cached so sequential traversal does not
  //! recompute the block address for every dereference.
  template <bool IsConstant>
  class BasicIterator
  {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = TheItemType;
    using difference_type   = std::ptrdiff_t;
    using pointer = typename std::conditional<IsConstant, const TheItemType*, TheItemType*>::type;
    using reference = typename std::conditional<IsConstant, const TheItemType&, TheItemType&>::type;
    using container_pointer = typename std::conditional<IsConstant,
                                                        const NCollection_DynamicArray*,
                                                        NCollection_DynamicArray*>::type;

    BasicIterator() noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    explicit BasicIterator(NCollection_DynamicArray& theArray) noexcept
        : myContainer(&theArray),
          myEnd(theArray.Size())
    {
      setIndex(0);
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    explicit BasicIterator(const NCollection_DynamicArray& theArray) noexcept
        : myContainer(&theArray),
          myEnd(theArray.Size())
    {
      setIndex(0);
    }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    BasicIterator(NCollection_DynamicArray& theArray, const size_t theIndex) noexcept
        : myContainer(&theArray),
          myIndex(theIndex),
          myEnd(theArray.Size())
    {
      setIndex(theIndex);
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(const NCollection_DynamicArray& theArray, const size_t theIndex) noexcept
        : myContainer(&theArray),
          myIndex(theIndex),
          myEnd(theArray.Size())
    {
      setIndex(theIndex);
    }

    BasicIterator(const BasicIterator&) noexcept = default;
    BasicIterator& operator=(const BasicIterator&) noexcept = default;

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator(const BasicIterator<false>& theOther) noexcept
        : myContainer(theOther.myContainer),
          myIndex(theOther.myIndex),
          myEnd(theOther.myEnd),
          myCurrent(theOther.myCurrent),
          myBlockEnd(theOther.myBlockEnd)
    {
    }

    template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
    BasicIterator& operator=(const BasicIterator<false>& theOther) noexcept
    {
      myContainer = theOther.myContainer;
      myIndex     = theOther.myIndex;
      myEnd       = theOther.myEnd;
      myCurrent   = theOther.myCurrent;
      myBlockEnd  = theOther.myBlockEnd;
      return *this;
    }

    reference operator*() const noexcept { return *myCurrent; }
    pointer operator->() const noexcept { return myCurrent; }
    reference Value() const noexcept { return operator*(); }

    template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
    reference ChangeValue() const noexcept
    {
      return operator*();
    }

    bool More() const noexcept { return myCurrent != nullptr; }

    BasicIterator& operator++() noexcept
    {
      ++myIndex;
      if (myIndex >= myEnd)
      {
        myCurrent  = nullptr;
        myBlockEnd = nullptr;
      }
      else
      {
        ++myCurrent;
        if (myCurrent == myBlockEnd)
        {
          setIndex(myIndex);
        }
      }
      return *this;
    }

    BasicIterator operator++(int) noexcept
    {
      BasicIterator anOld(*this);
      ++(*this);
      return anOld;
    }

    BasicIterator& operator--() noexcept
    {
      if (myCurrent == nullptr)
      {
        setIndex(myEnd == 0 ? 0 : myEnd - 1);
        return *this;
      }
      --myIndex;
      if (myIndex < myEnd && (myIndex & myContainer->myBlockMask) != myContainer->myBlockMask)
      {
        --myCurrent;
      }
      else
      {
        setIndex(myIndex);
      }
      return *this;
    }

    BasicIterator operator--(int) noexcept
    {
      BasicIterator anOld(*this);
      --(*this);
      return anOld;
    }

    BasicIterator& operator+=(const difference_type theOffset) noexcept
    {
      setIndex(static_cast<size_t>(static_cast<difference_type>(myIndex) + theOffset));
      return *this;
    }

    BasicIterator operator+(const difference_type theOffset) const noexcept
    {
      BasicIterator anIter(*this);
      anIter += theOffset;
      return anIter;
    }

    BasicIterator& operator-=(const difference_type theOffset) noexcept
    {
      return *this += -theOffset;
    }

    BasicIterator operator-(const difference_type theOffset) const noexcept
    {
      BasicIterator anIter(*this);
      anIter -= theOffset;
      return anIter;
    }

    template <bool theOtherIsConstant>
    difference_type operator-(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return static_cast<difference_type>(myIndex)
             - static_cast<difference_type>(theOther.myIndex);
    }

    reference operator[](const difference_type theOffset) const noexcept
    {
      return *(*this + theOffset);
    }

    template <bool theOtherIsConstant>
    bool operator==(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return myContainer == theOther.myContainer && myIndex == theOther.myIndex;
    }

    template <bool theOtherIsConstant>
    bool operator!=(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return !(*this == theOther);
    }

    template <bool theOtherIsConstant>
    bool operator<(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return (*this - theOther) < 0;
    }

    template <bool theOtherIsConstant>
    bool operator>(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return theOther < *this;
    }

    template <bool theOtherIsConstant>
    bool operator<=(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return !(theOther < *this);
    }

    template <bool theOtherIsConstant>
    bool operator>=(const BasicIterator<theOtherIsConstant>& theOther) const noexcept
    {
      return !(*this < theOther);
    }

    friend BasicIterator operator+(const difference_type theOffset,
                                   const BasicIterator&  theIter) noexcept
    {
      return theIter + theOffset;
    }

  private:
    template <bool>
    friend class BasicIterator;

    void setIndex(const size_t theIndex) noexcept
    {
      myIndex = theIndex;
      if (myContainer == nullptr || myIndex >= myEnd)
      {
        myCurrent  = nullptr;
        myBlockEnd = nullptr;
        return;
      }

      const size_t aBlockIndex = myIndex >> myContainer->myBlockShift;
      pointer      aBlockStart = myContainer->blockAt(aBlockIndex);
      myCurrent                = aBlockStart + (myIndex & myContainer->myBlockMask);
      myBlockEnd               = aBlockStart + myContainer->myInternalSize;
    }

    container_pointer myContainer = nullptr;
    size_t            myIndex     = 0;
    size_t            myEnd       = 0;
    pointer            myCurrent   = nullptr;
    pointer            myBlockEnd  = nullptr;
  };

public:
  using iterator       = BasicIterator<false>;
  using const_iterator = BasicIterator<true>;
  class Iterator : public iterator
  {
  public:
    Iterator() noexcept = default;

    explicit Iterator(NCollection_DynamicArray& theArray) noexcept
        : iterator(theArray)
    {
    }

    //! Legacy cursor construction from a const array.
    explicit Iterator(const NCollection_DynamicArray& theArray) noexcept
        : iterator(const_cast<NCollection_DynamicArray&>(theArray))
    {
    }

    virtual bool More() const noexcept { return iterator::More(); }
    virtual void Next() noexcept { ++(*this); }
    const_reference Value() const noexcept { return iterator::Value(); }
    reference ChangeValue() noexcept { return iterator::ChangeValue(); }

    void Init(NCollection_DynamicArray& theArray) noexcept { *this = Iterator(theArray); }
    void Init(const NCollection_DynamicArray& theArray) noexcept { *this = Iterator(theArray); }
    void Initialize(NCollection_DynamicArray& theArray) noexcept { Init(theArray); }
    void Initialize(const NCollection_DynamicArray& theArray) noexcept { Init(theArray); }
  };

  class ConstIterator : public const_iterator
  {
  public:
    ConstIterator() noexcept = default;

    explicit ConstIterator(const NCollection_DynamicArray& theArray) noexcept
        : const_iterator(theArray)
    {
    }

    bool More() const noexcept { return const_iterator::More(); }
    void Next() noexcept { ++(*this); }
    const_reference Value() const noexcept { return const_iterator::Value(); }

    void Init(const NCollection_DynamicArray& theArray) noexcept
    {
      *this = ConstIterator(theArray);
    }
    void Initialize(const NCollection_DynamicArray& theArray) noexcept { Init(theArray); }
  };

public:
  const_iterator begin() const noexcept { return const_iterator(*this); }

  iterator begin() noexcept { return iterator(*this); }

  const_iterator cbegin() const noexcept { return const_iterator(*this); }

  iterator end() noexcept { return iterator(*this, myUsedSize); }

  const_iterator end() const noexcept { return const_iterator(*this, myUsedSize); }

  const_iterator cend() const noexcept { return const_iterator(*this, myUsedSize); }

public: //! @name public methods
  //! Total number of items in the array.
  size_t Size() const noexcept { return myUsedSize; }

  //! Total number of items as the legacy integer length.
  int Length() const noexcept { return static_cast<int>(myUsedSize); }

  //! Lower bound (inclusive).
  int Lower() const noexcept { return 0; }

  //! Upper bound (inclusive).
  int Upper() const noexcept { return static_cast<int>(myUsedSize) - 1; }

  //! Empty query.
  bool IsEmpty() const noexcept { return myUsedSize == 0; }

  //! Return the allocator used for element blocks.
  const occ::handle<NCollection_BaseAllocator>& Allocator() const noexcept
  {
    return myAllocator;
  }

  //! Change the initial block size while the array is empty.
  void SetIncrement(const size_t theIncrement)
  {
    if (myUsedSize != 0)
    {
      return;
    }
    myInternalSize = roundUpPow2(theIncrement);
    myBlockShift   = log2Pow2(myInternalSize);
    myBlockMask    = myInternalSize - 1;
  }

  void SetIncrement(const int theIncrement)
  {
    SetIncrement(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement));
  }

  NCollection_DynamicArray(const size_t theIncrement)
      : myAllocator(),
        myInternalSize(roundUpPow2(theIncrement)),
        myBlockShift(log2Pow2(myInternalSize)),
        myBlockMask(myInternalSize - 1),
        myUsedSize(0)
  {
  }

  NCollection_DynamicArray(const int theIncrement = 256)
      : NCollection_DynamicArray(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement))
  {
  }

  // Constructor taking an allocator
  explicit NCollection_DynamicArray(const size_t                                  theIncrement,
                                    const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myAllocator(theAllocator),
        myInternalSize(roundUpPow2(theIncrement)),
        myBlockShift(log2Pow2(myInternalSize)),
        myBlockMask(myInternalSize - 1),
        myUsedSize(0)
  {
  }

  explicit NCollection_DynamicArray(const int                                     theIncrement,
                                    const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : NCollection_DynamicArray(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement),
                                 theAllocator)
  {
  }

  // Constructor taking an allocator
  explicit NCollection_DynamicArray(const size_t theIncrement, const allocator_type& theAllocator)
      : myAllocator(theAllocator.Allocator()),
        myInternalSize(roundUpPow2(theIncrement)),
        myBlockShift(log2Pow2(myInternalSize)),
        myBlockMask(myInternalSize - 1),
        myUsedSize(0)
  {
  }

  explicit NCollection_DynamicArray(const int theIncrement, const allocator_type& theAllocator)
      : NCollection_DynamicArray(static_cast<size_t>(theIncrement < 1 ? 1 : theIncrement),
                                 theAllocator)
  {
  }

  //! Copy constructor
  NCollection_DynamicArray(const NCollection_DynamicArray& theOther)
      : myAllocator(theOther.myAllocator),
        myInternalSize(theOther.myInternalSize),
        myBlockShift(theOther.myBlockShift),
        myBlockMask(theOther.myBlockMask),
        myUsedSize(0)
  {
    copyDate(theOther);
  }

  NCollection_DynamicArray(NCollection_DynamicArray&& theOther) noexcept
      : myContainer(std::move(theOther.myContainer)),
        myAllocator(std::move(theOther.myAllocator)),
        myInternalSize(theOther.myInternalSize),
        myBlockShift(theOther.myBlockShift),
        myBlockMask(theOther.myBlockMask),
        myUsedSize(theOther.myUsedSize)
  {
    theOther.myUsedSize = 0;
  }

  ~NCollection_DynamicArray() { Clear(true); }

  //! Assignment to the collection of the same type
  NCollection_DynamicArray& Assign(const NCollection_DynamicArray& theOther,
                                   const bool                      theOwnAllocator = true)
  {
    if (&theOther == this)
    {
      return *this;
    }
    Clear(true);
    copyParameters(theOther, !theOwnAllocator);
    copyDate(theOther);
    return *this;
  }

  NCollection_DynamicArray& Assign(NCollection_DynamicArray&& theOther)
  {
    if (&theOther == this)
    {
      return *this;
    }
    Clear(true);
    myContainer    = std::move(theOther.myContainer);
    myAllocator    = std::move(theOther.myAllocator);
    myInternalSize = theOther.myInternalSize;
    myBlockShift   = theOther.myBlockShift;
    myBlockMask    = theOther.myBlockMask;
    myUsedSize     = theOther.myUsedSize;
    theOther.myUsedSize = 0;
    return *this;
  }

  //! Assignment operator
  NCollection_DynamicArray& operator=(const NCollection_DynamicArray& theOther)
  {
    return Assign(theOther, false);
  }

  //! Assignment operator
  NCollection_DynamicArray& operator=(NCollection_DynamicArray&& theOther) noexcept
  {
    return Assign(std::forward<NCollection_DynamicArray>(theOther));
  }

  //! Append
  reference Append(const TheItemType& theValue)
  {
    if (myUsedSize >= availableSize())
    {
      expandBlock(sizeof(TheItemType));
    }
    pointer aPnt = &at(myUsedSize);
    constructAt(aPnt, theValue);
    ++myUsedSize;
    return *aPnt;
  }

  //! Append
  reference Append(TheItemType&& theValue)
  {
    if (myUsedSize >= availableSize())
    {
      expandBlock(sizeof(TheItemType));
    }
    pointer aPnt = &at(myUsedSize);
    constructAt(aPnt, std::forward<TheItemType>(theValue));
    ++myUsedSize;
    return *aPnt;
  }

  //! Insert a value after the element at theIndex, shifting subsequent elements right.
  //! @param theIndex index after which to insert (must be in [0, Size()-1])
  //! @param theValue value to insert
  //! @return reference to the inserted element
  reference InsertAfter(const size_t theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex + 1; --i)
    {
      at(i) = std::move(at(i - 1));
    }
    at(theIndex + 1) = theValue;
    return at(theIndex + 1);
  }

  //! Insert a value after the element at theIndex (move version).
  reference InsertAfter(const size_t theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex + 1; --i)
    {
      at(i) = std::move(at(i - 1));
    }
    at(theIndex + 1) = std::forward<TheItemType>(theValue);
    return at(theIndex + 1);
  }

  reference InsertAfter(const int theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    return InsertAfter(static_cast<size_t>(theIndex), theValue);
  }

  reference InsertAfter(const int theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertAfter: index out of range");
    return InsertAfter(static_cast<size_t>(theIndex), std::forward<TheItemType>(theValue));
  }

  //! Insert a value before the element at theIndex, shifting it and subsequent elements right.
  //! @param theIndex index before which to insert (must be in [0, Size()-1])
  //! @param theValue value to insert
  //! @return reference to the inserted element
  reference InsertBefore(const size_t theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex; --i)
    {
      at(i) = std::move(at(i - 1));
    }
    at(theIndex) = theValue;
    return at(theIndex);
  }

  //! Insert a value before the element at theIndex (move version).
  reference InsertBefore(const size_t theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex >= myUsedSize,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    Appended();
    for (size_t i = myUsedSize - 1; i > theIndex; --i)
    {
      at(i) = std::move(at(i - 1));
    }
    at(theIndex) = std::forward<TheItemType>(theValue);
    return at(theIndex);
  }

  reference InsertBefore(const int theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    return InsertBefore(static_cast<size_t>(theIndex), theValue);
  }

  reference InsertBefore(const int theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::InsertBefore: index out of range");
    return InsertBefore(static_cast<size_t>(theIndex), std::forward<TheItemType>(theValue));
  }

  void EraseLast()
  {
    if (myUsedSize == 0)
    {
      return;
    }
    if constexpr (!std::is_trivially_destructible_v<TheItemType>)
    {
      TheItemType* aLastElem = &ChangeLast();
      destroyAt(aLastElem);
    }
    myUsedSize--;
  }

  //! Appends an empty value and returns the reference to it
  reference Appended()
  {
    if (myUsedSize >= availableSize())
    {
      expandBlock(sizeof(TheItemType));
    }
    pointer aPnt = &at(myUsedSize);
    constructAt(aPnt);
    ++myUsedSize;
    return *aPnt;
  }

  //! Emplace one item at the end, constructing it in-place
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceAppend(Args&&... theArgs)
  {
    if (myUsedSize >= availableSize())
    {
      expandBlock(sizeof(TheItemType));
    }
    pointer aPnt = &at(myUsedSize);
    constructAt(aPnt, std::forward<Args>(theArgs)...);
    ++myUsedSize;
    return *aPnt;
  }

  //! Emplace value at the specified index, constructing it in-place
  //! If the index is beyond current size, default-constructs intermediate elements
  //! @param theIndex index at which to emplace the value
  //! @param theArgs arguments forwarded to TheItemType constructor
  //! @return reference to the newly constructed item
  template <typename... Args>
  reference EmplaceValue(const size_t theIndex, Args&&... theArgs)
  {
    ensureStorageForIndex(theIndex, sizeof(TheItemType));
    const bool isExisting = theIndex < myUsedSize;
    if (!isExisting)
    {
      const size_t anOldSize = myUsedSize;
      for (size_t anIndex = anOldSize; anIndex < theIndex; ++anIndex)
      {
        pointer aPnt = &at(anIndex);
        constructAt(aPnt);
      }
      pointer aPnt = &at(theIndex);
      constructAt(aPnt, std::forward<Args>(theArgs)...);
      myUsedSize = theIndex + 1;
      return *aPnt;
    }
    pointer aPnt = &at(theIndex);
    replaceAt(aPnt, std::forward<Args>(theArgs)...);
    return *aPnt;
  }

  template <typename... Args>
  reference EmplaceValue(const int theIndex, Args&&... theArgs)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::EmplaceValue: index out of range");
    return EmplaceValue(static_cast<size_t>(theIndex), std::forward<Args>(theArgs)...);
  }

  //! Operator() - query the const value
  const_reference operator()(const size_t theIndex) const noexcept { return at(theIndex); }

  const_reference operator()(const int theIndex) const noexcept
  {
    return at(static_cast<size_t>(theIndex));
  }

  //! Operator[] - query the const value
  const_reference operator[](const size_t theIndex) const noexcept { return at(theIndex); }

  const_reference operator[](const int theIndex) const noexcept
  {
    return at(static_cast<size_t>(theIndex));
  }

  const_reference Value(const size_t theIndex) const noexcept { return at(theIndex); }

  const_reference Value(const int theIndex) const noexcept
  {
    return at(static_cast<size_t>(theIndex));
  }

  //! @return first element
  const_reference First() const noexcept { return blockAt(0)[0]; }

  //! @return first element
  reference ChangeFirst() noexcept { return blockAt(0)[0]; }

  //! @return last element
  const_reference Last() const noexcept { return at(myUsedSize - 1); }

  //! @return last element
  reference ChangeLast() noexcept { return at(myUsedSize - 1); }

  //! Operator() - query the value
  reference operator()(const size_t theIndex) noexcept { return at(theIndex); }

  reference operator()(const int theIndex) noexcept { return at(static_cast<size_t>(theIndex)); }

  //! Operator[] - query the value
  reference operator[](const size_t theIndex) noexcept { return at(theIndex); }

  reference operator[](const int theIndex) noexcept { return at(static_cast<size_t>(theIndex)); }

  reference ChangeValue(const size_t theIndex) noexcept { return at(theIndex); }

  reference ChangeValue(const int theIndex) noexcept { return at(static_cast<size_t>(theIndex)); }

  //! SetValue () - set or append a value
  reference SetValue(const size_t theIndex, const TheItemType& theValue)
  {
    ensureStorageForIndex(theIndex, sizeof(TheItemType));
    const bool isExisting = theIndex < myUsedSize;
    if (!isExisting)
    {
      const size_t anOldSize = myUsedSize;
      for (size_t anIndex = anOldSize; anIndex < theIndex; ++anIndex)
      {
        pointer aPnt = &at(anIndex);
        constructAt(aPnt);
      }
      pointer aPnt = &at(theIndex);
      constructAt(aPnt, theValue);
      myUsedSize = theIndex + 1;
      return *aPnt;
    }
    pointer aPnt = &at(theIndex);
    replaceAt(aPnt, theValue);
    return *aPnt;
  }

  //! SetValue () - set or append a value
  reference SetValue(const size_t theIndex, TheItemType&& theValue)
  {
    ensureStorageForIndex(theIndex, sizeof(TheItemType));
    const bool isExisting = theIndex < myUsedSize;
    if (!isExisting)
    {
      const size_t anOldSize = myUsedSize;
      for (size_t anIndex = anOldSize; anIndex < theIndex; ++anIndex)
      {
        pointer aPnt = &at(anIndex);
        constructAt(aPnt);
      }
      pointer aPnt = &at(theIndex);
      constructAt(aPnt, std::forward<TheItemType>(theValue));
      myUsedSize = theIndex + 1;
      return *aPnt;
    }
    pointer aPnt = &at(theIndex);
    replaceAt(aPnt, std::forward<TheItemType>(theValue));
    return *aPnt;
  }

  reference SetValue(const int theIndex, const TheItemType& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::SetValue: index out of range");
    return SetValue(static_cast<size_t>(theIndex), theValue);
  }

  reference SetValue(const int theIndex, TheItemType&& theValue)
  {
    Standard_OutOfRange_Raise_if(theIndex < 0,
                                 "NCollection_DynamicArray::SetValue: index out of range");
    return SetValue(static_cast<size_t>(theIndex), std::forward<TheItemType>(theValue));
  }

  void Clear(const bool theReleaseMemory = false)
  {
    for (size_t aBlockInd = 0; aBlockInd < blockCount(); aBlockInd++)
    {
      TheItemType* aCurStart = static_cast<TheItemType*>(blockAt(aBlockInd));
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        const size_t aBlockStart = aBlockInd * myInternalSize;
        const size_t aCount      = (myUsedSize > aBlockStart + myInternalSize)
                                     ? myInternalSize
                                     : (myUsedSize > aBlockStart ? myUsedSize - aBlockStart : 0);
        for (size_t anElemInd = 0; anElemInd < aCount; anElemInd++)
        {
          aCurStart[anElemInd].~TheItemType();
        }
      }
    }
    if (theReleaseMemory)
    {
      clearBlocks();
    }
    else
    {
      setUsedSize(0);
    }
  }

protected:
  static size_t roundUpPow2(const size_t theValue)
  {
    size_t aValue = theValue < 1 ? 1 : theValue;
    --aValue;
    aValue |= aValue >> 1;
    aValue |= aValue >> 2;
    aValue |= aValue >> 4;
    aValue |= aValue >> 8;
    aValue |= aValue >> 16;
    if constexpr (sizeof(size_t) > 4)
    {
      aValue |= aValue >> 32;
    }
    return aValue + 1;
  }

  static constexpr size_t log2Pow2(const size_t theValue) noexcept
  {
    size_t aShift = 0;
    size_t aValue = theValue;
    while (aValue > 1)
    {
      aValue >>= 1;
      ++aShift;
    }
    return aShift;
  }

  void* allocateBlock(const size_t theItemSize) const
  {
    const size_t aSize = myInternalSize * theItemSize;
    return myAllocator.IsNull() ? Standard::AllocateOptimal(aSize)
                                : myAllocator->AllocateOptimal(aSize);
  }

  void deallocateBlock(void* theBlock) const noexcept
  {
    if (myAllocator.IsNull())
    {
      Standard::Free(theBlock);
    }
    else
    {
      myAllocator->Free(theBlock);
    }
  }

  void ensureStorageForIndex(const size_t theIndex, const size_t theItemSize)
  {
    ensureBlockCount((theIndex >> myBlockShift) + 1, theItemSize);
  }

  void ensureBlockCount(const size_t theBlockCount, const size_t theItemSize)
  {
    if (theBlockCount > myContainer.Capacity())
    {
      myContainer.Reserve(theBlockCount);
    }
    while (myContainer.Size() < theBlockCount)
    {
      expandBlock(theItemSize);
    }
  }

  void* expandBlock(const size_t theItemSize)
  {
    void* aNewBlock = allocateBlock(theItemSize);
    myContainer.Append(static_cast<TheItemType*>(aNewBlock));
    return aNewBlock;
  }

  void* blockAddress(const size_t theBlockIndex) noexcept { return myContainer[theBlockIndex]; }

  const void* blockAddress(const size_t theBlockIndex) const noexcept
  {
    return myContainer[theBlockIndex];
  }

  size_t blockCount() const noexcept { return myContainer.Size(); }

  size_t availableSize() const noexcept { return myContainer.Size() << myBlockShift; }

  void clearBlocks() noexcept
  {
    for (size_t aBlockIndex = 0; aBlockIndex < myContainer.Size(); ++aBlockIndex)
    {
      deallocateBlock(myContainer[aBlockIndex]);
    }
    myContainer.Clear(true);
    myUsedSize = 0;
  }

  void setUsedSize(const size_t theSize) noexcept { myUsedSize = theSize; }

  void copyParameters(const NCollection_DynamicArray& theOther,
                      const bool                       theCopyAllocator) noexcept
  {
    if (theCopyAllocator)
    {
      myAllocator = theOther.myAllocator;
    }
    myInternalSize = theOther.myInternalSize;
    myBlockShift   = theOther.myBlockShift;
    myBlockMask    = theOther.myBlockMask;
    myUsedSize     = 0;
  }

  reference at(const size_t theInd) noexcept
  {
    return blockAt(theInd >> myBlockShift)[theInd & myBlockMask];
  }

  const_reference at(const size_t theInd) const noexcept
  {
    return blockAt(theInd >> myBlockShift)[theInd & myBlockMask];
  }

  void copyDate(const NCollection_DynamicArray& theOther)
  {
    size_t aUsedSize = 0;
    for (size_t aBlockInd = 0; aBlockInd < theOther.blockCount(); aBlockInd++)
    {
      const TheItemType* aCurStart = static_cast<const TheItemType*>(theOther.blockAt(aBlockInd));
      TheItemType*       aNewBlock = static_cast<TheItemType*>(allocateBlock(sizeof(TheItemType)));
      const size_t           aCount = aUsedSize < theOther.myUsedSize
                                        ? std::min(myInternalSize, theOther.myUsedSize - aUsedSize)
                                        : 0;
      if constexpr (std::is_trivially_copyable_v<TheItemType>)
      {
        std::memcpy(aNewBlock, aCurStart, aCount * sizeof(TheItemType));
        aUsedSize += aCount;
      }
      else
      {
        for (size_t anElemInd = 0; anElemInd < aCount; ++anElemInd)
        {
          constructAt(aNewBlock + anElemInd, aCurStart[anElemInd]);
        }
        aUsedSize += aCount;
      }
      myContainer.Append(aNewBlock);
      myUsedSize = aUsedSize;
    }
  }

  template <typename... Args>
  static void constructAt(pointer thePointer, Args&&... theArgs)
  {
    ::new (static_cast<void*>(thePointer)) TheItemType(std::forward<Args>(theArgs)...);
  }

  static void destroyAt(pointer thePointer) noexcept { thePointer->~TheItemType(); }

  template <typename... Args>
  static void replaceAt(pointer thePointer, Args&&... theArgs)
  {
    // Keep replacement available for constructible but non-assignable item types.
    if constexpr (std::is_assignable_v<TheItemType&, TheItemType&&>)
    {
      *thePointer = TheItemType(std::forward<Args>(theArgs)...);
    }
    else if constexpr (std::is_constructible_v<TheItemType, TheItemType&&>
                       || std::is_constructible_v<TheItemType, const TheItemType&>)
    {
      TheItemType aReplacement(std::forward<Args>(theArgs)...);
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        destroyAt(thePointer);
      }
      constructAt(thePointer, std::move_if_noexcept(aReplacement));
    }
    else
    {
      if constexpr (!std::is_trivially_destructible_v<TheItemType>)
      {
        destroyAt(thePointer);
      }
      constructAt(thePointer, std::forward<Args>(theArgs)...);
    }
  }

  pointer blockAt(const size_t theBlockIndex) noexcept
  {
    return static_cast<pointer>(blockAddress(theBlockIndex));
  }

  const_pointer blockAt(const size_t theBlockIndex) const noexcept
  {
    return static_cast<const_pointer>(blockAddress(theBlockIndex));
  }

private:
  NCollection_LinearVector<TheItemType*> myContainer;
  occ::handle<NCollection_BaseAllocator> myAllocator;
  size_t                                 myInternalSize = 1;
  size_t                                 myBlockShift   = 0;
  size_t                                 myBlockMask    = 0;
  size_t                                 myUsedSize     = 0;
};

#endif // NCollection_DynamicArray_HeaderFile
