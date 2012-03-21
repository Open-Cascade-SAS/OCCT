// Created on: 2005-12-09
// Created by: Alexander GRIGORIEV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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



#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

class TColStd_intMapNode;

extern Standard_Integer TColStd_intMapNode_findNext (const TColStd_intMapNode *,
                                                     unsigned int& );

//=======================================================================
//function : TColStd_MapIteratorOfPackedMapOfInteger
//purpose  : Constructor
//=======================================================================

TColStd_MapIteratorOfPackedMapOfInteger::TColStd_MapIteratorOfPackedMapOfInteger
                                     (const TColStd_PackedMapOfInteger& theMap)
  : TCollection_BasicMapIterator (theMap),
    myIntMask                    (~0)
{
  if (myNode) {
    const TColStd_intMapNode * aNode =
      reinterpret_cast <const TColStd_intMapNode *>(myNode);
    myKey = TColStd_intMapNode_findNext (aNode, myIntMask);
  }
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TColStd_MapIteratorOfPackedMapOfInteger::Initialize
                                     (const TColStd_PackedMapOfInteger& theMap)
{
  TCollection_BasicMapIterator::Initialize (theMap);
  myIntMask = ~0;
  if (myNode) {
    const TColStd_intMapNode * aNode =
      reinterpret_cast <const TColStd_intMapNode *>(myNode);
    myKey = TColStd_intMapNode_findNext (aNode, myIntMask);
  }
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void TColStd_MapIteratorOfPackedMapOfInteger::Reset ()
{
  TCollection_BasicMapIterator::Reset();
  myIntMask = ~0;
  if (myNode) {
    const TColStd_intMapNode * aNode =
      reinterpret_cast <const TColStd_intMapNode *>(myNode);
    myKey = TColStd_intMapNode_findNext (aNode, myIntMask);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TColStd_MapIteratorOfPackedMapOfInteger::Next ()
{
  while (myNode) {
    const TColStd_intMapNode * aNode =
      reinterpret_cast <const TColStd_intMapNode *>(myNode);
    myKey = TColStd_intMapNode_findNext (aNode, myIntMask);
    if (myIntMask != ~0u)
      break;
    TCollection_BasicMapIterator::Next();
  }
}
