// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdLPersistent_TreeNode.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_TreeNode::Read (StdObjMgt_ReadData& theReadData)
{
  myDynamicData = new dynamic;
  theReadData >> myDynamicData->First >> myNext >> myDynamicData->TreeID;
}

//=======================================================================
//function : Write
//purpose  : Write persistent data to a file
//=======================================================================
void StdLPersistent_TreeNode::Write (StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myDynamicData->First << myNext << myDynamicData->TreeID;
}

//=======================================================================
//function : PChildren
//purpose  : Gets persistent child objects
//=======================================================================
void StdLPersistent_TreeNode::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myNext);
  if (!myDynamicData.IsNull())
    theChildren.Append(myDynamicData->First);
}

//=======================================================================
//function : CreateAttribute
//purpose  : Create an empty transient attribuite
//=======================================================================
Handle(TDF_Attribute) StdLPersistent_TreeNode::CreateAttribute()
{
  Static::CreateAttribute();
  myTransient->SetTreeID (myDynamicData->TreeID);
  return myTransient;
}

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
void StdLPersistent_TreeNode::ImportAttribute()
{
  if (myDynamicData)
  {
    Handle(StdLPersistent_TreeNode) aChild = myDynamicData->First;
    while (aChild)
    {
      if (aChild->myTransient)
        myTransient->Append(aChild->myTransient);
      StdLPersistent_TreeNode* aCurr = aChild.get();
      aChild = aChild->myNext;
      aCurr->myNext.Nullify(); // this reference is no longer needed
    }

    myDynamicData.Nullify();
  }
}
