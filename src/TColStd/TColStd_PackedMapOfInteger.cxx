// Created on: 2005-11-05
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

#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TCollection_MapNode.hxx>
#include <Standard_Type.hxx>

// 5 lower bits
#define MASK_LOW  0x001f
// 27 upper bits
#define MASK_HIGH (~MASK_LOW)

/**
 * Class implementing a block of 32 consecutive integer values as a node of
 * a Map collection. The data are stored in 64 bits as:
 * - bits  0 - 4 : (number of integers stored in the block) - 1;
 * - bits  5 - 31: base address of the block of integers (low bits assumed 0)
 * - bits 32 - 63: 32-bit field where each bit indicates the presence of the
 *                 corresponding integer in the block. Number of non-zero bits
 *                 must be equal to the number expressed in bits 0-4.
 */
class TColStd_intMapNode : public TCollection_MapNode
{
public:
  inline TColStd_intMapNode (TCollection_MapNode * ptr = 0L)
    : TCollection_MapNode       (ptr),
      myMask                    (0),
      myData                    (0) {}

  inline TColStd_intMapNode (const Standard_Integer theValue,
                             TCollection_MapNode *& ptr)
    : TCollection_MapNode (ptr),
      myMask              ((unsigned int) (theValue & MASK_HIGH)),
      myData              (1 << (theValue & MASK_LOW)) {}

  inline TColStd_intMapNode (unsigned int        theMask,
                             unsigned int        theData,
                             TCollection_MapNode * ptr)
    : TCollection_MapNode (ptr),
      myMask              (theMask),
      myData              (theData) {}

  inline unsigned int           Mask        () const
  { return myMask; }

  inline unsigned int           Data        () const
  { return myData; }

  inline unsigned int&          ChangeMask  ()
  { return myMask; }

  inline unsigned int&          ChangeData  ()
  { return myData; }

  inline Standard_Integer       Key         () const
  { return Standard_Integer (myMask & MASK_HIGH); }

  inline size_t                 NbValues    () const
  { return size_t(myMask & MASK_LOW) + 1; }

  inline Standard_Boolean       HasValues   () const
  { return (myData != 0); }

  Standard_Integer              HasValue (const Standard_Integer theValue) const
  { return (myData & (1 << (theValue & MASK_LOW))); }

  Standard_Boolean              AddValue (const Standard_Integer theValue);

  Standard_Boolean              DelValue (const Standard_Integer theValue);

  /**
   * Find the smallest non-zero bit under the given mask. Outputs the new mask
   * that does not contain the detected bit.
   */
  Standard_Integer              FindNext (unsigned int& theMask) const;

  /**
   * Support of Map interface.
   */
  inline Standard_Integer       HashCode (const Standard_Integer theUpper) const
  {
    return ::HashCode (Standard_Integer(myMask >> 5), theUpper);
  }

  /**
   * Support of Map interface.
   */
  inline Standard_Boolean       IsEqual  (const Standard_Integer theOther) const
  {
    return ((myMask >> 5) == (unsigned)theOther);
  }

  /**
   * Local friend function, used in TColStd_MapIteratorOfPackedMapOfInteger.
   */
  friend Standard_Integer TColStd_intMapNode_findNext(const TColStd_intMapNode*,
                                                      unsigned int& );

  /**
   * Local friend function.
   */
  friend Standard_Integer TColStd_intMapNode_findPrev(const TColStd_intMapNode*,
                                                      unsigned int& );

private:
  unsigned int      myMask;
  unsigned int      myData;
};


//=======================================================================
//function : TColStd_intMapNode::AddValue
//purpose  : 
//=======================================================================

