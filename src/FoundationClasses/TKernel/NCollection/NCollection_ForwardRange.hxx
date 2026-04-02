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

#ifndef NCollection_ForwardRange_HeaderFile
#define NCollection_ForwardRange_HeaderFile

#include <iterator>
#include <type_traits>
#include <utility>

//! @file NCollection_ForwardRange.hxx
//! @brief STL range-for adapter for OCCT iterators following the More()/Next() pattern.
//!
//! Provides reusable iterator/sentinel types that enable range-based for loops
//! on any OCCT iterator or explorer class with More(), Next(), and a value accessor.
//!
//! The value accessor is detected automatically via SFINAE with priority:
//! Value() > Current() > CurrentId().
//!
//! Two usage modes:
//! 1. In-class integration: add begin()/end() methods using the provided types.
//!    @code
//!      NCollection_ForwardRangeIterator<MyIterator> begin()
//!      {
//!        return NCollection_ForwardRangeIterator<MyIterator>(this);
//!      }
//!      NCollection_ForwardRangeSentinel end() const { return {}; }
//!    @endcode
//! 2. Standalone wrapper for unmodified classes:
//!    @code
//!      for (const TopoDS_Shape& aFace :
//!           NCollection_ForwardRange(TopExp_Explorer(aShape, TopAbs_FACE)))
//!      {
//!        // ...
//!      }
//!    @endcode

namespace NCollection_ForwardRangeDetail
{

//! SFINAE: detect .Value() const
template <typename T, typename = void>
struct HasValue : std::false_type
{
};

template <typename T>
struct HasValue<T, std::void_t<decltype(std::declval<const T&>().Value())>> : std::true_type
{
};

//! SFINAE: detect .Current() const
template <typename T, typename = void>
struct HasCurrent : std::false_type
{
};

template <typename T>
struct HasCurrent<T, std::void_t<decltype(std::declval<const T&>().Current())>> : std::true_type
{
};

//! SFINAE: detect .CurrentId() const
template <typename T, typename = void>
struct HasCurrentId : std::false_type
{
};

template <typename T>
struct HasCurrentId<T, std::void_t<decltype(std::declval<const T&>().CurrentId())>>
    : std::true_type
{
};

//! Accessor dispatch trait: resolves to the correct value accessor and its return type.
//! Uses std::enable_if_t for mutually-exclusive priority selection.
template <typename T, typename = void>
struct AccessorTraits;

//! Priority 1: has Value()
template <typename T>
struct AccessorTraits<T, std::enable_if_t<HasValue<T>::value>>
{
  using ReturnType = decltype(std::declval<const T&>().Value());

  static ReturnType Get(const T& theIter) { return theIter.Value(); }
};

//! Priority 2: has Current() but not Value()
template <typename T>
struct AccessorTraits<T, std::enable_if_t<!HasValue<T>::value && HasCurrent<T>::value>>
{
  using ReturnType = decltype(std::declval<const T&>().Current());

  static ReturnType Get(const T& theIter) { return theIter.Current(); }
};

//! Priority 3: has CurrentId() but not Value() or Current()
template <typename T>
struct AccessorTraits<T,
                      std::enable_if_t<!HasValue<T>::value && !HasCurrent<T>::value
                                       && HasCurrentId<T>::value>>
{
  using ReturnType = decltype(std::declval<const T&>().CurrentId());

  static ReturnType Get(const T& theIter) { return theIter.CurrentId(); }
};

//! Proxy for operator-> when the accessor returns a value (not a reference).
template <typename ValueT>
struct ArrowProxy
{
  ValueT myValue;

  const ValueT* operator->() const { return &myValue; }
};

} // namespace NCollection_ForwardRangeDetail

//! Empty sentinel type used as the end marker for range-for loops.
struct NCollection_ForwardRangeSentinel
{
};

//! @brief STL input iterator that wraps an OCCT More()/Next() iterator.
//!
//! Holds a non-owning pointer to the host iterator/explorer.
//! The host must outlive this iterator (guaranteed by range-for semantics).
//!
//! @tparam HostType OCCT iterator/explorer with More(), Next(), and a value accessor.
template <typename HostType>
class NCollection_ForwardRangeIterator
{
  using Accessor  = NCollection_ForwardRangeDetail::AccessorTraits<HostType>;
  using RawReturn = typename Accessor::ReturnType;

public:
  using value_type        = std::remove_cv_t<std::remove_reference_t<RawReturn>>;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;
  using reference = RawReturn;
  using pointer = std::conditional_t<std::is_reference_v<RawReturn>,
                                     std::add_pointer_t<std::remove_reference_t<RawReturn>>,
                                     NCollection_ForwardRangeDetail::ArrowProxy<value_type>>;

