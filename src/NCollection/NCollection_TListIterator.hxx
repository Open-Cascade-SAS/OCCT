// Created on: 2002-04-23
// Created by: Alexander KARTOMIN
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef NCollection_TListIterator_HeaderFile
#define NCollection_TListIterator_HeaderFile

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseList.hxx>
#include <NCollection_TListNode.hxx>

/**
 * Purpose:     This Iterator class iterates on BaseList of TListNode and is 
 *              instantiated in List/Set/Queue/Stack
 * Remark:      TListIterator is internal class
 */
template <class TheItemType> class NCollection_TListIterator 
  : public NCollection_BaseCollection<TheItemType>::Iterator,
    public NCollection_BaseList::Iterator
{
 public:
  //! Empty constructor - for later Init
  NCollection_TListIterator  (void) :
    NCollection_BaseList::Iterator () {}
  //! Constructor with initialisation
  NCollection_TListIterator  (const NCollection_BaseList& theList) :
    NCollection_BaseList::Iterator (theList) {}
  //! Assignment
  NCollection_TListIterator& operator= (const NCollection_TListIterator& theIt)
  {
    NCollection_BaseList::Iterator& me = * this;
    me.operator= (theIt);
    return * this;
  }
  //! Check end
  virtual Standard_Boolean More (void) const
  { return (myCurrent!=NULL); }
  //! Make step
  virtual void Next (void)
  {
    myPrevious = myCurrent;
    myCurrent = myCurrent->Next();
  }
  //! Constant Value access
  virtual const TheItemType& Value (void) const
  { return ((const NCollection_TListNode<TheItemType>*) myCurrent)->Value(); }
  //! Variable Value access
  virtual TheItemType& ChangeValue (void) const
  { return ((NCollection_TListNode<TheItemType> *)myCurrent)->ChangeValue(); }
};

#endif
