// File:	TDataStd_ChildNodeIterator.cxx
// Created:	Wed Jan 26 16:43:08 2000
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


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
