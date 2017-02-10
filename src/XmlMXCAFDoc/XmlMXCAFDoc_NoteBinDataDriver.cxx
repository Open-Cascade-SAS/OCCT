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

#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_NoteBinData.hxx>
#include <XmlObjMgt.hxx>
#include <XmlMXCAFDoc_NoteBinDataDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <LDOM_OSStream.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_NoteBinDataDriver, XmlMXCAFDoc_NoteDriver)
IMPLEMENT_DOMSTRING(Title, "title")
IMPLEMENT_DOMSTRING(MIMEtype, "mime_type")
IMPLEMENT_DOMSTRING(Size, "size")

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_NoteBinDataDriver::XmlMXCAFDoc_NoteBinDataDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : XmlMXCAFDoc_NoteDriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NoteBinData)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_NoteBinDataDriver::NewEmpty() const
{
  return new XCAFDoc_NoteBinData();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_NoteBinDataDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                      const Handle(TDF_Attribute)& theTarget,
                                                      XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  XmlMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable);

  const XmlObjMgt_Element& anElement = theSource;

  XmlObjMgt_DOMString aTitle = anElement.getAttribute(::Title());
  XmlObjMgt_DOMString aMIMEtype = anElement.getAttribute(::MIMEtype());
  XmlObjMgt_DOMString aSize = anElement.getAttribute(::Size());
  if (aTitle == NULL || aMIMEtype == NULL || aSize == NULL)
    return Standard_False;

  Handle(XCAFDoc_NoteBinData) aNote = Handle(XCAFDoc_NoteBinData)::DownCast(theTarget);
  if (aNote.IsNull())
    return Standard_False;

  Standard_Integer nbSize = 0;
  if (!aSize.GetInteger(nbSize))
    return Standard_False;

  XmlObjMgt_DOMString aDataStr = XmlObjMgt::GetStringValue(theSource);
  Standard_SStream anSS(aDataStr.GetString());

  Handle(TColStd_HArray1OfByte) aData = new TColStd_HArray1OfByte(1, nbSize);
  for (Standard_Integer i = 1; i <= nbSize; ++i)
  {
    Standard_Byte aValue;
    anSS >> aValue;
    aData->ChangeValue(i) = aValue;
  }

  aNote->Set(aTitle.GetString(), aMIMEtype.GetString(), aData);

  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_NoteBinDataDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                          XmlObjMgt_Persistent&        theTarget,
                                          XmlObjMgt_SRelocationTable&  theRelocTable) const
{
  XmlMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable);

  Handle(XCAFDoc_NoteBinData) aNote = Handle(XCAFDoc_NoteBinData)::DownCast(theSource);

  XmlObjMgt_DOMString aTitle(TCollection_AsciiString(aNote->Title()).ToCString());
  XmlObjMgt_DOMString aMIMEtype(aNote->MIMEtype().ToCString());

  theTarget.Element().setAttribute(::Title(), aTitle);
  theTarget.Element().setAttribute(::MIMEtype(), aMIMEtype);
  theTarget.Element().setAttribute(::Size(), aNote->Size());

  if (aNote->Size() > 0)
  {
    const Handle(TColStd_HArray1OfByte)& aData = aNote->Data();
    LDOM_OSStream anOSS(aNote->Size());
    for (Standard_Integer i = aData->Lower(); i <= aData->Upper(); ++i)
    {
      anOSS << std::hex << aData->Value(i);
    }
    Standard_Character* dump = (Standard_Character*)anOSS.str(); // copying! Don't forget to delete it.
    XmlObjMgt::SetStringValue(theTarget, dump, Standard_True);
    delete[] dump;
  }
}
