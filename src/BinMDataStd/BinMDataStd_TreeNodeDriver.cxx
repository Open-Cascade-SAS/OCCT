// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <BinMDataStd_TreeNodeDriver.ixx>
#include <TDataStd_TreeNode.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : BinMDataStd_TreeNodeDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_TreeNodeDriver::BinMDataStd_TreeNodeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_TreeNodeDriver::NewEmpty() const
{
  return (new TDataStd_TreeNode());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMDataStd_TreeNodeDriver::Paste
                               (const BinObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                BinObjMgt_RRelocationTable& theRelocTable) const
{
  Handle(TDataStd_TreeNode) aT = Handle(TDataStd_TreeNode)::DownCast(theTarget);

  // read int fields
  Standard_Integer i, aNb;
  for (i = 0 ; i < 4; ++i)
  {
    if (! (theSource >> aNb))
      return Standard_False;
    if (aNb < 0) continue;

    Handle(TDataStd_TreeNode) aNode;
    if (theRelocTable.IsBound(aNb))
      aNode = Handle(TDataStd_TreeNode)::DownCast(theRelocTable.Find(aNb));
    else
    {
      aNode = Handle(TDataStd_TreeNode)::DownCast( aT->NewEmpty() ); // already with tree ID
      theRelocTable.Bind(aNb, aNode);
    }
    switch (i) {
    case 0: aT->SetFather  (aNode); break;
    case 1: aT->SetNext    (aNode); break;  
    case 2: aT->SetPrevious(aNode); break;
    case 3: aT->SetFirst   (aNode); break;
    default: continue;
    }
  }

  // tree id
  Standard_GUID aGUID;
  if (! (theSource >> aGUID))
    return Standard_False;
  aT->SetTreeID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMDataStd_TreeNodeDriver::Paste
                               (const Handle(TDF_Attribute)& theSource,
                                BinObjMgt_Persistent&       theTarget,
                                BinObjMgt_SRelocationTable& theRelocTable) const
{
  Handle(TDataStd_TreeNode) aS = Handle(TDataStd_TreeNode)::DownCast(theSource);

  // first write int fields
  Standard_Integer i, aNb;
  for (i = 0 ; i < 4; ++i)
  {
    Handle(TDataStd_TreeNode) aNode;
    switch (i) {
    case 0: aNode = aS->Father();   break;
    case 1: aNode = aS->Next();     break;  
    case 2: aNode = aS->Previous(); break;
    case 3: aNode = aS->First();    break;
    default: continue;
    }
    if (aNode.IsNull())
      aNb = -1;
    else
      aNb = theRelocTable.Add(aNode);    // create and/or get index
    theTarget.PutInteger(aNb);
  }

  // tree id
  theTarget << aS->ID();
}
