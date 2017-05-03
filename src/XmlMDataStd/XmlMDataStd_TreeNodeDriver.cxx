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


#include <CDM_MessageDriver.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_TreeNodeDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlLDrivers.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_TreeNodeDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (TreeIdString,   "treeid")
IMPLEMENT_DOMSTRING (ChildrenString, "children")

//=======================================================================
//function : XmlMDataStd_TreeNodeDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_TreeNodeDriver::XmlMDataStd_TreeNodeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_TreeNodeDriver::NewEmpty() const
{
  return (new TDataStd_TreeNode());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_TreeNodeDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable& theRelocTable) const
{
  Handle(TDataStd_TreeNode) aT = Handle(TDataStd_TreeNode)::DownCast(theTarget);
  const XmlObjMgt_Element& anElement = theSource;

  // tree id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::TreeIdString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_TreeNode::GetDefaultTreeID();
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString()));
  aT->SetTreeID(aGUID);

  // children
  Handle(TDataStd_TreeNode) aTChild;

  XmlObjMgt_DOMString aChildrenStr = anElement.getAttribute(::ChildrenString());
  if (aChildrenStr != NULL)                     // void list is allowed
  {
    Standard_CString aChildren = Standard_CString(aChildrenStr.GetString());
    Standard_Integer aNb = 0;
    if (!XmlObjMgt::GetInteger(aChildren, aNb)) return Standard_False;

    while (aNb > 0)
    {
      // Find or create TreeNode attribute with the given ID
      if (theRelocTable.IsBound(aNb))
      {
        aTChild = Handle(TDataStd_TreeNode)::DownCast(theRelocTable.Find(aNb));
        if (aTChild.IsNull())
          return Standard_False;
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
      if (!XmlObjMgt::GetInteger(aChildren, aNb)) aNb = 0;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_TreeNodeDriver::Paste
                               (const Handle(TDF_Attribute)& theSource,
                                XmlObjMgt_Persistent&       theTarget,
                                XmlObjMgt_SRelocationTable& theRelocTable) const
{
  Handle(TDataStd_TreeNode) aS = Handle(TDataStd_TreeNode)::DownCast(theSource);

  // tree id
  // A not default ID is skipped for storage version 8 and newer.
  if (aS->ID() != TDataStd_TreeNode::GetDefaultTreeID() ||
      XmlLDrivers::StorageVersion() < 8)
  {
    Standard_Character aGuidStr [40];
    Standard_PCharacter pGuidStr=aGuidStr;
    aS->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute(::TreeIdString(), aGuidStr);
  }

  // Find number of children.
  int nbChildren = aS->NbChildren();
  
  // Allocate 11 digits for each ID (an integer) of the child + a space.
  Standard_Integer iChar = 0;
  NCollection_LocalArray<Standard_Character> str;
  if (nbChildren)
    str.Allocate(11 * nbChildren + 1);

  // form the string of numbers for the list of children
  Handle(TDataStd_TreeNode) aF = aS->First();
  while (!aF.IsNull())
  {
    Standard_Integer aNb = theRelocTable.FindIndex(aF);
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
    theTarget.Element().setAttribute(::ChildrenString(), (Standard_Character*)str);
  }
}
