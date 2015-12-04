// Created on: 2001-09-04
// Created by: Julia DOROVSKIKH
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
#include <Standard_PCharacter.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XmlMXCAFDoc_GraphNodeDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_GraphNodeDriver,XmlMDF_ADriver)
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
