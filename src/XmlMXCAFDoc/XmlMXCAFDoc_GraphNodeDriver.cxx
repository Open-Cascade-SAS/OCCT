// Created on: 2001-09-04
// Created by: Julia DOROVSKIKH
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


#include <XmlMXCAFDoc_GraphNodeDriver.ixx>

#include <XmlObjMgt.hxx>

#include <XCAFDoc_GraphNode.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <Standard_PCharacter.hxx>

IMPLEMENT_DOMSTRING (TreeIdString,   "treeid")
IMPLEMENT_DOMSTRING (ChildrenString, "children")
IMPLEMENT_DOMSTRING (FathersString,  "fathers")

//=======================================================================
//function : XmlMXCAFDoc_GraphNodeDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_GraphNodeDriver::XmlMXCAFDoc_GraphNodeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "GraphNode")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_GraphNodeDriver::NewEmpty() const
{
  return (new XCAFDoc_GraphNode());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_GraphNodeDriver::Paste
                (const XmlObjMgt_Persistent&  theSource,
                 const Handle(TDF_Attribute)& theTarget,
                 XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(XCAFDoc_GraphNode) aT = Handle(XCAFDoc_GraphNode)::DownCast(theTarget);
  const XmlObjMgt_Element& anElement = theSource;

  // tree id
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::TreeIdString());
  Standard_GUID aGUID (Standard_CString(aGUIDStr.GetString()));
  aT->SetGraphID(aGUID);

  XmlObjMgt_DOMString aDOMStr;
  Handle(XCAFDoc_GraphNode) aTChild;

  // fathers
  aDOMStr = anElement.getAttribute(::FathersString());
  if (aDOMStr != NULL)                  // void list is allowed
  {
    Standard_CString aChildren = Standard_CString(aDOMStr.GetString());
    Standard_Integer aNb = 0;
    if (!XmlObjMgt::GetInteger(aChildren, aNb)) return Standard_False;

    while (aNb > 0)
    {
      // Find or create GraphNode attribute with the given ID
      if (theRelocTable.IsBound(aNb))
      {
        aTChild = Handle(XCAFDoc_GraphNode)::DownCast(theRelocTable.Find(aNb));
        if (aTChild.IsNull())
          return Standard_False;
      }
      else
      {
        aTChild = new XCAFDoc_GraphNode;
        theRelocTable.Bind(aNb, aTChild);
      }

      // Add the child to the current tree
      aTChild->SetGraphID(aGUID);
      aT->SetFather(aTChild);

      // Get next child ID
      if (!XmlObjMgt::GetInteger(aChildren, aNb)) aNb = 0;
    }
  }

  // children
  aDOMStr = anElement.getAttribute(::ChildrenString());
  if (aDOMStr != NULL)                  // void list is allowed
  {
    Standard_CString aChildren = Standard_CString(aDOMStr.GetString());
    Standard_Integer aNb = 0;
    if (!XmlObjMgt::GetInteger(aChildren, aNb)) return Standard_False;

    while (aNb > 0)
    {
      // Find or create GraphNode attribute with the given ID
      if (theRelocTable.IsBound(aNb))
      {
        aTChild = Handle(XCAFDoc_GraphNode)::DownCast(theRelocTable.Find(aNb));
        if (aTChild.IsNull())
          return Standard_False;
      }
      else
      {
        aTChild = new XCAFDoc_GraphNode;
        theRelocTable.Bind(aNb, aTChild);
      }

      // Add the child to the current tree
      aTChild->SetGraphID(aGUID);
      aT->SetChild(aTChild);

      // Get next child ID
      if (!XmlObjMgt::GetInteger(aChildren, aNb)) aNb = 0;
    }
  }

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_GraphNodeDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(XCAFDoc_GraphNode) aS = Handle(XCAFDoc_GraphNode)::DownCast(theSource);
  if (aS.IsNull()) return;
  
  // graph id
  Standard_Character aGuidStr [40];
  Standard_PCharacter pGuidStr;
  //
  pGuidStr=(Standard_PCharacter)aGuidStr;
  aS->ID().ToCString (pGuidStr);
  theTarget.Element().setAttribute(::TreeIdString(), aGuidStr);

  Standard_Integer aNb;
  TCollection_AsciiString aStr;
  Handle(XCAFDoc_GraphNode) aF;
  Standard_Integer i;

  // fathers
  for (i = 1; i <= aS->NbFathers(); i++)
  {
    aF = aS->GetFather(i);
    if (!aF.IsNull())
    {
      aNb = theRelocTable.FindIndex(aF);
      if (aNb == 0)
      {
        aNb = theRelocTable.Add(aF);
      }
      TCollection_AsciiString aNbStr (aNb);
      aStr += aNbStr + " ";
    }
  }
  if (aStr.Length() > 0)
    theTarget.Element().setAttribute(::FathersString(), aStr.ToCString());
  
  // children
  aStr.Clear();
  for (i = 1; i <= aS->NbChildren(); i++)
  {
    aF = aS->GetChild(i);
    if (!aF.IsNull())
    {
      aNb = theRelocTable.FindIndex(aF);
      if (aNb == 0)
      {
        aNb = theRelocTable.Add(aF);
      }
      TCollection_AsciiString aNbStr (aNb);
      aStr += aNbStr + " ";
    }
  }
  if (aStr.Length() > 0)
    theTarget.Element().setAttribute(::ChildrenString(), aStr.ToCString());
}