Standard_Boolean TColStd_intMapNode::AddValue (const Standard_Integer theValue)
{
  Standard_Boolean aResult (Standard_False);
  const Standard_Integer aValInt = (1 << (theValue & MASK_LOW));
  if ((myData & aValInt) == 0) {
    myData ^= aValInt;
    myMask++;
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : TColStd_intMapNode::DelValue
//purpose  : 
//=======================================================================

Standard_Boolean TColStd_intMapNode::DelValue (const Standard_Integer theValue)
{
  Standard_Boolean aResult (Standard_False);
  const Standard_Integer aValInt = (1 << (theValue & MASK_LOW));
  if ((myData & aValInt) != 0) {
    myData ^= aValInt;
    myMask--;
    aResult = Standard_True;
  }
  return aResult;
}

//=======================================================================
//function : TColStd_Population
//purpose  : Compute the population (i.e., the number of non-zero bits) of
//           the 32-bit word theData. The population is stored decremented
//           as it is defined in TColStd_intMapNode.
//source   : H.S.Warren, Hacker''s Delight, Addison-Wesley Inc. 2002, Ch.5.1
//=======================================================================

inline size_t TColStd_Population (unsigned int&      theMask,
                                  const unsigned int theData)
{
  unsigned int aRes = theData - ((theData>>1) & 0x55555555);
  aRes  = (aRes & 0x33333333) + ((aRes>>2)    & 0x33333333);
  aRes  = (aRes + (aRes>>4)) & 0x0f0f0f0f;
  aRes = aRes + (aRes>>8);
  aRes = aRes + (aRes>>16);
  theMask = (theMask & MASK_HIGH) | ((aRes-1) & MASK_LOW);
  return (size_t) (aRes & 0x3f);
}

//=======================================================================
//function : TColStd_intMapNode_findNext
//purpose  : Find the smallest non-zero bit under the given mask. Outputs
//           the new mask that does not contain the detected bit.
//=======================================================================

Standard_Integer TColStd_intMapNode_findNext (const TColStd_intMapNode* theNode,
                                              unsigned int&             theMask)
{
  unsigned int val = theNode->myData & theMask;
  int nZeros (0);
  if (val == 0)
    theMask = ~0U;   // void, nothing to do
  else{
    unsigned int aMask = ~0U;
    if ((val & 0x0000ffff) == 0) {
      aMask = 0xffff0000;
      nZeros = 16;
      val >>= 16;
    }
    if ((val & 0x000000ff) == 0) {
      aMask <<= 8;
      nZeros += 8;
      val >>= 8;
    }
    if ((val & 0x0000000f) == 0) {
      aMask <<= 4;
      nZeros += 4;
      val >>= 4;
    }
    if ((val & 0x00000003) == 0) {
      aMask <<= 2;
      nZeros += 2;
      val >>= 2;
    }
    if ((val & 0x00000001) == 0) {
      aMask <<= 1;
      nZeros ++;
    }
    theMask = (aMask << 1);
  }
  return nZeros + theNode->Key();
}

//=======================================================================
//function : TColStd_intMapNode_findPrev
//purpose  : Find the highest non-zero bit under the given mask. Outputs
//           the new mask that does not contain the detected bit.
//=======================================================================

Standard_Integer TColStd_intMapNode_findPrev (const TColStd_intMapNode* theNode,
                                              unsigned int&             theMask)
{
  unsigned int val = theNode->myData & theMask;
  int nZeros (0);
  if (val == 0)
    theMask = ~0U;   // void, nothing to do
  else {
    unsigned int aMask = ~0U;
    if ((val & 0xffff0000) == 0) {
      aMask = 0x0000ffff;
      nZeros = 16;
      val <<= 16;
    }
    if ((val & 0xff000000) == 0) {
      aMask >>= 8;
      nZeros += 8;
      val <<= 8;
    }
    if ((val & 0xf0000000) == 0) {
      aMask >>= 4;
      nZeros += 4;
      val <<= 4;
    }
    if ((val & 0xc0000000) == 0) {
      aMask >>= 2;
      nZeros += 2;
      val <<= 2;
    }
    if ((val & 0x80000000) == 0) {
      aMask >>= 1;
      nZeros ++;
    }
    theMask = (aMask >> 1);
  }
  return (31 - nZeros) + theNode->Key();
}

//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================

TColStd_PackedMapOfInteger& TColStd_PackedMapOfInteger::Assign
                                  (const TColStd_PackedMapOfInteger& theOther)
{
  if (this != &theOther) {
    Clear();
    if  (!theOther.IsEmpty()) { 
      ReSize (theOther.InternalExtent());
      const Standard_Integer nBucketsSrc = theOther.NbBuckets();
      const Standard_Integer nBuckets    = NbBuckets();
      const TColStd_intMapNode** aDataSrc =
        (const TColStd_intMapNode**) theOther.myData1;
      TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
      for (Standard_Integer i = 0; i <= nBucketsSrc; i++) {
        const TColStd_intMapNode * p = aDataSrc[i];
        while (p) {
          const Standard_Integer aHashCode = p->HashCode(nBuckets);
          aData[aHashCode] =
            new TColStd_intMapNode (p->Mask(), p->Data(), aData[aHashCode]);
          Increment();
          p = reinterpret_cast <const TColStd_intMapNode*> (p->Next());
        }
      }
//       TColStd_MapIteratorOfPackedMapOfInteger anIt (theOther);
//       for (; anIt.More(); anIt.Next())
//         Add (anIt.Key());
    }
  }
  myExtent  = theOther.myExtent;
  return * this;
}

//=======================================================================
//function : ReSize
//purpose  : 
//=======================================================================

void TColStd_PackedMapOfInteger::ReSize (const Standard_Integer nbBuckets)
{
  Standard_Integer newBuck;
  Standard_Address newData1=NULL, dummy=NULL;
  if (BeginResize (nbBuckets, newBuck, newData1, dummy))
  {
    if (myData1) {
      TColStd_intMapNode** newdata = 
        reinterpret_cast <TColStd_intMapNode**> (newData1);
      TColStd_intMapNode** olddata =
        reinterpret_cast <TColStd_intMapNode**> (myData1);
      TColStd_intMapNode * p;
      Standard_Integer i,k;
      for (i = 0; i <= NbBuckets(); i++) {
	if (olddata[i]) {
	  p = olddata[i];
	  while (p) {
	    k = p->HashCode(newBuck);
            TCollection_MapNode * q = p->Next();
	    p->Next() = newdata[k];
	    newdata[k] = p;
	    p = static_cast <TColStd_intMapNode*> (q);
	  }
	}
      }
    }
    EndResize (nbBuckets, newBuck, newData1, dummy);
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TColStd_PackedMapOfInteger::Clear ()
{
  if (!IsEmpty()) {
    Standard_Integer i;
    TColStd_intMapNode** data =
      reinterpret_cast <TColStd_intMapNode**> (myData1);
    TColStd_intMapNode * p;
    for (i = 0; i <= NbBuckets(); i++) {
      if (data[i]) {
	p = data[i];
	while (p) {
          TCollection_MapNode * q = p->Next();
	  delete p;
          p = static_cast <TColStd_intMapNode*> (q);
	}
      }
    }
  }
  TCollection_BasicMap::Destroy();
  myExtent = 0;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::Add (const Standard_Integer aKey)
{
  if (Resizable())
    ReSize(InternalExtent());
  Standard_Boolean aResult (Standard_False);
  TColStd_intMapNode  ** data =
    reinterpret_cast <TColStd_intMapNode**> (myData1);
  const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
  const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
  TCollection_MapNodePtr aBucketHead = data[aHashCode];
  TColStd_intMapNode   * p = static_cast<TColStd_intMapNode*> (aBucketHead);
  while (p) {
    if (p->IsEqual(aKeyInt)) {
      aResult = p->AddValue (aKey);
//       break;
      goto finish;  // goto saves us 4 CPU clocks or 4% performance
    }
    p = reinterpret_cast <TColStd_intMapNode*> (p->Next());
  }
//    if (!p) {         // not needed, as long as we exit the loop by goto
    data[aHashCode] = new TColStd_intMapNode(aKey, aBucketHead);
    Increment();
    aResult = Standard_True;
//    }
 finish:
  if (aResult)
    myExtent++;
  return aResult;
}

//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::Contains
                                        (const Standard_Integer aKey) const
{
  Standard_Boolean aResult (Standard_False);
  if (!IsEmpty()) {
    TColStd_intMapNode** data = (TColStd_intMapNode**) myData1;
    const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
    TColStd_intMapNode * p = data[HashCode (aKeyInt, NbBuckets())];
    while (p) {
      if (p->IsEqual(aKeyInt)) {
        aResult = (p->HasValue (aKey) != 0);
        break;
      }
      p = reinterpret_cast <TColStd_intMapNode*> (p->Next());
    }
  }
  return aResult;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::Remove(const Standard_Integer aKey)
{
  Standard_Boolean aResult (Standard_False);
  if (!IsEmpty()) {
    TColStd_intMapNode**   data =
      reinterpret_cast <TColStd_intMapNode**> (myData1);
    const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
    TColStd_intMapNode*&   aBucketHead = data[HashCode(aKeyInt, NbBuckets())];
    TColStd_intMapNode*    p = aBucketHead;
    TColStd_intMapNode*    q = 0L;
    while (p) {
      if (p->IsEqual(aKeyInt)) {
        aResult = p->DelValue (aKey);
        if (aResult) {
          myExtent--;
          if (p->HasValues() == Standard_False) {
            Decrement();
            if (q) q->Next() = p->Next();
            else aBucketHead = reinterpret_cast<TColStd_intMapNode*>(p->Next());
            delete p;
          }
        }
        break;
      }
      q = p;
      p = reinterpret_cast <TColStd_intMapNode*> (p->Next());
    }
  }
  return aResult;
}

//=======================================================================
//function : GetMinimalMapped
//purpose  : Query the minimal contained key value.
//=======================================================================

Standard_Integer TColStd_PackedMapOfInteger::GetMinimalMapped () const
{
  Standard_Integer aResult (IntegerLast());
  if (!IsEmpty()) {
    const TCollection_MapNode** aData = (const TCollection_MapNode**) myData1;
    const TColStd_intMapNode * pFoundNode = 0L;
    for (Standard_Integer i = 0; i <= NbBuckets(); i++) {
      for (const TCollection_MapNode * p = aData[i]; p != 0L; p = p->Next()) {
        const Standard_Integer aKey =
          reinterpret_cast <const TColStd_intMapNode *>(p)->Key();
        if (aResult > aKey) {
          aResult = aKey;
          pFoundNode = reinterpret_cast<const TColStd_intMapNode *>(p);
        }
      }
    }
    if (pFoundNode) {
      unsigned int aFullMask (0xffffffff);
      aResult = TColStd_intMapNode_findNext (pFoundNode, aFullMask);
    }
  }
  return aResult;
}

//=======================================================================
//function : GetMaximalMapped
//purpose  : Query the maximal contained key value.
//=======================================================================

Standard_Integer TColStd_PackedMapOfInteger::GetMaximalMapped () const
{
  Standard_Integer aResult (IntegerFirst());
  if (!IsEmpty()) {
    const TCollection_MapNode** aData = (const TCollection_MapNode**) myData1;
    const TColStd_intMapNode * pFoundNode = 0L;
    for (Standard_Integer i = 0; i <= NbBuckets(); i++) {
      for (const TCollection_MapNode * p = aData[i]; p != 0L; p = p->Next()) {
        const Standard_Integer aKey =
          reinterpret_cast <const TColStd_intMapNode *>(p)->Key();
        if (aResult < aKey) {
          aResult = aKey;
          pFoundNode = reinterpret_cast<const TColStd_intMapNode *>(p);
        }
      }
    }
    if (pFoundNode) {
      unsigned int aFullMask (0xffffffff);
      aResult = TColStd_intMapNode_findPrev (pFoundNode, aFullMask);
    }
  }
  return aResult;
}

//=======================================================================
//function : Union
//purpose  : Boolean operation OR between 2 maps
//=======================================================================

void TColStd_PackedMapOfInteger::Union (const TColStd_PackedMapOfInteger& theMap1,
                                        const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty()) // 0 | B == B
    Assign (theMap2);
  else if (theMap2.IsEmpty()) // A | 0 == A
    Assign (theMap1);
  else if (myData1 == theMap1.myData1)
    Unite (theMap2);
  else if (myData1 == theMap2.myData1)
    Unite (theMap1);
  else {
    Standard_Integer i;
    const TColStd_intMapNode** aData1 =
      (const TColStd_intMapNode**) theMap1.myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap2.myData1;
    const Standard_Integer nBuckets1 = theMap1.NbBuckets();
    const Standard_Integer nBuckets2 = theMap2.NbBuckets();
    Clear();
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
    // Iteration of the 1st map.
    for (i = 0; i <= nBuckets1; i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        unsigned int aNewMask = p1->Mask();
        unsigned int aNewData = p1->Data();
        size_t       nValues (p1->NbValues());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            aNewData |= p2->Data();
            nValues = TColStd_Population (aNewMask, aNewData);
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        // Store the block - result of operation
        if (Resizable()) {
          ReSize(InternalExtent());
          aData = (TColStd_intMapNode**) myData1;
        }
        const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
        aData[aHashCode] = new TColStd_intMapNode (aNewMask, aNewData,
                                                   aData[aHashCode]);
        Increment();
        myExtent += nValues;
        p1 = reinterpret_cast <const TColStd_intMapNode*> (p1->Next());
      }
    }
    // Iteration of the 2nd map.
    for (i = 0; i <= nBuckets2; i++) {
      const TColStd_intMapNode * p2 = aData2[i];
      while (p2 != 0L) {
        // Find aKey - the base address of currently iterated block
        const Standard_Integer aKey = p2->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        // Find the corresponding block in the 1st map
        const TColStd_intMapNode * p1 =
          aData1 [HashCode (aKeyInt, nBuckets1)];
        while (p1) {
          if (p1->IsEqual(aKeyInt))
            break;
          p1 = reinterpret_cast <const TColStd_intMapNode*> (p1->Next());
        }
        // Add the block from the 2nd map only in the case when the similar
        // block has not been found in the 1st map
        if (p1 == 0L) {
          if (Resizable()) {
            ReSize(InternalExtent());
            aData = (TColStd_intMapNode**) myData1;
          }
          const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
          aData[aHashCode]= new TColStd_intMapNode (p2->Mask(), p2->Data(),
                                                    aData[aHashCode]);
          Increment();
          myExtent += p2->NbValues();
        }
        p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
      }
    }
  }
}

//=======================================================================
//function : Unite
//purpose  : Boolean operation OR with the given map
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::Unite(const TColStd_PackedMapOfInteger& theMap)
{
  if (theMap.IsEmpty() || myData1 == theMap.myData1) // A | 0 == A | A == A
    return Standard_False;
  else if ( IsEmpty() ) { // 0 | B == B
    Assign ( theMap );
    return Standard_True;
  }
  else {
    size_t aNewExtent (myExtent);
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap.myData1;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();

    // Iteration of the 2nd map.
    for (Standard_Integer i = 0; i <= nBuckets2; i++) {
      const TColStd_intMapNode * p2 = aData2[i];
      while (p2 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p2->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        // Find the corresponding block in the 1st (this) map
        Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
        TColStd_intMapNode * p1 = aData[aHashCode];
        while (p1) {
          if (p1->IsEqual(aKeyInt)) {
            const size_t anOldPop = p1->NbValues();
            unsigned int newData = p1->Data() | p2->Data();
            if ( newData != p1->Data() ) {
              p1->ChangeData() = newData;
              aNewExtent = aNewExtent - anOldPop +
                           TColStd_Population (p1->ChangeMask(), newData);
            }
            break;
          }
          p1 = reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        }
        // If the block is not found in the 1st map, add it to the 1st map
        if (p1 == 0L) {
          if (Resizable()) {
            ReSize(InternalExtent());
            aData = (TColStd_intMapNode**) myData1;
            aHashCode = HashCode (aKeyInt, NbBuckets());
          }
          aData[aHashCode] = new TColStd_intMapNode (p2->Mask(), p2->Data(),
                                                     aData[aHashCode]);
          Increment();
          aNewExtent += p2->NbValues();
        }
        p2 = reinterpret_cast <TColStd_intMapNode*> (p2->Next());
      }
    }
    Standard_Boolean isChanged = ( myExtent != aNewExtent );
    myExtent = aNewExtent;
    return isChanged;
  }
}

//=======================================================================
//function : Intersection
//purpose  : Boolean operation AND between 2 maps
//=======================================================================

void TColStd_PackedMapOfInteger::Intersection
                                (const TColStd_PackedMapOfInteger& theMap1,
                                 const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty() || theMap2.IsEmpty()) // A & 0 == 0 & B == 0
    Clear();
  else if (myData1 == theMap1.myData1)
    Intersect (theMap2);
  else if (myData1 == theMap2.myData1)
    Intersect (theMap1);
  else {
    const TColStd_intMapNode** aData1, ** aData2;
    Standard_Integer nBuckets1, nBuckets2;
    if (theMap1.Extent() < theMap2.Extent()) {
      aData1 = (const TColStd_intMapNode**) theMap1.myData1;
      aData2 = (const TColStd_intMapNode**) theMap2.myData1;
      nBuckets1 = theMap1.NbBuckets();
      nBuckets2 = theMap2.NbBuckets();
    } 
    else {
      aData1 = (const TColStd_intMapNode**) theMap2.myData1;
      aData2 = (const TColStd_intMapNode**) theMap1.myData1;
      nBuckets1 = theMap2.NbBuckets();
      nBuckets2 = theMap1.NbBuckets();
    }
    Clear();
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
    // Iteration of the 1st map.
    for (Standard_Integer i = 0; i <= nBuckets1; i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            const unsigned int aNewData = p1->Data() & p2->Data();
            // Store the block - result of operation
            if (aNewData) {
              if (Resizable()) {
                ReSize(InternalExtent());
                aData = (TColStd_intMapNode**) myData1;
              }
              const Standard_Integer aHashCode = HashCode (aKeyInt,
                                                           NbBuckets());
              unsigned int aNewMask = p1->Mask();
              myExtent += TColStd_Population (aNewMask, aNewData);
              aData[aHashCode]= new TColStd_intMapNode(aNewMask, aNewData,
                                                       aData[aHashCode]);
              Increment();
            }
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        p1 = reinterpret_cast <const TColStd_intMapNode*> (p1->Next());
      }
    }
  }
}

//=======================================================================
//function : Intersect
//purpose  : Boolean operation AND with the given map
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::Intersect
                 (const TColStd_PackedMapOfInteger& theMap)
{
  if ( IsEmpty() ) // 0 & B == 0
    return Standard_False;
  else if (theMap.IsEmpty()) { // A & 0 == 0
    Clear();
    return Standard_True;
  }
  else if (myData1 == theMap.myData1) // A & A == A
    return Standard_False;
  else {
    size_t aNewExtent (0);
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap.myData1;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();

    // Iteration of this map.
    for (Standard_Integer i = 0; i <= NbBuckets(); i++) {
      TColStd_intMapNode * q  = 0L;
      TColStd_intMapNode * p1 = aData[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            const unsigned int aNewData = p1->Data() & p2->Data();
            // Store the block - result of operation
            if (aNewData == 0)
              p2 = 0L;  // no match - the block has to be removed
            else
            {
              if ( aNewData != p1->Data() )
                p1->ChangeData() = aNewData;
              aNewExtent += TColStd_Population (p1->ChangeMask(), aNewData);
            }
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        TColStd_intMapNode* pNext =
          reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        // If p2!=NULL, then the map node is kept and we move to the next one
        // Otherwise we should remove the current node
        if (p2)
          q = p1;
        else {
          Decrement();
          if (q)  q->Next() = pNext;
          else    aData[i]  = pNext;
          delete p1;
        }
        p1 = pNext;
      }
    }
    Standard_Boolean isChanged = ( myExtent != aNewExtent );
    myExtent = aNewExtent;
    return isChanged;
  }
}

//=======================================================================
//function : Subtraction
//purpose  : Boolean operation SUBTRACT between two maps
//=======================================================================

void TColStd_PackedMapOfInteger::Subtraction
                                (const TColStd_PackedMapOfInteger& theMap1,
                                 const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty() || theMap2.myData1 == theMap1.myData1) // 0 \ A == A \ A == 0
    Clear();
  else if (theMap2.IsEmpty()) // A \ 0 == A
    Assign (theMap1);
  else if (myData1 == theMap1.myData1)
    Subtract (theMap2);
  else if (myData1 == theMap2.myData1) {
    TColStd_PackedMapOfInteger aMap;
    aMap.Subtraction ( theMap1, theMap2 );
    Assign ( aMap );
  }
  else {
    const TColStd_intMapNode** aData1 =
      (const TColStd_intMapNode**) theMap1.myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap2.myData1;
    const Standard_Integer nBuckets1 = theMap1.NbBuckets();
    const Standard_Integer nBuckets2 = theMap2.NbBuckets();
    Clear();
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
    // Iteration of the 1st map.
    for (Standard_Integer i = 0; i <= nBuckets1; i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        unsigned int aNewMask = p1->Mask();
        unsigned int aNewData = p1->Data();
        size_t       nValues (p1->NbValues());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            aNewData &= ~p2->Data();
            nValues = TColStd_Population (aNewMask, aNewData);
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        // Store the block - result of operation
        if (aNewData) {
          if (Resizable()) {
            ReSize(InternalExtent());
            aData = (TColStd_intMapNode**) myData1;
          }
          const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
          aData[aHashCode]= new TColStd_intMapNode (aNewMask, aNewData,
                                                    aData[aHashCode]);
          Increment();
          myExtent += nValues;
        }
        p1 = reinterpret_cast <const TColStd_intMapNode*> (p1->Next());
      }
    }
  }
}

