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

#include <NCollection_Array1.hxx>
#include <NCollection_Primes.hxx>

namespace
{
size_t HashCode(const int theKey, const size_t theBound)
{
  return static_cast<size_t>(theKey) % theBound + 1;
}
} // namespace

//=================================================================================================

int TColStd_PackedMapOfInteger::TColStd_intMapNode_findNext(const TColStd_intMapNode* theNode,
                                                            unsigned int&             theMask)
{
  const TColStd_intMapNode* aNode = reinterpret_cast<const TColStd_intMapNode*>(theNode);
  unsigned int              val   = aNode->Data() & theMask;
  int                       nZeros(0);
  if (val == 0)
    theMask = ~0U; // void, nothing to do
  else
  {
    unsigned int aMask = ~0U;
    if ((val & 0x0000ffff) == 0)
    {
      aMask  = 0xffff0000;
      nZeros = 16;
      val >>= 16;
    }
    if ((val & 0x000000ff) == 0)
    {
      aMask <<= 8;
      nZeros += 8;
      val >>= 8;
    }
    if ((val & 0x0000000f) == 0)
    {
      aMask <<= 4;
      nZeros += 4;
      val >>= 4;
    }
    if ((val & 0x00000003) == 0)
    {
      aMask <<= 2;
      nZeros += 2;
      val >>= 2;
    }
    if ((val & 0x00000001) == 0)
    {
      aMask <<= 1;
      nZeros++;
    }
    theMask = (aMask << 1);
  }
  return nZeros + aNode->Key();
}

//=================================================================================================

int TColStd_PackedMapOfInteger::TColStd_intMapNode_findPrev(const TColStd_intMapNode* theNode,
                                                            unsigned int&             theMask)
{
  const TColStd_intMapNode* aNode = reinterpret_cast<const TColStd_intMapNode*>(theNode);
  unsigned int              val   = aNode->Data() & theMask;
  int                       nZeros(0);
  if (val == 0)
    theMask = ~0U; // void, nothing to do
  else
  {
    unsigned int aMask = ~0U;
    if ((val & 0xffff0000) == 0)
    {
      aMask  = 0x0000ffff;
      nZeros = 16;
      val <<= 16;
    }
    if ((val & 0xff000000) == 0)
    {
      aMask >>= 8;
      nZeros += 8;
      val <<= 8;
    }
    if ((val & 0xf0000000) == 0)
    {
      aMask >>= 4;
      nZeros += 4;
      val <<= 4;
    }
    if ((val & 0xc0000000) == 0)
    {
      aMask >>= 2;
      nZeros += 2;
      val <<= 2;
    }
    if ((val & 0x80000000) == 0)
    {
      aMask >>= 1;
      nZeros++;
    }
    theMask = (aMask >> 1);
  }
  return (31 - nZeros) + aNode->Key();
}

//=================================================================================================

TColStd_PackedMapOfInteger& TColStd_PackedMapOfInteger::Assign(
  const TColStd_PackedMapOfInteger& theOther)
{
  if (this != &theOther)
  {
    Clear();
    if (!theOther.IsEmpty())
    {
      ReSize(theOther.myNbPackedMapNodes);
      const int nBucketsSrc = theOther.myNbBuckets;
      const int nBuckets    = myNbBuckets;
      for (int i = 0; i <= nBucketsSrc; i++)
      {
        for (const TColStd_intMapNode* p = theOther.myData1[i]; p != nullptr;)
        {
          const size_t aHashCode = p->HashCode(nBuckets);
          myData1[aHashCode]     = new TColStd_intMapNode(p->Mask(), p->Data(), myData1[aHashCode]);
          ++myNbPackedMapNodes;
          p = p->Next();
        }
      }
      //       TColStd_PackedMapOfInteger::Iterator anIt (theOther);
      //       for (; anIt.More(); anIt.Next())
      //         Add (anIt.Key());
    }
  }
  myExtent = theOther.myExtent;
  return *this;
}

//=================================================================================================

