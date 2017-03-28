// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <StdStorage_BacketOfPersistent.hxx>
#include <StdObjMgt_Persistent.hxx>

StdStorage_Bucket::~StdStorage_Bucket()
{
  Standard::Free(mySpace);
  mySpace = 0L;
  mySpaceSize = 0;
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void StdStorage_Bucket::Clear()
{
  myCurrentSpace = -1;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
void StdStorage_Bucket::Append(StdObjMgt_Persistent *sp)
{
  ++myCurrentSpace;
  mySpace[myCurrentSpace] = sp;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
StdObjMgt_Persistent* StdStorage_Bucket::Value(const Standard_Integer theIndex) const
{
  return mySpace[theIndex];
}

//=======================================================================
//function : Storage_BucketOfPersistent
//purpose  : 
//=======================================================================
StdStorage_BucketOfPersistent::StdStorage_BucketOfPersistent
(const Standard_Integer theBucketSize, const Standard_Integer theBucketNumber)
: myNumberOfBucket(1), myNumberOfBucketAllocated(theBucketNumber)
, myBucketSize(theBucketSize)
{
  myBuckets = (StdStorage_Bucket**)Standard::Allocate
    (sizeof(StdStorage_Bucket*) * theBucketNumber);
  myBuckets[0] = new StdStorage_Bucket(myBucketSize);
  myCurrentBucket = myBuckets[0];
  myLength = 0;
  myCurrentBucketNumber = 0;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void StdStorage_BucketOfPersistent::Clear()
{
  if (myBuckets) {
    Standard_Integer i;

    for (i = 1; i < myNumberOfBucket; i++) delete myBuckets[i];
    myNumberOfBucket = 1;
    myCurrentBucket = myBuckets[0];
    myCurrentBucket->Clear();
    myCurrentBucketNumber = 0;
    myLength = 0;
  }
}

StdStorage_BucketOfPersistent::~StdStorage_BucketOfPersistent()
{
  Clear();
  delete myBuckets[0];
  Standard::Free(myBuckets);
  myBuckets = 0L;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
StdObjMgt_Persistent* StdStorage_BucketOfPersistent::Value
  (const Standard_Integer theIndex)
{
  Standard_Integer theInd, theCurrentBucketNumber, tecurrentind = theIndex - 1;
  theCurrentBucketNumber = tecurrentind / myBucketSize;
  theInd = tecurrentind - (myBucketSize * theCurrentBucketNumber);

  return myBuckets[theCurrentBucketNumber]->mySpace[theInd];
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
void StdStorage_BucketOfPersistent::Append(const Handle(StdObjMgt_Persistent)& sp)
{
  myCurrentBucket->myCurrentSpace++;

  if (myCurrentBucket->myCurrentSpace != myBucketSize) {
    myLength++;
    myCurrentBucket->mySpace[myCurrentBucket->myCurrentSpace] = sp.operator->();
    return;
  }

  myCurrentBucket->myCurrentSpace--;
  myNumberOfBucket++;
  myCurrentBucketNumber++;

  if (myNumberOfBucket > myNumberOfBucketAllocated) {
    Standard_Size e = sizeof(StdStorage_Bucket*) * myNumberOfBucketAllocated;
    myBuckets = (StdStorage_Bucket**)Standard::Reallocate(myBuckets, e * 2);
    myNumberOfBucketAllocated *= 2;
  }

  myBuckets[myCurrentBucketNumber] = new StdStorage_Bucket(myBucketSize);
  myCurrentBucket = myBuckets[myCurrentBucketNumber];
  myCurrentBucket->myCurrentSpace++;
  myLength++;
  myCurrentBucket->mySpace[myCurrentBucket->myCurrentSpace] = sp.operator->();
}

//=======================================================================
//function : Storage_BucketIterator
//purpose  : 
//=======================================================================
StdStorage_BucketIterator::StdStorage_BucketIterator
  (StdStorage_BucketOfPersistent* aBucketManager)
{
  if (aBucketManager) {
    myBucket = aBucketManager;
    myCurrentBucket = myBucket->myBuckets[0];
    myBucketNumber = aBucketManager->myNumberOfBucket;
    myCurrentBucketIndex = 0;
    myCurrentIndex = 0;
    myMoreObject = Standard_True;
  }
  else myMoreObject = Standard_False;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================
void StdStorage_BucketIterator::Reset()
{
  if (myBucket) {
    myCurrentBucket = myBucket->myBuckets[0];
    myBucketNumber = myBucket->myNumberOfBucket;
    myCurrentIndex = 0;
    myCurrentBucketIndex = 0;
    myMoreObject = Standard_True;
  }
  else myMoreObject = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void StdStorage_BucketIterator::Init(StdStorage_BucketOfPersistent* aBucketManager)
{
  if (aBucketManager) {
    myBucket = aBucketManager;
    myCurrentBucket = myBucket->myBuckets[0];
    myBucketNumber = aBucketManager->myNumberOfBucket;
    myCurrentIndex = 0;
    myCurrentBucketIndex = 0;
    myMoreObject = Standard_True;
  }
  else myMoreObject = Standard_False;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
void StdStorage_BucketIterator::Next()
{
  if (!myMoreObject) return;

  if (myCurrentIndex < myCurrentBucket->myCurrentSpace) {
    myCurrentIndex++;
  }
  else {
    myCurrentIndex = 0;
    myCurrentBucketIndex++;
    if (myCurrentBucketIndex < myBucketNumber) {
      myCurrentBucket = myBucket->myBuckets[myCurrentBucketIndex];
    }
    else {
      myMoreObject = Standard_False;
    }
  }
}
