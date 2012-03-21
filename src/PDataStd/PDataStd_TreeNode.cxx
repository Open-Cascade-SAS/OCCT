// Created on: 1999-06-17
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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