//=======================================================================
//function : Subtract
//purpose  : Boolean operation SUBTRACT with the given map
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::Subtract
                                (const TColStd_PackedMapOfInteger& theMap)
{
  if ( IsEmpty() || theMap.IsEmpty() ) // 0 \ B == 0; A \ 0 == A
    return Standard_False;
  else if (myData1 == theMap.myData1) { // A \ A == 0
    Clear();
    return Standard_True;
  }
  else {
    size_t aNewExtent (0);
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap.myData1;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();
    // Iteration of this map.
    for (Standard_Integer i = 0; i <= NbBuckets(); i++) {
      TColStd_intMapNode * q  = 0L;
      TColStd_intMapNode * p1 = aData[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        TColStd_intMapNode* pNext =
          reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            const unsigned int aNewData = p1->Data() & ~p2->Data();
            // Store the block - result of operation
            if (aNewData == 0) {
              // no match - the block has to be removed
              Decrement();
              if (q)  q->Next() = pNext;
              else    aData[i]  = pNext;
              delete p1;
            } 
            else if ( aNewData != p1->Data() ) {
              p1->ChangeData() = aNewData;
              aNewExtent += TColStd_Population (p1->ChangeMask(), aNewData);
              q = p1;
            }
            else {
              aNewExtent += p1->NbValues();
              q = p1;
            }
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        if (p2 == 0L) {
          aNewExtent += p1->NbValues();
          q = p1;
        }
        p1 = pNext;
      }
    }
    Standard_Boolean isChanged = ( myExtent != aNewExtent );
    myExtent = aNewExtent;
    return isChanged;
  }
}

