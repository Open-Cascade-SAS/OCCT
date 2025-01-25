// Created on: 2002-04-23
// Created by: Alexander KARTOMIN
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef NCollection_TListIterator_HeaderFile
#define NCollection_TListIterator_HeaderFile

#include <NCollection_BaseList.hxx>
#include <NCollection_TListNode.hxx>

/**
 * Purpose:     This Iterator class iterates on BaseList of TListNode and is
 *              instantiated in List/Set/Queue/Stack
 * Remark:      TListIterator is internal class
 */
template <class TheItemType>
class NCollection_TListIterator : public NCollection_BaseList::Iterator
{
public:
  //! Empty constructor - for later Init
  NCollection_TListIterator(void)
      : NCollection_BaseList::Iterator()
  {
  }

  //! Constructor with initialisation
  NCollection_TListIterator(const NCollection_BaseList& theList)
      : NCollection_BaseList::Iterator(theList)
  {
  }

  //! Check end
  Standard_Boolean More(void) const { return (myCurrent != NULL); }

  //! Make step
  void Next(void)
  {
    myPrevious = myCurrent;
    myCurrent  = myCurrent->Next();
  }

  //! Constant Value access
  const TheItemType& Value(void) const
  {
    return ((const NCollection_TListNode<TheItemType>*)myCurrent)->Value();
  }

  //! Non-const Value access
  TheItemType& Value(void)
  {
    return ((NCollection_TListNode<TheItemType>*)myCurrent)->ChangeValue();
  }

  //! Non-const Value access
  TheItemType& ChangeValue(void) const
  {
    return ((NCollection_TListNode<TheItemType>*)myCurrent)->ChangeValue();
  }
};

#endif