void TColStd_PackedMapOfInteger::ReSize(const int theNbBuckets)
{
  int aNewBuck = NCollection_Primes::NextPrimeForMap(theNbBuckets);
  if (aNewBuck <= myNbBuckets)
  {
    if (!IsEmpty())
    {
      return;
    }
    aNewBuck = myNbBuckets;
  }

  TColStd_intMapNode** aNewData =
    (TColStd_intMapNode**)Standard::AllocateOptimal((aNewBuck + 1) * sizeof(TColStd_intMapNode*));
  memset(aNewData, 0, (aNewBuck + 1) * sizeof(TColStd_intMapNode*));
  if (myData1 != nullptr)
  {
    TColStd_intMapNode** anOldData = myData1;
    for (int i = 0; i <= myNbBuckets; ++i)
    {
      for (TColStd_intMapNode* p = anOldData[i]; p != nullptr;)
      {
        size_t              k = p->HashCode(aNewBuck);
        TColStd_intMapNode* q = p->Next();
        p->SetNext(aNewData[k]);
        aNewData[k] = p;
        p           = q;
      }
    }
  }

  Standard::Free(myData1);
  myNbBuckets = aNewBuck;
  myData1     = aNewData;
}

//=================================================================================================

void TColStd_PackedMapOfInteger::Clear()
{
  if (!IsEmpty())
  {
    for (int aBucketIter = 0; aBucketIter <= myNbBuckets; ++aBucketIter)
    {
      if (myData1[aBucketIter])
      {
        for (TColStd_intMapNode* aSubNodeIter = myData1[aBucketIter]; aSubNodeIter != nullptr;)
        {
          TColStd_intMapNode* q = aSubNodeIter->Next();
          delete aSubNodeIter;
          aSubNodeIter = q;
        }
      }
    }
  }

  myNbPackedMapNodes = 0;
  Standard::Free(myData1);
  myData1  = nullptr;
  myExtent = 0;
}

//=================================================================================================

bool TColStd_PackedMapOfInteger::Add(const int aKey)
{
  if (Resizable())
  {
    ReSize(myNbPackedMapNodes);
  }

  const int           aKeyInt     = packedKeyIndex(aKey);
  const size_t        aHashCode   = HashCode(aKeyInt, myNbBuckets);
  TColStd_intMapNode* aBucketHead = myData1[aHashCode];
  for (TColStd_intMapNode* p = aBucketHead; p != nullptr; p = p->Next())
  {
    if (p->IsEqual(aKeyInt))
    {
      if (p->AddValue(aKey))
      {
        ++myExtent;
        return true;
      }
      return false;
    }
  }

  myData1[aHashCode] = new TColStd_intMapNode(aKey, aBucketHead);
  ++myNbPackedMapNodes;
  ++myExtent;
  return true;
}

//=================================================================================================

bool TColStd_PackedMapOfInteger::Contains(const int aKey) const
{
  if (IsEmpty())
  {
    return false;
  }

  bool      aResult = false;
  const int aKeyInt = packedKeyIndex(aKey);
  for (TColStd_intMapNode* p = myData1[HashCode(aKeyInt, myNbBuckets)]; p != nullptr;)
  {
    if (p->IsEqual(aKeyInt))
    {
      aResult = (p->HasValue(aKey) != 0);
      break;
    }
    p = p->Next();
  }
  return aResult;
}

//=================================================================================================

bool TColStd_PackedMapOfInteger::Remove(const int aKey)
{
  if (IsEmpty())
  {
    return false;
  }

  bool                 aResult(false);
  const int            aKeyInt     = packedKeyIndex(aKey);
  TColStd_intMapNode*& aBucketHead = myData1[HashCode(aKeyInt, myNbBuckets)];
  TColStd_intMapNode*  p           = aBucketHead;
  TColStd_intMapNode*  q           = nullptr;
  while (p)
  {
    if (p->IsEqual(aKeyInt))
    {
      aResult = p->DelValue(aKey);
      if (aResult)
      {
        --myExtent;
        if (!p->HasValues())
        {
          --myNbPackedMapNodes;
          if (q != nullptr)
          {
            q->SetNext(p->Next());
          }
          else
          {
            aBucketHead = p->Next();
          }
          delete p;
        }
      }
      break;
    }
    q = p;
    p = p->Next();
  }
  return aResult;
}

//=======================================================================
// function : GetMinimalMapped
// purpose  : Query the minimal contained key value.
//=======================================================================

int TColStd_PackedMapOfInteger::GetMinimalMapped() const
{
  if (IsEmpty())
  {
    return IntegerLast();
  }

  int                       aResult(IntegerLast());
  const TColStd_intMapNode* pFoundNode = nullptr;
  for (int i = 0; i <= myNbBuckets; i++)
  {
    for (const TColStd_intMapNode* p = myData1[i]; p != nullptr; p = p->Next())
    {
      const int aKey = p->Key();
      if (aResult > aKey)
      {
        aResult    = aKey;
        pFoundNode = p;
      }
    }
  }
  if (pFoundNode)
  {
    unsigned int aFullMask(0xffffffff);
    aResult = TColStd_intMapNode_findNext(pFoundNode, aFullMask);
  }
  return aResult;
}

