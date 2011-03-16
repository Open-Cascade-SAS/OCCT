// File:	PDataStd_TreeNode.cxx
// Created:	Thu Jun 17 11:03:48 1999
// Author:	Vladislav ROMASHKO
//		<vro@flox.nnov.matra-dtv.fr>


#include <PDataStd_TreeNode.ixx>
#include <Standard_GUID.hxx>

PDataStd_TreeNode::PDataStd_TreeNode()
{}

Handle(PDataStd_TreeNode) PDataStd_TreeNode::First() const
{
  return myFirst;
}

void PDataStd_TreeNode::SetFirst(const Handle(PDataStd_TreeNode)& F)
{
  myFirst = F;
}

Handle(PDataStd_TreeNode) PDataStd_TreeNode::Next() const
{
  return myNext;
}

void PDataStd_TreeNode::SetNext(const Handle(PDataStd_TreeNode)& F )
{
  myNext = F;
}

void PDataStd_TreeNode::SetTreeID(const Standard_GUID& ID)
{
  myTreeID = ID;
}

Standard_GUID PDataStd_TreeNode::GetTreeID() const
{
  return myTreeID;
}
