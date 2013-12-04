// Created on: 1999-06-17
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
