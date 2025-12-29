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

#include <Message_Messenger.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <XmlMDataStd_TreeNodeDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_TreeNodeDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(TreeIdString, "treeid")
IMPLEMENT_DOMSTRING(ChildrenString, "children")

//=================================================================================================

XmlMDataStd_TreeNodeDriver::XmlMDataStd_TreeNodeDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_TreeNodeDriver::NewEmpty() const
{
  return (new TDataStd_TreeNode());
}

//=================================================================================================

bool XmlMDataStd_TreeNodeDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       XmlObjMgt_RRelocationTable&       theRelocTable) const
{
  occ::handle<TDataStd_TreeNode> aT        = occ::down_cast<TDataStd_TreeNode>(theTarget);
  const XmlObjMgt_Element&       anElement = theSource;

  // tree id
  Standard_GUID       aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::TreeIdString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_TreeNode::GetDefaultTreeID();
  else
    aGUID = Standard_GUID(static_cast<const char*>(aGUIDStr.GetString()));
  aT->SetTreeID(aGUID);

  // children
  occ::handle<TDataStd_TreeNode> aTChild;

  XmlObjMgt_DOMString aChildrenStr = anElement.getAttribute(::ChildrenString());
  if (aChildrenStr != nullptr) // void list is allowed
  {
    const char* aChildren = static_cast<const char*>(aChildrenStr.GetString());
    int         aNb       = 0;
    if (!XmlObjMgt::GetInteger(aChildren, aNb))
      return false;

    while (aNb > 0)
    {
      // Find or create TreeNode attribute with the given ID
      if (theRelocTable.IsBound(aNb))
      {
        aTChild = occ::down_cast<TDataStd_TreeNode>(theRelocTable.Find(aNb));
        if (aTChild.IsNull())
          return false;
      }
      else
      {
        aTChild = new TDataStd_TreeNode;
        theRelocTable.Bind(aNb, aTChild);
      }

      // Add the child to the current tree
      aTChild->SetTreeID(aGUID);
      aT->Append(aTChild);

      // Get next child ID
      if (!XmlObjMgt::GetInteger(aChildren, aNb))
        aNb = 0;
    }
  }
  return true;
}

//=================================================================================================

void XmlMDataStd_TreeNodeDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       XmlObjMgt_Persistent&             theTarget,
                                       XmlObjMgt_SRelocationTable&       theRelocTable) const
{
  occ::handle<TDataStd_TreeNode> aS = occ::down_cast<TDataStd_TreeNode>(theSource);

  // tree id
  // A not default ID is skipped for storage version 8 and newer.
  if (aS->ID() != TDataStd_TreeNode::GetDefaultTreeID()
      || theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
           < TDocStd_FormatVersion_VERSION_8)
  {
    char                aGuidStr[40];
    Standard_PCharacter pGuidStr = aGuidStr;
    aS->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::TreeIdString(), aGuidStr);
  }

  // Find number of children.
  int nbChildren = aS->NbChildren();

  // Allocate 11 digits for each ID (an integer) of the child + a space.
  int                          iChar = 0;
  NCollection_LocalArray<char> str;
  if (nbChildren)
    str.Allocate(11 * nbChildren + 1);

  // form the string of numbers for the list of children
  occ::handle<TDataStd_TreeNode> aF = aS->First();
  while (!aF.IsNull())
  {
    int aNb = theRelocTable.FindIndex(aF);
    if (aNb == 0)
    {
      aNb = theRelocTable.Add(aF);
    }

    // Add number to the long string.
    iChar += Sprintf(&(str[iChar]), "%d ", aNb);

    // next child
    aF = aF->Next();
  }

  if (nbChildren)
  {
    theTarget.Element().setAttribute(::ChildrenString(), (char*)str);
  }
}
