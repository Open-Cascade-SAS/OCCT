// Created on: 2002-04-23
// Created by: Alexander KARTOMIN (akm)
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

#ifndef NCollection_TListNode_HeaderFile
#define NCollection_TListNode_HeaderFile

#include <NCollection_ListNode.hxx>
#include <utility>

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
    NCollection_ListNode  (theNext), myValue(theItem) { }
  NCollection_TListNode (TheItemType&& theItem,
                         NCollection_ListNode* theNext=NULL) :
    NCollection_ListNode  (theNext), myValue(std::forward<TheItemType>(theItem)) { }
  //! Constant value access
  const TheItemType& Value () const { return myValue; }
  //! Variable value access
  TheItemType& ChangeValue () { return myValue; }

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
