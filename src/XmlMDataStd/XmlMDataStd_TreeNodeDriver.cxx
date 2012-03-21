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


#include <XmlMDataStd_TreeNodeDriver.ixx>
#include <TDataStd_TreeNode.hxx>
#include <XmlObjMgt.hxx>

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
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::TreeIdString());
  Standard_GUID aGUID (Standard_CString(aGUIDStr.GetString()));
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

  Standard_Integer aNb;

  TCollection_AsciiString aChildrenStr;

  // tree id
  Standard_Character aGuidStr [40];
  Standard_PCharacter pGuidStr=aGuidStr;
  aS->ID().ToCString (pGuidStr);
  theTarget.Element().setAttribute(::TreeIdString(), aGuidStr);

  // first child
  Handle(TDataStd_TreeNode) aF = aS->First();

  // form the string of numbers for the list of children
  while (!aF.IsNull())
  {
    aNb = theRelocTable.FindIndex(aF);
    if (aNb == 0)
    {
      aNb = theRelocTable.Add(aF);
    }
    TCollection_AsciiString aNbStr (aNb);
    aChildrenStr += aNbStr + " ";

    // next child
    aF = aF->Next();
  }

  if (aChildrenStr.Length() > 0)
    theTarget.Element().setAttribute(::ChildrenString(),
                                      aChildrenStr.ToCString());
}
