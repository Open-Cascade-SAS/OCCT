// Created on: 2002-04-17
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

#ifndef NCollection_ListNode_HeaderFile
#define NCollection_ListNode_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

/**
 * Purpose:     This class is used to  represent a node  in the BaseList and
 *              BaseMap.
 */
class NCollection_ListNode
{
public:
  // define new operator for use with NCollection allocators
  DEFINE_NCOLLECTION_ALLOC
public:
  //! The only constructor
  NCollection_ListNode(NCollection_ListNode* theNext)
      : myNext(theNext)
  {
  }

  //! Next pointer access
  NCollection_ListNode*& Next(void) { return myNext; }

  //! Next pointer const access
  NCollection_ListNode* Next(void) const { return myNext; }

private:
  //! operator= - forbidden
  NCollection_ListNode& operator=(const NCollection_ListNode&);

  //! copy constructor - forbidden
  NCollection_ListNode(const NCollection_ListNode&);

private:
  NCollection_ListNode* myNext; //!< Pointer to the next node
};

#endif
