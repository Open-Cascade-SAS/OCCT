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

//      	-----------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Nov 20 1997	Creation



#include <TDF_ChildIDIterator.ixx>

#include <TDF_ChildIterator.ixx>
#include <TDF_Label.hxx>
#include <TDF_LabelNode.hxx>
#include <TDF_LabelNodePtr.hxx>

#define ChildIDIterator_FindNext \
{ while( myItr.More() &&  !myItr.Value().FindAttribute(myID,myAtt)) myItr.Next(); }



//=======================================================================
//function : TDF_ChildIDIterator
//purpose  : 
//=======================================================================

TDF_ChildIDIterator::TDF_ChildIDIterator()
{}


//=======================================================================
//function : TDF_ChildIDIterator
//purpose  : 
//=======================================================================

TDF_ChildIDIterator::TDF_ChildIDIterator
(const TDF_Label& aLabel,
 const Standard_GUID& anID,
 const Standard_Boolean allLevels)
: myID(anID),
  myItr(aLabel,allLevels)
{ ChildIDIterator_FindNext; }


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::Initialize
(const TDF_Label& aLabel,
 const Standard_GUID& anID,
 const Standard_Boolean allLevels)
{
  myID = anID;
  myItr.Initialize(aLabel,allLevels);
  myAtt.Nullify();
  ChildIDIterator_FindNext;
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::Next() 
{
  myAtt.Nullify();
  if (myItr.More()) {
    myItr.Next();
    ChildIDIterator_FindNext;
  }
}


//=======================================================================
//function : NextBrother
//purpose  : 
//=======================================================================

void TDF_ChildIDIterator::NextBrother() 
{
  myAtt.Nullify();
  if (myItr.More()) {
    myItr.NextBrother();
    while (myItr.More() && !myItr.Value().FindAttribute(myID,myAtt))
      myItr.NextBrother();
  }
}


