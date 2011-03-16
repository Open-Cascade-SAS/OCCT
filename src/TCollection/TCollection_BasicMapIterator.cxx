
// File:	TCollection_BasicMapIterator.cxx
// Created:	Fri Feb 26 15:46:25 1993
// Author:	Remi LEQUETTE
//		<rle@phylox>


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