//=======================================================================
// function : GetMaximalMapped
// purpose  : Query the maximal contained key value.
//=======================================================================

int TColStd_PackedMapOfInteger::GetMaximalMapped() const
{
  if (IsEmpty())
  {
    return IntegerFirst();
  }

  int                       aResult(IntegerFirst());
  const TColStd_intMapNode* pFoundNode = nullptr;
  for (int i = 0; i <= myNbBuckets; i++)
  {
    for (const TColStd_intMapNode* p = myData1[i]; p != nullptr; p = p->Next())
    {
      const int aKey = p->Key();
      if (aResult < aKey)
      {
        aResult    = aKey;
        pFoundNode = p;
      }
    }
  }
  if (pFoundNode)
  {
    unsigned int aFullMask(0xffffffff);
    aResult = TColStd_intMapNode_findPrev(pFoundNode, aFullMask);
  }
  return aResult;
}

//=======================================================================
// function : Union
// purpose  : Boolean operation OR between 2 maps
//=======================================================================

void TColStd_PackedMapOfInteger::Union(const TColStd_PackedMapOfInteger& theMap1,
                                       const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty()) // 0 | B == B
    Assign(theMap2);
  else if (theMap2.IsEmpty()) // A | 0 == A
    Assign(theMap1);
  else if (myData1 == theMap1.myData1)
    Unite(theMap2);
  else if (myData1 == theMap2.myData1)
    Unite(theMap1);
  else
  {
    const int nBuckets1 = theMap1.myNbBuckets;
    const int nBuckets2 = theMap2.myNbBuckets;
    Clear();
    // Iteration of the 1st map.
    for (int i = 0; i <= nBuckets1; i++)
    {
      const TColStd_intMapNode* p1 = theMap1.myData1[i];
      while (p1 != nullptr)
      {
        // Find aKey - the base address of currently iterated block
        const int    aKey     = p1->Key();
        const int    aKeyInt  = packedKeyIndex(aKey);
        unsigned int aNewMask = p1->Mask();
        unsigned int aNewData = p1->Data();
        size_t       nValues(p1->NbValues());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode* p2 = theMap2.myData1[HashCode(aKeyInt, nBuckets2)];
        while (p2)
        {
          if (p2->IsEqual(aKeyInt))
          {
            aNewData |= p2->Data();
            nValues = TColStd_Population(aNewMask, aNewData);
            break;
          }
          p2 = p2->Next();
        }
        // Store the block - result of operation
        if (Resizable())
        {
          ReSize(myNbPackedMapNodes);
        }
        const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
        myData1[aHashCode]     = new TColStd_intMapNode(aNewMask, aNewData, myData1[aHashCode]);
        ++myNbPackedMapNodes;
        myExtent += nValues;
        p1 = p1->Next();
      }
    }
    // Iteration of the 2nd map.
    for (int i = 0; i <= nBuckets2; i++)
    {
      const TColStd_intMapNode* p2 = theMap2.myData1[i];
      while (p2 != nullptr)
      {
        // Find aKey - the base address of currently iterated block
        const int aKey    = p2->Key();
        const int aKeyInt = packedKeyIndex(aKey);
        // Find the corresponding block in the 1st map
        const TColStd_intMapNode* p1 = theMap1.myData1[HashCode(aKeyInt, nBuckets1)];
        while (p1)
        {
          if (p1->IsEqual(aKeyInt))
            break;
          p1 = p1->Next();
        }
        // Add the block from the 2nd map only in the case when the similar
        // block has not been found in the 1st map
        if (p1 == nullptr)
        {
          if (Resizable())
          {
            ReSize(myNbPackedMapNodes);
          }
          const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
          myData1[aHashCode] = new TColStd_intMapNode(p2->Mask(), p2->Data(), myData1[aHashCode]);
          ++myNbPackedMapNodes;
          myExtent += p2->NbValues();
        }
        p2 = p2->Next();
      }
    }
  }
}

//=======================================================================
// function : Unite
// purpose  : Boolean operation OR with the given map
//=======================================================================