  //! Construct from a pointer to the host iterator.
  explicit NCollection_ForwardRangeIterator(HostType* theHost)
      : myHost(theHost)
  {
  }

  //! Dereference: returns the current value from the host.
  RawReturn operator*() const { return Accessor::Get(*myHost); }

  //! Arrow operator: returns a pointer or proxy to the current value.
  pointer operator->() const
  {
    if constexpr (std::is_reference_v<RawReturn>)
    {
      return &Accessor::Get(*myHost);
    }
    else
    {
      return NCollection_ForwardRangeDetail::ArrowProxy<value_type>{Accessor::Get(*myHost)};
    }
  }

  //! Prefix increment: advances the host iterator.
  NCollection_ForwardRangeIterator& operator++()
  {
    myHost->Next();
    return *this;
  }

  //! Postfix increment: captures the current value, then advances.
  //! Returns a proxy holding the old value (safe for non-copyable hosts).
  struct PostfixProxy
  {
    value_type myValue;

    const value_type& operator*() const { return myValue; }
  };

  PostfixProxy operator++(int)
  {
    PostfixProxy aProxy{Accessor::Get(*myHost)};
    myHost->Next();
    return aProxy;
  }

  //! Equality with sentinel: true when the host is exhausted.
  friend bool operator==(const NCollection_ForwardRangeIterator& theLhs,
                          NCollection_ForwardRangeSentinel)
  {
    return !theLhs.myHost->More();
  }

  friend bool operator!=(const NCollection_ForwardRangeIterator& theLhs,
                          NCollection_ForwardRangeSentinel)
  {
    return theLhs.myHost->More();
  }

  friend bool operator==(NCollection_ForwardRangeSentinel,
                          const NCollection_ForwardRangeIterator& theRhs)
  {
    return !theRhs.myHost->More();
  }

  friend bool operator!=(NCollection_ForwardRangeSentinel,
                          const NCollection_ForwardRangeIterator& theRhs)
  {
    return theRhs.myHost->More();
  }

private:
  HostType* myHost; //!< Non-owning pointer to the host iterator.
};

//! @brief Standalone range wrapper for OCCT iterators.
//!
//! Owns a copy/move of the host iterator and provides begin()/end()
//! for range-based for loops. Use this when the host class does not
//! have its own begin()/end() methods.
//!
//! @code
//!   for (const TopoDS_Shape& aFace :
//!        NCollection_ForwardRange(TopExp_Explorer(aShape, TopAbs_FACE)))
//!   {
//!     // process aFace
//!   }
//! @endcode
//!
//! @tparam HostType OCCT iterator/explorer with More(), Next(), and a value accessor.
template <typename HostType>
class NCollection_ForwardRange
{
public:
  using iterator       = NCollection_ForwardRangeIterator<HostType>;
  using const_iterator = iterator;
  using sentinel       = NCollection_ForwardRangeSentinel;

  //! Construct from a copyable host (lvalue).
  template <typename U = HostType, typename = std::enable_if_t<std::is_copy_constructible_v<U>>>
  explicit NCollection_ForwardRange(const HostType& theHost)
      : myHost(theHost)
  {
  }

  //! Construct from an rvalue (moves the host).
  explicit NCollection_ForwardRange(HostType&& theHost)
      : myHost(std::move(theHost))
  {
  }

  NCollection_ForwardRange(const NCollection_ForwardRange&) = delete;
  NCollection_ForwardRange& operator=(const NCollection_ForwardRange&) = delete;
  NCollection_ForwardRange(NCollection_ForwardRange&&)                 = default;
  NCollection_ForwardRange& operator=(NCollection_ForwardRange&&) = default;

  //! Returns iterator to the current position of the host.
  //! Const-qualified: iteration advances the internal cursor (mutable),
  //! not the logical state of the range.
  [[nodiscard]] iterator begin() const { return iterator(&myHost); }

  //! Returns sentinel marking the end.
  [[nodiscard]] sentinel end() const { return sentinel{}; }

  //! Returns const-access iterator (same as begin).
  [[nodiscard]] const_iterator cbegin() const { return const_iterator(&myHost); }

  //! Returns sentinel marking the end.
  [[nodiscard]] sentinel cend() const { return sentinel{}; }

private:
  mutable HostType myHost; //!< Owned host iterator (mutable: iteration is cursor movement).
};

//! CTAD deduction guide: deduces HostType from the constructor argument.
template <typename T>
NCollection_ForwardRange(T&&) -> NCollection_ForwardRange<std::remove_reference_t<T>>;

#endif // NCollection_ForwardRange_HeaderFile
