// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <BinMDataStd_TreeNodeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_TreeNodeDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_TreeNodeDriver::BinMDataStd_TreeNodeDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_TreeNodeDriver::NewEmpty() const
{
  return (new TDataStd_TreeNode());
}

//=================================================================================================

bool BinMDataStd_TreeNodeDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                   const occ::handle<TDF_Attribute>& theTarget,
                                                   BinObjMgt_RRelocationTable&  theRelocTable) const
{
  occ::handle<TDataStd_TreeNode> aT = occ::down_cast<TDataStd_TreeNode>(theTarget);

  // read int fields
  int i, aNb;
  for (i = 0; i < 4; ++i)
  {
    if (!(theSource >> aNb))
      return false;
    if (aNb < 0)
      continue;

    occ::handle<TDataStd_TreeNode> aNode;
    if (theRelocTable.IsBound(aNb))
      aNode = occ::down_cast<TDataStd_TreeNode>(theRelocTable.Find(aNb));
    else
    {
      aNode = occ::down_cast<TDataStd_TreeNode>(aT->NewEmpty()); // already with tree ID
      theRelocTable.Bind(aNb, aNode);
    }
    switch (i)
    {
      case 0:
        aT->SetFather(aNode);
        break;
      case 1:
        aT->SetNext(aNode);
        break;
      case 2:
        aT->SetPrevious(aNode);
        break;
      case 3:
        aT->SetFirst(aNode);
        break;
      default:
        continue;
    }
  }

  // tree id
  Standard_GUID aGUID;
  if (!(theSource >> aGUID))
    return false;
  aT->SetTreeID(aGUID);

  return true;
}

//=================================================================================================

void BinMDataStd_TreeNodeDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       NCollection_IndexedMap<occ::handle<Standard_Transient>>&  theRelocTable) const
{
  occ::handle<TDataStd_TreeNode> aS = occ::down_cast<TDataStd_TreeNode>(theSource);

  // first write int fields
  int i, aNb;
  for (i = 0; i < 4; ++i)
  {
    occ::handle<TDataStd_TreeNode> aNode;
    switch (i)
    {
      case 0:
        aNode = aS->Father();
        break;
      case 1:
        aNode = aS->Next();
        break;
      case 2:
        aNode = aS->Previous();
        break;
      case 3:
        aNode = aS->First();
        break;
      default:
        continue;
    }
    if (aNode.IsNull())
      aNb = -1;
    else
      aNb = theRelocTable.Add(aNode); // create and/or get index
    theTarget.PutInteger(aNb);
  }

  // tree id
  theTarget << aS->ID();
}