bool TColStd_PackedMapOfInteger::Unite(const TColStd_PackedMapOfInteger& theMap)
{
  if (theMap.IsEmpty() || myData1 == theMap.myData1) // A | 0 == A | A == A
    return false;
  else if (IsEmpty())
  { // 0 | B == B
    Assign(theMap);
    return true;
  }

  size_t    aNewExtent(myExtent);
  const int nBuckets2 = theMap.myNbBuckets;

  // Iteration of the 2nd map.
  for (int i = 0; i <= nBuckets2; i++)
  {
    const TColStd_intMapNode* p2 = theMap.myData1[i];
    while (p2 != nullptr)
    {
      // Find aKey - the base address of currently iterated block of integers
      const int aKey    = p2->Key();
      const int aKeyInt = packedKeyIndex(aKey);
      // Find the corresponding block in the 1st (this) map
      size_t              aHashCode = HashCode(aKeyInt, myNbBuckets);
      TColStd_intMapNode* p1        = myData1[aHashCode];
      while (p1)
      {
        if (p1->IsEqual(aKeyInt))
        {
          const size_t anOldPop = p1->NbValues();
          unsigned int newData  = p1->Data() | p2->Data();
          if (newData != p1->Data())
          {
            p1->ChangeData() = newData;
            aNewExtent = aNewExtent - anOldPop + TColStd_Population(p1->ChangeMask(), newData);
          }
          break;
        }
        p1 = p1->Next();
      }
      // If the block is not found in the 1st map, add it to the 1st map
      if (p1 == nullptr)
      {
        if (Resizable())
        {
          ReSize(myNbPackedMapNodes);
          aHashCode = HashCode(aKeyInt, myNbBuckets);
        }
        myData1[aHashCode] = new TColStd_intMapNode(p2->Mask(), p2->Data(), myData1[aHashCode]);
        ++myNbPackedMapNodes;
        aNewExtent += p2->NbValues();
      }
      p2 = p2->Next();
    }
  }
  bool isChanged = (myExtent != aNewExtent);
  myExtent       = aNewExtent;
  return isChanged;
}

//=======================================================================
// function : Intersection
// purpose  : Boolean operation AND between 2 maps
//=======================================================================

void TColStd_PackedMapOfInteger::Intersection(const TColStd_PackedMapOfInteger& theMap1,
                                              const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty() || theMap2.IsEmpty()) // A & 0 == 0 & B == 0
    Clear();
  else if (myData1 == theMap1.myData1)
    Intersect(theMap2);
  else if (myData1 == theMap2.myData1)
    Intersect(theMap1);
  else
  {
    const TColStd_intMapNode* const* aData1;
    const TColStd_intMapNode* const* aData2;
    int                              nBuckets1, nBuckets2;
    if (theMap1.Extent() < theMap2.Extent())
    {
      aData1    = theMap1.myData1;
      aData2    = theMap2.myData1;
      nBuckets1 = theMap1.myNbBuckets;
      nBuckets2 = theMap2.myNbBuckets;
    }
    else
    {
      aData1    = theMap2.myData1;
      aData2    = theMap1.myData1;
      nBuckets1 = theMap2.myNbBuckets;
      nBuckets2 = theMap1.myNbBuckets;
    }
    Clear();

    // Iteration of the 1st map.
    for (int i = 0; i <= nBuckets1; i++)
    {
      const TColStd_intMapNode* p1 = aData1[i];
      while (p1 != nullptr)
      {
        // Find aKey - the base address of currently iterated block
        const int aKey    = p1->Key();
        const int aKeyInt = packedKeyIndex(aKey);
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode* p2 = aData2[HashCode(aKeyInt, nBuckets2)];
        while (p2)
        {
          if (p2->IsEqual(aKeyInt))
          {
            const unsigned int aNewData = p1->Data() & p2->Data();
            // Store the block - result of operation
            if (aNewData)
            {
              if (Resizable())
              {
                ReSize(myNbPackedMapNodes);
              }
              const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
              unsigned int aNewMask  = p1->Mask();
              myExtent += TColStd_Population(aNewMask, aNewData);
              myData1[aHashCode] = new TColStd_intMapNode(aNewMask, aNewData, myData1[aHashCode]);
              ++myNbPackedMapNodes;
            }
            break;
          }
          p2 = p2->Next();
        }
        p1 = p1->Next();
      }
    }
  }
}

//=======================================================================
// function : Intersect
// purpose  : Boolean operation AND with the given map
//=======================================================================

