// Created on: 2006-11-23
// Created by: Andrey BETENEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef NCollection_SparseArray_HeaderFile
#define NCollection_SparseArray_HeaderFile

#include <NCollection_SparseArrayBase.hxx>

/**
 * Dynamically resizable sparse array of objects
 *
 * This class is similar to NCollection_Vector: it works like virtually
 * unlimited array of items accessible by index; however unlike simple
 * Vector it distinguishes items that have been set from the ones that
 * have not been set explicitly.
 *
 * This class can be also seen as equivalence of
 * NCollection_DataMap<int,TheItemType>
 * with the only one practical difference: it can be much less
 * memory-expensive if items are small (e.g. Integer or Handle).
 *
 * The index starts from 0, i.e. should be non-negative. Memory is allocated
 * when item is set by SetValue().
 *
 * Iterator returns only defined items;
 * the item can be tested for being defined by IsSet(),
 * and undefined by UnsetValue().
 *
 * The attempt to access the item that has not been set will result
 * in OutOfRange exception in Debug mode; in Release mode this will either
 * return null-filled object or cause access violation.
 */

template <class TheItemType>
class NCollection_SparseArray : public NCollection_SparseArrayBase
{
public:
  //! Constructor; accepts size of blocks
  explicit NCollection_SparseArray(size_t theIncrement) noexcept
      : NCollection_SparseArrayBase(sizeof(TheItemType), theIncrement, destroyItemImpl)
  {
  }

  //! Explicit assignment operator
  NCollection_SparseArray& Assign(const NCollection_SparseArray& theOther)
  {
    if (this == &theOther)
      return *this;
    this->assign(theOther, createItemImpl, destroyItemImpl, copyItemImpl);
    return *this;
  }

  //! Exchange the data of two arrays;
  //! can be used primarily to move contents of theOther into the new array
  //! in a fast way (without creation of duplicated data)
  void Exchange(NCollection_SparseArray& theOther) noexcept { this->exchange(theOther); }

  //! Clears all the data
  void Clear() { clearItems(destroyItemImpl); }

public:
  //!@name Array-like interface (in addition to inherited methods)
  //!@{

  //! Direct const access to the item
  const TheItemType& Value(const size_t theIndex) const
  {
    return *(const TheItemType*)this->getValue(theIndex);
  }

  //! Const access to the item - operator()
  const TheItemType& operator()(const size_t theIndex) const { return Value(theIndex); }

  //! Modification access to the item
  TheItemType& ChangeValue(const size_t theIndex)
  {
    return *(TheItemType*)(this->getValue(theIndex));
  }

  //! Access to the item - operator()
  TheItemType& operator()(const size_t theIndex) { return ChangeValue(theIndex); }

  //! Set a value at specified index method
  TheItemType& SetValue(const size_t theIndex, const TheItemType& theValue)
  {
    return *(TheItemType*)this->setValue(theIndex, (void*)&theValue, createItemImpl, copyItemImpl);
  }

  //! Deletes the item from the array;
  //! returns True if that item was defined
  bool UnsetValue(const size_t theIndex) { return this->unsetValue(theIndex, destroyItemImpl); }

  //!@}

public:
  //!@name DataMap-like interface
  //!@{

  //! Returns number of items in the array
  size_t Extent() const noexcept { return Size(); }

  //! Returns True if array is empty
  bool IsEmpty() const noexcept { return Size() == 0; }

  //! Direct const access to the item
  const TheItemType& Find(const size_t theIndex) const { return Value(theIndex); }

  //! Modification access to the item
  TheItemType& ChangeFind(const size_t theIndex) { return ChangeValue(theIndex); }

  //! Set a value as explicit method
  TheItemType& Bind(const size_t theIndex, const TheItemType& theValue)
  {
    return SetValue(theIndex, theValue);
  }

  //! Returns True if the item is defined
  bool IsBound(const size_t theIndex) const { return this->HasValue(theIndex); }

  //! Remove the item from array
  bool UnBind(const size_t theIndex) { return UnsetValue(theIndex); }

  //!@}

public:
  // Iterator interface

  /**
   * Implementation of type-specific const Iterator class
   */
  class ConstIterator : public NCollection_SparseArrayBase::Iterator
  {
  public:
    //! Empty constructor - for later Init
    ConstIterator() noexcept = default;

    //! Constructor with initialisation
    ConstIterator(const NCollection_SparseArray& theVector)
        : NCollection_SparseArrayBase::Iterator(&theVector)
    {
    }

    //! Initialisation
    void Init(const NCollection_SparseArray& theVector) { this->init(&theVector); }

    //! Constant value access
    const TheItemType& Value() const { return *(const TheItemType*)this->value(); }

    //! Constant value access operator
    const TheItemType& operator()() const { return *(const TheItemType*)this->value(); }

    //! Access current index with 'a-la map' interface
    size_t Key() const noexcept { return Index(); }
  };

  /**
   * Implementation of type-specific non-const Iterator class
   */
  class Iterator : public ConstIterator
  {
  public:
    //! Empty constructor - for later Init
    Iterator() noexcept = default;

    //! Constructor with initialisation
    Iterator(NCollection_SparseArray& theVector)
        : ConstIterator(theVector)
    {
    }

    //! Initialisation
    void Init(const NCollection_SparseArray& theVector) { this->init(&theVector); }

    //! Value access
    TheItemType& ChangeValue() { return *(TheItemType*)this->value(); }

    //! Value access operator
    TheItemType& operator()() { return *(TheItemType*)this->value(); }

    //! Const access operator - the same as in parent class
    const TheItemType& operator()() const { return *(const TheItemType*)this->value(); }
  };

private:
  // Static functions providing type-specific item operations

  //! Copy-construct a new item from existing item
  static void createItemImpl(void* theAddress, void* theOther)
  {
    new (theAddress) TheItemType(*(const TheItemType*)theOther);
  }

  //! Call destructor on the item at given address
  static void destroyItemImpl(void* theAddress)
  {
    ((TheItemType*)theAddress)->TheItemType::~TheItemType();
  }

  //! Call assignment operator on the item
  static void copyItemImpl(void* theAddress, void* theOther)
  {
    (*(TheItemType*)theAddress) = *(const TheItemType*)theOther;
  }
};

#endif
