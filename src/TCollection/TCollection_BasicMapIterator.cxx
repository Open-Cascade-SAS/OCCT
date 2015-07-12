// Created on: 1993-02-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <TCollection_BasicMap.hxx>
#include <TCollection_BasicMapIterator.hxx>
#include <TCollection_MapNode.hxx>

//=======================================================================
//function : TCollection_BasicMapIterator
//purpose  : 
//=======================================================================
TCollection_BasicMapIterator::TCollection_BasicMapIterator () :
       myNode(NULL),
       myNbBuckets(0),
       myBuckets(NULL),
       myBucket(0)
{}


//=======================================================================
//function : TCollection_BasicMapIterator
//purpose  : 
//=======================================================================

TCollection_BasicMapIterator::TCollection_BasicMapIterator 
  (const TCollection_BasicMap& M) :
  myNode(NULL),
  myNbBuckets(M.myNbBuckets),
  myBuckets(M.myData1),
  myBucket(-1)
{
  if (!myBuckets) myNbBuckets = -1;
  Next();
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TCollection_BasicMapIterator::Initialize
  (const TCollection_BasicMap& M)
{
  myNbBuckets = M.myNbBuckets;
  myBuckets = M.myData1;
  myBucket = -1;
  myNode = NULL;
  if (!myBuckets) myNbBuckets = -1;
  Next();
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void TCollection_BasicMapIterator::Reset()
{
  myBucket = -1;
  myNode = NULL;
  Next();
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TCollection_BasicMapIterator::Next()
{
  if (!myBuckets) return; 

  if (myNode) {
    myNode = ((TCollection_MapNode*) myNode)->Next();
    if (myNode) return;
  }

  while (!myNode) {
    myBucket++;
    if (myBucket > myNbBuckets) return;
    myNode = ((TCollection_MapNodePtr*)myBuckets)[myBucket];
  }
}