bool TColStd_PackedMapOfInteger::Intersect(const TColStd_PackedMapOfInteger& theMap)
{
  if (IsEmpty()) // 0 & B == 0
    return false;
  else if (theMap.IsEmpty())
  { // A & 0 == 0
    Clear();
    return true;
  }
  else if (myData1 == theMap.myData1) // A & A == A
    return false;

  size_t    aNewExtent(0);
  const int nBuckets2 = theMap.myNbBuckets;

  // Iteration of this map.
  for (int i = 0; i <= myNbBuckets; i++)
  {
    TColStd_intMapNode* q  = nullptr;
    TColStd_intMapNode* p1 = myData1[i];
    while (p1 != nullptr)
    {
      // Find aKey - the base address of currently iterated block of integers
      const int aKey    = p1->Key();
      const int aKeyInt = packedKeyIndex(aKey);
      // Find the corresponding block in the 2nd map
      const TColStd_intMapNode* p2 = theMap.myData1[HashCode(aKeyInt, nBuckets2)];
      while (p2)
      {
        if (p2->IsEqual(aKeyInt))
        {
          const unsigned int aNewData = p1->Data() & p2->Data();
          // Store the block - result of operation
          if (aNewData == 0)
            p2 = nullptr; // no match - the block has to be removed
          else
          {
            if (aNewData != p1->Data())
              p1->ChangeData() = aNewData;
            aNewExtent += TColStd_Population(p1->ChangeMask(), aNewData);
          }
          break;
        }
        p2 = p2->Next();
      }
      TColStd_intMapNode* pNext = p1->Next();
      // If p2!=NULL, then the map node is kept and we move to the next one
      // Otherwise we should remove the current node
      if (p2)
      {
        q = p1;
      }
      else
      {
        --myNbPackedMapNodes;
        if (q)
          q->SetNext(pNext);
        else
          myData1[i] = pNext;
        delete p1;
      }
      p1 = pNext;
    }
  }
  bool isChanged = (myExtent != aNewExtent);
  myExtent       = aNewExtent;
  return isChanged;
}

//=======================================================================
// function : Subtraction
// purpose  : Boolean operation SUBTRACT between two maps
//=======================================================================

void TColStd_PackedMapOfInteger::Subtraction(const TColStd_PackedMapOfInteger& theMap1,
                                             const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty() || theMap2.myData1 == theMap1.myData1) // 0 \ A == A \ A == 0
    Clear();
  else if (theMap2.IsEmpty()) // A \ 0 == A
    Assign(theMap1);
  else if (myData1 == theMap1.myData1)
    Subtract(theMap2);
  else if (myData1 == theMap2.myData1)
  {
    TColStd_PackedMapOfInteger aMap;
    aMap.Subtraction(theMap1, theMap2);
    Assign(aMap);
  }
  else
  {
    const int nBuckets1 = theMap1.myNbBuckets;
    const int nBuckets2 = theMap2.myNbBuckets;
    Clear();

    // Iteration of the 1st map.
    for (int i = 0; i <= nBuckets1; i++)
    {
      const TColStd_intMapNode* p1 = theMap1.myData1[i];
      while (p1 != nullptr)
      {
        // Find aKey - the base address of currently iterated block of integers
        const int    aKey     = p1->Key();
        const int    aKeyInt  = packedKeyIndex(aKey);
        unsigned int aNewMask = p1->Mask();
        unsigned int aNewData = p1->Data();
        size_t       nValues(p1->NbValues());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode* p2 = theMap2.myData1[HashCode(aKeyInt, nBuckets2)];
        while (p2)
        {
          if (p2->IsEqual(aKeyInt))
          {
            aNewData &= ~p2->Data();
            nValues = TColStd_Population(aNewMask, aNewData);
            break;
          }
          p2 = p2->Next();
        }
        // Store the block - result of operation
        if (aNewData)
        {
          if (Resizable())
          {
            ReSize(myNbPackedMapNodes);
          }
          const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
          myData1[aHashCode]     = new TColStd_intMapNode(aNewMask, aNewData, myData1[aHashCode]);
          ++myNbPackedMapNodes;
          myExtent += nValues;
        }
        p1 = p1->Next();
      }
    }
  }
}

//=======================================================================
// function : Subtract
// purpose  : Boolean operation SUBTRACT with the given map
//=======================================================================

