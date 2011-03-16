// File:	HLRAlgo_EdgeIterator.cxx
// Created:	Mon Jan 11 13:58:26 1993
// Author:	Christophe MARION
//		<cma@sdsun1>
#ifndef No_Exception
#define No_Exception
#endif
#include <HLRAlgo_EdgeIterator.ixx>

//=======================================================================
//function : EdgeIterator
//purpose  : 
//=======================================================================

HLRAlgo_EdgeIterator::HLRAlgo_EdgeIterator ()
{}

//=======================================================================
//function : InitHidden
//purpose  : 
//=======================================================================

void HLRAlgo_EdgeIterator::InitHidden (const HLRAlgo_EdgeStatus& status)
{
  EHid = (Standard_Address)&status;
  iHid = 1;
  if (((HLRAlgo_EdgeStatus*)EHid)->AllHidden()) {
    ((HLRAlgo_EdgeStatus*)EHid)->Bounds
      (myHidStart,myHidTolStart,myHidEnd,myHidTolEnd);
    myNbHid = 0;
  }
  else {
    myNbHid = ((HLRAlgo_EdgeStatus*)EHid)->NbVisiblePart();
    Standard_Real B1;
    Standard_ShortReal B2;
    ((HLRAlgo_EdgeStatus*)EHid)->Bounds
      (myHidStart,myHidTolStart,B1,B2);
    ((HLRAlgo_EdgeStatus*)EHid)->VisiblePart
      (iHid,myHidEnd,myHidTolEnd,B1,B2);
  }
  if (myHidStart + myHidTolStart >= myHidEnd   - myHidTolEnd && 
      myHidEnd   + myHidTolEnd   >= myHidStart - myHidTolStart   )
    NextHidden(); 
}

//=======================================================================
//function : NextHidden
//purpose  : 
//=======================================================================

void HLRAlgo_EdgeIterator::NextHidden ()
{
  if (iHid >= myNbHid + 1) iHid++;
  else {
    Standard_Real B1;
    Standard_ShortReal B2;
    ((HLRAlgo_EdgeStatus*)EHid)->VisiblePart
      (iHid,B1,B2,myHidStart,myHidTolStart);
    iHid++;
    if (iHid == myNbHid + 1) {
      ((HLRAlgo_EdgeStatus*)EHid)->Bounds(B1,B2,myHidEnd,myHidTolEnd);
      if (myHidStart + myHidTolStart >= myHidEnd   - myHidTolEnd && 
	  myHidEnd   + myHidTolEnd   >= myHidStart - myHidTolStart ) iHid++;
    }
    else ((HLRAlgo_EdgeStatus*)EHid)->VisiblePart
      (iHid,myHidEnd,myHidTolEnd,B1,B2);
  }
}

