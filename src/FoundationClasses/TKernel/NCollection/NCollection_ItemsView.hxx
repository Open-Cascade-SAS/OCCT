// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef NCollection_ItemsView_HeaderFile
#define NCollection_ItemsView_HeaderFile

#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>

//! @file NCollection_ItemsView.hxx
//! @brief Template utilities for key-value pair iteration with structured bindings.
//!
//! This header provides reusable template classes for implementing Items() views
//! across NCollection map classes, enabling C++17 structured binding syntax.
//!
//! All utilities are organized under the NCollection_ItemsView namespace.

namespace NCollection_ItemsView
{

//==================================================================================================

//! Key-value pair reference for structured binding support.
//! Enables: for (auto [key, value] : map.Items())
//! @tparam TheKeyType   the key type
//! @tparam TheValueType the value type
//! @tparam IsConst      if true, value is const reference; if false, mutable reference
template <class TheKeyType, class TheValueType, bool IsConst>
struct KeyValueRef
{
  using ValueRef = std::conditional_t<IsConst, const TheValueType&, TheValueType&>;

  const TheKeyType& Key;
  ValueRef          Value;

  template <std::size_t I>
  decltype(auto) get() const noexcept
  {
    if constexpr (I == 0)
      return Key;
    else if constexpr (I == 1)
      return Value;
  }
};

//==================================================================================================

//! Key-value-index tuple reference for structured binding support.
//! Enables: for (auto [key, value, index] : map.IndexedItems())
//! @tparam TheKeyType   the key type
//! @tparam TheValueType the value type
//! @tparam IsConst      if true, value is const reference; if false, mutable reference
template <class TheKeyType, class TheValueType, bool IsConst>
struct KeyValueIndexRef
{
  using ValueRef = std::conditional_t<IsConst, const TheValueType&, TheValueType&>;

  const TheKeyType& Key;
  ValueRef          Value;
  int               Index;

  template <std::size_t I>
  decltype(auto) get() const noexcept
  {
    if constexpr (I == 0)
      return Key;
    else if constexpr (I == 1)
      return Value;
    else if constexpr (I == 2)
      return Index;
  }
};

//==================================================================================================

//! Key-index pair reference for structured binding support (key-only indexed maps).
//! Enables: for (auto [key, index] : map.IndexedItems())
//! @tparam TheKeyType the key type
template <class TheKeyType>
struct KeyIndexRef
{
  const TheKeyType& Key;
  int               Index;

  template <std::size_t I>
  decltype(auto) get() const noexcept
  {
    if constexpr (I == 0)
      return Key;
    else if constexpr (I == 1)
      return Index;
  }
};

//==================================================================================================

//! Generic forward iterator for View classes.
//! @tparam BaseIterator the map's native Iterator type
//! @tparam RefType      the reference type returned by operator*
//! @tparam Extractor    functor class with static Extract(iter) returning RefType
template <class BaseIterator, class RefType, class Extractor>
class Iterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = RefType;
  using difference_type   = std::ptrdiff_t;
  using pointer           = void;
  using reference         = RefType;

  //! Default constructor (creates end iterator)
  Iterator() = default;

  //! Constructor from map
  template <class MapType>
  explicit Iterator(const MapType& theMap)
      : myInner(theMap)
  {
  }

  //! Dereference operator
  RefType operator*() const { return Extractor::Extract(myInner); }

  //! Prefix increment
  Iterator& operator++()
  {
    myInner.Next();
    return *this;
  }

  //! Postfix increment
  Iterator operator++(int)
  {
    Iterator aTmp = *this;
    ++(*this);
    return aTmp;
  }

  //! Equality comparison
  bool operator==(const Iterator& theOther) const
  {
    return myInner.More() == theOther.myInner.More()
           && (!myInner.More() || myInner.IsEqual(theOther.myInner));
  }

  //! Inequality comparison
  bool operator!=(const Iterator& theOther) const { return !(*this == theOther); }

private:
  mutable BaseIterator myInner;
};

//==================================================================================================

//! Generic view class for Items() iteration.
//! @tparam MapType   the map class type
//! @tparam RefType   the reference type for structured bindings
//! @tparam Extractor functor class for extracting RefType from iterator
//! @tparam IsConst   if true, this is a const view
template <class MapType, class RefType, class Extractor, bool IsConst>
class View
{
public:
  using MapRef   = std::conditional_t<IsConst, const MapType&, MapType&>;
  using iterator = Iterator<typename MapType::Iterator, RefType, Extractor>;

  //! Constructor
  explicit View(MapRef theMap)
      : myMap(theMap)
  {
  }

  //! Returns iterator to first element
  iterator begin() const { return iterator(myMap); }

  //! Returns iterator past the end
  iterator end() const { return iterator(); }

private:
  MapRef myMap;
};

} // namespace NCollection_ItemsView

//==================================================================================================

// Structured binding support (std::tuple_size and std::tuple_element specializations)

namespace std
{

// KeyValueRef tuple interface
template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_size<NCollection_ItemsView::KeyValueRef<TheKeyType, TheValueType, IsConst>>
    : std::integral_constant<std::size_t, 2>
{
};

template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_element<0, NCollection_ItemsView::KeyValueRef<TheKeyType, TheValueType, IsConst>>
{
  using type = const TheKeyType&;
};

template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_element<1, NCollection_ItemsView::KeyValueRef<TheKeyType, TheValueType, IsConst>>
{
  using type =
    typename NCollection_ItemsView::KeyValueRef<TheKeyType, TheValueType, IsConst>::ValueRef;
};

// KeyValueIndexRef tuple interface
template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_size<NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheValueType, IsConst>>
    : std::integral_constant<std::size_t, 3>
{
};

template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_element<0, NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheValueType, IsConst>>
{
  using type = const TheKeyType&;
};

template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_element<1, NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheValueType, IsConst>>
{
  using type =
    typename NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheValueType, IsConst>::ValueRef;
};

template <class TheKeyType, class TheValueType, bool IsConst>
struct tuple_element<2, NCollection_ItemsView::KeyValueIndexRef<TheKeyType, TheValueType, IsConst>>
{
  using type = int;
};

// KeyIndexRef tuple interface
template <class TheKeyType>
struct tuple_size<NCollection_ItemsView::KeyIndexRef<TheKeyType>>
    : std::integral_constant<std::size_t, 2>
{
};

template <class TheKeyType>
struct tuple_element<0, NCollection_ItemsView::KeyIndexRef<TheKeyType>>
{
  using type = const TheKeyType&;
};

template <class TheKeyType>
struct tuple_element<1, NCollection_ItemsView::KeyIndexRef<TheKeyType>>
{
  using type = int;
};

} // namespace std

#endif // NCollection_ItemsView_HeaderFile