bool TColStd_PackedMapOfInteger::Subtract(const TColStd_PackedMapOfInteger& theMap)
{
  if (IsEmpty() || theMap.IsEmpty()) // 0 \ B == 0; A \ 0 == A
    return false;
  else if (myData1 == theMap.myData1)
  { // A \ A == 0
    Clear();
    return true;
  }
  else
  {
    size_t    aNewExtent(0);
    const int nBuckets2 = theMap.myNbBuckets;
    // Iteration of this map.
    for (int i = 0; i <= myNbBuckets; i++)
    {
      TColStd_intMapNode* q  = nullptr;
      TColStd_intMapNode* p1 = myData1[i];
      while (p1 != nullptr)
      {
        // Find aKey - the base address of currently iterated block of integers
        const int           aKey    = p1->Key();
        const int           aKeyInt = packedKeyIndex(aKey);
        TColStd_intMapNode* pNext   = p1->Next();
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode* p2 = theMap.myData1[HashCode(aKeyInt, nBuckets2)];
        while (p2)
        {
          if (p2->IsEqual(aKeyInt))
          {
            const unsigned int aNewData = p1->Data() & ~p2->Data();
            // Store the block - result of operation
            if (aNewData == 0)
            {
              // no match - the block has to be removed
              --myNbPackedMapNodes;
              if (q)
                q->SetNext(pNext);
              else
                myData1[i] = pNext;
              delete p1;
            }
            else if (aNewData != p1->Data())
            {
              p1->ChangeData() = aNewData;
              aNewExtent += TColStd_Population(p1->ChangeMask(), aNewData);
              q = p1;
            }
            else
            {
              aNewExtent += p1->NbValues();
              q = p1;
            }
            break;
          }
          p2 = p2->Next();
        }
        if (p2 == nullptr)
        {
          aNewExtent += p1->NbValues();
          q = p1;
        }
        p1 = pNext;
      }
    }
    bool isChanged = (myExtent != aNewExtent);
    myExtent       = aNewExtent;
    return isChanged;
  }
}

//=================================================================================================

void TColStd_PackedMapOfInteger::Difference(const TColStd_PackedMapOfInteger& theMap1,
                                            const TColStd_PackedMapOfInteger& theMap2)
{
  if (theMap1.IsEmpty()) // 0 ^ B == B
    Assign(theMap2);
  else if (theMap2.IsEmpty()) // A ^ 0 == A
    Assign(theMap1);
  else if (myData1 == theMap1.myData1)
    Differ(theMap2);
  else if (myData1 == theMap2.myData1)
    Differ(theMap1);
  else
  {
    int       i;
    const int nBuckets1 = theMap1.myNbBuckets;
    const int nBuckets2 = theMap2.myNbBuckets;
    Clear();

    // Iteration of the 1st map.
    for (i = 0; i <= nBuckets1; i++)
    {
      const TColStd_intMapNode* p1 = theMap1.myData1[i];
      while (p1 != nullptr)
      {
        // Find aKey - the base address of currently iterated block of integers
        const int    aKey     = p1->Key();
        const int    aKeyInt  = packedKeyIndex(aKey);
        unsigned int aNewMask = p1->Mask();
        unsigned int aNewData = p1->Data();
        size_t       nValues(p1->NbValues());
        // Find the corresponding block in the 2nd map
        const TColStd_intMapNode* p2 = theMap2.myData1[HashCode(aKeyInt, nBuckets2)];
        while (p2)
        {
          if (p2->IsEqual(aKeyInt))
          {
            aNewData ^= p2->Data();
            nValues = TColStd_Population(aNewMask, aNewData);
            break;
          }
          p2 = p2->Next();
        }
        // Store the block - result of operation
        if (aNewData)
        {
          if (Resizable())
          {
            ReSize(myNbPackedMapNodes);
          }
          const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
          myData1[aHashCode]     = new TColStd_intMapNode(aNewMask, aNewData, myData1[aHashCode]);
          ++myNbPackedMapNodes;
          myExtent += nValues;
        }
        p1 = p1->Next();
      }
    }

    // Iteration of the 2nd map.
    for (i = 0; i <= nBuckets2; i++)
    {
      const TColStd_intMapNode* p2 = theMap2.myData1[i];
      while (p2 != nullptr)
      {
        // Find aKey - the base address of currently iterated block
        const int aKey    = p2->Key();
        const int aKeyInt = packedKeyIndex(aKey);
        // Find the corresponding block in the 1st map
        const TColStd_intMapNode* p1 = theMap1.myData1[HashCode(aKeyInt, nBuckets1)];
        while (p1)
        {
          if (p1->IsEqual(aKeyInt))
            break;
          p1 = p1->Next();
        }
        // Add the block from the 2nd map only in the case when the similar
        // block has not been found in the 1st map
        if (p1 == nullptr)
        {
          if (Resizable())
          {
            ReSize(myNbPackedMapNodes);
          }
          const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
          myData1[aHashCode] = new TColStd_intMapNode(p2->Mask(), p2->Data(), myData1[aHashCode]);
          ++myNbPackedMapNodes;
          myExtent += p2->NbValues();
        }
        p2 = p2->Next();
      }
    }
  }
}

