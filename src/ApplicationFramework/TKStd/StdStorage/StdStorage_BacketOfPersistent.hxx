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

#ifndef _StdStorage_BucketOfPersistent_HeaderFile
#define _StdStorage_BucketOfPersistent_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>

class StdObjMgt_Persistent;

class StdStorage_Bucket
{
  friend class StdStorage_BucketIterator;
  friend class StdStorage_BucketOfPersistent;

  StdObjMgt_Persistent** mySpace;
  int                    mySpaceSize;
  int                    myCurrentSpace;

  void Append(StdObjMgt_Persistent*);

  StdObjMgt_Persistent* Value(const int theIndex) const;

public:
  StdStorage_Bucket()
      : mySpace(0L),
        mySpaceSize(200000),
        myCurrentSpace(-1)
  {
    mySpace =
      (StdObjMgt_Persistent**)Standard::Allocate(sizeof(StdObjMgt_Persistent*) * mySpaceSize);
  }

  StdStorage_Bucket(const int theSpaceSize)
      : mySpace(0L),
        mySpaceSize(theSpaceSize),
        myCurrentSpace(-1)
  {
    mySpace =
      (StdObjMgt_Persistent**)Standard::Allocate(sizeof(StdObjMgt_Persistent*) * mySpaceSize);
  }

  void Clear();

  ~StdStorage_Bucket();
};

class StdStorage_BucketOfPersistent
{
  friend class StdStorage_BucketIterator;
  StdStorage_Bucket** myBuckets;
  int                 myNumberOfBucket;
  int                 myNumberOfBucketAllocated;
  StdStorage_Bucket*  myCurrentBucket;
  int                 myCurrentBucketNumber;
  int                 myLength;
  int                 myBucketSize;

public:
  StdStorage_BucketOfPersistent(const int theBucketSize = 300000, const int theBucketNumber = 100);

  int Length() const { return myLength; }

  void Append(const occ::handle<StdObjMgt_Persistent>& sp);

  StdObjMgt_Persistent* Value(const int theIndex);

  void Clear();

  ~StdStorage_BucketOfPersistent();
};

class StdStorage_BucketIterator
{
  StdStorage_BucketOfPersistent* myBucket;
  StdStorage_Bucket*             myCurrentBucket;
  int                            myCurrentBucketIndex;
  int                            myCurrentIndex;
  int                            myBucketNumber;
  bool                           myMoreObject;

public:
  StdStorage_BucketIterator(StdStorage_BucketOfPersistent*);

  void Init(StdStorage_BucketOfPersistent*);
  void Reset();

  StdObjMgt_Persistent* Value() const
  {
    if (myCurrentBucket)
    {
      return myCurrentBucket->mySpace[myCurrentIndex];
    }
    else
      return 0L;
  }

  bool More() const { return myMoreObject; }

  void Next();
};

#endif // _StdStorage_BucketOfPersistent_HeaderFile
