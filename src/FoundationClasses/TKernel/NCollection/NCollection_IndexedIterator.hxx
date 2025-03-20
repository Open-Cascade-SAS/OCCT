// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef NCollection_IndexedIterator_HeaderFile
#define NCollection_IndexedIterator_HeaderFile

#include <Standard_Assert.hxx>
#include <iterator>

//! Helper class that allows to use NCollection iterators as STL iterators.
//! NCollection iterator can be extended to STL iterator of any category by
//! adding necessary methods: STL forward iterator requires IsEqual method,
//! STL bidirectional iterator requires Previous method, and STL random access
//! iterator requires Offset and Differ methods. See NCollection_Vector as
//! example of declaring custom STL iterators.
template <class Category, class BaseIndexedMap, class ItemType, bool IsConstant>
class NCollection_IndexedIterator
{
public:
  // Since C++20 inheritance from std::iterator is deprecated, so define predefined types manually:
  using iterator_category = Category;
  using value_type        = ItemType;
  using difference_type   = ptrdiff_t;
  using pointer           = typename std::conditional<IsConstant, const ItemType*, ItemType*>::type;
  using reference         = typename std::conditional<IsConstant, const ItemType&, ItemType&>::type;

  //! Default constructor
  NCollection_IndexedIterator()
      : myIndex(0),
        myIndexedMap(nullptr)
  {
  }

  //! Constructor from NCollection_Indexed*Map
  NCollection_IndexedIterator(const BaseIndexedMap& theMap)
      : myIndex(0),
        myIndexedMap((&const_cast<BaseIndexedMap&>(theMap)))
  {
  }

  //! Constructor from NCollection_Indexed*Map
  NCollection_IndexedIterator(const size_t theIndex, const BaseIndexedMap& theMap)
      : myIndex(theIndex),
        myIndexedMap(&const_cast<BaseIndexedMap&>(theMap))
  {
  }

  //! Cast from non-const variant to const one
  NCollection_IndexedIterator(
    const NCollection_IndexedIterator<Category, BaseIndexedMap, ItemType, false>& theOther)
      : myIndex(theOther.myIndex),
        myIndexedMap(theOther.myIndexedMap)
  {
  }

  //! Assignment of non-const iterator to const one
  NCollection_IndexedIterator& operator=(
    const NCollection_IndexedIterator<Category, BaseIndexedMap, ItemType, false>& theOther)
  {
    myIndex      = theOther.myIndex;
    myIndexedMap = theOther.myIndexedMap;
    return *this;
  }

protected: //! @name methods related to forward STL iterator
  // Note: Here we use SFINAE (Substitution failure is not an error) to choose
  // an appropriate method based on template arguments (at instantiation time).

  template <bool Condition>
  typename std::enable_if<!Condition, ItemType&>::type Reference() const
  {
    return myIndexedMap->at(myIndex);
  }

  template <bool Condition>
  typename std::enable_if<Condition, const ItemType&>::type Reference() const
  {
    return myIndexedMap->at(myIndex);
  }

public: //! @name methods related to forward STL iterator
  //! Test for equality
  bool operator==(const NCollection_IndexedIterator& theOther) const
  {
    return myIndexedMap == theOther.myIndexedMap && myIndex == theOther.myIndex;
  }

  template <bool theOtherIsConstant>
  bool operator==(
    const NCollection_IndexedIterator<Category, BaseIndexedMap, ItemType, theOtherIsConstant>&
      theOther) const
  {
    return myIndexedMap == theOther.myIndexedMap && myIndex == theOther.myIndex;
  }

  template <bool theOtherIsConstant>
  bool operator!=(
    const NCollection_IndexedIterator<Category, BaseIndexedMap, ItemType, theOtherIsConstant>&
      theOther) const
  {
    return myIndexedMap != theOther.myIndexedMap || myIndex != theOther.myIndex;
  }

  //! Test for inequality
  bool operator!=(const NCollection_IndexedIterator& theOther) const
  {
    return !(*this == theOther);
  }

  //! Get reference to current item
  typename NCollection_IndexedIterator::reference operator*() const
  {
    return Reference<IsConstant>();
  }

  //! Dereferencing operator
  typename NCollection_IndexedIterator::pointer operator->() const
  {
    return &Reference<IsConstant>();
  }

  //! Prefix increment
  NCollection_IndexedIterator& operator++()
  {
    myIndex++;
    return *this;
  }

  //! Postfix increment
  NCollection_IndexedIterator operator++(int)
  {
    const NCollection_IndexedIterator theOld(*this);
    ++(*this);
    return theOld;
  }

public: //! @name methods related to bidirectional STL iterator
  //! Prefix decrement
  NCollection_IndexedIterator& operator--()
  {
    Standard_STATIC_ASSERT(
      (opencascade::std::is_same<std::bidirectional_iterator_tag, Category>::value
       || opencascade::std::is_same<std::random_access_iterator_tag, Category>::value));
    myIndex--;
    return *this;
  }

  //! Postfix decrement
  NCollection_IndexedIterator operator--(int)
  {
    NCollection_IndexedIterator theOld(*this);
    --(*this);
    return theOld;
  }

public: //! @name methods related to random access STL iterator
  //! Move forward
  NCollection_IndexedIterator& operator+=(
    typename NCollection_IndexedIterator::difference_type theOffset)
  {
    Standard_STATIC_ASSERT(
      (opencascade::std::is_same<std::random_access_iterator_tag, Category>::value));
    myIndex += theOffset;
    return *this;
  }

  //! Addition
  NCollection_IndexedIterator operator+(
    typename NCollection_IndexedIterator::difference_type theOffset) const
  {
    NCollection_IndexedIterator aTemp(*this);
    return aTemp += theOffset;
  }

  //! Move backward
  NCollection_IndexedIterator& operator-=(
    typename NCollection_IndexedIterator::difference_type theOffset)
  {
    return *this += -theOffset;
  }

  //! Decrease
  NCollection_IndexedIterator operator-(
    typename NCollection_IndexedIterator::difference_type theOffset) const
  {
    NCollection_IndexedIterator aTemp(*this);
    return aTemp += -theOffset;
  }

  //! Difference
  typename NCollection_IndexedIterator::difference_type operator-(
    const NCollection_IndexedIterator& theOther) const
  {
    Standard_STATIC_ASSERT(
      (opencascade::std::is_same<std::random_access_iterator_tag, Category>::value));
    return myIndex - theOther.myIndex;
  }

  //! Get item at offset from current
  typename NCollection_IndexedIterator::reference operator[](
    typename NCollection_IndexedIterator::difference_type theOffset) const
  {
    return *(*this + theOffset);
  }

  //! Comparison
  bool operator<(const NCollection_IndexedIterator& theOther) const
  {
    return (*this - theOther) < 0;
  }

  //! Comparison
  bool operator>(const NCollection_IndexedIterator& theOther) const { return theOther < *this; }

  //! Comparison
  bool operator<=(const NCollection_IndexedIterator& theOther) const { return !(theOther < *this); }

  //! Comparison
  bool operator>=(const NCollection_IndexedIterator& theOther) const { return !(*this < theOther); }

  friend class NCollection_IndexedIterator<Category, BaseIndexedMap, ItemType, !IsConstant>;

private:
  //! NCollection iterator
  size_t          myIndex;
  BaseIndexedMap* myIndexedMap;
};

#endif // NCollection_IndexedIterator_HeaderFile