//=================================================================================================

bool TColStd_PackedMapOfInteger::Differ(const TColStd_PackedMapOfInteger& theMap)
{
  if (theMap.IsEmpty()) // A ^ 0 = A
    return false;
  else if (IsEmpty())
  { // 0 ^ B = B
    Assign(theMap);
    return true;
  }
  else if (myData1 == theMap.myData1)
  { // A ^ A == 0
    Clear();
    return true;
  }

  size_t    aNewExtent(0);
  const int nBuckets2 = theMap.myNbBuckets;
  bool      isChanged = false;
  // Iteration by other map
  for (int i = 0; i <= nBuckets2; i++)
  {
    TColStd_intMapNode*       q  = nullptr;
    const TColStd_intMapNode* p2 = theMap.myData1[i];
    while (p2 != nullptr)
    {
      // Find aKey - the base address of currently iterated block
      const int aKey    = p2->Key();
      const int aKeyInt = packedKeyIndex(aKey);

      // Find the corresponding block in the 1st map
      TColStd_intMapNode* p1    = myData1[HashCode(aKeyInt, myNbBuckets)];
      TColStd_intMapNode* pNext = p1->Next();
      while (p1)
      {
        if (p1->IsEqual(aKeyInt))
        {
          const unsigned int aNewData = p1->Data() ^ p2->Data();
          // Store the block - result of operation
          if (aNewData == 0)
          {
            // no match - the block has to be removed
            --myNbPackedMapNodes;
            if (q)
              q->SetNext(pNext);
            else
              myData1[i] = pNext;
            delete p1;
          }
          else if (aNewData != p1->Data())
          {
            p1->ChangeData() = aNewData;
            isChanged        = true;
            aNewExtent += TColStd_Population(p1->ChangeMask(), aNewData);
            q = p1;
          }
          break;
        }
        p1 = pNext;
      }
      // Add the block from the 2nd map only in the case when the similar
      // block has not been found in the 1st map
      if (p1 == nullptr)
      {
        if (Resizable())
        {
          ReSize(myNbPackedMapNodes);
        }
        const size_t aHashCode = HashCode(aKeyInt, myNbBuckets);
        myData1[aHashCode]     = new TColStd_intMapNode(p2->Mask(), p2->Data(), myData1[aHashCode]);
        ++myNbPackedMapNodes;
        aNewExtent += p2->NbValues();
        isChanged = true;
      }
      p2 = p2->Next();
    }
  }
  myExtent = aNewExtent;
  return isChanged;
}

//=======================================================================
// function : IsEqual
// purpose  : Boolean operation returns true if this map is equal to the other map
//=======================================================================

bool TColStd_PackedMapOfInteger::IsEqual(const TColStd_PackedMapOfInteger& theMap) const
{
  if (IsEmpty() && theMap.IsEmpty())
    return true;
  else if (Extent() != theMap.Extent())
    return false;
  else if (myData1 == theMap.myData1)
    return true;

  const int nBuckets2 = theMap.myNbBuckets;
  // Iteration of this map.
  for (int i = 0; i <= myNbBuckets; i++)
  {
    const TColStd_intMapNode* p1 = myData1[i];
    while (p1 != nullptr)
    {
      // Find aKey - the base address of currently iterated block of integers
      const int           aKey    = p1->Key();
      const int           aKeyInt = packedKeyIndex(aKey);
      TColStd_intMapNode* pNext   = p1->Next();
      // Find the corresponding block in the 2nd map
      const TColStd_intMapNode* p2 = theMap.myData1[HashCode(aKeyInt, nBuckets2)];
      while (p2)
      {
        if (p2->IsEqual(aKeyInt))
        {
          if (p1->Data() != p2->Data())
            return false;
          break;
        }
        p2 = p2->Next();
      }
      // if the same block not found, maps are different
      if (p2 == nullptr)
        return false;

      p1 = pNext;
    }
  }
  return true;
}

//=======================================================================
// function : IsSubset
// purpose  : Boolean operation returns true if this map if subset of other map
//=======================================================================

