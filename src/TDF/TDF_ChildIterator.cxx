// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	----------------------
// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb  7 1997	Creation

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>

#define ChildIterator_UpToBrother \
{ \
    while (myNode && (myNode->Depth() > myFirstLevel) && !myNode->Brother()) \
      myNode = myNode->Father(); \
	if (myNode && (myNode->Depth() > myFirstLevel) && myNode->Father()) \
	  myNode = myNode->Brother(); \
	else \
	  myNode = NULL; \
}


//=======================================================================
//function : TDF_ChildIterator
//purpose  : 
//=======================================================================

TDF_ChildIterator::TDF_ChildIterator()
: myNode(NULL),
  myFirstLevel(0)
{}


//=======================================================================
//function : TDF_ChildIterator
//purpose  : 
//=======================================================================

TDF_ChildIterator::TDF_ChildIterator
(const TDF_Label& aLabel,
 const Standard_Boolean allLevels)
: myNode(aLabel.myLabelNode->FirstChild()),
  myFirstLevel(allLevels ? aLabel.Depth() : -1)
{}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDF_ChildIterator::Initialize
(const TDF_Label& aLabel,
 const Standard_Boolean allLevels)
{
  myNode = aLabel.myLabelNode->FirstChild();
  myFirstLevel = allLevels ? aLabel.Depth() : -1;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDF_ChildIterator::Next() 
{
  if (myFirstLevel == -1) {
    myNode = myNode->Brother();
  }
  else {
    if (myNode->FirstChild()) myNode = myNode->FirstChild();
    else ChildIterator_UpToBrother;
  }
}


//=======================================================================
//function : NextBrother
//purpose  : 
//=======================================================================

void TDF_ChildIterator::NextBrother() 
{
  if ((myFirstLevel  == -1) || myNode->Brother()) myNode = myNode->Brother();
  else ChildIterator_UpToBrother;
}
