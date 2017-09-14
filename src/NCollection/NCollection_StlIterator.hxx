// Created on: 2014-04-15
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef NCollection_StlIterator_HeaderFile
#define NCollection_StlIterator_HeaderFile

#include <Standard_Assert.hxx>
#include <iterator>

//! Helper class that allows to use NCollection iterators as STL iterators.
//! NCollection iterator can be extended to STL iterator of any category by
//! adding necessary methods: STL forward iterator requires IsEqual method,
//! STL bidirectional iterator requires Previous method, and STL random access
//! iterator requires Offset and Differ methods. See NCollection_Vector as
//! example of declaring custom STL iterators.
template<class Category, class BaseIterator, class ItemType, bool IsConstant>
class NCollection_StlIterator :
  public std::iterator<Category, ItemType, ptrdiff_t,
                       typename opencascade::std::conditional<IsConstant, const ItemType*, ItemType*>::type,
                       typename opencascade::std::conditional<IsConstant, const ItemType&, ItemType&>::type>
{
public:

  //! Default constructor
  NCollection_StlIterator () {}

  //! Constructor from NCollection iterator
  NCollection_StlIterator (const BaseIterator& theIterator)
    : myIterator (theIterator)
  { }

  //! Cast from non-const variant to const one
  NCollection_StlIterator (const NCollection_StlIterator<Category, BaseIterator, ItemType, false>& theIterator)
    : myIterator (theIterator.Iterator())
  { }

  //! Assignment of non-const iterator to const one
  NCollection_StlIterator& operator= (const NCollection_StlIterator<Category, BaseIterator, ItemType, false>& theIterator)
  {
    myIterator = theIterator.myIterator;
    return *this;
  }

  //! Access to NCollection iterator instance
  const BaseIterator& Iterator () const
  {
    return myIterator;
  }

  //! Access to NCollection iterator instance
  BaseIterator& ChangeIterator()
  {
    return myIterator;
  }

protected: //! @name methods related to forward STL iterator

  // Note: Here we use SFINAE (Substitution failure is not an error) to choose
  // an appropriate method based on template arguments (at instantiation time).

  template<bool Condition>
  typename opencascade::std::enable_if<!Condition, ItemType&>::type Reference() const
  {
    return myIterator.ChangeValue();
  }

  template<bool Condition>
  typename opencascade::std::enable_if<Condition, const ItemType&>::type Reference() const
  {
    return myIterator.Value();
  }

public: //! @name methods related to forward STL iterator

  //! Test for equality
  bool operator== (const NCollection_StlIterator& theOther) const
  {
    return myIterator.More() == theOther.myIterator.More() &&
           (!myIterator.More() || myIterator.IsEqual (theOther.myIterator));
  }

  //! Test for inequality
  bool operator!= (const NCollection_StlIterator& theOther) const
  {
    return !(*this == theOther);
  }

  //! Get reference to current item
  typename NCollection_StlIterator::reference operator*() const
  {
    return Reference<IsConstant>();
  }

  //! Dereferencing operator
  typename NCollection_StlIterator::pointer operator->() const
  {
    return &Reference<IsConstant>();
  }

  //! Prefix increment
  NCollection_StlIterator& operator++()
  {
    myIterator.Next();
    return *this;
  }

  //! Postfix increment
  NCollection_StlIterator operator++(int)
  {
    const NCollection_StlIterator theOld (*this);
    ++(*this);
    return theOld;
  }

public: //! @name methods related to bidirectional STL iterator
  
  //! Prefix decrement
  NCollection_StlIterator& operator--()
  {
    Standard_STATIC_ASSERT((opencascade::std::is_same<std::bidirectional_iterator_tag,Category>::value ||
                            opencascade::std::is_same<std::random_access_iterator_tag,Category>::value));
    myIterator.Previous();
    return *this;
  }
  
  //! Postfix decrement
  NCollection_StlIterator operator--(int)
  {
    NCollection_StlIterator theOld (*this);
    --(*this);
    return theOld;
  }
  
public: //! @name methods related to random access STL iterator

  //! Move forward
  NCollection_StlIterator& operator+= (typename NCollection_StlIterator::difference_type theOffset)
  {
    Standard_STATIC_ASSERT((opencascade::std::is_same<std::random_access_iterator_tag,Category>::value));
    myIterator.Offset (theOffset);
    return *this;
  }

  //! Addition
  NCollection_StlIterator operator+ (typename NCollection_StlIterator::difference_type theOffset) const
  {
    NCollection_StlIterator aTemp (*this);
    return aTemp += theOffset;
  }

  //! Move backward
  NCollection_StlIterator& operator-= (typename NCollection_StlIterator::difference_type theOffset)
  {
    return *this += -theOffset;
  }

  //! Decrease
  NCollection_StlIterator operator- (typename NCollection_StlIterator::difference_type theOffset) const
  {
    NCollection_StlIterator aTemp (*this);
    return aTemp += -theOffset;
  }

  //! Difference
  typename NCollection_StlIterator::difference_type operator- (const NCollection_StlIterator& theOther) const
  {
    Standard_STATIC_ASSERT((opencascade::std::is_same<std::random_access_iterator_tag,Category>::value));
    return myIterator.Differ (theOther.myIterator);
  }

  //! Get item at offset from current
  typename NCollection_StlIterator::reference operator[] (typename NCollection_StlIterator::difference_type theOffset) const
  {
    return *(*this + theOffset);
  }
  
  //! Comparison
  bool operator< (const NCollection_StlIterator& theOther) const
  {
    return (*this - theOther) < 0;
  }

  //! Comparison
  bool operator> (const NCollection_StlIterator& theOther) const
  {
    return theOther < *this;
  }

  //! Comparison
  bool operator<= (const NCollection_StlIterator& theOther) const
  {
    return !(theOther < *this);
  }

  //! Comparison
  bool operator>= (const NCollection_StlIterator& theOther) const
  {
    return !(*this < theOther);
  }

private:
  //! NCollection iterator
  BaseIterator myIterator;
};

#endif // NCollection_StlIterator_HeaderFile
