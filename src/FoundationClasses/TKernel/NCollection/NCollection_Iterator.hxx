// Copyright (c) 2023-2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.

#ifndef NCollection_Iterator_HeaderFile
#define NCollection_Iterator_HeaderFile

#include <iterator>
#include <type_traits>
#include <utility>

//! OCCT cursor over a container's native iterator pair.
//!
//! The cursor is deliberately const-qualified at the type level.  It is not a
//! wrapper around another OCCT cursor and never obtains mutable storage from a
//! const container.  The default specialization is the mutable cursor kept for
//! source code using the traditional `Container::Iterator` name.
template <class Container, bool IsConstant = false>
class NCollection_Iterator
{
public:
  using owner_type    = typename std::conditional<IsConstant, const Container, Container>::type;
  using iterator_type = typename std::
    conditional<IsConstant, typename Container::const_iterator, typename Container::iterator>::type;

  NCollection_Iterator()                                           = default;
  NCollection_Iterator(const NCollection_Iterator&)                = default;
  NCollection_Iterator(NCollection_Iterator&&) noexcept            = default;
  NCollection_Iterator& operator=(const NCollection_Iterator&)     = default;
  NCollection_Iterator& operator=(NCollection_Iterator&&) noexcept = default;
  ~NCollection_Iterator() noexcept                                 = default;

  template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
  NCollection_Iterator(const NCollection_Iterator<Container, false>& theOther)
      : myCur(theOther.myCur),
        myLast(theOther.myLast)
  {
  }

  template <bool B = IsConstant, typename std::enable_if<B, int>::type = 0>
  NCollection_Iterator& operator=(const NCollection_Iterator<Container, false>& theOther)
  {
    myCur  = theOther.myCur;
    myLast = theOther.myLast;
    return *this;
  }

  explicit NCollection_Iterator(owner_type& theContainer)
      : myCur(theContainer.begin()),
        myLast(theContainer.end())
  {
  }

  //! Legacy cursor construction from a const container.
  //! The old OCCT cursor API permitted this; const_iterator remains the
  //! const-correct STL-style interface.
  template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
  explicit NCollection_Iterator(const Container& theContainer)
      : myCur(const_cast<Container&>(theContainer).begin()),
        myLast(const_cast<Container&>(theContainer).end())
  {
  }

  NCollection_Iterator(owner_type& theContainer, const iterator_type& thePosition)
      : myCur(thePosition),
        myLast(theContainer.end())
  {
  }

  NCollection_Iterator(owner_type& theContainer, iterator_type&& thePosition)
      : myCur(std::move(thePosition)),
        myLast(theContainer.end())
  {
  }

  void Init(owner_type& theContainer)
  {
    myCur  = theContainer.begin();
    myLast = theContainer.end();
  }

  void Initialize(owner_type& theContainer) { Init(theContainer); }

  template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
  void Init(const Container& theContainer)
  {
    myCur  = const_cast<Container&>(theContainer).begin();
    myLast = const_cast<Container&>(theContainer).end();
  }

  template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
  void Initialize(const Container& theContainer)
  {
    Init(theContainer);
  }

  bool More() const noexcept { return myCur != myLast; }

  void Next() noexcept { ++myCur; }

  const iterator_type& ValueIter() const noexcept { return myCur; }

  iterator_type& ChangeValueIter() noexcept { return myCur; }

  const iterator_type& EndIter() const noexcept { return myLast; }

  iterator_type& ChangeEndIter() noexcept { return myLast; }

  typename Container::const_reference Value() const { return *myCur; }

  template <bool B = IsConstant, typename std::enable_if<!B, int>::type = 0>
  typename Container::reference ChangeValue()
  {
    return *myCur;
  }

  template <bool theOtherIsConstant>
  bool operator==(const NCollection_Iterator<Container, theOtherIsConstant>& theOther) const
  {
    return myLast == theOther.myLast && myCur == theOther.myCur;
  }

  template <bool theOtherIsConstant>
  bool operator!=(const NCollection_Iterator<Container, theOtherIsConstant>& theOther) const
  {
    return !(*this == theOther);
  }

private:
  template <class, bool>
  friend class NCollection_Iterator;

  iterator_type myCur{};
  iterator_type myLast{};
};

#endif // NCollection_Iterator_HeaderFile
