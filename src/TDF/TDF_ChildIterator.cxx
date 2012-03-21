// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
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

//      	----------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb  7 1997	Creation



#include <TDF_ChildIterator.ixx>

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