//=======================================================================
//function : Difference
//purpose  : Boolean operation XOR 
//=======================================================================

void TColStd_PackedMapOfInteger::Difference  (const TColStd_PackedMapOfInteger& theMap1,
                                              const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty()) // 0 ^ B == B
    Assign (theMap2);
  else if (theMap2.IsEmpty()) // A ^ 0 == A
    Assign (theMap1);
  else if (myData1 == theMap1.myData1)
    Differ(theMap2);
  else if (myData1 == theMap2.myData1)
    Differ(theMap1);
  else {
    Standard_Integer i;
    const TColStd_intMapNode** aData1 =
      (const TColStd_intMapNode**) theMap1.myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap2.myData1;
    const Standard_Integer nBuckets1 = theMap1.NbBuckets();
    const Standard_Integer nBuckets2 = theMap2.NbBuckets();
    Clear();
    TColStd_intMapNode** aData = (TColStd_intMapNode**) myData1;

    // Iteration of the 1st map.
    for (i = 0; i <= nBuckets1; i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        unsigned int aNewMask = p1->Mask();
        unsigned int aNewData = p1->Data();
        size_t       nValues (p1->NbValues());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            aNewData ^= p2->Data();
            nValues = TColStd_Population (aNewMask, aNewData);
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        // Store the block - result of operation
        if (aNewData) {
          if (Resizable()) {
            ReSize(InternalExtent());
            aData = (TColStd_intMapNode**) myData1;
          }
          const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
          aData[aHashCode]= new TColStd_intMapNode (aNewMask, aNewData,
                                                    aData[aHashCode]);
          Increment();
          myExtent += nValues;
        }
        p1 = reinterpret_cast <const TColStd_intMapNode*> (p1->Next());
      }
    }
    
    // Iteration of the 2nd map.
    for (i = 0; i <= nBuckets2; i++) {
      const TColStd_intMapNode * p2 = aData2[i];
      while (p2 != 0L) {
        // Find aKey - the base address of currently iterated block
        const Standard_Integer aKey = p2->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        // Find the corresponding block in the 1st map
        const TColStd_intMapNode * p1 =
          aData1 [HashCode (aKeyInt, nBuckets1)];
        while (p1) {
          if (p1->IsEqual(aKeyInt))
            break;
          p1 = reinterpret_cast <const TColStd_intMapNode*> (p1->Next());
        }
        // Add the block from the 2nd map only in the case when the similar
        // block has not been found in the 1st map
        if (p1 == 0L) {
          if (Resizable()) {
            ReSize(InternalExtent());
            aData = (TColStd_intMapNode**) myData1;
          }
          const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
          aData[aHashCode]= new TColStd_intMapNode (p2->Mask(), p2->Data(),
                                                    aData[aHashCode]);
          Increment();
          myExtent += p2->NbValues();
        }
        p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
      }
    }
  }
}

