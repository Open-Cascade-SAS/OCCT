// Created on: 1993-02-26
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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




#include <TCollection_BasicMapIterator.ixx>
#include <TCollection_BasicMap.hxx>
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

