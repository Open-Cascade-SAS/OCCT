// Created on: 2017-02-14
// Created by: Sergey NIKONOV
// Copyright (c) 2008-2017 OPEN CASCADE SAS
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
#include <Standard_Type.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>
#include <XmlMXCAFDoc_AssemblyItemRefDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_AssemblyItemRefDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(Path, "path")
IMPLEMENT_DOMSTRING(AttrGUID, "guid")
IMPLEMENT_DOMSTRING(SubshapeIndex, "subshape_index")

//=================================================================================================

XmlMXCAFDoc_AssemblyItemRefDriver::XmlMXCAFDoc_AssemblyItemRefDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_AssemblyItemRef)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMXCAFDoc_AssemblyItemRefDriver::NewEmpty() const
{
  return new XCAFDoc_AssemblyItemRef();
}

//=================================================================================================

bool XmlMXCAFDoc_AssemblyItemRefDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                              const occ::handle<TDF_Attribute>& theTarget,
                                              XmlObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  const XmlObjMgt_Element& anElement = theSource;

  XmlObjMgt_DOMString aPath = anElement.getAttribute(::Path());
  if (aPath == NULL)
    return false;

  occ::handle<XCAFDoc_AssemblyItemRef> aThis = occ::down_cast<XCAFDoc_AssemblyItemRef>(theTarget);
  if (aThis.IsNull())
    return false;

  aThis->SetItem(aPath.GetString());

  XmlObjMgt_DOMString anAttrGUID = anElement.getAttribute(::AttrGUID());
  if (anAttrGUID != NULL)
  {
    Standard_GUID aGUID(anAttrGUID.GetString());
    aThis->SetGUID(aGUID);
    return true;
  }

  XmlObjMgt_DOMString aSubshapeIndex = anElement.getAttribute(::SubshapeIndex());
  if (aSubshapeIndex != NULL)
  {
    int anIndex;
    if (!aSubshapeIndex.GetInteger(anIndex))
      return false;

    aThis->SetSubshapeIndex(anIndex);
    return true;
  }

  return true;
}

//=================================================================================================

void XmlMXCAFDoc_AssemblyItemRefDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                              XmlObjMgt_Persistent&             theTarget,
                                              XmlObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_AssemblyItemRef> aThis = occ::down_cast<XCAFDoc_AssemblyItemRef>(theSource);

  XmlObjMgt_DOMString aPath(aThis->GetItem().ToString().ToCString());
  theTarget.Element().setAttribute(::Path(), aPath);

  if (aThis->IsGUID())
  {
    Standard_GUID aGUID = aThis->GetGUID();
    char          aGUIDStr[Standard_GUID_SIZE + 1];
    aGUID.ToCString(aGUIDStr);
    aGUIDStr[Standard_GUID_SIZE] = '\0';
    XmlObjMgt_DOMString anAttrGUID(aGUIDStr);
    theTarget.Element().setAttribute(::AttrGUID(), anAttrGUID);
  }
  else if (aThis->IsSubshapeIndex())
  {
    TCollection_AsciiString aSubshapeIndexStr(aThis->GetSubshapeIndex());
    XmlObjMgt_DOMString     aSubshapeIndex(aSubshapeIndexStr.ToCString());
    theTarget.Element().setAttribute(::SubshapeIndex(), aSubshapeIndex);
  }
}
