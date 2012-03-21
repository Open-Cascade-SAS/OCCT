// Created on: 2002-04-23
// Created by: Alexander KARTOMIN (akm)
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


#ifndef NCollection_TListNode_HeaderFile
#define NCollection_TListNode_HeaderFile

#include <NCollection_ListNode.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

/**
 * Purpose:     Abstract list node class. Used by BaseList
 * Remark:      Internal class
 */              
template <class TheItemType> class NCollection_TListNode 
  : public NCollection_ListNode
{
 public:
  //! Constructor
  NCollection_TListNode (const TheItemType& theItem,
                         NCollection_ListNode* theNext=NULL) :
    NCollection_ListNode  (theNext) { myValue = theItem; }
  //! Constant value access
  const TheItemType& Value () const { return myValue; }
  //! Variable value access
  TheItemType& ChangeValue () { return myValue; }
  //! Memory allocation
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC
  //! Static deleter to be passed to BaseList
  static void delNode (NCollection_ListNode * theNode, 
                       Handle(NCollection_BaseAllocator)& theAl)
  {
    ((NCollection_TListNode *) theNode)->myValue.~TheItemType();
    theAl->Free(theNode);
  }

  
 protected:
  TheItemType    myValue; //!< The item stored in the node
  
};

#endif
