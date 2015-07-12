// Created on: 2000-01-26
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_TreeNode.hxx>

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
