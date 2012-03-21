// Created on: 1993-01-11
// Created by: Christophe MARION
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
