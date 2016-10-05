// Created on: 1993-01-11
// Created by: Christophe MARION
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

#ifndef No_Exception
#define No_Exception
#endif


#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRAlgo_EdgeStatus.hxx>

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

void HLRAlgo_EdgeIterator::InitHidden (HLRAlgo_EdgeStatus& status)
{
  EHid = &status;
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
