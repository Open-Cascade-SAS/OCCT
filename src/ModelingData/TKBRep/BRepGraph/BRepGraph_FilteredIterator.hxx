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

#ifndef _BRepGraph_FilteredIterator_HeaderFile
#define _BRepGraph_FilteredIterator_HeaderFile

#include <NCollection_ForwardRange.hxx>

#include <cstdint>
#include <type_traits>
#include <utility>

//! @brief Predicate adapter for OCCT More()/Next() iterators.
//!
//! The predicate receives the inner iterator, so filters can inspect any method
//! exposed by that iterator: Current(), CurrentId(), CurrentRefId(), Index(), or
//! graph data captured by the predicate.
template <typename IteratorT, typename PredicateT>
class BRepGraph_FilteredIterator
{
public:
  BRepGraph_FilteredIterator(IteratorT theIterator, PredicateT thePredicate)
      : myIterator(std::move(theIterator)),
        myPredicate(std::move(thePredicate))
  {
    skipRejected();
  }

  [[nodiscard]] bool More() const { return myIterator.More(); }

  [[nodiscard]] uint32_t Count() const
  {
    uint32_t aCount = 0;
    for (BRepGraph_FilteredIterator anIt(*this); anIt.More(); anIt.Next())
    {
      ++aCount;
    }
    return aCount;
  }

  void Next()
  {
    myIterator.Next();
    skipRejected();
  }

  template <typename T = IteratorT>
  [[nodiscard]] auto Current() const -> decltype(std::declval<const T&>().Current())
  {
    return myIterator.Current();
  }

  template <typename T = IteratorT>
  [[nodiscard]] auto CurrentId() const -> decltype(std::declval<const T&>().CurrentId())
  {
    return myIterator.CurrentId();
  }

  template <typename T = IteratorT>
  [[nodiscard]] auto CurrentRefId() const -> decltype(std::declval<const T&>().CurrentRefId())
  {
    return myIterator.CurrentRefId();
  }

  template <typename T = IteratorT>
  [[nodiscard]] auto Index() const -> decltype(std::declval<const T&>().Index())
  {
    return myIterator.Index();
  }

  NCollection_ForwardRangeIterator<BRepGraph_FilteredIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_FilteredIterator>(this);
  }

  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void skipRejected()
  {
    while (myIterator.More() && !myPredicate(myIterator))
    {
      myIterator.Next();
    }
  }

  IteratorT  myIterator;
  PredicateT myPredicate;
};

template <typename IteratorT, typename PredicateT>
[[nodiscard]] BRepGraph_FilteredIterator<std::decay_t<IteratorT>, std::decay_t<PredicateT>>
  BRepGraph_MakeFilteredIterator(IteratorT&& theIterator, PredicateT&& thePredicate)
{
  return BRepGraph_FilteredIterator<std::decay_t<IteratorT>, std::decay_t<PredicateT>>(
    std::forward<IteratorT>(theIterator),
    std::forward<PredicateT>(thePredicate));
}

#endif // _BRepGraph_FilteredIterator_HeaderFile
