// File:      TColStd_MapIteratorOfPackedMapOfInteger.cxx
// Created:   09.12.05 10:01:49
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2005


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
