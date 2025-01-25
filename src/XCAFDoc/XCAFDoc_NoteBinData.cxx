// Copyright (c) 2017-2018 OPEN CASCADE SAS
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

#include <XCAFDoc_NoteBinData.hxx>

#include <OSD_File.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_NoteBinData, XCAFDoc_Note)

// =======================================================================
// function : GetID
// purpose  :
// =======================================================================
const Standard_GUID& XCAFDoc_NoteBinData::GetID()
{
  static Standard_GUID s_ID("E9055501-F0FC-4864-BE4B-284FDA7DDEAC");
  return s_ID;
}

// =======================================================================
// function : Get
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteBinData) XCAFDoc_NoteBinData::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NoteBinData) aThis;
  theLabel.FindAttribute(XCAFDoc_NoteBinData::GetID(), aThis);
  return aThis;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteBinData) XCAFDoc_NoteBinData::Set(const TDF_Label&                  theLabel,
                                                     const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theTitle,
                                                     const TCollection_AsciiString&    theMIMEtype,
                                                     OSD_File&                         theFile)
{
  Handle(XCAFDoc_NoteBinData) aNoteBinData;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_NoteBinData::GetID(), aNoteBinData))
  {
    aNoteBinData = new XCAFDoc_NoteBinData();
    aNoteBinData->XCAFDoc_Note::Set(theUserName, theTimeStamp);
    if (aNoteBinData->Set(theTitle, theMIMEtype, theFile))
      theLabel.AddAttribute(aNoteBinData);
    else
      aNoteBinData.Nullify();
  }
  return aNoteBinData;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteBinData) XCAFDoc_NoteBinData::Set(const TDF_Label&                  theLabel,
                                                     const TCollection_ExtendedString& theUserName,
                                                     const TCollection_ExtendedString& theTimeStamp,
                                                     const TCollection_ExtendedString& theTitle,
                                                     const TCollection_AsciiString&    theMIMEtype,
                                                     const Handle(TColStd_HArray1OfByte)& theData)
{
  Handle(XCAFDoc_NoteBinData) aNoteBinData;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_NoteBinData::GetID(), aNoteBinData))
  {
    aNoteBinData = new XCAFDoc_NoteBinData();
    aNoteBinData->XCAFDoc_Note::Set(theUserName, theTimeStamp);
    aNoteBinData->Set(theTitle, theMIMEtype, theData);
    theLabel.AddAttribute(aNoteBinData);
  }
  return aNoteBinData;
}

// =======================================================================
// function : XCAFDoc_NoteBinData
// purpose  :
// =======================================================================
XCAFDoc_NoteBinData::XCAFDoc_NoteBinData() {}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Standard_Boolean XCAFDoc_NoteBinData::Set(const TCollection_ExtendedString& theTitle,
                                          const TCollection_AsciiString&    theMIMEtype,
                                          OSD_File&                         theFile)
{
  if (!theFile.IsOpen() || !theFile.IsReadable())
    return Standard_False;

  Backup();

  if (theFile.Size() > (Standard_Size)IntegerLast())
    return Standard_False;

  myData.reset(new TColStd_HArray1OfByte(1, (Standard_Integer)theFile.Size()));
  Standard_Integer nbReadBytes = 0;
  theFile.Read((Standard_Address)&myData->First(), myData->Length(), nbReadBytes);
  if (nbReadBytes < myData->Length())
    return Standard_False;

  myTitle    = theTitle;
  myMIMEtype = theMIMEtype;

  return Standard_True;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
void XCAFDoc_NoteBinData::Set(const TCollection_ExtendedString&    theTitle,
                              const TCollection_AsciiString&       theMIMEtype,
                              const Handle(TColStd_HArray1OfByte)& theData)
{
  Backup();

  myData     = theData;
  myTitle    = theTitle;
  myMIMEtype = theMIMEtype;
}

// =======================================================================
// function : ID
// purpose  :
// =======================================================================
const Standard_GUID& XCAFDoc_NoteBinData::ID() const
{
  return GetID();
}

// =======================================================================
// function : NewEmpty
// purpose  :
// =======================================================================
Handle(TDF_Attribute) XCAFDoc_NoteBinData::NewEmpty() const
{
  return new XCAFDoc_NoteBinData();
}

// =======================================================================
// function : Restore
// purpose  :
// =======================================================================
void XCAFDoc_NoteBinData::Restore(const Handle(TDF_Attribute)& theAttr)
{
  XCAFDoc_Note::Restore(theAttr);

  Handle(XCAFDoc_NoteBinData) aMine = Handle(XCAFDoc_NoteBinData)::DownCast(theAttr);
  if (!aMine.IsNull())
  {
    myTitle    = aMine->myTitle;
    myMIMEtype = aMine->myMIMEtype;
    myData     = aMine->myData;
  }
}

// =======================================================================
// function : Paste
// purpose  :
// =======================================================================
void XCAFDoc_NoteBinData::Paste(const Handle(TDF_Attribute)&       theAttrInto,
                                const Handle(TDF_RelocationTable)& theRT) const
{
  XCAFDoc_Note::Paste(theAttrInto, theRT);

  Handle(XCAFDoc_NoteBinData) aMine = Handle(XCAFDoc_NoteBinData)::DownCast(theAttrInto);
  if (!aMine.IsNull())
    aMine->Set(myTitle, myMIMEtype, myData);
}

// =======================================================================
// function : Dump
// purpose  :
// =======================================================================
Standard_OStream& XCAFDoc_NoteBinData::Dump(Standard_OStream& theOS) const
{
  XCAFDoc_Note::Dump(theOS);
  theOS << "\n"
        << "Title : " << (!myTitle.IsEmpty() ? myMIMEtype : "<untitled>") << "\n"
        << "MIME type : " << (!myMIMEtype.IsEmpty() ? myMIMEtype : "<none>") << "\n"
        << "Size : " << Size() << " bytes" << "\n";
  if (!myData.IsNull())
  {
    for (Standard_Integer i = myData->Lower(); i <= myData->Upper(); ++i)
      theOS << myData->Value(i);
  }
  return theOS;
}
