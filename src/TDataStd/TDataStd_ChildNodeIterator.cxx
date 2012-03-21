// Created on: 2000-01-26
// Created by: Denis PASCAL
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <TDataStd_ChildNodeIterator.ixx>

#define ChildNodeIterator_UpToBrother \
{ \
    while (!myNode.IsNull() && (myNode->Depth() > myFirstLevel) && myNode->myNext == NULL) \
      myNode = myNode->myFather; \
	if (!myNode.IsNull() && (myNode->Depth() > myFirstLevel) && myNode->myFather != NULL) \
	  myNode = myNode->myNext; \
	else \
	  myNode = NULL; \
}

//=======================================================================
//function : TDataStd_ChildNodeIterator
//purpose  : 
//=======================================================================

TDataStd_ChildNodeIterator::TDataStd_ChildNodeIterator()
     : myFirstLevel(0)
{}

//=======================================================================
//function : TDataStd_ChildNodeIterator
//purpose  : 
//=======================================================================

TDataStd_ChildNodeIterator::TDataStd_ChildNodeIterator (const Handle(TDataStd_TreeNode)& aTreeNode,
							const Standard_Boolean allLevels)
: myNode(aTreeNode->myFirst),
  myFirstLevel(allLevels ? aTreeNode->Depth() : -1)
{}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void TDataStd_ChildNodeIterator::Initialize(const Handle(TDataStd_TreeNode)& aTreeNode,
					    const Standard_Boolean allLevels)
{
  myNode = aTreeNode->myFirst;
  myFirstLevel = allLevels ? aTreeNode->Depth() : -1;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TDataStd_ChildNodeIterator::Next() 
{
  if (myFirstLevel == -1) {
    myNode = myNode->myNext;
  }
  else {
    if (myNode->myFirst != NULL) myNode = myNode->myFirst;
    else ChildNodeIterator_UpToBrother;
  }
}

//=======================================================================
//function : NextBrother
//purpose  : 
//=======================================================================

void TDataStd_ChildNodeIterator::NextBrother() 
{
  if (myNode->myNext != NULL) myNode = myNode->myNext;
  else ChildNodeIterator_UpToBrother;
}