//=======================================================================
//function : Differ
//purpose  : Boolean operation XOR 
//=======================================================================
  
Standard_Boolean TColStd_PackedMapOfInteger::Differ(const TColStd_PackedMapOfInteger& theMap)
{
  if (theMap.IsEmpty()) // A ^ 0 = A
    return Standard_False;    
  else if (IsEmpty()) { // 0 ^ B = B
    Assign ( theMap );
    return Standard_True;
  }
  else if( myData1 == theMap.myData1) { // A ^ A == 0
    Clear();
    return Standard_True;
  }
  else {
    size_t aNewExtent (0);
    TColStd_intMapNode** aData1 = (TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 =
      (const TColStd_intMapNode**) theMap.myData1;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();
    Standard_Boolean isChanged = Standard_False;
    Standard_Integer i = 0;
    // Iteration by other map
    for ( ; i <= nBuckets2; i++) {
       TColStd_intMapNode * q  = 0L;
      const TColStd_intMapNode * p2 = aData2[i];
      while (p2 != 0L) {
        // Find aKey - the base address of currently iterated block
        const Standard_Integer aKey = p2->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        
        // Find the corresponding block in the 1st map
        TColStd_intMapNode * p1 =
          aData1[HashCode (aKeyInt, NbBuckets())];
        TColStd_intMapNode* pNext =
          reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        
        while (p1) {
          if (p1->IsEqual(aKeyInt)) {
            const unsigned int aNewData = p1->Data() ^ p2->Data();
            // Store the block - result of operation
            if (aNewData == 0) {
              // no match - the block has to be removed
              Decrement();
              if (q)  q->Next() = pNext;
              else    aData1[i]  = pNext;
              delete p1;
            } 
            else if ( aNewData != p1->Data() ) {
              p1->ChangeData() = aNewData;
              isChanged = Standard_True;
              aNewExtent += TColStd_Population (p1->ChangeMask(), aNewData);
              q = p1;
            }
            break;
          }
          p1 = pNext;
        }
        // Add the block from the 2nd map only in the case when the similar
        // block has not been found in the 1st map
        TColStd_intMapNode** aData = NULL;
        if (p1 == 0L) {
          if (Resizable()) {
            ReSize(InternalExtent());
            aData = (TColStd_intMapNode**) myData1;
          }
          const Standard_Integer aHashCode = HashCode (aKeyInt, NbBuckets());
          aData[aHashCode]= new TColStd_intMapNode (p2->Mask(), p2->Data(),
                                                    aData[aHashCode]);
          Increment();
          aNewExtent += p2->NbValues();
          isChanged = Standard_True;
        }
        p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
      }
    }
    myExtent = aNewExtent;
    return isChanged;
  }
}

//=======================================================================
//function : IsEqual
//purpose  : Boolean operation returns true if this map is equal to the other map  
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::IsEqual(const TColStd_PackedMapOfInteger& theMap) const
{
  if (IsEmpty() && theMap.IsEmpty())
    return Standard_True;
  else if ( Extent() != theMap.Extent() )
    return Standard_False;
  else {
    const TColStd_intMapNode** aData1 = (const TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 = (const TColStd_intMapNode**) theMap.myData1;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();
    if(aData1 == aData2)
      return Standard_True;
   
    Standard_Integer i = 0;
    // Iteration of this map.
    for (; i <= NbBuckets(); i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        TColStd_intMapNode* pNext =
          reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if ( p2->IsEqual(aKeyInt) ) {
            if ( p1->Data() != p2->Data() )
              return Standard_False;
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        // if the same block not found, maps are different
        if (p2 == 0L) 
          return Standard_False;
        
        p1 = pNext;
      }
    }
    return Standard_True;
  }
}

//=======================================================================
//function : IsSubset
//purpose  : Boolean operation returns true if this map if subset of other map
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::IsSubset (const TColStd_PackedMapOfInteger& theMap) const
{
  if ( IsEmpty() ) // 0 <= A 
    return Standard_True;
  else if ( theMap.IsEmpty() ) // ! ( A <= 0 )
    return Standard_False;
  else if ( Extent() > theMap.Extent() )
    return Standard_False;
  else {
    const TColStd_intMapNode** aData1 = (const TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 = (const TColStd_intMapNode**) theMap.myData1;
    if(aData1 == aData2)
      return Standard_True;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();
        
    Standard_Integer i = 0;
    // Iteration of this map.
    for (; i <= NbBuckets(); i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        TColStd_intMapNode* pNext =
          reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        if (!p2)
          return Standard_False;
        while (p2) {
          if ( p2->IsEqual(aKeyInt) ) {
            if ( p1->Data() & ~p2->Data() ) // at least one bit set in p1 is not set in p2
              return Standard_False;
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        p1 = pNext;
      }
    }
    return Standard_True;
  }
}

//=======================================================================
//function : HasIntersection
//purpose  : Boolean operation returns true if this map intersects with other map
//=======================================================================

Standard_Boolean TColStd_PackedMapOfInteger::HasIntersection (const TColStd_PackedMapOfInteger& theMap) const
{
  if (IsEmpty() || theMap.IsEmpty()) // A * 0 == 0 * B == 0
    return Standard_False;
  else {
    const TColStd_intMapNode** aData1 = (const TColStd_intMapNode**) myData1;
    const TColStd_intMapNode** aData2 = (const TColStd_intMapNode**) theMap.myData1;
    const Standard_Integer nBuckets2 = theMap.NbBuckets();
    if(aData1 == aData2)
      return Standard_True;
        
    Standard_Integer i = 0;
    // Iteration of this map.
    for (; i <= NbBuckets(); i++) {
      const TColStd_intMapNode * p1 = aData1[i];
      while (p1 != 0L) {
        // Find aKey - the base address of currently iterated block of integers
        const Standard_Integer aKey = p1->Key();
        const Standard_Integer aKeyInt = (unsigned)aKey >> 5;
        TColStd_intMapNode* pNext =
          reinterpret_cast <TColStd_intMapNode*> (p1->Next());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode * p2 =
          aData2 [HashCode (aKeyInt, nBuckets2)];
        while (p2) {
          if (p2->IsEqual(aKeyInt)) {
            if ( p1->Data() & p2->Data() )
              return Standard_True;
            break;
          }
          p2 = reinterpret_cast <const TColStd_intMapNode*> (p2->Next());
        }
        p1 = pNext;
      }
    }
    return Standard_False;
  }
}
