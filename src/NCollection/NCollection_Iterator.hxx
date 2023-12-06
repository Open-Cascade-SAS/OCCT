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

#ifndef NCollection_Iterator_HeaderFile
#define NCollection_Iterator_HeaderFile

#include <Standard_Assert.hxx>
#include <iterator>

//! Helper class that allows to use NCollection iterators as STL iterators.
//! NCollection iterator can be extended to STL iterator of any category by
//! adding necessary methods: STL forward iterator requires IsEqual method,
//! STL bidirectional iterator requires Previous method, and STL random access
//! iterator requires Offset and Differ methods. See NCollection_Vector as
//! example of declaring custom STL iterators.
template<class Container>
class NCollection_Iterator
{
public:

  NCollection_Iterator() : myCur(typename Container::iterator()), myLast(typename Container::iterator()) {}

  NCollection_Iterator(const NCollection_Iterator& theOther) : myCur(theOther.myCur), myLast(theOther.myLast) {}

  NCollection_Iterator(const Container& theList) : myCur(const_cast<Container&>(theList).begin()), myLast(const_cast<Container&>(theList).end()) {}

  NCollection_Iterator(const Container& theList, const typename Container::iterator& theOther)
    : myCur(theOther), myLast(const_cast<Container&>(theList).end()) {}

  NCollection_Iterator(const Container& theList, typename Container::iterator&& theOther)
    : myCur(theOther), myLast(const_cast<Container&>(theList).end())
  {}

  ~NCollection_Iterator() {}

  void Init(Container& theList)
  {
    myCur = theList.begin();
    myLast = theList.end();
  }

  void Init(const Container& theList)
  {
    Init(const_cast<Container&>(theList));
  }

  virtual bool More() const
  {
    return myCur != myLast;
  }

  void Initialize(Container& theList)
  {
    Init(theList);
  }

  void Initialize(const Container& theList)
  {
    Init(theList);
  }

  const typename Container::iterator& ValueIter() const
  {
    return myCur;
  }

  typename Container::iterator& ChangeValueIter()
  {
    return myCur;
  }

  const typename Container::iterator& EndIter() const
  {
    return myLast;
  }

  typename Container::iterator& ChangeEndIter()
  {
    return myLast;
  }

  virtual void Next()
  {
    ++(myCur);
  }

  const typename Container::const_reference Value() const
  {
    return *myCur;
  }

  const typename Container::reference ChangeValue()
  {
    return *myCur;
  }

  bool operator==(const NCollection_Iterator& theOther) { return myLast == theOther.myLast && myCur == theOther.myCur; }

  bool operator!=(const NCollection_Iterator& theOther) { return myLast != theOther.myLast || myCur != theOther.myCur; }

  NCollection_Iterator& operator=(const NCollection_Iterator& theOther)
  {
    if (this != &theOther)
    {
      myLast = theOther.myLast;
      myCur = theOther.myCur;
    }
    return *this;
  }

  NCollection_Iterator& operator=(NCollection_Iterator&& theOther)
  {
    if (this != &theOther)
    {
      myLast = std::move(theOther.myLast);
      myCur = std::move(theOther.myCur);
    }
    return *this;
  }

private:
  typename Container::iterator myCur;
  typename Container::iterator myLast;
};


#endif // NCollection_Iterator_HeaderFile
