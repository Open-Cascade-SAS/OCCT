// Created on: 2002-04-17
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


#ifndef NCollection_ListNode_HeaderFile
#define NCollection_ListNode_HeaderFile

#include <NCollection_BaseAllocator.hxx>

/**
 * Purpose:     This class is used to  represent a node  in the BaseList and
 *              BaseMap. 
 */              
class NCollection_ListNode
{
 public:
  //! The only constructor
  NCollection_ListNode (NCollection_ListNode* theNext)
  { myNext = theNext; }

  //! Next pointer access
  NCollection_ListNode*& Next (void)
  { return myNext; }

  //! Next pointer const access
  NCollection_ListNode* Next (void) const
  { return myNext; }

 private:
  //! operator= - forbidden
  NCollection_ListNode& operator=(const NCollection_ListNode& )
  {return *this;}
  //! copy constructor - forbidden
  NCollection_ListNode (const NCollection_ListNode& ) {}

 private:
  NCollection_ListNode * myNext; //!< Pointer to the next node
};

#endif
