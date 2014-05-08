// Created on: 2002-04-18
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

// Purpose:     Implementation of the BaseMap class

#include <NCollection_BaseMap.hxx>
#include <TCollection.hxx>

//=======================================================================
//function : BeginResize
//purpose  : 
//=======================================================================

Standard_Boolean  NCollection_BaseMap::BeginResize
  (const Standard_Integer  NbBuckets,
   Standard_Integer&       N,
   NCollection_ListNode**& data1,
   NCollection_ListNode**& data2) const 
{
  if (mySaturated) return Standard_False;
  N = NextPrimeForMap(NbBuckets);
  if (N <= myNbBuckets) {
    if (!myData1)
      N = myNbBuckets;
    else
      return Standard_False;
  }
  data1 = (NCollection_ListNode **)
    myAllocator->Allocate((N+1)*sizeof(NCollection_ListNode *));
  memset(data1, 0, (N+1)*sizeof(NCollection_ListNode *));
  if (isDouble) 
  {
    data2 = (NCollection_ListNode **)
    myAllocator->Allocate((N+1)*sizeof(NCollection_ListNode *));
    memset(data2, 0, (N+1)*sizeof(NCollection_ListNode *));
  }
  else
    data2 = NULL;
  return Standard_True;
}

//=======================================================================
//function : EndResize
//purpose  : 
//=======================================================================

void  NCollection_BaseMap::EndResize
  (const Standard_Integer NbBuckets,
   const Standard_Integer N,
   NCollection_ListNode** data1,
   NCollection_ListNode** data2)
{
  if (myData1) 
    myAllocator->Free(myData1);
  if (myData2) 
    myAllocator->Free(myData2);
  myNbBuckets = N;
  mySaturated = (myNbBuckets <= NbBuckets);
  myData1 = data1;
  myData2 = data2;
}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void  NCollection_BaseMap::Destroy (NCollection_DelMapNode fDel,
                                    Standard_Boolean doReleaseMemory)
{
  if (!IsEmpty()) 
  {
    Standard_Integer i;
    NCollection_ListNode** data = (NCollection_ListNode**) myData1;
    NCollection_ListNode *p,*q;
    for (i = 0; i <= NbBuckets(); i++) 
    {
      if (data[i]) 
      {
        p = data[i];
        while (p) 
        {
          q = (NCollection_ListNode*)p->Next();
          fDel (p, myAllocator);
          p = q;
        }
        data[i] = NULL;
      }
    }
  }

  mySize = 0;
  if (doReleaseMemory)
  {
    mySaturated = Standard_False;
    if (myData1) 
      myAllocator->Free(myData1);
    if (isDouble && myData2) 
      myAllocator->Free(myData2);
    myData1 = myData2 = NULL;
  }
}


//=======================================================================
//function : Statistics
//purpose  : 
//=======================================================================

void NCollection_BaseMap::Statistics(Standard_OStream& S) const
{
  S <<"\nMap Statistics\n---------------\n\n";
  S <<"This Map has "<<myNbBuckets<<" Buckets and "<<mySize<<" Keys\n\n";
  if (mySaturated) S<<"The maximum number of Buckets is reached\n";
  
  if (mySize == 0) return;

  // compute statistics on 1
  Standard_Integer * sizes = new Standard_Integer [mySize+1];
  Standard_Integer i,l,nb;
  NCollection_ListNode* p;
  NCollection_ListNode** data;
  
  S << "\nStatistics for the first Key\n";
  for (i = 0; i <= mySize; i++) sizes[i] = 0;
  data = (NCollection_ListNode **) myData1;
  nb = 0;
  for (i = 0; i <= myNbBuckets; i++) 
  {
    l = 0;
    p = data[i];
    if (p) nb++;
    while (p) 
    {
      l++;
      p = p->Next();
    }
    sizes[l]++;
  }

  // display results
  l = 0;
  for (i = 0; i<= mySize; i++) 
  {
    if (sizes[i] > 0) 
    {
      l += sizes[i] * i;
      S << setw(5) << sizes[i] <<" buckets of size "<<i<<"\n";
    }
  }

  Standard_Real mean = ((Standard_Real) l) / ((Standard_Real) nb);
  S<<"\n\nMean of length : "<<mean<<"\n";

  delete [] sizes;
}

//=======================================================================
//function : NextPrimeForMap
//purpose  : 
//=======================================================================

Standard_Integer NCollection_BaseMap::NextPrimeForMap
  (const Standard_Integer N) const
{
  return TCollection::NextPrimeForMap ( N );
}

