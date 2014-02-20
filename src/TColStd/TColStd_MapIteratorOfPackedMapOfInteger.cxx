// Created on: 2005-12-09
// Created by: Alexander GRIGORIEV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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
    myIntMask                    (~0U)
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
  myIntMask = ~0U;
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
  myIntMask = ~0U;
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