bool TColStd_PackedMapOfInteger::IsSubset(const TColStd_PackedMapOfInteger& theMap) const
{
  if (IsEmpty()) // 0 <= A
    return true;
  else if (theMap.IsEmpty()) // ! ( A <= 0 )
    return false;
  else if (Extent() > theMap.Extent())
    return false;
  else if (myData1 == theMap.myData1)
    return true;

  const int nBuckets2 = theMap.myNbBuckets;
  // Iteration of this map.
  for (int i = 0; i <= myNbBuckets; i++)
  {
    const TColStd_intMapNode* p1 = myData1[i];
    while (p1 != nullptr)
    {
      // Find aKey - the base address of currently iterated block of integers
      const int           aKey    = p1->Key();
      const int           aKeyInt = packedKeyIndex(aKey);
      TColStd_intMapNode* pNext   = p1->Next();
      // Find the corresponding block in the 2nd map
      const TColStd_intMapNode* p2 = theMap.myData1[HashCode(aKeyInt, nBuckets2)];
      if (!p2)
        return false;
      while (p2)
      {
        if (p2->IsEqual(aKeyInt))
        {
          if (p1->Data() & ~p2->Data()) // at least one bit set in p1 is not set in p2
            return false;
          break;
        }
        p2 = p2->Next();
      }
      p1 = pNext;
    }
  }
  return true;
}

//=======================================================================
// function : HasIntersection
// purpose  : Boolean operation returns true if this map intersects with other map
//=======================================================================

bool TColStd_PackedMapOfInteger::HasIntersection(const TColStd_PackedMapOfInteger& theMap) const
{
  if (IsEmpty() || theMap.IsEmpty()) // A * 0 == 0 * B == 0
    return false;

  if (myData1 == theMap.myData1)
    return true;

  const int nBuckets2 = theMap.myNbBuckets;
  // Iteration of this map.
  for (int i = 0; i <= myNbBuckets; i++)
  {
    const TColStd_intMapNode* p1 = myData1[i];
    while (p1 != nullptr)
    {
      // Find aKey - the base address of currently iterated block of integers
      const int           aKey    = p1->Key();
      const int           aKeyInt = packedKeyIndex(aKey);
      TColStd_intMapNode* pNext   = p1->Next();
      // Find the corresponding block in the 2nd map
      const TColStd_intMapNode* p2 = theMap.myData1[HashCode(aKeyInt, nBuckets2)];
      while (p2)
      {
        if (p2->IsEqual(aKeyInt))
        {
          if (p1->Data() & p2->Data())
            return true;
          break;
        }
        p2 = p2->Next();
      }
      p1 = pNext;
    }
  }
  return false;
}

//=================================================================================================

void TColStd_PackedMapOfInteger::Statistics(Standard_OStream& theStream) const
{
  theStream << "\nMap Statistics\n---------------\n\n";
  theStream << "This Map has " << myNbBuckets << " Buckets and " << myNbPackedMapNodes
            << " Keys\n\n";
  if (myNbPackedMapNodes == 0)
  {
    return;
  }

  NCollection_Array1<int> aSizes(0, myNbPackedMapNodes);
  aSizes.Init(0);

  theStream << "\nStatistics for the first Key\n";
  int aNbNonEmpty = 0;
  for (int aBucketIter = 0; aBucketIter <= myNbBuckets; ++aBucketIter)
  {
    TColStd_intMapNode* aSubNodeIter = myData1[aBucketIter];
    if (aSubNodeIter != nullptr)
    {
      ++aNbNonEmpty;
    }

    int aNbMapSubNodes = 0;
    for (; aSubNodeIter != nullptr; aSubNodeIter = aSubNodeIter->Next())
    {
      ++aNbMapSubNodes;
    }
    ++aSizes[aNbMapSubNodes];
  }

  // display results
  int aNbMapSubNodesTotal = 0;
  for (int aNbMapSubNodes = 0; aNbMapSubNodes <= myNbPackedMapNodes; ++aNbMapSubNodes)
  {
    if (aSizes[aNbMapSubNodes] > 0)
    {
      aNbMapSubNodesTotal += aSizes[aNbMapSubNodes] * aNbMapSubNodes;
      theStream << std::setw(5) << aSizes[aNbMapSubNodes] << " buckets of size " << aNbMapSubNodes
                << "\n";
    }
  }

  const double aMean = ((double)aNbMapSubNodesTotal) / ((double)aNbNonEmpty);
  theStream << "\n\nMean of length: " << aMean << "\n";
}
