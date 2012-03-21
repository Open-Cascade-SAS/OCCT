// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef _Storage_BucketOfPersistent_HeaderFile
#define _Storage_BucketOfPersistent_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Persistent.hxx>

class Storage_Schema;
class Storage_BucketOfPersistent;
class Storage_BucketIterator;

class Storage_Bucket {
  friend class Storage_BucketIterator;
  friend class Storage_Schema;
  friend class Storage_BucketOfPersistent;

  Standard_Persistent** mySpace;
  Standard_Integer mySpaceSize;
  Standard_Integer myCurrentSpace;


  void Append(Standard_Persistent *);
 
  Standard_Persistent* Value(const Standard_Integer theIndex) const;

public:
  Storage_Bucket() : mySpace(0L), mySpaceSize(200000), myCurrentSpace(-1)
    {
      mySpace = (Standard_Persistent**)Standard::Allocate(sizeof(Standard_Persistent*) * mySpaceSize);
    }

  Storage_Bucket(const Standard_Integer theSpaceSize) :  mySpace(0L), mySpaceSize(theSpaceSize), myCurrentSpace(-1)
    {
      mySpace = (Standard_Persistent**)Standard::Allocate(sizeof(Standard_Persistent*) * mySpaceSize);
    }

  void Clear();

  ~Storage_Bucket();
};


class Storage_BucketOfPersistent {
  friend class Storage_BucketIterator;
  Storage_Bucket** myBuckets;
  Standard_Integer myNumberOfBucket;
  Standard_Integer myNumberOfBucketAllocated;
  Storage_Bucket*  myCurrentBucket;
  Standard_Integer myCurrentBucketNumber;
  Standard_Integer myLength;
  Standard_Integer myBucketSize;
  
public:
  Storage_BucketOfPersistent(const Standard_Integer theBucketSize = 300000, const Standard_Integer theBucketNumber = 100);
  
  Standard_Integer Length() const
    {
      return myLength;
    }

  void Append(const Handle(Standard_Persistent)& sp);
  
  Standard_Persistent* Value(const Standard_Integer theIndex);

  void Clear();

  ~Storage_BucketOfPersistent() ;

};

class Storage_BucketIterator {
  Storage_BucketOfPersistent *myBucket;
  Storage_Bucket             *myCurrentBucket;
  Standard_Integer            myCurrentBucketIndex;
  Standard_Integer            myCurrentIndex;
  Standard_Integer            myBucketNumber;
  Standard_Boolean            myMoreObject;

public:
  Storage_BucketIterator(Storage_BucketOfPersistent*);
  void Init(Storage_BucketOfPersistent*);
  void Reset();

  Standard_Persistent* Value() const
    {
      if (myCurrentBucket) {
	return myCurrentBucket->mySpace[myCurrentIndex];
      }
      else return 0L;
    }

  Standard_Boolean More() const
    {
      return myMoreObject;
    }

  void Next();
};

#endif
